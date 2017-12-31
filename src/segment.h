#include <pthread.h>
#include "linkedlist.h"
#include "pipe.h"

typedef struct segment segment;
struct segment;

segment *segment_create();
int segment_destroy(segment*);
int segment_add(segment*, pipe_*, linkedlist*);
void segment_run(segment*);
void segment_main(segment*);  //specify main segment

void segment_kill_data(segment*);

int segment_thread_start(segment*);
int segment_thread_stop(segment*);
pthread_t *segment_get_thread_ptr(segment*);
