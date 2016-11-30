#define STRUCTURE_LIB
#include <sys/time.h>
#include <time.h>

void errExit(char *str){
	printf("%s ERROR.", str);
	exit(-1);
}

typedef struct message{
	double last_ms;
	int last_frame;
	double bpm;
	unsigned long long start_time;

}Message;

struct timeval tv;
unsigned long long getCurrentTimestamp(){
	if(gettimeofday(&tv, NULL) == -1)
       errExit("last_beat_time gettimeofday");
    return 1000000 * tv.tv_sec + tv.tv_usec;
}

/*
struct timespec{
	time_t tv_sec; //seconds
	long tv_nsec; // nanoseconds
}

timeval
{
time_t tv_sec; //second [long int]
suseconds_t tv_usec; //microsecond [long int]

*/
