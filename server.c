#include "segel.h"
#include "request.h"
#include <pthread.h>
#include <stdbool.h>
#include "queue.h"
#include "double_queue.h"
#include <string.h>
//
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too

typedef enum
{
    BLOCK,
    DT,
    DH,
    RANDOM
} Policy;

void getargs(int *port, int *thread_count, int *queue_size, Policy *policy, int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *thread_count = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    if (strcmp(argv[4], "BLOCK") == 0)
    {
        *policy = BLOCK;
    }
    else if (strcmp(argv[4], "DT") == 0)
    {
        *policy = DT;
    }
    else if (strcmp(argv[4], "DH") == 0)
    {
        *policy = DH;
    }
    else if (strcmp(argv[4], "RANDOM") == 0)
    {
        *policy = RANDOM;
    }
    else
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
}

void *thread_func(void *request)
{
    while (true)
    {
        int connfd = addToRunningList((DQueueu *)request);
        requestHandle(connfd);
        removeFromRunnig((DQueueu *)request, connfd);
        Close(connfd);
    }
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, thread_count, queue_size;
    struct sockaddr_in clientaddr;

    Policy policy = BLOCK;

    getargs(&port, &thread_count, &queue_size, &policy, argc, argv);

    //
    // HW3: Create some threads...
    //
    DQueueu *request = dqueueuCreate(queue_size);
    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; ++i)
    {
        pthread_create(&threads[i], NULL, thread_func, (void *)request);
    }
    // Queue *waiting_requests = queueCreate(queue_size);
    // Queue *running_requests = queueCreate(thread_count);

    listenfd = Open_listenfd(port);
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
        if (!addToWaitingQueue(request, connfd))
        {
            // Queue is full
            switch (policy)
            {
            case BLOCK:
                /* code */
                break;
            case DT:
                /* code */
                break;
            case DH:
                /* code */
                break;
            default:
                break;
            }
            Close(connfd);
        }
        //
        // HW3: In general, don't handle the request in the main thread.
        // Save the relevant info in a buffer and have one of the worker threads
        // do the work.
        //
    }
}