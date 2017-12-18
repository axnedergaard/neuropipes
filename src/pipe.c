#include "pipe.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pipebuilder.h"

struct pipe {
  int id;
  data *output;
  int status;  // <-1:error -1:not init 0:not complete >0:complete
  void *auxiliary; //auxiliary data structure
  int(*init)(pipe_*, linkedlist*);
  int(*run)(pipe_*, linkedlist*);
  int(*kill)(pipe_*, linkedlist*);
  int concurrent; 
  int params_n;
  char **params;
  concurrent_pipe *concurrent_pipe;
  //debug_pipe *debug_pipe; 
};

pipe_* pipe_create()  {
  pipe_ *p = (pipe_*)malloc(sizeof(pipe_));
  if (p != NULL)  {
    //p->debug_pipe = debug_pipe_create();
    //if (p->debug_pipe == NULL)  {
    //  free(p);
    //  fprintf(stderr, "pipe_create: failed to create debug\n");
    //  return NULL;
    //}
    p->run = NULL;
    p->init = NULL;
    p->kill = NULL;
    p->output = NULL;
    p->auxiliary = NULL;
    p->status = -1;  //not init
    p->concurrent = 0;
    p->concurrent_pipe = NULL;
    p->params_n = 0;
    p->params = NULL;
  }
  return p;
}

int pipe_destroy(pipe_ *p)  {
  if (p != NULL)  {
    //debug_pipe_destroy(p->debug_pipe);
    if (p->output != NULL)  data_destroy(p->output);
    if (p->auxiliary != NULL)  free(p->auxiliary);
    if (p->concurrent_pipe != NULL)  concurrent_pipe_destroy(p->concurrent_pipe);
    free(p);
    return 1;
  }
  return 0;
}

int pipe_init(pipe_* p, linkedlist* l)  {
  //printf("initialising pipe %p\n", p);
  if (p != NULL)  {
    if (p->init != NULL)  {
        if (p->init(p, l) == 1)  {  //init success
	  //if concurrent pipe, create concurrent pipe struct and make output data blocking
          if (p->concurrent == 1)  { 
            p->concurrent_pipe = concurrent_pipe_create();
            if (p->concurrent_pipe == NULL)  {
              fprintf(stderr, "pipe_init: failed to create concurrent pipe\n");
            }
            if (p->output != NULL)  {
              data_make_blocking(p->output);
            }
          }
	  //increment readers for blocking inputs
          data **data_ptr = NULL;
          if (l != NULL)  {
            while ((data_ptr = (data**)linkedlist_iterate(l)) != NULL)  {  
              data *d = *data_ptr;
              if (data_get_blocking(d))  {
                if (p->concurrent == 1)  {  //give warning if concurrent pipe has blocking input as this is not supported
                  printf("pipe_init: warning concurrent pipes reading blocking input is not supported\n");
                }
                data_increment_readers(d); 
              }
            }
          }
          p->status = 0;  //initialised
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

int pipe_run(pipe_* p, linkedlist* l)  {
  //printf("running pipe %p\n", p);
  int status = p->run(p, l);  //run
  if (status >= 0)  {  //run success
    p->status = 1; 
    return 1;
  }
  else  {  //run failure
    fprintf(stderr, "pipe_run: pipe %p failed to run\n", p);
    p->status = -2;
  }
  return 0;
}

int pipe_kill(pipe_ *p, linkedlist *l)  {
  //printf("killing pipe %p\n", p);
  if (p != NULL)  {
    if (p->kill != NULL)  {
      if (p->kill(p, l) != 1)  {
        fprintf(stderr, "pipe_kill: pipe %p failed to kill\n", p);
      }
    }
  }
  return 0;
}

void pipe_set_id(pipe_* p, int id)  {
  p->id = id;
}

int pipe_get_id(pipe_* p)  {
  return p->id;
}

void pipe_set_output(pipe_ *p, data *d)  {
  p->output = d;
}

data *pipe_get_output(pipe_ *p)  {
  return p->output;
}

data **pipe_get_output_pointer(pipe_ *p)  {
  return &p->output;
}

void pipe_set_status(pipe_* p, int status)  {
  p->status = status;
}

int pipe_get_status(pipe_* p)  {
  return p->status;
}

void pipe_set_auxiliary(pipe_* p, void *aux)  {
  p->auxiliary = aux;
}

void *pipe_get_auxiliary(pipe_* p)  {
  return p->auxiliary;
}

void pipe_set_init(pipe_* p, int(*init)(pipe_*, linkedlist*))  {
  p->init = init;
}
int (*pipe_get_init(pipe_* p))(pipe_*, linkedlist*)  {
  return p->init;
}

void pipe_set_run(pipe_* p, int(*run)(pipe_*, linkedlist*))  {
  p->run = run;
}

int (*pipe_get_run(pipe_* p))(pipe_*, linkedlist*)  {
  return p->run;
}

void pipe_set_kill(pipe_* p, int(*kill)(pipe_*, linkedlist*))  {
  p->kill = kill;
}

int (*pipe_get_kill(pipe_* p))(pipe_*, linkedlist*)  {
  return p->kill;
}

void pipe_set_concurrent(pipe_ *p, int concurrent)  {
  p->concurrent = concurrent;
}

void pipe_set_params_n(pipe_ *p, int params_n)  {
  p->params_n = params_n;
}

int pipe_get_params_n(pipe_ *p)  {
  return p->params_n;
}

void pipe_set_params(pipe_ *p, char** params)  {
  p->params = params;
}

char **pipe_get_params(pipe_ *p)  {
  return p->params;
}

int pipe_get_concurrent(pipe_* p)  {
  return p->concurrent;
}

concurrent_pipe *pipe_get_concurrent_pipe(pipe_* p)  {
  return p->concurrent_pipe;
}
/*
debug_pipe *pipe_get_debug_pipe(pipe_* p)  {
  return p->debug_pipe;
}*/


