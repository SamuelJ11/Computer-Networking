#include <stdio.h>      
#include <sys/socket.h> 
#include <unistd.h> 
#include <errno.h>
#include <stdlib.h>

#define MESSAGE_SIZE 16

static void DieWithError(char *errorMessage) 
{ 
    perror(errorMessage); 
    exit(1); 
} 

void HandleTCPClient(int clntSocket) 
{
    char echoBuffer[MESSAGE_SIZE];
    int recvMsgSize = recv(clntSocket, echoBuffer, MESSAGE_SIZE, 0);

    if (recvMsgSize > 0)  /* Successful read: Attempt to echo the data back */
    {
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) < 0 && errno != EAGAIN) 
        {
            DieWithError("send() failed"); /* A fatal error occurred, not just a "try again later" notification */
        }
    } 
    else if (recvMsgSize == 0) /* Client is done sending, close its socket */
    {
        printf("Client disconnected gracefully at connection socket %d\n", clntSocket);
        close(clntSocket);
    } 
    else if (errno != EAGAIN) 
    {
        DieWithError("recv() failed"); /* A fatal error occurred, not just a "try again later" notification */ 
    }
}