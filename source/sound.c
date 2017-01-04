#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void beat_sound() {
	system("aplay -q ./sound_beat01.wav");

	long long t = getCurrentTimestamp() - addr->start_time;
  	long long t_sec = t/1000000;
	fprintf(f_sound, "%lld.%lld\n", t_sec, t-t_sec*1000000);
	fflush(f_sound);
	return;
}

void start_sound() {
	int pid = fork();
	switch(pid){
		case 0:
			system("aplay -q ./sound_beat01.wav");
			exit(1);
		default:
			return;
	}
}

void end_sound() {
	int pid = fork();
	switch(pid){
		case 0:
			system("aplay -q ./sound_end.wav");
			exit(1);;
		default:
			return;
	}
}
