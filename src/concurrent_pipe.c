#include "concurrent_pipe.h"
#include "pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

//ASSUME PARALLEL PIPE HAS NO INPUTS

struct concurrent_pipe {
  pthread_t thread;
  int started;
  int done;
  //int buffer_ready;
  //mutex and cond for buffer_ready
//  pthread_mutex_t mutex;
  //pthread_cond_t cond;
};

void *concurrent_pipe_run(void *pipe)  {
  pipe_ *p = (pipe_*)pipe;
  concurrent_pipe *pp = p->concurrent_pipe;
  while (pp->started == 1) {
  //assume concurrent pipe has no inputs
    if (p->run(p, NULL) == 0)  {  //pipe finished
      pp->done = 1;
      concurrent_pipe_stop(pp);
    //  data_unblock(p->output);
    }
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
  //pp->buffer_ready = 0;
  //pthread_mutex_init(&pp->mutex, NULL);
  //pthread_cond_init(&pp->cond, NULL);

  return pp;  
}

int concurrent_pipe_destroy(concurrent_pipe *pp)  {
  free(pp);  //??
  return 1;
}

int concurrent_pipe_done(concurrent_pipe *pp)  {
  return pp->done;
}

int concurrent_pipe_stop(concurrent_pipe *pp)  {
  pp->started = 0;
  return 1;
}

int concurrent_pipe_start(concurrent_pipe *pp, void* pipe)  {
  pp->started = 1;
  if (pthread_create(&pp->thread, NULL, &concurrent_pipe_run, pipe) != 0)  {
    fprintf(stderr, "concurrent_pipe_create: failed to create pthread\n");
    free(pp);
    return 0;
  }

/* 
  //set policy of thread (decrease) 
  int policy;
  struct sched_param param;
  pthread_getschedparam(pp->thread, &policy, &param);
  param.sched_priority = 10;
  pthread_setschedparam(pp->thread, policy, &param);
  printf("policy = %d sched prio = %d\n", policy, param.sched_priority);
*/
  return 1;
}
/*
pthread_cond_t *concurrent_pipe_cond(concurrent_pipe *pp)  {
  return &pp->cond;
}

pthread_mutex_t *concurrent_pipe_mutex(concurrent_pipe *pp)  {
  return &pp->mutex;
}*/

pthread_t *concurrent_pipe_thread(concurrent_pipe *pp)  {
  return &pp->thread;
}

int concurrent_pipe_started(concurrent_pipe *pp)  {
  return pp->started;
}
/*
int concurrent_pipe_set_buffer_ready(concurrent_pipe *pp, int ready)  {
  return pp->buffer_ready = ready;
}

int concurrent_pipe_get_buffer_ready(concurrent_pipe *pp)  {
  return pp->buffer_ready;
}*/
