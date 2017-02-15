#include "pipeline.h"
#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "pipebuilder.h"
#include "piperegistry.h"
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include "gettime.h"
#include <string.h>
#include <signal.h>

#define INITIAL_MAX 8

static int next_id = 0;
static int quit = 0;

void sig_handler(int signo)  {
  if (signo == SIGINT)  {
    quit = 1;
  }
}

struct pipeline {  //graph
  pipe_ **nodes;  //critical: assume (entries[x] | 0 <= x < entries_n)
  int *sort; //in-order sort of nodes
  int *nc_sort;  //in-order sort of non-concurrent nodes
  int *c_sort;  //in-order sort of concurrent nodes
  int nc_n; //number of non-concurrent nodes
  int nodes_n;
  int nodes_max;
  linkedlist **adjacency_list;  //adjacency list with ints
  linkedlist **in_data; //in data for every node
  int loop;
  double interval;
};

pipeline* pipeline_create()  {
  pipeline* pl = (pipeline*)malloc(sizeof(pipeline));
  if (pl == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for pipe");
    return NULL;
  }

  pl->nodes_max = INITIAL_MAX;
  pl->nodes_n = 0;
  pl->nc_n = 0;

  pl->nodes = (pipe_**)malloc(sizeof(pipe_*)*pl->nodes_max);
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

  pl->nc_sort = NULL;
  pl->c_sort = NULL;

  for (int i = 0; i < pl->nodes_max; i++)  {
    pl->nodes[i] = NULL;
    pl->sort[i] = -1;  //-1 = invalid
  }

  pl->loop = 1;
  pl->interval = 0;

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
    for (int i = 0; i < pl->nodes_n; i++)  {
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
    free(pl->nc_sort);
    free(pl->c_sort);
    free(pl);
    return 1;
  }
  return 0;
}

int pipeline_size(pipeline *pl)  {
  return pl->nodes_n;
}

void pipeline_set_loop(pipeline *pl, int d)  {
  pl->loop = d;
}

int pipeline_get_loop(pipeline *pl)  {
  return pl->loop;
}

void pipeline_set_interval(pipeline *pl, double d)  {
  pl->interval = d;
}

pipe_ *pipeline_get_pipe(pipeline *pl, int id)  {
  if ((id >= 0) && (id < pl->nodes_n))  {
    return pl->nodes[id];
  }
  else  {
//    fprintf(stderr, "pipeline_get_pipe: pipe_ does not exist\n");
    return NULL;
  }
}

