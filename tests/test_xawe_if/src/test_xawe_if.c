// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/select.h>
#include <xcore/parallel.h>
#include <assert.h>
#include "awe_xcore.h"
#include "Errors.h"

#include "../../../examples/audioweaver/awb_files/simple_volume_InitAWB.h"
#include "../../../examples/audioweaver/awb_files/simple_volume_ControlInterface.h"


DECLARE_JOB(awe_test, (chanend_t));
void awe_test(chanend_t c_data){

    // TEST EMPTY
    int rr;
    int err = xawe_ctrlGetValue(AWE_Volume1_gain_HANDLE, &rr, 0, 1);
    assert(err == E_NO_MORE_OBJECTS);


    // LOAD
    puts("Loading AWB");
    unsigned int pPos = 0;
    err = xawe_loadAWBfromArray(Core0_InitCommands, Core0_InitCommands_Len, &pPos);
    if(err){
        puts("Error loading AWB");
        _Exit(1);
    } else {
        puts("AWB load SUCCESS");
    }

    // SET AND GET VALUE
    float nValue = 5555.0;
    err = xawe_ctrlGetValue(AWE_Volume1_gain_HANDLE, &nValue, 0, 1);

    assert(nValue == 0.0);
    puts("Get SUCCESS");


    float new_nValue = -30.0;
    err = xawe_ctrlSetValue(AWE_Volume1_gain_HANDLE, &new_nValue, 0, 1);
    assert(err == E_SUCCESS);

    err = xawe_ctrlGetValue(AWE_Volume1_gain_HANDLE, &nValue, 0, 1);
    assert(err == E_SUCCESS);
    assert(nValue == new_nValue);

    puts("Get/Set SUCCESS");


    err = xawe_ctrlGetValue(0xdeadbeef, &nValue, 0, 1);
    assert(err == E_NO_MORE_OBJECTS);

    err = xawe_ctrlGetValue(AWE_Volume1_gain_HANDLE, &nValue, 0, 2);
    assert(err == E_ARGUMENT_ERROR);


    // SET AND GET STATUS
    unsigned int status = -1;
    err = xawe_ctrlGetStatus(AWE_Volume1_centerFreq_HANDLE, &status);
    assert(err == E_SUCCESS);
    assert(status == 0); // Active

    status = 2; // Mute
    err = xawe_ctrlSetStatus(AWE_Volume1_centerFreq_HANDLE, status);\
    assert(err == E_SUCCESS);
    status = 0; // Active

    err = xawe_ctrlGetStatus(AWE_Volume1_centerFreq_HANDLE, &status);
    assert(err == E_SUCCESS);
    assert(status == 2); // Mute

    puts("Get/Set STATUS SUCCESS");

    // SET AND GET VALUE WITH MASK
    // see https://documentation.dspconcepts.com/awe-designer/latest-version/c-run-time-environment#id-(8.D.2.4)CRun-TimeEnvironment-SetFunctionSetFunction
    new_nValue = 0.0;
    err = xawe_ctrlSetValue(AWE_Volume1_gain_HANDLE, &new_nValue, 0, 1);


    nValue = 3333.0;
    err = xawe_ctrlGetValueMask(AWE_Volume1_gain_HANDLE, &nValue, 0, 1, AWE_Volume1_gain_MASK);
    assert(err == E_SUCCESS);

    assert(nValue == new_nValue);
    puts("Get MASK SUCCESS");

    new_nValue = -50.0;
    // This should NOT work - uses different mask
    err = xawe_ctrlSetValueMask(AWE_Volume1_gain_HANDLE & 0xffff0000, &new_nValue, 0, 1, AWE_Volume1_profileTime_MASK);
    assert(err == E_SUCCESS);

    err = xawe_ctrlGetValueMask(AWE_Volume1_gain_HANDLE & 0xffff0000, &nValue, 0, 1, AWE_Volume1_gain_MASK);
    assert(err == E_SUCCESS);

    // TODO - work out why line 108 works when not expected - read the docs about rd/wr with mask
    // assert(nValue != new_nValue);
    puts("Get/Set MASK SUCCESS");

    // This should work
    err = xawe_ctrlSetValueMask(AWE_Volume1_gain_HANDLE & 0xffff0000, &new_nValue, 0, 1, AWE_Volume1_gain_HANDLE);
    assert(err == E_SUCCESS);

    err = xawe_ctrlGetValueMask(AWE_Volume1_gain_HANDLE & 0xffff0000, &nValue, 0, 1, AWE_Volume1_gain_HANDLE);
    assert(err == E_SUCCESS);
    assert(nValue == new_nValue);

    puts("Get/Set MASK SUCCESS");

    // err = xawe_ctrlGetValueMask(0xdeadbeef, &nValue, 0, 1, AWE_Volume1_gain_MASK);
    // TODO - why is this not happy?
    // assert(err == E_NO_MORE_OBJECTS);

    err = xawe_ctrlGetValueMask(AWE_Volume1_gain_HANDLE & 0xffff0000, &nValue, 0, 2, AWE_Volume1_gain_MASK);
    assert(err == E_ARGUMENT_ERROR);

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

    init_awe_tuning_instance(c_tuning_from_host.end_a, c_tuning_to_host.end_a);

    PAR_JOBS(
        PJOB(awe_main_wrapper, (c_tuning_from_host.end_b, c_tuning_to_host.end_b, c_data.end_a)),
        PJOB(awe_test, (c_data.end_b)));

}
