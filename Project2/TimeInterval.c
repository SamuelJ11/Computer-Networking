#include <math.h>
#include "TimeInterval.h"

void TimeInterval(calcTimeIntval *pt) 
{
    /* Extract start and end times from the nested timespec structs that were populated by clock_gettime() */
    long start = (pt->start.tv_sec*1000000) + (pt->start.tv_nsec/1000); /* start time in µs */
    long end = (pt->end.tv_sec*1000000) + (pt->end.tv_nsec/1000); /* end time in µs */

    /* SampleRTT is cacluated calculated using the gettimeofday() function and the timeval struct */
    pt->SampleRTT = end - start;

    /* Upon obtaining a new SampleRTT, we update EstimatedRTT according to the formula below */
    pt->EstimatedRTT = (1 - pt->alpha) * pt->EstimatedRTT + pt->alpha*(pt->SampleRTT);

    /* We calculate DevRTT be an estimate of how much Sample RTT typically deviates from EstimatedRTT */
    pt->DevRTT = (1 - pt->beta) * pt->DevRTT + pt->beta*fabs(pt->SampleRTT - pt->EstimatedRTT);

    /* Compute the round trip time in microsends before populating the timespec struct*/
    long rto_µs = pt->EstimatedRTT + 4*(pt->DevRTT); /* time in µs */
    
    /* Finally, use the formula for retransmission timeout interval as a function of EstimatedRTT and DevRTT */
    pt->TimeoutInterval.tv_sec = rto_µs  / 1000000; /* convert µs to s */
    pt->TimeoutInterval.tv_nsec = (rto_µs % 1000000) * 1000; /* convert µs to ns safely without overflowing */
}