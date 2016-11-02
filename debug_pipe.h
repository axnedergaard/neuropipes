#ifndef DEBUG_PIPE_H
#define DEBUG_PIPE_H

typedef struct debug_pipe debug_pipe;
struct debug_pipe;

debug_pipe *debug_pipe_create();
int debug_pipe_destroy();

#endif
