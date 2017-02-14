#ifndef PIPEREGISTRY_H
#define PIPEREGISTRY_H

#include "pipe.h"
#include "linkedlist.h"
#include "hashtable.h"

//pipe names/descriptions?
typedef struct pipedes pipedes;  //pipe description
struct pipedes  {
  int(*init)(pipe_*, linkedlist*);
  int(*run)(pipe_*, linkedlist*);
  int(*kill)(pipe_*, linkedlist*);
  char* valid_inputs;  //valid input data, format "NAME1, NAME2, NAME3"
};

int piperegistry_init();
int piperegistry_register(char* name, int(*)(pipe_*, linkedlist*), int(*)(pipe_*, linkedlist*), int(*)(pipe_*, linkedlist*), char*);
int piperegistry_deregister(char*);
linkedlist *piperegistry_get_valid_inputs(char *name);
hashtable *piperegistry_ht();

#endif
