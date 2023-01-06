#include "segel.h"
#include "request.h"
#include <pthread.h>
#include "double_queue.h"
#include <string.h>
#include "request_struct.h"
#include "stats.h"
#include <sys/time.h>
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
    DQueue *request;
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
    char *policy_str = argv[4];
    if (strcmp(policy_str, "block") == 0)
    {
        *policy = BLOCK;
    }
    else if (strcmp(policy_str, "dt") == 0)
    {
        *policy = DT;
    }
    else if (strcmp(policy_str, "dh") == 0)
    {
        *policy = DH;
    }
    else if (strcmp(policy_str, "random") == 0)
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
    DQueue *dqueue = args->request;
    Stats stats;
    stats.count = 0;
    stats.static_count = 0;
    stats.dynamic_count = 0;
    stats.id = args->id;

    while (1)
    {
        RequestStruct *data = addToRunningList(dqueue, &stats);
        int connfd = data->connfd;
        requestHandle(connfd, &stats);
        removeFromRunning(dqueue, stats.id);
    }
    return NULL;
}

int main(int argc, char *argv[])
{

    int listenfd, connfd, port, clientlen, thread_count, queue_size;
    struct sockaddr_in clientaddr;
    Policy policy = BLOCK;

    getargs(&port, &thread_count, &queue_size, &policy, argc, argv);

    DQueue *dqueue = dqueueCreate(queue_size, policy, thread_count);
    threadArgs t_args[thread_count];
    for (int i = 0; i < thread_count; ++i)
    {
        t_args[i].id = i;
        t_args[i].request = dqueue;
    }
    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; ++i)
    {
        pthread_create(&threads[i], NULL, thread_func, (void *)(&t_args[i]));
    }

    listenfd = Open_listenfd(port);
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);

        RequestStruct *request = malloc(sizeof(RequestStruct));
        request->connfd = connfd;
        gettimeofday(&request->arrival_time, NULL);
        addToWaitingQueue(dqueue, request);
    }
    dqueueDestroy(dqueue);
    return 0;
}