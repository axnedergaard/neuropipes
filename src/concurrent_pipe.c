#include "concurrent_pipe.h"
#include "pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

struct concurrent_pipe {
  pthread_t thread;
  int running;
};

struct run_input  {
  pipe_ *p;
  linkedlist *input;
  int *quit;
};

void *concurrent_pipe_run(void *inp)  {
  struct run_input *rinp = (struct run_input*)inp;
  pipe_ *p = rinp->p;
  int *quit = rinp->quit;
  linkedlist *input = rinp->input;

  concurrent_pipe *pp = pipe_get_concurrent_pipe(p);
  while (pp->running == 1) {
    debug_pipe_start_timer(pipe_get_debug_pipe(p)); //DEBUG
    if (pipe_run(p, input) == 0)  {  //pipe finished
      *quit = 1;
      concurrent_pipe_stop(pp);
    }
    debug_pipe_stop_timer(pipe_get_debug_pipe(p)); //DEBUG
    double pipe_time = debug_pipe_time(pipe_get_debug_pipe(p));  //DEBUG
    printf("concurrent pipe %d ran in %fs\n", pipe_get_id(p), pipe_time);  //DEBUG
  }
  return NULL;
}

concurrent_pipe *concurrent_pipe_create()  {
  concurrent_pipe *pp = (concurrent_pipe*)malloc(sizeof(concurrent_pipe));
  if (pp == NULL)  {
    fprintf(stderr, "concurrent_pipe_create: failed to alloc mem for concurrent pipe\n");
    return NULL;
  }
  pp->running = 0;

  return pp;  
}

int concurrent_pipe_destroy(concurrent_pipe *pp)  {
  free(pp);  //??
  return 1;
}

int concurrent_pipe_start(concurrent_pipe *pp, void *p, linkedlist *input, int *quit)  {
  struct run_input *rinp = (struct run_input*)malloc(sizeof(struct run_input));  
  rinp->p = (pipe_*)p;
  rinp->input = input;
  rinp->quit = quit;

  pp->running = 1;
  if (pthread_create(&pp->thread, NULL, &concurrent_pipe_run, rinp) != 0)  {
    fprintf(stderr, "concurrent_pipe_create: failed to create pthread\n");
    free(pp);
    return 0;
  }

/*//set policy of thread (decrease) 
  int policy;
  struct sched_param param;
  pthread_getschedparam(pp->thread, &policy, &param);
  param.sched_priority = 10;
  pthread_setschedparam(pp->thread, policy, &param);
  printf("policy = %d sched prio = %d\n", policy, param.sched_priority);
*/

  return 1;
}

int concurrent_pipe_stop(concurrent_pipe *pp)  {
  pp->running = 0;
  return 1;
}

pthread_t *concurrent_pipe_thread(concurrent_pipe *pp)  {
  return &pp->thread;
}

int concurrent_pipe_running(concurrent_pipe *pp)  {
  return pp->running;
}
