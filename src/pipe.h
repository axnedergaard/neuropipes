#ifndef PIPE_H
#define PIPE_H

#include "data.h"
#include "linkedlist.h"
#include "debug_pipe.h"

typedef struct pipe pipe_;  //poor solution to unistd.h name clash
struct pipe {
  int id;
  data* output;
  int status;  // <-1:error -1:not init 0:not complete >0:complete
  void* auxiliary; //auxiliary data structure, e.g. for emokit 
  void* buffer;  //buffers used during runtime
  int(*init)(pipe_*, linkedlist*);
  int(*run)(pipe_*, linkedlist*); 
  //destructors?? TODO
  debug_pipe *debug;  //TODO everything in class relating to this
};

pipe_ *pipe_create();
int pipe_destroy(pipe_*);
void pipe_set_id(pipe_*, int);
int pipe_get_id(pipe_*);
int pipe_run(pipe_*, linkedlist*);  
int pipe_init(pipe_*, linkedlist*);  

#endif
