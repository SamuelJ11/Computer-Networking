#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

#include "TimeInterval.h"

#define MAX_EVENTS 64
#define MESSAGE_SIZE 16
#define DEFAULT_CLIENT_THREADS 4
#define NUM_REQUESTS 1000000

char *server_ip = "127.0.0.1";
int server_port = 12345;
int num_client_threads = DEFAULT_CLIENT_THREADS;
int num_requests = NUM_REQUESTS;

static void DieWithError(char *errorMessage) 
{ 
    perror(errorMessage); 
    exit(1); 
}  

void SetNonBlocking(int fd); /* Function for setting non-blocking flags for file descriptors */

/* This structure is used to store per-thread data in the client */
typedef struct {
    int epoll_fd;       /* File descriptor for the epoll instance, used for monitoring events on the socket. */
    int client_fd;      /* File descriptor for the client socket on which data is sent to the server. */
    long tx_count;       /* Accumulated number of sent packets for each thread */
    long rx_count;       /* Accumulated number of recieved packets for each thread */
} client_thread_data_t;

/* This function runs in a separate client thread to handle communication with the server */ 
void *client_thread_func(void *arg) 
{
    client_thread_data_t *data = (client_thread_data_t *)arg; /* initialize client_thread_data_t struct within the thread */
    calcTimeIntval packetdata; /* initialize calcTimeIntval struct */
    long long starttime = 0, endtime = 0; /* used for passing start and endtimes to TimeInterval function */
   
    /* Zero out accumulation metrics */
    data->tx_count = 0;
    data->rx_count = 0;

    /* Zero out timeout metrics, set initial timeout interval to 1 second (1000000 µs) */
    memset(&packetdata, 0, sizeof(packetdata)); /* Zero out structure */
    packetdata.alpha = 0.125;
    packetdata.beta = 0.25;
    packetdata.EstimatedRTT = 10000;
    packetdata.TimeoutInterval = 1000000;

    /* Variables for epoll events, sending/receiving messages, and measuring RTT */
    struct epoll_event ev, events[MAX_EVENTS];
    char send_buf[MESSAGE_SIZE] = "ABCDEFGHIJKMLNOP"; /* Send 16-Bytes message every time */
    char recv_buf[MESSAGE_SIZE];
    struct timeval start, end;

    /* Set client_fd to non-blocking when epoll starts watching it*/
    SetNonBlocking(data->client_fd);

    /* Initialize the ev struct for the client and round trip time (RTT) metrics */
    ev.events = EPOLLIN;
    ev.data.fd = data->client_fd;

    /* Register the connected socket from the interest list using epoll_ctl() */
    if (epoll_ctl(data->epoll_fd, EPOLL_CTL_ADD, data->client_fd, &ev) < 0) 
    {
        DieWithError("failed to register client's connection socket to the interest list");
    }

    /* Since UDP is connectionless, each packet must include the source and destination address */
    struct sockaddr_in ServAddr;  /* IPv4 address struct for server */
    ServAddr.sin_family = AF_INET; /* Internet address family */ 
    ServAddr.sin_addr.s_addr = inet_addr(server_ip); /* Server IP address */
    ServAddr.sin_port = htons(server_port); /* Server port */ 

    /* Initialize the struct that recvfrom() will populate */
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);

    /* Client thread sends messsages to the server and waits for a potential timeout before sending next packet */
    for (int i = 0; i < num_requests; i++) 
    {
        /* Use gettimeofday() to start the per-packet timer */
        gettimeofday(&start, NULL);

        if (sendto(data->client_fd, send_buf, MESSAGE_SIZE, 0, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) != MESSAGE_SIZE)
        {
            DieWithError("sendto() sent a different number of bytes than expected");
        }

        /* Update the timout interval for epoll for each request */
        int timeout_ms = packetdata.TimeoutInterval;
        
        /* Wait until a packet arrives on the socket OR until the timeout expires */
        int nfds = epoll_wait(data->epoll_fd, events, MAX_EVENTS, timeout_ms);

        if (nfds > 0) /* No time out */
        {
            recvfrom(data->client_fd, recv_buf, MESSAGE_SIZE, 0, (struct sockaddr *)&fromAddr, &fromLen);
            gettimeofday(&end, NULL);

            /* Extract the start and end times to pass to TimeInterval() function */
            starttime = start.tv_sec*(1000) + (start.tv_usec / 1000);
            endtime = end.tv_sec*(1000) + (end.tv_usec / 1000);

            /* Recalculate the timeout interval using the TimeInterval() function */
            TimeInterval(&packetdata, &starttime, &endtime);  

            data->tx_count ++;
            data->rx_count ++;
        }
        else if (nfds == 0) /* timeout (packet loss) */
        {
            data->tx_count++;
            packetdata.TimeoutInterval *= 2;  /* double the TimeoutInterval (temporarily) to avoid a premature timeout for the next packet */
        }
        else
        {
             DieWithError("epoll_wait() failed");
        }
    }

    return NULL;
}

