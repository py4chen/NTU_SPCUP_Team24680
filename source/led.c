#define LED_C

#include <stdio.h>
#include <unistd.h>

#ifndef STRUCTURE_LIB
#include "structure.h"
#endif

void _ledACT(){
	FILE *f = fopen("/sys/class/leds/led0/shot", "w");
	if (f == NULL)
	{
    	printf("Error opening file!\n");
	}
	const char *text = "1";
	fprintf(f, "%s", text);
	fclose(f);
}

void ledACT(Message *addr){

	printf("LED0! Current Time:%llu  Relative Time:%llu \n\n", 
		getCurrentTimestamp(), getCurrentTimestamp()-addr->start_time);
}

