#include <unistd.h>
#include <fcntl.h>  
#include <stdio.h>
#include <stdlib.h>

static void DieWithError(char *errorMessage) 
{ 
    perror(errorMessage); 
    exit(1); 
} 

void SetNonBlocking(int fd) 
{
    int flags;

    /* Get the current flags for the file descriptor */
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0) 
    {
        DieWithError("failed to fetch flags for descriptor");
    }

    flags |= O_NONBLOCK;  /* Use a "Bitwise OR" (|) to add the O_NONBLOCK flag to the existing flags */

    /* Set the updated flags back to the file descriptor */
    if (fcntl(fd, F_SETFL, flags) < 0) 
    {
        DieWithError("failed to set flags for descriptor");
    }
}