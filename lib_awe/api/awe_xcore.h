#ifndef AWE_XCORE_H
#define AWE_XCORE_H

#include <stdint.h>
#include <xcore/channel.h>
#include <xcore/port.h>


#if defined(__awe_conf_h_exists__)
#include "awe_conf.h"
#endif

/** Function that spawns all the child threads. This function does not return.
 * It always spawns at least one tuning thread, one data-transport
 * thread, and one DSP thread. Up to four additional DSP threads are spawned
 * for a maximum of seven threads.
 *
 * The data-transport thread has a channel to each DSP thread in order
 * to start them when data is available. A single channel-end is used to send
 * audio-samples to the data-transport thread and receive samples back.
 *
 * The tuning thread occupies another channel-end, it is used for sending
 * tuning packets to the AWE library. Part packets are sent over this channel
 * and assembled in the tuning thread; when a complete packet has arrived
 * the tuning packet is handled by AWE and a reply is sent back over the same
 * channel-end.
 *
 * You must create a awe_conf.h file to configure the AWE instantiation.
 * This defines the number of channels in the audio stack (AUDIO_INPUT_CHANNELS,
 * AUDIO_OUTPUT_CHANNELS), the number of channels that AWE expects
 * (AWE_INPUT_CHANNELS, AWE_OUTPUT_CHANNELS), and the number of threads that
 * should be used for DSP (AWE_DSP_THREAD_NUM).
 *
 * \param c_tuning_from_host
 *                    The channel end over which tuning packets arrive. Over
 *                    this channel you will send a sequence [N, N words, CT_END],
 *                    and then receive an [CT_END] empty message as ack. You do
 *                    this until all data has been sent through.
 * \param c_tuning_to_host
 *                    A [CT_END] empty message will be sent back over c_tuning,
 *                    which you must acknowledge with a word M for the maximum
 *                    packet size you are willing to accept, whereupon you will
 *                    be sent [M words, CT_END] that you must acknowledge with a
 *                    [CT_END]. This is repeated until the reply has been sent
 *                    back.
 *
 * \param c_data      The channelend over which audio is communicated. Either
 *                    use ``awe_offload_data_to_dsp_engine`` or output
 *                    [ AUDIO_INPUT_CHANNELS words, CT_END] over c_data, then
 *                    input [ AUDIO_OUTPUT_CHANNELS, CT_END ] from c_data.
 */
extern void awe_xcore_main(chanend_t c_control_from_host,
                           chanend_t c_tuning_to_host,
                           chanend_t c_data
#ifdef AWE_I2C_CONTROL
                           , port_t p_i2c_scl
                           , port_t p_i2c_sda
                           , uint8_t i2c_address
#endif
    );

/** Convenience function that pushes an audio frame stored in an array to the
 * AWE stack. This function may be avoided and instead data can be pushed
 * directly onto the channelend.
 *
 * NOTE: THIS DESCRIPTION IS WRONG AT PRESENT
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

#define AWE_DSP_MAX_THREAD_NUM        5


#ifndef AWE_DSP_THREAD_NUM
#define AWE_DSP_THREAD_NUM              2
#endif

#if AWE_DSP_THREAD_NUM > AWE_DSP_MAX_THREAD_NUM
#error "AWE_DSP_THREAD_NUM too high, max " ## AWE_DSP_MAX_THREAD_NUM
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
