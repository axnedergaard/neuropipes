#include "../pipe.h"
#include "../data.h"

#include <stdio.h>
#include <stdlib.h>

int writefile_init(pipe_ *p, linkedlist *l)  {
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "writefile_init: writefile pipe must have exactly 1 input\n");
  }
  p->output = NULL;  //writefile pipe has no output
  return 1;
}

int writefile_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_iterate(l);
  linkedlist_reset_iterater(l);

  char *filename = "recording.edf";
  data_write_edf(input, filename);  
  //data_write(input, f);
  
  return 1; 
}