int pipeline_expand(pipeline *pl)  {
  pl->nodes_max *= 2;
  pipe_ **nodes_new = realloc(pl->nodes, sizeof(pipe_*)*pl->nodes_max);
  if (nodes_new == NULL)  {
    fprintf(stderr, "pipeline_expand: failed to realloc nodes\n");
    return 0;
  }
  pl->nodes = nodes_new;
  int *sort_new = realloc(pl->sort, sizeof(int)*pl->nodes_max);
  if (sort_new == NULL)  {
    fprintf(stderr, "pipeline_expand: failed to realloc sort\n");
    return 0;
  }
  pl->sort = sort_new;
  linkedlist **adjacency_list_new = realloc(pl->adjacency_list, sizeof(int)*pl->nodes_max);
  if (adjacency_list_new == NULL)  {
    fprintf(stderr, "pipeline_expand: failed to realloc adjacency_list\n");
    return 0;
  }
  pl->adjacency_list = adjacency_list_new;
  linkedlist **in_data_new = realloc(pl->in_data, sizeof(int)*pl->nodes_max);
  if (in_data_new == NULL)  {
    fprintf(stderr, "pipeline_expand: failed to realloc in_data\n");
    return 0;
  }
  pl->in_data = in_data_new;
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

int pipeline_insert(pipeline* pl, char* spec, int concurrent)  {
  pipe_* p = build_pipe(spec, concurrent);

  pipe_set_id(p, next_id++);

  if (pl->nodes_n >= pl->nodes_max)  {  
    pipeline_expand(pl);
  }

  pl->nodes[pl->nodes_n] = p;
  pl->adjacency_list[pl->nodes_n] = linkedlist_create();
  pl->in_data[pl->nodes_n] = linkedlist_create();

  pl->nodes_n++;  

  if (concurrent == 0)  {
    pl->nc_n++;
  }

  return pipe_get_id(p);  //return pipe_ id
}

int pipeline_sort(pipeline* pl)  { 
  //in-order sort: for every unused source node (in edges = 0), reduce in edge of each successor node by 1, and make the successor node a source node if it has no more in edges (Dijkstra)
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

  //separate into concurrent and non-concurrent
  pl->nc_sort = (int*)malloc(sizeof(int)*pl->nc_n);
  pl->c_sort = (int*)malloc(sizeof(int)*(pl->nodes_n - pl->nc_n));

  int nc_added = 0;
  int c_added = 0;

  for (int i = 0; i < added; i++)  {  //for every sorted node
    if (pipe_get_concurrent(pl->nodes[pl->sort[i]]) == 1)  {  //if concurrent
      pl->c_sort[c_added++] = pl->sort[i];  //add to concurrent sorted
    }
    else  {  //else add to non-concurrent sorted
      pl->nc_sort[nc_added++] = pl->sort[i];
    }
  }

  return 1;
}

int pipeline_init(pipeline* pl)  {
//  printf("initing: n=%d\n", pl->nodes_n);
  pipeline_sort(pl);
  //pipes are now sorted 
  for (int i = 0; i < pl->nodes_n; i++)  {
  //  printf("initing pipe_ %p\n", pl->nodes[pl->sort[i]]);
    linkedlist* in_data = pl->in_data[pl->sort[i]];
    if (pipe_init(pl->nodes[pl->sort[i]], in_data) != 1)  {
      fprintf(stderr, "pipeline_init: failed to init pipe %d\n", pl->sort[i]); 
      return 0;
    }
  } 
  return 1;
}

int pipeline_run(pipeline* pl)  {
  if (pipeline_size(pl) <= 0)  {  //remove???
    fprintf(stderr, "no pipes to run\n");
    return 0;
  }

  double total_time_before = get_clock_time(); 

  //run concurrent pipes
  for (int i = 0; i < (pl->nodes_n - pl->nc_n); i++)  { 
    pipe_ *p = pl->nodes[pl->c_sort[i]];
    concurrent_pipe_start(p->concurrent_pipe, p, pl->in_data[pl->c_sort[i]], &quit);
  }

  //run non-concurrent pipes
  while (quit == 0)  {  //time sync?
    if (signal(SIGINT, sig_handler) == SIG_ERR)  {
      fprintf(stderr, "pipeline_run: failed to catch sig\n");
    }
    if (pl->loop == 1)  {
      quit = 1;
    }
    double pipeline_time_before = get_clock_time();
    for (int i = 0; i < pl->nc_n; i++)  {
      debug_pipe *debug = pl->nodes[pl->nc_sort[i]]->debug;
      debug_pipe_start_timer(debug);
      int status = pipe_run(pl->nodes[pl->nc_sort[i]], pl->in_data[pl->nc_sort[i]]);       
      if (status < 0)  {
        fprintf(stderr, "pipeline_run: failed to run pipe %d\n", pl->nc_sort[i]); 
        return 0;
      }
      else if (status == 0)  {  //pipe called pipeline termination
        quit = 1;
      } 
      
      debug_pipe_stop_timer(debug);     
      double pipe_time = debug_pipe_time(debug);
      printf("pipe %d ran in %fs\n", pl->nc_sort[i], pipe_time);
    }
    
    double pipeline_time = get_clock_time() - pipeline_time_before;
    printf("pipeline ran in %fs\n", pipeline_time);
    double lag = pl->interval - pipeline_time;
    if ((pl->loop != 1) && (lag > 0))  {
      printf("sleeping for %fs\n\n", lag);
      usleep((int)(lag*1000000));
    }
    else  {
      printf("\n");
    }

    if (pl->loop != 0)  {
      pl->loop--;
    }
  }

  for (int i = 0; i < (pl->nodes_n - pl->nc_n); i++)  {
    pipe_* p = pl->nodes[pl->c_sort[i]];
    concurrent_pipe_stop(p->concurrent_pipe);  //send signal to stop thread
    if (p->output != NULL)  {
      data_unblock(p->output);
    }
    pthread_join(*(concurrent_pipe_thread(p->concurrent_pipe)), NULL); //wait for thread to stop 
  }

  //kill concurrent pipes, print pipe averages run times
  for (int i = 0; i < pl->nodes_n; i++)  {
    pipe_* p = pl->nodes[pl->sort[i]];
    double pipe_average_time = debug_pipe_average_time(p->debug);
    fprintf(stdout, "pipe: %d average run time=%fs, times run=%d\n", i, pipe_average_time, debug_pipe_get_times_run(p->debug));
    pipe_kill(p, pl->in_data[pl->sort[i]]);  //TODO check for fail?
  }

  //print total run time
  double total_time = get_clock_time() - total_time_before;
  fprintf(stdout, "total run time: %fs\n", total_time);

  return 1;
}
