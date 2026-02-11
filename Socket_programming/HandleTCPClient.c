#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* External error handling function */

void HandleTCPClient(int clntSocket) {
    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */

    /* Receive message from client */
    /* recv() blocks until the client sends something */
    recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0);
    if (recvMsgSize < 0) {
        DieWithError("recv() failed");
    }

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0) {           /* zero indicates end of transmission (client closed) */
        
        /* Echo message back to client */
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize) {
            DieWithError("send() failed");
        }

        /* See if there is more data to receive */
        recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0);
        if (recvMsgSize < 0) {
            DieWithError("recv() failed");
        }
    }

    /* Close client socket - the "Meeting Room" door is now locked */
    close(clntSocket);
}