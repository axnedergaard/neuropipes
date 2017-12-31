#ifndef NEUROPIPES_H
#define NEUROPIPES_H

typedef struct pipeline pipeline;
struct pipeline;

typedef struct pipe pipe_;
struct pipe;

pipeline* pipeline_create();
int pipeline_destroy(pipeline*);
int pipeline_size(pipeline*);
int pipeline_add(pipeline*, char*, int);
int pipeline_link(pipeline*, int, int);
int pipeline_init(pipeline*);
int pipeline_run(pipeline*);
int pipeline_segment(pipeline*);
int *pipeline_adjacency_matrix(pipeline*);
int list_available_pipes(char***);
int pipeline_start_threads(pipeline*);
int pipeline_stop_threads(pipeline*);
int pipeline_kill(pipeline*);
char *pipe_get_type(pipe_*);
pipe_* pipeline_get_pipe(pipeline*, int);

#endif
