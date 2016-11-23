#include "debug_pipe.h"
#include "gettime.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct time_interval time_interval;
struct time_interval {
  double time_before;
  double time_after;  
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

  interval->time_before = get_clock_time();

  linkedlist_insert(d->time_intervals, (void*)interval);

  return 1;
}

int debug_pipe_stop_timer(debug_pipe *d)  {
  time_interval *interval = (time_interval*)linkedlist_tail(d->time_intervals);

  interval->time_after = get_clock_time();

  return 1;
}

double debug_pipe_time(debug_pipe *d)  {
  if (linkedlist_size(d->time_intervals) > 0)  {
    time_interval *interval = (time_interval*)linkedlist_tail(d->time_intervals);
    return interval->time_after - interval->time_before; 
  }
  return -1;
}

double debug_pipe_average_time(debug_pipe *d)  {
  if (linkedlist_size(d->time_intervals) > 0)  {
    double average = 0;
    time_interval *interval = NULL;
    while ((interval = linkedlist_iterate(d->time_intervals)) != NULL)  {
      average += interval->time_after - interval->time_before;  
    }
    average /= (double)linkedlist_size(d->time_intervals);
    return average;
  }
  return -1;   
}
