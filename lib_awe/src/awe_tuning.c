#if 1
#include <xcore/channel.h>
#include <xcore/select.h>
#include "awe_xcore_internal.h"

// TODO: change USB side to fit this
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
        chanend_out_end_token(c_control);
        int max_words = chanend_in_word(c_control);
        do {
            chanend_check_end_token(c_control);
            int num_words = (full_packet_in_words > max_words ?
                             max_words :
                             full_packet_in_words);
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

#if defined(AWE_TUNING_OVER_USB_HID)
#include "xud.h"
#include "xud_device.h"
#include "xua_endpoint0.h"

void awe_usb_hid(chanend_t c_hid_to_host, chanend_t c_hid_from_host, chanend_t c_tuning) {
    XUD_ep ep_hid_from_host = XUD_InitEp(c_hid_from_host);
    XUD_ep ep_hid_to_host   = XUD_InitEp(c_hid_to_host);

    // TODO: these can be combined into one.
    unsigned int g_hid_from_host_buffer[16];
    unsigned int g_hid_to_host_buffer[16];
    
    /* Mark the OUT endpoint to be ready to receive data from host */
    XUD_SetReady_Out(ep_hid_from_host, (unsigned char*)g_hid_from_host_buffer);

    int ready_for_next_out_packet = 1;
    XUD_Result_t result;
    unsigned length;
        
    /* Wait for response from XUD and service relevant EP */
    SELECT_RES(
        CASE_GUARD_THEN(c_tuning, ready_for_next_out_packet, input_from_awe),
        CASE_THEN(c_hid_from_host, xud_hid_output),
        CASE_THEN(c_hid_to_host, xud_hid_input)
        )
    {
    input_from_awe: /* Data frmo the AWE stack */
        chanend_check_end_token(c_tuning);
        chanend_out_word(c_tuning, 13);
        chanend_out_end_token(c_tuning);
        int num_words = chanend_in_word(c_tuning);
        g_hid_to_host_buffer[0] = ((num_words << 2) << 24) | 0x00000001;
        for(int i = 0; i < num_words; i++) {
            g_hid_to_host_buffer[i+1] = chanend_in_word(c_tuning);
        }
        chanend_out_end_token(c_tuning);
        chanend_check_end_token(c_tuning);
        XUD_SetReady_In(ep_hid_to_host, (unsigned char *)g_hid_to_host_buffer, 56);
        ready_for_next_out_packet = 0;
        continue;
    xud_hid_output: /* HID OUT from host */
        XUD_GetData_Select(c_hid_from_host, ep_hid_from_host, &length, &result);
        if (result == XUD_RES_OKAY) {
            uint32_t header = g_hid_from_host_buffer[0];
            uint32_t bytecount = header >> 24;
            uint32_t num_words = bytecount >> 2;
            chanend_out_word(c_tuning, num_words);
            for(int i = 0; i < num_words; i++) {
                chanend_out_word(c_tuning, g_hid_from_host_buffer[i+1]);
            }
            chanend_out_end_token(c_tuning);
            chanend_check_end_token(c_tuning);
        }
        XUD_SetReady_Out(ep_hid_from_host, (unsigned char*)g_hid_from_host_buffer);
        continue;
    xud_hid_input:  /* HID IN to host */
        XUD_SetData_Select(c_hid_to_host, ep_hid_to_host, &result);
        ready_for_next_out_packet = 1;
        continue;

    }
}

#endif
#endif
