#include "concurrent_pipe.h"
#include "pipe.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//ASSUME PARALLEL PIPE HAS NO INPUTS

struct concurrent_pipe {
  pthread_t thread;
  int started;
  int buffer_ready;
};

void *concurrent_pipe_run(void *pipe)  {
  pipe_ *p = (pipe_*)pipe;
  while (1)  {
    p->run(p, NULL); //assume concurrent pipe has no inputs
    concurrent_pipe_set_buffer_ready((concurrent_pipe*)((pipe_*)pipe)->concurrent_pipe, 1);
    debug_pipe_increment_times_run(p->debug);  //DEBUG
  }
  return NULL;
}

concurrent_pipe *concurrent_pipe_create()  {
  concurrent_pipe *pp = (concurrent_pipe*)malloc(sizeof(concurrent_pipe));
  if (pp == NULL)  {
    fprintf(stderr, "concurrent_pipe_create: failed to alloc mem for concurrent pipe\n");
    return NULL;
  }
  pp->started = 0;
  pp->buffer_ready = 0;

  return pp;  
}

int concurrent_pipe_destroy(concurrent_pipe* pp)  {
//  pthread_destroy(pp->thread); TODO ???
  free(pp); 
  return 1;
}

int concurrent_pipe_start(concurrent_pipe* pp, void* pipe)  {
  if (pthread_create(&pp->thread, NULL, &concurrent_pipe_run, pipe) != 0)  {
    fprintf(stderr, "concurrent_pipe_create: failed to create pthread\n");
    free(pp);
    return 0;
  }
  pp->started = 1;
  return 1;
}

int concurrent_pipe_started(concurrent_pipe* pp)  {
  return pp->started;
}

int concurrent_pipe_set_buffer_ready(concurrent_pipe* pp, int ready)  {
  return pp->buffer_ready = ready;
}

int concurrent_pipe_get_buffer_ready(concurrent_pipe* pp)  {
  return pp->buffer_ready;
}
