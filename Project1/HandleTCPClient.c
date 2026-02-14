#include <stdio.h>      
#include <sys/socket.h> 
#include <unistd.h> 

#define MESSAGE_SIZE 16

void HandleTCPClient(int clntSocket) {
    char echoBuffer[MESSAGE_SIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */

    recvMsgSize = recv(clntSocket, echoBuffer, MESSAGE_SIZE, 0);
    if (recvMsgSize < 0) 
    {
        DieWithError("recv() failed");
    }

    else if (recvMsgSize == 0) 
    {
        printf("Client disconnected (Socket %d)\n", clntSocket);
    }   
    else 
    {
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize) 
        {
            DieWithError("send() failed");
        }
    }

    close(clntSocket);
}