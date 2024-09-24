// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <print.h>
#include <stdio.h>
#include <stdlib.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/hwtimer.h>
#include <xcore/select.h>
#include <xcore/parallel.h>
#include <assert.h>
#include "awe_xcore.h"
#include "awe_ffs_rpc.h"
#include "Errors.h"


DECLARE_JOB(data_path, (chanend_t));
void data_path(chanend_t c_data){

    hwtimer_t t = hwtimer_alloc();

    while(1){
        unsigned sampsToAWE[AWE_INPUT_CHANNELS] = {0};
        unsigned sampsFromAWE[AWE_OUTPUT_CHANNELS] = {0};
        awe_offload_data_to_dsp_engine(c_data, sampsToAWE, sampsFromAWE);
        hwtimer_delay(t, XS1_TIMER_HZ / 48000);
    }
    
    hwtimer_free(t);
}

DECLARE_JOB(awe_test, (chanend_t, chanend_t));
void awe_test(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host){
    init_awe_tuning_instance(c_tuning_from_host, c_tuning_to_host);

    char *awb_filenames[] = {"playBasic_3thread.awb", "simple_volume.awb"};
    int errors = 0;

    for(int i = 0; i < 1000; i++){
        char *awb_filename = awb_filenames[i % 2]; // swap each one
        int err = xawe_loadAWBfromFFS(awb_filename);
        if(err != E_SUCCESS){
            puts("ERROR - xawe_loadAWBfromFFS failed");
            printintln(err);
            errors++;
        }
        if(i % 100 == 0){
            printintln(i);
        }
    }

    if(errors){
        puts("xawe_loadAWBfromFFS FAIL");
    } else {
        puts("xawe_loadAWBfromFFS SUCCESS");
    }

  
    _Exit(0);
}

DECLARE_JOB(awe_main_wrapper, (chanend_t, chanend_t, chanend_t, chanend_t));
#pragma stackfunction 5000
__attribute__((noinline))
void awe_main_wrapper(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data, chanend_t c_ffs_rpc){
    init_ffs_rpc_client_chanend(c_ffs_rpc);
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data);
}

int main(int argc, char** argv){
    channel_t c_tuning_from_host = chan_alloc();
    channel_t c_tuning_to_host = chan_alloc();
    channel_t c_data = chan_alloc();
    channel_t c_ffs_rpc = chan_alloc();

    PAR_JOBS(
        PJOB(awe_main_wrapper, (c_tuning_from_host.end_a, c_tuning_to_host.end_a, c_data.end_a, c_ffs_rpc.end_a)),
        PJOB(awe_test, (c_tuning_from_host.end_b, c_tuning_to_host.end_b)),
        PJOB(data_path, (c_data.end_b)),
        PJOB(ffs_server, (c_ffs_rpc.end_b))
    );
}
