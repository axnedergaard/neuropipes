#include <math.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include "../pipe.h"
#include <fftw3.h>

struct auxiliary_fouriertransform {
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
  struct auxiliary_fouriertransform *aux = (struct auxiliary_fouriertransform*)malloc(sizeof(struct auxiliary_fouriertransform));
  pipe_set_auxiliary(p, aux);
  pipe_set_output(p, data_create_real_from_complex(input));

  int *shape = data_get_shape(input);
  int c = shape[0];  //number of channels
  int n = shape[1]/2;  //number of recordings, divide by 2 because input complex
  
  if (aux == NULL)  {
    fprintf(stderr, "inversefouriertransform_init: mem alloc for aux failed\n");
    return 0;
  }
  aux->ft_p = (fftw_plan*)malloc(sizeof(fftw_plan)*c);
  if (aux->ft_p == NULL)  {
    fprintf(stderr, "inversefouriertransform_init: mem alloc for aux ft_p failed\n");
    return 0;
  }
  aux->ft_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*c*n);
  if (aux->ft_in == NULL)  {
    fprintf(stderr, "inversefouriertransform_init: mem alloc for aux ft_in failed\n");
    return 0;
  }
  aux->ft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*c*n);
  if (aux->ft_out == NULL)  {
    fprintf(stderr, "fouriertransform_init: mem alloc for buffer ft_out failed\n");
    return 0;
  }
  for (int i = 0; i < c; i++)  {
    aux->ft_p[i] = fftw_plan_dft_1d(n, (aux->ft_in + i*n), (aux->ft_out + i*n), FFTW_BACKWARD, FFTW_ESTIMATE);
  }

  return 1;
}

int inversefouriertransform_run(pipe_ *p, linkedlist *l)  {
  data* input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "inversefouriertransform_run: pipe_ has no input\n");
    return 0;
  }
 
  struct auxiliary_fouriertransform *aux = (struct auxiliary_fouriertransform*)pipe_get_auxiliary(p);
  
  data_copy_from_data(input, (void*)aux->ft_in);

  //inverse fft
  int *shape = data_get_shape(input);
  int c = shape[0];
  int n = shape[1]/2;
  for (int i = 0; i < c; i++)  {
    fftw_execute(aux->ft_p[i]);
    for (int j = 0; j < n; j++)  {  
      *(aux->ft_out + i*n + j) = (double)*(aux->ft_out + i*n + j) / (double)n;  //divide by n because fftw3 inverse fft doesn't automatically TODO CBLAS?
    }
  }

  //copy to buffer
  data_copy_to_data_complex_to_real(pipe_get_output(p), (void*)aux->ft_out);
  
  return 1;
}

int inversefouriertransform_kill(pipe_* p, linkedlist* l)  {
  struct auxiliary_fouriertransform *aux = (struct auxiliary_fouriertransform*)pipe_get_auxiliary(p);
  fftw_destroy_plan(*aux->ft_p);
  fftw_free(aux->ft_in);
  fftw_free(aux->ft_out);
  return 1;
}
