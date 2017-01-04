#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "structure.h"



void ledACT(Message *addr){
	
	if (f == NULL)
	{
   		printf("Error opening file!\n");
    	exit(-1);
	}
	const char *text = "1";
	fprintf(f, "%s", text);


	long long t = getCurrentTimestamp() - addr->start_time;
  	long long t_sec = t/1000000;

	//fprintf(stdout, "%lld.%lld\n", t_sec, t-t_sec*1000000);
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


// FILE *fR = fopen("/sys/class/gpio/gpio22/value", "w");
// FILE *fG = fopen("/sys/class/gpio/gpio27/value", "w");
// FILE *fB = fopen("/sys/class/gpio/gpio17/value", "w");

void ledRGB_setup(){
	const char *text = "1";
	fprintf(fB, "%s", text);
	fflush(fB);
}

void ledRGB_exit(){

	const char *text = "0";
	fprintf(fB, "%s", text);
	fprintf(fR, "%s", text);
	fprintf(fG, "%s", text);
	fflush(fR);
	fflush(fG);
	fflush(fB);

}


void ledRGBACT(Message *addr){
	sigset_t LED_blockSet, LED_prevMask;
	sigemptyset(&LED_blockSet);
    sigaddset(&LED_blockSet, SIGHUP);
    if(sigprocmask(SIG_BLOCK, &LED_blockSet, &LED_prevMask) == -1)
		errExit("LED sigprocmask1");

	const char *text0 = "0";
	const char *text1 = "1";

	
	fprintf(fR, "%s", text1);
	fprintf(fG, "%s", text1);
	fflush(fR);
	fflush(fG);


	struct timespec toSleep;
	toSleep.tv_sec = 0;
    toSleep.tv_nsec = 200000000;
	nanosleep(&toSleep, NULL);

	fprintf(fR, "%s", text0);
	fprintf(fG, "%s", text0);
	fflush(fR);
	fflush(fG);

	long long t = getCurrentTimestamp() - addr->start_time;
  	long long t_sec = t/1000000;
	fprintf(f_rgb, "%lld.%lld\n", t_sec, t-t_sec*1000000);
	fflush(f_rgb);

	if(sigprocmask(SIG_SETMASK, &LED_prevMask, NULL) == -1)
        errExit("LED sigprocmask2");

  return;
}




