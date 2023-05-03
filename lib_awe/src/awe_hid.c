#include <xcore/channel.h>
#include "awe_xcore_internal.h"

// TODO: move this to USB side
// Leave a stub here that calls awe_packet_process.
// TODO: clean up all the 13s and 52s
// TODO: cleanup stacksize

#pragma stackfunction 2048
void awe_tuning_thread(chanend_t c_control) {
    while(1) {
        int input_message_word_count = 0;
        int full_packet_in_words = 0;
        for(int i = 0; i < AWE_HID_PACKET_BUFFER_SIZE;i++) {
            AWE_Packet_Buffer[i] = 0;
        }
        do {
            
            uint32_t header = chanend_in_word(c_control);
            uint32_t bytecount = header >> 24;
            uint32_t wordcount = bytecount >> 2;
            for(int i = 0; i < 52/4+0*AWE_HID_PACKET_BUFFER_SIZE; i++) {
                int x = chanend_in_word(c_control);
                if (input_message_word_count == 0) {
                    full_packet_in_words = x >> 16;
                }
                if (i < wordcount) {
                    AWE_Packet_Buffer[input_message_word_count++] = x;
                }
            }
            chanend_check_end_token(c_control);
        } while(input_message_word_count < full_packet_in_words);
        awe_packetProcess((AWEInstance *)&g_AWEInstance);
        full_packet_in_words = AWE_Packet_Buffer_Reply[0]>>16;
        if (full_packet_in_words > AWE_HID_PACKET_BUFFER_SIZE) {
            full_packet_in_words = AWE_HID_PACKET_BUFFER_SIZE;
        }
        int output_message_word_count = 0;
        do {
            int words = full_packet_in_words > 13 ? 13 : full_packet_in_words;
            int header = ((words << 2) << 24) | 0x00000001;
            chanend_out_word(c_control, header);
            for(int i = 0; i < 52/4+0*AWE_HID_PACKET_BUFFER_SIZE; i++) {
                int x;
                if (full_packet_in_words > 0) {
                    x = AWE_Packet_Buffer_Reply[output_message_word_count++];
                    full_packet_in_words--;
                } else {
                    x = 0;
                }
                chanend_out_word(c_control, x);
            }
            chanend_out_end_token(c_control);
        } while(full_packet_in_words > 0);
    }
}
