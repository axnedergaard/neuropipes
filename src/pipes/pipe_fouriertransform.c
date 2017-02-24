#include <math.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include "../pipe.h"
#include <fftw3.h>

struct ft_aux {
  fftw_plan *ft_p;
  fftw_complex *ft_in;
  fftw_complex *ft_out;
};

int fouriertransform_init(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_iterate(l); 
  if (input == NULL)  {
    fprintf(stderr, "fouriertransform_init: pipe_ must have 1 input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);
  
  p->output = data_create_complex_from_real(input);  //TODO extend to C->C case using data_get_type()

  //init ft_aux and FFT TODO mem alloc fail frees?
  int c = input->shape[0];  //number of channels
  int n = input->shape[1];  //number of recordings
 
  struct ft_aux *ft_aux = (struct ft_aux*)malloc(sizeof(struct ft_aux));
  if (ft_aux == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for ft_aux failed\n");
    return 0;
  }
  ft_aux->ft_p = (fftw_plan*)malloc(sizeof(fftw_plan)*c);
  if (ft_aux->ft_p == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for ft_aux ft_p failed\n");
    return 0;
  }
  ft_aux->ft_in = (fftw_complex*)malloc(sizeof(fftw_complex)*c*n);
  if (ft_aux->ft_in == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for ft_aux ft_in failed\n");
    return 0;
  }
  ft_aux->ft_out = (fftw_complex*)malloc(sizeof(fftw_complex)*c*n);
  if (ft_aux->ft_out == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for ft_aux ft_out failed\n");
    return 0;
  }
  for (int i = 0; i < c; i++)  {
    ft_aux->ft_p[i] = fftw_plan_dft_1d(n, (ft_aux->ft_in + i*n), (ft_aux->ft_out + i*n), FFTW_FORWARD, FFTW_ESTIMATE);
  }
  p->auxiliary = ft_aux;
 
  return 1;
}

int fouriertransform_run(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_iterate(l);
  if (input == NULL)  {
    fprintf(stderr, "fouriertransform_run: pipe_ has no input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);

  struct ft_aux *ft_aux = (struct ft_aux*)p->auxiliary;
  
  data_copy_from_data_real_to_complex(input, (void*)ft_aux->ft_in);

  //fft
  for (int i = 0; i < input->shape[0]; i++)  {
    fftw_execute(ft_aux->ft_p[i]);
  }
  
  data_copy_to_data(p->output, (void*)ft_aux->ft_out);
  
  return 1;
}

int fouriertransform_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
