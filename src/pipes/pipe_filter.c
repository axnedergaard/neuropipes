#include "../pipe.h"
#include "../parameters.h"
#include "../fidlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Butterworth filter using fidlib

#define SPEC_MAX 32  //max length of fidlib pipe specification

struct filter_aux {
  FidFilter *ff;
  FidRun *run;
  FidFunc *func;
  void **buf;
  double *input;
  double *output;
};

int filter_init(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l);
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "filter_init: pipe_ must have 1 input\n");
  }
  struct filter_aux *aux = (struct filter_aux*)malloc(sizeof(struct filter_aux));  
  pipe_set_auxiliary(p, aux);
  data *output = data_create_from(input);
  pipe_set_output(p, output);

  //get parameters 
  char pass[PARAM_MAX];  //filter pass type
  strncpy(pass, "band", PARAM_MAX);
  int order = 2; //filter order
  int lc = 8;  //lower cutoff frequency
  int hc = 12;  //higher (upper) cutoff frequency
  int rate = 128; //sampling rate
  set_parameter_string(p, "pass", pass);
  set_parameter_int(p, "order", &order);
  set_parameter_int(p, "lc", &lc);
  set_parameter_int(p, "hc", &hc);
  set_parameter_int(p, "rate", &rate);
  
  //set spec
  char *spec = (char*)malloc(sizeof(char)*SPEC_MAX);
  if (strcmp(pass, "band") == 0)  {
    snprintf(spec, SPEC_MAX, "BpBu%d/%d-%d", order, lc, hc);
  }
  else if (strcmp(pass, "high") == 0)  {
    snprintf(spec, SPEC_MAX, "HpBu%d/%d", order, lc);
  }
  else if (strcmp(pass, "low") == 0)  {
    snprintf(spec, SPEC_MAX, "LpBu%d/%d", order, hc);
  }
 
  //create filter
  char *error = fid_parse(rate, &spec, &aux->ff);
  if (error != NULL)  {
    fprintf(stderr, "pipe_filter_init: fidlib parse error %s\n", error);
  }

  int c = data_get_shape(output)[0];  //number of channels
  aux->run = fid_run_new(aux->ff, &aux->func);
  aux->input = (double*)malloc(data_size(output));
  aux->output = (double*)malloc(data_size(output));
  aux->buf = (void**)malloc(c*sizeof(void*));

  for (int i = 0; i < c; i++)  {
    aux->buf[i] = fid_run_newbuf(aux->run);
  }

  return 1;
}

int filter_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "filter_run: must have 1 input\n");
    return 0;
  }
  struct filter_aux *aux = (struct filter_aux*)pipe_get_auxiliary(p);
  data *output = pipe_get_output(p);

  data_copy_from_data(input, (void*)aux->input);
  
  //filter
  int *shape = data_get_shape(output);
  int c = shape[0];
  int n = shape[1];
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      aux->output[i*n + j] = aux->func(aux->buf[i], aux->input[i*n + j]);
    }
  }
  
  data_copy_to_data(output, (void*)aux->output); 

  return 1;
}

int filter_kill(pipe_* p, linkedlist* l)  {
  struct filter_aux *aux = (struct filter_aux*)pipe_get_auxiliary(p);
  fid_run_freebuf(aux->buf);
  fid_run_free(aux->run);
  free(aux->input);
  free(aux->output);
  return 1;
}
