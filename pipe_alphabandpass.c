#include "pipe.h"
#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <liquid/liquid.h>

//assume emotivc -> emotivc for now...
int alphabandpass_init(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_iterate(l);
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "alphabandpass_init: pipe_ must have 1 input\n");
  }
  linkedlist_reset_iterater(l);
  //init output
  int n = input->n;
  int *shape = input->shape;
  int *stride = input->stride;
  p->output = data_create(n, shape, stride);
  //init to 0
  for (int i = 0; i < p->output->shape[0]; i++)  {
    for (int j = 0; j < p->output->shape[1]; j++)  {
      p->output->buffer[i*p->output->shape[1] + j] = 0; 
    }
  } 
  return 1;
}

int alphabandpass_run(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_iterate(l);
  if (input == NULL)  {
    fprintf(stderr, "alphabandpass_run: must have 1 input\n");
    return 0;
  }
  linkedlist_reset_iterater(l);

  //copy from memory
  int c = p->output->shape[0];
  int n = p->output->shape[1];
  float complex x[c][n/2];
  float complex y[c][n/2];
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n/2; j++)  {
      x[i][j] = input->buffer[i*n + j*input->stride[1]] + I*(input->buffer[i*n + j*input->stride[1]+1]);
    }
  } 

  //filter
  int order = 5;
  float fc = 8.0/128.0;  //cutoff freq???
  float f0 = 10.0/128.0; //center freq
  iirfilt_crcf q = iirfilt_crcf_create_prototype(LIQUID_IIRDES_BUTTER, LIQUID_IIRDES_BANDPASS, LIQUID_IIRDES_SOS, order, fc, f0, 1, 1); //last 2 params???

  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n/2; j++)  {
      iirfilt_crcf_execute(q, x[i][j], &y[i][j]);
    }
  }

  iirfilt_crcf_destroy(q);
 
  //copy to memory
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n/2; j++)  {
      float real = creal(y[i][j]);
      float imag = cimag(y[i][j]);
      p->output->buffer[i*n + j*p->output->stride[1]] = real;
      p->output->buffer[i*n + j*p->output->stride[1]+1] = imag;
    }
  }

/*
  //copy from memory
  int c = input->shape[0];
  int n = input->shape[1];
  double in[c][n][2];
  double out[c][n][2];
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      in[i][j][0] = input->buffer[i*n + j];
      in[i][j][1] = input->buffer[i*n + j + 1];
      //init out?
      out[i][j][0] = 0;
      out[i][j][1] = 0;
    }
  }

  //TODO PROPER

  //create filter
  FidFilter *lp_filter, *hp_filter;
  double lp_rate = 128;
  char *lp_p = "LpBu4/12";
  char *hp_p = "HpBu4/8";

  //order of filter? error checking
  fid_parse(lp_rate, &lp_p, &lp_filter);  //12 upper limit?
  fid_parse(lp_rate, &hp_p, &hp_filter);  //12 upper limit?
  //merge filters?
  FidFilter *filter = fid_cat(1, lp_filter, hp_filter);
  FidFunc **funcp = (FidFunc**)malloc(sizeof(FidFunc*)*c);
  FidRun **run = (FidRun**)malloc(sizeof(FidRun*)*c);
//  int delay;
  void **buf = (void**)malloc(sizeof(void*)*c);

  for (int i = 0; i < c; i++)  {
    run[i] = fid_run_new(filter, &funcp[i]);   
    buf[i] = fid_run_newbuf(run[i]);
  } 

  //run filter on data
  for (int i = 0; i < c; i++)  {
   // out[i] = funcp[i](buf[i], in[i]);
  }
  
  //copy to memory
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      double real = out[i][j][0];
      double imag = out[i][j][1];
      p->output->buffer[i*n + j] = real;
      p->output->buffer[i*n + j] = imag;
    }
  } 
 */
  return 1;
}

