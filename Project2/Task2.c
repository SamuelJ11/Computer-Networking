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
#include "Packet.h"

#define MAX_EVENTS 64
#define DEFAULT_CLIENT_THREADS 4
#define NUM_REQUESTS 40000
#define PIPELINE 4

char *server_ip = "127.0.0.1";
unsigned int server_port = 12345;
unsigned int num_threads = DEFAULT_CLIENT_THREADS;
unsigned int num_requests = NUM_REQUESTS;
unsigned short pipeline_size = PIPELINE;

volatile sig_atomic_t stop = 0; /* used for the signal handler we install later */
sigset_t sigmask; /* used for the signal mask in epoll_pwait2 */

/* Create a type alias named sighandler_t to simplify call to Signal() */
typedef void (*sighandler_t)(int);

/* Simple atomic signal handler */
void mysighandler(int signal)
{
    stop = 1;
}

/* Simple error reporting function */
static void DieWithError(char *errorMessage) 
{ 
    perror(errorMessage); 
    exit(1); 
}  

/* Implement a wrapper function called Signal() that handles the unwieldy sigaction() function call */
void Signal(int signum, sighandler_t handler)
{
    struct sigaction new_action;
    new_action.sa_handler = handler; /* assigns the function pointer passed to the wrapper to the handler field */
    sigfillset(&new_action.sa_mask); /* initializes the set to include every possible signal (all bits are 1) */

    if (sigaction(signum, &new_action, NULL) < 0)
    {
        DieWithError("sigaction() failed");
    }
}

/* This structure is used to store per-thread metadata in the client */
typedef struct {
    unsigned short progress; /* used for tracking the percent completion of each thread */
    int epoll_fd;       /* file descriptor for the epoll instance, used for monitoring events on the socket. */
    int client_fd;      /* file descriptor for the client socket on which data is sent to the server. */
    unsigned int retransmission; /* used for tracking the number of retransmissions for each thread */
    unsigned long tx_count;       /* accumulated number of sent packets for each thread */
    unsigned long rx_count;       /* accumulated number of recieved packets for each thread */
    unsigned long avg_timeout; /* average timeout interval for each thread (in µs) */
} clientthread_metadata;

