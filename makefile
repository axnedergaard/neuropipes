CC = gcc
maindir = src/
pipesdir = src/pipes/
main = concurrent_pipe.c gettime.c debug_pipe.c hashtable.c linkedlist.c data.c pipe.c register_pipes.c piperegistry.c pipeline.c
pipes = pipe_dummyemotivconcurrent.c pipe_mean.c pipe_inversefouriertransform.c pipe_dummyemotiv.c pipe_alphabandpass.c pipe_fouriertransform.c pipe_print.c pipe_emotiv.c pipe_writefile.c pipe_readfile.c
ext = emokit.c
maindeps = $(addprefix $(maindir), $(main))
pipesdeps = $(addprefix $(pipesdir), $(pipes))
extdeps = $(addprefix $(maindir), $(ext))
lflags = -lhidapi-libusb -lmcrypt -lfftw3 -lliquid -lcmocka -lm -lpthread
cflags = -Wall -g -pg -std=gnu99

all : driver

clean : 
	rm -f driver tests/test

driver :  $(maindir)driver.c $(maindeps) $(pipesdeps)
	$(CC) -o driver $(maindir)driver.c $(maindeps) $(pipesdeps) $(extdeps) $(lflags) $(cflags)

test :	tests/test_pipeline.c $(maindeps) $(pipedeps) 
	$(CC) -o tests/test tests/test_pipeline.c $(maindeps) $(pipesdeps) $(extdeps) $(lflags) $(cflags)
