#include "debug_pipe.h"
#include "linkedlist.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct time_interval time_interval;
struct time_interval {
  struct timeval time_before;
  struct timeval time_after;  
};

struct debug_pipe {
  linkedlist *time_intervals;
};

debug_pipe *debug_pipe_create()  {
  debug_pipe *d = (debug_pipe*)malloc(sizeof(debug_pipe));
  if (d == NULL)  {
    fprintf(stderr, "debug_pipe_create: failed to alloc mem\n");
  }
  d->time_intervals = linkedlist_create();
  if (d->time_intervals == NULL)  {
    fprintf(stderr, "debug_pipe_create: failed to alloc mem for intervals\n");
    free(d);
    return NULL;
  }
  return d;
}

int debug_pipe_destroy(debug_pipe *d)  {
  //destroy each time interval TODO
  linkedlist_destroy(d->time_intervals);
  free(d);
  return 1;
}

int debug_pipe_start_timer(debug_pipe *d)  {
  time_interval *interval = (time_interval*)malloc(sizeof(time_interval));
  if (interval == NULL)  {
    fprintf(stderr, "debug_pipe_start_timer: failed to alloc mem for time interval\n");
    return -1;
  }

  gettimeofday(&(interval->time_before), NULL);

  linkedlist_insert(d->time_intervals, (void*)interval);

  return 1;
}

int debug_pipe_stop_timer(debug_pipe *d)  {
  time_interval *interval = (time_interval*)linkedlist_tail(d->time_intervals);

  gettimeofday((&interval->time_after), NULL);

  return 1;
}

double debug_pipe_time(debug_pipe *d)  {
  if (linkedlist_size(d->time_intervals) > 0)  {
    time_interval *interval = (time_interval*)linkedlist_tail(d->time_intervals);
    struct timeval time_before = interval->time_before;
    struct timeval time_after = interval->time_after;
    //TODO fix bug
    double time = (((double)time_after.tv_sec - (double)time_before.tv_sec) + ((double)time_after.tv_usec - (double)time_before.tv_usec) / 1000.0);  
    return time;
  }
  return -1;
}

double debug_pipe_average_time(debug_pipe *d)  {
  if (linkedlist_size(d->time_intervals) > 0)  {
    double average = 0;
    time_interval *interval = NULL;
    while ((interval = linkedlist_iterate(d->time_intervals)) != NULL)  {
      struct timeval time_before = interval->time_before;
      struct timeval time_after = interval->time_after;
      average += ((double)(time_after.tv_sec - time_before.tv_sec) + (double)(time_after.tv_usec - time_before.tv_usec) / 1000.0);  
    }
    average /= (double)linkedlist_size(d->time_intervals);
    return average;
  }
  return -1;   
}
