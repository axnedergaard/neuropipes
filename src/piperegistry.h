#ifndef PIPEREGISTRY_H
#define PIPEREGISTRY_H

#include "pipe.h"
#include "linkedlist.h"

int piperegistry_init();
int piperegistry_register(char* name, int(*)(pipe_*, linkedlist*), int(*)(pipe_*, linkedlist*), int(*)(pipe_*, linkedlist*), char*);
int piperegistry_deregister(char*);
linkedlist *piperegistry_get_valid_inputs(char *name);
pipe_ *build_pipe(char*, int);

#endif
