#ifndef DOUBLE_QUEUE_H
#define DOUBLE_QUEUE_H

#include "queue.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "request_struct.h"

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
    LinkedList *running_list;
    int max_size;
    Policy policy;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;

} DQueue;

DQueue *dqueueCreate(int max_size, Policy policy);
void dqueueDestroy(DQueue *dqueue);
void addToWaitingQueue(DQueue *dqueue, RequestStruct *data);
RequestStruct *addToRunningList(DQueue *dqueue);
void removeFromWaiting(DQueue *dqueue);
void removeFromRunning(DQueue *dqueue, RequestStruct *data);

#endif // DOUBLE_QUEUE_H