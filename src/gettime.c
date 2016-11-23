#include <sys/time.h>
#include <stdio.h>

double get_clock_time()  {
  struct timeval time;
  gettimeofday(&time, NULL);
  return (double)time.tv_sec + (double)time.tv_usec / 1000000.0;
}
