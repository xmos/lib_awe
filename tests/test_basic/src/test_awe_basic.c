#include <stdio.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/parallel.h>
#include "awe_xcore.h"
#include "awe_xcore_internal.h"

// DECLARE_JOB(awe_dsp_thread, (uint32_t, chanend_t, chanend_t));
// DECLARE_JOB(awe_tuning_thread, (chanend_t, chanend_t, chanend_t*));
// DECLARE_JOB(awe_data_transport_thread, (chanend_t, chanend_t*));

int main(void){
    // chanend_t c_tuning_from_host,
    //         chanend_t c_tuning_to_host,
    //                 chanend_t c_data) {
    // channel_t t[AWE_DSP_THREAD_NUM];
    // channel_t d[AWE_DSP_THREAD_NUM];
    // chanend_t t_end_b[AWE_DSP_THREAD_NUM];
    // chanend_t d_end_b[AWE_DSP_THREAD_NUM];
    // for(int i = 0; i < AWE_DSP_THREAD_NUM; i++) {
    //     t[i] = chan_alloc();
    //     t_end_b[i] = t[i].end_b;
    //     d[i] = chan_alloc();
    //     d_end_b[i] = d[i].end_b;
    // }
    printf("awe_xcore_init\n");
    awe_xcore_init();
    printf("awe_xcore_init complete\n");

//     PAR_JOBS(
//         PJOB(awe_dsp_thread, (0, t[0].end_a, d[0].end_a)),
// #if AWE_DSP_THREAD_NUM > 1
//         PJOB(awe_dsp_thread, (1, t[1].end_a, d[1].end_a)),
// #if AWE_DSP_THREAD_NUM > 2
//         PJOB(awe_dsp_thread, (2, t[2].end_a, d[2].end_a)),
// #if AWE_DSP_THREAD_NUM > 3
//         PJOB(awe_dsp_thread, (3, t[3].end_a, d[3].end_a)),
// #if AWE_DSP_THREAD_NUM > 4
//         PJOB(awe_dsp_thread, (4, t[4].end_a, d[4].end_a)),
// #if AWE_DSP_THREAD_NUM > 5
// #error "No more than 5 threads"
// #endif
// #endif
// #endif
// #endif
// #endif
//         PJOB(awe_tuning_thread, (c_tuning_from_host, c_tuning_to_host, d_end_b)),
//         PJOB(awe_data_transport_thread, (c_data, t_end_b)));
}











    // channel_t c_data = chan_alloc();
    // g_c_to_dspc = c_data.end_a;
    // awe_xcore_main(c_control_from_host, c_control_to_host, c_data.end_b);