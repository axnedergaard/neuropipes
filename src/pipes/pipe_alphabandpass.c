#include "../pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <liquid/liquid.h>

struct bandpass_aux {
  iirfilt_crcf lp; //lowpass filter
  iirfilt_crcf hp; //highpass filter
  double complex *bp_in;
  double complex *bp_tmp;  //buffer for between low pass and high pass
  double complex *bp_out;
};

int alphabandpass_init(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l);
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "alphabandpass_init: pipe_ must have 1 input\n");
  }

  //init output
  if (data_type(input) == TYPE_COMPLEX)  {
    p->output = data_create_from(input);
  }
  else  if (data_type(input) == TYPE_REAL)  {
    p->output = data_create_complex_from_real(input);
  }

  //init aux
  //printf("%p size of double complex=%d size of data=%d\n", p->output, sizeof(double complex), data_size(p->output));
  struct bandpass_aux *aux = (struct bandpass_aux*)malloc(sizeof(struct bandpass_aux));  
  if (aux == NULL)  {
    fprintf(stderr, "alphabandpass_init: failed to alloc mem for aux\n");
    return 0;
  }
  aux->bp_in = (double complex*)malloc(data_size(p->output));  
  if (aux->bp_in == NULL)  {
    fprintf(stderr, "alphabandpass_init: failed to alloc mem for bp_in\n");
    return 0;
  }
  aux->bp_tmp = (double complex*)malloc(data_size(p->output));
  if (aux->bp_tmp == NULL)  {
    fprintf(stderr, "alphabandpass_init: failed to allocate memory for bp_tmp\n");
    return 0;
  }
  aux->bp_out = (double complex*)malloc(data_size(p->output));
  if (aux->bp_out == NULL)  {
    fprintf(stderr, "alphabandpass_init: failed to allocate memory for bp_out\n");
    return 0;
  }
  //create filters
  int order = 4; //filter order
  float f0 = 0.0; //center freq
  float fc_lp = 13.0/128.0;  //lowpass cutoff freq
  float fc_hp = 8.0/128.0;  //highpass cutoff freq
  float Ap = 1.0; //pass-band ripple TODO
  float As = 1.0; //stop-band attenuation TODO
  aux->lp = iirfilt_crcf_create_prototype(LIQUID_IIRDES_BUTTER, LIQUID_IIRDES_LOWPASS, LIQUID_IIRDES_SOS, order, fc_lp, f0, Ap, As);
  aux->hp = iirfilt_crcf_create_prototype(LIQUID_IIRDES_BUTTER, LIQUID_IIRDES_HIGHPASS, LIQUID_IIRDES_SOS, order, fc_hp, f0, Ap, As);

  p->auxiliary = aux;
 
  return 1;
}

int alphabandpass_run(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "alphabandpass_run: must have 1 input\n");
    return 0;
  }

  struct bandpass_aux *aux = (struct bandpass_aux*)p->auxiliary;

  //copy from data
  if (data_type(input) == TYPE_COMPLEX)  {
    data_copy_from_data(input, (void*)aux->bp_in);
  }
  else if (data_type(input) == TYPE_REAL)  {
    data_copy_from_data_real_to_complex(input, (void*)aux->bp_in);
  }

  //filter TODO ASSUME 2D 
  int c = input->shape[0];
  int n = input->shape[1]/2; //divide by 2 because buffer is complex, but data is not
  //low-pass 
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      iirfilt_crcf_execute(aux->lp, aux->bp_in[i*n + j], &(aux->bp_tmp[i*n + j]));
    }
  }
  //high-pass
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      iirfilt_crcf_execute(aux->hp, aux->bp_tmp[i*n + j], &(aux->bp_out[i*n + j]));
    }
  }
 
  //copy to memory
  data_copy_to_data(p->output, (void*)aux->bp_out); 

  return 1;
}

