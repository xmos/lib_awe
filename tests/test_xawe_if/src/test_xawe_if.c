// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <print.h>
#include <stdlib.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/select.h>
#include <xcore/parallel.h>
#include <xclib.h>
#include "awe_xcore.h"

#include "../../awb_files/simple_volume_InitAWB.h"
#include "../../awb_files/simple_volume_ControlInterface.h"


DECLARE_JOB(awe_test, (chanend_t, chanend_t, chanend_t));
void awe_test(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    
    xAWEInstance_t xAWEInstance = {c_tuning_from_host, c_tuning_to_host};

    int num_words_rx = 0;

    uint32_t pPos = 0;

    printstr("Loading AWB\n");
    int err = xawe_loadAWBfromArray(&xAWEInstance, Core0_InitCommands, Core0_InitCommands_Len, &pPos);
    if(err){
        printstr("Error loading AWB: ");printint(err); printstr(" at pos: "); printintln(pPos);
        _Exit(1);
    } else {
        printstr("AWB load SUCCESS\n");
    }

    float nValue = 0.0;
    err = xawe_ctrlGetValue(&xAWEInstance, AWE_Volume1_gain_HANDLE, &nValue, 0, 1);
    float new_nValue = -30.0;
    err = xawe_ctrlSetValue(&xAWEInstance, AWE_Volume1_gain_HANDLE, &new_nValue, 0, 1);

    err = xawe_ctrlGetValue(&xAWEInstance, AWE_Volume1_gain_HANDLE, &nValue, 0, 1);

    if(nValue == new_nValue){
        printstr("Get/Set SUCCESS\n");
    } else {
        printstr("Error on get/set: ");printint(err); printstr(" "); printhex(nValue); printstr(" "); printhexln(new_nValue);
        _Exit(1);
    }


    _Exit(0);
}

DECLARE_JOB(awe_main_wrapper, (chanend_t, chanend_t, chanend_t));
#pragma stackfunction 5000
void awe_main_wrapper(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data);
}

int main(int argc, char** argv){
    channel_t c_tuning_from_host = chan_alloc();
    channel_t c_tuning_to_host = chan_alloc();
    channel_t c_data = chan_alloc();

    PAR_JOBS(
        PJOB(awe_main_wrapper, (c_tuning_from_host.end_a, c_tuning_to_host.end_a, c_data.end_a)),
        PJOB(awe_test, (
            c_tuning_from_host.end_b, c_tuning_to_host.end_b, c_data.end_b)));

}
