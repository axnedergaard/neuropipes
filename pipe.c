#include "pipe.h"
#include <stdlib.h>
#include <stdio.h>
#include "pipeline.h"  //?

pipe_* pipe_create()  {
  pipe_ *p = (pipe_*)malloc(sizeof(pipe_));
  if (p != NULL)  {
    p->run = NULL;
    p->init = NULL;
    p->output = NULL;
    p->auxiliary = NULL;
    p->buffer = NULL;
    p->status = -1;  //not init
  }
  return p;
}

int pipe_destroy(pipe_ *p)  {
  if (p != NULL)  {
    if (p->output != NULL)  data_destroy(p->output);
    if (p->auxiliary != NULL)  free(p->auxiliary);
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

int pipe_init(pipe_* p, linkedlist* l)  {  //linkedlist with input pipes
  if (p != NULL)  {
    if (p->init != NULL)  {
        if (p->init(p, l) == 1)  {
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
      if (p->run(p, l) == 1)  {  //success
        p->status = 1;  //ran
        return 1;
      }
      else  {
        fprintf(stderr, "pipe_run: pipe %p failed to run\n", p);
      }
    }
    else  {
      printf("pipe has no call\n");
      return -1;
    }
  }
  return 0;
}

