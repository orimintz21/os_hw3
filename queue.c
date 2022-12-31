#include "queue.h"
#include <pthread.h>
#include <stdbool.h>
#include "request_struct.h"

Queue *queueCreate(int max_size)
{
    Queue *queue = malloc(sizeof(Queue));
    queue->data = malloc(max_size * sizeof(RequestStruct *));
    queue->front = 0;
    queue->rear = 0;
    queue->max_size = max_size;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    return queue;
}

void queueDestroy(Queue *queue)
{
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->mutex);
    free(queue->data);
    free(queue);
}

bool enqueue(Queue *queue, RequestStruct *value)
{
    pthread_mutex_lock(&queue->mutex);
    if ((queue->rear + 1) % queue->max_size == queue->front)
    {
        // Queue is full
        pthread_mutex_unlock(&queue->mutex);
        return false;
    }
    queue->data[queue->rear] = value;
    queue->rear = (queue->rear + 1) % queue->max_size;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
    return true;
}

RequestStruct *dequeue(Queue *queue)
{
    pthread_mutex_lock(&queue->mutex);
    while (queue->front == queue->rear)
    {
        // Queue is empty
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    RequestStruct *value = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->max_size;
    pthread_mutex_unlock(&queue->mutex);
    return value;
}
