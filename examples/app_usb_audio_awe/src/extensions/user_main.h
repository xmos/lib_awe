// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef USER_MAIN_H
#define USER_MAIN_H

#ifdef __XC__

#include <print.h>
#include <xs1.h>
#include <platform.h>
#include "i2c.h"
#if AWE_USE_FLASH_FILE_SYSTEM
#include "awe_ffs_rpc.h"

/* FFS vars */
extern unsafe chanend g_ffs_rpc_client;
#endif /* AWE_USE_FLASH_FILE_SYSTEM */

/* Board hardware setup */
extern unsafe client interface i2c_master_if i_i2c_client;
extern void board_setup();

/* I2C interface ports */
extern port p_scl;
extern port p_sda;

/* AWE components and tuning */
extern void dsp_main(chanend control_from_host, chanend control_to_host);
extern void awe_usb_hid(chanend c_hid_to_host, chanend c_hid_from_host);
extern void awe_standalone_tuning(void);
extern void init_awe_tuning_instance(chanend c_tuning_from_host, chanend c_tuning_to_host);

/* Macros which will depend on whether AWE_USE_FLASH_FILE_SYSTEM is used or not */
#if AWE_USE_FLASH_FILE_SYSTEM
#define CHAN_FOR_FFS    chan c_ffs_rpc;
#define INIT_FOR_FFS    init_ffs_rpc_client_chanend(c_ffs_rpc);
#define FFS_SERVER_TASK ffs_server(c_ffs_rpc);
#else
#define CHAN_FOR_FFS    /* Define as nothing */
#define INIT_FOR_FFS
#define FFS_SERVER_TASK
#endif

/* Declarations that will be inserted in main.xc from lib_xua */
#define USER_MAIN_DECLARATIONS                                          \
    interface i2c_master_if i2c[1];                                     \
    chan c_tuning_from_host, c_tuning_to_host;                          \
    CHAN_FOR_FFS


/* Macros which will depend on how tuning is provided to AWE */
#if STANDALONE_TUNING
#define TUNING_TASK     awe_standalone_tuning();
#else
#define TUNING_TASK     awe_usb_hid(c_xud_in[ENDPOINT_NUMBER_IN_HID],   \
                                    c_xud_out[ENDPOINT_NUMBER_OUT_HID]);
#endif  /* STANDALONE_TUNING */



#define USER_MAIN_CORES                                                 \
    on tile[0]: {                                                       \
        board_setup();                                                  \
        init_awe_tuning_instance(c_tuning_from_host, c_tuning_to_host); \
        par {                                                           \
            i2c_master(i2c, 1, p_scl, p_sda, 100);                      \
            FFS_SERVER_TASK                                             \
            TUNING_TASK                                                 \
        }                                                               \
    }                                                                   \
    on tile[1]: {                                                       \
        unsafe                                                          \
        {                                                               \
            i_i2c_client = i2c[0];                                      \
            INIT_FOR_FFS                                                \
        }                                                               \
        dsp_main(c_tuning_from_host, c_tuning_to_host);                 \
    }
#endif // __XC__

#endif // USER_MAIN_H
