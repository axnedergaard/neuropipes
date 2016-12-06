#include <stdio.h>
#include <stdlib.h>
#include "pipeline.h"
#include <sys/resource.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <unistd.h>

static int next = 0;
void insert_next(pipeline *pl, char *pipe)  {
  pipeline_insert(pl, pipe, 0);
  if (next > 0)  {
    pipeline_insert_edge(pl, next-1, next);
  }
  next++;
}

int main(int argc, char **argv)  {
  pipeline* pl = pipeline_create();

  if (argc > 1)  {
    pipeline_set_loop(pl, atoi(argv[1]));
  }
  
  pipeline_insert(pl, "DUMMYEMOTIVCON", 1);
  next++;
 // insert_next(pl, "EMOTIV");
  insert_next(pl, "FOURIERTRANSFORM");
  insert_next(pl, "INVERSEFOURIERTRANSFORM");
  insert_next(pl, "PRINT"); 

  if (pipeline_init(pl)) printf("[!]init\n");
  if (pipeline_run(pl)) printf("[!]run\n");
  pipeline_destroy(pl);
  return 0;
}
