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

int inversefouriertransform_init(pipe_ *p, linkedlist *l)  {
  data* input = *(data**)linkedlist_head(l); 
  if (input == NULL)  {
    fprintf(stderr, "inversefouriertransform_init: pipe_ must have 1 input\n");
    return 0;
  }
  
  p->output = data_create_real_from_complex(input);

  //init buffer and FFT TODO mem alloc fail frees?
  int c = input->shape[0];  //number of channels
  int n = input->shape[1]/2;  //number of recordings, divide by 2 because input complex
  
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
    buffer->ft_p[i] = fftw_plan_dft_1d(n, (buffer->ft_in + i*n), (buffer->ft_out + i*n), FFTW_BACKWARD, FFTW_ESTIMATE);
  }
  p->auxiliary = buffer;

  return 1;
}

int inversefouriertransform_run(pipe_ *p, linkedlist *l)  {
  data* input = *(data**)linkedlist_iterate(l);
  if (input == NULL)  {
    fprintf(stderr, "inversefouriertransform_run: pipe_ has no input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);
 
  struct buffer_fouriertransform *buffer = (struct buffer_fouriertransform*)p->auxiliary;
  
  data_copy_from_data(input, (void*)buffer->ft_in);

  //fft
  int c = input->shape[0];
  int n = input->shape[1]/2;
  for (int i = 0; i < c; i++)  {
    fftw_execute(buffer->ft_p[i]);
    for (int j = 0; j < n; j++)  {  //divide by n because fftw3 inverse fft doesn't automatically TODO CBLAS?
      *(buffer->ft_out + i*n + j) = (double)*(buffer->ft_out + i*n + j) / (double)n;
    }
  }

  //copy to buffer
  data_copy_to_data_complex_to_real(p->output, (void*)buffer->ft_out);
  
  return 1;
}

int inversefouriertransform_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
