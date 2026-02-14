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

/*
 * This function runs in a separate client thread to handle communication with the server
 */
void *client_thread_func(void *arg) {
    client_thread_data_t *data = (client_thread_data_t *)arg;
    struct epoll_event ev, events[MAX_EVENTS];
    char send_buf[MESSAGE_SIZE] = "ABCDEFGHIJKMLNOP"; /* Send 16-Bytes message every time */
    char recv_buf[MESSAGE_SIZE];
    struct timeval start, end;

    // Hint 1: register the "connected" client_thread's socket in the its epoll instance

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

    for (int i = 0; i < num_requests; i++) {

        /* TODO:
        * It sends messages to the server, waits for a response using epoll,
        * and measures the round-trip time (RTT) of this request-response.
        */

        char* echoString = send_buf;
        unsigned int echoStringLen = strlen(echoString); /* Determine input length */

        // Hint 2: use gettimeofday() and "struct timeval start, end" to record timestamp, which can be used to calculated RTT.
        gettimeofday(&start, NULL);

        if (send(data->client_fd, echoString, echoStringLen, 0) != echoStringLen)
        {
            DieWithError("send() sent a different number of bytes than expected");
        }
            
        /* Use epoll_wait() to wait for a response from the server */
        if (epoll_wait(data->epoll_fd, events, MAX_EVENTS, -1) < 0)
        {
            DieWithError("epoll_wait failed");
        }

         /* TODO:
         * The function exits after sending and receiving a predefined number of messages (num_requests). 
         * It calculates the request rate based on total messages and RTT
         */

        if ((recv(data->client_fd, recv_buf, MESSAGE_SIZE, 0)) <= 0)
        {
            DieWithError("recv() failed or connection closed prematurely");
        } 
        
        gettimeofday(&end, NULL);

        /* calculate the total number of microsends that have elapsed */
        RTT = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
        data->total_rtt += RTT;
        data->total_messages += 1; /* Keep tally of total messages recieved */      
    }

    return NULL;
}

/*
 * This function orchestrates multiple client threads to send requests to a server,
 * collect performance data of each threads, and compute aggregated metrics of all threads.
 */
void run_client() {
    pthread_t threads[num_client_threads];
    client_thread_data_t thread_data[num_client_threads];
    struct sockaddr_in server_addr;

    /* TODO:
     * Create sockets and epoll instances for client threads
     * and connect these sockets of client threads to the server
     */

    /* Populate the server_addr struct so the client socket can connect to the server */
    server_addr.sin_family = AF_INET; /* Internet address family */ 
    server_addr.sin_addr.s_addr = inet_addr(server_ip); /* Server IP address */
    server_addr.sin_port = htons(server_port); /* Server port */ 

    /* Zero out garbage data in the thread_data struct */
    memset(thread_data, 0, sizeof(thread_data));

    for (int i = 0; i < num_client_threads; i++)
    {
        /* Create a reliable, stream socket using TCP */ 
        if ((thread_data[i].client_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            DieWithError(" socket() failed");
        } 
             
        /* Establish the connection to the echo server */ 
        if (connect(thread_data[i].client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        {
            DieWithError("connect() failed"); 
        } 

        /* Create the interest list (aka the epoll instance) for the client thread */
        thread_data[i].epoll_fd = epoll_create(0);
        if (thread_data[i].epoll_fd == -1) 
        {
            DieWithError("failed to the created the epoll instance for client");
        }   
    }
    
    // Hint: use thread_data to save the created socket and epoll instance for each thread
    // You will pass the thread_data to pthread_create() as below
    for (int i = 0; i < num_client_threads; i++) {
        pthread_create(&threads[i], NULL, client_thread_func, &thread_data[i]);
    }

    /* TODO:
     * Wait for client threads to complete and aggregate metrics of all client threads
     */

    long long total_rtt = 0;
    int total_messages = 0;
    double total_request_rate = 0;

    for (int i = 0; i < num_client_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < num_client_threads; i++)
    {
        total_rtt += thread_data[i].total_rtt;
        total_messages += thread_data[i].total_messages;
    }

    total_request_rate = (total_messages*1000000) / total_rtt;

    printf("Average RTT: %lld us\n", total_rtt / total_messages);
    printf("Total Request Rate: %f messages/s\n", total_request_rate);
}

void run_server() {

    /* TODO:
     * Server creates listening socket and epoll instance.
     * Server registers the listening socket to epoll
     */

    struct sockaddr_in ServAddr; /* Local address of server */ 
    struct sockaddr_in ClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */     
    unsigned int clntLen; /* Length of client address data structure */

    /* Create socket for incoming connections */ 
    int listenSock; /* Socket descriptor for server */ 
    int connSock; /* Socket descriptor for client */

    /* Initialize the ev struct for the server*/
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
        DieWithError ( "bind () failed"); 

    /* Mark the socket so it will listen for incoming connections */ 
    if (listen(listenSock, MAX_EVENTS) < 0)
    {
        DieWithError("listen() failed");
    } 
         
    /* Create the epoll instance for the server */
    int server_fd = epoll_create(10);
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
        /* TODO:
         * Server uses epoll to handle connection establishment with clients
         * or receive the message from clients and echo the message back
         */

        clntLen = sizeof(ClntAddr); /* Set the size of the in-out parameter */ 
        int nfds = epoll_wait(server_fd, events, MAX_EVENTS, -1);
        if (nfds < 0)
        {
            DieWithError("epoll_wait() error");
        }

        for (int n = 0; n < nfds; ++n) 
        {
            /* If the incoming message is a connection request from the client */
            if (events[n].data.fd == listenSock) 
            {
                /* Wait for a client to connect */     
                if ((connSock = accept(listenSock, (struct sockaddr *) &ClntAddr, &clntLen)) < 0)
                {
                    DieWithError("accept() failed"); 
                }

                setnonblocking(connSock);
                ev.events = EPOLLIN;
                ev.data.fd = connSock;

                /* Register the connection socket to epoll */
                if (epoll_ctl(server_fd, EPOLL_CTL_ADD, connSock, &ev) == -1) 
                {
                    DieWithError("failed to register server's connection socket to the interest list");
                }
            } 
            else 
            {
                HandleTCPClient(events[n].data.fd);
            }               
        }       
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        if (argc > 2) server_ip = argv[2];
        if (argc > 3) server_port = atoi(argv[3]);

        run_server();
    } else if (argc > 1 && strcmp(argv[1], "client") == 0) {
        if (argc > 2) server_ip = argv[2];
        if (argc > 3) server_port = atoi(argv[3]);
        if (argc > 4) num_client_threads = atoi(argv[4]);
        if (argc > 5) num_requests = atoi(argv[5]);

        run_client();
    } else {
        printf("Usage: %s <server|client> [server_ip server_port num_client_threads num_requests]\n", argv[0]);
    }

    return 0;
}