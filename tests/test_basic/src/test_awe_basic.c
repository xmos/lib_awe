#include <stdio.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/parallel.h>
#include "awe_xcore.h"

DECLARE_JOB(awe_test, (chanend_t, chanend_t, chanend_t));
void awe_test(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    printf("awe_test\n");

}

DECLARE_JOB(awe_main_wrapper, (chanend_t, chanend_t, chanend_t));
#pragma stackfunction 1024
void awe_main_wrapper(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    printf("awe_xcore_main\n");
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data);
}

int main(void){
    channel_t c_tuning_from_host = chan_alloc();
    channel_t c_tuning_to_host = chan_alloc();
    channel_t c_data = chan_alloc();

    PAR_JOBS(
        PJOB(awe_main_wrapper, (c_tuning_from_host.end_a, c_tuning_to_host.end_a, c_data.end_a)),
        PJOB(awe_test, (c_tuning_from_host.end_b, c_tuning_to_host.end_b, c_data.end_b)));

}
