#include "../pipe.h"
#include "../data.h"

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
  aux->handle = data_edf_open(input, fn); 
  p->auxiliary = aux;
  return 1;
}

int writefile_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);

  data_edf_write(input, ((struct writefile_aux*)p->auxiliary)->handle);  
  //data_write(input, f);
  
  return 1; 
}

int writefile_kill(pipe_* p, linkedlist* l)  {
  data_edf_close(((struct writefile_aux*)p->auxiliary)->handle);
  return 1;
}
