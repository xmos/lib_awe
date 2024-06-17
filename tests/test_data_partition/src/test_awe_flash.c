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
#include <xclib.h>

#include "awe_xcore_internal.h"
#include "ProxyIDs.h"
#include "AWECoreUtils.h"

typedef int ERROR_CODE;

/* ----------------------------------------------------------------------
** Specify flash memory available for flash file system
** ------------------------------------------------------------------- */
#define FLASH_MEMORY_SIZE_IN_BYTES 0x4000000
#define ERASEABLE_SECTOR_SIZE 0x10000
#define FILE_SYSTEM_START_OFFSET 0xB0000
#define SECTOR_ERASE_TIME_MS    400



BOOL usrInitFlashFileSystem(void)
{

    printf("usrInitFlashFileSystem\n");

    // Implement any flash specific initializations
    return 1;
}   // End usrInitFlashFileSystem


///-----------------------------------------------------------------------------
/// @name  BOOL usrReadFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToRead)
/// @brief Read 4-byte words from flash memory
///
/// @param[in]  UINT32  nAddress - address in flash to start reading
/// @param[in]  UINT32 *pBuffer  - buffer to read into
/// @param[in]  UINT32  nDWordsToRead - number of 4-bytes elements to read
///
/// @retval     SUCCESS - read succeeded
/// @retval     FAILURE - read failed
///-----------------------------------------------------------------------------
BOOL usrReadFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToRead)
{
    BOOL bSuccess = 0;

    // Check for the count zero and skip remaining
    if(nDWordsToRead == 0)
        return 1;

    // Flash specific read implementation

    printf("usrReadFlashMemory\n");


    return 1;
}   // End usrReadFlashMemory





///-----------------------------------------------------------------------------
/// @name  BOOL usrWriteFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToWrite)
/// @brief Write 4-byte words to flash memory
///
/// @param[in]  UINT32  nAddress - address in flash to start writing
/// @param[in]  UINT32 * pBuffer  - buffer to write into
/// @param[in]  UINT32  nDWordsToWrite - number of 4-bytes elements to write
///
/// @retval     SUCCESS - write succeeded
/// @retval     FAILURE - write failed
///-----------------------------------------------------------------------------
BOOL usrWriteFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToWrite)
{
    BOOL bSuccess = 0;
    printf("usrWriteFlashMemory\n");

    // Check for the count zero and skip remaining
    if(nDWordsToWrite == 0)
        return 1;

    // Flash device write specific implementation


    return 1;
}   // End usrWriteFlashMemory



///-----------------------------------------------------------------------------
/// @name  BOOL usrEraseFlashSector(UINT32 nStartingAddress, UINT32 nNumberOfSectors)
/// @brief Erase flash memory starting at address for number of sectors
///
/// @param[in]  UINT32  nStartingAddress - address in flash to start erasing
/// @param[in]  UINT32  nNumberOfSectors  - number of flash memory sectors to erase
///
/// @retval     SUCCESS - erase succeeded
/// @retval     FAILURE - erase failed
///-----------------------------------------------------------------------------
//AWE_OPTIMIZE_FOR_SPEED
BOOL usrEraseFlashSector(UINT32 nStartingAddress, UINT32 nNumberOfSectors)
{
    printf("usrEraseFlashSector\n");


    UINT32 nSectorAddress, index, nSector;
    // ERROR_CODE ErrorCode = NO_ERR;


    nSectorAddress = nStartingAddress;

    // Loop through number of sectors and erase each sector
    for (index = 0; index < nNumberOfSectors; index++)
    {
        // Flash device specific sector erase implementation

        nSectorAddress += ERASEABLE_SECTOR_SIZE;
    }


    return 1;

}   // End usrEraseFlashMemorySector

DECLARE_JOB(awe_main_wrapper, (chanend_t, chanend_t, chanend_t));
#pragma stackfunction 8000
void awe_main_wrapper(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    hwtimer_realloc_xc_timer();
    delay_milliseconds(1);

    printf("awe_xcore_main\n");
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data);
}

/** Flash file system instance */
AWEFlashFSInstance2 g_AWEFlashFSInstance;
extern AWEInstance2 g_AWEInstance;

DECLARE_JOB(awe_flash_test, (void));
#pragma stackfunction 8000
void awe_flash_test(void){
    hwtimer_realloc_xc_timer();
    printf("Hello\n");

    // Setup the flash file system */
    memset(&g_AWEFlashFSInstance, 0, sizeof(AWEFlashFSInstance2) );

    g_AWEFlashFSInstance.cbInit = &usrInitFlashFileSystem;
    g_AWEFlashFSInstance.cbEraseSector = &usrEraseFlashSector;
    g_AWEFlashFSInstance.cbFlashWrite = &usrWriteFlashMemory;
    g_AWEFlashFSInstance.cbFlashRead = &usrReadFlashMemory;

    g_AWEFlashFSInstance.flashSizeInBytes = FLASH_MEMORY_SIZE_IN_BYTES;
    g_AWEFlashFSInstance.flashErasableBlockSizeInBytes = ERASEABLE_SECTOR_SIZE;
    g_AWEFlashFSInstance.flashStartOffsetInBytes = FILE_SYSTEM_START_OFFSET;
    g_AWEFlashFSInstance.flashEraseTimeInMs = (INT32)((FLOAT32)((( (FLASH_MEMORY_SIZE_IN_BYTES - FILE_SYSTEM_START_OFFSET)/ ERASEABLE_SECTOR_SIZE)*SECTOR_ERASE_TIME_MS/1000) + 0.5f) + 5);

    printf("pre init fs\n");
    awe_initFlashFS((AWEInstance *)&g_AWEInstance, (AWEFlashFSInstance *)&g_AWEFlashFSInstance);
    g_AWEInstance.pFlashFileSystem = (AWEFlashFSInstance *)&g_AWEFlashFSInstance;
    printf("post init fs\n");

    // awe_init((AWEInstance*)&g_AWEInstance);

    // Wait for init on other thread
    delay_milliseconds(2);

    unsigned int pPos = 0;
    // aweOpenFile(0, "greee", 30);
    // awe_fwWriteFile
    // awe_fwReadFile
    printf("pre load AWB\n");
    int res = awe_loadAWBfromFlash((AWEInstance *)&g_AWEInstance, "test.awb", &pPos);
    // int res = awe_pltExecuteFile((AWEInstance *)&g_AWEInstance, (AWEFlashFSInstance *)&g_AWEFlashFSInstance, "raahh");
    printf("post load AWB: %d\n", res);

    // awe_loadAWBfromArray((AWEInstance *)&g_AWEInstance, &pPos, 100, &pPos);
    // awe_layoutIsValid((AWEInstance *)&g_AWEInstance);
    // awe_packetProcess((AWEInstance *)&g_AWEInstance);


}

int main(int argc, char** argv){
    channel_t c_tuning_from_host = chan_alloc();
    channel_t c_tuning_to_host = chan_alloc();
    channel_t c_data = chan_alloc();

    PAR_JOBS(
        PJOB(awe_main_wrapper, (c_tuning_from_host.end_a, c_tuning_to_host.end_a, c_data.end_a)),
        PJOB(awe_flash_test, ()));

}
