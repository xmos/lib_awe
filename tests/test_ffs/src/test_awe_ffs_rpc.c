// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/select.h>
#include <xcore/hwtimer.h>
#include <xcore/parallel.h>
#include <xcore/assert.h>
#include <xclib.h>

#include "awe_xcore_internal.h"
#include "AWECoreUtils.h"
#include "awe_ffs_rpc.h"


DECLARE_JOB(awe_main_wrapper, (chanend_t, chanend_t, chanend_t, chanend_t));
#pragma stackfunction 8000
void awe_main_wrapper(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data, chanend_t c_ffs_rpc){
    init_ffs_rpc_client_chanend(c_ffs_rpc);
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data);
}

extern volatile char g_AWE_IsInitialised;


void write_dword_bin_file(const char *fname, UINT32 *data, UINT32 numwords){
    FILE *fptr;
    fptr = fopen(fname,"wb");
    for(int i = 0; i < numwords; i++){
        fwrite(&data[i], sizeof(data[0]), 1, fptr);
    }
    fclose(fptr);
}

DECLARE_JOB(awe_ffs_test, (chanend_t));
void awe_ffs_test(chanend_t c_ffs_rpc){

    printf("awe_ffs_test\n");
    while(!g_AWE_IsInitialised);
    printf("awe_ffs_test\n");

#define BUFF_SIZE_DWORDS 512
    UINT32 buffer[BUFF_SIZE_DWORDS] = {0};
    
    xassert(usrInitFlashFileSystem() == 1);
    xassert(usrReadFlashMemory(0, buffer, BUFF_SIZE_DWORDS) == 1);
    write_dword_bin_file("init.bin", buffer, 64);

    for(int i = 0; i < BUFF_SIZE_DWORDS; i++){
        buffer[i] = 0x1000 + i;
    }
    xassert(usrWriteFlashMemory(0, buffer, 256) == 1);

    xassert(usrReadFlashMemory(0, buffer, BUFF_SIZE_DWORDS) == 1);
    write_dword_bin_file("increment.bin", buffer, 64);


    xassert(usrEraseFlashSector(0, 1) == 1);
    xassert(usrReadFlashMemory(0, buffer, BUFF_SIZE_DWORDS) == 1);
    write_dword_bin_file("empty.bin", buffer, 64);


    _Exit(0);
}

int main(int argc, char** argv){
    channel_t c_tuning_from_host = chan_alloc();
    channel_t c_tuning_to_host = chan_alloc();
    channel_t c_data = chan_alloc();
    channel_t c_ffs_rpc = chan_alloc();

    PAR_JOBS(
        PJOB(ffs_server, (c_ffs_rpc.end_a)),
        PJOB(awe_main_wrapper, (c_tuning_from_host.end_a, c_tuning_to_host.end_a, c_data.end_a, c_ffs_rpc.end_b)),
        PJOB(awe_ffs_test, ()));

}
