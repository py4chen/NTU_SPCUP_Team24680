#define LED_C

#include <stdio.h>
#include <unistd.h>

#ifndef STRUCTURE_LIB
#include "structure.h"
#endif

void ledACT(Message *addr){
	FILE *f = fopen("/sys/class/leds/led0/shot", "w");
	if (f == NULL)
	{
   		printf("Error opening file!\n");
    	exit(-1);
	}
	const char *text = "1";
	fprintf(f, "%s", text);
	fclose(f);

	FILE *f_led = fopen("./log_led.txt", "aw");
	long long t = getCurrentTimestamp() - addr->start_time;
	fprintf(f_led, "%lld.%lld\n", t/1000000, t-t/1000000);
	fclose(f_led);
}

void _ledACT(Message *addr){
	FILE *fp = fopen("./text.txt", "aw");
	fprintf(fp ,"LED0! Current Time:%llu  Relative Time:%llu \n\n", 
		getCurrentTimestamp(), getCurrentTimestamp()-addr->start_time);
	fclose(fp);
}