/* This function orchestrates multiple client threads to send requests to a server,
collect performance data of each threads, and compute aggregated metrics of all threads */
void run_client() 
{
    pthread_t threads[num_client_threads];
    client_thread_data_t thread_data[num_client_threads];

    /* Create sockets and epoll instances for client threads
    and connect these sockets of client threads to the server */    
    for (int i = 0; i < num_client_threads; i++)
    {
        /* Create an unreliable, UDP datagram socket using UDP */ 
        if ((thread_data[i].client_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        {
            DieWithError("socket() failed");
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

    /* Wait for client threads to complete and aggregate metrics of all client threads */
    puts("==============================================================");
    for (int i = 0; i < num_client_threads; i++) 
    {
        pthread_join(threads[i], NULL);
        /* initialize per-thread data*/       
        long thread_tx_cnt = thread_data[i].tx_count;
        long thread_rx_cnt = thread_data[i].rx_count;
        long thread_lost_pkt_cnt = thread_tx_cnt - thread_rx_cnt;
        
        printf("Results For Thread %d: \n\n", i + 1);
        printf("Total packets sent: %ld \n", thread_tx_cnt);
        printf("Total packets recieved: %ld \n", thread_rx_cnt);
        printf("Number of packets lost: %ld\n", thread_lost_pkt_cnt);
        puts("==============================================================");
    }
}

void run_server() 
{
    struct sockaddr_in ServAddr; /* Local address of server */ 
    struct sockaddr_in ClntAddr; /* Client address */     
    unsigned int clntLen; /* Length of client address data structure */
    char echobuf[MESSAGE_SIZE]; /* length of message to echo back to client */
    int recvMsgSize; /* Size of received message */ 
    int sentMsgSize; /* Size of echoed message */

    /* Create socket for incoming connections */ 
    int UDPSock; 

    /* Initialize the event struct for the server*/
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN; /* The server is listening for read operation */


    if ((UDPSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        DieWithError("socket() failed");
    }

    /* Set the file descriptor for the server to be the generic UDP socket */
    ev.data.fd = UDPSock;
    
    /* Construct local address structure */ 
    memset(&ServAddr, 0, sizeof(ServAddr)); /* Zero out structure */
    ServAddr.sin_family = AF_INET; /* Internet address family */ 
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    ServAddr.sin_port = htons(server_port); /* Server port */ 
    
    /* Bind the socket to address of the server */ 
    if (bind(UDPSock, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) < 0)
    {
        DieWithError ("bind() failed");
    } 

    /* Set UDP socket to non-blocking to ensure recvfrom() returns immediately if no 
    data is ready, rather than freezing the thread */
    SetNonBlocking(UDPSock);
    
    /* Create the epoll instance for the server */
    int server_fd = epoll_create(1);
    if (server_fd < 0) 
    {
        DieWithError("failed to the created the epoll instance for server");
    } 
    /* Register the listening socket to epoll */
    if (epoll_ctl(server_fd, EPOLL_CTL_ADD, UDPSock, &ev) < 0) 
    {
        DieWithError("failed to register server's listening socket to the interest list");
    }          

    /* Server's run-to-completion event loop */
    while (1) 
    {
        clntLen = sizeof(ClntAddr); /* Set the size of the in-out parameter */ 
        
        if (epoll_wait(server_fd, events, MAX_EVENTS, -1) < 0)
        {
            DieWithError("epoll_wait() failed");
        }

        while ((recvMsgSize = recvfrom(UDPSock, echobuf, MESSAGE_SIZE, 0, (struct sockaddr*)&ClntAddr, &clntLen)) >= 0)
        {
            sentMsgSize = sendto(UDPSock, echobuf, recvMsgSize, 0, (struct sockaddr*)&ClntAddr, sizeof(ClntAddr));

            if (sentMsgSize != recvMsgSize) 
            {
                DieWithError("sendto() sent a different number of bytes than expected");
            }
        }

        if (recvMsgSize < 0 && errno != EAGAIN)
        {
            DieWithError("recvfrom() failed or connection closed prematurely");
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