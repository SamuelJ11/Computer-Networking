#include <math.h>
#include "TimeInterval.h"

void TimeInterval(calcTimeIntval *packetdata, long long *start, long long *end) 
{
    /* SampleRTT is cacluated calculated using the gettimeofday() function and the timeval struct */
    packetdata->SampleRTT = *end - *start;

    /* Upon obtaining a new SampleRTT, we update EstimatedRTT according to the formula below */
    packetdata->EstimatedRTT = (1 - packetdata->alpha)* packetdata->EstimatedRTT + packetdata->alpha*(packetdata->SampleRTT);

    /* We calculate DevRTT be an estimate of how much Sample RTT typically deviates from EstimatedRTT */
    packetdata->DevRTT = (1 - packetdata->beta)* packetdata->DevRTT + packetdata->beta*fabs(packetdata->SampleRTT - packetdata->EstimatedRTT);

    /* Finally, obtain use the formula for retransmission timeout interval as a function of EstimatedRTT and DevRTT */
    packetdata->TimeoutInterval = packetdata->EstimatedRTT + 4*(packetdata->DevRTT);
}