/* This function runs in a separate client thread to handle communication with the server */ 
void *client_thread_func(void *arg) 
{
    clientthread_metadata *packetdata = (clientthread_metadata *)arg; /* initialize clientthread_metadata struct within the thread */
    calcTimeIntval packettiming; /* initialize calcTimeIntval (packet metadata) struct */
    
    /* Zero out accumulation metrics */
    packetdata->tx_count = 0;
    packetdata->rx_count = 0;
    packetdata->avg_timeout = 0;
    packetdata->progress = 0;
    packetdata->retransmission = 0;

    /* Zero out timeout metrics, set initial timeout interval to 200 microseconds (µs) */
    memset(&packettiming, 0, sizeof(packettiming)); /* Zero out structure before initializing */
    packettiming.alpha = 0.125; /* textbook value */
    packettiming.beta = 0.25; /* textbook value */
    packettiming.EstimatedRTT = 10; /* time in µs */
    packettiming.TimeoutInterval.tv_sec = 0; /* time in s */
    packettiming.TimeoutInterval.tv_nsec = (10000 * num_threads * pipeline_size); /* time in ns, scales with initial load */

    /* This value will represent timeout of each packet in µs */
    long timeout_µs = 0;

    /* Variables for epoll events, sending/receiving messages, and measuring RTT */
    struct epoll_event ev, events[MAX_EVENTS];
    int recvMsgSize; /* size of received message */ 
    int sentMsgSize; /* size of echoed message */

    /* Declare a client and server packet struct that will hold the packet payload and header (control) information */
    server_struct server_packet; /* this struct is only used for sending packets back to the server */
    client_struct client_packet; /* this struct is only used for deserializing packets from the client */

    /* Zero out the client packet struct before initializing */
    memset(&client_packet, 0, CLIENT_PACKET_SIZE); 
    
    /* Initialize the recieve buffer that will temporarily hold the deserialized server packet */
    char recv_buf[SERVER_PACKET_SIZE]; 

    /* Initialize the ev struct for the client and round trip time (RTT) metrics */
    ev.events = EPOLLIN;
    ev.data.fd = packetdata->client_fd;

    /* Register the socket from the interest list using epoll_ctl() */
    if (epoll_ctl(packetdata->epoll_fd, EPOLL_CTL_ADD, packetdata->client_fd, &ev) < 0) 
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

    /* Initialize the master loop control variable that represents the number of requests processed */
    int i = 0;

    /* Client thread sends messsages to the server and waits for a potential timeout before sending next packet */
    while (client_packet.base < num_requests && !stop) 
    {
        /* Use clock_gettime() to start the per-packet-burst timer as it provides nanosecond precision */
        /* Since the timespec struct is inside the packetdata struct, we pass the address of that field */
        clock_gettime(CLOCK_MONOTONIC, &packettiming.start);

        /* Only send if we are within the window and have requests left */
        while (i < client_packet.base + pipeline_size && i < num_requests) 
        {
            /* Assign the appropriate sequence number and serialize the packet into a byte stream for sending */
            client_packet.next_seqnum = i;
            char send_buf[CLIENT_PACKET_SIZE];
            SerializeClient(&client_packet, send_buf); 

            /* Send the message using sendto(), which includes destination arguments */
            sentMsgSize = sendto(packetdata->client_fd, send_buf, CLIENT_PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr*)&ServAddr, sizeof(ServAddr));

            if (sentMsgSize != CLIENT_PACKET_SIZE)
            {
                DieWithError("sendto() sent a different number of bytes than expected");
            }

            packetdata->tx_count++; /* update the number of sent packets */
            i++; /* update the master loop control variable */
        }
        /* Calculate the timeout in microseconds, updates with each iteration */
        timeout_µs = (packettiming.TimeoutInterval.tv_sec * 1000000) + (packettiming.TimeoutInterval.tv_nsec / 1000);

        /* Reset timeout to a minimum of 10 µs to avoid a potential infinite loop of timeouts if the TimeoutInterval becomes too small */
        if (timeout_µs < (10 * num_threads * pipeline_size))
        {
            /* Restore timeout and update the timespec struct that epoll_pwait2() actually uses */
            timeout_µs = (10 * num_threads * pipeline_size);
            packettiming.TimeoutInterval.tv_nsec = timeout_µs * 1000; /* timespec struct uses nanoseconds */
        }
        /* If timeout exceeds 1000x minimum threshold, something has gone seriously wrong */
        if (timeout_µs > (10000 * num_threads * pipeline_size)) 
        {
            puts("server is overloaded or not responding; maximum timeout exceeded");
            exit(1);
        }
        /* Wait until a packet arrives on the socket OR until the timeout expires */
        int nfds = epoll_pwait2(packetdata->epoll_fd, events, MAX_EVENTS, &packettiming.TimeoutInterval, &sigmask); /* now epoll_pwait2 can be interrupted */           
    
        if (nfds > 0) /* no time out */
        {
            while ((recvMsgSize = recvfrom(packetdata->client_fd, recv_buf, SERVER_PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr *)&fromAddr, &fromLen)) > 0) 
            {
                /* Deserialize the packets recieved from the client and reconstruct its data */
                DeserializeServer(recv_buf, &server_packet);

                /* Slide the window: the server says it's ready for server_packet.expected_seqnum */
                /* So everything before that is officially received */

                if (server_packet.expected_seqnum > client_packet.base)
                {
                    client_packet.base = server_packet.expected_seqnum;                
                }                
                /* Update client's next sequence number and receive counts */
                packetdata->rx_count++;
            }
            /* Something is functionally wrong with the socket */
            if (recvMsgSize < 0 && errno != EAGAIN)
            {
                DieWithError("recvfrom() failed or connection closed prematurely");
            }
            /* Use clock_gettime() to stop the per-packet-burst timer */
            clock_gettime(CLOCK_MONOTONIC, &packettiming.end);

            /* Recalculate the timeout interval using the TimeInterval() function */
            TimeInterval(&packettiming);  
        }
        else if (nfds == 0) /* timeout (packet loss) */
        {
            /* Double the TimeoutInterval (temporarily) to avoid a premature timeout for the next packet */
            timeout_µs *= 2;

            /* Update the timespec struct that epoll_pwait2() actually uses */
            packettiming.TimeoutInterval.tv_sec = timeout_µs / 1000000;
            packettiming.TimeoutInterval.tv_nsec = (timeout_µs % 1000000) * 1000;

            /* Update the number of retransmissions for the current thread */
            packetdata->retransmission += (i - client_packet.base); /* 'i' is where we were, 'client_packet.base' is where we are going back to */

            /* Go-Back-N: Reset 'i' to the 'base' so the next loop resends the whole window */
            i = client_packet.base;
        }
        else /* nfds < 0 */
        {
            DieWithError("epoll_pwait2() failed");
        }
        /* Update the average timeout for every 10% of messages sent */
        if (i > 0 && i % (NUM_REQUESTS / 10) == 0)
        {
            /* Add microsecond timeout values for the accumulation metrics in runclient() */
            packetdata->avg_timeout += (timeout_µs);
            packetdata->progress += 1; /* update the number of 10% increments completed */

            /* Testing purposes only */
            // printf("current timeout: %ld µs; ", timeout_µs);
            // printf("progress: %d%%\n", packetdata->progress * 10);
        }
    }

    return NULL;
}

