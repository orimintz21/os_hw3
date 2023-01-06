#ifndef DOUBLE_QUEUE_H
#define DOUBLE_QUEUE_H

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "request_struct.h"
#include "stats.h"

typedef enum
{
    BLOCK,
    DT,
    DH,
    RANDOM
} Policy;
typedef struct
{
    Queue *waiting_queue;
    RequestStruct **running_list;
    int max_size;
    Policy policy;
    int count_running;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;

} DQueue;

DQueue *dqueueCreate(int max_size, Policy policy, int num_threads);
void dqueueDestroy(DQueue *dqueue);
void addToWaitingQueue(DQueue *dqueue, RequestStruct *data);
RequestStruct *addToRunningList(DQueue *dqueue, Stats *stats);
void removeFromWaiting(DQueue *dqueue);
void removeFromRunning(DQueue *dqueue, int thread_id);

#endif // DOUBLE_QUEUE_H