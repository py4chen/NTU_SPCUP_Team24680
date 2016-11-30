#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stddef.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#ifndef LED_C
#include "led.c"
#endif
#ifndef STRUCTURE_LIB
#include "structure.h"
#endif
#ifndef DETECTOR_C
#include "detector.c"
#endif

/* Global Variable */
// Pointer to shared memory region
Message *addr;   

// Beat Time Variables
long long sec_interval;
long long nano_interval;

// Nanosleep Sturct
struct timespec request, remain;
unsigned long long last_beat_time, cur_time, next_time;

// Signal set
sigset_t blockSet, prevMask;

static void sigHandler(int sig){
	// set Beat time sleep
	if(sig == SIGUSR1){
		// calculate seconds per bit
		double spb = (double)60 / addr->bpm;
		sec_interval = (int) spb;
		nano_interval = (spb - sec_interval) * 1000000000;
		printf("new sec_interval:%lld, nano_interval:%lld\n", sec_interval,
			nano_interval);

		// calculate remain time
		cur_time = getCurrentTimestamp();
		next_time = addr->start_time + addr->last_ms * 1000;
		while(next_time < cur_time){
			next_time += spb*1000000;
		}
		remain.tv_sec = (next_time-cur_time)/1000000;
		remain.tv_nsec = (next_time-cur_time - remain.tv_sec) * 1000;
    }
	return;
}

int main(int argc, char *argv[]){               
    addr = mmap(NULL, sizeof(Message), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED)
        errExit("mmap");

    /* Initialize Beat Message in mapped region */
 	memset (addr, 0, sizeof(Message));

    /* Initialize lastBeatTime & msbp(millisecond per beat)*/
    last_beat_time = 0;
    cur_time = 0;
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
        detector(getpid(), addr);
        exit(1);

    default:                    /* Parent: wait for child to terminate */
        remain.tv_sec = sec_interval;
        remain.tv_nsec = nano_interval;
        for(;;){
        	// printf("here: %d %d\n", (int)sec_interval, (int) nano_interval);
        	while(1){
        		// printf("remain: %2ld.%09ld\n", (long)remain.tv_sec,
        		// 	remain.tv_nsec);
        		request = remain;
        		int s = nanosleep(&request, &remain);
        		if (s != -1)
        			break;
        		
        	}
        	if(sigprocmask(SIG_BLOCK, &blockSet, &prevMask) == -1)
        		errExit("sigprocmask1");
        	if(gettimeofday(&tv, NULL) == -1)
        		errExit("last_beat_time gettimeofday");
        	last_beat_time = getCurrentTimestamp();
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
