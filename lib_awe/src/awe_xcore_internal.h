// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef _AWE_XCORE_INTERNAL_H
#define _AWE_XCORE_INTERNAL_H

#include <xcore/chanend.h>
#include "AWECore.h"
#include "Errors.h"
#include "awe_xcore.h"

// The global AWE instance.

extern AWEInstance2 g_AWEInstance;
extern UINT32 AWE_Packet_Buffer[AWE_HID_PACKET_BUFFER_SIZE];

/**
@brief The XMOS AWE instance.
@details For XMOS this is an array of channel ends which represent
the public API to the XMOS AWE instance and presents a tuning interface suitable for USB, internal use or other IO interfaces specified by the user.
*/
typedef struct xAWETuningInstance_t{
    chanend_t c_tuning_from_host;
    chanend_t c_tuning_to_host;
    lock_t l_api_lock;
} xAWETuningInstance_t;


/** Function that initialises all global data structures
 */
void awe_xcore_init();

/** Thread that implements control over a HID endpoint. This function does
 * not return
 *
 * \param c_control channelend over which control packets arrive
 */
void awe_tuning_thread(chanend_t c_control_from_host,
                       chanend_t c_tuning_to_host,
                       chanend_t c_deferred_work[AWE_DSP_MAX_THREAD_NUM]);

/** Thread that implements data transport between the AWE DSP tasks and
 * the rest of the system. This function does not return
 *
 * \param c_data      The channelend over which audio is communicated. Either
 *                    use ``awe_offload_data_to_dsp_engine`` or output
 *                    AWE_INPUT_CHANNELS ints over the channel followed by
 *                    CT_END, then input AWE_OUTPUT_CHANNELS ints over the
 *                    channelend and then input a CT_END.
 */
void awe_data_transport_thread(chanend_t c_data, chanend_t c_children[]);

/** Thread that implements one chunk of DSP. This function does not return.
 *
 * \param thread_number number of the thread, [0.. AWE_DSP_THREAD_NUM-1]
 *
 * \param c_parent      channelend to the main task. a single CT_END token
 *                      must be posted on this channelend for the DSP
 *                      thread to jump into action.
 *
 * \param c_deferred_work channelend to a task that can perform deferred work.
 *                      Anytime deferred work is due, an END token is popped into
 *                      it to require deferred work to be computed.
 *                      This is acknowledged from the other side.
 */
void awe_dsp_thread(uint32_t thread_number, chanend_t c_parent,
                    chanend_t c_deferred_work);

#endif
