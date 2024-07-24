// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include <xcore/select.h>
#include "awe_xcore_internal.h"

#include "xud.h"
#include "xud_device.h"

extern xAWETuningInstance_t g_xAWETuningInstance;

#define HID_PACKET_SIZE_BYTES   56  // Fixed by AWE server
#define HID_MAX_MESSAGE_WORDS   (HID_PACKET_SIZE_BYTES / 4 - 1)

void awe_usb_hid(chanend_t c_hid_to_host, chanend_t c_hid_from_host) {
    XUD_ep ep_hid_from_host = XUD_InitEp(c_hid_from_host);
    XUD_ep ep_hid_to_host   = XUD_InitEp(c_hid_to_host);

    // TODO: these can be combined into one.
    unsigned int g_hid_from_host_buffer[16];
    unsigned int g_hid_to_host_buffer[16];

    /* Mark the OUT endpoint to be ready to receive data from host */
    XUD_SetReady_Out(ep_hid_from_host, (unsigned char*)g_hid_from_host_buffer);

    int ready_for_next_response_packet = 0; // This guards the select which receives responses from AWE
    XUD_Result_t result;                    // Because we share the channel end from two different threads
    unsigned length;                        // potentially we only want to select on it if not already busy

    /* Wait for response from XUD and service relevant EP */
    SELECT_RES(
        CASE_GUARD_THEN(g_xAWETuningInstance.c_tuning_to_host, ready_for_next_response_packet, input_from_awe),
        CASE_THEN(c_hid_from_host, xud_hid_output),
        CASE_THEN(c_hid_to_host, xud_hid_input)
        )
    {
    input_from_awe: /* Reply packet from the AWE stack */
        chanend_check_end_token(g_xAWETuningInstance.c_tuning_to_host);
        chanend_out_word(g_xAWETuningInstance.c_tuning_to_host, HID_MAX_MESSAGE_WORDS);
        chanend_out_end_token(g_xAWETuningInstance.c_tuning_to_host);
        int num_words = chanend_in_word(g_xAWETuningInstance.c_tuning_to_host);
        g_hid_to_host_buffer[0] = ((num_words << 2) << 24) | 0x00000001; // Form HID header

        for(int i = 0; i < num_words; i++) {
            g_hid_to_host_buffer[i+1] = chanend_in_word(g_xAWETuningInstance.c_tuning_to_host);
        }
        int response_finished = chanend_in_word(g_xAWETuningInstance.c_tuning_to_host);
        ready_for_next_response_packet = 0; // Clear before synch
        chanend_out_end_token(g_xAWETuningInstance.c_tuning_to_host);
        chanend_check_end_token(g_xAWETuningInstance.c_tuning_to_host);
        // If we have transmitted the whole response then unlock the tuning channels
        if(response_finished){
            lock_release(g_xAWETuningInstance.l_api_lock);
        }

        XUD_SetReady_In(ep_hid_to_host, (unsigned char *)g_hid_to_host_buffer, HID_PACKET_SIZE_BYTES);

        continue;
    xud_hid_output: /* HID OUT from host */
        XUD_GetData_Select(c_hid_from_host, ep_hid_from_host, &length, &result);
        if (result == XUD_RES_OKAY) {
            uint32_t hid_header = g_hid_from_host_buffer[0];
            uint32_t sequence =  ((hid_header >> 8) & 0xff) - 1;// AWE sends a sequence number for HID packets
            uint32_t bytecount = hid_header >> 24;
            uint32_t num_words = bytecount >> 2;
            // If the first packet of a message then lock the tuning channels
            if(sequence == 0){
                lock_acquire(g_xAWETuningInstance.l_api_lock);
                ready_for_next_response_packet = 1; // enable reception of response
            }
            chanend_out_word(g_xAWETuningInstance.c_tuning_from_host, num_words);
            for(int i = 0; i < num_words; i++) {
                chanend_out_word(g_xAWETuningInstance.c_tuning_from_host, g_hid_from_host_buffer[i+1]);
            }
            chanend_out_end_token(g_xAWETuningInstance.c_tuning_from_host);
            chanend_check_end_token(g_xAWETuningInstance.c_tuning_from_host);
        }
        XUD_SetReady_Out(ep_hid_from_host, (unsigned char*)g_hid_from_host_buffer);

        continue;
    xud_hid_input:  /* HID IN to host. This indicates the previous XUD_SetReady_In is complete */
        XUD_SetData_Select(c_hid_to_host, ep_hid_to_host, &result);
        ready_for_next_response_packet = 1;

        continue;

    }
}
