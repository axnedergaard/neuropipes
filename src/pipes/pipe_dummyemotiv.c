#include "../pipe.h"
#include "../parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//TODO random parameter

struct dummyemotiv_aux {
  double *buffer;
  int random;
};

int dummyemotiv_init(pipe_* p, linkedlist* l)  {
  p->output = data_create_from_string("EMOTIV");

  struct dummyemotiv_aux *aux = (struct dummyemotiv_aux*)malloc(sizeof(struct dummyemotiv_aux));
  
  aux->buffer = (double*)malloc(data_size(p->output));
  aux->random = 1;

  char *random_param = get_parameter(p, "random");
  if (random_param != NULL)  {
    if (strcmp(random_param, "0") == 0)  {
      aux->random = 0;
    }
  }

  if (aux->random == 1)  {
    srand(time(NULL));
  }
  
  p->auxiliary = aux;
  
  return 1;
}

int dummyemotiv_run(pipe_* p, linkedlist* l)  {
  double *buffer = (double*)((struct dummyemotiv_aux*)p->auxiliary)->buffer;
  int random = (int)((struct dummyemotiv_aux*)p->auxiliary)->random;
  for (int i = 0; i < p->output->shape[0]; i++)  {
    for (int j = 0; j < p->output->shape[1]; j++)  { 
      if (random == 1)  {
        buffer[i*p->output->shape[1] + j] = (rand() % 10000) + 0;  //random
      }  
      else  {
        buffer[i*p->output->shape[1] + j] = i*p->output->shape[1] + j;
      }
    }
  }
  data_copy_to_data(p->output, buffer);
  return 1; 
}

int dummyemotiv_kill(pipe_* p, linkedlist* l)  {

  return 1;
}
