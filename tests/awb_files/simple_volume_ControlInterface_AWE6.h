// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
/****************************************************************************
 *
 *		Target Tuning Symbol File
 *		-------------------------
 *
 * 		This file is populated with symbol information only for modules
 *		that have an object ID of 30000 or greater assigned.
 *
 *          Generated on:  12-Jun-2024 12:13:59
 *
 ***************************************************************************/

#ifndef AWE_SIMPLE_VOLUME_CONTROLINTERFACE_AWE6_H
#define AWE_SIMPLE_VOLUME_CONTROLINTERFACE_AWE6_H

// ----------------------------------------------------------------------
// Volume1 [VolumeControl]
// Volume control with Fletcher-Munson loudness compensation

#define AWE_Volume1_ID 30000

// int profileTime - 24.8 fixed point filtered execution time. Must be pumped 1000 times to get to within .1% accuracy
#define AWE_Volume1_profileTime_OFFSET 7
#define AWE_Volume1_profileTime_MASK 0x00000080
#define AWE_Volume1_profileTime_SIZE -1

// float gain - Gain through the volume control, in dB.
// Default value: 0
// Range: -90 to 0.  Step size = 0.1
#define AWE_Volume1_gain_OFFSET 8
#define AWE_Volume1_gain_MASK 0x00000100
#define AWE_Volume1_gain_SIZE -1

// float centerFreq - Center frequency of the low frequency resonance, 
//         in Hz.
#define AWE_Volume1_centerFreq_OFFSET 9
#define AWE_Volume1_centerFreq_MASK 0x00000200
#define AWE_Volume1_centerFreq_SIZE -1

// float c2 - X^2 coefficient.
#define AWE_Volume1_c2_OFFSET 10
#define AWE_Volume1_c2_MASK 0x00000400
#define AWE_Volume1_c2_SIZE -1

// float c1 - X^1 coefficient.
#define AWE_Volume1_c1_OFFSET 11
#define AWE_Volume1_c1_MASK 0x00000800
#define AWE_Volume1_c1_SIZE -1

// float c0 - X^0 coefficient.
#define AWE_Volume1_c0_OFFSET 12
#define AWE_Volume1_c0_MASK 0x00001000
#define AWE_Volume1_c0_SIZE -1

// float b0_LPF - First numerator coefficient.
#define AWE_Volume1_b0_LPF_OFFSET 13
#define AWE_Volume1_b0_LPF_MASK 0x00002000
#define AWE_Volume1_b0_LPF_SIZE -1

// float b1_LPF - Second numerator coefficient.
#define AWE_Volume1_b1_LPF_OFFSET 14
#define AWE_Volume1_b1_LPF_MASK 0x00004000
#define AWE_Volume1_b1_LPF_SIZE -1

// float b2_LPF - Third numerator coefficient.
#define AWE_Volume1_b2_LPF_OFFSET 15
#define AWE_Volume1_b2_LPF_MASK 0x00008000
#define AWE_Volume1_b2_LPF_SIZE -1



#endif // AWE_SIMPLE_VOLUME_CONTROLINTERFACE_AWE6_H

