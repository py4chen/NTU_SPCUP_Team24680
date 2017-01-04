#include <aubio.h>
#include <stdlib.h>
#include <stdio.h>
#include "structure.h"

float bpm=0;
/* Discard Threshold */
double theta_nondiscard = 0.8;
double theta_discard = 0.3;
double theta_bpm_tolerant_small = 0.03;
double theta_bpm_tolerant_large = 0.80; // TODO: Do not change first 10 secs
// Conf
double best_conf = 0;
double threshhold_conf = 0.05;
double conf_decrease = 0.9;
unsigned long long start_time = 0, old_spb=0, old_next_time=0, aubio_delay = 500000;
void sigHandler(double new_bpm, double new_conf, double last_ms, unsigned long long cur_time){
    best_conf = best_conf * conf_decrease;
    if(best_conf > threshhold_conf && new_conf < best_conf){
        fprintf(stderr, "Discard update. Because conf %.2f < best_conf %.2f \n", new_conf, best_conf);
        return;
    }
    best_conf = new_conf;
    
    //calculate spb
		unsigned long long spb = (unsigned long long)(((double)60 / new_bpm)*1000000);
		bpm = new_bpm;
		// calculate next time
		unsigned long long next_time = start_time + last_ms * 1000;

    if(old_spb){
            // simulate
            while(old_next_time < cur_time){
              fprintf(f_led, "%f\n", (double)old_next_time/1000000);
              old_next_time += old_spb;
            }
            unsigned long long last_beat_time = old_next_time - old_spb;

              // check if discard old_next_beat_time
              if((next_time > old_next_time) && ((next_time - old_next_time) > (old_next_time - last_beat_time) * theta_nondiscard)){
                  fprintf(stderr, "Non discard old_next_time at %lld\n", cur_time);
                  fprintf(f_led, "%f\n", (double)old_next_time/1000000);
              }
              else if ((next_time < old_next_time) && (next_time - last_beat_time) < spb * theta_discard){
                  fprintf(stderr, "Discard next_predicted_time at %lld\n", cur_time);

              }
    }
    old_next_time = next_time + spb;
    old_spb = spb;
	return;
}
int main (int argc, char **argv)
{
  uint_t err = 0;
  if (argc < 2) {
    err = 2;
    fprintf(stderr, "not enough arguments\n");
    fprintf(stderr, "read a wave file as a mono vector\n");
    fprintf(stderr, "usage: %s <source_path> [samplerate] [win_size] [hop_size]\n", argv[0]);
    return err;
  }
  uint_t samplerate = 0;
  if ( argc >= 3 ) samplerate = atoi(argv[2]);
  uint_t win_size = 1024; // window size
  if ( argc >= 4 ) win_size = atoi(argv[3]);
  uint_t hop_size = win_size / 4;
  if ( argc >= 5 ) hop_size = atoi(argv[4]);
  uint_t n_frames = 0, read = 0;

  char_t *source_path = argv[1];
  aubio_source_t * source = new_aubio_source(source_path, samplerate, hop_size);
  if (!source) { err = 1; goto beach; }

  if (samplerate == 0 ) samplerate = aubio_source_get_samplerate(source);

  // create some vectors
  fvec_t * in = new_fvec (hop_size); // input audio buffer
  fvec_t * out = new_fvec (1); // output position

  // create tempo object
  aubio_tempo_t * o = new_aubio_tempo("default", win_size, hop_size, samplerate);
  f_aubio= fopen("./log_aubio.txt", "w");
  f_led= fopen("./log_led.txt", "w");

  do {
    // put some fresh data in input vector
    aubio_source_do(source, in, &read);
    // execute tempo
    aubio_tempo_do(o,in,out);
    // do something with the beats
    if (out->data[0] != 0) {
      fprintf(stderr, "beat at %.3fms, %.3fs, frame %d, %.2fbpm with confidence %.2f\n",
          aubio_tempo_get_last_ms(o), aubio_tempo_get_last_s(o),
          aubio_tempo_get_last(o), aubio_tempo_get_bpm(o), aubio_tempo_get_confidence(o));
      fprintf(f_aubio, "%.3f\n",aubio_tempo_get_last_s(o));
      sigHandler( aubio_tempo_get_bpm(o), aubio_tempo_get_confidence(o), aubio_tempo_get_last_ms(o), (unsigned long long)((aubio_tempo_get_last_ms(o)*1000)+aubio_delay));
    }
    n_frames += read;
  } while ( read == hop_size );

  fprintf(stderr, "read %.2fs, %d frames at %dHz (%d blocks) from %s\n",
      n_frames * 1. / samplerate,
      n_frames, samplerate,
      n_frames / hop_size, source_path);
  while(old_next_time <= 30){
    fprintf(f_led, "%f\n", (double)old_next_time/1000000);
    old_next_time += old_spb;
  }

  // clean up memory
  del_aubio_tempo(o);
  del_fvec(in);
  del_fvec(out);
  del_aubio_source(source);
  fclose(f_aubio);
beach:
  aubio_cleanup();

  return err;
}
