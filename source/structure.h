#define STRUCTURE_LIB
#include <sys/time.h>
#include <time.h>

void errExit(char *str){
	printf("%s ERROR.", str);
	exit(-1);
}

typedef struct message{
	float last_ms;
	float last_s;
	int last_frame;
	int bpm;
	struct timeval last_beat_time;
}Message;

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
