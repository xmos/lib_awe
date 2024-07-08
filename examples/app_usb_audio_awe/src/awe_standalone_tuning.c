// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include <xcore/port.h>
#include <xcore/hwtimer.h>
#include <print.h>
#include "awe_xcore.h"
#include "Errors.h"

// Rename the AWB arrays (which are the same name from AWD) so we can have more than one array referenced in this file
// We do the same in the CMAKE file so the compiled objects get renamed too
#define Core0_InitCommands  Core0_InitCommands_Basic_3thread
#define Core0_InitCommands_Len  Core0_InitCommands_Basic_3thread_Len
#include "playBasic_3thread_InitAWB.h"
#undef Core0_InitCommands
#undef Core0_InitCommands_Len
#define Core0_InitCommands  Core0_InitCommands_simple_volume
#define Core0_InitCommands_Len  Core0_InitCommands_simple_volume_Len
#include "simple_volume_InitAWB.h"
#include "simple_volume_ControlInterface.h"
#undef Core0_InitCommands
#undef Core0_InitCommands_Len


port_t p_buttons = PORT_BUTTONS;

typedef enum awb_state_t{
    AWB_NONE,
    AWB_BASIC_3THREAD,
    AWB_SIMPLE_VOLUME
}awb_state_t;

char *awb_filenames[] = {"", "playBasic_3thread.awb", "simple_volume.awb"};

void delay_ms(unsigned delay_ms){
    hwtimer_t tmr = hwtimer_alloc();
    hwtimer_delay(tmr, XS1_TIMER_KHZ * delay_ms);
    hwtimer_free(tmr);
}

int wait_for_button_change(port_t p_buttons){
    int current_val = port_in(p_buttons);
    port_set_trigger_in_not_equal(p_buttons, current_val);
    delay_ms(10); // Debounce
    current_val = port_in(p_buttons);
    port_clear_trigger_in(p_buttons);

    return current_val;
}

void awe_standalone_tuning(chanend_t control_from_host, chanend_t control_to_host){
    xAWEInstance_t pAWE = {control_from_host, control_to_host};

    // AWB image state
    awb_state_t awb_state_old = AWB_NONE;
    awb_state_t awb_state = AWB_NONE;
    unsigned int *awb_array = NULL;
    unsigned int awb_array_len = 0;

    // Setup GPIO
    port_enable(p_buttons);
    delay_ms(10); // Allow port to settle after boot
    const int buttons_not_pressed = 0x7;

    // Control for AWB_SIMPLE_VOLUME
    float volume = 0.0;

    while(1){
        int buttons = wait_for_button_change(p_buttons);
        // printintln(buttons);

        // See if button pressed and take action if so
        if(buttons != buttons_not_pressed){
            switch(buttons){
                case 0x03:
                    awb_state = AWB_BASIC_3THREAD;                                                                                                                                                                                                          
                break;
                case 0x05:
                    awb_state = AWB_SIMPLE_VOLUME;
                break;
                case 0x06:
                    // Only do for AWB_SIMPLE_VOLUME because it has the vol control
                    if(awb_state == AWB_SIMPLE_VOLUME){
                        volume -= 10.0;
                        if(volume < -50.0){
                            volume = 0.0;
                        }
                        int err = xawe_ctrlSetValue(&pAWE, AWE_Volume1_gain_HANDLE, &volume, 0, 1);
                        if(err != E_SUCCESS){
                            printstrln("ERROR - setting volume failed");
                            printintln(err);
                        }
                    }
                break;
                default:
                    // Do nothing
                break;
            }

            // See if we need to load AWBs
            if(awb_state != awb_state_old){
                switch(awb_state){
                case AWB_BASIC_3THREAD:
                    awb_array = (unsigned int *)Core0_InitCommands_Basic_3thread;
                    awb_array_len = Core0_InitCommands_Basic_3thread_Len;
                    // printstrln("AWB_BASIC_3THREAD");
                break;
                case AWB_SIMPLE_VOLUME:
                    awb_array = (unsigned int *)Core0_InitCommands_simple_volume;
                    awb_array_len = Core0_InitCommands_simple_volume_Len;
                    // printstrln("AWB_SIMPLE_VOLUME");
                break;
                case AWB_NONE:
                    printstrln("ERROR - we should not be here");
                break;

                }
                // Load the AWB
#if AWE_USE_FLASH_FILE_SYSTEM
                char *awb_filename = awb_filenames[awb_state];
                int err = xawe_loadAWBfromFFS(&pAWE, awb_filename);
                if(err != E_SUCCESS){
                    printstrln("ERROR - xawe_loadAWBfromFFS failed");
                    printintln(err);
                }
#else
                unsigned int pPos = 0;
                int err = xawe_loadAWBfromArray(&pAWE, awb_array, awb_array_len, &pPos);
                if(err != E_SUCCESS){
                    printstrln("ERROR - xawe_loadAWBfromArray failed");
                    printintln(err);
                    printintln(pPos);
                }
#endif
                awb_state_old = awb_state;
            }
        }
    }
}
