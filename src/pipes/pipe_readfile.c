#include "../pipe.h"
#include "../data.h"

#include <stdio.h>
#include <stdlib.h>

int readfile_init(pipe_ *p, linkedlist *l)  {
  if (linkedlist_size(l) != 0)  {
    fprintf(stderr, "readfile_init: readfile pipe must have exactly 0 inputs\n");
  }
  //extract data dimensions
  p->output = NULL;  //TODO
  return 1;
}

int readfile_run(pipe_ *p, linkedlist *l)  {
  char *filestring = "input.txt";
  FILE *f = fopen(filestring, "r");

  data_read(p->output, f);

  fclose(f); 
  
  return 1; 
}
