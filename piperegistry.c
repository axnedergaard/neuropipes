#include "piperegistry.h"
#include "pipe.h"
#include "linkedlist.h"
#include "hashtable.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//TODO hashtable ht (char* -> struct) where struct contains init, run, and input_spec
hashtable* ht; //hashtable_create();

//pipe names/descriptions?
typedef struct pipedes pipedes;  //pipe description
struct pipedes  {
  int(*init)(pipe_*, linkedlist*);
  int(*run)(pipe_*, linkedlist*);
  char* valid_inputs;  //valid input data, format "NAME1, NAME2, NAME3"
};

//bad? macro?

int dummyemotiv_init(pipe_*, linkedlist*);
int dummyemotiv_run(pipe_*, linkedlist*);
int emotiv_init(pipe_*, linkedlist*);
int emotiv_run(pipe_*, linkedlist*);
int fouriertransform_init(pipe_*, linkedlist*);
int fouriertransform_run(pipe_*, linkedlist*);
int inversefouriertransform_init(pipe_*, linkedlist*);
int inversefouriertransform_run(pipe_*, linkedlist*);
int graph_init(pipe_*, linkedlist*);
int graph_run(pipe_*, linkedlist*);
int mean_init(pipe_*, linkedlist*);
int mean_run(pipe_*, linkedlist*);
int normalise_init(pipe_*, linkedlist*);
int normalise_run(pipe_*, linkedlist*);
int print_init(pipe_*, linkedlist*);
int print_run(pipe_*, linkedlist*);
int web_init(pipe_*, linkedlist*);
int web_run(pipe_*, linkedlist*);
int alphabandpass_init(pipe_*, linkedlist*);
int alphabandpass_run(pipe_*, linkedlist*);

int piperegistry_init()  {  //???
  ht = hashtable_create();
  if (ht == NULL)  {
    return 0;
  }
  piperegistry_register("DUMMYEMOTIV", &dummyemotiv_init, &dummyemotiv_run, "");
  piperegistry_register("EMOTIV", &emotiv_init, &emotiv_run, "");
  piperegistry_register("FOURIERTRANSFORM", &fouriertransform_init, &fouriertransform_run, "");
  piperegistry_register("INVERSEFOURIERTRANSFORM", &inversefouriertransform_init, &inversefouriertransform_run, "");
  piperegistry_register("GRAPH", &graph_init, &graph_run, "");
  piperegistry_register("MEAN", &mean_init, &mean_run, "");
  piperegistry_register("NORMALISE", &normalise_init, &normalise_run, "");
  piperegistry_register("PRINT", &print_init, &print_run, "");
  piperegistry_register("WEB", &web_init, &web_run, "WEB");
  piperegistry_register("ALPHABANDPASS", &alphabandpass_init, &alphabandpass_run, "");

  return 1;
} 

//destroy register?

int piperegistry_register(char *name, int(*init)(pipe_*, linkedlist*), int(*run)(pipe_*, linkedlist*), char *valid_inputs)  {
  pipedes *pd = (pipedes*)malloc(sizeof(pipedes));
  if (pd == NULL)  {
    fprintf(stderr, "register_pipe: failed to allocate memory\n");
    return 0;
  }
  pd->init = init;
  pd->run = run;
  pd->valid_inputs = strdup(valid_inputs);  //dangerous?
  if (hashtable_insert(ht, name, (void*)pd) == 0)  {  //name already in table
    free(pd->valid_inputs);
    free(pd);
    fprintf(stderr, "register pipe: failed to register pipe %s\n", name);
    return 0;
  }
  return 1;
}

int piperegistry_deregister(char* name)  {
  pipedes *pd = (pipedes*)hashtable_lookup(ht, name);
  if (pd == NULL)  {
    fprintf(stderr, "deregister_pipe: failed to lookup name\n");
    return 0;
  }
  hashtable_remove(ht, name);
  free(pd->valid_inputs); //?
  free(pd);
  return 1;
}

//return list of valid input data (char*) 
linkedlist* piperegistry_get_valid_inputs(char *name)  {
  return NULL;
}

pipe_* build_pipe(char *name)  {  //move somewhere else? pipefactory-like...
  pipedes* pd = (pipedes*)hashtable_lookup(ht, name);
  if (pd == NULL)  {
    fprintf(stderr, "create_pipe: failed to lookup name %s\n", name);
    return NULL;
  }
  pipe_* p = pipe_create();  //TODO check alloc
  p->init = pd->init;
  p->run = pd->run;
  return p;
}
