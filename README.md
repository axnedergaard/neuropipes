# neuropipes
An efficient and extensible real-time EEG signal processing framework written in C

Dependencies: mcrypt, hidapi-libusb, fftw3, cmocka (for testing)

Supported EEG systems: EMOTIV Epoc

I INSTALLATION INSTRUCTIONS (Arch Linux)
sudo pacman -S hidapi fftw libmcrypt
git clone https://github.com/axnedergaard/neuropipes
cd neuropipes
make

Note: you will need cmocka (sudo pacman -S cmocka) if you want to run unit tests.

II API
Pipeline provides the callable interface of neuropipes through the functions declared in pipeline.h.
A pipeline instance is created with pipeline_create() and destroyed with pipeline_destroy().
1) Construction: Add pipes with pipeline_insert() and connect them with pipeline_insert_edge. Optionally set loop and interval variables with pipeline_set_loop() and pipeline_set_interval(). Loop specifies how many times the pipeline should run (0 indicates infinite loop), and interval many seconds each run should mimimally take.
  a) pipeline_insert() takes as parameters the pipeline instance, the pipe specification (format: "TYPE;param1=value1,param2=value2") and a boolean indicating whether the pipe is concurrent (should run on a separate thread). Note that concurrent pipes connected to concurrent pipes are not supported. The function will return the id of the pipe.
  b) pipeline_insert_edge() takes as parameters the pipeline instance, and the ids of two pipes to connect. The first pipe will provide input to the second pipe.
2) Initialisation: Initialise the pipeline with pipeline_init()
3) Running: Run the pipeline with pipeline_run()

Example usage code (get concurrent input from Emotiv EEG device and write to shared memory location with key 9000 until interrupted:
#include "pipeline.c"

int main()  {
  pipeline *pl = pipeline_create();
  
  pipeline_set_loop(pl, 0)

  int emotiv = pipeline_insert(pl, "EMOTIV", 1);
  int writeshared = pipeline_insert(pl, "WRITESHARED;key=9000);
  pipeline_insert_edge(pl, emotiv, shared);

  pipeline_init(pl);
  pipeline_run(pl);

  pipeline_destroy(pl);
 
  return 0
}

Implemented pipes types with (parameters):
EMOTIV; Emotiv EEG input (14 channels).
FOURIERTRANSFORM: Fast Fourier Transform (FFT).
INVERSEFOURIERTRANSFORM: Inverse FFT.
FILTER (pass, order, lc, hc, rate, samplefreq): Perform Butterworth filter of order 'order' (default 2). The type of filter (bandpass, low-pass or high-pass) is specified by 'pass', options are 'band', 'low' and 'high' (default 'band'). Sampling rate is specified by 'rate' (default 128). Low-cutoff frequency is specified by 'lc' (default 8) and high-cutoff frequency by 'hc' (default 12). Sampling freqeuncy is specified by 'samplefreq' (default 128). Unparameterised, this specifies a second order alpha-wave bandpass filter.
POWER: Compute absolute values of complex input, can be connected to a FFT pipe to get power spectrum.
READFILE (filename): Read input from EDF-PLUS formatted file specified by 'filename' (default recording.edf).
WRITEFILE (filename): Write output to EDF-PLUS formatted file specified by 'filename' (default recording.edf).
WRITESHAREDMEM (key): Write output to Unix System V shared memory segment with key 'key' (default 6667).
PRINT: Write output to standard output.
DUMMYEMOTIV (random): Simulate Emotiv EEG input. 'random' (default 0) is boolean flag that specifies whether simulated values should be random or a linear sequence.
DUMMYCOMPUTATION (replace, replace_value): Pass input to output (do nothing), or change values to 'replace_value' (default 0) depending on boolean flag replace (default 0).


III How to write new pipes
1) Write code for pipe
  a) Create a new C file src/pipes/pipe_NAME.c
  b) Add '#include "../pipe.h"' to beginning of file ('#include "../parameters.h"' if you want to parameterise your pipe)
  c) Define the functions int 'NAME_init(pipe_* p, linkedlist *l)', 'int NAME_run(pipe_ *p, linkedlist *l)' and 'int NAME_kill(pipe_* p, linkedlist *l)'
    i) Functions must return 1 on success, <0 on failure and 0 to request pipeline termination.
    ii) Parameters: p is the calling pipe instance, l is a linkedlist with input data from pipes connected to this pipe.
    iii) (See 'Details for writing pipes' for more additional instructions)
2) Register pipes in register_pipes.c:
  a) Add function declarations 'NAME_init(pipe_* p, linkedlist *l) 'int NAME_run(pipe_ *p, linkedlist *l)' and 'int NAME_kill(pipe_* p, linkedlist *l)' under the other function declarations
  b) In the function definition register_pipes(), add the line 'piperegistry_register("NAME", &NAME_init, &NAME_run, &NAME_kill, "")' under the other function calls.
3) Modify makefile and compile:
  a) Add pipe_NAME.c at the end of the 'pipes=' line in makefile
  b) make
4) You can now use your pipe with pipeline_insert(pl, "NAME")

Details for writing pipes:
_init should allocate memory and perform pre-run operations (e.g. opening files and devices, calculations). If your pipe provides an output, you must create and store it during init, using 'data *d = data_create...();' (see 'Creating data structures') and store it using 'pipe_set_output(p, d);'
_run should contain code to be called while the pipeline is running, including copying from/to data structures
_kill should free memory and perform post-run operations (e.g. closing files and devices). You should not free memory for output data or auxiliary structures, but you should free memory allocated to attributes of an auxiliary structure.
You may define an auxiliary struct to store additional variables and calculations. An instance of the struct s should be created and stored during init. Use 'pipe_set_auxiliary(p, s);' to store during init, and pipe_get_auxiliary(p) to retrieve during run and kill.
data.h provides the following functions to create a data structure: data_create(), data_create_from(), data_create_real_from_complex(), data_create_complex_from_real
data.h provides the following functions to copy to/from a data structure: data_copy_to(), data_copy_from()
There data buffer can be accessed directly with data_get_buffer(). If you copy manually, must explicitly lock and unlock the data structure using data_write_lock and data_write_unlock, or data_read_lock and data_read_unlock()
You can access with input data with 'data *input = *(data**)linkedlist_head(l);' (use linkedlist_iterate(l) if pipe has multiple inputs, remember to call linked_reset_iterater(l) when done).
You can access parameters with get_parameter(), which returns a char* with the parameter value if the parameter is found, NULL otherwise. You must deallocate memory for the returned value once you have used it.