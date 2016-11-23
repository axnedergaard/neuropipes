#include "pipeline.h"
#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "piperegistry.h"
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include "gettime.h"

#define INITIAL_MAX 8

static int next_id = 0;

struct pipeline {  //graph
  pipe_** nodes;  //critical: assume (entries[x] | 0 <= x < entries_n)
  int* sort; //in-order sort of nodes
  int nodes_n;
  int nodes_max;
  linkedlist **adjacency_list;  //adjacency list with ints
  linkedlist **in_data; //in data for every node
//  int iterater;
  int loop;
};

pipeline* pipeline_create()  {
  pipeline* pl = (pipeline*)malloc(sizeof(pipeline));
  if (pl == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for pipe");
    return NULL;
  }

  pl->nodes_max = INITIAL_MAX;
  pl->nodes_n = 0;

  pl->nodes = (pipe_**)malloc(sizeof(void*)*pl->nodes_max);
  if (pl->nodes == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for nodes");
    free(pl);
    return NULL;
  }

  pl->sort = (int*)malloc(sizeof(int)*pl->nodes_max);
  if (pl->nodes == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for sort");
    free(pl->nodes);
    free(pl);
    return NULL;
  }

  pl->adjacency_list = (linkedlist**)malloc(sizeof(linkedlist*)*pl->nodes_max);
  if (pl->nodes == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for adjacency list");
    free(pl->nodes);
    free(pl->sort);
    free(pl);
    return NULL;
  }

  pl->in_data = (linkedlist**)malloc(sizeof(linkedlist*)*pl->nodes_max);
  if (pl->nodes == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for in data");
    free(pl->nodes);
    free(pl->sort);
    free(pl->in_data);
    free(pl);
    return NULL;
  }
 // pl->iterater = 0;

  for (int i = 0; i < pl->nodes_max; i++)  {
    pl->nodes[i] = NULL;
    pl->sort[i] = -1;  //-1 = invalid
  }

  piperegistry_init();

  return pl;
}

int pipeline_destroy(pipeline* pl)  {  //excessive to check if null before destroying elements? TODO
  if (pl != NULL)  {
    for (int i = 0; i < pl->nodes_n; i++)  {
      if (pl->nodes[i] != NULL)  {
        if (pipe_destroy(pl->nodes[i]) == 0)  {
          return 0;
        }
      }
    }
    for (int i = 0; i < pl->nodes_max; i++)  {
      if (pl->adjacency_list[i] != NULL)  {
        if (linkedlist_destroy(pl->in_data[i]) == 0)  {
          return 0;
        }
      }
      if (pl->in_data[i] != NULL)  {
        if (linkedlist_destroy(pl->adjacency_list[i]) == 0)  {
          return 0;
        }
      }
    }
    free(pl->nodes);
    free(pl->sort);
    free(pl);
    return 1;
  }
  return 0;
}

int pipeline_size(pipeline* pl)  {
  return pl->nodes_n;
}

void pipeline_set_loop(pipeline* pl, int d)  {
  pl->loop = d;
}

int pipeline_get_loop(pipeline* pl)  {
  return pl->loop;
}

pipe_* pipeline_get_pipe(pipeline* pl, int id)  {
  if ((id >= 0) && (id < pl->nodes_n))  {
    return pl->nodes[id];
  }
  else  {
//    fprintf(stderr, "pipeline_get_pipe: pipe_ does not exist\n");
    return NULL;
  }
}

int pipeline_expand(pipeline* pl)  {
  if (realloc(pl->nodes, sizeof(pl->nodes)*2) == NULL)  {
    fprintf(stderr, "pipeline_expand: failed to realloc nodes\n");
    return 0;
  }
  if (realloc(pl->sort, sizeof(pl->sort)*2) == NULL)  {
    fprintf(stderr, "pipeline_expand: failed to realloc sort\n");
    return 0;
  }
  pl->nodes_max *= 2;
  return 1;
}

int pipeline_insert_edge(pipeline* pl, int u, int v)  {  //create edge u->v
  if (linkedlist_insert(pl->adjacency_list[u], (void*)(intptr_t)v) == 0)  {
    return 0;
  }
  if (linkedlist_insert(pl->in_data[v], &pl->nodes[u]->output) == 0)  {
    return 0;
  }
  return 1;   
}

int pipeline_remove_edge(pipeline* pl, int u, int v)  {  //TODO fix (not working proper)
  if (linkedlist_remove(pl->adjacency_list[u], (void*)(intptr_t)v) == 0)  {
    return 0;
  }
  if (linkedlist_remove(pl->in_data[u], &pl->nodes[v]->output) == 0)  {
    return 0;
  }
  return 1;
}

int pipeline_insert(pipeline* pl, char* type)  {
  pipe_* p = build_pipe(type);

  pipe_set_id(p, next_id++);

  if (pl->nodes_n >= pl->nodes_max)  {  
    pipeline_expand(pl);
  }

  pl->nodes[pl->nodes_n] = p;
  pl->adjacency_list[pl->nodes_n] = linkedlist_create();
  pl->in_data[pl->nodes_n] = linkedlist_create();

  pl->nodes_n++;  

  return pipe_get_id(p);  //return pipe_ id
}

