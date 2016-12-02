#include "pipe.h"
#include <stdlib.h>
#include <stdio.h>
#include "pipeline.h"  //?

pipe_* pipe_create()  {
  pipe_ *p = (pipe_*)malloc(sizeof(pipe_));
  if (p != NULL)  {
    p->debug = debug_pipe_create();
    if (p->debug == NULL)  {
      free(p);
      fprintf(stderr, "pipe_create: failed to create debug\n");
      return NULL;
    }
    p->run = NULL;
    p->init = NULL;
    p->output = NULL;
    p->auxiliary = NULL;
    p->buffer = NULL;
    p->status = -1;  //not init
    p->concurrent = 0;
    p->concurrent_pipe = NULL;
  }
  return p;
}

int pipe_destroy(pipe_ *p)  {
  if (p != NULL)  {
    debug_pipe_destroy(p->debug);
    if (p->output != NULL)  data_destroy(p->output);
    if (p->auxiliary != NULL)  free(p->auxiliary);
    if (p->concurrent_pipe != NULL)  concurrent_pipe_destroy(p->concurrent_pipe);
    free(p);
    return 1;
  }
  return 0;
}

void pipe_set_id(pipe_* p, int id)  {
  p->id = id;
}

int pipe_get_id(pipe_* p)  {
  return p->id;
}

void pipe_set_concurrent(pipe_* p, int concurrent)  {
  p->concurrent = concurrent;
}

int pipe_get_concurrent(pipe_* p)  {
  return p->concurrent;
}

int pipe_init(pipe_* p, linkedlist* l)  {  //linkedlist with input pipes
  if (p != NULL)  {
    if (p->init != NULL)  {
        if (p->init(p, l) == 1)  {
          if (p->concurrent == 1)  {  //concurrent pipe
            p->concurrent_pipe = concurrent_pipe_create();
            if (p->concurrent_pipe == NULL)  {
              fprintf(stderr, "pipe_init: failed to create concurrent pipe\n");
            }
          }
          p->status = 0;  //inited
          return 1;
        }
        else  {
          fprintf(stderr, "pipe_init: pipe %p failed to init\n", p);
        }
    }
    else  {
      fprintf(stdout, "pipe_init: pipe has no init call\n"); 
      return -1;
    }
  } 
  return 0; 
}

int pipe_run(pipe_* p, linkedlist* l)  {  //linkedlist with input
  if (p != NULL)  {
    if (p->run != NULL)  {
      if (p->concurrent == 1)  {  //concurrent pipe
        concurrent_pipe* pp = p->concurrent_pipe;
        if (concurrent_pipe_started(pp) == 0)  {  //thread not started
          concurrent_pipe_start(pp, (void*)p);  //start thread
        }
        while (concurrent_pipe_get_buffer_ready(pp) == 0)  {  //wait for buffer to be ready
          pthread_cond_wait(concurrent_pipe_cond(pp), concurrent_pipe_mutex(pp)); //wait
        }
        p->status = 1; //ran
        return 1;
      }
      else {
        if (p->run(p, l) == 1)  {  //non-concurrent pipe, ran succesfully
          p->status = 1;  //ran
          debug_pipe_increment_times_run(p->debug);  //DEBUG
          return 1;
        }
        else  {
          fprintf(stderr, "pipe_run: pipe %p failed to run\n", p);
        }
      }
    }
    else  {
      printf("pipe has no call\n");
      return -1;
    }
  }
  return 0;
}

