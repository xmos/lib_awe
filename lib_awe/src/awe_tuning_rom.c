#include <xcore/channel.h>
#include <xcore/select.h>
#include <print.h>
#if defined(AWE_TUNING_OVER_USB_HID)

#include "ProxyIDs.h"
#define DEFINE_ERROR_STRINGS 0 // Prints error code as str but costs memory. Define to 0 for enum.
#include "Errors.h"
#include "awe_xcore_internal.h"

#include "playBasic_3thread_InitAWB.h"


int load_awb_lib_awe(const unsigned int *awb_c_array, size_t num_words, chanend_t c_tuning_from_host, chanend_t c_tuning_to_host){
    int success = 0;

    unsigned int stop_audio = 0x20000 + PFID_StopAudio;
    send_pkt(c_tuning_from_host, 1, &stop_audio);
    success |= get_response(c_tuning_to_host);
    delay_milliseconds(1); // Required to allow audio to stop before issuing destroy as part of AWB

    unsigned int cmd_idx = 0;
    while(cmd_idx < Core0_InitCommands_Len){
        unsigned int *msg_header = (unsigned int *)&awb_c_array[cmd_idx];
        if(*msg_header == PFID_Undefined){
            break;
        }

        unsigned int num_words = (*msg_header >> 16) - 1;
        send_pkt(c_tuning_from_host, num_words, msg_header);

        success |= get_response(c_tuning_to_host);
        cmd_idx += num_words;
    }

    return success;
}

void awe_rom_loader(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host) {
    while(1){
        // Reload every second
        delay_milliseconds(1000);

        load_awb_lib_awe(Core0_InitCommands, Core0_InitCommands_Len, c_tuning_from_host, c_tuning_to_host);
    }
}

#endif