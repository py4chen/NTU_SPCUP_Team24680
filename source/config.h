
/* record */
unsigned int samplerate = 44100; //16000;
const unsigned int win_size = 2048;
const unsigned int hop_size = 256;
const int duration = 30000000;

/* aubio init */
unsigned int bitspersample = 16;

/* Sound Effect Trigger */
short SOUND_EFFECT = 0; // 1: open, 0: close

/* Discard Threshold */
double theta_nondiscard = 0.9;
double theta_discard = 0.5;
double theta_bpm_tolerant_small = 0.03;
double theta_bpm_tolerant_large = 0.80; // TODO: Do not change first 10 secs

/* File Path */
const char *record_file = "./Raw/record";
const char *led_log = "./log_led.txt";
const char *aubio_log = "./log_aubio.txt";
