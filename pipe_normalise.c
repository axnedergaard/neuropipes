#include "pipe.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>

#define LARGE 999999

struct auxiliary_normalise  {
  double min;
  double max;
};

int normalise_init(pipe_ *p, linkedlist *l)  {
  struct auxiliary_normalise *a = (struct auxiliary_normalise*)malloc(sizeof(struct auxiliary_normalise));
  a->min = LARGE;
  a->max = -LARGE;
  p->output = data_create_from_string("SINGLE");
  p->auxiliary = a;
  return 1;
}

int normalise_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_iterate(l);
  linkedlist_reset_iterater(l);
  struct auxiliary_normalise *a = (struct auxiliary_normalise*)p->auxiliary;
  int c = input->shape[0];
  double channel_mean = (float)input->buffer[10];
//  for (int i = 0; i < c; i++)  {
//    channel_mean += (float)input->buffer[i];
//  }
//  channel_mean /= c;
  if (channel_mean > a->max)  {
    a->max = channel_mean; 
  }
  if (channel_mean < a->min)  {
    a->min = channel_mean;
  }
  double normalised = (channel_mean - a->min) / (a->max - a->min);
  p->output->buffer[0] = normalised;
  return 1;
}

