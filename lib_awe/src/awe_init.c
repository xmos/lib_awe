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

    int ret;
    ret = awe_initPin((IOPinDescriptor *)&g_InputPin,  AWE_INPUT_CHANNELS, NULL);
    assert(ret == 0);
    ret = awe_initPin((IOPinDescriptor *)&g_OutputPin, AWE_OUTPUT_CHANNELS, NULL);
    assert(ret == 0);
    ret = awe_init((AWEInstance*)&g_AWEInstance);
    assert(ret == 0);
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
