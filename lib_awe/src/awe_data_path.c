// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include <xcore/select.h>
#include <stdio.h>
#include "assert.h"
#include "awe_xcore_internal.h"

void awe_offload_data_to_dsp_engine(chanend_t c_to_dspc, unsigned sampsToAWE[], unsigned sampsFromAWE[])
{
    for(int i = 0; i < AWE_INPUT_CHANNELS; i++) {
        chanend_out_word(c_to_dspc, sampsToAWE[i]);
    }

    chanend_out_end_token(c_to_dspc);
    for(int i = 0; i < AWE_INPUT_CHANNELS; i++) {
        sampsFromAWE[i] = chanend_in_word(c_to_dspc);
    }

    chanend_check_end_token(c_to_dspc);
}

/** Function that transfers data to and from the AWE subsystem. This
 * provides one fundamental block worth of audio frames, and reads one
 * fundamental block.
 *
 * \param  data_from_awe 32-bit sample values that have been processed
 * \param  data_to_awe   32-bit sample values ready for audio processing
 */
static void data_to_and_from_awe(
    int32_t data_from_awe[AWE_BLOCK_SIZE][AWE_OUTPUT_CHANNELS],
    int32_t data_to_awe  [AWE_BLOCK_SIZE][AWE_INPUT_CHANNELS],
    chanend_t c_dsp_threads[]) {
    int passthrough = 0;

    for(int i = 0; i < AWE_INPUT_CHANNELS; i++) {
        // Call this for each input channel in your system.
        int res = awe_audioImportSamples((AWEInstance *)&g_AWEInstance,
                                         &data_to_awe[0][i],
                                         AWE_INPUT_CHANNELS, i,
                                         Sample24bit_high);
        if (res == E_AUDIO_NOT_STARTED) {
            passthrough = 1;
            break;
        }
        assert(res == E_SUCCESS);
    }
    for(int i = 0; i < AWE_OUTPUT_CHANNELS; i++) {
        // Call this for each output channel in your system.
        int res = awe_audioExportSamples((AWEInstance *)&g_AWEInstance, &data_from_awe[0][i],
                                         AWE_OUTPUT_CHANNELS, i,
                                         Sample24bit_high);
        if (res == E_AUDIO_NOT_STARTED) {
            passthrough = 1;
            break;
        }
        assert(res == E_SUCCESS);
    }
    if (passthrough) {
        int channels_to_copy = (AWE_OUTPUT_CHANNELS < AWE_INPUT_CHANNELS ?
                                AWE_OUTPUT_CHANNELS :
                                AWE_INPUT_CHANNELS);
        memset(data_from_awe, 0, channels_to_copy * AWE_BLOCK_SIZE * sizeof(int32_t));
        return;
    }
    unsigned layoutMask = awe_audioGetPumpMask((AWEInstance *)&g_AWEInstance);
    for( int i = 0; i < AWE_DSP_THREAD_NUM; i++) {
        if (layoutMask & 1) {
            chanend_out_end_token(c_dsp_threads[i]);
        }
        layoutMask >>= 1;
    }
}

/** Function that blocks data up for the AWE stack, and when ready starts the AWE
 * threads.
 */
void awe_data_transport_thread(chanend_t c_data, chanend_t c_children[]) {
    int32_t input_data[AWE_BLOCK_SIZE][AWE_INPUT_CHANNELS];
    int32_t output_data[AWE_BLOCK_SIZE][AWE_OUTPUT_CHANNELS];
    int frame = 0;
    while(1) {

        /* Get samples to process */
        #pragma unroll
        for(int i = 0; i < AWE_INPUT_CHANNELS; i++) {
            input_data[frame][i] = chanend_in_word(c_data);
        }
        chanend_check_end_token(c_data);

        /* Send out processed samples */
        #pragma unroll
        for(int i = 0; i < AWE_OUTPUT_CHANNELS; i++) {
            chanend_out_word(c_data, output_data[frame][i]);
        }

        for(int i = 0; i < AWE_OUTPUT_CHANNELS; i++) {
            output_data[frame][i] = 0;
        }
        chanend_out_end_token(c_data);
        frame++;
        if (frame == AWE_BLOCK_SIZE) { // TODO: this may not fit between samples
            frame = 0;
            /* Process a frame of samples */
            data_to_and_from_awe(output_data, input_data, c_children);
        }
    }
}

#pragma stackfunction 1024
void awe_dsp_thread(uint32_t thread_number,
                    chanend_t c_parent,
                    chanend_t c_deferred_work) {
    int no_deferred_work = 1;
    SELECT_RES(
        CASE_THEN(c_parent,        go),
        CASE_THEN(c_deferred_work, clear_deferred_work)
        )
    {
    go:
        chanend_check_end_token(c_parent);
        int res = awe_audioPump((AWEInstance *)&g_AWEInstance, thread_number);
        if (res && no_deferred_work) {
            chanend_out_end_token(c_deferred_work);
            no_deferred_work = 0;
        }
        continue;      // TODO: CONTINUE_NO_RESET
    clear_deferred_work:
        chanend_check_end_token(c_deferred_work);
        no_deferred_work = 1;
        continue;      // TODO: CONTINUE_NO_RESET
    }
}
