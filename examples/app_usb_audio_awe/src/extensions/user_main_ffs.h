// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef USER_MAIN_H
#define USER_MAIN_H

#ifdef __XC__

#include "i2c.h"
#include <print.h>
#include <xs1.h>
#include <platform.h>
#include "awe_ffs_rpc.h"

/* Board hardware setup */
extern unsafe client interface i2c_master_if i_i2c_client;
extern void interface_saver(client interface i2c_master_if i);
extern void board_setup();

/* I2C interface ports */
extern port p_scl;
extern port p_sda;

/* FFS vars */
extern unsafe chanend g_ffs_rpc_client;

/* AWE components and tuning */
extern void dsp_main(chanend control_from_host, chanend control_to_host);
extern void awe_usb_hid(chanend c_hid_to_host, chanend c_hid_from_host, chanend c_tuning_from_host, chanend c_tuning_to_host);

#define USER_MAIN_DECLARATIONS \
    interface i2c_master_if i2c[1]; \
    chan c_hid_control_from_host, c_hid_control_to_host; \
    chan c_ffs_rpc;


#define USER_MAIN_CORES \
    on tile[0]: {                                                       \
        board_setup();                                                  \
        i2c_master(i2c, 1, p_scl, p_sda, 100);                          \
    }                                                                   \
    on tile[0]: {                                                       \
        ffs_server(c_ffs_rpc);                                          \
    }                                                                   \
    on tile[0]: {                                                       \
        awe_usb_hid(c_xud_in[ENDPOINT_NUMBER_IN_HID],                   \
                    c_xud_out[ENDPOINT_NUMBER_OUT_HID],                 \
                    c_hid_control_from_host,                            \
                    c_hid_control_to_host);                             \
    }                                                                   \
    on tile[1]: {                                                       \
        unsafe                                                          \
        {                                                               \
            i_i2c_client = i2c[0];                                      \
            init_ffs_rpc_client_chanend(c_ffs_rpc);                     \
        }                                                               \
        dsp_main(c_hid_control_from_host, c_hid_control_to_host);       \
    }
#endif

#endif
