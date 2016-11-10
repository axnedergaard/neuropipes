#include "../pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int dummyemotiv_init(pipe_* p, linkedlist* l)  {
  if (linkedlist_size(l) > 0)  {
    fprintf(stderr, "dummpemoinp_init: pipe_ must have 0 inputs\n");
    return -1;
  }

  p->output = data_create_from_string("EMOTIV");

  return 1;
}

int dummyemotiv_run(pipe_* p, linkedlist* l)  {
  srand(time(NULL));
  for (int i = 0; i < p->output->shape[0]; i++)  {
    for (int j = 0; j < p->output->shape[1]; j++)  {
      p->output->buffer[i*p->output->shape[1] + j] = i*p->output->shape[1] + j;
   //   p->output->buffer[i*p->output->shape[1] + j] = rand() % 10000 / 10000.0;  //random
    }
  }
  return 1; 
}

