#include <math.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include "../pipe.h"
#include <fftw3.h>

struct buffer_fouriertransform {
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

  //init buffer and FFT TODO mem alloc fail frees?
  int c = input->shape[0];  //number of channels
  int n = input->shape[1];  //number of recordings
 
  struct buffer_fouriertransform *buffer = (struct buffer_fouriertransform*)malloc(sizeof(struct buffer_fouriertransform));
  if (buffer == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for buffer failed\n");
    return 0;
  }
  buffer->ft_p = (fftw_plan*)malloc(sizeof(fftw_plan)*c);
  if (buffer->ft_p == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for buffer ft_p failed\n");
    return 0;
  }
  buffer->ft_in = (fftw_complex*)malloc(sizeof(fftw_complex)*c*n);
  if (buffer->ft_in == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for buffer ft_in failed\n");
    return 0;
  }
  buffer->ft_out = (fftw_complex*)malloc(sizeof(fftw_complex)*c*n);
  if (buffer->ft_out == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for buffer ft_out failed\n");
    return 0;
  }
  for (int i = 0; i < c; i++)  {
    buffer->ft_p[i] = fftw_plan_dft_1d(n, (buffer->ft_in + i*n), (buffer->ft_out + i*n), FFTW_FORWARD, FFTW_ESTIMATE);
  }
  p->auxiliary = buffer;
 
  return 1;
}

int fouriertransform_run(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_iterate(l);
  if (input == NULL)  {
    fprintf(stderr, "fouriertransform_run: pipe_ has no input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);

  struct buffer_fouriertransform *buffer = (struct buffer_fouriertransform*)p->auxiliary;
  
  data_copy_from_data_real_to_complex(input, (void*)buffer->ft_in);

  //fft
  for (int i = 0; i < input->shape[0]; i++)  {
    fftw_execute(buffer->ft_p[i]);
  }
  
  data_copy_to_data(p->output, (void*)buffer->ft_out);
  
  return 1;
}

int fouriertransform_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
