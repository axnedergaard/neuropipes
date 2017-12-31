#include "neuropipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <unistd.h>

static int next = 0;
void insert_next(pipeline *pl, char *pipe)  {
  pipeline_add(pl, pipe, 0);
  if (next > 0)  {
    pipeline_link(pl, next-1, next);
  }
  next++;
}

void insert_next_concurrent(pipeline *pl, char *pipe)  {
  pipeline_add(pl, pipe, 1);
  if (next > 0)  {
    pipeline_link(pl, next-1, next);
  }
  next++;
}

int main(int argc, char **argv)  {
  int run_n = 100;
  if (argc > 1)  run_n = atoi(argv[1]);

  pipeline* pl = pipeline_create();
 
/*
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  int input = pipeline_add(pl, "DUMMYEMOTIV;random=0", 0);
  int fft = pipeline_add(pl, "FOURIERTRANSFORM", 0);
  int ift = pipeline_add(pl, "INVERSEFOURIERTRANSFORM", 0);
  int print = pipeline_add(pl, "PRINT", 0);
  int write1 = pipeline_add(pl, "WRITEFILE;fn=1", 1);
  int write2 = pipeline_add(pl, "WRITEFILE;fn=2", 2);
  int write3 = pipeline_add(pl, "WRITEFILE;fn=3", 3);
  pipeline_link(pl, input, fft);
  pipeline_link(pl, fft, ift);
  pipeline_link(pl, ift, print);
  pipeline_link(pl, input, write1);
  pipeline_link(pl, fft, write2);
  pipeline_link(pl, ift, write3);
*/
/*
  int input = pipeline_add(pl, "DUMMYEMOTIV;random=0", 0);
  int fft = pipeline_add(pl, "FOURIERTRANSFORM", 0);
  int ift = pipeline_add(pl, "INVERSEFOURIERTRANSFORM", 0);
  int print = pipeline_add(pl, "PRINT", 0);
  pipeline_link(pl, input, fft);
  pipeline_link(pl, fft, ift);
  pipeline_link(pl, ift, print);
*/
/*
  pipeline_segment(pl);
  //pipeline_segment(pl);
  int input = pipeline_add(pl, "DUMMYEMOTIV;random=0", 0);
  //int fft = pipeline_add(pl, "FOURIERTRANSFORM", 0);
  int fft = pipeline_add(pl, "FILTER", 0);
  //int ift = pipeline_add(pl, "INVERSEFOURIERTRANSFORM", 0);
  int print = pipeline_add(pl, "WRITEFILE", 1);
  pipeline_link(pl, input, fft);
  //pipeline_link(pl, fft, ift);
  pipeline_link(pl, fft, print);
  */

/*  
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  int input = pipeline_add(pl, "DUMMYEMOTIV", 1);
  int fft = pipeline_add(pl, "FOURIERTRANSFORM", 0);
  int ift = pipeline_add(pl, "INVERSEFOURIERTRANSFORM", 0);
  int print = pipeline_add(pl, "WRITEFILE;fn=1", 2);
  int write = pipeline_add(pl, "WRITEFILE;fn=2", 3);
  pipeline_link(pl, input, fft);
  pipeline_link(pl, fft, ift);
  pipeline_link(pl, ift, print);
  pipeline_link(pl, ift, write);
*/

/*  
  //construct pipeline 
  pipeline_segment(pl);
  int input = pipeline_add(pl, "DUMMYEMOTIV;random=0", 0);
  int write = pipeline_add(pl, "WRITEFILE", 0);
  int print = pipeline_add(pl, "PRINT", 1);
  pipeline_link(pl, input, write);
  pipeline_link(pl, input, print);
*/
/*  
  //print
  int input = pipeline_add(pl, "DUMMYEMOTIV", 0);
  int shared = pipeline_add(pl, "PRINT", 0);
  pipeline_link(pl, input, shared);
*/
    /* 
  //write file
  int emotiv = pipeline_add(pl, "DUMMYEMOTIV;random=0", 0);
  int writefile = pipeline_add(pl, "WRITEFILE", 1);
  pipeline_link(pl, emotiv, writefile);
*/

/*
  //read file
  int readfile = pipeline_add(pl, "READFILE", 1);
  int print = pipeline_add(pl, "PRINT", 1);
  pipeline_link(pl, readfile, print);
*/

  /*
  //dummy
  int input = pipeline_add(pl, "DUMMYEMOTIV;random=0", 0);
  int computation = pipeline_add(pl, "DUMMYCOMPUTATION;replace=1,replace_value=3.14", 0);
//  int output = pipeline_add(pl, "PRINT", 0);
  pipeline_link(pl, input, computation);
//  pipeline_link(pl, computation, output);
*/
/*
  //functional filter
  int emotiv = pipeline_add(pl, "DUMMYEMOTIV;random=1,frames=254", 0);
  int filt1 = pipeline_add(pl, "FILTER;order=4,pass=band,lc=8,hc=12", 0);
  int writemem1 = pipeline_add(pl, "WRITESHAREDMEM;key=42", 0);
  int writemem2 = pipeline_add(pl, "WRITESHAREDMEM;key=43", 0);
  pipeline_link(pl, emotiv, filt1);
  pipeline_link(pl, emotiv, writemem1);
  pipeline_link(pl, filt1, writemem2);
*/
 /* 
  //functional power
  int input = pipeline_add(pl, "DUMMYEMOTIV;random=1", 0);
  int fft = pipeline_add(pl, "FOURIERTRANSFORM", 0);
  int power = pipeline_add(pl, "POWER", 0);
  int shared1 = pipeline_add(pl, "WRITESHAREDMEM;key=72", 0);
  int shared2 = pipeline_add(pl, "WRITESHAREDMEM;key=73", 0);
  pipeline_link(pl, input, fft);
  pipeline_link(pl, input, shared1);
  pipeline_link(pl, fft, power);
  pipeline_link(pl, power, shared2);
*/

  /*
  //RPI test
  int rpi = pipeline_add(pl, "RPI", 0);
  int print = pipeline_add(pl, "WRITEFILE", 0);
  pipeline_link(pl, rpi, print);
*/
/* 
  //performance simple
  int input = pipeline_add(pl, "DUMMYEMOTIV", 1);
  int shared = pipeline_add(pl, "WRITESHAREDMEM;key=42", 0);
  pipeline_link(pl, input, shared);
*/
/*
  //performance complex
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);

  int input = pipeline_add(pl, "DUMMYEMOTIV", 1);
  int shared1 = pipeline_add(pl, "WRITESHAREDMEM;key=42", 2);
  int shared2 = pipeline_add(pl, "WRITESHAREDMEM;key=43", 3);
  int shared3 = pipeline_add(pl, "WRITESHAREDMEM;key=44", 4);
  int shared4 = pipeline_add(pl, "WRITESHAREDMEM;key=45", 5);
  int shared5 = pipeline_add(pl, "WRITESHAREDMEM;key=46", 6);
  int fft = pipeline_add(pl, "FOURIERTRANSFORM", 0);
  int power = pipeline_add(pl, "POWER", 0);
  int file = pipeline_add(pl, "WRITEFILE", 0); //filename?
  int print = pipeline_add(pl, "PRINT", 0); //filename?
  int tbp = pipeline_add(pl, "FILTER;pass=band,lc=4,hc=7", 0);
  int abp = pipeline_add(pl, "FILTER;pass=band,lc=8,hc=12", 0);
  int bbp = pipeline_add(pl, "FILTER;pass=band,lc=13,hc=25", 0); 
  pipeline_link(pl, input, shared1);
  pipeline_link(pl, input, file);
  pipeline_link(pl, input, fft);
  pipeline_link(pl, input, tbp);
  pipeline_link(pl, input, abp);
  pipeline_link(pl, input, bbp);
  pipeline_link(pl, fft, power);
  pipeline_link(pl, power, shared2);
  pipeline_link(pl, fft, print);
  pipeline_link(pl, tbp, shared3);
  pipeline_link(pl, abp, shared4);
  pipeline_link(pl, bbp, shared5);
*/
 
  //all
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  pipeline_segment(pl);
  int input = pipeline_add(pl, "DUMMYEMOTIV;random=0", 1);
  int dummy = pipeline_add(pl, "DUMMYCOMPUTATION", 0);
  int fft = pipeline_add(pl, "FOURIERTRANSFORM", 2);
  int ift = pipeline_add(pl, "INVERSEFOURIERTRANSFORM", 2);
  int filter = pipeline_add(pl, "FILTER", 2);
  int power = pipeline_add(pl, "POWER", 3);
  int print = pipeline_add(pl, "PRINT", 4);
  int writes = pipeline_add(pl, "WRITESHAREDMEM", 5);
  int writef1 = pipeline_add(pl, "WRITEFILE;fn=1", 6);
  int writef2 = pipeline_add(pl, "WRITEFILE;fn=2", 7);
  pipeline_link(pl, input, dummy);
  pipeline_link(pl, dummy, fft);
  pipeline_link(pl, fft, ift);
  pipeline_link(pl, ift, filter);
  pipeline_link(pl, filter, writes);
  pipeline_link(pl, fft, power);
  pipeline_link(pl, power, writef1);
  pipeline_link(pl, ift, print);
  pipeline_link(pl, input, writef2);

/*
  //acceptance experiment (power)
  int input = pipeline_add(pl, "EMOTIV", 0);
  int fft = pipeline_add(pl, "FOURIERTRANSFORM", 0);
  int power = pipeline_add(pl, "POWER", 0);
  int shared = pipeline_add(pl, "WRITESHAREDMEM;key=42", 0);
  pipeline_link(pl, input, fft);
  pipeline_link(pl, fft, power);
  pipeline_link(pl, power, shared);
*/
  //init and run
  if (pipeline_init(pl)) printf("[!]init\n");
  if (pipeline_start_threads(pl))  printf("[!]started\n");
  for (int i = 0; i < run_n; i++)  {
    if (pipeline_run(pl) != 1)  return -1; 
  }
  printf("[!]run\n");
  if (pipeline_stop_threads(pl))  printf("[!]stopped\n");
  if (pipeline_kill(pl)) printf("[!]kill\n");

  pipeline_destroy(pl);
 
  return 0;
}
