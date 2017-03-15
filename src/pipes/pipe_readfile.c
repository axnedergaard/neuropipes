#include "../pipe.h"
#include "../data.h"
#include "../edflib.h"

#include <stdio.h>
#include <stdlib.h>

struct readfile_aux {
  int handle; 
  int current_set;
  int set_n; //number of sets
};

int readfile_init(pipe_ *p, linkedlist *l)  {
  //read header to get dimensions
  int c;
  int n;
  int set_n;
  char *fn = "recording.edf";

  struct edf_hdr_struct *hdr = (struct edf_hdr_struct*)malloc(sizeof(struct edf_hdr_struct));
  if (edfopen_file_readonly(fn, hdr, EDFLIB_READ_ALL_ANNOTATIONS) != 0)  {
    printf("failed to read file\n");
  }
  c = hdr->edfsignals;
  struct edf_param_struct sig_par = hdr->signalparam[0];  //assume n and sets is same for all channels
  n = sig_par.smp_in_datarecord;
  set_n = sig_par.smp_in_file / sig_par.smp_in_datarecord;

  int shape[2];
  int stride[2];
  shape[0] = c;
  shape[1] = n;
  stride[0] = 1;
  stride[1] = 1;
  pipe_set_output(p, data_create(2, shape, stride));

  struct readfile_aux *aux = (struct readfile_aux*)malloc(sizeof(struct readfile_aux));
  aux->current_set = 0;
  aux->set_n = set_n;
  aux->handle = hdr->handle;

  pipe_set_auxiliary(p, aux);

  return 1;
}

int readfile_run(pipe_ *p, linkedlist *l)  {
  struct readfile_aux *aux = (struct readfile_aux*)pipe_get_auxiliary(p);

  data *output = pipe_get_output(p);
  double *buffer = data_get_buffer(output);

  //read
  int *shape = data_get_shape(output);
  int c = shape[0];
  int n = shape[1];

  write_lock(output);

  for (int i = 0; i < c; i++)  {
    if (edfread_physical_samples(aux->handle, i, n, (buffer + i*n)) < 0)  {
      printf("data_edf_read: failed to read samples\n");
    }
  }
 
  write_unlock(output); 

  aux->current_set++;

  if (aux->current_set >= aux->set_n)  {  //all signals read
    return 0;
  }

  return 1; 
}

int readfile_kill(pipe_* p, linkedlist* l)  {
  return (edfclose_file(((struct readfile_aux*)pipe_get_auxiliary(p))->handle) == 0);


}
