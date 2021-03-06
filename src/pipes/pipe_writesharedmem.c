#include "../pipe.h"
#include "../parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define DLEN 32 //length of each double in shared memory

struct writesharedmem_aux {
  key_t key;
  int shmid;
  char *shm;
  char *buffer; //string containing doubles to be written
};

int writesharedmem_init(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "writesharedmem_init: writesharedmem pipe must have an input\n");
    return -1;
  }
  struct writesharedmem_aux *aux = (struct writesharedmem_aux*)malloc(sizeof(struct writesharedmem_aux));
  pipe_set_auxiliary(p, aux);

  int shmid;
  key_t key = 6667;
  int n = data_get_n(input); //number of dimensions
  int data_len = data_get_len(input);
  int mem_len = (1 + 2*n + data_len)*DLEN;  //n_dimensions + shape+stride + data
  char *shm;
  char *buffer = (char*)malloc(sizeof(char)*DLEN);
  set_parameter_int(p, "key", &key);

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
  aux->buffer = buffer;

  return 1;
}

int writesharedmem_run(pipe_ *p, linkedlist *l)  {
  data *input = *(data**)linkedlist_head(l);
  if (input == NULL)  {
    fprintf(stderr, "writesharedmem_run: pipe must have an input\n");
  }
  struct writesharedmem_aux *aux = (struct writesharedmem_aux*)pipe_get_auxiliary(p);

  int *shape, *stride;
  int n = data_spec(input, &shape, &stride);
  int data_len = data_get_len(input);
  double *input_buffer = data_get_buffer(input);
  char *s = aux->shm;
  char *buffer = aux->buffer;
  
  //write to memory
  read_lock(input);
 
  //n
  sprintf(buffer, "%f", (double)n);
  for (int i = 0; i < DLEN; i++)  {
    *s++ = buffer[i];
  }
  //shape
  for (int i = 0; i < n; i++)  {
    sprintf(buffer, "%f", (double)shape[i]);
    for (int j = 0; j < DLEN; j++)  {
      *s++ = buffer[j];
    }
  }
  //stride
  for (int i = 0; i < n; i++)  {
    sprintf(buffer, "%f", (double)stride[i]);
    for (int j = 0; j < DLEN; j++)  {
      *s++ = buffer[j];
    }
  }
  //data
  for (int i = 0; i < data_len; i++)  {
    sprintf(buffer, "%f", (double)input_buffer[i]);
    for (int j = 0; j < DLEN; j++)  {
      *s++ = buffer[j];
    }
  }
  
  read_unlock(input);  

  return 1; 
}

int writesharedmem_kill(pipe_* p, linkedlist* l)  {
  struct writesharedmem_aux *aux = (struct writesharedmem_aux*)pipe_get_auxiliary(p);
  shmdt(aux->shm);
  shmctl(aux->shmid, 0, NULL);
  free(aux->buffer);
  return 1;
}
