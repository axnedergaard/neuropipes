#include "data.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

data* data_create(int n, int *shape, int *stride)  {
  data *d = (data*)malloc(sizeof(data));
  if (d == NULL)  {
    fprintf(stderr, "data_create: failed to allocate memory for struct\n");
    return NULL;  //mem alloc failed
  }
  d->n = n;
  d->shape = (int*)malloc(sizeof(int)*d->n);
  d->stride = (int*)malloc(sizeof(int)*d->n);
  d->len = 1;
  //printf("creating data n=%d stride0=%d stride1=%d\n", n, stride[0], stride[1]);
  for (int i = 0; i < d->n; i++)  {
    if (shape == NULL)  {
      d->shape[i] = 1;
    }
    else  {
      d->shape[i] = shape[i];
    }
    if (stride == NULL)  {
      d->stride[i] = 1;
    }
    else  {
      d->stride[i] = stride[i];
    }
    d->len *= shape[i];
  }
  if ((d->buffer = (double*)malloc(sizeof(double)*d->len)) == NULL)  {
    fprintf(stderr, "data_create: failed to allocate memory for buffer\n");
  }
//  printf("created data with %d %d\n", d->shape[0], d->shape[1]);
  return d;
}

data* data_create_from_string(char *str)  {
  if (strcmp(str, "SINGLE") == 0)  {
    return data_create(0, NULL, NULL);
  }
  else if (strcmp(str, "EMOTIV") == 0)  {  //emotiv 14 channels 128 readings
    int shape[2] = {14, 128};
    int stride[2] = {1, 1};
    return data_create(2, shape, stride);
  }
  else  {
    fprintf(stderr, "data_create: data str not recognised\n");
    return NULL;
  } 
}

int data_destroy(data *d)  {
  free(d->buffer);
  free(d->shape);
  free(d->stride);
  free(d);
  return 1;
} 

int data_size(data *d)  {  //returns size of buffer in memory TODO possible to use sizeof(buffer) instead?
  return d->len*sizeof(double);
}

int data_get_type(data *d)  {  //complex if last dimension stride is larger than 1 (will be 2)
  if (d->stride[d->n-1] > 1)  {
    return TYPE_COMPLEX;
  }
  else  {
    return TYPE_REAL;
  }
}

data *data_create_complex_from_real(data *input)  {
  data *output = NULL;
  int n = input->n;
  int shape[n];
  int stride[n];
  for (int i = 0; i < n - 1; i++)  {
    shape[i] = input->shape[i];
    stride[i] = input->stride[i];
  }
  shape[n-1] = input->shape[n-1]*2;
  stride[n-1] = input->stride[n-1]*2;
  output = data_create(n, shape, stride);
  //initialise to 0?
  return output;
}

data *data_create_real_from_complex(data *input)  {
  data *output = NULL;
  int n = input->n;
  int shape[n];
  int stride[n];
  for (int i = 0; i < n - 1; i++)  {
    shape[i] = input->shape[i];
    stride[i] = input->stride[i];
  }
  shape[n-1] = input->shape[n-1]/2;
  stride[n-1] = input->stride[n-1]/2;
  output = data_create(n, shape, stride);
  //initialise to 0?
  return output;
}

int data_copy_from_data_real_to_complex(data *d, double *buf)  {  //only 2D atm
  int c = d->shape[0];
  int n = d->shape[1];
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      double real = d->buffer[i*n + j];
      buf[i*n*2 + j*2] = real; //2*j because real->complex
    }
  }
  return 1;
}

int data_copy_to_data_complex_to_real(data *d, double *buf)  {
  int c = d->shape[0];
  int n = d->shape[1];
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      double real = buf[i*n*2 + j*2];
      d->buffer[i*n + j] = real;  //ignore complex values
    }
  }
  return 1;
}

int data_copy_to_data(data *d, double *buf)  {
  int len = 1;
  for (int i = 0; i < d->n; i++)  {
    len *= d->shape[i];
  }

  for (int i = 0; i < len; i++)  {
    d->buffer[i] = buf[i];
  }
  return 1;
}

int data_copy_from_data(data *d, double *buf)  {
  int len = 1;
  for (int i = 0; i < d->n; i++)  {
    len *= d->shape[i];
  }
  for (int i = 0; i < len; i++)  {
    buf[i] = d->buffer[i];
  }
  return 1; 
}

int data_write(data *d, FILE* f)  { 
  int c = d->shape[0];
  int n = d->shape[1];
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      float reading = d->buffer[i*n + j];  
      fprintf(f, "%f ", reading);
    }
    fprintf(f, "\n");
  }
  return 1;
}

int data_read(data *d, FILE* f)  {
  return 1;
}
