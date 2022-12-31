#ifndef LIST_H
#define LIST_H

#include <pthread.h>

typedef struct Node
{
    int data;
    struct Node *next;
} Node;

typedef struct LinkedList
{
    Node *head;
    pthread_mutex_t lock;
} LinkedList;

LinkedList *listCreate();
void listDestroy(LinkedList *list);
void listAdd(LinkedList *list, int data);
void removeNode(LinkedList *list, int data);

#endif // LIST_H