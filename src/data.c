#include "data.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_THREADS 2 //TODO 1 invalid?

//len==shape[0]*...*shape[n] must hold
//number of elements in dimension i is shape(i)/stride(i) where 1 <= stride(i) <= shape(i)
struct data {
  double *buffer;  //memory
  int len; //length
  int n; //number of dimensions
  int *shape; //sizes of each dimensions
  int *stride;  //strides of each dimension
  
  int blocking;
  pthread_mutex_t mutex;
  pthread_cond_t cond_written;
  pthread_cond_t cond_read;
  int writes; 
  int reads;
  int readers;
  pthread_t **threads;
  int threads_n;
  int threads_max;
  int *thread_reads;
  int *thread_readers;
  int kill;
};

data* data_create(int n, int *shape, int *stride)  {
  data *d = (data*)malloc(sizeof(data));
  if (d == NULL)  {
    fprintf(stderr, "data_create: failed to allocate memory for struct\n");
    return NULL;  //mem alloc failed
  }
  d->n = n;
  d->shape = (int*)malloc(sizeof(int)*d->n);
  if (d->shape == NULL)  {
    fprintf(stderr, "data_create: failed to allocate memory for shape\n");
    return NULL;
  }
  d->stride = (int*)malloc(sizeof(int)*d->n);
  if (d->stride == NULL)  {
    fprintf(stderr, "data_create: failed to allocate memory for stride\n");
    return NULL;
  }
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

  d->reads = 0;
  d->readers = 0; 
  d->writes = 0;
  d->threads_max = 0;
  d->threads_n = 0;
  d->threads = NULL;
  d->thread_reads = NULL;
  d->thread_readers = NULL;
  d->kill = 0;
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
  if (d->blocking)  {
    pthread_mutex_destroy(&d->mutex);
    pthread_cond_destroy(&d->cond_written);
    pthread_cond_destroy(&d->cond_read);
    free(d->threads);
    free(d->thread_reads);
    free(d->thread_readers);

  }
  free(d->buffer);
  free(d->shape);
  free(d->stride);
  free(d);
  return 1;
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
 
  read_lock(d);
 
  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      buf[(i*n + j)*2] = d->buffer[i*n + j];
    }
  }
  
  read_unlock(d);

  return 1;
}

int data_copy_to_data_complex_to_real(data *d, double *buf)  {  //TODO blocking
  int c = d->shape[0];
  int n = d->shape[1];

  write_lock(d);

  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      d->buffer[i*n + j] = buf[(i*n + j)*2];  //ignore complex values
    }
  }

  write_unlock(d);

  return 1;
}

int data_copy_to_data(data *d, double *buf)  {
  int len = 1;
  for (int i = 0; i < d->n; i++)  {
    len *= d->shape[i];
  }

  write_lock(d);

  for (int i = 0; i < len; i++)  {
    d->buffer[i] = buf[i];
 //   printf("%f ", d->buffer[i]);
  }

  write_unlock(d);
  
  return 1;
}

int data_copy_from_data(data *d, double *buf)  {
  int len = 1;
  for (int i = 0; i < d->n; i++)  {
    len *= d->shape[i];
  }

  read_lock(d);

  for (int i = 0; i < len; i++)  {
    buf[i] = d->buffer[i];
    //buf + i*8) = d->buffer[i];
  }
  
  read_unlock(d);
  
  return 1; 
}

int data_write(data *d, FILE* f)  { 
  int c = d->shape[0];
  int n = d->shape[1];

  read_lock(d);

  for (int i = 0; i < c; i++)  {
    for (int j = 0; j < n; j++)  {
      float reading = d->buffer[i*n + j];  
      fprintf(f, "%.3f ", reading);
    }
    fprintf(f, "\n");
  }

  read_unlock(d);

  return 1;
}

int data_make_blocking(data *d)  {  //make data blocking
  pthread_mutex_init(&d->mutex, NULL);  //TODO check init success
  pthread_cond_init(&d->cond_read, NULL);
  pthread_cond_init(&d->cond_written, NULL);
  d->blocking = 1;
  d->reads = 0;
  d->readers = 0; 
  d->writes = 0;
  d->threads_max = INITIAL_THREADS;
  d->threads_n = 0;
  d->kill = 0;
  d->threads = (pthread_t**)malloc(sizeof(pthread_t*)*d->threads_max);
  d->thread_reads = (int*)malloc(sizeof(int)*d->threads_max);
  d->thread_readers = (int*)malloc(sizeof(int)*d->threads_max);
  return 1;
}

