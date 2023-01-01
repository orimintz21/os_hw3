#ifndef STATS_H
#define STATS_H

#include <pthread.h>
#include <sys/time.h>
typedef struct
{
    int id;
    int count;
    int static_count;
    int dynamic_count;
    struct timeval arrival_time;
    struct timeval dispatch_time;

} Stats;
#endif