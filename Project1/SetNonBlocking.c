#include <unistd.h>
#include <fcntl.h>  
#include <stdio.h> /* for perror() */ 

void DieWithError(char *errorMessage); /* Error handling function */ 

int setnonblocking(int fd) 
{
    int flags;

    // Step 1: Get the current flags for the file descriptor
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }

    // Step 2: Use a "Bitwise OR" (|) to add the O_NONBLOCK flag to the existing flags
    flags |= O_NONBLOCK;

    // Step 3: Set the updated flags back to the file descriptor
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}