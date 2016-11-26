/*
This example reads from the default PCM device
and writes to standard output for 5 seconds of data.
*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>


int main() {
long loops;
int rc;
int size;
snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
unsigned int val;
int dir;
snd_pcm_uframes_t frames;
char *buffer;
char *allBuffer;
int duration = 10000000;

/* Open PCM device for recording (capture). */
rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
if (rc < 0) {
	fprintf(stderr,
			"unable to open pcm device: %s\n",
			snd_strerror(rc));
	exit(1);
}

/* Allocate a hardware parameters object. */
snd_pcm_hw_params_alloca(&params);

/* Fill it in with default values. */
snd_pcm_hw_params_any(handle, params);

/* Set the desired hardware parameters. */

/* Interleaved mode */
snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

/* Signed 16-bit little-endian format */
snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

/* Two channels (stereo) */
snd_pcm_hw_params_set_channels(handle, params, 1);

/* 44100 bits/second sampling rate (CD quality) */
val = 44100;
snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

/* Set period size to 32 frames. */
frames = 32;
snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

/* Write the parameters to the driver */
rc = snd_pcm_hw_params(handle, params);
if (rc < 0) {
	fprintf(stderr,
			"unable to set hw parameters: %s\n",
			snd_strerror(rc));
	exit(1);
}

/* Use a buffer large enough to hold one period */
snd_pcm_hw_params_get_period_size(params, &frames, &dir);
size = frames * 2; /* 2 bytes/sample, 2 channels */
buffer = (char *) malloc(size);


/* We want to loop for 5 seconds */
snd_pcm_hw_params_get_period_time(params, &val, &dir);
loops = duration / val;
allBuffer = (char *) malloc(size * loops);
if (allBuffer==NULL){
	fprintf(stderr, "Fail\n");
	exit(1);
}


char filename[10];
int fileNo = 1;
int i = 0;
int sec = duration / 1000000;
int LPerS = loops / sec;
int LPer2S = LPerS * 2;
while (loops > 0) {
	loops--;
	rc = snd_pcm_readi(handle, buffer, frames);
	if (rc == -EPIPE) {
	  /* EPIPE means overrun */
	  fprintf(stderr, "overrun occurred\n");
	  snd_pcm_prepare(handle);
	} else if (rc < 0) {
	  fprintf(stderr,
			  "error from read: %s\n",
			  snd_strerror(rc));
	} else if (rc != (int)frames) {
	  fprintf(stderr, "short read, read %d frames\n", rc);
	}
	strcpy(allBuffer + size * i, buffer);	
	i = i + 1;

	if (i == LPer2S*fileNo){
		
		sprintf(filename, "./Raw/record_%03d", fileNo);
		int fd = open(filename, O_WRONLY | O_CREAT, 0777);
		if(i < LPerS*5){
			write(fd, allBuffer, size * i);
		}else{
			write(fd, allBuffer + size * (i - LPerS*5), LPerS*5*size);
		}
		close(fd);
		fileNo = fileNo + 1;		
	}
	
}


//We don't care about the last part which less than 2 seconds, because there is no more beat point we need to predict.


snd_pcm_drain(handle);
snd_pcm_close(handle);
free(buffer);
free(allBuffer);

return 0;
}
