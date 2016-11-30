#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stddef.h>
#include <fcntl.h>
#include <signal.h>

#ifndef LED_C
#include "led.c"
#endif
#ifndef STRUCTURE_LIB
#include "structure.h"
#endif

/* Global Variable */
// Pointer to shared memory region
Message *addr;   

// Beat Time Variables
double sec_interval;
double nano_interval;

// Nanosleep Sturct
struct timespec request, remain;
struct timeval last_beat_time, cur_time;

// Signal set
sigset_t blockSet, prevMask;

static void sigHandler(int sig){
	// set Beat time sleep
	if(sig == SIGUSR1){
		double spb = (double)60 / (double)addr->bpm;
		sec_interval = (int) spb;
		nano_interval = (spb - sec_interval) * 1000;
		if(gettimeofday(&cur_time, NULL) == -1)
    		errExit("cur gettimeofday");
    	remain.tv_sec = cur_time.tv_sec - addr->last_beat_time.tv_sec;
    	remain.tv_nsec = (cur_time.tv_usec - addr->last_beat_time.tv_usec) * 1000;
    }
	return;
}

int main(int argc, char *argv[]){               
    addr = mmap(NULL, sizeof(Message), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED)
        errExit("mmap");

    /* Initialize Beat Message in mapped region */
 	addr->last_ms = 0;                  
 	addr->last_s = 0;
 	addr->last_frame = 0;
 	addr->bpm = 0;

    /* Initialize lastBeatTime & msbp(millisecond per beat)*/
    last_beat_time.tv_sec = 0;
    last_beat_time.tv_usec = 0;
    sec_interval = 1;
    nano_interval = 0;

    /* Initialize Signal Set*/
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGUSR1);

    if (signal(SIGUSR1, sigHandler) == SIG_ERR)
    	errExit("SIGUSR1 Initialize");
    if (signal(SIGINT, sigHandler) == SIG_ERR)
    	errExit("SIGINT Initialize");

    switch (fork()) {           /* Parent and child share mapping */
    case -1:
        errExit("fork");

    case 0:                     /* Child: increment shared integer and exit */
        
        exit(1);

    default:                    /* Parent: wait for child to terminate */
        remain.tv_sec = sec_interval;
        remain.tv_nsec = nano_interval;
        for(;;){
        	// printf("here: %d %d\n", (int)sec_interval, (int) nano_interval);
        	while(1){
        		printf("remain: %2ld.%09ld\n", (long)remain.tv_sec,
        			remain.tv_nsec);
        		request = remain;
        		int s = nanosleep(&request, &remain);
        		if (s != -1)
        			break;
        		
        	}
        	if(sigprocmask(SIG_BLOCK, &blockSet, &prevMask) == -1)
        		errExit("sigprocmask1");
        	if(gettimeofday(&last_beat_time, NULL) == -1)
        		errExit("last_beat_time gettimeofday");
        	ledACT();
        	remain.tv_sec = sec_interval;
        	remain.tv_nsec = nano_interval;
        	if(sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        		errExit("sigprocmask2");
        }
        wait(NULL);
    }
    return 0;
}