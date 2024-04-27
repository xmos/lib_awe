#include <xcore/channel.h>
#include <xcore/port.h>
#include "awe_xcore.h"

static chanend_t g_c_to_dspc;

void UserBufferManagementInit()
{
}

void UserBufferManagement(unsigned sampsFromUsbToAudio[], unsigned sampsFromAudioToUsb[])
{
    awe_offload_data_to_dsp_engine(g_c_to_dspc, sampsFromUsbToAudio, sampsFromAudioToUsb);
}

#ifdef AWE_I2C_CONTROL
port_t    p_client_scl = XS1_PORT_4E;
port_t    p_client_sda = XS1_PORT_4F;
#endif

void dsp_main(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host) {
    channel_t c_data = chan_alloc();
    g_c_to_dspc = c_data.end_a;
    
#ifdef AWE_I2C_CONTROL
    port_enable(p_client_scl);
    port_enable(p_client_sda);
#endif
    
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data.end_b
#ifdef AWE_I2C_CONTROL
                   , p_client_scl
                   , p_client_sda
                   , 0x04
#endif
        );
}
