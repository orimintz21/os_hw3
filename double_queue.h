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
    pthread_cond_t cond;

} DQueueu;

DQueueu *dqueueuCreate(int max_size, Policy policy);
void dqueueuDestroy(DQueueu *dqueueu);
void addToWaitingQueue(DQueueu *dqueueu, RequestStruct *data);
RequestStruct *addToRunningList(DQueueu *dqueueu);
void removeFromWaiting(DQueueu *dqueueu);
void removeFromRunning(DQueueu *dqueueu, RequestStruct *data);

#endif // DOUBLE_QUEUE_H