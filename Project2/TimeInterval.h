#ifndef TIMEINTERVAL_H
#define TIMEINTERVAL_H

#include <time.h>

typedef struct {
    struct timespec start;
    struct timespec end;
    double SampleRTT;
    double EstimatedRTT;
    double DevRTT;
    double alpha;
    double beta;
    struct timespec TimeoutInterval; /* this needs to be a struct for epoll_pwait2() */
} calcTimeIntval;

void TimeInterval(calcTimeIntval *packetdata);

#endif