int pipeline_remove(pipeline* pl, int pid)  {  //TODO (working?)
  pipe_* p = pipeline_get_pipe(pl, pid);
  if (p == NULL)  {
    fprintf(stderr, "pipeline_remove: pipe_ not in pipeline\n");
    return 0;
  }
  else  {
    pipe_* successor = NULL;
    while ((successor = linkedlist_iterate(pl->adjacency_list[pid])) != NULL)  {
      if (pipeline_remove_edge(pl, pid, pipe_get_id(successor)))  {
        fprintf(stderr, "pipeline_remove: failed to remove edge\n");
        return 0;
      } 
    } 
    pipe_destroy(p);
    return 1;
  }
}

int pipeline_sort(pipeline* pl)  {  //make ordered sort, filling in empty gaps?
  //make ordered sort...
  //for every unused source node (in edges = 0), reduce in edge of each successor node by 1, and make the successor node a source node if it has no more in edges
  int added = 0;
  int in[pl->nodes_n];  //in edges
  for (int i = 0; i < pl->nodes_n; i++)  {  //set in edges
    in[i] = linkedlist_size(pl->in_data[i]);  //use in_data instead of adjacency list for less computations...
    if (in[i] == 0)  {
      pl->sort[added++] = i;
    }
  }
  for (int i = 0; i < pl->nodes_n; i++)  {
    if (added <= i)  {  //did not finish (added should always be greater than i)
      fprintf(stderr, "pipeline_sort: was unable to order nodes, cycle exists or graph is not connected\n");
      return 0;
    } 
    
   int out_edge;
   while ((out_edge = (int)(intptr_t)linkedlist_iterate(pl->adjacency_list[i])) != 0)  {
     if (--in[out_edge] == 0)  {
       pl->sort[added++] = out_edge;
     }
   }
  }
  //sort completed

  //set remaining values as invalid
  for (int i = added; i < pl->nodes_n; i++)  {
    pl->sort[i] = -1;
  }
 
  return 1;
}

int pipeline_init(pipeline* pl)  {
//  printf("initing: n=%d\n", pl->nodes_n);
  pipeline_sort(pl);
  //pipe_s are now sorted 
  for (int i = 0; i < pl->nodes_n; i++)  {
  //  printf("initing pipe_ %p\n", pl->nodes[pl->sort[i]]);
    if (pipe_init(pl->nodes[pl->sort[i]], pl->in_data[pl->sort[i]]) != 1)  {
      fprintf(stderr, "pipeline_init: failed to init pipe %d\n", pl->sort[i]); 
      return 0;
    }
  } 
  return 1;
}

int pipeline_run(pipeline* pl)  {
  if (pipeline_size(pl) <= 0)  {  //remove???
    fprintf(stderr, "no pipe_s to run\n");
    return 0;
  }

  int quit = 0;
  double interval = 0;
  double total_time_before = get_clock_time(); 
  //double interval = 1.0;
  while (quit == 0)  {  //time sync?
    double pipeline_time_before = get_clock_time();
    for (int i = 0; i < pl->nodes_n; i++)  {
      debug_pipe *debug = pl->nodes[pl->sort[i]]->debug;
      debug_pipe_start_timer(debug);
      if (pipe_run(pl->nodes[pl->sort[i]], pl->in_data[pl->sort[i]]) != 1)  {
        fprintf(stderr, "pipeline_run: failed to run pipe %d\n", pl->sort[i]); 
        return 0;
      }
      debug_pipe_stop_timer(debug);     
      double pipe_time = debug_pipe_time(debug);
      printf("pipe %d ran in %fs\n", pl->sort[i], pipe_time);
    }
    if (pl->loop == 0)  {
      quit = 1;
    }
    else if (pl->loop > 0)  {
      pl->loop--;
    }
    double pipeline_time = get_clock_time() - pipeline_time_before;
    printf("pipeline ran in %fs\n", pipeline_time);
    double lag = interval - pipeline_time;
    if ((pl->loop != 0) && (lag > 0))  {
      printf("sleeping for %fs\n\n", lag);
      usleep((int)(lag*1000000));
    }
    else  {
      printf("\n");
    }
  }
  //print pipe averages run times
  for (int i = 0; i < pl->nodes_n; i++)  {
    double pipe_average_time = debug_pipe_average_time(pl->nodes[pl->sort[i]]->debug);
    fprintf(stdout, "pipe %d average run time: %fs\n", i, pipe_average_time);
  }
  //print total run time
  double total_time = get_clock_time() - total_time_before;
  fprintf(stdout, "total run time: %fs\n", total_time);

  return 1;
}

/*
pipe_* pipeline_iterate(pipeline* pl)  {
  //init iterater if appropriate
  if (pl->iterater < 0)  {  
    pl->iterater = 0;
  }
  if (pl->iterater < pl->nodes_n)  {  //more entries left
    return pl->nodes[pl->iterater];
  } 
  else  {  //iterater finished
    pl->iterater = -1;
    return NULL;
  }
}
*/
