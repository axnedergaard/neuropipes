#include "pipeline.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include "gettime.h"
#include <string.h>
#include <signal.h>
#include "linkedlist.h"
#include "pipebuilder.h"
#include "piperegistry.h"
#include "segment.h"

#define INITIAL_MAX 8

struct pipeline {  //graph
  pipe_ **nodes;  //critical: assume (entries[x] | 0 <= x < entries_n)
  segment **segments;  
  int *sort; //in-order sort of nodes
  int nodes_n;
  int nodes_max;
  int segments_n;
  int segments_max;
  linkedlist **adjacency_list;  //adjacency list with ints
  linkedlist **in_data; //in data for every node
  int next_id;
};

pipeline* pipeline_create()  {
  pipeline* pl = (pipeline*)malloc(sizeof(pipeline));
  if (pl == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for pipe");
    return NULL;
  }
 
  pl->next_id = 0;
  pl->nodes_max = INITIAL_MAX;
  pl->segments_max = INITIAL_MAX;
  pl->nodes_n = 0;
  pl->segments_n = 0;

  pl->nodes = (pipe_**)malloc(sizeof(pipe_*)*pl->nodes_max);
  if (pl->nodes == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for nodes");
    free(pl);
    return NULL;
  }

  pl->segments = (segment**)malloc(sizeof(segment*)*pl->segments_max);
  if (pl->segments == NULL)  {
    fprintf(stderr, "pipeline_create: failed to allocate memory for segments\n");
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

  for (int i = 0; i < pl->nodes_max; i++)  {
    pl->nodes[i] = NULL;
    pl->sort[i] = -1;  //-1 = invalid
  }
  
  pipeline_segment(pl);

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
    for (int i = 0; i < pl->segments_n; i++)  {
      segment_destroy(pl->segments[i]);
    }
    free(pl->nodes);
    free(pl->adjacency_list);
    free(pl->in_data);
    free(pl->sort);
    free(pl->segments);
    free(pl);

    piperegistry_deinit();

    return 1;
  }
  return 0;
}

int pipeline_segment(pipeline *pl)  {
  if (pl->segments_n >= pl->segments_max)  {
    pl->segments_max *= 2;
    segment **segments_new = realloc(pl->segments, sizeof(segment*)*pl->segments_max);
    pl->segments = segments_new;
  }
  pl->segments[pl->segments_n] = segment_create();
  return pl->segments_n++;
}

int *pipeline_adjacency_matrix(pipeline *pl)  {
  int matrix_size = pl->nodes_n*pl->nodes_n;
  int *adjacency_matrix = (int*)malloc(sizeof(int)*matrix_size);
  for (int i = 0; i < matrix_size; i++)  {
    adjacency_matrix[i] = 0;
  }
  for (int i = 0; i < pl->nodes_n; i++)  {
    int adjacency_list_n = linkedlist_size(pl->adjacency_list[i]);
    for (int j = 0; j < adjacency_list_n; j++)  {  //necessary to distinguish index 0 and end of iterater
      int index = (intptr_t)linkedlist_iterate(pl->adjacency_list[i]);
      adjacency_matrix[i*pl->nodes_n + index] = 1;
    }
    linkedlist_reset_iterater(pl->adjacency_list[i]);
  }
  return adjacency_matrix;
}

int pipeline_size(pipeline *pl)  {
  return pl->nodes_n;
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
  linkedlist **adjacency_list_new = realloc(pl->adjacency_list, sizeof(linkedlist*)*pl->nodes_max);
  if (adjacency_list_new == NULL)  {
    fprintf(stderr, "pipeline_expand: failed to realloc adjacency_list\n");
    return 0;
  }
  pl->adjacency_list = adjacency_list_new;
  linkedlist **in_data_new = realloc(pl->in_data, sizeof(linkedlist*)*pl->nodes_max);
  if (in_data_new == NULL)  {
    fprintf(stderr, "pipeline_expand: failed to realloc in_data\n");
    return 0;
  }
  pl->in_data = in_data_new;
  return 1;
}

int pipeline_link(pipeline* pl, int u, int v)  {  //create edge u->v
  if ((u < 0) || (u > (pl->nodes_n-1)) || (v < 0) || (v > (pl->nodes_n-1)))  {
    fprintf(stderr, "pipeline_link: pipe index out of bounds\n");
    return 0;
  }

  if (linkedlist_insert(pl->adjacency_list[u], (void*)(intptr_t)v) == 0)  {
    return 0;
  }
  data **input = pipe_get_output_pointer(pl->nodes[u]);
  if (linkedlist_insert(pl->in_data[v], input) == 0)  {
    return 0;
  }
  return 1;   
}

int pipeline_add(pipeline* pl, char* spec, int segment)  {
  if (segment >= pl->segments_n)  {
    fprintf(stderr, "pipeline_add: segment out of bounds\n");
    return -1;
  } 

  pipe_* p = build_pipe(spec);

  if (p == NULL)  {
    fprintf(stderr, "pipeline_add: failed to create pipe\n");
    return -1;
  }

  pipe_set_id(p, pl->next_id++);
  pipe_set_segment(p, segment);

  if (pl->nodes_n >= pl->nodes_max)  {  
    pipeline_expand(pl);
  }

  pl->nodes[pl->nodes_n] = p;
  pl->adjacency_list[pl->nodes_n] = linkedlist_create();
  pl->in_data[pl->nodes_n] = linkedlist_create();

  pl->nodes_n++;  

  return pipe_get_id(p);  //return pipe id
}

