#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../pipe.h"

int power_init(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "power_init: pipe must have 1 input\n");
    return 0;
  }
  if (data_type(input) != TYPE_COMPLEX)  {
    fprintf(stderr, "power_init: pipe input must be complex\n");
    return 0;
  }
 
  int *input_shape, *input_stride;
  int n = data_spec(input, &input_shape, &input_stride);
  int shape[n], stride[n];
  shape[0] = input_shape[0];
  shape[1] = input_shape[1]/4+1; //complex->real->only need half for power due to complex symmetry, +1 extra for 0 freq
  stride[0] = 1;
  stride[1] = 1;  
  pipe_set_output(p, data_create(n, shape, stride));

  return 1;
}

int power_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "power_run: pipe_ has no input\n");
    return 0;
  }
  data *output = pipe_get_output(p);

  double buffer[data_size(output)];
  int *input_shape, *input_stride;
  data_spec(input, &input_shape, &input_stride);
  double *input_buffer = data_get_buffer(input);
  int *output_shape;
  data_spec(output, &output_shape, NULL);
  int c = output_shape[0]; //channels
  int n = output_shape[1];  //recording for each channel

  read_lock(input);
  
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      double real = input_buffer[i*input_shape[1] + j*input_stride[1]]; 
      double imag = input_buffer[i*input_shape[1] + j*input_stride[1] + 1]; 
      double abs = sqrt(real*real + imag*imag);
      buffer[i*n + j] = abs;
    }
  }

  read_unlock(input);
  
  data_copy_to_data(output, buffer);

  return 1;
}

int power_kill(pipe_ *p, linkedlist *l)  {
  return 1;
}
