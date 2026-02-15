#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_EVENTS 64
#define MESSAGE_SIZE 16
#define DEFAULT_CLIENT_THREADS 4

char *server_ip = "127.0.0.1";
int server_port = 12345;
int num_client_threads = DEFAULT_CLIENT_THREADS;
int num_requests = 1000000;

static void DieWithError(char *errorMessage) 
{ 
    perror(errorMessage); 
    exit(1); 
}  

void HandleTCPClient(int clntSocket); /* TCP client handling function */ 

int SetNonBlocking(int fd); /* Function for setting non-blocking flags for file descriptors */

/*
 * This structure is used to store per-thread data in the client
 */
typedef struct {
    int epoll_fd;        /* File descriptor for the epoll instance, used for monitoring events on the socket. */
    int client_fd;       /* File descriptor for the client socket connected to the server. */
    long long total_rtt; /* Accumulated Round-Trip Time (RTT) for all messages sent and received (in microseconds). */
    long total_messages; /* Total number of messages sent and received. */
    float request_rate;  /* Computed request rate (requests per second) based on RTT and total messages. */
} client_thread_data_t;

/* This function runs in a separate client thread to handle communication with the server */ 
void *client_thread_func(void *arg) 
{
    client_thread_data_t *data = (client_thread_data_t *)arg;
    struct epoll_event ev, events[MAX_EVENTS];
    char send_buf[MESSAGE_SIZE] = "ABCDEFGHIJKMLNOP"; /* Send 16-Bytes message every time */
    char recv_buf[MESSAGE_SIZE];
    struct timeval start, end;

    /* Initialize the ev struct for the client*/
    ev.events = EPOLLIN;
    ev.data.fd = data->client_fd;

    /* Initialize the round trip time metrics*/
    long long RTT = 0;
    int total_messages = 0;

    /* Register the connected socket from the interest list using epoll_ctl() */
    if (epoll_ctl(data->epoll_fd, EPOLL_CTL_ADD, data->client_fd, &ev) == -1) 
    {
        DieWithError("failed to register client's connection socket to the interest list");
    }

    /* Client thread sends messages to the server, waits for a response using epoll, 
       and measures the round-trip time (RTT) of this request-response.
    */
    for (int i = 0; i < num_requests; i++) 
    {
        /* Use gettimeofday() to "start the timer" for the client's sent messsage */
        gettimeofday(&start, NULL);

        if (send(data->client_fd, send_buf, MESSAGE_SIZE, 0) != MESSAGE_SIZE)
        {
            DieWithError("send() sent a different number of bytes than expected");
        }
            
        /* Use epoll_wait() to wait for a response from the server */
        if (epoll_wait(data->epoll_fd, events, MAX_EVENTS, -1) < 0)
        {
            DieWithError("epoll_wait() failed");
        }

        /* Wait (block) until the message is recieved from the server before sending another message */
        if ((recv(data->client_fd, recv_buf, MESSAGE_SIZE, 0)) < 0)
        {
            DieWithError("recv() failed or connection closed prematurely");
        } 
        
        /* Use gettimeofday() to "stop the timer" and record the RTT time */
        gettimeofday(&end, NULL);

        /* Calculate the total number of microsends that have elapsed */
        RTT = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
        data->total_rtt += RTT;
        data->total_messages += 1; 
    }

    return NULL;
}

