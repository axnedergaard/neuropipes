#include "../pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int dummyemotiv_init(pipe_* p, linkedlist* l)  {
  p->output = data_create_from_string("EMOTIV");

  p->auxiliary = (double*)malloc(data_size(p->output));

  srand(time(NULL));
  
  return 1;
}

int dummyemotiv_run(pipe_* p, linkedlist* l)  {
  double *buffer = (double*)p->auxiliary;
  for (int i = 0; i < p->output->shape[0]; i++)  {
    for (int j = 0; j < p->output->shape[1]; j++)  { 
      buffer[i*p->output->shape[1] + j] = i*p->output->shape[1] + j;
  //    buffer[i*p->output->shape[1] + j] = rand() % 10000;  //random
    }
  }
  data_copy_to_data(p->output, buffer);
  return 1; 
}

int dummyemotiv_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
