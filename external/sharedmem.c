#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define DLEN 32

int main()  {
  int shmid;
  key_t key = 42;
  char *shm;
  char *s;

  if ((shmid = shmget(key, 0, 0)) < 0)  printf("shmget failed\n");

  if ((shm = shmat(shmid, NULL, 0)) == (char*)-1)  printf("shmat failed\n");

  struct shmid_ds ds;
  if (shmctl(shmid, IPC_STAT, &ds) < 0)  printf("shmctl stat failed\n");

  char buffer[DLEN];  //buffer for one double
  int data_n = ds.shm_segsz / DLEN;
  double data[data_n]; 

  s = shm;

  //read n
  for (int i = 0; i < DLEN; i++)  {
    buffer[i] = *s++;
  }  
  int n = atof(buffer);
  int shape[n];
  int stride[n];
  //read shape
  for (int i = 0; i < n; i++)  {
    for (int j = 0; j < DLEN; j++)  {
      buffer[j] = *s++; 
    }
    shape[i] = atof(buffer);
  }
  //read stride
  for (int i = 0; i < n; i++)  {
    for (int j = 0; j < DLEN; j++)  {
      buffer[j] = *s++; 
    }
    stride[i] = atof(buffer);
  }
  //read data
  for (int i = 0; i < data_n; i++)  {
    for (int j = 0; j < DLEN; j++)  {
      buffer[j] = *s++; 
    }
    data[i] = atof(buffer);
  }

  int k = 0;
  for (int i = 0; i < shape[0]; i++)  {
    for (int j = 0; j < shape[1]; j++)  {
      printf("%f ", data[i*shape[1] + j]);
    }
    printf("\n");
  }

  if (shmdt(shm) < 0)  printf("shmdt failed\n");

  return 0;
}
