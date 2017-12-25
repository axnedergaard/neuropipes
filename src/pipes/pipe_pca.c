#include "../pipe.h"
#include "../parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//pca pipe used for testing

struct pca_aux {
  int pc_n;
};

int pca_init(pipe_* p, linkedlist* l)  {  //assume 2D
  data *input = *(data**)linkedlist_head(l);
  if (input == 0)  {
    return -1;
  }
  
  int *input_shape, *input_stride;
  int input_n = data_spec(input, &input_shape, &input_stride); 
  int shape[input_n]; 
  shape[0] = input_shape[0];
  shape[1] = input_shape[1];
  
  set_parameter_int(p, "pc_n", &shape[0]);  //TODO order of operations for &shape?
 
  if (shape[0] > input_shape[0])  {
    fprintf(stderr, "pca_init: number of principal components must be less than number of channels");
    return -1;
  }

  data *output = data_create(input_n, shape, input_stride); 

  pipe_set_output(p, output);

  struct pca_aux *aux = (struct pca_aux*)malloc(sizeof(struct pca_aux)); 

  aux->pc_n = shape[0];

  pipe_set_auxiliary(p, aux);
  
  return 1;
}

int pca_run(pipe_* p, linkedlist* l)  {  
  data *input = *(data**)linkedlist_head(l);
  data *output = pipe_get_output(p);
  struct pca_aux *aux = (struct pca_aux*)pipe_get_auxiliary(p);

  //center data
  //calculate covariance matrix
  //calculate eigenvectors
  //discard eigenvectors
  //multiply into data

  return 1; 
}

int pca_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
