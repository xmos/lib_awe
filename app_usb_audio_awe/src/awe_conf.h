#ifndef _awe_conf_h_
#define _awe_conf_h_

#define AWE_I2C_CONTROL

#define AWE_DSP_THREAD_NUM           4
#define AWE_INPUT_CHANNELS           2
#define AWE_OUTPUT_CHANNELS          2
#define AUDIO_INPUT_CHANNELS         4
#define AUDIO_OUTPUT_CHANNELS        4

#define AWE_HEAP_SIZE         (350*1024/4)

#define AWE_TUNING_OVER_USB_HID      1

#define AWE_MODULE_LIST \
		awe_modAGCLimiterCoreClass, \
		awe_modAdderClass, \
		awe_modDeinterleaveClass, \
		awe_modDelayClass, \
		awe_modDelayMsecClass, \
		awe_modMaxAbsClass, \
		awe_modMeterClass, \
		awe_modMultiplierV2Class, \
		awe_modRouterClass, \
		awe_modScalerV2Class, \
		awe_modSecondOrderFilterSmoothedClass, \
		awe_modSecondOrderFilterSmoothedCascadeClass, \
		awe_modSoftClipClass, \
		awe_modWetDryClass, \
		awe_modZeroPaddingClass, \
		awe_modComplexModulateClass, \
		awe_modCfftClass, \
		awe_modSbComplexFIRClass, \
		awe_modComplexToRealImagClass, \
		awe_modBlockExtractClass, \
		awe_modBlockConcatenateClass, \
		awe_modOverlapAddClass, \
		awe_modFifoInClass, \
		awe_modFifoOutClass, \
		awe_modTypeConversionClass

#endif
