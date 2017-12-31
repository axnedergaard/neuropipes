#include "neuropipes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COMMAND_MAX 128

#define HELP 0
#define QUIT 1
#define ADD 2
#define LINK 3
#define INIT 4
#define RUN 5
#define LIST 6
#define SHOW 7
#define EXECUTE 8
#define RESET 9
#define ADD_CONCURRENT 10
#define SEGMENT 11
#define START 12
#define STOP 13
#define KILL 14

const char *welcome_string = "[NEUROPIPES]\n";

const char *help_string = \
"Available commandss:\n\
help: print this message\n\
quit: end program\n\
add: add pipe to pipeline\n\
link: add edge between pipes\n\
init: initialise pipeline\n\
run: run pipeline\n\
list: list available pipes\n\
show: show adjacency matrix for pipe edges\n\
reset: remove all pipes and links\n"\
;

void get_string(char *buffer, const char *message)  {
    printf(message);
    fgets(buffer, COMMAND_MAX, stdin);
    buffer[strlen(buffer)-1] = '\0';
}

int equal(char* l, char *s)  {
  int min = (strlen(l) < strlen(s)) ? strlen(l) : strlen(s);
  int matched = 0;
  for (int i = 0; i < min; i++)  {
    if (l[i] == ' ')  return matched;  //return true if at least one char matched and line string ends
    else if (l[i] == s[i])  matched = 1;
    else return 0;
  }
  return matched;
}

int main()  {
  printf(welcome_string);

  pipeline *pl = pipeline_create();
  char **available_pipes = NULL;
  int available_pipes_n = list_available_pipes(&available_pipes);
  int segments = 0;
  int command = HELP;
  char line[COMMAND_MAX];
  int quit = 0;
  while (quit != 1)  {
    get_string(line, "");
    if (equal(line, "quit"))  command = QUIT;
    else if (equal(line, "add"))  command = ADD;
    else if (equal(line, "cadd")) command = ADD_CONCURRENT;
    else if (equal(line, "link"))  command = LINK;
    else if (equal(line, "init"))  command = INIT;
    else if (equal(line, "run"))  command = RUN;
    else if (equal(line, "list"))  command = LIST;
    else if (equal(line, "show"))  command = SHOW;
    else if (equal(line, "reset"))  command = RESET;
    else if (equal(line, "segment")) command = SEGMENT;
    else if (equal(line, "start")) command = START;
    else if (equal(line, "stop")) command = STOP;
    else if (equal(line, "kill")) command = KILL;
    else if (equal(line, "exexute")) command = EXECUTE;
    else  command = HELP;

    if (command == HELP)  {
      printf(help_string);
    }
    else if (command == QUIT)  {
      quit = 1;
    }
    else if (command == ADD)  {
      char *tok = strtok(line, " ");
      tok = strtok(NULL, " ");
      if (tok == NULL)  {
        printf("Usage: add [pipe name] {segment}\n");
      }
      else  {
	char type[COMMAND_MAX];
	strncpy(type, tok, COMMAND_MAX);

	int segment = 0;
        if (segments > 0)  {
          tok = strtok(NULL, " ");
	  if (tok != NULL)  {
            segment = atoi(tok);
	  }
	}

        int id = pipeline_add(pl, type, segment);
	if (id >= 0)  printf("[Added pipe %d with type %s to segment %d]\n", id, type, segment); 
      }
    }
    else if (command == LINK)  {
      char *tok = strtok(line, " "); 
      tok = strtok(NULL, " ");
      if (tok == NULL)  {
        printf("Usage: link [input pipe id] [output pipe id]\n");
      }
      else  {
	int input = atoi(tok);
        tok = strtok(NULL, " ");
	if (tok == NULL)  {
          printf("Usage: link [input pipe id] [output pipe id]\n");
	}
	else  {
          int output = atoi(tok);
          pipeline_link(pl, input, output);
	  printf("[Added edge from pipe %d to %d]\n", input, output);
	}
      }
    } 
    else if (command == INIT)  {
      pipeline_init(pl);
      printf("[Initialised pipes]\n");
    }
    else if (command == RUN)  {
      int runs = 1;
      char *tok = strtok(line, " ");
      tok = strtok(NULL, " ");
      if (tok != NULL)  {
        runs = atoi(tok);
      }
      for (int i = 0; i < runs; i++)  {
        pipeline_run(pl);
      }
      printf("[Ran pipes %d time(s)]\n", runs);
    }
    else if (command == LIST)  {
      for (int i = 0; i < available_pipes_n; i++)  printf("%s\n", available_pipes[i]); 
    }
    else if (command == SHOW)  {
      for (int i = 0; i < pipeline_size(pl); i++)  {
        char *type = pipe_get_type(pipeline_get_pipe(pl, i));
	printf("%d: %s\n", i, type);
      }

      int *matrix = pipeline_adjacency_matrix(pl);
      int n = pipeline_size(pl);
      for (int i = 0; i < n; i++)  {
        for (int j = 0; j < n; j++)  {
          printf("%d ", matrix[i*n + j]);
	}	
	printf("\n");
      }
      free(matrix);
    }
    else if (command == RESET)  {
      pipeline_destroy(pl);
      pl = pipeline_create();
      segments = 0;
      printf("[Reset pipeline]\n");
    }
    else if (command == SEGMENT)  {
      segments = pipeline_segment(pl);
      printf("[Created segment %d]\n", segments);
    }
    else if (command == START)  {
      pipeline_start_threads(pl);
      printf("[Started threads]\n");
    }
    else if (command == STOP)  {
      pipeline_stop_threads(pl);
      printf("[Stopped threads]\n");
    }
    else if (command == KILL)  {
      pipeline_kill(pl);
      printf("[Killed pipes]\n");
    }
    else if (command == EXECUTE)  {
      int runs = 1;
      char *tok = strtok(line, " ");
      tok = strtok(NULL, " ");
      if (tok != NULL)  {
        runs = atoi(tok);
      }
      pipeline_init(pl);
      pipeline_start_threads(pl);
      for (int i = 0; i < runs; i++) {
        pipeline_run(pl);
      }
      pipeline_stop_threads(pl);
      pipeline_kill(pl);
      printf("[Executed pipeline with %d run(s)]\n", runs);
    }
  }

  return 0;
}
