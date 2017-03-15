CC = gcc
maindir = src/
pipesdir = src/pipes/
main = concurrent_pipe.c gettime.c debug_pipe.c hashtable.c linkedlist.c data.c pipe.c parameters.c register_pipes.c pipebuilder.c piperegistry.c pipeline.c
pipes = pipe_power.c pipe_dummy_computation.c pipe_writesharedmem.c pipe_inversefouriertransform.c pipe_dummyemotiv.c pipe_filter.c pipe_fouriertransform.c pipe_print.c pipe_emotiv.c pipe_writefile.c pipe_readfile.c
ext = emokit.c edflib.c fidlib.c
maindeps = $(addprefix $(maindir), $(main))
pipesdeps = $(addprefix $(pipesdir), $(pipes))
extdeps = $(addprefix $(maindir), $(ext))
lflags = -lhidapi-libusb -lmcrypt -lfftw3 -lcmocka -lm -lpthread -DT_LINUX
cflags = -Wall -g -pg -std=gnu99

all : driver

clean : 
	rm -f driver tests/test

driver :  $(maindir)driver.c $(maindeps) $(pipesdeps)
	$(CC) -o driver $(maindir)driver.c $(maindeps) $(pipesdeps) $(extdeps) $(lflags) $(cflags)

test :	tests/test.c $(maindeps) $(pipedeps) 
	$(CC) -o tests/test tests/test.c $(maindeps) $(pipesdeps) $(extdeps) $(lflags) $(cflags)
