#include "queue.h"
#include <pthread.h>
#include "request_struct.h"
#include <stdlib.h>
#include <stdio.h>

Queue *queueCreate(int max_size)
{
    Queue *queue = malloc(sizeof(Queue));
    queue->data = malloc(max_size * sizeof(RequestStruct *));
    queue->front = 0;
    queue->rear = 0;
    queue->max_size = max_size;
    // pthread_mutex_init(&queue->mutex, NULL);
    // pthread_cond_init(&queue->cond, NULL);
    return queue;
}

void queueDestroy(Queue *queue)
{
    // pthread_cond_destroy(&queue->cond);
    // pthread_mutex_destroy(&queue->mutex);
    free(queue->data);
    free(queue);
}

int enqueue(Queue *queue, RequestStruct *value)
{
    // pthread_mutex_lock(&queue->mutex);
    if ((queue->rear + 1) % queue->max_size == queue->front)
    {
        // Queue is full
        // pthread_mutex_unlock(&queue->mutex);
        return 0;
    }
    queue->data[queue->rear] = value;
    queue->rear = (queue->rear + 1) % queue->max_size;
    // pthread_cond_signal(&queue->cond);
    // pthread_mutex_unlock(&queue->mutex);
    return 1;
}

RequestStruct *dequeue(Queue *queue)
{
    // pthread_mutex_lock(&queue->mutex);
    // while (queue->front == queue->rear)
    // {
    // Queue is empty
    // pthread_cond_wait(&queue->cond, &queue->mutex);
    // }

    RequestStruct *value = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->max_size;
    // pthread_mutex_unlock(&queue->mutex);
    return value;
}

void moveToStart(Queue *queue)
{
    int size = (queue->rear - queue->front + queue->max_size) % queue->max_size;

    // create a new array to hold the elements
    RequestStruct **newData = (RequestStruct **)malloc(sizeof(RequestStruct *) * size);
    int newIndex = 0;

    // copy the elements to the new array
    for (int i = 0; i < queue->max_size; i++)
    {
        if (i < size)
        {
            int index = (queue->front + i) % queue->max_size;
            newData[newIndex++] = queue->data[index];
        }
        else
        {
            newData[newIndex++] = NULL;
        }
    }

    // update the front and rear pointers
    queue->front = 0;
    queue->rear = size;

    // free the old data array and update the queue to use the new array
    free(queue->data);
    queue->data = newData;
}

int removeRandom(Queue *queue)
{
    // pthread_mutex_lock(&queue->mutex);
    moveToStart(queue);
    int size = (queue->rear - queue->front + queue->max_size) % queue->max_size;
    int index = rand() % queue->max_size;
    int num_to_remove = size / 2;

    RequestStruct **temp_data = (RequestStruct **)malloc(sizeof(RequestStruct *) * queue->max_size);
    for (int i = 0; i < queue->max_size; i++)
    {
        temp_data[i] = queue->data[i];
    }

    while (num_to_remove > 0)
    {
        if (temp_data[index] != NULL)
        {
            temp_data[index] = NULL;
            num_to_remove--;
        }
        index = rand() % queue->max_size;
    }
    int j = 0;
    int front = -1;
    for (int i = 0; i < queue->max_size; i++)
    {
        if (temp_data[i] != NULL)
        {
            if (front == -1)
            {
                front = i;
            }
            if (queue->data[j] != NULL)
            {
                free(queue->data[j]);
            }
            queue->data[j] = temp_data[i];
            j++;
        }
    }
    queue->front = front;
    queue->rear = j;
    for (int i = j; i < queue->max_size; i++)
    {
        if (queue->data[i] != NULL)
        {
            free(queue->data[i]);
        }
        queue->data[i] = NULL;
    }
    free(temp_data);
    // pthread_mutex_unlock(&queue->mutex);
    return size / 2;
}

bool isEmpty(Queue *queue)
{
    return queue->front == queue->rear;
}