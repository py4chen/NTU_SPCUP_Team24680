
/* Use the newer ALSA API */
#define DETECTOR_C
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <aubio.h>
#include <signal.h>

#include "structure.h"

// defined in config.h
extern unsigned int samplerate;
extern const unsigned int win_size;
extern const unsigned int hop_size;
extern const int duration;
extern unsigned int bitspersample;
extern const char *record_file;

int detector(int pid, Message *addr) {
long loops;
int rc;
int size;
snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
unsigned int n_frames = 0;
int dir;
snd_pcm_uframes_t frames;
unsigned char *buffer;
unsigned char *allBuffer;

/* Open PCM device for recording (capture). */
rc = snd_pcm_open(&handle, "plughw:1,0", SND_PCM_STREAM_CAPTURE, 0);
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
unsigned int wwww; //get_channels!
snd_pcm_hw_params_get_channels(params, &wwww);

/* 44100 bits/second sampling rate (CD quality) */
snd_pcm_hw_params_set_rate_near(handle, params, &samplerate, &dir);

/* Set period size to 32 frames. */
frames = hop_size;
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
buffer = (unsigned char *) malloc(size);


/* We want to loop for 5 seconds */
int bufferDuration = 1000000*size/samplerate/2;
loops = duration / bufferDuration;
allBuffer = (unsigned char *) malloc(size * loops);
if (allBuffer==NULL){
	fprintf(stderr, "Fail\n");
	exit(1);
}

// create some vectors
fvec_t * in = new_fvec (hop_size); // input audio buffer
fvec_t * out = new_fvec (1); // output position

// create tempo object
aubio_tempo_t * o = new_aubio_tempo("default", win_size, hop_size, samplerate);

int i = 0;
addr->start_time = getCurrentTimestamp();
//int skip = 0;
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
	memcpy(allBuffer + size * i, buffer, size);	
	i = i + 1;

	if (i % 1 == 0){
		unsigned char* char_ptr = allBuffer + size * (i - 1);
		//int length = size * 64;
		//int s;
		//smpl_t data[512];
		//for(s = 0;s < 512;s++){
		//	
		//}
		//usleep(80000);	
		uint_t wrap_at = (1 << ( bitspersample - 1 ) );
		uint_t wrap_with = (1 << bitspersample);
		smpl_t scaler = 1. / wrap_at;
		int signed_val = 0;
		unsigned int j, b, unsigned_val = 0;
		for (j = 0; j < hop_size; j++) {
			unsigned_val = 0;
			for (b = 0; b < bitspersample; b+=8 ) {
				unsigned_val += *(char_ptr) << b;
				char_ptr++;
			}
			signed_val = unsigned_val;
			// FIXME why does 8 bit conversion maps [0;255] to [-128;127]
			// instead of [0;127] to [0;127] and [128;255] to [-128;-1]
			if (bitspersample == 8) signed_val -= wrap_at;
			else if (unsigned_val >= wrap_at) signed_val = unsigned_val - wrap_with;
			in->data[j] = signed_val * scaler;
		}
    aubio_tempo_do(o,in,out);
		n_frames+= hop_size;
    if (out->data[0] != 0) {
//  if (skip % 2 == 0){
		addr->bpm = aubio_tempo_get_bpm(o);
	    	addr->last_ms = aubio_tempo_get_last_ms(o);
	    	addr->last_frame = aubio_tempo_get_last(o);
	    	kill(pid, SIGUSR1);
		fprintf(stderr, "Current time : %llu\n",getCurrentTimestamp()-addr->start_time);
	      	fprintf(stderr, "beat at %.3fms, %.3fs, frame %d, %.2fbpm with confidence %.9f\n",
		   aubio_tempo_get_last_ms(o), aubio_tempo_get_last_s(o),
		   aubio_tempo_get_last(o), aubio_tempo_get_bpm(o), aubio_tempo_get_confidence(o));
	        fprintf(f_aubio, "%.3f\n",aubio_tempo_get_last_s(o));
          fflush(f_aubio);
//	}	
//	skip++;
    }
}
	
}
int fd = open(record_file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
int fileSize = duration /bufferDuration*size;
write(fd, allBuffer, fileSize);
close(fd);
fprintf(stderr, "Written to file, size: %d\n", fileSize);

fprintf(stderr, "read %.2fs, %d frames at %dHz (%d blocks)\n",
		n_frames * 1. / samplerate,
		n_frames, samplerate,
		n_frames / hop_size);


//We don't care about the last part which less than 2 seconds, because there is no more beat point we need to predict.


snd_pcm_drain(handle);
snd_pcm_close(handle);
free(buffer);
free(allBuffer);

/* aubio free */
del_aubio_tempo(o);
del_fvec(in);
del_fvec(out);
aubio_cleanup();

fclose(f_aubio);

return 0;
}
