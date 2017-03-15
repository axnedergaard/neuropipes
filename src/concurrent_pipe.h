#ifndef CONCURRENT_PIPE_H
#define CONCURRENT_PIPE_H

#include <pthread.h>
#include "linkedlist.h"

typedef struct concurrent_pipe concurrent_pipe;
struct concurrent_pipe;

concurrent_pipe *concurrent_pipe_create();
int concurrent_pipe_destroy(concurrent_pipe*);
int concurrent_pipe_start(concurrent_pipe*, void*, linkedlist*, int*);  //start thread
int concurrent_pipe_stop(concurrent_pipe*);  //stop thread
pthread_t *concurrent_pipe_thread(concurrent_pipe*);  //get thread

#endif
