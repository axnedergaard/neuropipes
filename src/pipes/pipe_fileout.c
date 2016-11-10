#include "../pipe.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LEN 64

//semi-toy pipe_ that prints all channels of EMOTIV-like data to files readable by a GNU-plot script

int fileout_init(pipe_ *p, linkedlist *l)  {
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "graph_init: graph pipe_ must have exactly 1 input\n");
  }
  p->output = NULL;  //print pipe_ has no output
  return 1;
}

int fileout_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_iterate(l);
  linkedlist_reset_iterater(l);
  char *filestring = "output.txt";
  FILE *f = fopen(filestring, "w");
  //fprintf(f, "{\n  \"data\": %f\n}\n", (float)input->buffer[0]);
  int len = 1;
  for (int i = 0; i < input->n; i++)  {
    len *= input->shape[i];
  }
  fwrite(input->buffer, len, 1, f);
/*
  fprintf(f, "{\n");
  for (int i = 0; i < c; i++)  {
    fprintf(f, "\"ch%d\": ", i);
    for (int j = 0; j < n; j++)  {
      float reading = input->buffer[i*n + j];  
      fprintf(f, "%f", reading);
      if (j + 1 != n)  {
        fprintf(f, ",");
      } 
    }
    if (i + 1 != c)  {
      fprintf(f, ",");
    }
    fprintf(f, "\n");
  }
  fprintf(f, "}\n");
*/
  fclose(f); 
  
  return 1; 
}
