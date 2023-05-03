#include <xcore/channel.h>
#include <xcore/hwtimer.h>
#include <stdio.h>
#include "assert.h"
#include "awe_xcore_internal.h"

// TODO: remove printf
// TODO: delete the reply buffer and share it with input buffer

AWEInstance2 g_AWEInstance = {0};
static IOPinDescriptor2 g_InputPin;
static IOPinDescriptor2 g_OutputPin;

extern const void* g_module_descriptor_table[];
extern const void* g_module_descriptor_table_end[];

static UINT32 g_FastHeapA[FAST_HEAP_A_SIZE];
static UINT32 g_FastHeapB[FAST_HEAP_B_SIZE];
static UINT32 g_SlowHeap[SLOW_HEAP_SIZE];

UINT32 AWE_Packet_Buffer[AWE_HID_PACKET_BUFFER_SIZE];
UINT32 AWE_Packet_Buffer_Reply[AWE_HID_PACKET_BUFFER_SIZE];

// TODO: remove these

UINT32 pCommands[] = {
        0x0002000c, 0x00060006, 0x473b8000, 0x40008002, 0x00000020, 0x00000000, 0x00060006, 0x473b8000,
        0x40008002, 0x00000020, 0x00000000, 0x00060006, 0x473b8000, 0x40008002, 0x00000020, 0x00000000,
        0x00060006, 0x473b8000, 0x40008001, 0x00000020, 0x00000000, 0x00060006, 0x473b8000, 0x40008001,
        0x00000020, 0x00000000, 0x00060006, 0x473b8000, 0x40008001, 0x00000020, 0x00000000, 0x00040007,
        0x00000001, 0x00000001, 0x00040007, 0x00000002, 0x00000002, 0x0009000f, 0xbeef0866, 0x00000101,
        0x00000002, 0x00000001, 0x00000003, 0x00000001, 0x00000000, 0x0008000f, 0xbeef0861, 0x00000201,
        0x00000000, 0x00000003, 0x00000004, 0x00000005, 0x000d000f, 0xbeef0813, 0x00000101, 0x00000006,
        0x00000005, 0x00000005, 0x00000000, 0x00000000, 0x00000001, 0x3f800000, 0x3f800000, 0x3f800000,
        0x000c000f, 0xbeef086b, 0x00010001, 0x00000005, 0x00000005, 0x00000006, 0x00000012, 0x42820000,
        0x42820000, 0x3cbf5380, 0x3cbf5380, 0x000d000f, 0xbeef0813, 0x00000101, 0x00000006, 0x00000004,
        0x00000004, 0x00000000, 0x00000000, 0x00000001, 0x3f800000, 0x3f800000, 0x3f800000, 0x0008000f,
        0xbeef0862, 0x00000102, 0x00000000, 0x00000004, 0x00000005, 0x00000003, 0x0009000f, 0xbeef0866,
        0x00000101, 0x00000002, 0x00000003, 0x00000002, 0x00000000, 0x00000001, 0x000c000f, 0xbeef086b,
        0x00010001, 0x00000005, 0x00000004, 0x00000005, 0x00000012, 0x42820000, 0x42820000, 0x3cbf5380,
        0x3cbf5380, 0x00040010, 0x00000008, 0x00000001, 0x000c002f, 0x0000000f, 0x00000000, 0x00000007,
        0x00000008, 0x00000009, 0x0000000a, 0x0000000b, 0x0000000c, 0x0000000d, 0x0000000e, 0x00040056,
        0x00000009, 0x00007531, 0x00040056, 0x0000000a, 0x00007533, 0x00040056, 0x0000000b, 0x00007530,
        0x00040056, 0x0000000e, 0x00007532, 0x0002001b, 0x00000000,
};

void awe_xcore_init() {
    g_AWEInstance.instanceId = 0;
    g_AWEInstance.pInputPin = (IOPinDescriptor *)&g_InputPin;
    g_AWEInstance.pOutputPin = (IOPinDescriptor *)&g_OutputPin;
    g_AWEInstance.pPacketBuffer = AWE_Packet_Buffer;
    g_AWEInstance.pReplyBuffer = AWE_Packet_Buffer_Reply;
    g_AWEInstance.packetBufferSize = AWE_HID_PACKET_BUFFER_SIZE;
    g_AWEInstance.pModuleDescriptorTable = g_module_descriptor_table;
    g_AWEInstance.numModules = g_module_descriptor_table_end - g_module_descriptor_table;
    g_AWEInstance.numThreads = AWE_DSP_THREAD_NUM;
    g_AWEInstance.sampleRate = 48000.0f;
    
    g_AWEInstance.fundamentalBlockSize = AWE_BLOCK_SIZE;
    g_AWEInstance.pFlashFileSystem = 0;
    
    g_AWEInstance.fastHeapASize = FAST_HEAP_A_SIZE;
    g_AWEInstance.fastHeapBSize = FAST_HEAP_B_SIZE;
    g_AWEInstance.slowHeapSize  = SLOW_HEAP_SIZE;
    g_AWEInstance.pFastHeapA    = g_FastHeapA;
    g_AWEInstance.pFastHeapB    = g_FastHeapB;
    g_AWEInstance.pSlowHeap     = g_SlowHeap;
    
    g_AWEInstance.coreSpeed     = 100e6f;
    g_AWEInstance.profileSpeed  = 12.5e6f;
    
    g_AWEInstance.pName = "XMOS_USB_Audio";
    g_AWEInstance.userVersion = 0x20230406;

    int ret;
    ret = awe_initPin((IOPinDescriptor *)&g_InputPin,  AWE_INPUT_CHANNELS, NULL);
    assert(ret == 0);
    ret = awe_initPin((IOPinDescriptor *)&g_OutputPin, AWE_OUTPUT_CHANNELS, NULL);
    assert(ret == 0);
    ret = awe_init((AWEInstance*)&g_AWEInstance);
    assert(ret == 0);
    UINT32 nErrOffset;
    INT32 err = awe_loadAWBfromArray ((AWEInstance*)&g_AWEInstance,
                                      pCommands, sizeof(pCommands) / sizeof(pCommands[0]), &nErrOffset );
    if (err)
    {
        // report the error
        printf("error code %d due to command at position %u had %d modules\n", err, nErrOffset, g_AWEInstance.numModules);
        // handle the error
    }
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
