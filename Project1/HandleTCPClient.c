#include <stdio.h>      
#include <sys/socket.h> 
#include <unistd.h> 
#include <errno.h>

#define MESSAGE_SIZE 16

void DieWithError(char *errorMessage); 

void HandleTCPClient(int clntSocket) 
{
    char echoBuffer[MESSAGE_SIZE];
    int recvMsgSize = recv(clntSocket, echoBuffer, MESSAGE_SIZE, 0);

    if (recvMsgSize < 0) 
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
        
        if (errno == ECONNRESET) 
        {
            printf("Client reset during recv (Socket %d)\n", clntSocket);
        } 
        else 
        {
            DieWithError("recv() failed");
        }

        close(clntSocket); // Clean up and return to epoll_wait
        return;
    } 

    if (recvMsgSize == 0) 
    {
        printf("Client disconnected gracefully (Socket %d)\n", clntSocket);
        close(clntSocket);
        return;
    } 

    // Logic for successful echo
    if (send(clntSocket, echoBuffer, recvMsgSize, 0) < 0) 
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return;
        }

        if (errno == ECONNRESET || errno == EPIPE) 
        {
            printf("Client closed during send (Socket %d)\n", clntSocket);
        } 
        else 
        {
            DieWithError("send() failed");
        }

        close(clntSocket); // Clean up and return to epoll_wait
    }
}