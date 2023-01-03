#ifndef QUEUE_H
#define QUEUE_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "request_struct.h"
#include <stdbool.h>

typedef struct node
{
    RequestStruct *data;
    struct node *next;
    struct node *prev;
} Node;

typedef struct queue
{
    Node *head;
    Node *tail;
    int count;
} Queue;

Queue *queueCreate();
void queueDestroy(Queue *queue);
void enqueue(Queue *queue, RequestStruct *data);
RequestStruct *dequeue(Queue *queue);
int removeRandom(Queue *queue);
bool isEmpty(Queue *queue);

#endif // QUEUE_H