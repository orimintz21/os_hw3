#include "segel.h"
#include "request.h"
#include <pthread.h>
#include <stdbool.h>
#include "queue.h"
#include "double_queue.h"
#include <string.h>
#include "request_struct.h"
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
typedef struct
{
    int id;
    DQueueu *request;
} threadArgs;

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
    if (strcmp(argv[4], "block") == 0)
    {
        *policy = BLOCK;
    }
    else if (strcmp(argv[4], "dt") == 0)
    {
        *policy = DT;
    }
    else if (strcmp(argv[4], "dh") == 0)
    {
        *policy = DH;
    }
    else if (strcmp(argv[4], "random") == 0)
    {
        *policy = RANDOM;
    }
    else
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
}

void *thread_func(void *t_args)
{
    threadArgs *args = (threadArgs *)t_args;
    DQueueu *request = args->request;
    int id = args->id;
    int count = 0;
    struct timeval start_time;
    struct timeval end_time;
    int static_count = 0;
    int dynamic_count = 0;
    while (true)
    {
        RequestStruct *data = addToRunningList(request);
        int connfd = data->connfd;
        start_time = data->arrival_time;
        int is_static = requestHandle(connfd);
        gettimeofday(&end_time, NULL);
        double elapsed = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec);
        if (is_static == 0)
        {
            static_count++;
        }
        else if (is_static == 1)
        {
            dynamic_count++;
        }
        ++count;
        removeFromRunnig(request, data);
        Close(connfd);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, thread_count, queue_size;
    struct sockaddr_in clientaddr;

    Policy policy = BLOCK;

    getargs(&port, &thread_count, &queue_size, &policy, argc, argv);

    DQueueu *requests = dqueueuCreate(queue_size, policy);
    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; ++i)
    {
        threadArgs t_args;
        t_args.id = i;
        t_args.request = requests;
        pthread_create(&threads[i], NULL, thread_func, (void *)(&t_args));
    }

    listenfd = Open_listenfd(port);
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);

        RequestStruct *request = malloc(sizeof(RequestStruct));
        request->connfd = connfd;
        gettimeofday(&request->arrival_time, NULL);
        addToWaitingQueue(requests, request);

        //
        // HW3: In general, don't handle the request in the main thread.
        // Save the relevant info in a buffer and have one of the worker threads
        // do the work.
        //
    }
}