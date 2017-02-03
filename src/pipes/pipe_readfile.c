#include "../pipe.h"
#include "../data.h"

#include <stdio.h>
#include <stdlib.h>

struct readfile_aux {
  int handle; 
  int current_set;
  int set_n; //number of sets
};

int readfile_init(pipe_ *p, linkedlist *l)  {
  //read header to get dimensions
  int c;
  int n;
  int set_n;
  char *filename = "recording.edf";
  int handle = data_edf_open_read(filename, &c, &n, &set_n);

  int shape[2];
  int stride[2];
  shape[0] = c;
  shape[1] = n;
  stride[0] = 1;
  stride[1] = 1;
  p->output = data_create(2, shape, stride);

  struct readfile_aux *aux = (struct readfile_aux*)malloc(sizeof(struct readfile_aux));
  aux->current_set = 0;
  aux->set_n = set_n;
  aux->handle = handle;

  p->auxiliary = aux;

  return 1;
}

int readfile_run(pipe_ *p, linkedlist *l)  {
  struct readfile_aux *aux = (struct readfile_aux*)p->auxiliary;

  //read
  data_edf_read(p->output, aux->handle); 
 
  aux->current_set++;

  if (aux->current_set >= aux->set_n)  {  //all signals read
    return 0;
  }
  return 1; 
}

int readfile_kill(pipe_* p, linkedlist* l)  {
  struct readfile_aux *aux = (struct readfile_aux*)p->auxiliary;
  data_edf_close(aux->handle);
  return 1;
}
