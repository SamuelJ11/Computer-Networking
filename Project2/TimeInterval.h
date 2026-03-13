#ifndef TIMEINTERVAL_H
#define TIMEINTERVAL_H

#include <time.h>
#include <math.h>

typedef struct {
    struct timespec start;
    struct timespec end;
    struct timespec TimeoutInterval; /* this needs to be a struct for epoll_pwait2() */
    double SampleRTT;
    double EstimatedRTT;
    double DevRTT;
    double alpha;
    double beta;
} calcTimeIntval;

void TimeInterval(calcTimeIntval *packettiming);

#endif