#include <stdio.h>
#include <stdlib.h>
#include "structure.h"

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

	long long t = getCurrentTimestamp() - addr->start_time;
  long long t_sec = t/1000000;
	fprintf(f_led, "%lld.%lld\n", t_sec, t-t_sec*1000000);
	fflush(f_led);
  return;
}

void _ledACT(Message *addr){
	FILE *fp = fopen("./text.txt", "aw");
	fprintf(fp ,"LED0! Current Time:%llu  Relative Time:%llu \n\n", 
		getCurrentTimestamp(), getCurrentTimestamp()-addr->start_time);
	fclose(fp);
}

