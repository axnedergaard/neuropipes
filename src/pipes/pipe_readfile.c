#include "../pipe.h"
#include "../data.h"

#include <stdio.h>
#include <stdlib.h>

struct readfile_aux {
  int current_set;
  int set_n;
  int c; //number of channels
  double *data;  //all recordings in file
};

int readfile_init(pipe_ *p, linkedlist *l)  {
  if (linkedlist_size(l) != 0)  {
    fprintf(stderr, "readfile_init: readfile pipe must have exactly 0 inputs\n");
  }
 
  //read header to get dimensions
  int c;
  int n;
  int sets;
  char *filename = "recording.edf";
  p->output = data_create_from_edf(filename, &c, &n, &sets);

  struct readfile_aux *aux = (struct readfile_aux*)malloc(sizeof(struct readfile_aux));
  aux->c = c;
  aux->current_set = 0;
  aux->set_n = sets;
  aux->data = (double*)malloc(sizeof(double)*aux->set_n*c*n);

  //read data
  for (int i = 0; i < aux->set_n; i++)  {
    
  }

  p->auxiliary = aux;

  return 1;
}

int readfile_run(pipe_ *p, linkedlist *l)  {
  struct readfile_aux *aux = (struct readfile_aux*)p->auxiliary;

  if (aux->current_set > aux->set_n)  {  //all signals read
    return 0;
  }

  //read... 
 
  return 1; 
}

int readfile_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
