// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include <xcore/hwtimer.h>
#include <stdio.h>
#include "assert.h"
#include "awe_xcore_internal.h"

AWEInstance2 g_AWEInstance = {0};
static IOPinDescriptor2 g_InputPin;
static IOPinDescriptor2 g_OutputPin;

extern const void* g_module_descriptor_table[];
extern const void* g_module_descriptor_table_end[];

static UINT32 g_AWEHeap[AWE_HEAP_SIZE];

UINT32 AWE_Packet_Buffer[AWE_HID_PACKET_BUFFER_SIZE];

#if USE_AWE_FLASH_FILE_SYSTEM
#include "awe_ffs_rpc.h"
AWEFlashFSInstance2 g_AWEFlashFSInstance;
#endif

volatile char g_AWE_IsInitialised = 0;

void awe_xcore_init() {
    g_AWEInstance.instanceId = 0;
    g_AWEInstance.pInputPin = (IOPinDescriptor *)&g_InputPin;
    g_AWEInstance.pOutputPin = (IOPinDescriptor *)&g_OutputPin;
    g_AWEInstance.pPacketBuffer = AWE_Packet_Buffer;
    g_AWEInstance.pReplyBuffer = AWE_Packet_Buffer;
    g_AWEInstance.packetBufferSize = AWE_HID_PACKET_BUFFER_SIZE;
    g_AWEInstance.pModuleDescriptorTable = g_module_descriptor_table;
    g_AWEInstance.numModules = g_module_descriptor_table_end - g_module_descriptor_table;
    g_AWEInstance.numThreads = AWE_DSP_THREAD_NUM;
    g_AWEInstance.sampleRate = 48000.0f;

    g_AWEInstance.fundamentalBlockSize = AWE_BLOCK_SIZE;
    g_AWEInstance.pFlashFileSystem = 0;

    g_AWEInstance.fastHeapASize = AWE_HEAP_SIZE;
    g_AWEInstance.fastHeapBSize = 0;
    g_AWEInstance.slowHeapSize  = 0;
    g_AWEInstance.pFastHeapA    = g_AWEHeap;
    g_AWEInstance.pFastHeapB    = NULL;
    g_AWEInstance.pSlowHeap     = NULL;

    g_AWEInstance.coreSpeed     = 100e6f;
    g_AWEInstance.profileSpeed  = 12.5e6f;

    g_AWEInstance.pName = "XMOS_USB_Audio";
    g_AWEInstance.userVersion = 0x20230406;

#if USE_AWE_FLASH_FILE_SYSTEM
    // Init the remote flash server and fetch flash info for setting up AWE FFS
    usrInitFlashFileSystem();
    UINT32 flash_memory_size_in_bytes = 0;
    UINT32 eraseable_sector_size = 0;
    ffs_rpc_get_flash_info(&flash_memory_size_in_bytes, &eraseable_sector_size);

    memset(&g_AWEFlashFSInstance, 0, sizeof(g_AWEFlashFSInstance));

    g_AWEFlashFSInstance.cbInit = &usrInitFlashFileSystem;
    g_AWEFlashFSInstance.cbEraseSector = &usrEraseFlashSector;
    g_AWEFlashFSInstance.cbFlashWrite = &usrWriteFlashMemory;
    g_AWEFlashFSInstance.cbFlashRead = &usrReadFlashMemory;


    g_AWEFlashFSInstance.flashSizeInBytes = flash_memory_size_in_bytes;
    g_AWEFlashFSInstance.flashErasableBlockSizeInBytes = eraseable_sector_size;
    g_AWEFlashFSInstance.flashStartOffsetInBytes = FILE_SYSTEM_START_OFFSET;
    g_AWEFlashFSInstance.flashEraseTimeInMs = (INT32)((FLOAT32)((( (flash_memory_size_in_bytes - FILE_SYSTEM_START_OFFSET)/ eraseable_sector_size)*SECTOR_ERASE_TIME_MS/1000) + 0.5f) + 5);

    awe_initFlashFS((AWEInstance *)&g_AWEInstance, (AWEFlashFSInstance *)&g_AWEFlashFSInstance);
    
    g_AWEInstance.pFlashFileSystem = (AWEFlashFSInstance *)&g_AWEFlashFSInstance;
#endif

    int ret;
    ret = awe_initPin((IOPinDescriptor *)&g_InputPin,  AWE_INPUT_CHANNELS, NULL);
    assert(ret == 0);
    ret = awe_initPin((IOPinDescriptor *)&g_OutputPin, AWE_OUTPUT_CHANNELS, NULL);
    assert(ret == 0);
    ret = awe_init((AWEInstance*)&g_AWEInstance);
    assert(ret == 0);

    g_AWE_IsInitialised = 1;
}

/** Function that returns the number of elapsed "cycles". This has to be a
 * frequency between 10 and 20 MHz
 */
unsigned aweuser_getCycleCount() {
    unsigned int t;
    asm("gettime %0" : "=r" (t));
    t >>= 3;
    return t;
}

/** Function that calculates the difference between two "cycle" counts.
 * Note that as the cycle count has zeroes in the three top bits we need to
 * mask those for the subtraction to work when the timer wraps.
 */
extern unsigned aweuser_getElapsedCycles(UINT32 start, UINT32 end) {
    return (end - start) & 0x1fffffff;
}
