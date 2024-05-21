#include <xcore/channel.h>
#include "awe_xcore.h"

static chanend_t g_c_to_dspc;

void UserBufferManagementInit()
{
}

void UserBufferManagement(unsigned sampsFromUsbToAudio[], unsigned sampsFromAudioToUsb[])
{
    #if(I2S_ONLY)
        #pragma unroll loop(AUDIO_OUTPUT_CHANNELS)
        for(int i = 0; i < AUDIO_OUTPUT_CHANNELS; i++) {            // These are ADC
            chanend_out_word(g_c_to_dspc, sampsFromAudioToUsb[i]);
        }
        chanend_out_end_token(g_c_to_dspc);
        #pragma unroll loop(AUDIO_INPUT_CHANNELS)
        for(int i = 0; i < AUDIO_INPUT_CHANNELS; i++) {           // These go to DAC
            sampsFromUsbToAudio[i] = chanend_in_word(g_c_to_dspc);
        }
        chanend_check_end_token(g_c_to_dspc);
    #else
        awe_offload_data_to_dsp_engine(g_c_to_dspc, sampsFromUsbToAudio, sampsFromAudioToUsb);
    #endif // I2S_ONLY

}


void dsp_main(chanend_t c_control_from_host, chanend_t c_control_to_host) {
    channel_t c_data = chan_alloc();
    g_c_to_dspc = c_data.end_a;
    awe_xcore_main(c_control_from_host, c_control_to_host, c_data.end_b);
}
