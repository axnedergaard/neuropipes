#include "../pipe.h"
#include "../data.h"

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
  int data_len = input->len;
  int mem_len = (1 + 2*n + data_len)*DLEN;  //n_dimensions + shape+stride + data
  char *shm; 

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
  int data_len = input->len;

  //wait for read?
 
  //WRITE
  char *s = aux->shm;
  char *buffer = (char*)malloc(sizeof(char)*DLEN); //string containing doubles to be written
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
    sprintf(buffer, "%f", (double)input->buffer[i]);
    for (int j = 0; j < DLEN; j++)  {
      *s++ = buffer[j];
    }
  }
 
  free(buffer);

  return 1; 
}

int writesharedmem_kill(pipe_* p, linkedlist* l)  {
  struct writesharedmem_aux *aux = (struct writesharedmem_aux*)p->auxiliary;

  shmdt(aux->shm);
  shmctl(aux->shmid, 0, NULL);
 
  return 1;
}
