CC = gcc
maindir = src/
pipesdir = src/pipes/
main = debug_pipe.c hashtable.c linkedlist.c data.c pipe.c register_pipes.c piperegistry.c pipeline.c
pipes = pipe_mean.c pipe_inversefouriertransform.c pipe_dummyemotiv.c pipe_alphabandpass.c pipe_fouriertransform.c pipe_print.c pipe_emotiv.c pipe_fileout.c
ext = emokit.c
maindeps = $(addprefix $(maindir), $(main))
pipesdeps = $(addprefix $(pipesdir), $(pipes))
extdeps = $(addprefix $(maindir), $(ext))
emoflags = -lmcrypt -lhidapi-libusb
miscflags = -lfftw3 -lliquid -lm -g -Wall

all : driver

clean : 
	rm -f driver

driver :  $(maindir)driver.c $(maindeps) $(pipesdeps)
	$(CC) -o driver $(maindir)driver.c $(maindeps) $(pipesdeps) $(extdeps) $(emoflags) $(miscflags)
