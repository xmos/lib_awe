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
__attribute__((noinline))
void awe_main_wrapper(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data, chanend_t c_ffs_rpc){
    init_ffs_rpc_client_chanend(c_ffs_rpc);
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data);
}

extern volatile char g_AWE_IsInitialised;

#define SAVE_FILES      0

void write_dword_bin_file(const char *fname, UINT32 addr, UINT32 *data, UINT32 numwords){
#if SAVE_FILES
    FILE *fptr;
    char full_fname[128];

    sprintf(full_fname, "%s_0x%x.bin", fname, addr);
    printf("writing - %s\n", full_fname);
    fptr = fopen(full_fname, "wb");
    for(int i = 0; i < numwords; i++){
        fwrite(&data[i], sizeof(data[0]), 1, fptr);
    }
    fclose(fptr);
#endif
}

/* Address is the byte address */
void test_at_address(UINT32 addr){
    #define BUFF_SIZE_DWORDS 512

    UINT32 buffer[BUFF_SIZE_DWORDS] = {0};
    UINT32 buff_size_bytes = BUFF_SIZE_DWORDS * sizeof(UINT32);

    // Extract DP and sector sizes
    UINT32 dp_size = 0;
    UINT32 sector_size_bytes = 0;
    ffs_rpc_get_flash_info(&dp_size, &sector_size_bytes);
    printf("FLASH d.p. size: 0x%x sector size: %u\n", dp_size, sector_size_bytes);

    // Grab current contents
    xassert(usrReadFlashMemory(addr, buffer, BUFF_SIZE_DWORDS) == 1);
    write_dword_bin_file("init", addr, buffer, BUFF_SIZE_DWORDS);

    // Now write a known pattern
    for(int i = 0; i < BUFF_SIZE_DWORDS; i++){
        buffer[i] = 0x1000 + i;
    }
    xassert(usrWriteFlashMemory(addr, buffer, BUFF_SIZE_DWORDS) == 1);
    printf("usrWriteFlashMemory OK\n");

    // Verify and save this pattern
    xassert(usrReadFlashMemory(addr, buffer, BUFF_SIZE_DWORDS) == 1);
    printf("usrReadFlashMemory OK\n");
    write_dword_bin_file("increment", addr, buffer, BUFF_SIZE_DWORDS);
    for(int i = 0; i < BUFF_SIZE_DWORDS; i++){
        xassert(buffer[i] == 0x1000 + i);
    }
    printf("Verify OK\n");

    // TODO GET THIS CALC SORTED
    // Now erase
    UINT32 erase_sector_base_addr = addr - (addr % sector_size_bytes);
    UINT32 erase_sector_end_addr = addr + buff_size_bytes;
    int num_sectors = ((sector_size_bytes - 1) + (erase_sector_end_addr - erase_sector_base_addr)) / sector_size_bytes; // round up to sector size

    xassert(usrEraseFlashSector(addr, num_sectors) == 1);
    printf("usrEraseFlashSector, num_sectors: (%d) OK\n", num_sectors);

    // Grab empty flash
    xassert(usrReadFlashMemory(addr, buffer, BUFF_SIZE_DWORDS) == 1);
    printf("usrReadFlashMemory OK\n");
    write_dword_bin_file("empty", addr, buffer, BUFF_SIZE_DWORDS);
    for(int i = 0; i < BUFF_SIZE_DWORDS; i++){
        xassert(buffer[i] == 0xffffffff);
    }
    printf("Verify OK\n\n");
}

DECLARE_JOB(awe_ffs_test, (chanend_t));
void awe_ffs_test(chanend_t c_ffs_rpc){

    printf("awe_ffs_test\n");
    while(!g_AWE_IsInitialised);
    printf("g_AWE_IsInitialised OK\n");

    // Check we can re-init without issues (awe_init will have already done this)
    xassert(usrInitFlashFileSystem() == 1);
    printf("usrInitFlashFileSystem OK\n");

    test_at_address(0x0000);
    test_at_address(0x2000);
    test_at_address(0x4100); // In the middle of sector
    test_at_address(0x4e00); // Straddles a sector
    test_at_address(0x10000);


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
