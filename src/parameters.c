#include "parameters.h"
#include <stdlib.h>
#include <string.h>

int tokenise(char *spec, char **ptype, int *pparams_n, char ***pparams)  {
  int params_n = 0;
  char **params = NULL;
  char *type = NULL;
  char *spec_token = strdup(spec);
  char *token = strtok(spec_token, ";");
  if (token != NULL)  {
    type = strdup(token);  
    token = strtok(NULL, ";"); //parameters
    if (token != NULL)  { 
      //get param_n
      params_n++;
      for (int i = 0; i < strlen(token); i++)  {
        if (token[i] == ',')  {
          params_n++;
        }
      }
      params = (char**)malloc(sizeof(char*)*params_n);
      token = strtok(token, ",");
      for (int i = 0; i < params_n; i++)  {
        params[i] = strdup(token);
        token = strtok(NULL, ",");
      }
    }
  }
  free(spec_token);
  
  *ptype = type;
  *pparams_n = params_n;
  *pparams = params;

  return 1;
}

char *get_parameter(pipe_* p, char *s)  {
  char *s_tok = NULL; 
  for (int i = 0; i < p->params_n; i++)  {
    s_tok = strdup(p->params[i]);
    if (strcmp(strtok(s_tok, "="), s) == 0)  {  //param matched
      return strdup(strtok(NULL, "="));
    }
    free(s_tok);
  }
  return NULL;  //fail
}

