#ifndef PARAMETER_H
#define PARAMETER_H

#include "pipe.h"

int tokenise(char *spec, char **type, int *params_n, char ***params);
char *get_parameter(pipe_*, char *s);

#endif
