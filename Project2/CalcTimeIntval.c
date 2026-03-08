#include <sys/time.h>  /* for struct timeval */
#include <math.h>

/* The stupid compiler keeps complaining so I had to copy and paste this */
typedef struct {
    double SampleRTT;
    double EstimatedRTT;
    double DevRTT;
    double alpha;
    double beta;
    double TimeoutInterval;
} calcTimeIntval;

void TimeInterval(calcTimeIntval *packetdata, struct timeval *start, struct timeval *end) 
{
    /* SampleRTT is cacluated calculated using the gettimeofday() function and the timeval struct */
    packetdata->SampleRTT = (end->tv_sec - start->tv_sec)*1000000 + (end->tv_usec - start->tv_usec);

    /* Upon obtaining a new SampleRTT, we update EstimatedRTT according to the formula below */
    packetdata->EstimatedRTT = (1 - packetdata->alpha)* packetdata->EstimatedRTT + packetdata->alpha*(packetdata->SampleRTT);

    /* We calculate DevRTT be an estimate of how much Sample RTT typically deviates from EstimatedRTT */
    packetdata->DevRTT = (1 - packetdata->beta)* packetdata->DevRTT + packetdata->beta*fabs(packetdata->SampleRTT - packetdata->EstimatedRTT);

    /* Finally, obtain use the formula for retransmission timeout interval as a function of EstimatedRTT and DevRTT */
    packetdata->TimeoutInterval = packetdata->EstimatedRTT + 4*(packetdata->DevRTT);
}