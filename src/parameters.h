#ifndef PARAMETER_H
#define PARAMETER_H

#include "pipe.h"

#define PARAM_MAX 32  //max length of parameter

int tokenise(char*, char**, int*, char***); //separate pipe specification into type, number of parameters and parameters
char *get_parameter(pipe_*, char*);  //get parameter as string
int get_parameter_int(pipe_*, char*);
double get_parameter_double(pipe_*, char*);

#endif
