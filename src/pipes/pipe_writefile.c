#include "../pipe.h"
#include "../data.h"
#include "../edflib.h"

#include <stdio.h>
#include <stdlib.h>

struct writefile_aux {
  int handle;
};

int writefile_init(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == NULL)  {  
    fprintf(stderr, "writefile_init: writefile pipe must have exactly 1 input\n");
  }
  p->output = NULL;  //writefile pipe has no output
  struct writefile_aux *aux = (struct writefile_aux*)malloc(sizeof(struct writefile_aux)); 

  char *fn = "recording.edf";
  //get file name from param

  //open edf file for writing
  int c = input->shape[0];
  int n = input->shape[1];
  int handle = edfopen_file_writeonly(fn, EDFLIB_FILETYPE_EDFPLUS, c);
  for (int i = 0; i < c; i++)  {  //TODO params
   if (edf_set_samplefrequency(handle, i, n) != 0)  printf("failed to set sample frequency\n");
    if (edf_set_physical_maximum(handle, i, 10000) != 0)  printf("failed to set max phy\n");
    if (edf_set_physical_minimum(handle, i, 0) != 0)  printf("failed to set min phy\n");
    if (edf_set_digital_maximum(handle, i, 32767) != 0)  printf("failed to max dig\n");
    if (edf_set_digital_minimum(handle, i, 0) != 0)  printf("failed to set min dig\n");
  }

  aux->handle = handle;
  p->auxiliary = aux;
  return 1;
}

int writefile_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);

//  data_edf_write(input, ((struct writefile_aux*)p->auxiliary)->handle);  
  //data_write(input, f);
  int c = input->shape[0];
  int n = input->shape[1];

  read_lock(input);

  for (int i = 0; i < c; i++)  {
    if (edfwrite_physical_samples(((struct writefile_aux*)p->auxiliary)->handle, (input->buffer + i*n)) != 0)  {  //stride?
      fprintf(stderr, "writefile_run: failed to write to file\n");
    }
  }

  read_unlock(input);
  
  return 1; 
}

int writefile_kill(pipe_* p, linkedlist* l)  {
  return (edfclose_file(((struct writefile_aux*)p->auxiliary)->handle) == 0);
}
