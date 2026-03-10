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
#include <signal.h>

#include "TimeInterval.h"

#define MAX_EVENTS 64
#define MESSAGE_SIZE 16
#define DEFAULT_CLIENT_THREADS 4
#define NUM_REQUESTS 1000000

char *server_ip = "127.0.0.1";
int server_port = 12345;
int num_client_threads = DEFAULT_CLIENT_THREADS;
int num_requests = NUM_REQUESTS;

volatile sig_atomic_t stop = 0; /* used for the signal handler we install later */
sigset_t sigmask; /* used for the signal mask in epoll_pwait2 */

/* Create a type alias named sighandler_t to simplify call to Signal() */
typedef void (*sighandler_t)(int);

/* Simple atomic signal handler */
void mysighandler(int signal)
{
    stop = 1;
}

static void DieWithError(char *errorMessage) 
{ 
    perror(errorMessage); 
    exit(1); 
}  

/* Implement a wrapper function called Signal() that handles the unwiedly sigaction() function call */
void Signal(int signum, sighandler_t handler)
{
    struct sigaction new_action;
    new_action.sa_handler = handler;        // assigns the function pointer passed to the wrapper to the handler field
    sigfillset(&new_action.sa_mask);        // initializes the set to include every possible signal (all bits are 1)

    if (sigaction(signum, &new_action, NULL) < 0)
    {
        DieWithError("sigaction() failed");
    }
}

void SetNonBlocking(int fd); /* Function for setting non-blocking flags for file descriptors */

/* This structure is used to store per-thread data in the client */
typedef struct {
    int epoll_fd;       /* File descriptor for the epoll instance, used for monitoring events on the socket. */
    int client_fd;      /* File descriptor for the client socket on which data is sent to the server. */
    long tx_count;       /* Accumulated number of sent packets for each thread */
    long rx_count;       /* Accumulated number of recieved packets for each thread */
    long long avg_timeout; /* Average timeout interval for each thread (in µs) */
} client_thread_data_t;

/* This function runs in a separate client thread to handle communication with the server */ 
void *client_thread_func(void *arg) 
{
    client_thread_data_t *data = (client_thread_data_t *)arg; /* initialize client_thread_data_t struct within the thread */
    calcTimeIntval packetdata; /* initialize calcTimeIntval struct */
    
    /* Zero out accumulation metrics */
    data->tx_count = 0;
    data->rx_count = 0;
    data->avg_timeout = 0;

    /* Zero out timeout metrics, set initial timeout interval to 200 microseconds (µs) */
    memset(&packetdata, 0, sizeof(packetdata)); /* Zero out structure before initializing */
    packetdata.alpha = 0.125; /* textbook value */
    packetdata.beta = 0.25; /* textbook value */
    packetdata.EstimatedRTT = 10; /* time in µs */
    packetdata.TimeoutInterval.tv_sec = 0; /* time in s (this value will pretty much always be zero) */
    packetdata.TimeoutInterval.tv_nsec = 100000; /* time in ns */

    /* This value will represent timeout of each packet in µs */
    long timeout_µs = 0;

    /* Variables for epoll events, sending/receiving messages, and measuring RTT */
    struct epoll_event ev, events[MAX_EVENTS];
    char send_buf[MESSAGE_SIZE] = "ABCDEFGHIJKMLNOP"; /* Send 16-Bytes message every time */
    char recv_buf[MESSAGE_SIZE];

    /* Set client_fd to non-blocking when epoll starts watching it*/
    SetNonBlocking(data->client_fd);

    /* Initialize the ev struct for the client and round trip time (RTT) metrics */
    ev.events = EPOLLIN;
    ev.data.fd = data->client_fd;

    /* Register the socket from the interest list using epoll_ctl() */
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
    int i = 0;
    while (i < num_requests && !stop) 
    {
        /* Use clock_gettime() to start the per-packet timer as it provides nanosecond precision */
        /* Since the timespec struct is inside the packetdata struct, we pass the address of that field */
        clock_gettime(CLOCK_MONOTONIC, &packetdata.start);

        /* Send the message using sendto(), which includes destination arguments */
        if (sendto(data->client_fd, send_buf, MESSAGE_SIZE, 0, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) != MESSAGE_SIZE)
        {
            DieWithError("sendto() sent a different number of bytes than expected");
        }

        /* Calculate the timeout in microseconds, updates with each iteration */
        timeout_µs = (packetdata.TimeoutInterval.tv_sec * 1000000) + (packetdata.TimeoutInterval.tv_nsec / 1000);

        /* Reset timeout to a minimum of 100 µs to avoid a potential infinite loop of timeouts if the TimeoutInterval becomes too small */
        if (timeout_µs < 100)
        {
            packetdata.TimeoutInterval.tv_nsec = 100000; 
        }

        if (timeout_µs > 1000000) /* If timeout exceeds 1 second, something has gone seriously wrong */
        {
            puts("server is overloaded or not responding; maximum timeout exceeded");
            exit(1);
        }

        /* Wait until a packet arrives on the socket OR until the timeout expires */
        int nfds = epoll_pwait2(data->epoll_fd, events, MAX_EVENTS, &packetdata.TimeoutInterval, &sigmask); /* now epoll_pwait2 can be interrupted */           
    
        if (nfds > 0) /* no time out */
        {
            /* Recieve the data using recvfrom() which includes source arguments */
            recvfrom(data->client_fd, recv_buf, MESSAGE_SIZE, 0, (struct sockaddr *)&fromAddr, &fromLen);
            clock_gettime(CLOCK_MONOTONIC, &packetdata.end);

            /* Recalculate the timeout interval using the TimeInterval() function */
            TimeInterval(&packetdata);  

            /* Update both send and recieve counts */
            data->tx_count ++;
            data->rx_count ++;  
        }
        else if (nfds == 0) /* timeout (packet loss) */
        {
            /* Only update the number of sent packets */
            data->tx_count++; 

            /* Double the TimeoutInterval (temporarily) to avoid a premature timeout for the next packet */
            timeout_µs *= 2;

            /* Update the timespec struct that epoll_pwait2() actually uses */
            packetdata.TimeoutInterval.tv_sec = timeout_µs / 1000000;
            packetdata.TimeoutInterval.tv_nsec = (timeout_µs % 1000000) * 1000;
        }
        else /* nfds < 0 */
        {
            DieWithError("epoll_pwait2() failed");
        }

        /* Update the average timeout for every 10% of messages sent */
        if (i % (NUM_REQUESTS / 10) == 0)
        {
            /* Add microsecond timeout values for the accumulation metrics in runclient() */
            data->avg_timeout += (timeout_µs);
        }

        i++;
    }

    return NULL;
}

