#include "../pipe.h"
#include "../data.h"

#include <stdio.h>

int print_init(pipe_ *p, linkedlist *l)  {
  if (linkedlist_size(l) <= 0)  {
    fprintf(stderr, "init_print: print pipe must have 1 or more inputs\n");
  }
  linkedlist_reset_iterater(l);
  p->output = NULL;  //print pipe has no output
  return 1;
}

int print_run(pipe_ *p, linkedlist *l)  {
  data** input_pointer = NULL;

  while ((input_pointer = (data**)linkedlist_iterate(l)) != NULL)  {
    data* input = *input_pointer;
    data_write(input, stdout);
    /*
    if (input->n > 2)  {
      printf(">2d printing not implemented\n");
    }
    else if (input->n > 1)  {
      for (int i = 0; i < input->shape[0]; i += input->stride[0])  {
        printf("[");
        for (int j = 0; j < input->shape[1]; j += input->stride[1])  {
          if (input->stride[1] == 1)  {//double
            printf("%.1f", input->buffer[i*input->shape[1]+j]);;
            //space if not last element
            if (j + 1 != input->shape[1])  {
              printf(", ");
            }
          }
          else  {  //complex
            printf("%.1f + %.1fi", input->buffer[i*input->stride[0]*input->shape[1] + j], input->buffer[i*input->stride[0]*input->shape[1] + j + 1]);
            //space if not last element
            if (j + 2 != input->shape[1])  {
              printf(", ");
            }
          }
        }
        printf("]");
        if (i*input->stride[0] + 1 != input->shape[0])  {
          printf("\n");
        }
      }
    }
    else if (input->n > 0)  {
      for (int i = 0; i < input->shape[0]; i++)  {
        printf("%.1f ", input->buffer[i]);
      }
      printf("\n");
    }
    else  {
      printf("%.1f\n", input->buffer[0]);
    }
    printf("\n");
    input = NULL;
  */}
  linkedlist_reset_iterater(l);

  return 1; 
}

