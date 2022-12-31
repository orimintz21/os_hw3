#ifndef QUEUE_H
#define QUEUE_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct
{
    int *data;
    int front;
    int rear;
    int max_size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

} Queue;

Queue *queueCreate(int max_size);
void queueDestroy(Queue *queue);
bool enqueue(Queue *queue, int data);
int dequeue(Queue *queue);
#endif // QUEUE_H