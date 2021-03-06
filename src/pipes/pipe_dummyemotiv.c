#include "../pipe.h"
#include "../parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct dummyemotiv_aux {
  double *buffer;
  int random;
};

int dummyemotiv_init(pipe_* p, linkedlist* l)  {
  int channels = 14;
  int frames = 16; 
  set_parameter_int(p, "channels", &channels);
  set_parameter_int(p, "frames", &frames);
  int n = 2;
  int shape[n], stride[n];
  stride[0] = 1;
  stride[1] = 1;
  shape[0] = channels;
  shape[1] = frames;
  data *output = data_create(n, shape, stride);
  pipe_set_output(p, output);

  struct dummyemotiv_aux *aux = (struct dummyemotiv_aux*)malloc(sizeof(struct dummyemotiv_aux));
  aux->buffer = (double*)malloc(data_size(pipe_get_output(p)));
  aux->random = 1;
  set_parameter_int(p, "random", &aux->random);
  if (aux->random == 1)  {
    srand(time(NULL));
  }
  pipe_set_auxiliary(p, aux);
  
  return 1;
}

int dummyemotiv_run(pipe_* p, linkedlist* l)  {
  data *output = pipe_get_output(p);
  struct dummyemotiv_aux *aux = (struct dummyemotiv_aux*)pipe_get_auxiliary(p);
  
  double *buffer = aux->buffer;
  int random = aux->random;
  int *shape = data_get_shape(output);

  for (int i = 0; i < shape[0]; i++)  {
    for (int j = 0; j < shape[1]; j++)  { 
      if (random == 1)  {
        buffer[i*shape[1] + j] = (rand() % 10000) + 0;  //random
      }  
      else  {
        buffer[i*shape[1] + j] = i*shape[1] + j;
      }
    }
  }

  data_copy_to_data(output, buffer);

  return 1; 
}

int dummyemotiv_kill(pipe_* p, linkedlist* l)  {
  struct dummyemotiv_aux *aux = (struct dummyemotiv_aux*)pipe_get_auxiliary(p);
  free(aux->buffer);
  return 1;
}
