#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/parallel.h>
#include "awe_xcore_internal.h"

DECLARE_JOB(awe_dsp_thread, (uint32_t, chanend_t));
DECLARE_JOB(awe_hid_control, (chanend_t));
DECLARE_JOB(awe_data_transport_thread, (chanend_t, chanend_t*));

void awe_xcore_main(chanend_t c_control, chanend_t c_data) {
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
        PJOB(awe_hid_control, (c_control)),
        PJOB(awe_data_transport_thread, (c_data, t_end_b)));
}
