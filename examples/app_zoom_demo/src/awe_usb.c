// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include "awe_xcore.h"

static chanend_t g_c_to_dspc;

void UserBufferManagementInit(unsigned sampFreq)
{
}

void UserBufferManagement(unsigned sampsFromUsbToAudio[], unsigned sampsFromAudioToUsb[])
{
    /* Intercept samples from ADC and process them,
     * Send processed samples to DAC */
        awe_offload_data_to_dsp_engine(g_c_to_dspc, sampsFromAudioToUsb, sampsFromUsbToAudio);
}

void dsp_main(chanend_t c_control_from_host, chanend_t c_control_to_host)
{
    channel_t c_data = chan_alloc();
    g_c_to_dspc = c_data.end_a;
    awe_xcore_main(c_control_from_host, c_control_to_host, c_data.end_b);
}
