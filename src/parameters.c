#include "parameters.h"
#include <stdlib.h>
#include <string.h>

int tokenise(char *spec, char **p_type, int *p_params_n, char ***p_params)  {
  int params_n = 0;
  char **params = NULL;
  char *type = NULL;

  char *spec_token = strdup(spec);
  char *token = strtok(spec_token, ";");  //separate type and parameters
  if (token != NULL)  {
    type = strdup(token);  //type
    //separate parameters
    token = strtok(NULL, ";");
    if (token != NULL)  { 
      //get number of params
      params_n++;
      for (int i = 0; i < strlen(token); i++)  {
        if (token[i] == ',')  {
          params_n++;
        }
      }
      //get params
      params = (char**)malloc(sizeof(char*)*params_n);
      token = strtok(token, ",");
      for (int i = 0; i < params_n; i++)  {
        params[i] = strdup(token);
        token = strtok(NULL, ",");
      }
    }
  }
  free(spec_token);
  
  *p_type = type;
  *p_params_n = params_n;
  *p_params = params;

  return 1;
}

char *get_parameter(pipe_* p, char *s)  {
  char *matched_param = NULL;
  char *s_tok = NULL;
  int matched = 0;
  //check if any parameters of p match s, end if match
  for (int i = 0; matched == 0 && i < pipe_get_params_n(p); i++)  {
    s_tok = strdup(pipe_get_params(p)[i]);
    if (strcmp(strtok(s_tok, "="), s) == 0)  {  //param matched
      matched_param = strdup(strtok(NULL, "="));
      matched = 1;
    }
    free(s_tok);
  }
  return matched_param; //note: user must dealloc matched_param themselves
}

int set_parameter_int(pipe_ *p, char *s, int *d)  {
  char *param = get_parameter(p, s);
  if (param != NULL)  {
    *d = atoi(param);
    free(param);
    return 1;
  }
  return 0;
}

int set_parameter_double(pipe_ *p, char *s, double *d)  {
  char *param = get_parameter(p, s);
  if (param != NULL)  {
    *d = atof(param);
    free(param);
    return 1;
  }
  return 0;
}


int set_parameter_string(pipe_ *p, char *s, char *d)  { //note that char* and not pointer to char* is used, unlike for int or double
  char *param = get_parameter(p, s);
  if (param != NULL)  {
    strncpy(d, param, PARAM_MAX);
    free(param);
    return 1;
  }
  return 0;
}	
