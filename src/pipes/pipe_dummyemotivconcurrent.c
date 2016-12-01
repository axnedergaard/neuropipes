#include "../pipe.h"
#include "../concurrent_pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int dummyemotivconcurrent_init(pipe_* p, linkedlist* l)  {
  if (l != NULL)  {
    fprintf(stderr, "dummpemoinpcon_init: concurrent pipe cannot have any inputs\n");
    return -1;
  }
  
  p->output = data_create_from_string("EMOTIV");

  p->auxiliary = (double*)malloc(data_size(p->output));

  return 1;
}

int dummyemotivconcurrent_run(pipe_* p, linkedlist* l)  {
  srand(time(NULL));
  double *buffer = (double*)p->auxiliary;
  for (int i = 0; i < p->output->shape[0]; i++)  {
    for (int j = 0; j < p->output->shape[1]; j++)  {
      buffer[i*p->output->shape[1] + j] = i*p->output->shape[1] + j;
    }
  }
  //mutex lock TODO (if concurrent)
  data_copy_to_data(p->output, buffer);
  //mutex unlock
  return 1; 
}

