#include "../pipe.h"
#include "../data.h"

#include <stdio.h>
#include <stdlib.h>

//TODO

int writesharedmem_init(pipe_ *p, linkedlist *l)  {
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "writesharedmem_init: writesharedmem pipe must have exactly 1 input\n");
  }
  p->output = NULL;  //writesharedmem pipe has no output
  return 1;
}

int writesharedmem_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_iterate(l);
  linkedlist_reset_iterater(l);

  //create shared memory and get file descriptor
  FILE *f = fopen(filestring, "w");

  data_write(input, f);

  fclose(f); 
  
  return 1; 
}

int writesharedmem_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
