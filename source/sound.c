#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
short SOUND_EFFECT = 0; // 1: open, 0: close

void beat_sound() {
	//system("aplay -q ./sound_beat01.wav");
  //return;
	if(SOUND_EFFECT==0){
		return;
	}
	int pid = fork();
	switch(pid){
		case 0:
			system("aplay -q ./sound_beat01.wav");
			exit(1);
		default:
			return;
	}
}


void start_sound() {
        return;
	if(SOUND_EFFECT==0){
		return;
	}
	int pid = fork();
	switch(pid){
		case 0:
			system("aplay -q ./sound_start.wav");
			exit(1);
		default:
			return;
	}
}

void end_sound() {
        return;
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
