#include <math.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include "../pipe.h"
#include <fftw3.h>

struct fouriertransform_auxiliary {
  fftw_plan *ft_p;
  fftw_complex *ft_in;
  fftw_complex *ft_out;
};

int fouriertransform_init(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l); 
  if (input == NULL)  {
    fprintf(stderr, "fouriertransform_init: pipe_ must have 1 input\n");
    return 0;
  }
  
  pipe_set_output(p, data_create_complex_from_real(input));  //TODO extend to C->C case using data_get_type()

  //init aux and FFT TODO mem alloc fail frees?
  int *shape = data_get_shape(input);
  int c = shape[0];  //number of channels
  int n = shape[1];  //number of recordings
 
  struct fouriertransform_auxiliary *aux = (struct fouriertransform_auxiliary*)malloc(sizeof(struct fouriertransform_auxiliary));
  if (aux == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for aux failed\n");
    return 0;
  }
  aux->ft_p = (fftw_plan*)malloc(sizeof(fftw_plan)*c);
  if (aux->ft_p == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for aux ft_p failed\n");
    return 0;
  }
  aux->ft_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*c*n);
  if (aux->ft_in == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for aux ft_in failed\n");
    return 0;
  }
  aux->ft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*c*n);
  if (aux->ft_out == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for aux ft_out failed\n");
    return 0;
  }
  for (int i = 0; i < c; i++)  {
    aux->ft_p[i] = fftw_plan_dft_1d(n, (aux->ft_in + i*n), (aux->ft_out + i*n), FFTW_FORWARD, FFTW_ESTIMATE);
  }
  
  pipe_set_auxiliary(p, aux);
 
  return 1;
}

int fouriertransform_run(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "fouriertransform_run: pipe_ has no input\n");
    return 0;
  }

  struct fouriertransform_auxiliary *aux = (struct fouriertransform_auxiliary*)pipe_get_auxiliary(p);
  
  data_copy_from_data_real_to_complex(input, (void*)aux->ft_in);

  int c = data_get_shape(input)[0];
  //fft
  for (int i = 0; i < c; i++)  {
    fftw_execute(aux->ft_p[i]);
  }
  
  data_copy_to_data(pipe_get_output(p), (void*)aux->ft_out);
  
  return 1;
}

int fouriertransform_kill(pipe_* p, linkedlist* l)  {
  struct fouriertransform_auxiliary *aux = (struct fouriertransform_auxiliary*)pipe_get_auxiliary(p);
  fftw_destroy_plan(*aux->ft_p);
  fftw_free(aux->ft_in);
  fftw_free(aux->ft_out);
  return 1;
}
