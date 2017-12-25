#include "../pipe.h"
#include "../parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//dummy_computation pipe used for testing

struct dummy_computation_aux {
  int replace;
  double replace_value;
  double *buffer;
};

int dummy_computation_init(pipe_* p, linkedlist* l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == 0)  {
    return -1;
  } 
  struct dummy_computation_aux *aux = (struct dummy_computation_aux*)malloc(sizeof(struct dummy_computation_aux)); 
  pipe_set_auxiliary(p, aux);
  pipe_set_output(p, data_create_from(input));

  aux->replace = 0;
  aux->replace_value = 0;
  aux->buffer = (double*)malloc(data_size(input));
  set_parameter_int(p, "replace", &aux->replace);
  set_parameter_double(p, "replace_value", &aux->replace_value); 
  
  return 1;
}

int dummy_computation_run(pipe_* p, linkedlist* l)  {  
  data *input = *(data**)linkedlist_head(l);
  data *output = pipe_get_output(p);
  struct dummy_computation_aux *aux = (struct dummy_computation_aux*)pipe_get_auxiliary(p);

  double *buffer = aux->buffer;
  int *shape, *stride;
  data_spec(output, &shape, &stride);

  data_copy_from_data(input, buffer);

  //replace values in buffer with replace_value if replace is true
  if (aux->replace == 1)  {
    for (int i = 0; i < shape[0]; i++)  {
      for (int j = 0; j < shape[1]; j++)  { 
        buffer[i*shape[1] + j] = aux->replace_value;
      }
    }
  }

  data_copy_to_data(output, buffer);

  return 1; 
}

int dummy_computation_kill(pipe_* p, linkedlist* l)  {
  struct dummy_computation_aux *aux = (struct dummy_computation_aux*)pipe_get_auxiliary(p);
  free(aux->buffer);
  return 1;
}