int pipeline_sort(pipeline* pl)  { 
  //in-order sort: for every unused source node (in edges = 0), reduce in edge of each successor node by 1, and make the successor node a source node if it has no more in edges
  int added = 0;
  int in[pl->nodes_n];  //in edges
  linkedlist *S = linkedlist_create();  //nodes with no more in edges
  for (int i = 0; i < pl->nodes_n; i++)  {  //set in edges
    in[i] = linkedlist_size(pl->in_data[i]);  //use in_data instead of adjacency list for less computations...
    if (in[i] == 0)  {
      linkedlist_insert(S, (void*)(intptr_t)i);
    }
  }
  while (linkedlist_size(S) != 0)  {
    int s = (intptr_t)linkedlist_pop(S);  //node with no more in edges
    pl->sort[added++] = s;
    int out_edge = 0;
    for (int i = 0; i < linkedlist_size(pl->adjacency_list[s]); i++)  {  //clumsy loop-iteration due to problems with linkedlist having int 0 entry
      out_edge = (intptr_t)linkedlist_iterate(pl->adjacency_list[s]);
      if (--in[out_edge] == 0)  {
        linkedlist_insert(S, (void*)(intptr_t)out_edge);
      }
    }
  }
  if (added < pl->nodes_n)  {
    printf("pipeline_sort: warning - %d nodes not sorted, cycles exist or graph is not connected\n", pl->nodes_n - added);
  }
  linkedlist_destroy(S);
  //sort completed
  
  return 1;
}

int pipeline_start_threads(pipeline *pl)  {
  segment_main(pl->segments[0]);  //set thread of main segment
  //start threads of non-main segments
  for (int i = 1; i < pl->segments_n; i++)  { 
    segment_thread_start(pl->segments[i]);
  }
  return 1;
}

int pipeline_stop_threads(pipeline *pl)  {
  segment_kill_data(pl->segments[0]);
  //stop threads for non-main segments
  for (int i = 1; i < pl->segments_n; i++)  {
    segment_thread_stop(pl->segments[i]);
  }
  return 1;
}

int pipeline_run(pipeline* pl)  { 
  segment_run(pl->segments[0]);
  return 1;
}

int pipeline_kill(pipeline *pl)  {
  for (int i = 0; i < pl->nodes_n; i++)  {
    pipe_* p = pl->nodes[pl->sort[i]];
    pipe_kill(p, pl->in_data[pl->sort[i]]);  //TODO check for fail?
  }
  return 1;
}

int pipeline_init(pipeline *pl)  {
  //printf("initing: n=%d\n", pl->nodes_n);
  pipeline_sort(pl);
  //pipes are now sorted 
  for (int i = 0; i < pl->nodes_n; i++)  {
   // printf("initing pipe %p %s\n", pl->nodes[pl->sort[i]], pipe_get_type(pl->nodes[pl->sort[i]]));
    pipe_* p = pl->nodes[pl->sort[i]];
    linkedlist* in_data = pl->in_data[pl->sort[i]];
    if (pipe_init(p, in_data) != 1)  {
      fprintf(stderr, "pipeline_init: failed to init pipe %d\n", pl->sort[i]); 
      return 0;
    }
    //make pipe and output pipes concurrent if any outputs pipes are from separate segment
    int concurrent = 0;
    int output_n = linkedlist_size(pl->adjacency_list[pl->sort[i]]);
    linkedlist_reset_iterater(pl->adjacency_list[pl->sort[i]]);
    int output_pipes[output_n];
    for (int j = 0; j < output_n; j++)  {  //must do for loop due to null (0) id possibility
      output_pipes[j] = (intptr_t)linkedlist_iterate(pl->adjacency_list[pl->sort[i]]);
      if (pipe_get_segment(p) != pipe_get_segment(pl->nodes[output_pipes[j]]))  {
        concurrent = 1;
      }
    }
    if (concurrent == 1)  {
      pipe_set_concurrent(p, 1);
      for (int j = 0; j < output_n; j++)  {
        pipe_set_concurrent(pl->nodes[output_pipes[j]], 1);
      } 
    }
    //make data blocking if concurrent
    if (pipe_get_concurrent(p) == 1)  {
      data *d = pipe_get_output(p);
      if (d != NULL)  {
        data_make_blocking(d);
      }
    }
    //increment readers of concurrent outputs
    data **data_ptr = NULL;
    while ((data_ptr = (data**)linkedlist_iterate(in_data)) != NULL)  {
      data *d = *data_ptr;
      if (data_get_blocking(d))  {
        data_increment_readers(d, segment_get_thread_ptr(pl->segments[pipe_get_segment(p)]));
      }
    }
    segment_add(pl->segments[pipe_get_segment(p)], p, in_data); ///add to segment
  }
  return 1;
}
