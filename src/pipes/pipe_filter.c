#include "../pipe.h"
#include "../parameters.h"
#include "../fidlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPEC_MAX 32

struct filter_aux {
  FidFilter *ff;
  FidRun *run;
  FidFunc *func;
  void **buf;
  double *input;
  double *output;
};

//TODO work on complex signals?

int filter_init(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l);
  if (linkedlist_size(l) != 1)  {
    fprintf(stderr, "filter_init: pipe_ must have 1 input\n");
  }

  p->output = data_create_from(input);
  
  //get parameters 
  char pass[] = "band";
  int order = 2;
  int lc = 8;  //lower cutoff
  int hc = 12;  //higher cutoff

  char *pass_param = get_parameter(p, "pass"); //band, high, low...
  if (pass_param != NULL)  {
    if (strcmp(pass_param, "band") == 0)  {  //redundant?
      strcpy(pass, "band");  //TODO unsafe
    }
    else if (strcmp(pass_param, "high") == 0)  {
      strcpy(pass, "high");
    }
    else if (strcmp(pass_param, "low") == 0)  {
      strcpy(pass, "low");
    }
  } 
  free(pass_param);
  char *lc_param = get_parameter(p, "lc");
  if (lc_param != NULL)  {
    lc = atoi(lc_param);
  }
  free(lc_param);
  char *hc_param = get_parameter(p, "hc");
  if (hc_param != NULL)  {
    hc = atoi(hc_param);
  }
  free(hc_param);
  char *order_param = get_parameter(p, "order");
  if (order_param != NULL)  {
    order = atoi(order_param);
  }
  free(order_param);
 
  struct filter_aux *aux = (struct filter_aux*)malloc(sizeof(struct filter_aux));  
  if (aux == NULL)  {
    fprintf(stderr, "filter_init: failed to alloc mem for aux\n");
    return 0;
  }

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

  int c = p->output->shape[0];
  double rate = 128; //param?
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

int filter_run(pipe_* p, linkedlist* l)  {
  data* input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "filter_run: must have 1 input\n");
    return 0;
  }

  struct filter_aux *aux = (struct filter_aux*)p->auxiliary;

  //copy from data
  data_copy_from_data(input, (void*)aux->input);
/*
  if (data_pass(input) == TYPE_COMPLEX)  {
    data_copy_from_data(input, (void*)aux->input);
  }
  else if (data_pass(input) == TYPE_REAL)  { //?
    data_copy_from_data_real_to_complex(input, (void*)aux->input);
  }
*/
  //filter
  int c = p->output->shape[0];
  int n = p->output->shape[1];
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      aux->output[i*n + j] = aux->func(aux->buf[i], aux->input[i*n + j]);
    }
  }
  
  //copy to memory
  data_copy_to_data(p->output, (void*)aux->output); 

  return 1;
}


int filter_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
