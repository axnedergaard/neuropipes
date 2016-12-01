#ifndef CONCURRENT_PIPE_H
#define CONCURRENT_PIPE_H

typedef struct concurrent_pipe concurrent_pipe;
struct concurrent_pipe;

concurrent_pipe *concurrent_pipe_create();
int concurrent_pipe_destroy(concurrent_pipe*);
int concurrent_pipe_start(concurrent_pipe*, void*);
int concurrent_pipe_started(concurrent_pipe*);
int concurrent_pipe_set_buffer_ready(concurrent_pipe*, int);
int concurrent_pipe_get_buffer_ready(concurrent_pipe*);

#endif
