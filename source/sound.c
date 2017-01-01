#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void beat_sound() {
	system("aplay -q ./sound_beat01.wav");
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