/*
 * This function orchestrates multiple client threads to send requests to a server,
 * collect performance data of each threads, and compute aggregated metrics of all threads.
*/
void run_client() 
{
    pthread_t threads[num_client_threads];
    client_thread_data_t thread_data[num_client_threads];
    struct sockaddr_in server_addr;

    /* Populate the server_addr struct so the client socket can connect to the server */
    memset(thread_data, 0, sizeof(thread_data)); /* Zero out garbage data in the thread_data struct */
    server_addr.sin_family = AF_INET; /* Internet address family */ 
    server_addr.sin_addr.s_addr = inet_addr(server_ip); /* Server IP address */
    server_addr.sin_port = htons(server_port); /* Server port */ 

    /* Initialize variables for the average RTT and request rate */
    double total_rtt = 0;
    int total_messages = 0;
    double total_request_rate = 0;

    /* Create sockets and epoll instances for client threads
    and connect these sockets of client threads to the server */    
    for (int i = 0; i < num_client_threads; i++)
    {
        /* Create a reliable, stream socket using TCP */ 
        if ((thread_data[i].client_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            DieWithError("socket() failed");
        } 
             
        /* Establish the connection to the echo server */ 
        if (connect(thread_data[i].client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        {
            DieWithError("connect() failed"); 
        } 

        /* Create the interest list (aka the epoll instance) for the client thread */
        thread_data[i].epoll_fd = epoll_create(1);
        if (thread_data[i].epoll_fd == -1) 
        {
            DieWithError("failed to create the epoll instance for client");
        }

        /* Pass the thread_data to pthread_create() */   
        pthread_create(&threads[i], NULL, client_thread_func, &thread_data[i]);
    }

    for (int i = 0; i < num_client_threads; i++) 
    {
        /* Wait for client threads to complete and aggregate metrics of all client threads */
        pthread_join(threads[i], NULL);
        total_rtt += thread_data[i].total_rtt;
        total_messages += thread_data[i].total_messages;
    }
    
    total_request_rate = ((double)total_messages * 1000000.0) / total_rtt;
    
    printf("Average RTT: %.2f µs\n", total_rtt / (double)total_messages);
    printf("Total Request Rate: %.2f messages/s\n", total_request_rate);
}

void run_server() 
{
    struct sockaddr_in ServAddr; /* Local address of server */ 
    struct sockaddr_in ClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */     
    unsigned int clntLen; /* Length of client address data structure */

    /* Create socket for incoming connections */ 
    int listenSock; /* Socket descriptor for server */ 
    int connSock; /* Socket descriptor for client */

    /* Initialize the event struct for the server*/
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;

    if ((listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        DieWithError("socket() failed");
    }

    /* Set the file descriptor for the server to be the listening socket */
    ev.data.fd = listenSock;

    /* Construct local address structure */ 
    memset(&ServAddr, 0, sizeof(ServAddr)); /* Zero out structure */
    ServAddr.sin_family = AF_INET; /* Internet address family */ 
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    ServAddr.sin_port = htons(server_port); /* Server port */     

    /* Bind to the local address */ 
    if (bind(listenSock, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) < 0)
    {
        DieWithError ("bind() failed");
    } 
         
    /* Mark the socket so it will listen for incoming connections */ 
    if (listen(listenSock, MAX_EVENTS) < 0)
    {
        DieWithError("listen() failed");
    } 

    /* Set listening socket to non-blocking to prevent a race condition between the client’s 
    connection state and the server’s call to accept() */
    SetNonBlocking(listenSock);
         
    /* Create the epoll instance for the server */
    int server_fd = epoll_create(1);
    if (server_fd == -1) 
    {
        DieWithError("failed to the created the epoll instance for server");
    }   

    /* Register the listening socket to epoll */
    if (epoll_ctl(server_fd, EPOLL_CTL_ADD, listenSock, &ev) == -1) 
    {
        DieWithError("failed to register server's listening socket to the interest list");
    }
        
    /* Server's run-to-completion event loop */
    while (1) 
    {
        clntLen = sizeof(ClntAddr); /* Set the size of the in-out parameter */ 
        int nfds = epoll_wait(server_fd, events, MAX_EVENTS, -1); /* number of ready fds in our epoll interest list*/

        if (nfds < 0)
        {
            DieWithError("epoll_wait() error");
        }

        for (int n = 0; n < nfds; ++n) 
        {
            /* Check if the incoming message is a connection request from a new client */
            if (events[n].data.fd == listenSock) 
            {
                /* Wait for a client to connect */     
                if ((connSock = accept(listenSock, (struct sockaddr *) &ClntAddr, &clntLen)) < 0)
                {
                    DieWithError("accept() failed"); 
                }

                /* Set connection socket to non-blocking to allow the server to handle multiple 
                concurrent threads without waiting for any one client to finish communicating.
                */
                SetNonBlocking(connSock);
                ev.events = EPOLLIN;
                ev.data.fd = connSock;

                /* Register the connection socket to epoll */
                if (epoll_ctl(server_fd, EPOLL_CTL_ADD, connSock, &ev) == -1) 
                {
                    DieWithError("failed to register server's connection socket to the interest list");
                }
            } 
            else /* Handle existing connections to clients */
            {
                HandleTCPClient(events[n].data.fd);
            }               
        }       
    }
}

int main(int argc, char *argv[]) 
{
    if (argc > 1 && strcmp(argv[1], "server") == 0) 
    {
        if (argc > 2) server_ip = argv[2];
        if (argc > 3) server_port = atoi(argv[3]);

        run_server();
    } 
    else if (argc > 1 && strcmp(argv[1], "client") == 0) 
    {
        if (argc > 2) server_ip = argv[2];
        if (argc > 3) server_port = atoi(argv[3]);
        if (argc > 4) num_client_threads = atoi(argv[4]);
        if (argc > 5) num_requests = atoi(argv[5]);

        run_client();
    } 
    else 
    {
        printf("Usage: %s <server|client> [server_ip server_port num_client_threads num_requests]\n", argv[0]);
    }

    return 0;
}