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
  pipe_set_init(p, pd->init);
  pipe_set_run(p, pd->run);
  pipe_set_kill(p, pd->kill);
  pipe_set_concurrent(p, concurrent);
  pipe_set_params_n(p, params_n);
  pipe_set_params(p, params);

//  printf("built pipe %p with spec %s\n", p, spec);

  return p;
}

