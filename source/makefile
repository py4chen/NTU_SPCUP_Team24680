AUBIO_PATH=../../aubio/build/src # path of aubio
AUBIO_SRC=../../aubio/src

flow: flow.c
	gcc flow.c -Wall -l asound -L $(AUBIO_PATH) -l aubio -I $(AUBIO_SRC) -o flow


