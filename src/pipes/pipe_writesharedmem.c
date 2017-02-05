#include "../pipe.h"
#include "../data.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <unistd.h> //DEBUG

struct writesharedmem_aux {
  key_t key;
  int shmid;
  double *shm;
};

int writesharedmem_init(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "writesharedmem_init: writesharedmem pipe must have an input\n");
    return -1;
  }
  p->output = NULL;  //writesharedmem pipe has no output
 

  struct writesharedmem_aux *aux = (struct writesharedmem_aux*)malloc(sizeof(struct writesharedmem_aux));

  int shmid;
  key_t key = 6667;
  int n = input->n; //number of dimensions
  int buf_len = data_size(input)/sizeof(double); //TODO??
  int mem_len = (1 + 1 + 1 + 2*n + buf_len)*sizeof(double);  //read + n + shape+stride + data
  double *shm; 
  //open and attach shared memory
  if ((shmid = shmget(key, mem_len, IPC_CREAT | 0666)) < 0)  {  //TODO permissions?
    fprintf(stderr, "writesharedmem_init: failed to create shared memory\n");
    return -1;
  }

  if ((shm = shmat(shmid, NULL, 0)) < 0)  {  
    fprintf(stderr, "writesharedmem_init: failed to attach shared memory\n"); 
    return -1;
  }

  aux->key = key;
  aux->shmid = shmid;
  aux->shm = shm;
  p->auxiliary = aux;

  return 1;
}

int writesharedmem_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);

  struct writesharedmem_aux *aux = (struct writesharedmem_aux*)p->auxiliary;

  int n = input->n;
  int *shape = input->shape;
  int *stride = input->stride;
  int len = data_size(input)/sizeof(double); //input->len instead??

  //wait for read?
 
  double *s = aux->shm;
  *s++ = 0; //not read
  *s++ = n;
  for (int i = 0; i < n; i++)  {
    *s++ = shape[i];
  }
  for (int i = 0; i < n; i++)  {
    *s++ = stride[i];
  }
  for (int i = 0; i < len; i++)  {
    *s++ = (double)input->buffer[i];
  }

  return 1; 
}

int writesharedmem_kill(pipe_* p, linkedlist* l)  {
  struct writesharedmem_aux *aux = (struct writesharedmem_aux*)p->auxiliary;

  shmdt(aux->shm);
  shmctl(aux->shmid, 0, NULL);
 
  return 1;
}
