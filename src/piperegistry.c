#include "piperegistry.h"
#include "pipe.h"
#include "linkedlist.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "register_pipes.h"

hashtable *ht;
linkedlist *keys;  //TODO more elegant solution using uthash?

int piperegistry_init()  {  //???
  ht = hashtable_create();
  if (ht == NULL)  {
    return -1;
  }
  keys = linkedlist_create();
  if (keys == NULL)  {
    return -1;
  }

  register_pipes();
  
  return 1;
}

int piperegistry_deinit()  {	
  char **keys_array = NULL;
  int keys_n = list_available_pipes(&keys_array);
  for (int i = 0; i < keys_n; i++)  {
    pipedes *pd = (pipedes*)hashtable_lookup(ht, keys_array[i]);
    free(pd->description);
    free(pd);
    hashtable_remove(ht, keys_array[i]);
  }
  free(keys_array);  
  hashtable_destroy(ht);
  linkedlist_destroy(keys);
  return 1;
}

int piperegistry_register(char *name, int(*init)(pipe_*, linkedlist*), int(*run)(pipe_*, linkedlist*), int(*kill)(pipe_*, linkedlist*), char *description)  {
  pipedes *pd = (pipedes*)malloc(sizeof(pipedes));
  if (pd == NULL)  {
    fprintf(stderr, "register_pipe: failed to allocate memory\n");
    return 0;
  }
  pd->init = init;
  pd->run = run;
  pd->kill = kill;
  pd->description = (char*)malloc(sizeof(char)*strlen(description));
  strncpy(pd->description, description, strlen(description));
  if (hashtable_insert(ht, name, (void*)pd) == 0)  {  //name already in table
    free(pd->description);
    free(pd);
    fprintf(stderr, "register pipe: failed to register pipe %s\n", name);
    return 0;
  }

  linkedlist_insert(keys, (void*)name);

  return 1;
}

int piperegistry_deregister(char* name)  {
  pipedes *pd = (pipedes*)hashtable_lookup(ht, name);
  if (pd == NULL)  {
    fprintf(stderr, "deregister_pipe: failed to lookup name\n");
    return 0;
  }
  hashtable_remove(ht, name);
  free(pd->description);
  free(pd);

  linkedlist_remove(keys, name);

  return 1;
}

//return list of valid input data (char*) TODO
//linkedlist* piperegistry_get_description(char *name)  {
//  return NULL;
//}

hashtable *piperegistry_ht()  {
  return ht;
}

int list_available_pipes(char*** list)  {  //TODO warning: requires piperegistry_init to have been called
  int n = linkedlist_size(keys);
  *list = (char**)malloc(sizeof(char*)*n);
  for (int i = 0; i < n; i++)  {
    (*list)[i] = linkedlist_iterate(keys);
  }
  return n;
}
