#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../pipe.h"

int power_init(pipe_* p, linkedlist* l)  {
  printf("POWER INIT\n");
  data* input = *(data**)linkedlist_head(l); 
  if (input == NULL)  {
    fprintf(stderr, "power_init: pipe_ must have 1 input\n");
    return 0;
  }
 
  //2d for now
  int n = input->n;
  int shape[n], stride[n];
  shape[0] = input->shape[0];
  shape[1] = input->shape[1]/4+1; //complex->real->only need half for power due to complex symmetry, +1 extra for 0 freq
  stride[0] = 1;
  stride[1] = 1;  
  p->output = data_create(n, shape, stride);

  return 1;
}

int power_run(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "power_run: pipe_ has no input\n");
    return 0;
  }

  double *buffer = (double*)malloc(data_size(p->output));


  int c = p->output->shape[0];
  int n = p->output->shape[1];
  
  read_lock(input);
  
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      double real = input->buffer[i*input->shape[1] + j*input->stride[1]]; 
      double imag = input->buffer[i*input->shape[1] + j*input->stride[1] + 1]; 
      double abs = sqrt(real*real + imag*imag);
      buffer[i*n + j] = abs;
   //   printf("%f %f = %f\n", real, imag, abs);
    }
  }

  read_unlock(input);
  
  data_copy_to_data(p->output, buffer);

  return 1;
}

int power_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
