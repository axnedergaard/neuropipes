CC = gcc
maindeps = debug_pipe.c hashtable.c linkedlist.c data.c pipe.c piperegistry.c pipeline.c
pipes = pipe_normalise.c pipe_mean.c pipe_web.c pipe_graph.c pipe_inversefouriertransform.c pipe_dummyemotiv.c pipe_alphabandpass.c pipe_fouriertransform.c pipe_print.c pipe_emotiv.c
extdeps = emokit.c
emoflags = -lmcrypt -lhidapi-libusb
miscflags = -lfftw3 -lliquid -lm -g -Wall

all : cli driver

clean : 
	rm -f driver

driver :  driver.c $(maindeps) $(pipes)
	$(CC) -o driver driver.c $(maindeps) $(pipes) $(extdeps) $(emoflags) $(miscflags)
