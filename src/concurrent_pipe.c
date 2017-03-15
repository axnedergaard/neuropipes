#include "concurrent_pipe.h"
#include "pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

struct concurrent_pipe {
  pthread_t thread;
  int running;
};

struct run_input  {  //input for thread run function
  pipe_ *p;
  linkedlist *in_data;
  int *quit;
};

void *concurrent_pipe_run(void *input)  {  //thread run function
  struct run_input *run_input = (struct run_input*)input;
  pipe_ *p = run_input->p;
  int *quit = run_input->quit;
  linkedlist *in_data = run_input->in_data;

  //run thread
  concurrent_pipe *pp = pipe_get_concurrent_pipe(p);
  while (pp->running == 1) {  
    debug_pipe_start_timer(pipe_get_debug_pipe(p)); //DEBUG
    //run pipe
    if (pipe_run(p, in_data) == 0)  {  //pipe signalled to terminate
      *quit = 1; //signal pipeline to terminate
      concurrent_pipe_stop(pp);
    }
    debug_pipe_stop_timer(pipe_get_debug_pipe(p)); //DEBUG
    double pipe_time = debug_pipe_time(pipe_get_debug_pipe(p));  //DEBUG
    printf("concurrent pipe %d ran in %fs\n", pipe_get_id(p), pipe_time);  //DEBUG
  }

  free(run_input);
  
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

int concurrent_pipe_start(concurrent_pipe *pp, void *p, linkedlist *in_data, int *quit)  {
  struct run_input *run_input = (struct run_input*)malloc(sizeof(struct run_input));  
  run_input->p = (pipe_*)p;
  run_input->in_data = in_data;
  run_input->quit = quit;

  //create thread
  pp->running = 1;
  if (pthread_create(&pp->thread, NULL, &concurrent_pipe_run, run_input) != 0)  {
    fprintf(stderr, "concurrent_pipe_create: failed to create pthread\n");
    free(pp);
    return 0;
  }

  return 1;
}

int concurrent_pipe_stop(concurrent_pipe *pp)  {
  pp->running = 0;
  return 1;
}

pthread_t *concurrent_pipe_thread(concurrent_pipe *pp)  {
  return &pp->thread;
}
