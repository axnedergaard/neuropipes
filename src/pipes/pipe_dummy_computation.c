#include "../pipe.h"
#include "../parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct dummy_computation_aux {
  int replace;
  double replace_value;
  double *buffer;
};

//dummy_computation pipe used for testing
int dummy_computation_init(pipe_* p, linkedlist* l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == 0)  {
    return -1;
  }
  
  p->output = data_create_from(input);

  struct dummy_computation_aux *aux = (struct dummy_computation_aux*)malloc(sizeof(struct dummy_computation_aux)); 

  aux->replace = 0;
  aux->replace_value = 0;
  char *replace_param = get_parameter(p, "replace");
  if (replace_param != NULL)  {
    if (strcmp(replace_param, "1") == 0)  {
      aux->replace = 1;
    }
  }
  char *replace_value_param = get_parameter(p, "replace_value");
  if (replace_value_param != NULL)  {
    aux->replace_value = atof(replace_value_param);
  }
  aux->buffer = (double*)malloc(data_size(input));

  p->auxiliary = aux;
  
  return 1;
}

int dummy_computation_run(pipe_* p, linkedlist* l)  {  
  data *input = *(data**)linkedlist_head(l);

  struct dummy_computation_aux *aux = (struct dummy_computation_aux*)p->auxiliary;

  double *buffer = aux->buffer;

  data_copy_from_data(input, buffer);

  //replace values in buffer with replace_value if replace is true
  if (aux->replace == 1)  {
    for (int i = 0; i < p->output->shape[0]; i++)  {
      for (int j = 0; j < p->output->shape[1]; j++)  { 
        buffer[i*p->output->shape[1] + j] = aux->replace_value;
      }
    }
  }

  data_copy_to_data(p->output, buffer);

  return 1; 
}

int dummy_computation_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
