//
//  alsa.c
//  fcdsdr
//
//  Created by Albin Stigö on 21/11/2017.
//  Copyright © 2017 Albin Stigo. All rights reserved.
//

#include "alsa.h"

/* Try to get an ALSA capture handle */
snd_pcm_t* sdr_pcm_handle(const char* pcm_name,
                          snd_pcm_uframes_t frames,
                          snd_pcm_stream_t stream) {

    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *hwparams;

    int dir = 0;
    int err = 0;
    unsigned int channels = 2;
    unsigned int periods = 4;
    unsigned int rate = 96000;

    snd_pcm_hw_params_alloca(&hwparams);
    
    // Open normal blocking
    if ((err = snd_pcm_open(&pcm_handle, pcm_name, stream, 0)) < 0) {
        fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(err));
        return NULL;
    }
    
    // Init hwparams with full configuration space
    if ((err = snd_pcm_hw_params_any(pcm_handle, hwparams)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_any: %s\n", snd_strerror(err));
        return NULL;
    }
    
    // Interleaved access. (IQ interleaved).
    if ((err = snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_access: %s\n", snd_strerror(err));
        return NULL;
    }

    // Set number of channels
    if ((err = snd_pcm_hw_params_set_channels_near(pcm_handle, hwparams, &channels)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_channels_near: %s\n", snd_strerror(err));
        return NULL;
    }
    
    // ALSA will convert from S32 to float for us.
    if ((err = snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_FLOAT)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_format: %s\n", snd_strerror(err));
        return NULL;
    }
    
    // Set sample rate. If the exact rate is not supported exit
    if ((err = snd_pcm_hw_params_set_rate(pcm_handle, hwparams, rate, 0)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_rate: %s\n", snd_strerror(err));
        return NULL;
    }
    
    // Period size
    dir = 1;
    if ((err = snd_pcm_hw_params_set_period_size_near(pcm_handle, hwparams, &frames, &dir)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_period_size: %s\n", snd_strerror(err));
        return NULL;
    }
    
    // Set number of periods. Periods used to be called fragments.
    dir = 1;
    if ((err = snd_pcm_hw_params_set_periods_near(pcm_handle, hwparams, &periods, &dir)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params_set_periods: %s\n", snd_strerror(err));
        return NULL;
    }
    
    // Apply HW params
    if ((err = snd_pcm_hw_params(pcm_handle, hwparams)) < 0) {
        fprintf(stderr, "snd_pcm_hw_params: %s\n", snd_strerror(err));
        return NULL;
    }
    
    snd_pcm_uframes_t bufs = 0;
    snd_pcm_hw_params_get_buffer_size(hwparams, &bufs);
    
    // printf("Got buffer size %u\n", (unsigned int) bufs);

    snd_pcm_sw_params_t *swparams;
    snd_pcm_sw_params_alloca(&swparams);
    // Get the current swparams
    if ((err = snd_pcm_sw_params_current(pcm_handle, swparams)) < 0) {
        printf("snd_pcm_sw_params_current: %s\n", snd_strerror(err));
        return NULL;
    }

    /*if ((err = snd_pcm_sw_params_set_avail_min(pcm_handle, swparams, frames)) < 0) {
        printf("snd_pcm_sw_params_set_avail_min: %s\n", snd_strerror(err));
        return NULL;
    }*/
    
    // Enable period events when requested
    /*if ((err = snd_pcm_sw_params_set_period_event(pcm_handle, swparams, 1)) < 0) {
        printf("snd_pcm_sw_params_set_period_event: %s\n", snd_strerror(err));
        return NULL;
    }*/
    
    // Write the parameters to the playback device
    if ((err = snd_pcm_sw_params(pcm_handle, swparams)) < 0) {
        printf("snd_pcm_sw_params: %s\n", snd_strerror(err));
        return NULL;
    }
    
    return pcm_handle;
}
