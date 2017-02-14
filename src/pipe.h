#ifndef PIPE_H
#define PIPE_H

#include "data.h"
#include "linkedlist.h"
#include "debug_pipe.h"
#include "concurrent_pipe.h"

typedef struct pipe pipe_;  //poor solution to unistd.h name clash
struct pipe {
  int id;
  data* output;
  int status;  // <-1:error -1:not init 0:not complete >0:complete
  void* auxiliary; //auxiliary data structure, e.g. for emokit 
  int(*init)(pipe_*, linkedlist*);
  int(*run)(pipe_*, linkedlist*);
  int(*kill)(pipe_*, linkedlist*);
  int concurrent; 
  int params_n;
  char **params;
  concurrent_pipe *concurrent_pipe;
  debug_pipe *debug; 
};

pipe_ *pipe_create();
int pipe_destroy(pipe_*);
void pipe_set_id(pipe_*, int);
int pipe_get_id(pipe_*);
int pipe_run(pipe_*, linkedlist*);  
int pipe_init(pipe_*, linkedlist*);  
int pipe_kill(pipe_*, linkedlist*);
void pipe_set_concurrent(pipe_*, int);
int pipe_get_concurrent(pipe_*);

#endif
