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
  pthread_mutex_t mutex;
};

int data_get_type(data*);  //get type (e.g. real or complex) from data
int data_size(data*);
data *data_create(int, int*, int*);
data *data_create_from_string(char*);
data *data_create_complex_from_real(data*);
data *data_create_real_from_complex(data*);
int data_destroy(data*);
int data_copy_from_data(data*, double*);
int data_copy_to_data(data*, double*);
int data_copy_to_data_complex_to_real(data*, double*);
int data_copy_from_data_real_to_complex(data*, double*);
int data_copy_from_data_complex_to_real(data*, double*);
int data_write(data*, FILE*);
int data_read(data*, FILE*);

#endif
