#include <math.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include "pipe.h"
#include "linkedlist.h"
#include <fftw3.h>

int fouriertransform_init(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_iterate(l); 
  if (input == NULL)  {
    fprintf(stderr, "fouriertransform_init: pipe_ must have 1 input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);
  p->output = data_create_complex_from_real(input);  //TODO extend to C->C case using data_get_type()
  return 1;
}

int fouriertransform_run(pipe_* p, linkedlist* l)  {

  data* input = *(data**)linkedlist_iterate(l);
  if (input == NULL)  {
    fprintf(stderr, "fouriertransform_run: pipe_ has no input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);

  //TODO auxiliary structure and init plan...

  int c = input->shape[0];  //number of channels
  int n = input->shape[1];  //number of recordings
  double ft_in[c][n][2];
  double ft_out[c][n][2];
  fftw_plan *ft_p;

  ft_p = (fftw_plan*)malloc(sizeof(fftw_plan)*c);
  if (ft_p == NULL)  {
    fprintf(stderr, "fouriertransform_run: mem alloc failed\n");
    return 0;
  }

  for (int i = 0; i < c; i++)  {
    ft_p[i] = fftw_plan_dft_1d(n, ft_in[i], ft_out[i], FFTW_FORWARD, FFTW_ESTIMATE);
  }
  //(end aux init)
 
  data_copy_from_data_real_to_complex(input, ft_in);

  //fft
  for (int i = 0; i < c; i++)  {
    fftw_execute(ft_p[i]);
  }
  
  data_copy_to_data(p->output, ft_out);
//  data_copy_to_data_complex_to_complex(p->output, ft_out);
  
  free(ft_p);
  printf("what\n");

  return 1;
}
