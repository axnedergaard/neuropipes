#include "../pipe.h"
#include "../emokit.h"
#include <stdio.h>
#include <stdlib.h>

//EMOINP
struct auxiliary_emokit  {
  struct emokit_frame frame;
  struct emokit_device* device;
  double *buffer;
 // struct emokit_contact_quality quality;
};

int emotiv_init(pipe_* p, linkedlist* l)  {
  data *output = data_create_from_string("EMOTIV");
  
  pipe_set_output(p, output);

  struct auxiliary_emokit* aux = (struct auxiliary_emokit*)malloc(sizeof(struct auxiliary_emokit));
  if (aux == NULL)  {
    fprintf(stderr, "init_emoinp: failed to allocate memory for auxiliary emokit\n");
    return -1;
  }
  aux->device = emokit_create();
  //printf("connected: %d\n", emokit_get_count(auxiliary->device, EMOKIT_VID, EMOKIT_PID));
  if (emokit_open(aux->device, EMOKIT_VID, EMOKIT_PID, 1) != 0)  {
    fprintf(stderr, "init_emoinp: could not open emokit\n");
    return -1;
  }
  aux->buffer = (double*)malloc(data_size(output));
  pipe_set_auxiliary(p, aux);

  return 1;
}

int emotiv_run(pipe_* p)  { 
  data *output = pipe_get_output(p);
  struct auxiliary_emokit *aux = (struct auxiliary_emokit*)pipe_get_auxiliary(p);
  struct emokit_frame frame = aux->frame;
  struct emokit_device *device = aux->device;
  double *buffer = aux->buffer;
  int *shape = data_get_shape(output);

  for (int i = 0; i < shape[1]; i++)  {
    int read;
    if ((read = emokit_read_data_timeout(device, 1000)) > 0)  {
      frame = emokit_get_next_frame(device);
      for (int j = 0; j < shape[0]; j++)  {
        buffer[j*shape[1]+i] = *(&frame.F3 + j);
      }
    }
    else if (read == 0)  {
      fprintf(stderr, "run_emoinp: EEG timeout\n");
      return -1;
    }
    else  {
      fprintf(stderr, "run_emoinp: EEG error\n");
      return -1;
    }
  }
  data_copy_to_data(output, buffer);
//  emokit_close(device);
//  emokit_delete(device);
 
  return 1;
}

int emotiv_kill(pipe_* p, linkedlist* l)  {
  return 1;
}
