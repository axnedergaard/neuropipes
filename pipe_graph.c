#include "pipe.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LEN 64

//semi-toy pipe_ that prints all channels of EMOTIV-like data to files readable by a GNU-plot script

int graph_init(pipe_ *p, linkedlist *l)  {
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "graph_init: graph pipe_ must have exactly 1 input\n");
  }
  p->output = NULL;  //print pipe_ has no output
  return 1;
}

int graph_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_iterate(l);
  linkedlist_reset_iterater(l);
  int c = input->shape[0];
  int n = input->shape[1];
//  FILE* f0 = fopen("plot0.dat", "w");
//  FILE* f1 = fopen("plot1.dat", "w");
  FILE *f = fopen("john.dat", "w");
  for (int i = 0; i < n; i++) {
    fprintf(f, "%d %f\n", i, (float)input->buffer[10*n + i]);
  }
  fclose(f);
/*
  char *prestring = "plot";
  for (int i = 0; i < c; i++) {
    char filestring[MAX_STRING_LEN];
    sprintf(filestring, "%s%d.dat", prestring, i); 
    FILE *f = fopen(filestring, "w");   
    for (int j = 0; j < n; j++)  {
      fprintf(f, "%d %f\n", j, input->buffer[i*n + j]);  
    }
    fclose(f); 
  }*/
  return 1; 
}

