#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <pthread.h>
#include "linkedlist.h"

#define TYPE_REAL 0
#define TYPE_COMPLEX 1

typedef struct data data;
struct data;

data *data_create(int, int*, int*);
data *data_create_from_string(char*);
data *data_create_complex_from_real(data*);
data *data_create_real_from_complex(data*);
data *data_create_from(data*);
int data_destroy(data*);

int data_copy_from_data(data*, double*);
int data_copy_to_data(data*, double*);
int data_copy_to_data_complex_to_real(data*, double*);
int data_copy_from_data_real_to_complex(data*, double*);
int data_copy_from_data_complex_to_real(data*, double*);

int data_write(data*, FILE*);

int data_make_blocking(data*);
int data_unblock(data*);
void read_lock(data*);
void write_lock(data*);
void read_unlock(data*);
void write_unlock(data*);
void data_increment_readers(data*);
void data_broadcast_read(data*);

int data_type(data*);  //get type (e.g. real or complex) from data
int data_size(data*);  //get size of buffer
int data_spec(data*, int**, int**);  //return n and set params to shape and stride

int data_get_len(data*);
int data_get_n(data*);
double *data_get_buffer(data*);
int *data_get_stride(data*);
int *data_get_shape(data*);
int data_get_blocking(data*);
int data_get_kill(data*);  //testing purposes...


#endif
