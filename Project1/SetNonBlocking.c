#include <unistd.h>
#include <fcntl.h>  
#include <stdio.h>
#include <stdlib.h>

static void DieWithError(char *errorMessage) 
{ 
    perror(errorMessage); 
    exit(1); 
} 

int SetNonBlocking(int fd) 
{
    int flags;

    /* Get the current flags for the file descriptor */
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) 
    {
        DieWithError("fcntl F_GETFL");
    }

    /* Use a "Bitwise OR" (|) to add the O_NONBLOCK flag to the existing flags */
    flags |= O_NONBLOCK;

    /* Set the updated flags back to the file descriptor */
    if (fcntl(fd, F_SETFL, flags) == -1) 
    {
        DieWithError("fcntl F_SETFL");
    }

    return 0;
}