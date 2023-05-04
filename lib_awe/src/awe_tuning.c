#include <xcore/channel.h>
#include <stdio.h>
#include "awe_xcore_internal.h"

// TODO: cleanup stacksize

#pragma stackfunction 2048
void awe2_tuning_thread(chanend_t c_control) {
    while(1) {
        int input_message_word_count = 0;
        int full_packet_in_words = 0;
        for(int i = 0; i < AWE_HID_PACKET_BUFFER_SIZE;i++) {
            AWE_Packet_Buffer[i] = 0;
        }
        do {
            int num_words = chanend_in_word(c_control);
            for(int i = 0; i < num_words; i++) {
                int x = chanend_in_word(c_control);
                if (input_message_word_count == 0) {
                    full_packet_in_words = x >> 16;
                }
                AWE_Packet_Buffer[input_message_word_count++] = x;
            }
            chanend_out_end_token(c_control);
            chanend_check_end_token(c_control);
        } while(input_message_word_count < full_packet_in_words);
        awe_packetProcess((AWEInstance *)&g_AWEInstance);
        full_packet_in_words = AWE_Packet_Buffer_Reply[0]>>16;
        if (full_packet_in_words > AWE_HID_PACKET_BUFFER_SIZE) {
            full_packet_in_words = AWE_HID_PACKET_BUFFER_SIZE;
        }
        int output_message_word_count = 0;
        do {
            chanend_out_end_token(c_control);
            int max_words = chanend_in_word(c_control);
            chanend_check_end_token(c_control);
            int num_words = full_packet_in_words - output_message_word_count;
            if (num_words > max_words) {
                num_words = max_words;
            }
            chanend_out_word(c_control, num_words);
            for(int i = 0; i < num_words; i++) {
                int x = AWE_Packet_Buffer_Reply[output_message_word_count++];
                chanend_out_word(c_control, x);
            }
            chanend_out_end_token(c_control);
            chanend_check_end_token(c_control);
        } while(output_message_word_count < full_packet_in_words);
    }
}
