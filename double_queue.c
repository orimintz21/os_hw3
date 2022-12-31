#include "double_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

DQueueu *dqueueuCreate(int max_size)
{
    DQueueu *dqueueu = malloc(sizeof(DQueueu));
    dqueueu->waiting_queue = queueCreate(max_size);
    dqueueu->running_list = listCreate();
    dqueueu->count = 0;
    dqueueu->max_size = max_size;
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
bool addToWaitingQueue(DQueueu *dqueueu, int connfd)
{
    pthread_mutex_lock(&dqueueu->mutex);
    if (dqueueu->count == dqueueu->max_size)
    {
        // Queue is full
        pthread_mutex_unlock(&dqueueu->mutex);
        return false;
    }
    bool ans = enqueue(dqueueu->waiting_queue, connfd);
    if (ans)
    {
        dqueueu->count++;
    }
    pthread_cond_signal(&dqueueu->cond);
    pthread_mutex_unlock(&dqueueu->mutex);
    return ans;
}

int addToRunningList(DQueueu *dqueueu)
{
    pthread_mutex_lock(&dqueueu->mutex);
    int connfd = dequeue(dqueueu->waiting_queue);
    listAdd(dqueueu->running_list, connfd);
    pthread_mutex_unlock(&dqueueu->mutex);
    return connfd;
}

void removeFromRunnig(DQueueu *dqueueu, int connfd)
{
    pthread_mutex_lock(&dqueueu->mutex);
    dqueueu->count--;
    removeNode(dqueueu->running_list, connfd);
    pthread_mutex_unlock(&dqueueu->mutex);
}