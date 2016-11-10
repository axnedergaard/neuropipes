#include <stdio.h>
#include "pipeline.h"

static int next = 0;
void insert_next(pipeline *pl, char *pipe)  {
  pipeline_insert(pl, pipe);
  if (next > 0)  {
    pipeline_insert_edge(pl, next-1, next);
  }
  next++;
}

int main()  {
  pipeline* pl = pipeline_create();
  
  insert_next(pl, "DUMMYEMOTIV");
  insert_next(pl, "FOURIERTRANSFORM");
  insert_next(pl, "INVERSEFOURIERTRANSFORM");
  insert_next(pl, "FILEOUT"); 

  pipeline_set_loop(pl, 0);

  if (pipeline_init(pl)) printf("[!]init\n");
  if (pipeline_run(pl)) printf("[!]run\n");
  pipeline_destroy(pl);
  return 0;
}
