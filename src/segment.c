#include "segment.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PIPES_INITIAL_MAX 2

struct segment {
  int pipes_n;
  int pipes_max;
  pipe_ **pipes;
  linkedlist **in_data;
  pthread_t thread;
  int kill;
};

segment *segment_create()  {
  segment *s = (segment*)malloc(sizeof(segment));
  if (s == NULL)  {
    fprintf(stderr, "segment_create: failed to allocate memory\n");
  }
  s->pipes_max = PIPES_INITIAL_MAX;
  s->pipes_n = 0;
  s->pipes = (pipe_**)malloc(sizeof(pipe_*)*s->pipes_max);
  s->in_data = (linkedlist**)malloc(sizeof(linkedlist*)*s->pipes_max);
  s->thread = 0;
  s->kill = 0;

  return s;
}

int segment_destroy(segment *s)  {
  free(s->pipes);
  free(s->in_data);
  free(s);
  return 1;
}

int segment_add(segment *s, pipe_* p, linkedlist *d)  {
  if (s->pipes_n >= s->pipes_max)  {  //expand
    s->pipes_max *= 2;
    pipe_ **pipes_new = realloc(s->pipes, sizeof(pipe_*)*s->pipes_max);
    s->pipes = pipes_new;
    linkedlist **in_data_new = realloc(s->in_data, sizeof(linkedlist*)*s->pipes_max);
    s->in_data = in_data_new;
  }
  s->pipes[s->pipes_n] = p;
  s->in_data[s->pipes_n] = d;
  //pipe_set_segment(p, (intptr_t)s);
  s->pipes_n++;
  return 1;
}

void segment_run(segment *s)  {
  for (int i = 0; i < s->pipes_n; i++)  {
    pipe_run(s->pipes[i], s->in_data[i]);
  }
}

void segment_main(segment *s)  {
  s->thread = pthread_self();
}

void *segment_thread_run(void* arg)  {
  segment *s = (segment*)arg;
  while (s->kill == 0)  {
    segment_run(s);
  }
  return NULL;
}

int segment_thread_start(segment *s)  {
  if (pthread_create(&s->thread, NULL, &segment_thread_run, s) != 0)  {
    fprintf(stderr, "segment_thread_start: failed to create thread\n");
    return 0;
  }
  return 1;
}

void segment_kill_data(segment *s)  {
  for (int i = 0; i < s->pipes_n; i++)  {
    data *output = pipe_get_output(s->pipes[i]);
    if (output != NULL)  {
      if (data_get_blocking(output))  {
        data_kill(output);
      }
    }
  }

}

int segment_thread_stop(segment *s)  {
  s->kill = 1;
  segment_kill_data(s);
  pthread_join(s->thread, NULL);
  return 1;
}

pthread_t *segment_get_thread_ptr(segment *s)  {
  return &s->thread;
}
