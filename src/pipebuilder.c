#include "pipebuilder.h"
#include "piperegistry.h"
#include "parameters.h"
#include <string.h>
#include <stdlib.h>

pipe_* build_pipe(char *spec, int concurrent)  {
  //tokenise spec    
  int params_n = 0;
  char **params = NULL;
  char *type = NULL;
  tokenise(spec, &type, &params_n, &params);

  pipedes* pd = (pipedes*)hashtable_lookup(piperegistry_ht(), type);
  if (pd == NULL)  {
    fprintf(stderr, "create_pipe: failed to lookup name %s\n", type);
    return NULL;
  }
  pipe_* p = pipe_create();  //TODO check alloc
  p->init = pd->init;
  p->run = pd->run;
  p->kill = pd->kill;
  p->concurrent = concurrent;  //TODO proper
  p->params_n = params_n;
  p->params = params;
  
  return p;
}

