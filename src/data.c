#include "data.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "edflib.h"

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
  d->blocking = 0;
//  printf("created data with %d %d\n", d->shape[0], d->shape[1]);
  
  return d;
}

int data_make_blocking(data *d)  {  //make data blocking
  pthread_mutex_init(&d->mutex, NULL);  //TODO check init success
  pthread_cond_init(&d->cond_read, NULL);
  pthread_cond_init(&d->cond_written, NULL);
  d->blocking = 1;
  d->reads = 0;
  d->readers = 0; 
  return 1;
}

int data_blocking(data *d)  {
  return d->blocking;
}

void data_reset_reads(data *d)  {
  d->reads = 0;
}

void data_increment_reads(data *d)  {
  d->reads++;
}

void data_increment_readers(data *d)  {
  d->readers++;
}

int data_unblock(data *d)  {
  pthread_mutex_lock(&d->mutex);
  d->readers = 0;
  d->reads = 0;
  d->writes = 0;
  d->blocking = 0;
  pthread_cond_broadcast(&d->cond_written);
  pthread_cond_broadcast(&d->cond_read);
  pthread_mutex_unlock(&d->mutex);
  return 1;
}

void data_reset_readers(data *d)  {  //to allow killed thread to finish without waiting for data to be read
  d->readers = 0;
}

void read_lock(data *d)  {
  pthread_mutex_lock(&d->mutex);
  while (d->writes == 0)  {
    pthread_cond_wait(&d->cond_written, &d->mutex);
  }
}

void write_lock(data *d)  {
  pthread_mutex_lock(&d->mutex);
  while ((d->writes > 0)  && (d->reads < d->readers))  {
    pthread_cond_wait(&d->cond_read, &d->mutex);
  } 
}

void write_unlock(data *d)  {
  d->reads = 0;
  d->writes++;
  pthread_cond_broadcast(&d->cond_written);
  pthread_mutex_unlock(&d->mutex);
}

void read_unlock(data *d)  {
  d->reads++;
  if (d->reads >= d->readers)  {
    d->writes = 0;
    pthread_cond_broadcast(&d->cond_read);
  }
  pthread_mutex_unlock(&d->mutex); 
}

data* data_create_from_string(char *str)  {
  if (strcmp(str, "SINGLE") == 0)  {
    return data_create(0, NULL, NULL);
  }
  else if (strcmp(str, "EMOTIV") == 0)  {  //emotiv 14 channels 8 readings
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

int data_type(data *d)  {  //complex if last dimension stride is larger than 1 (will be 2)
  if (d->stride[d->n-1] > 1)  {
    return TYPE_COMPLEX;
  }
  else  {
    return TYPE_REAL;
  }
}

int data_ready(data *d)  {  //if (new) data has been written to buffer, i.e. buffer ready
  //return d->writes != 0;
  if (d->writes == 0)  return 0;
  else  return 1;
}

void data_broadcast_read(data *d)  {
  pthread_cond_broadcast(&d->cond_read);
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

data *data_create_from(data *input)  {
  data *output = NULL;
  int n = input->n;
  int shape[n];
  int stride[n];
  for (int i = 0; i < n; i++)  {
    shape[i] = input->shape[i];
    stride[i] = input->stride[i];
  }
  output = data_create(n, shape, stride);
  return output;
}

int data_copy_from_data_real_to_complex(data *d, double *buf)  {  //only 2D atm
  int c = d->shape[0];
  int n = d->shape[1];
 
  if (d->blocking == 1)  {
    read_lock(d);
  }
 
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      buf[(i*n + j)*2] = d->buffer[i*n + j];
    }
  }
  
  if (d->blocking == 1)  {
    read_unlock(d);
  }

  return 1;
}

int data_copy_to_data_complex_to_real(data *d, double *buf)  {  //TODO blocking
  int c = d->shape[0];
  int n = d->shape[1];

  if (d->blocking == 1)  {
    write_lock(d);
  }

  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      d->buffer[i*n + j] = buf[(i*n + j)*2];  //ignore complex values
    }
  }

  if (d->blocking == 1)  {
    write_unlock(d);
  }

  return 1;
}

int data_copy_to_data(data *d, double *buf)  {
  int len = 1;
  for (int i = 0; i < d->n; i++)  {
    len *= d->shape[i];
  }

  if (d->blocking == 1)  {
    write_lock(d);
  }

  for (int i = 0; i < len; i++)  {
    d->buffer[i] = buf[i];
  }

  if (d->blocking == 1)  {
    write_unlock(d);
  }
  
  return 1;
}

int data_copy_from_data(data *d, double *buf)  {
  int len = 1;
  for (int i = 0; i < d->n; i++)  {
    len *= d->shape[i];
  }

  if (d->blocking == 1)  {
    read_lock(d);
  }

  for (int i = 0; i < len; i++)  {
    buf[i] = d->buffer[i];
    //buf + i*8) = d->buffer[i];
  }
  
  if (d->blocking == 1)  {
    read_unlock(d);
  }
  return 1; 
}

int data_write(data *d, FILE* f)  { 
  int c = d->shape[0];
  int n = d->shape[1];

  if (d->blocking == 1)  {
    read_lock(d);
  }

  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      float reading = d->buffer[i*n + j];  
      fprintf(f, "%.0f ", reading);
    }
    fprintf(f, "\n");
  }

  if (d->blocking == 1)  {
    read_unlock(d);
  }

  return 1;
}

int data_write_edf(data *d, char* fn)  {
  int c = d->shape[0];
  int n = d->shape[1];

  //open edf file
  int handle = edfopen_file_writeonly(fn, EDFLIB_FILETYPE_EDFPLUS, c);
  if (handle < 0)  {
    fprintf(stderr, "data_write_edf: failed to open file for writing\n");
  }
  //set attributes
  for (int i = 0; i < c; i++)  {
   if (edf_set_samplefrequency(handle, i, n) != 0)  printf("failed to set sample frequency\n");
    if (edf_set_physical_maximum(handle, i, 10000) != 0)  printf("failed to set max phy\n");
    if (edf_set_physical_minimum(handle, i, 0) != 0)  printf("failed to set min phy\n");
    if (edf_set_digital_maximum(handle, i, 32767) != 0)  printf("failed to max dig\n");
    if (edf_set_digital_minimum(handle, i, 0) != 0)  printf("failed to set min dig\n");
  }

  if (d->blocking == 1)  {
    read_lock(d);
  }

  for (int i = 0; i < c; i++)  {
    if (edfwrite_physical_samples(handle, (d->buffer + i*c)) != 0)  {  //stride?
      fprintf(stderr, "data_write_edf: failed to write to file\n");
    }
  }

  edfclose_file(handle);

  if (d->blocking == 1)  {
    read_unlock(d);
  }

  return 1;
}

int data_read_edf(data *d, char* fn)  {
  return 1;
}

int data_read(data *d, FILE* f)  {
  //TODO
  return 1;
}
