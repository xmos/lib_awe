#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/parallel.h>
#include "awe_xcore_internal.h"

DECLARE_JOB(awe_dsp_thread, (uint32_t, chanend_t));
DECLARE_JOB(awe2_tuning_thread, (chanend_t));
DECLARE_JOB(awe_data_transport_thread, (chanend_t, chanend_t*));

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
extern void awe2_tuning_thread(chanend_t c_control);

void awe_xcore_main(chanend_t c_tuning, chanend_t c_data) {
    channel_t t[AWE_DSP_THREAD_NUM];
    chanend_t t_end_b[AWE_DSP_THREAD_NUM];
    for(int i = 0; i < AWE_DSP_THREAD_NUM; i++) {
        t[i] = chan_alloc();
        t_end_b[i] = t[i].end_b;
    }
    awe_xcore_init();
    PAR_JOBS(
        PJOB(awe_dsp_thread, (0, t[0].end_a)),
#if AWE_DSP_THREAD_NUM > 1
        PJOB(awe_dsp_thread, (1, t[1].end_a)),
#if AWE_DSP_THREAD_NUM > 2
        PJOB(awe_dsp_thread, (2, t[2].end_a)),
#if AWE_DSP_THREAD_NUM > 3
        PJOB(awe_dsp_thread, (3, t[3].end_a)),
#if AWE_DSP_THREAD_NUM > 4
        PJOB(awe_dsp_thread, (4, t[4].end_a)),
#if AWE_DSP_THREAD_NUM > 5
#error "No more than 5 threads"
#endif
#endif
#endif
#endif
#endif
        PJOB(awe2_tuning_thread, (c_tuning)),
        PJOB(awe_data_transport_thread, (c_data, t_end_b)));
}
