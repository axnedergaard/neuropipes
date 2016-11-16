#ifndef DEBUG_PIPE_H
#define DEBUG_PIPE_H

typedef struct debug_pipe debug_pipe;
struct debug_pipe;

debug_pipe *debug_pipe_create();
int debug_pipe_destroy(debug_pipe*);
int debug_pipe_start_timer(debug_pipe*);
int debug_pipe_stop_timer(debug_pipe*);
double debug_pipe_time(debug_pipe*);
double debug_pipe_average_time(debug_pipe*);

#endif
