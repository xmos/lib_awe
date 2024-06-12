// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <print.h>
#include <stdlib.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/select.h>
#include <xcore/parallel.h>
#include <assert.h>
#include "awe_xcore.h"
#include "Errors.h"

#include "../../awb_files/simple_volume_InitAWB.h"
#include "../../awb_files/simple_volume_ControlInterface.h"


DECLARE_JOB(awe_test, (chanend_t, chanend_t, chanend_t));
void awe_test(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    
    xAWEInstance_t xAWEInstance = {c_tuning_from_host, c_tuning_to_host};

    int rr;
    int err = xawe_ctrlGetValue(&xAWEInstance, AWE_Volume1_gain_HANDLE, &rr, 0, 1);
    assert(err == E_NO_MORE_OBJECTS);


    printstr("Loading AWB\n");
    unsigned int pPos = 0;
    err = xawe_loadAWBfromArray(&xAWEInstance, Core0_InitCommands, Core0_InitCommands_Len, &pPos);
    if(err){
        printstr("Error loading AWB: ");printint(err); printstr(" at pos: "); printintln(pPos);
        _Exit(1);
    } else {
        printstr("AWB load SUCCESS\n");
    }

    float nValue = 5555.0;
    err = xawe_ctrlGetValue(&xAWEInstance, AWE_Volume1_gain_HANDLE, &nValue, 0, 1);

    if(nValue == 0.0){
        printstr("Get SUCCESS\n");
    } else {
        printstr("Error on get: ");printint(err); printstr(", Expected 0.0 got: "); printhex(nValue);;
        _Exit(1);
    }

    float new_nValue = -30.0;
    err = xawe_ctrlSetValue(&xAWEInstance, AWE_Volume1_gain_HANDLE, &new_nValue, 0, 1);

    err = xawe_ctrlGetValue(&xAWEInstance, AWE_Volume1_gain_HANDLE, &nValue, 0, 1);

    if(nValue == new_nValue){
        printstr("Get/Set SUCCESS\n");
    } else {
        printstr("Error on get/set: ");printint(err); printstr(", "); printhex(nValue); printstr(" "); printhexln(new_nValue);
        _Exit(1);
    }

    err = xawe_ctrlGetValue(&xAWEInstance, 0xdeadbeef, &nValue, 0, 1);
    assert(err == E_NO_MORE_OBJECTS);

    err = xawe_ctrlGetValue(&xAWEInstance, AWE_Volume1_gain_HANDLE, &nValue, 0, 2);
    assert(err == E_ARGUMENT_ERROR);


    unsigned int status = -1;
    err = xawe_ctrlGetStatus(&xAWEInstance, AWE_Volume1_centerFreq_HANDLE, &status);
    assert(err == E_SUCCESS);
    assert(status == 0); // Active

    status = 2; // Mute
    err = xawe_ctrlSetStatus(&xAWEInstance, AWE_Volume1_centerFreq_HANDLE, status);\
    assert(err == E_SUCCESS);
    status = 0; // Active

    err = xawe_ctrlGetStatus(&xAWEInstance, AWE_Volume1_centerFreq_HANDLE, &status);
    assert(err == E_SUCCESS);
    assert(status == 2); // Mute

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
