#include "register_pipes.h"
#include "piperegistry.h"

int dummyemotiv_init(pipe_*, linkedlist*);
int dummyemotiv_run(pipe_*, linkedlist*);
int emotiv_init(pipe_*, linkedlist*);
int emotiv_run(pipe_*, linkedlist*);
int fouriertransform_init(pipe_*, linkedlist*);
int fouriertransform_run(pipe_*, linkedlist*);
int inversefouriertransform_init(pipe_*, linkedlist*);
int inversefouriertransform_run(pipe_*, linkedlist*);
int mean_init(pipe_*, linkedlist*);
int mean_run(pipe_*, linkedlist*);
int print_init(pipe_*, linkedlist*);
int print_run(pipe_*, linkedlist*);
int alphabandpass_init(pipe_*, linkedlist*);
int alphabandpass_run(pipe_*, linkedlist*);
int fileout_init(pipe_*, linkedlist*);
int fileout_run(pipe_*, linkedlist*);

int register_pipes()  {
  piperegistry_register("DUMMYEMOTIV", &dummyemotiv_init, &dummyemotiv_run, "");
  piperegistry_register("EMOTIV", &emotiv_init, &emotiv_run, "");
  piperegistry_register("FOURIERTRANSFORM", &fouriertransform_init, &fouriertransform_run, "");
  piperegistry_register("INVERSEFOURIERTRANSFORM", &inversefouriertransform_init, &inversefouriertransform_run, "");
  piperegistry_register("MEAN", &mean_init, &mean_run, "");
  piperegistry_register("PRINT", &print_init, &print_run, "");
  piperegistry_register("ALPHABANDPASS", &alphabandpass_init, &alphabandpass_run, "");
  piperegistry_register("FILEOUT", &fileout_init, &fileout_run, "");
}
