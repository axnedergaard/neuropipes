#include <stdio.h>
#include <stdlib.h>
#include "pipeline.h"
#include <sys/resource.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <unistd.h>

static int next = 0;
void insert_next(pipeline *pl, char *pipe)  {
  pipeline_insert(pl, pipe, 0);
  if (next > 0)  {
    pipeline_insert_edge(pl, next-1, next);
  }
  next++;
}

void insert_next_concurrent(pipeline *pl, char *pipe)  {
  pipeline_insert(pl, pipe, 1);
  if (next > 0)  {
    pipeline_insert_edge(pl, next-1, next);
  }
  next++;
}

int main(int argc, char **argv)  {
  pipeline* pl = pipeline_create();

  if (argc > 2)  {
    pipeline_set_interval(pl, atof(argv[2]));
  }
  if (argc > 1)  { 
    pipeline_set_loop(pl, atoi(argv[1]));
  }

  //construct pipeline 

  /* 
  //write file
  int emotiv = pipeline_insert(pl, "DUMMYEMOTIV;random=0", 1);
  int writefile = pipeline_insert(pl, "WRITEFILE", 1);
  pipeline_insert_edge(pl, emotiv, writefile);
*/

/*
  //read file
  int readfile = pipeline_insert(pl, "READFILE", 1);
  int print = pipeline_insert(pl, "PRINT", 1);
  pipeline_insert_edge(pl, readfile, print);
*/

  /*
  //dummy
  int input = pipeline_insert(pl, "DUMMYEMOTIV;random=0", 0);
  int computation = pipeline_insert(pl, "DUMMYCOMPUTATION;replace=1,replace_value=3.14", 0);
//  int output = pipeline_insert(pl, "PRINT", 0);
  pipeline_insert_edge(pl, input, computation);
//  pipeline_insert_edge(pl, computation, output);
*/

/*
  //functional filter
  int emotiv = pipeline_insert(pl, "DUMMYEMOTIV;random=1", 1);
  int filt1 = pipeline_insert(pl, "FILTER;order=4,pass=band,lc=8,hc=12", 0);
  int writemem1 = pipeline_insert(pl, "WRITESHAREDMEM;key=42", 0);
  int writemem2 = pipeline_insert(pl, "WRITESHAREDMEM;key=43", 0);
  pipeline_insert_edge(pl, emotiv, filt1);
  pipeline_insert_edge(pl, emotiv, writemem1);
  pipeline_insert_edge(pl, filt1, writemem2);
*/
  
  //functional power
  int input = pipeline_insert(pl, "DUMMYEMOTIV;random=1", 0);
  int fft = pipeline_insert(pl, "FOURIERTRANSFORM", 0);
  int power = pipeline_insert(pl, "POWER", 0);
  int shared1 = pipeline_insert(pl, "WRITESHAREDMEM;key=42", 0);
  int shared2 = pipeline_insert(pl, "WRITESHAREDMEM;key=43", 0);
  pipeline_insert_edge(pl, input, fft);
  pipeline_insert_edge(pl, input, shared1);
  pipeline_insert_edge(pl, fft, power);
  pipeline_insert_edge(pl, power, shared2);


 /* 
  //performance simple
  int input = pipeline_insert(pl, "EMOTIV", 1);
  int shared = pipeline_insert(pl, "WRITESHAREDMEM;key=42", 0);
  pipeline_insert_edge(pl, input, shared);
*/

/* 
  //performance complex
  int input = pipeline_insert(pl, "EMOTIV", 1);
  int shared1 = pipeline_insert(pl, "WRITESHAREDMEM;key=42", 0);
  int shared2 = pipeline_insert(pl, "WRITESHAREDMEM;key=43", 0);
  int shared3 = pipeline_insert(pl, "WRITESHAREDMEM;key=44", 0);
  int shared4 = pipeline_insert(pl, "WRITESHAREDMEM;key=45", 0);
  int shared5 = pipeline_insert(pl, "WRITESHAREDMEM;key=46", 0);
  int fft = pipeline_insert(pl, "FOURIERTRANSFORM", 0);
  int power = pipeline_insert(pl, "POWER", 0);
  int file = pipeline_insert(pl, "WRITEFILE", 0); //filename?
  int print = pipeline_insert(pl, "PRINT", 0); //filename?
  int tbp = pipeline_insert(pl, "FILTER;pass=band,lc=4,hc=7", 0);
  int abp = pipeline_insert(pl, "FILTER;pass=band,lc=8,hc=12", 0);
  int bbp = pipeline_insert(pl, "FILTER;pass=band,lc=13,hc=25", 0); 
  pipeline_insert_edge(pl, input, shared1);
  pipeline_insert_edge(pl, input, file);
  pipeline_insert_edge(pl, input, fft);
  pipeline_insert_edge(pl, input, tbp);
  pipeline_insert_edge(pl, input, abp);
  pipeline_insert_edge(pl, input, bbp);
  pipeline_insert_edge(pl, fft, power);
  pipeline_insert_edge(pl, power, shared2);
  pipeline_insert_edge(pl, fft, print);
  pipeline_insert_edge(pl, tbp, shared3);
  pipeline_insert_edge(pl, abp, shared4);
  pipeline_insert_edge(pl, bbp, shared5);
*/

/*
  //acceptance experiment (power)
  int input = pipeline_insert(pl, "EMOTIV", 0);
  int fft = pipeline_insert(pl, "FOURIERTRANSFORM", 0);
  int power = pipeline_insert(pl, "POWER", 0);
  int shared = pipeline_insert(pl, "WRITESHAREDMEM;key=42", 0);
  pipeline_insert_edge(pl, input, fft);
  pipeline_insert_edge(pl, fft, power);
  pipeline_insert_edge(pl, power, shared);
*/
  //init and run
  if (pipeline_init(pl)) printf("[!]init\n");
  if (pipeline_run(pl)) printf("[!]run\n");
  pipeline_destroy(pl);
 
  return 0;
}
