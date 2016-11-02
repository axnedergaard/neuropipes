#include "pipe.h"
#include "linkedlist.h"

int mean_init(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_iterate(l);
  linkedlist_reset_iterater(l);
  int n = input->n;
  int shape[n];
  shape[0] = input->shape[0];
  shape[1] = 1;
  p->output = data_create(n, shape, input->stride);
  return 1;
}

int mean_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_iterate(l);
  linkedlist_reset_iterater(l);
  int c = input->shape[0];
  int n = input->shape[1];
  for (int i = 0; i < c; i++)  {
    float mean = 0;
    for (int j = 0; j < n; j++)  {
      mean += (float)input->buffer[i*n + j];
    }
    mean /= (float)n;
    p->output->buffer[i] = mean;
  }
  return 1;
}

