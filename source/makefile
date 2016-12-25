AUBIO_PATH=../../aubio/build/src # path of aubio
AUBIO_SRC=../../aubio/src

## RGB_LED
LED_BLUE=17
LED_GREEN=27
LED_RED=22
LED_ENABLE=0
LED_DISABLE=1
RGB_ENABLE=1
RGB_DISABLE=0

flow: ./flow.c ./led.c ./detector.c ./structure.h
	sh -c 'echo oneshot > /sys/class/leds/led0/trigger'
	sudo sh -c 'echo 1 > /sys/class/leds/led0/shot'
	gcc flow.c led.c detector.c sound.c -Wall -l asound -L $(AUBIO_PATH) -l aubio -I $(AUBIO_SRC) -o flow

	#LED RGB
	#Set up GPIO to Outputs
	# Set up GPIO Pin, to output and put in disable state
	echo $(LED_RED) > /sys/class/gpio/export
	echo $(LED_GREEN) > /sys/class/gpio/export
	echo $(LED_BLUE) > /sys/class/gpio/export
	echo "out" > /sys/class/gpio/gpio$(LED_RED)/direction
	echo "out" > /sys/class/gpio/gpio$(LED_GREEN)/direction
	echo "out" > /sys/class/gpio/gpio$(LED_BLUE)/direction
	echo $(RGB_DISABLE) > /sys/class/gpio/gpio$(LED_RED)/value
	echo $(RGB_DISABLE) > /sys/class/gpio/gpio$(LED_GREEN)/value
	echo $(RGB_DISABLE) > /sys/class/gpio/gpio$(LED_BLUE)/value

run:
	sudo ./flow

ledtest:
	sh -c 'echo oneshot > /sys/class/leds/led0/trigger'
	sudo sh -c 'echo 1 > /sys/class/leds/led0/shot'
	gcc led_test.c -o ledtest
	./ledtest

ledRGBtest:
	echo 1 > /sys/class/gpio/gpio$LED_RED/value
	echo 1 > /sys/class/gpio/gpio$LED_GREEN/value
	echo 1 > /sys/class/gpio/gpio$LED_BLUE/value
	echo 0 > /sys/class/gpio/gpio$LED_RED/value
	echo 0 > /sys/class/gpio/gpio$LED_GREEN/value
	echo 0 > /sys/class/gpio/gpio$LED_BLUE/value


clean:
	rm -f ./flow || true
	rm -f ./ledtest || true

	echo $LED_RED > /sys/class/gpio/unexport
	echo $LED_GREEN > /sys/class/gpio/unexport
	echo $LED_BLUE > /sys/class/gpio/unexport

