#include "debug_pipe.h"
#include <stdio.h>

struct debug_pipe {

};

debug_pipe *debug_pipe_create()  {
  debug_pipe *d = (debug_pipe*)malloc(sizeof(debug_pipe));
  if (d == NULL)  {
    fprintf(stderr, "debug_pipe_create: failed to alloc mem\n");
  }
  return d;
}

int debug_pipe_destroy(debug_pipe *d)  {
  free(d);
  return 1;
}
