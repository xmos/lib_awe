#ifndef AWE_XCORE_H
#define AWE_XCORE_H

#include <stdint.h>
#include <xcore/channel.h>

/** Thread that implements the AWE DSP functionality. This function does not
 * return and forks into multiple threads: one thread for control, one thread
 * data transport, and AWE_DSP_THREAD_NUM threads for DSP.
 *
 * You must create a awe_conf.h file to configure the AWE instantiation.
 * This defines the number of channels in the audio stack (AUDIO_INPUT_CHANNELS,
 * AUDIO_OUTPUT_CHANNELS), the number of channels that AWE expects
 * (AWE_INPUT_CHANNELS, AWE_OUTPUT_CHANNELS), and the number of threads that
 * should be used for DSP (AWE_DSP_THREAD_NUM).
 *
 * \param c_control   The channel end over which control packets arrive
 *
 * \param c_data      The channelend over which audio is communicated. Either
 *                    use ``awe_offload_data_to_dsp_engine`` or output
 *                    AUDIO_INPUT_CHANNELS ints over the channel followed by
 *                    CT_END, then input AUDIO_OUTPUT_CHANNELS ints over the 
 *                    channelend and then input a CT_END.
 */
extern void awe_xcore_main(chanend_t c_control, chanend_t c_data);

/** Convenience function that pushes an audio frame stored in an array to the
 * AWE stack. This function may be avoided and instead data can be pushed
 * directly onto the channelend.
 *
 * \param c_data      Channelend for audio communication
 *
 * \param toAWE       frame of data destined for AWE. This frame should be
 *                    AUDIO_OUTPUT_CHANNELS in size
 *
 * \param fromAWE     frame of data from AWE. This array should be
 *                    AUDIO_INPUT_CHANNELS in size and will be filled by the
 *                    function
 */
extern void awe_offload_data_to_dsp_engine(chanend_t c_to_awe,
                                           unsigned toAWE[],
                                           unsigned fromAWE[]);




#if defined(__awe_conf_h_exists__)
#include "awe_conf.h"
#endif

#ifndef AWE_DSP_THREAD_NUM
#define AWE_DSP_THREAD_NUM              2
#endif

#ifndef AWE_INPUT_CHANNELS
#error "Must define AWE_INPUT_CHANNELS"
#endif

#ifndef AWE_OUTPUT_CHANNELS
#error "Must define AWE_OUTPUT_CHANNELS"
#endif

#ifndef AUDIO_INPUT_CHANNELS
#error "Must define AUDIO_INPUT_CHANNELS"
#endif

#ifndef AUDIO_OUTPUT_CHANNELS
#error "Must define AUDIO_OUTPUT_CHANNELS"
#endif

#define AWE_HID_PACKET_BUFFER_SIZE 264

#define AWE_BLOCK_SIZE              32

#ifndef FAST_HEAP_A_SIZE
#define FAST_HEAP_A_SIZE          (5*1024)
#endif

#ifndef FAST_HEAP_B_SIZE
#define FAST_HEAP_B_SIZE          (5*1024)
#endif

#ifndef SLOW_HEAP_SIZE
#define SLOW_HEAP_SIZE            (5*1024)
#endif

#endif
