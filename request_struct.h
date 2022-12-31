#ifndef REQUEST_STRUCT_H
#define REQUEST_STRUCT_H
#include <sys/time.h>

typedef struct
{
    int connfd;
    struct timeval arrival_time;
} RequestStruct;

#endif // REQUEST_STRUCT_H