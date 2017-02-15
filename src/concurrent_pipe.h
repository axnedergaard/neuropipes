#ifndef CONCURRENT_PIPE_H
#define CONCURRENT_PIPE_H

#include <pthread.h>
#include "linkedlist.h"

typedef struct concurrent_pipe concurrent_pipe;
struct concurrent_pipe;

concurrent_pipe *concurrent_pipe_create();
int concurrent_pipe_destroy(concurrent_pipe*);
int concurrent_pipe_start(concurrent_pipe*, void*, linkedlist*, int*);
int concurrent_pipe_stop(concurrent_pipe*);
int concurrent_pipe_done(concurrent_pipe*);
int concurrent_pipe_started(concurrent_pipe*);
int concurrent_pipe_set_buffer_ready(concurrent_pipe*, int);
int concurrent_pipe_get_buffer_ready(concurrent_pipe*);
pthread_cond_t *concurrent_pipe_cond(concurrent_pipe*);
pthread_mutex_t *concurrent_pipe_mutex(concurrent_pipe*);
pthread_t *concurrent_pipe_thread(concurrent_pipe*);

#endif
