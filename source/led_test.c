#include <stdio.h>

#ifndef LED_C
#include "led.c"
#endif

#ifndef STRUCTURE_LIB
#include "structure.h"
#endif

int main(){
	fprintf(stderr ,"%llu\n",
		getCurrentTimestamp());
	ledACT();
	fprintf(stderr ,"%llu\n",
		getCurrentTimestamp());
	return 0;
}