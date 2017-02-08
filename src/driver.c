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

  

if (argc > 1)  {
    pipeline_set_loop(pl, atoi(argv[1]));
  }

  //construct pipeline  
  int dummy = pipeline_insert(pl, "DUMMYEMOTIV", 0);
  int writemem1 = pipeline_insert(pl, "WRITESHAREDMEM;KEY:42", 0);
  int fft = pipeline_insert(pl, "FOURIERTRANSFORM", 0);
  int bp1 = pipeline_insert(pl, "ALPHABANDPASS", 0);
  int bp2 = pipeline_insert(pl, "ALPHABANDPASS", 0);
  int ift = pipeline_insert(pl, "INVERSEFOURIERTRANSFORM", 0);
  int writemem2 = pipeline_insert(pl, "WRITESHAREDMEM;KEY:43", 0);
  int writemem3 = pipeline_insert(pl, "WRITESHAREDMEM;KEY:44", 0);
  int print = pipeline_insert(pl, "PRINT", 0);
  pipeline_insert_edge(pl, dummy, writemem1);
  pipeline_insert_edge(pl, dummy, fft);
  pipeline_insert_edge(pl, fft, bp1);
  pipeline_insert_edge(pl, bp1, ift);
  pipeline_insert_edge(pl, ift, writemem2);
  pipeline_insert_edge(pl, dummy, bp2);
  pipeline_insert_edge(pl, bp2, writemem3);
  pipeline_insert_edge(pl, bp2, print);

  //init and run
  if (pipeline_init(pl)) printf("[!]init\n");
  if (pipeline_run(pl)) printf("[!]run\n");
  pipeline_destroy(pl);
 
  return 0;
}
