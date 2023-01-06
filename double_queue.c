#include "double_queue.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "segel.h"
#include <sys/time.h>

DQueue *dqueueCreate(int max_size, Policy policy, int num_threads)
{
    DQueue *dqueue = malloc(sizeof(DQueue));
    dqueue->waiting_queue = queueCreate();
    dqueue->running_list = malloc(sizeof(RequestStruct *) * num_threads);
    dqueue->count_running = 0;
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

int numOfRequests(DQueue *dqueue)
{
    return dqueue->count_running + dqueue->waiting_queue->count;
}

void addToWaitingQueue(DQueue *dqueue, RequestStruct *data)
{
    pthread_mutex_lock(&dqueue->mutex);
    int removed = 0;
    RequestStruct *last = NULL;
    if (numOfRequests(dqueue) >= dqueue->max_size)
    {
        // DQueue is full
        switch (dqueue->policy)
        {
        case BLOCK:
            while (numOfRequests(dqueue) >= dqueue->max_size)
                pthread_cond_wait(&dqueue->not_full, &dqueue->mutex);
            break;
        case DT:
            Close(data->connfd);
            free(data);
            pthread_mutex_unlock(&dqueue->mutex);
            return;
        case DH:
            last = dequeue(dqueue->waiting_queue);
            if (last == NULL)
            {
                Close(data->connfd);
                free(data);
                pthread_mutex_unlock(&dqueue->mutex);
                return;
            }
            else
            {
                Close(last->connfd);
                free(last);
            }
            break;
        case RANDOM:
            removed = removeRandom(dqueue->waiting_queue);
            if (removed == -1)
            {
                Close(data->connfd);
                free(data);
                pthread_mutex_unlock(&dqueue->mutex);
                return;
            }
            break;
        default:
            perror("Policy not found\n");
            exit(1);
        }
    }

    enqueue(dqueue->waiting_queue, data);
    pthread_cond_signal(&dqueue->not_empty);
    pthread_mutex_unlock(&dqueue->mutex);
}

RequestStruct *addToRunningList(DQueue *dqueue, Stats *stats)
{
    pthread_mutex_lock(&dqueue->mutex);
    int thread_id = stats->id;
    while (isEmpty(dqueue->waiting_queue))
        pthread_cond_wait(&dqueue->not_empty, &dqueue->mutex);

    RequestStruct *data = dequeue(dqueue->waiting_queue);
    struct timeval end_time;
    dqueue->running_list[thread_id] = data;
    gettimeofday(&end_time, NULL);
    stats->arrival_time = data->arrival_time;
    timersub(&end_time, &stats->arrival_time, &stats->dispatch_time);
    dqueue->count_running++;
    pthread_mutex_unlock(&dqueue->mutex);
    return data;
}

void removeFromRunning(DQueue *dqueue, int thread_id)
{
    pthread_mutex_lock(&dqueue->mutex);
    dqueue->count_running--;
    RequestStruct *data = dqueue->running_list[thread_id];
    Close(data->connfd);
    free(data);
    dqueue->running_list[thread_id] = NULL;
    pthread_cond_signal(&dqueue->not_full);
    pthread_mutex_unlock(&dqueue->mutex);
}