void data_increment_readers(data *d, pthread_t *t)  {
  d->readers++;
  int new_thread = 1;
  for (int i = 0; i < d->threads_n; i++)  {
    if (t == d->threads[i])  {  //thread already registered
      new_thread = 0;
      d->thread_readers[i]++;
    }
  }
  if (new_thread == 1)  {  //register new thread
    if (d->threads_n >= d->threads_max)  {  //expand if needed
      d->threads_max *= 2;
      pthread_t **threads_new = realloc(d->threads, sizeof(pthread_t*)*d->threads_max);
      d->threads = threads_new;
    }
    d->threads[d->threads_n] = t;
    d->thread_readers[d->threads_n] = 1;
    d->thread_reads[d->threads_n] = 0;
    d->threads_n++;
  }
}

int data_allow_read(data *d)  {
  int index = data_thread_index(d);
  if (d->thread_reads[index] < d->thread_readers[index])  {  //more reads allowed for thread
    return 1;
  }
  else  {  //no more reads allowed for thread
    return 0;
  }
}

int data_thread_index(data *d)  {
  pthread_t t = pthread_self();
  for (int i = 0; i < d->threads_n; i++)  {
    if (pthread_equal(t, *d->threads[i]) != 0)  {  //thread matched
      return i;
    }
  }
  return -1; //not matched (error)
}

void data_reset_reads(data *d)  {
  d->reads = 0;
  for (int i = 0; i < d->threads_n; i++)  {
    d->thread_reads[i] = 0;
  }
}

void data_kill(data *d) { 
  pthread_mutex_lock(&d->mutex);
  d->kill = 1;
  data_unblock(d);
  pthread_mutex_unlock(&d->mutex);
}

int data_unblock(data *d)  {
  pthread_cond_broadcast(&d->cond_written);
  pthread_cond_broadcast(&d->cond_read);
  return 1;
}

void read_lock(data *d)  {
  if (d->blocking)  {
    pthread_mutex_lock(&d->mutex); 
    while ((d->writes == 0) || (data_allow_read(d) == 0))  {
      if (d->kill == 1)  {
	pthread_mutex_unlock(&d->mutex);
        pthread_exit(NULL);
      }
      pthread_cond_wait(&d->cond_written, &d->mutex);
    }
  }
}

void write_lock(data *d)  {
  if (d->blocking)  {
    pthread_mutex_lock(&d->mutex);
    while ((d->writes > 0)  && (d->reads < d->readers))  {
      if (d->kill == 1)  {
	pthread_mutex_unlock(&d->mutex);
        pthread_exit(NULL);
      }
      pthread_cond_wait(&d->cond_read, &d->mutex);
    }
  }
}

void write_unlock(data *d)  {
  if (d->blocking)  {
    d->writes++;
    data_reset_reads(d);
    pthread_cond_broadcast(&d->cond_written);
    pthread_mutex_unlock(&d->mutex);
  }
}

void read_unlock(data *d)  {
  if (d->blocking)  {
    d->reads++;
    int index = data_thread_index(d);
    d->thread_reads[index]++;
    if (d->reads >= d->readers)  {
      d->writes = 0;
      pthread_cond_broadcast(&d->cond_read);
    }
    pthread_mutex_unlock(&d->mutex); 
  }
}


void data_broadcast_read(data *d)  {
  pthread_cond_broadcast(&d->cond_read);
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

int data_spec(data *d, int **shape, int **stride)  {  //return n and set shape, stride pointers
  if (shape != NULL)  {
    *shape = d->shape;
  }
  if (stride != NULL)  {
    *stride = d->stride;
  }
  return d->n;
}

int data_get_len(data *d)  {
  return d->len;
}

int data_get_n(data *d)  {
  return d->n;
}

int *data_get_shape(data *d)  {
  return d->shape;
}

int *data_get_stride(data *d)  {
  return d->stride;
}

double *data_get_buffer(data *d)  {
  return d->buffer;
}

int data_get_blocking(data *d)  {
  return d->blocking;
}

