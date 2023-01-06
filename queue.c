#include "queue.h"
#include <pthread.h>
#include "request_struct.h"
#include <stdlib.h>
#include <stdio.h>
#include "segel.h"
Queue *queueCreate()
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
    return q;
}

void queueDestroy(Queue *q)
{
    while (q->head)
    {
        Node *temp = q->head;
        Close(q->head->data->connfd);
        q->head = q->head->next;
        free(temp);
    }
    free(q);
}

void enqueue(Queue *q, RequestStruct *data)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = q->tail;
    if (q->tail)
    {
        q->tail->next = new_node;
    }
    q->tail = new_node;
    if (!q->head)
    {
        q->head = new_node;
    }
    q->count++;
}

RequestStruct *dequeue(Queue *q)
{
    if (!q->head)
    {
        return NULL;
    }
    RequestStruct *data = q->head->data;
    Node *temp = q->head;
    q->head = q->head->next;
    if (q->head)
    {
        q->head->prev = NULL;
    }
    if (q->tail == temp)
    {
        q->tail = NULL;
    }
    free(temp);
    q->count--;
    return data;
}

bool isEmpty(Queue *queue)
{
    return queue->count == 0;
}

int removeNthNode(Queue *queue, int n)
{
    int to_remove = -1;
    if (n > queue->count)
    {
        return to_remove;
    }
    Node *temp = queue->head;
    for (int i = 0; i < n; i++)
    {
        temp = temp->next;
    }
    to_remove = temp->data->connfd;
    if (temp->prev)
    {
        temp->prev->next = temp->next;
    }
    if (temp->next)
    {
        temp->next->prev = temp->prev;
    }
    if (temp == queue->head)
    {
        queue->head = temp->next;
    }
    if (temp == queue->tail)
    {
        queue->tail = temp->prev;
    }
    free(temp);
    queue->count--;
    return to_remove;
}

int removeRandom(Queue *queue)
{
    int fd = -1;
    int to_remove = -1;
    if (queue->count == 0)
    {
        return to_remove;
    }
    to_remove = queue->count / 2 + (queue->count % 2);
    for (int i = 0; i < to_remove; ++i)
    {
        fd = removeNthNode(queue, abs(rand() % queue->count));
        Close(fd);
    }
    return to_remove;
}