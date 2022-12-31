#ifndef LIST_H
#define LIST_H

#include <pthread.h>
#include "request_struct.h"
typedef struct Node
{
    RequestStruct *data;
    struct Node *next;
} Node;

typedef struct LinkedList
{
    Node *head;
    pthread_mutex_t lock;
} LinkedList;

LinkedList *listCreate();
void listDestroy(LinkedList *list);
void listAdd(LinkedList *list, RequestStruct *data);
void removeNode(LinkedList *list, RequestStruct *data);

#endif // LIST_H