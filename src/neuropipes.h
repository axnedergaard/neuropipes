#ifndef NEUROPIPES_H
#define NEUROPIPES_H

typedef struct pipeline pipeline;
struct pipeline;

pipeline* pipeline_create();
int pipeline_destroy(pipeline*);
int pipeline_size(pipeline*);
void pipeline_set_loop(pipeline* pl, int);
int pipeline_get_loop(pipeline* pl);
void pipeline_set_interval(pipeline *pl, double);
int pipeline_insert(pipeline*, char*, int);
int pipeline_insert_edge(pipeline*, int, int);
int pipeline_init(pipeline*);
int pipeline_run(pipeline*);
int *pipeline_adjacency_matrix(pipeline*);
int list_available_pipes(char***);


#endif
