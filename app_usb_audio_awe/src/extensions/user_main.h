#ifndef USER_MAIN_H
#define USER_MAIN_H

#ifdef __XC__

#include "i2c.h"
#include <print.h>
#include <xs1.h>
#include <platform.h>

extern unsafe client interface i2c_master_if i_i2c_client;
extern void interface_saver(client interface i2c_master_if i);
extern void ctrlPort();
extern void awe_usb_hid(chanend c_hid_to_host, chanend c_hid_from_host, chanend c_tuning_from_host, chanend c_tuning_to_host);

/* I2C interface ports */
extern port p_scl;
extern port p_sda;

extern void dsp_main(chanend tuning_from_host, chanend tuning_to_host);

#define USER_MAIN_DECLARATIONS \
    interface i2c_master_if i2c[1]; \
    chan c_hid_tuning_from_host, c_hid_tuning_to_host;

#define USER_MAIN_CORES \
    on tile[0]: {                                                       \
        ctrlPort();                                                     \
        i2c_master(i2c, 1, p_scl, p_sda, 100);                          \
    }                                                                   \
    on tile[0]: {      /* create HID handler for AWE tuning */          \
        awe_usb_hid(c_xud_in[ENDPOINT_NUMBER_IN_HID],                   \
                    c_xud_out[ENDPOINT_NUMBER_OUT_HID],                 \
                    c_hid_tuning_from_host,                             \
                    c_hid_tuning_to_host);                              \
    }                                                                   \
    on tile[1]: {      /* create DSP process for AWE processing */      \
        unsafe                                                          \
        {                                                               \
            i_i2c_client = i2c[0];      /* TODO: delete, audiohw on tile 1 */              \
        }                                                               \
        dsp_main(c_hid_tuning_from_host, c_hid_tuning_to_host);         \
    }
#endif                  /* End of DSP process */

#define HID_OUT_REQUIRED   1

#endif
