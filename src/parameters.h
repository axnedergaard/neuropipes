#ifndef PARAMETER_H
#define PARAMETER_H

#include "pipe.h"

#define PARAM_MAX 128  //max length of parameter

int tokenise(char*, char**, int*, char***); //separate pipe specification into type, number of parameters and parameters
char *get_parameter(pipe_*, char*);  //get parameter as string
int set_parameter_int(pipe_*, char*, int*);
int set_parameter_double(pipe_*, char*, double*);
int set_parameter_string(pipe_*, char*, char*);

#endif
