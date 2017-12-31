#ifndef PIPELINE_H
#define PIPELINE_H

#include "pipe.h"

typedef struct pipeline pipeline;
struct pipeline;

pipeline* pipeline_create();
int pipeline_destroy(pipeline*);
int *pipeline_adjacency_matrix(pipeline*);
int pipeline_size(pipeline*);
int pipeline_add(pipeline*, char*, int);
int pipeline_link(pipeline*, int, int);
int pipeline_init(pipeline*);
int pipeline_run(pipeline*);
int *pipeline_adjacency_matrix(pipeline*);
int pipeline_segment(pipeline*);
int pipeline_start_threads(pipeline*);
int pipeline_stop_threads(pipeline*);
int pipeline_kill(pipeline*);

#endif