/* This function orchestrates multiple client threads to send requests to a server,
collect performance data of each threads, and compute aggregated metrics of all threads */
void run_client() 
{
    pthread_t threads[num_client_threads];
    client_thread_data_t thread_data[num_client_threads];

    int num_threads_created = 0; /* keep track of the number of threads successfully created */
    long total_packets_sent = 0; /* accumulate the total number of sent packets for each thread */
    long total_packets_dropped = 0; /* accumulate the total number of dropped packets for each thread */
    long average_timeout = 0; /* accumulate the average timeout for each thread to compute an overall average timeout across all threads */

    /* Create sockets and epoll instances for client threads
    and connect these sockets of client threads to the server */
    int i = 0;
    while (i < num_client_threads && !stop)
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

        num_threads_created++;
        i++;
    }

    /* Wait for client threads to complete and aggregate metrics of all client threads */
    puts("==============================================================");
    for (int i = 0; i < num_threads_created; i++) 
    {
        pthread_join(threads[i], NULL);

        /* Initialize per-thread data */       
        long thread_tx_cnt = thread_data[i].tx_count;
        long thread_rx_cnt = thread_data[i].rx_count;
        long thread_lost_pkt_cnt = thread_tx_cnt - thread_rx_cnt;

        /* Update accumulators */
        total_packets_sent += thread_tx_cnt;
        total_packets_dropped += thread_lost_pkt_cnt;
        average_timeout += thread_data[i].avg_timeout / 10; 
        
        printf("Results For Thread %d: \n\n", i + 1);
        printf("Total packets sent: %ld \n", thread_tx_cnt);
        printf("Total packets recieved: %ld \n", thread_rx_cnt);
        printf("Number of packets lost: %ld\n", thread_lost_pkt_cnt);
        puts("==============================================================");
    }

    /* Wait for client threads to complete and aggregate metrics of all client threads */
    float loss_proportion = (float)total_packets_dropped / (float)(total_packets_sent);

    puts("==============================================================");
    printf("Summary Statistics: Finished Processing %ld Total Requests \n\n", total_packets_sent);
    printf("Average Packet Loss Rate Across All Threads: %.2f %%\n", loss_proportion*100);
    printf("Average Timeout Across all Threads: %.2ld µs\n", average_timeout / num_threads_created);
    puts("==============================================================");
    
    /* Close all client sockets */
    puts("client is shutting down, closing all thread sockets ...");
    for (int i = 0; i < num_threads_created; i++)
    {
        if (thread_data[i].client_fd > 0) /* socket is open */
        {
            close(thread_data[i].client_fd); /* close it */
            close(thread_data[i].epoll_fd); /* close the epoll instance */
        }    
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

    /* Intitialize a file descriptor that will become the UDP server socket */  
    int UDPSock; 

    /* Initialize the event struct for the server*/
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN; /* The server is listening for read operation */

    /* Create socket for incoming connections */
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
    while (!stop) 
    {
        clntLen = sizeof(ClntAddr); /* Set the size of the in-out parameter */ 
        
        if (epoll_wait(server_fd, events, MAX_EVENTS, -1) < 0 && errno != EINTR)
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

    puts("\ninterrupt signal recieved; shutting down server and closing UDP listening socket ...");
    close(UDPSock);
}

int main(int argc, char *argv[]) 
{
    /* Install the signal handler to handle SIGINT */
    Signal(SIGINT, mysighandler);
    
    /* Initialize the mask used by epoll_pwait2 */
    sigemptyset(&sigmask);

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