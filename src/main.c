#include <stdio.h> 
#include <limits.h> 
#include <math.h> 
#include "alsa_lowlevel.h"
#include "audio_hw.h"
#include "alsa_midi_lowlevel.h"
#include "midi_hw.h" 

#define NCHANS 2 
#define SAMPLE_RATE 44100 

double phase[NCHANS];
double phasev[NCHANS];

void audio_hw_io(audio_hw_io_t *params)
{
    int n, c;
    for (n = 0; n < params->length; n++) {
        for (c = 0; c < params->nchans_out; c++) {
            params->out[n*params->nchans_out + c] = 
                sin(phase[c]*2.*M_PI) * AUDIO_HW_SAMPLE_T_MAX;
            phase[c] += phasev[c];
            if (phase[c] >= 1.0) {
                phase[c] -= 1.0;
            }
        }
    }
}

void midi_hw_process_msg(MIDIMsg *msg)
{
    if (msg->data[0] == 0x91) {
        /* NOTE ON BABY */
        double note = msg->data[1];
        phasev[0] = pow(2.,(note - 69)/12.)*440.
            / (double)SAMPLE_RATE;
        phasev[1] = phasev[0]*3./2.;
    }
    printf("%02x\n",msg->data[0]);
}

int main (int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr,
                "Arguments are %s audio-device midi-input-port\n",argv[0]);
        return(-1);
    }
    audio_hw_setup_t ahs;
    ahs.device = argv[1];           /* playback device */
    ahs.format = SND_PCM_FORMAT_S16;/* sample format */
    ahs.rate   = SAMPLE_RATE;			    /* stream rate */
    ahs.channels = NCHANS;		    /* count of channels */
    ahs.buffer_time = 50000;	    /* ring buffer length in us */
    ahs.period_time = 10000;	    /* period time in us */
    ahs.verbose = 1;				/* verbose flag */
    ahs.resample = 1;				/* enable alsa-lib resampling */
    ahs.period_event = 0;	        /* produce poll event after each period */
    ahs.method = "async";           /* the transfer method.
                                       Should be 'async", "async_direct" */
    phase[0] = 0.;
    phase[1] = 0.;
    phasev[0] = 400./(double)SAMPLE_RATE;
    phasev[1] = 500./(double)SAMPLE_RATE;

    midi_hw_setup_t mhs;
    mhs.device_in = argv[2];
    mhs.verbose = 0;

    if (midi_hw_setup(&mhs)) {
        fprintf(stderr,"Error setting up MIDI.\n");
        return(-1);
    }

    if (audio_hw_setup(&ahs)){ 
        fprintf(stderr,"Error setting up audio.\n");
        return(-1);
    }

    audio_hw_start(NULL);

    while (1) {
        midi_hw_process_input(NULL);
    }
    return(0);
}


