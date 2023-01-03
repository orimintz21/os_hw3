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
    Node *temp = q->head;
    q->head = q->head->next;
    if (q->head)
    {
        q->head->prev = NULL;
    }
    RequestStruct *data = temp->data;
    free(temp);
    q->count--;
    return data;
}

bool isEmpty(Queue *queue)
{
    return queue->count == 0;
}

int *randomNodes(int n, int k)
{
    int *numbers = malloc(sizeof(int) * n);
    int i, j, num;
    bool found;

    /* Seed the random number generator */
    srand(time(NULL));

    /* Generate the random numbers */
    for (i = 0; i < n; i++)
    {
        do
        {
            num = rand() % (k + 1);
            found = false;
            for (j = 0; j < i; j++)
            {
                if (numbers[j] == num)
                {
                    found = true;
                    break;
                }
            }
        } while (found);
        numbers[i] = num;
    }

    /* Sort the numbers */
    for (i = 0; i < n - 1; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (numbers[i] > numbers[j])
            {
                int temp = numbers[i];
                numbers[i] = numbers[j];
                numbers[j] = temp;
            }
        }
    }

    /* Print the numbers */
    for (i = 0; i < n; i++)
    {
        printf("%d\n", numbers[i]);
    }

    return numbers;
}

int removeRandom(Queue *queue)
{
    int to_remove = -1;
    if (queue->count == 0)
    {
        return -1;
    }
    to_remove = queue->count % 2 == 0 ? queue->count / 2 : (queue->count + 1) / 2;

    int *randoms = randomNodes(to_remove, queue->count);
    int i = 0;
    int j = 0;
    Node *temp = queue->head;
    while (i < queue->count)
    {
        if (i == randoms[j])
        {
            if (i == 0)
            {
                queue->head = queue->head->next;
                queue->head->prev = NULL;
                Close(temp->data->connfd);
                free(temp);
                temp = queue->head;
            }
            else
            {
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                Close(temp->data->connfd);
                free(temp);
                temp = temp->next;
            }
            j++;
        }
        i++;
    }
    queue->count = queue->count - j;
    return to_remove;
}