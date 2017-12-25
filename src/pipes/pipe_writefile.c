#include "../pipe.h"
#include "../data.h"
#include "../edflib.h"
#include "../parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct writefile_aux {
  int handle;
};

int writefile_init(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == NULL)  {  
    fprintf(stderr, "writefile_init: writefile pipe must have exactly 1 input\n");
  }
  struct writefile_aux *aux = (struct writefile_aux*)malloc(sizeof(struct writefile_aux)); 

  char fn[PARAM_MAX]; //file name
  strncpy(fn, "save.edf", PARAM_MAX); 
  set_parameter_string(p, "fn", fn);
  
  //int sf = 128; //sample frequency
  //param = get_parameter(p, "sf");
  //if (param != NULL)  {
  //  sf = atoi(param);
  //  free(param);
  //}

  //open edf file for writing
  int *shape = data_get_shape(input);
  int c = shape[0];
  int sf = shape[1];
  int handle = edfopen_file_writeonly(fn, EDFLIB_FILETYPE_EDFPLUS, c);
  if (handle < 0)  {
    printf("writefile_init: error opening file\n");
  }
  
  for (int i = 0; i < c; i++)  {  //TODO params
    if (edf_set_samplefrequency(handle, i, sf) != 0)  printf("failed to set sample frequency\n"); //note: frames per sample, NOT sample frequency!
    if (edf_set_digital_maximum(handle, i, 32767) != 0)  printf("failed to max dig\n");
    if (edf_set_digital_minimum(handle, i, -32767) != 0)  printf("failed to set min dig\n");
    if (edf_set_physical_maximum(handle, i, 32767) != 0)  printf("failed to set max phy\n");
    if (edf_set_physical_minimum(handle, i, -32767) != 0)  printf("failed to set min phy\n");
  }

  aux->handle = handle;

  pipe_set_auxiliary(p, aux);
  return 1;
}

int writefile_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);

  int *shape = data_get_shape(input);
  int c = shape[0];
  int n = shape[1];

  read_lock(input);

  for (int i = 0; i < c; i++)  {
    if (edfwrite_physical_samples(((struct writefile_aux*)pipe_get_auxiliary(p))->handle, (data_get_buffer(input) + i*n)) != 0)  {  //stride?
      fprintf(stderr, "writefile_run: failed to write to file\n");
    }
  }

  read_unlock(input);
  
  return 1; 
}

int writefile_kill(pipe_* p, linkedlist* l)  {
  return (edfclose_file(((struct writefile_aux*)pipe_get_auxiliary(p))->handle) == 0);
}
