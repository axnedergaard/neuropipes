CC = gcc
maindir = src/
pipesdir = src/pipes/
main = debug_pipe.c hashtable.c linkedlist.c data.c pipe.c register_pipes.c piperegistry.c pipeline.c
pipes = pipe_mean.c pipe_inversefouriertransform.c pipe_dummyemotiv.c pipe_alphabandpass.c pipe_fouriertransform.c pipe_print.c pipe_emotiv.c pipe_writefile.c pipe_readfile.c
ext = emokit.c
maindeps = $(addprefix $(maindir), $(main))
pipesdeps = $(addprefix $(pipesdir), $(pipes))
extdeps = $(addprefix $(maindir), $(ext))
emoflags = -lmcrypt -lhidapi-libusb
miscflags = -lfftw3 -lliquid -lcmocka -lm
cflags = -g -Wall -std=c99 
depflags = -Ishared/include -Lshared/lib

all : driver

clean : 
	rm -f driver tests/test

driver :  $(maindir)driver.c $(maindeps) $(pipesdeps)
	$(CC) -o driver $(maindir)driver.c $(maindeps) $(pipesdeps) $(extdeps) $(depflags) $(emoflags) $(miscflags) $(cflags)

test :	tests/test_pipeline.c $(maindeps) $(pipedeps) 
	$(CC) -o tests/test tests/test_pipeline.c $(maindeps) $(pipesdeps) $(extdeps) $(depflags) $(emoflags) $(miscflags) $(cflags)