/* This function orchestrates multiple client threads to send requests to a server,
collect performance data of each threads, and compute aggregated metrics of all threads */
void run_client() 
{
    pthread_t threads[num_threads];
    clientthread_metadata thread_data[num_threads];

    unsigned int num_threads_created = 0; /* keep track of the number of threads successfully created */
    unsigned long total_packets_sent = 0; /* accumulate the total number of sent packets for each thread */
    unsigned long total_packets_dropped = 0; /* accumulate the total number of dropped packets for each thread */
    unsigned long average_timeout = 0; /* accumulate the average timeout for each thread to compute an overall average timeout across all threads */
    unsigned long total_retransmissions = 0; /* accumulate the total number of retransmissions for each thread */

    /* Create sockets and epoll instances for client threads
    and connect these sockets of client threads to the server */
    int i = 0;
    while (i < num_threads && !stop)
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

        /* Update the number of threads actually created and loop control variable*/
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
        int thread_retransmissions = thread_data[i].retransmission; 

        /* Update accumulators */
        total_packets_sent += thread_tx_cnt;
        total_packets_dropped += thread_lost_pkt_cnt;
        total_retransmissions += thread_retransmissions;

        /* Add guard against floating point exception (division by zero) */
        if (thread_data[i].progress > 0) /* only update average timeout if the thread has sent at least 10% of its messages */
        {
            average_timeout += thread_data[i].avg_timeout / thread_data[i].progress;
        }
        /* Print thread results */
        printf("Results For Thread %d: \n\n", i + 1);
        printf("Total packets sent: %ld \n", thread_tx_cnt);
        printf("Total packets recieved: %ld \n", thread_rx_cnt);
        printf("Number of packets lost: %ld\n", thread_lost_pkt_cnt);
        printf("Number of retransmissions: %d\n", thread_retransmissions);
        puts("==============================================================");
    }

    puts("==============================================================");
    printf("Summary Statistics: Finished Processing %ld Total Requests \n\n", total_packets_sent);

    /* Calculate the loss proportion across all threads */
    float loss_proportion = (float)total_packets_dropped / (float)(total_packets_sent);

    printf("Average Packet Loss Rate Across All Threads: %.4f %%\n", loss_proportion*100);

    if (average_timeout > 0) /* only print average timeout if all threads have sent at least 10% of their packets */
    {
        printf("Average Timeout Interval Across all Threads: %.2ld µs\n", average_timeout / num_threads_created);
    }
    else
    {
        puts("Average Timeout Interval Across all Threads: NA");
    }

    printf("Total Number of Retransmissions Across All Threads: %ld\n", total_retransmissions);
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
    struct sockaddr_in ServAddr; /* local address of server */ 
    struct sockaddr_in ClntAddr; /* client address */     
    unsigned int clntLen; /* length of client address data structure */
    int recvMsgSize; /* size of received message */ 
    int sentMsgSize; /* size of echoed message */
   
    /* Initialize a boolean flag for tracking initial data receipt */
    unsigned char data_recieved = 0; 

    /* Declare a client and server packet struct that will hold the packet payload and header (control) information */
    server_struct server_packet; /* this struct is used for sending acknowledge packets back to the client */
    client_struct client_packet; /* this struct is only used for deserializing packets from the client */

    /* Initialize the buffer that will temporarily hold the deserialized packet from the client */
    char recv_buf[CLIENT_PACKET_SIZE]; 

    /* Initialize the event struct for the server*/
    int UDPSock;
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
    memset(&ServAddr, 0, sizeof(ServAddr)); /* zero out structure */
    ServAddr.sin_family = AF_INET; /* internet address family */ 
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* any incoming interface */
    ServAddr.sin_port = htons(server_port); /* server port */ 
    
    /* Bind the socket to address of the server */ 
    if (bind(UDPSock, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) < 0)
    {
        DieWithError ("bind() failed");
    } 
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
        clntLen = sizeof(ClntAddr); /* set the size of the in-out parameter */         
        int nfds = epoll_wait(server_fd, events, MAX_EVENTS, -1); /* wait indefinitely for events on the socket */

        if (nfds > 0)
        {
            /* Only print this message once per loop iteration */
            if (data_recieved == 0)
            {
                puts("server has successfully recieved data; responding to client ...");
                data_recieved = 1; /* do not enter this block again in the current loop iteration */
            }
            /* Recieve all ready packets in one go */
            while((recvMsgSize = recvfrom(UDPSock, recv_buf, CLIENT_PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr*)&ClntAddr, &clntLen)) >= 0)
            {
                /* Deserialize the packets recieved from the client and reconstruct its data */
                DeserializeClient(recv_buf, &client_packet); 

                /* Update the server's expected sequence number to be the next sequence number in the client's sequence number space */
                server_packet.expected_seqnum = client_packet.next_seqnum + 1;

                /* Echo the message back to the client*/
                memcpy(server_packet.echo_buf, client_packet.message, MESSAGE_SIZE); /* copy the message from the client packet to the server packet's echo buffer */
                char echo_buf[SERVER_PACKET_SIZE];

                SerializeServer(&server_packet, echo_buf); 
                sentMsgSize = sendto(UDPSock, echo_buf, SERVER_PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr*)&ClntAddr, sizeof(ClntAddr));

                if (sentMsgSize != SERVER_PACKET_SIZE) 
                {
                    DieWithError("sendto() sent a different number of bytes than expected");
                } 
            }
            /* Something is functionally wrong with the socket */
            if (recvMsgSize < 0 && errno != EAGAIN)
            {
                DieWithError("recvfrom() failed or connection closed prematurely");
            }
        }
        else if (nfds < 0 && !stop) /* if epoll_wait() returns an error other than being interrupted by a signal */
        {
            DieWithError("epoll_wait() failed");
        }          
    }

    puts("\ninterrupt signal recieved; shutting down server and closing UDP listening socket ...");

    /* Close the listening socket and the server epoll instance */
    close(UDPSock);
    close(server_fd);
}

int main(int argc, char *argv[]) 
{
    /* Install the signal handler to handle SIGINT */
    Signal(SIGINT, mysighandler);
    
    /* Initialize the mask used by epoll_pwait2() */
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
        if (argc > 4) num_threads = atoi(argv[4]); /* initialize number of threads (if given), defaults to 4 */
        if (argc > 5) num_requests = atoi(argv[5]); /* initialize number of requests (if given), defaults to 40000 */
        if (argc > 6) pipeline_size = atoi(argv[6]); /* initialize pipeline size (if given), defaults to 4 */

        run_client();
    } 
    else 
    {
        printf("Usage: %s <server|client> [server_ip server_port num_client_threads num_requests pipeline_size]\n", argv[0]);
    }

    return 0;
}