// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef _AWE_CONF_H_
#define _AWE_CONF_H_

#include "xua.h"

#define AWE_DSP_THREAD_NUM           (3)

#ifdef I2S_ONLY
#define AWE_INPUT_CHANNELS           (I2S_CHANS_ADC)
#define AWE_OUTPUT_CHANNELS          (I2S_CHANS_DAC)
#else
#define AWE_INPUT_CHANNELS           (NUM_USB_CHAN_OUT)
#define AWE_OUTPUT_CHANNELS          (NUM_USB_CHAN_IN)
#endif

#if !USE_AWE_FLASH_FILE_SYSTEM
#define AWE_HEAP_SIZE                (44*1024)
#else
#define AWE_HEAP_SIZE                (42*1024)
#endif


#endif
