#include <stdio.h>
#include <stdlib.h>
short SOUND_EFFECT = 1; // 1: open, 0: close

void beat_sound() {
	if(SOUND_EFFECT==0){
		return;
	}
	int pid = fork();
	switch(pid){
		case 0:
			system("aplay -q ./sound_beat01.wav");
			exit(1);;
		default:
			return;
	}
}


void start_sound() {
	if(SOUND_EFFECT==0){
		return;
	}
	int pid = fork();
	switch(pid){
		case 0:
			system("aplay -q ./sound_start.wav");
			exit(1);;
		default:
			return;
	}
}

void end_sound() {
	if(SOUND_EFFECT==0){
		return;
	}
	int pid = fork();
	switch(pid){
		case 0:
			system("aplay -q ./sound_end.wav");
			exit(1);;
		default:
			return;
	}
}