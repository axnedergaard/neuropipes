#include "../pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include "../fidlib.h"

struct bandpass_aux {
  FidFilter *ff;
  FidRun *run;
  FidFunc *func;
  void **buf;
  double *input;
  double *output;
};

int alphabandpass_init(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l);
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "alphabandpass_init: pipe_ must have 1 input\n");
  }

  p->output = data_create_from(input);
/*
  //init output
  if (data_type(input) == TYPE_COMPLEX)  {
    p->output = data_create_from(input);
  }
  else  if (data_type(input) == TYPE_REAL)  {
    p->output = data_create_complex_from_real(input);
  }
*/
  //printf("%p size of double complex=%d size of data=%d\n", p->output, sizeof(double complex), data_size(p->output));
  struct bandpass_aux *aux = (struct bandpass_aux*)malloc(sizeof(struct bandpass_aux));  
  if (aux == NULL)  {
    fprintf(stderr, "alphabandpass_init: failed to alloc mem for aux\n");
    return 0;
  }
  int len = p->output->len;
  int c = p->output->shape[0];
  int n = p->output->shape[1];
  double rate = 128; //TODO??
  char *spec = "BpBu2/8-12";  //bandpass butterworth, order 2, range 8Hz-12Hz 
  fid_parse(rate, &spec, &aux->ff);  //TODO error check
  aux->run = fid_run_new(aux->ff, &aux->func);
  aux->input = (double*)malloc(data_size(p->output));
  aux->output = (double*)malloc(data_size(p->output));
  aux->buf = (void**)malloc(c*sizeof(void*));
  for (int i = 0; i < c; i++)  {
    aux->buf[i] = fid_run_newbuf(aux->run);
  }

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
  data_copy_from_data(input, (void*)aux->input);
/*
  if (data_type(input) == TYPE_COMPLEX)  {
    data_copy_from_data(input, (void*)aux->input);
  }
  else if (data_type(input) == TYPE_REAL)  { //?
    data_copy_from_data_real_to_complex(input, (void*)aux->input);
  }
*/
  //filter
  int len = p->output->len;
  int c = p->output->shape[0];
  int n = p->output->shape[1];
  int bi = 0;
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      aux->output[i*n + j] = aux->func(aux->buf[i], aux->input[i*n + j]);
    }
  }
  
  //copy to memory
  data_copy_to_data(p->output, (void*)aux->output); 

  return 1;
}

int alphabandpass_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
