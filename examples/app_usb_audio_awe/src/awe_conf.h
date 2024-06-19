// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef _awe_conf_h_
#define _awe_conf_h_

#define AWE_DSP_THREAD_NUM           3
#define AWE_INPUT_CHANNELS           2
#define AWE_OUTPUT_CHANNELS          2
#define AUDIO_INPUT_CHANNELS        16
#define AUDIO_OUTPUT_CHANNELS       16

#define AWE_HEAP_SIZE         (44*1024)

#if !STANDALONE
#define AWE_TUNING_OVER_USB_HID      1
#endif

#endif
