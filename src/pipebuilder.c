#include "pipebuilder.h"
#include "piperegistry.h"

pipe_* build_pipe(char *name)  {  //move somewhere else? pipefactory-like...
  pipedes* pd = (pipedes*)hashtable_lookup(ht, name);
  if (pd == NULL)  {
    fprintf(stderr, "create_pipe: failed to lookup name\n");
    return NULL;
  }
  pipe_* p = pipe_create();  //TODO check alloc
  p->init = pd->init;
  p->run = pd->run;
  return p;
}
