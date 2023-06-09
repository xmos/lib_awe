#include <xcore/channel.h>
#include <xcore/select.h>
#include <stdio.h>
#include "awe_xcore_internal.h"

#if AWE_DSP_MAX_THREAD_NUM > 5
#error "AWE_DSP_MAX_THREAD_NUM should be 5 or tuning needs to be extended"
#endif

// TODO: cleanup stacksize

#pragma stackfunction 1024
void awe_tuning_thread(chanend_t c_control_from_host,
                       chanend_t c_control_to_host,
                       chanend_t c_deferred_work[AWE_DSP_MAX_THREAD_NUM]) {
    int input_message_word_count = 0;
    int full_packet_in_words = 0;
    int output_message_word_count = 0;
    int num_words, max_words;
    int deferred_thread_mask = 0;
    int bMoreProcessingRequired = 0;
    SELECT_RES(
        CASE_THEN(c_deferred_work[0],  deferred_0),
        CASE_THEN(c_deferred_work[1],  deferred_1),
        CASE_THEN(c_deferred_work[2],  deferred_2),
        CASE_THEN(c_deferred_work[3],  deferred_3),
        CASE_THEN(c_deferred_work[4],  deferred_4),
        DEFAULT_GUARD_THEN(deferred_thread_mask, deferred_work),
        CASE_THEN(c_control_from_host, control_from_host),
        CASE_THEN(c_control_to_host,   control_to_host)
        )
    {
    deferred_0:
        chanend_check_end_token(c_deferred_work[0]);
        deferred_thread_mask |= 1 << 0;
        continue;
    deferred_1:
        chanend_check_end_token(c_deferred_work[1]);
        deferred_thread_mask |= 1 << 1;
        continue;
    deferred_2:
        chanend_check_end_token(c_deferred_work[2]);
        deferred_thread_mask |= 1 << 2;
        continue;
    deferred_3:
        chanend_check_end_token(c_deferred_work[3]);
        deferred_thread_mask |= 1 << 3;
        continue;
    deferred_4:
        chanend_check_end_token(c_deferred_work[4]);
        deferred_thread_mask |= 1 << 4;
        continue;
    deferred_work:
        bMoreProcessingRequired = awe_deferredSetCall((AWEInstance *)&g_AWEInstance);
        if (!bMoreProcessingRequired) {
            for(int i = 0; i < AWE_DSP_MAX_THREAD_NUM; i++) {
                if ((deferred_thread_mask >> i) & 1) {
                    chanend_out_end_token(c_deferred_work[i]);
                }
            }
            deferred_thread_mask = 0;
        }
        continue;
    control_from_host:
        num_words = chanend_in_word(c_control_from_host);
        for(int i = 0; i < num_words; i++) {
            int x = chanend_in_word(c_control_from_host);
            if (input_message_word_count == 0) {
                full_packet_in_words = x >> 16;
            }
            AWE_Packet_Buffer[input_message_word_count++] = x;
        }
        chanend_out_end_token(c_control_from_host);
        chanend_check_end_token(c_control_from_host);
        if (input_message_word_count >= full_packet_in_words) {
            input_message_word_count = 0;
            awe_packetProcess((AWEInstance *)&g_AWEInstance);
            full_packet_in_words = AWE_Packet_Buffer[0]>>16;
            if (full_packet_in_words > AWE_HID_PACKET_BUFFER_SIZE) {
                full_packet_in_words = AWE_HID_PACKET_BUFFER_SIZE;
            }
            output_message_word_count = 0;
            chanend_out_end_token(c_control_to_host);
        }
        continue;
    control_to_host:
        max_words = chanend_in_word(c_control_to_host);
        chanend_check_end_token(c_control_to_host);
        num_words = full_packet_in_words - output_message_word_count;
        if (num_words > max_words) {
            num_words = max_words;
        }
        chanend_out_word(c_control_to_host, num_words);
        for(int i = 0; i < num_words; i++) {
            int x = AWE_Packet_Buffer[output_message_word_count++];
            chanend_out_word(c_control_to_host, x);
        }
        chanend_out_end_token(c_control_to_host);
        chanend_check_end_token(c_control_to_host);
        if (output_message_word_count < full_packet_in_words) {
            chanend_out_end_token(c_control_to_host);
        }
        continue;
    }
}
