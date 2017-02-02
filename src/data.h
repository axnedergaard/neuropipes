#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <pthread.h>

#define TYPE_REAL 0
#define TYPE_COMPLEX 1

//len==shape[0]*...*shape[n] must hold
//number of elements in dimension i is shape(i)/stride(i) where 1 <= stride(i) <= shape(i)
typedef struct data data;
struct data {
  double *buffer;  //memory
  int len; //length
  int n; //number of dimensions
  int *shape; //sizes of each dimensions
  int *stride;  //strides of each dimension
  //blocking params, make into struct? TODO
  int blocking;
  pthread_mutex_t mutex;
  pthread_cond_t cond_written;
  pthread_cond_t cond_read;
  int writes;  //TODO better naming?
  int reads;
  int readers;
};

int data_type(data*);  //get type (e.g. real or complex) from data
int data_size(data*);
int data_ready(data*);
int data_blocking(data*);
void data_broadcast_read(data*);
data *data_create(int, int*, int*);
data *data_create_from_string(char*);
data *data_create_complex_from_real(data*);
data *data_create_real_from_complex(data*);
data *data_create_from(data*);
data *data_create_from_edf(char*, int*, int*, int*);
int data_make_blocking(data*);
int data_unblock(data*);
void read_lock(data*);
void write_lock(data*);
void read_unlock(data*);
void write_unlock(data*);
void data_reset_reads(data*);
void data_increment_reads(data*);
void data_increment_readers(data*);
void data_reset_readers(data*);
int data_destroy(data*);
int data_copy_from_data_blocking(data*, double*);
int data_copy_to_data_blocking(data*, double*);
int data_copy_from_data(data*, double*);
int data_copy_to_data(data*, double*);
int data_copy_to_data_complex_to_real(data*, double*);
int data_copy_from_data_real_to_complex(data*, double*);
int data_copy_from_data_complex_to_real(data*, double*);
int data_write(data*, FILE*);
int data_read(data*, FILE*);
int data_edf_open(data*, char*);
int data_edf_write(data*, int);
int data_edf_read(data*, int);
int data_edf_close(int);

#endif
