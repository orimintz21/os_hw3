#include "double_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "segel.h"

DQueueu *dqueueuCreate(int max_size, Policy policy)
{
    DQueueu *dqueueu = malloc(sizeof(DQueueu));
    dqueueu->waiting_queue = queueCreate(max_size);
    dqueueu->running_list = listCreate();
    dqueueu->count = 0;
    dqueueu->max_size = max_size;
    dqueueu->policy = policy;
    pthread_mutex_init(&dqueueu->mutex, NULL);
    pthread_cond_init(&dqueueu->cond, NULL);
    return dqueueu;
};

void dqueueuDestroy(DQueueu *dqueueu)
{
    pthread_cond_destroy(&dqueueu->cond);
    pthread_mutex_destroy(&dqueueu->mutex);
    queueDestroy(dqueueu->waiting_queue);
    listDestroy(dqueueu->running_list);
    free(dqueueu);
}

void addToWaitingQueue(DQueueu *dqueueu, RequestStruct *data)
{
    pthread_mutex_lock(&dqueueu->mutex);
    if (dqueueu->count == dqueueu->max_size)
    {
        // Queue is full
        switch (dqueueu->policy)
        {
        case BLOCK:
            while (dqueueu->count == dqueueu->max_size)
                pthread_cond_wait(&dqueueu->mutex, &dqueueu->cond);
            break;
        case DT:
            Close(data->connfd);
            break;
        case DH:
            dequeue(dqueueu->waiting_queue);
            dqueueu->count--;
            break;
        case RANDOM:

            break;
        default:
            perror("Policy not found\n");
            pthread_mutex_unlock(&dqueueu->mutex);
            exit(1);
        }
    }
    bool ans = enqueue(dqueueu->waiting_queue, data);
    if (ans)
    {
        dqueueu->count++;
    }
    pthread_mutex_unlock(&dqueueu->mutex);
    return ans;
}

RequestStruct *addToRunningList(DQueueu *dqueueu)
{
    pthread_mutex_lock(&dqueueu->mutex);
    RequestStruct *data = dequeue(dqueueu->waiting_queue);
    listAdd(dqueueu->running_list, data);
    pthread_mutex_unlock(&dqueueu->mutex);
    return data;
}

void removeFromRunnig(DQueueu *dqueueu, RequestStruct *data)
{
    pthread_mutex_lock(&dqueueu->mutex);
    dqueueu->count--;
    removeNode(dqueueu->running_list, data);
    free(data);
    pthread_cond_signal(&dqueueu->cond);
    pthread_mutex_unlock(&dqueueu->mutex);
}