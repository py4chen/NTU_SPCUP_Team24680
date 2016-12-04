AUBIO_PATH=../../aubio/build/src # path of aubio
AUBIO_SRC=../../aubio/src

flow: ./flow.c
	sh -c 'echo oneshot > /sys/class/leds/led0/trigger'
	sudo sh -c 'echo 1 > /sys/class/leds/led0/shot'
	gcc flow.c -Wall -l asound -L $(AUBIO_PATH) -l aubio -I $(AUBIO_SRC) -o flow

run:
	sudo ./flow

clean:
	rm -f ./flow || true

