#ifndef PIPEREGISTRY_H
#define PIPEREGISTRY_H

#include "pipe.h"
#include "linkedlist.h"
#include "hashtable.h"

//pipe names/descriptions?
typedef struct pipedes pipedes;  //pipe descriptor
struct pipedes  {
  int(*init)(pipe_*, linkedlist*);
  int(*run)(pipe_*, linkedlist*);
  int(*kill)(pipe_*, linkedlist*);
  char* description; 
};

//destroy?
int piperegistry_init();
int piperegistry_deinit();
int piperegistry_register(char* name, int(*)(pipe_*, linkedlist*), int(*)(pipe_*, linkedlist*), int(*)(pipe_*, linkedlist*), char*);
int piperegistry_deregister(char*);
hashtable *piperegistry_ht();
int list_available_pipes(char***);

#endif
