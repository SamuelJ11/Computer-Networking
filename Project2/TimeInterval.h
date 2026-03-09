#ifndef TIMEINTERVAL_H
#define TIMEINTERVAL_H

typedef struct {
    double SampleRTT;
    double EstimatedRTT;
    double DevRTT;
    double alpha;
    double beta;
    double TimeoutInterval;
} calcTimeIntval;

void TimeInterval(calcTimeIntval *packetdata, long long *start, long long *end);

#endif