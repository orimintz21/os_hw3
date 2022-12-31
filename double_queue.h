#ifndef DOUBLE_QUEUE_H
#define DOUBLE_QUEUE_H

#include "queue.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct
{
    Queue *waiting_queue;
    LinkedList *running_list;
    int max_size;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

} DQueueu;

DQueueu *dqueueuCreate(int max_size);
void dqueueuDestroy(DQueueu *dqueueu);
bool addToWaitingQueue(DQueueu *dqueueu, int connfd);
int addToRunningList(DQueueu *dqueueu);
void removeFromRunnig(DQueueu *dqueueu, int connfd);

#endif // DOUBLE_QUEUE_H