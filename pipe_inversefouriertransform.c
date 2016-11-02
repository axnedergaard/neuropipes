#include <math.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include "pipe.h"
#include "linkedlist.h"
#include <fftw3.h>

int inversefouriertransform_init(pipe_ *p, linkedlist *l)  {
  data* input = *(data**)linkedlist_iterate(l); 
  if (input == NULL)  {
    fprintf(stderr, "inversefouriertransform_init: pipe_ must have 1 input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);
  
  p->output = data_create_real_from_complex(input);

  return 1;
}

int inversefouriertransform_run(pipe_ *p, linkedlist *l)  {
  data* input = *(data**)linkedlist_iterate(l);
  if (input == NULL)  {
    fprintf(stderr, "inversefouriertransform_run: pipe_ has no input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);
 
 
  //translate input buffer into fftw_complex
  //TODO auxiliary structure and init plan...
  int c = p->output->shape[0];  //number of channels
  int n = p->output->shape[1];  //number of recordings
  double ft_in[c][n][2], ft_out[c][n][2];
  fftw_plan *ft_p;
  
  ft_p = (fftw_plan*)malloc(sizeof(fftw_plan)*c);
  if (ft_p == NULL)  {
    fprintf(stderr, "inversefouriertransform_run: mem alloc failed\n");
    return 0;
  }

  for (int i = 0; i < c; i++)  {
    ft_p[i] = fftw_plan_dft_1d(n, ft_in[i], ft_out[i], FFTW_BACKWARD, FFTW_ESTIMATE);
  }

  data_copy_from_data(input, ft_in);

  //fft
  for (int i = 0; i < c; i++)  {
    fftw_execute(ft_p[i]);
    for (int j = 0; j < n; j++)  {  //divide by n because fftw3 inverse fft doesn't automatically
      ft_out[i][j][0] = ft_out[i][j][0]/n;
    }
  }

  //copy to buffer
  data_copy_to_data_complex_to_real(p->output, ft_out);
  
  free(ft_p);

  return 1;
}

