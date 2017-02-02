#include "../pipe.h"
#include "../data.h"

#include <stdio.h>

int print_init(pipe_ *p, linkedlist *l)  {
  if (linkedlist_size(l) <= 0)  {
    fprintf(stderr, "init_print: print pipe must have 1 or more inputs\n");
  }
  linkedlist_reset_iterater(l);
  p->output = NULL;  //print pipe has no output
  return 1;
}

int print_run(pipe_ *p, linkedlist *l)  {
  data** input_pointer = NULL;

  while ((input_pointer = (data**)linkedlist_iterate(l)) != NULL)  {
    data* input = *input_pointer;
    data_write(input, stdout);
  }
  linkedlist_reset_iterater(l);

  return 1; 
}

int print_kill(pipe_* p, linkedlist* l)  {
  return 1;
}

