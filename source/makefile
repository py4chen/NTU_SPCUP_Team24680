AUBIO_PATH=../../aubio/build/src # path of aubio
AUBIO_SRC=../../aubio/src

flow: ./flow.c ./led.c ./detector.c ./structure.h
	sh -c 'echo oneshot > /sys/class/leds/led0/trigger'
	sudo sh -c 'echo 1 > /sys/class/leds/led0/shot'
	gcc flow.c led.c detector.c -Wall -l asound -L $(AUBIO_PATH) -l aubio -I $(AUBIO_SRC) -o flow

run:
	sudo ./flow

ledtest:
	sh -c 'echo oneshot > /sys/class/leds/led0/trigger'
	sudo sh -c 'echo 1 > /sys/class/leds/led0/shot'
	gcc led_test.c -o ledtest
	./ledtest

clean:
	rm -f ./flow || true
	rm -f ./ledtest || true

