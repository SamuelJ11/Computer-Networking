#include <math.h>
#include "TimeInterval.h"

void TimeInterval(calcTimeIntval *packetdata) 
{
    /* Extract start and end times from the nested timespec structs that were populated by clock_gettime() */
    long start = (packetdata->start.tv_sec*1000000) + (packetdata->start.tv_nsec/1000); /* start time in µs */
    long end = (packetdata->end.tv_sec*1000000) + (packetdata->end.tv_nsec/1000); /* end time in µs */

    /* SampleRTT is cacluated calculated using the gettimeofday() function and the timeval struct */
    packetdata->SampleRTT = end - start;

    /* Upon obtaining a new SampleRTT, we update EstimatedRTT according to the formula below */
    packetdata->EstimatedRTT = (1 - packetdata->alpha)* packetdata->EstimatedRTT + packetdata->alpha*(packetdata->SampleRTT);

    /* We calculate DevRTT be an estimate of how much Sample RTT typically deviates from EstimatedRTT */
    packetdata->DevRTT = (1 - packetdata->beta)* packetdata->DevRTT + packetdata->beta*fabs(packetdata->SampleRTT - packetdata->EstimatedRTT);

    /* Compute the round trip time in microsends before populating the timespec struct*/
    long rt0_µs = packetdata->EstimatedRTT + 4*(packetdata->DevRTT); /* time in µs */
    
    /* Finally, obtain use the formula for retransmission timeout interval as a function of EstimatedRTT and DevRTT */
    packetdata->TimeoutInterval.tv_sec = rt0_µs  / 1000000; /* convert µs to s */
    packetdata->TimeoutInterval.tv_nsec = (rt0_µs % 1000000) * 1000; /* convert µs to ns safely without overflowing */
}