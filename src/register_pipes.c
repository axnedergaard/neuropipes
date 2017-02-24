#include "register_pipes.h"
#include "piperegistry.h"

int dummyemotiv_init(pipe_*, linkedlist*);
int dummyemotiv_run(pipe_*, linkedlist*);
int dummyemotiv_kill(pipe_*, linkedlist*);
int dummy_computation_init(pipe_*, linkedlist*);
int dummy_computation_run(pipe_*, linkedlist*);
int dummy_computation_kill(pipe_*, linkedlist*);
int emotiv_init(pipe_*, linkedlist*);
int emotiv_run(pipe_*, linkedlist*);
int emotiv_kill(pipe_*, linkedlist*);
int fouriertransform_init(pipe_*, linkedlist*);
int fouriertransform_run(pipe_*, linkedlist*);
int fouriertransform_kill(pipe_*, linkedlist*);
int inversefouriertransform_init(pipe_*, linkedlist*);
int inversefouriertransform_run(pipe_*, linkedlist*);
int inversefouriertransform_kill(pipe_*, linkedlist*);
int mean_init(pipe_*, linkedlist*);
int mean_run(pipe_*, linkedlist*);
int mean_kill(pipe_*, linkedlist*);
int print_init(pipe_*, linkedlist*);
int print_run(pipe_*, linkedlist*);
int print_kill(pipe_*, linkedlist*);
int filter_init(pipe_*, linkedlist*);
int filter_run(pipe_*, linkedlist*);
int filter_kill(pipe_*, linkedlist*);
int writefile_init(pipe_*, linkedlist*);
int writefile_run(pipe_*, linkedlist*);
int writefile_kill(pipe_*, linkedlist*);
int readfile_init(pipe_*, linkedlist*);
int readfile_run(pipe_*, linkedlist*);
int readfile_kill(pipe_*, linkedlist*);
int writesharedmem_init(pipe_*, linkedlist*);
int writesharedmem_run(pipe_*, linkedlist*);
int writesharedmem_kill(pipe_*, linkedlist*);

int register_pipes()  {
  piperegistry_register("DUMMYEMOTIV", &dummyemotiv_init, &dummyemotiv_run, &dummyemotiv_kill, "");
  piperegistry_register("DUMMYCOMPUTATION", &dummy_computation_init, &dummy_computation_run, &dummy_computation_kill, "");
  piperegistry_register("EMOTIV", &emotiv_init, &emotiv_run, &emotiv_kill, "");
  piperegistry_register("FOURIERTRANSFORM", &fouriertransform_init, &fouriertransform_run, &fouriertransform_kill, "");
  piperegistry_register("INVERSEFOURIERTRANSFORM", &inversefouriertransform_init, &inversefouriertransform_run, &inversefouriertransform_kill, "");
  piperegistry_register("MEAN", &mean_init, &mean_run, &mean_kill, "");
  piperegistry_register("PRINT", &print_init, &print_run, &print_kill, "");
  piperegistry_register("FILTER", &filter_init, &filter_run, &filter_kill, "");
  piperegistry_register("WRITEFILE", &writefile_init, &writefile_run, &writefile_kill, "");
  piperegistry_register("READFILE", &readfile_init, &readfile_run, &readfile_kill, "");
  piperegistry_register("WRITESHAREDMEM", &writesharedmem_init, &writesharedmem_run, &writesharedmem_kill, "");
  return 1;
}
