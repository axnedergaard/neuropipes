#include "../pipe.h"
#include "../parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //for UART
#include <fcntl.h>  //for UART
#include <termios.h>  //for UART

//OpenEEG-formated (P2) Raspberry Pi serial port EEG input
//reference for UART: http://www.raspberry-projects.com/pi/programming-in-c/uart-serial-port/using-the-uart
//reference for OpenEEG data format: http://openeeg.blogspot.dk/2009/10/openeeg-boards-testing.html

#define RPI_UAF_LEN 16
#define RPI_BUF_LEN 6

//auxiliary structure to hold filestream and buffers
struct rpi_aux {
  unsigned char uaf_buf[RPI_UAF_LEN];  //buffer for OpenEEG format data
  double buf[RPI_BUF_LEN]; //buffer for signal in native format of neuropipes
  int uart_fs;  //UART filestream
};


int rpi_init(pipe_* p, linkedlist* l)  {
  //create auiliary structure
  struct rpi_aux *aux = (struct rpi_aux*)malloc(sizeof(struct rpi_aux));
  pipe_set_auxiliary(p, aux);
  
  //open UART filestream
  int uart_fs = open("/dev/ttyUSB0", O_RDONLY);
  if (uart_fs == -1)  {  //failed to open filestream
    fprintf(stderr, "rpi_init: failed to open UART filestream\n");
    return -1;
  }
  struct termios options;
  tcgetattr(uart_fs, &options);
  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(uart_fs, TCIFLUSH);
  tcsetattr(uart_fs, TCSANOW, &options);
  aux->uart_fs = uart_fs;

  //initialise buffer
  for (int i = 0; i < RPI_BUF_LEN; i++)  aux->buf[i] = 0x00; 

  //create data structure
  int channels = RPI_BUF_LEN;
  int records = 1;
  int n = 2;
  int shape[n], stride[n];
  stride[0] = 1;
  stride[1] = 1;
  shape[0] = channels;
  shape[1] = records;
  data *output = data_create(n, shape, stride);
  pipe_set_output(p, output);

  return 1;
}

int rpi_run(pipe_* p)  { 
  data *output = pipe_get_output(p);  //retrieve data structure
  struct rpi_aux *aux = (struct rpi_aux*)pipe_get_auxiliary(p);  //retrieve auxiliary structure

  //read UART filestream and parse OpenEEG data format bytes into data structure
  int uart_fs = aux->uart_fs; 
  unsigned char *uaf_buf = aux->uaf_buf;  
  double *buf = aux->buf;
  int begin_byte = 4;  //first byte in OpenEEG format with signal data
  int uaf_len = read(uart_fs, (void*)uaf_buf, RPI_UAF_LEN);
  if (uaf_len > 0)  {  //bytes read
    //parse bytes into buffer
    for (int i = 0; i < RPI_BUF_LEN; i++)  { 
      int high_byte = uaf_buf[2*i + begin_byte];
      int low_byte = uaf_buf[2*i + 1 + begin_byte];
      buf[i] = (high_byte << 8) | low_byte;
    }
    data_copy_to_data(output, buf); //copy from buffer to output data structure
  }
  else if (uaf_len == 0)  { //no bytes to read 
    fprintf(stderr, "rpi_run (warning): no bytes read from UART filestream\n");
  }
  else  {  //error occurred
    fprintf(stderr, "rpi_run: failed to read from UART filestream\n");
    return -1;
  }
  
  return 1;
}

int rpi_kill(pipe_* p, linkedlist* l)  {
  struct rpi_aux *aux = (struct rpi_aux*)pipe_get_auxiliary(p);
  close(aux->uart_fs);
  return 1;
}
