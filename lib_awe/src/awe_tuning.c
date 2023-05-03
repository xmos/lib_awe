#if 0
#include <xcore/channel.h>
#include "awe_xcore_internal.h"

// TODO: change USB side to fit this
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
            int num_words = chanend_in_word(c_control);
            for(int i = 0; i < num_words; i++) {
                int x = chanend_in_word(c_control);
                if (input_message_word_count == 0) {
                    full_packet_in_words = x >> 16;
                }
                AWE_Packet_Buffer[input_message_word_count++] = x;
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
            chanend_out_end_token(c_control);
            int max_words = chanend_in_word(c_control);
            chanend_check_end_token(c_control);
            int words = full_packet_in_words > max_words ? max_words : full_packet_in_words;
            chanend_out_word(c_control, num_words);
            for(int i = 0; i < num_words; i++) {
                x = AWE_Packet_Buffer_Reply[output_message_word_count++];
                chanend_out_word(c_control, x);
            }
            chanend_out_end_token(c_control);
        } while(output_message_word_count < full_packet_in_words);
    }
}

#if defined(AWE_USB_HID)
#include "xud_device.h"                 /* XMOS USB Device Layer defines and functions */
#include "xua_endpoint0.h"

void awe_usb_hid(chanend c_hid_to_host, chanend c_hid_from_host, chanend c_hid_control) {
    XUD_ep ep_hid_from_host = XUD_InitEp(c_hid_from_host);
    XUD_ep ep_hid_to_host = XUD_InitEp(c_hid_to_host);


    unsigned int g_hid_from_host_buffer[16];
    unsigned int g_hid_to_host_buffer[16];
    xc_ptr hid_from_host_buffer = array_to_xc_ptr(g_hid_from_host_buffer);
    

    
    /* Mark OUT endpoints ready to receive data from host */
    XUD_SetReady_OutPtr(ep_hid_from_host, hid_from_host_buffer);

    int ready_for_next_out_packet = 1;
    while(1)
    {
        XUD_Result_t result;
        unsigned length;
        unsigned char ct;
        
        /* Wait for response from XUD and service relevant EP */
        select
        {
            case ready_for_next_out_packet => inct_byref(c_hid_control, ct):
                outuint(c_hid_control, 13);
                outct(c_hid_control, 1);
                int num_words = inuint(c_hid_control);
                g_hid_to_host_buffer[0] = ((num_words << 2) << 24) | 0x00000001;
                for(int i = 0; i < num_words; i++) {
                    g_hid_to_host_buffer[i+1] = inuint(c_hid_control);
                }
                chkct(c_hid_control, 1);
                XUD_SetReady_In(ep_hid_to_host, (g_hid_to_host_buffer, unsigned char[]), 56);
                ready_for_next_out_packet = 0;
                break;
                
            /* HID OUT from host */
            case XUD_GetData_Select(c_hid_from_host, ep_hid_from_host, length, result):
                if (result == XUD_RES_OKAY) {
                    uint32_t header = g_hid_from_host_buffer[0];
                    uint32_t bytecount = header >> 24;
                    uint32_t num_words = bytecount >> 2;
                    outuint(c_hid_control, num_words);
                    for(int i = 0; i < num_words; i++) {
                        outuint(c_hid_control, g_hid_from_host_buffer[i+1]);
                    }
                    outct(c_hid_control, 1);
                }
                XUD_SetReady_OutPtr(ep_hid_from_host, hid_from_host_buffer);
                break;

            /* HID IN to host */
            case XUD_SetData_Select(c_hid_to_host, ep_hid_to_host, result):
                /* The buffer has been sent to the host, so we can ack the hid thread */
                ready_for_next_out_packet = 1;
                break;


        }
    }
}

#endif
#endif
