#ifndef QUEUE_H
#define QUEUE_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "request_struct.h"

typedef struct
{
    RequestStruct **data;
    int front;
    int rear;
    int max_size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

} Queue;

Queue *queueCreate(int max_size);
void queueDestroy(Queue *queue);
int enqueue(Queue *queue, RequestStruct *data);
RequestStruct *dequeue(Queue *queue);
#endif // QUEUE_H