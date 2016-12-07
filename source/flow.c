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

/* Discard Threshold */
double theta_discard = 0.9;  
double theta_bpm_tolerant_small = 0.03;
double theta_bpm_tolerant_large = 0.40;
/* Global Variable */
// Pointer to shared memory region
Message *addr;   
int child_pid;

// Beat Time Variables
float bpm;
long long sec_interval;
long long nano_interval;

// Nanosleep Sturct
struct timespec request, remain, nuremain;
unsigned long long last_beat_time, cur_time, next_time, old_next_time;

// flag
int handler_flag = 0;
int nondiscard_flag = 0;

// Signal set
sigset_t blockSet, prevMask;

unsigned long long getCurrentTimestamp(){
	if(gettimeofday(&tv, NULL) == -1)
       errExit("last_beat_time gettimeofday");
    return 1000000 * tv.tv_sec + tv.tv_usec;
}
void errExit(char *str){
	printf("%s ERROR.", str);
	exit(-1);
}

void sigHandler(int sig){
	// set Beat time sleep
	if(sig == SIGUSR1){
		handler_flag = 1;
		// calculate seconds per bit
        if( (addr->bpm < bpm * (1+theta_bpm_tolerant_small) &&
           addr->bpm > bpm * (1-theta_bpm_tolerant_small) ) ||
            (addr->bpm > bpm *(1+theta_bpm_tolerant_large) &&
            (addr->bpm < bpm *(1-theta_bpm_tolerant_large)) ) ){
                printf("Discard bpm update at %lld\n ", getCurrentTimestamp() - addr->start_time);
            return;
        }

		double spb = (double)60 / addr->bpm;
		bpm = addr->bpm;
        sec_interval = (int) spb;
		nano_interval = (spb - sec_interval) * 1000000000;
		//printf("new sec_interval:%lld, nano_interval:%lld\n", sec_interval,
		//	nano_interval);

		// calculate next time
		cur_time = getCurrentTimestamp();
		next_time = addr->start_time + addr->last_ms * 1000;
		while(next_time < cur_time){
			next_time += spb*1000000;
		}

        // check if discard old_next_beat_time
        old_next_time = cur_time + remain.tv_sec*1000000 + remain.tv_nsec/1000;
        if((next_time > old_next_time) && ((next_time - old_next_time) > (old_next_time - last_beat_time) * theta_discard)){
            nondiscard_flag = 1;
            nuremain.tv_sec = (next_time-old_next_time)/1000000;
            nuremain.tv_nsec = (next_time-old_next_time - nuremain.tv_sec*1000000) * 1000;
            printf("Non discard old_next_time at %lld\n ", getCurrentTimestamp() - addr->start_time);
        }
        else{
		  remain.tv_sec = (next_time-cur_time)/1000000;
		  remain.tv_nsec = (next_time-cur_time - remain.tv_sec*1000000) * 1000;
		  // printf("updated remain: %2ld.%09ld, current time is: %llu, updated interval:%lld.%lld\n", (long)remain.tv_sec,
    //     			remain.tv_nsec, cur_time-addr->start_time, sec_interval, nano_interval);
        }
    }
	return;
}
static void exit_handler(void)
{
    kill(child_pid, SIGINT);
    printf("EXIT HANDLER\n");
}

int main(int argc, char *argv[]){               
    f_led= fopen("./log_led.txt", "w");
    f_aubio= fopen("./log_aubio.txt", "w");
    addr = mmap(NULL, sizeof(Message), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED)
        errExit("mmap");

    /* Initialize Beat Message in mapped region */
 	memset (addr, 0, sizeof(Message));

    /* Initialize lastBeatTime & msbp(millisecond per beat)*/
    last_beat_time = 0;
    cur_time = 0;
    sec_interval = 100;
    nano_interval = 0;

    /* Initialize Signal Set*/
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGUSR1);

    if (signal(SIGUSR1, sigHandler) == SIG_ERR)
    	errExit("SIGUSR1 Initialize");
    // if (signal(SIGSTP, sigHandler) == SIG_ERR)
    // 	errExit("SIGSTP Initialize");

    child_pid = fork();
    switch (child_pid) {           /* Parent and child share mapping */
    case -1:
        errExit("fork");

    case 0:                     /* Child: increment shared integer and exit */
        detector(getppid(), addr);
        exit(1);

    default:                    /* Parent: wait for child to terminate */
        atexit(exit_handler);
        remain.tv_sec = sec_interval;
        remain.tv_nsec = nano_interval;
        for(;;){
        	// printf("here: %d %d\n", (int)sec_interval, (int) nano_interval);
        	while(1){
		        //printf("sleep remain: %2ld.%09ld\n", (long)remain.tv_sec,
		        //    remain.tv_nsec);
				request = remain;
				// printf("request==remain:%d\n", &remain==&request);
		        //printf("sleep request: %2ld.%09ld\n", (long)request.tv_sec,
		        //    request.tv_nsec);
			    int s = nanosleep(&request, &remain);
		        if(handler_flag == 1){
				    // printf("wakeup remain: %2ld.%09ld\n", (long)remain.tv_sec,
		      //   	    	remain.tv_nsec);
		      //       printf("wakeup request: %2ld.%09ld\n", (long)request.tv_sec,
		      //       		request.tv_nsec);
				    handler_flag = 0;	
			    }
				if (s != -1){
		            // fprintf(stderr, "good sleep\n");
					break;
		        }
		        //fprintf(stdout, "bad sleep\n");
        	}
        	if(sigprocmask(SIG_BLOCK, &blockSet, &prevMask) == -1)
        		errExit("sigprocmask1");
        	if(gettimeofday(&tv, NULL) == -1)
        		errExit("last_beat_time gettimeofday");
        	last_beat_time = getCurrentTimestamp();
            ledACT(addr);
            if(nondiscard_flag == 1){
                nondiscard_flag = 0;
                remain.tv_sec = nuremain.tv_sec;
                remain.tv_nsec = nuremain.tv_nsec;
            } 
            else{
        	   remain.tv_sec = sec_interval;
        	   remain.tv_nsec = nano_interval;
            }
        	if(sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        		errExit("sigprocmask2");
        }
        wait(NULL);
    }
    return 0;
}
