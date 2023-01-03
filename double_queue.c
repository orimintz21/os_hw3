#include "double_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "segel.h"

DQueue *dqueueCreate(int max_size, Policy policy, int num_threads)
{
    DQueue *dqueue = malloc(sizeof(DQueue));
    dqueue->waiting_queue = queueCreate();
    dqueue->running_list = malloc(sizeof(RequestStruct *) * num_threads);
    dqueue->count = 0;
    dqueue->max_size = max_size;
    dqueue->policy = policy;
    pthread_mutex_init(&dqueue->mutex, NULL);
    pthread_cond_init(&dqueue->not_empty, NULL);
    pthread_cond_init(&dqueue->not_full, NULL);
    return dqueue;
};

void dqueueDestroy(DQueue *dqueue)
{
    pthread_cond_destroy(&dqueue->not_full);
    pthread_cond_destroy(&dqueue->not_empty);
    pthread_mutex_destroy(&dqueue->mutex);
    queueDestroy(dqueue->waiting_queue);
    free(dqueue->running_list);
    free(dqueue);
}

void addToWaitingQueue(DQueue *dqueue, RequestStruct *data)
{
    int removed = 0;
    RequestStruct *last = NULL;
    if (dqueue->count == dqueue->max_size)
    {
        // DQueue is full
        switch (dqueue->policy)
        {
        case BLOCK:
            while (dqueue->count == dqueue->max_size)
                pthread_cond_wait(&dqueue->not_full, &dqueue->mutex);
            break;
        case DT:
            Close(data->connfd);
            free(data);
            return;
            break;
        case DH:
            if (isEmpty(dqueue->waiting_queue))
            {
                Close(data->connfd);
                free(data);
                return;
            }
            else
            {
                last = dequeue(dqueue->waiting_queue);
                Close(last->connfd);
                free(last);
                dqueue->count--;
            }
            break;
        case RANDOM:
            removed = removeRandom(dqueue->waiting_queue);
            if (removed == -1)
            {
                Close(data->connfd);
                free(data);
                return;
            }
            dqueue->count -= removed;
            break;
        default:
            perror("Policy not found\n");
            exit(1);
        }
    }

    enqueue(dqueue->waiting_queue, data);
    dqueue->count++;
}

RequestStruct *addToRunningList(DQueue *dqueue, int thread_id)
{
    while (isEmpty(dqueue->waiting_queue))
        pthread_cond_wait(&dqueue->not_empty, &dqueue->mutex);

    RequestStruct *data = dequeue(dqueue->waiting_queue);
    dqueue->running_list[thread_id] = data;
    return data;
}

void removeFromRunning(DQueue *dqueue, int thread_id)
{
    dqueue->count--;
    RequestStruct *data = dqueue->running_list[thread_id];
    Close(data->connfd);
    free(data);
    dqueue->running_list[thread_id] = NULL;
}