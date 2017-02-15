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

void insert_next_concurrent(pipeline *pl, char *pipe)  {
  pipeline_insert(pl, pipe, 1);
  if (next > 0)  {
    pipeline_insert_edge(pl, next-1, next);
  }
  next++;
}

int main(int argc, char **argv)  {
  pipeline* pl = pipeline_create();

  if (argc > 2)  {
    pipeline_set_interval(pl, atof(argv[2]));
  }
  if (argc > 1)  { 
    pipeline_set_loop(pl, atoi(argv[1]));
  }

  //construct pipeline 
  int emotiv = pipeline_insert(pl, "DUMMYEMOTIV;random=1", 1);
  int filt1 = pipeline_insert(pl, "FILTER;order=4,pass=band,lc=8,hc=12", 0);
  int writemem1 = pipeline_insert(pl, "WRITESHAREDMEM;key=42", 0);
  int writemem2 = pipeline_insert(pl, "WRITESHAREDMEM;key=43", 0);
  int print = pipeline_insert(pl, "PRINT", 0); 
  pipeline_insert_edge(pl, emotiv, filt1);
  pipeline_insert_edge(pl, emotiv, print);
  pipeline_insert_edge(pl, emotiv, writemem1);
  pipeline_insert_edge(pl, filt1, writemem2);
 

  //init and run
  if (pipeline_init(pl)) printf("[!]init\n");
  if (pipeline_run(pl)) printf("[!]run\n");
  pipeline_destroy(pl);
 
  return 0;
}
