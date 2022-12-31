#include <pthread.h>
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include "request_struct.h"

// Initialize a linked list
LinkedList *listCreate()
{
    LinkedList *list = malloc(sizeof(LinkedList));
    list->head = NULL;
    pthread_mutex_init(&list->lock, NULL);
    return list;
}

void listDestroy(LinkedList *list)
{
    pthread_mutex_lock(&list->lock);
    Node *curr = list->head;
    while (curr != NULL)
    {
        Node *temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);
    free(list);
}

// Insert a new node at the beginning of the linked list
void listAdd(LinkedList *list, RequestStruct *data)
{
    pthread_mutex_lock(&list->lock);
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;
    pthread_mutex_unlock(&list->lock);
}

// Remove a node with the specified data value from the linked list
void removeNode(LinkedList *list, RequestStruct *data)
{
    pthread_mutex_lock(&list->lock);
    Node *prev = NULL;
    Node *curr = list->head;
    while (curr != NULL)
    {
        if (curr->data == data)
        {
            if (prev == NULL)
            {
                // Remove the first node
                list->head = curr->next;
            }
            else
            {
                // Remove a node other than the first
                prev->next = curr->next;
            }
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    pthread_mutex_unlock(&list->lock);
}