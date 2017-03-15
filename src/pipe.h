#ifndef PIPE_H
#define PIPE_H

#include "data.h"
#include "linkedlist.h"
#include "debug_pipe.h"
#include "concurrent_pipe.h"

typedef struct pipe pipe_;  //poor solution to unistd.h name clash
struct pipe;

pipe_ *pipe_create();
int pipe_destroy(pipe_*);

int pipe_run(pipe_*, linkedlist*);  
int pipe_init(pipe_*, linkedlist*);  
int pipe_kill(pipe_*, linkedlist*);

void pipe_set_id(pipe_*, int);
int pipe_get_id(pipe_*);
void pipe_set_output(pipe_*, data*);
data *pipe_get_output(pipe_*);
data **pipe_get_output_pointer(pipe_*);  //required for pipeline in_data purposes
void pipe_set_status(pipe_*, int);
int pipe_get_status(pipe_*);
void pipe_set_auxiliary(pipe_*, void*);
void *pipe_get_auxiliary(pipe_*);
void pipe_set_init(pipe_*, int(*init)(pipe_*, linkedlist*));
int (*pipe_get_init(pipe_*))(pipe_*, linkedlist*);
void pipe_set_run(pipe_*, int(*run)(pipe_*, linkedlist*));
int (*pipe_get_run(pipe_*))(pipe_*, linkedlist*);
void pipe_set_kill(pipe_*, int(*kill)(pipe_*, linkedlist*));
int (*pipe_get_kill(pipe_*))(pipe_*, linkedlist*);
void pipe_set_params_n(pipe_*, int);
int pipe_get_params_n(pipe_*);
void pipe_set_params(pipe_*, char**);
char **pipe_get_params(pipe_*);
void pipe_set_concurrent(pipe_*, int);
int pipe_get_concurrent(pipe_*);
void pipe_set_concurrent_pipe(pipe_*, concurrent_pipe*);
concurrent_pipe *pipe_get_concurrent_pipe(pipe_*);
void pipe_set_debug_pipe(pipe_*, debug_pipe*);
debug_pipe *pipe_get_debug_pipe(pipe_*);

#endif
