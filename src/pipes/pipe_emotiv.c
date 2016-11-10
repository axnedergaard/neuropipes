#include "../pipe.h"
#include "../emokit.h"
#include <stdio.h>
#include <stdlib.h>

//EMOINP
struct auxiliary_emokit  {
  struct emokit_frame frame;
  struct emokit_device* device;
 // struct emokit_contact_quality quality;
};

int emotiv_init(pipe_* p, linkedlist* l)  {
  if (linkedlist_size(l) > 0)  {
    fprintf(stderr, "emoinp_init: pipe_ cannot have any inputs\n");
    return -1;
  }

  p->output = data_create_from_string("EMOTIV");

  //init auxiliary structure
  struct auxiliary_emokit* auxiliary = (struct auxiliary_emokit*)malloc(sizeof(struct auxiliary_emokit));
  if (auxiliary == NULL)  {
    fprintf(stderr, "init_emoinp: failed to allocate memory for auxiliary emokit\n");
    return -1;
  }
  auxiliary->device = emokit_create();
  //printf("connected: %d\n", emokit_get_count(auxiliary->device, EMOKIT_VID, EMOKIT_PID));
  if (emokit_open(auxiliary->device, EMOKIT_VID, EMOKIT_PID, 1) != 0)  {
    fprintf(stderr, "init_emoinp: could not open emokit\n");
    return -1;
  }
  p->auxiliary = auxiliary;

  return 1;
}

int emotiv_run(pipe_* p)  { 
  //printf("running emoinp\n");
  struct emokit_frame frame = (struct emokit_frame)((struct auxiliary_emokit*)p->auxiliary)->frame;
  struct emokit_device *device = (struct emokit_device*)((struct auxiliary_emokit*)p->auxiliary)->device;

  for (int i = 0; i < p->output->shape[1]; i++)  {
    int read;
    if ((read = emokit_read_data_timeout(device, 1000)) > 0)  {
      frame = emokit_get_next_frame(device);
      for (int j = 0; j < p->output->shape[0]; j++)  {
        p->output->buffer[j*p->output->shape[1]+i] = *(&frame.F3 + j);
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
//  emokit_close(device);
//  emokit_delete(device);
 
  return 1;
}


