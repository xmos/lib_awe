#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/parallel.h>
#include <xcore/port.h>
#include "awe_xcore_internal.h"

DECLARE_JOB(awe_dsp_thread, (uint32_t, chanend_t, chanend_t));
DECLARE_JOB(awe_tuning_thread, (chanend_t, chanend_t, chanend_t*));
DECLARE_JOB(awe_data_transport_thread, (chanend_t, chanend_t*));
#ifdef AWE_I2C_CONTROL
DECLARE_JOB(awe_i2c_main,              (port_t, port_t, int));
#endif

/** Function that spawns all the child threads
 * It always spawns at least one tuning thread, one data-transport
 * thread, and one DSP thread. Up to four additional DSP threads are spawned
 * for a maximum of 7 threads.
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
 */
void awe_xcore_main(chanend_t c_tuning_from_host,
                    chanend_t c_tuning_to_host,
                    chanend_t c_data
#ifdef AWE_I2C_CONTROL
                    , port_t p_i2c_scl
                    , port_t p_i2c_sda
                    , uint8_t i2c_address
#endif
    ) {
    channel_t t[AWE_DSP_THREAD_NUM];
    channel_t d[AWE_DSP_THREAD_NUM];
    chanend_t t_end_b[AWE_DSP_THREAD_NUM];
    chanend_t d_end_b[AWE_DSP_THREAD_NUM];
    uint32_t *AWEHeap = malloc(4*AWE_HEAP_SIZE);  // # Workaround bug 18804

    for(int i = 0; i < AWE_DSP_THREAD_NUM; i++) {
        t[i] = chan_alloc();
        t_end_b[i] = t[i].end_b;
        d[i] = chan_alloc();
        d_end_b[i] = d[i].end_b;
    }
    awe_xcore_init(AWEHeap);
    PAR_JOBS(
        PJOB(awe_dsp_thread, (0, t[0].end_a, d[0].end_a)),
#if AWE_DSP_THREAD_NUM > 1
        PJOB(awe_dsp_thread, (1, t[1].end_a, d[1].end_a)),
#if AWE_DSP_THREAD_NUM > 2
        PJOB(awe_dsp_thread, (2, t[2].end_a, d[2].end_a)),
#if AWE_DSP_THREAD_NUM > 3
        PJOB(awe_dsp_thread, (3, t[3].end_a, d[3].end_a)),
#if AWE_DSP_THREAD_NUM > 4
        PJOB(awe_dsp_thread, (4, t[4].end_a, d[4].end_a)),
#if AWE_DSP_THREAD_NUM > 5
#error "No more than 5 threads"
#endif
#endif
#endif
#endif
#endif
        PJOB(awe_tuning_thread, (c_tuning_from_host, c_tuning_to_host, d_end_b)),
        PJOB(awe_data_transport_thread, (c_data, t_end_b))
#ifdef AWE_I2C_CONTROL
        , PJOB(awe_i2c_main, (p_i2c_scl, p_i2c_sda, i2c_address))
#endif
        );
}
