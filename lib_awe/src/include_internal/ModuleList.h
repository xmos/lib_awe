/*******************************************************************************
*
*               Audio Framework
*               ---------------
*
********************************************************************************
*     ModuleList.h
********************************************************************************
*
* -----------------------------------------------------------------------------
* Specifies the audio modules that will be linked into the target application
* -----------------------------------------------------------------------------
*
*     Copyright:    DSP Concepts, Inc., 2007 - 2020
*                   3235 Kifer Road, Suite 100
*                   Santa Clara, CA 95051
*
*******************************************************************************/


#ifndef _MODULELIST_H
#define _MODULELIST_H

#ifdef	__cplusplus
extern "C" {
#endif

// Standard Modules
extern const int awe_modAGCCoreClass;
extern const int awe_modAGCLimiterCoreClass;
extern const int awe_modAGCLimiterCoreFract32Class;
extern const int awe_modAGCLimiterCoreV2Class;
extern const int awe_modAGCLimiterCoreV2Fract32Class;
extern const int awe_modAGCMultiplierFract32Class;
extern const int awe_modAGCNoiseGateCoreClass;
extern const int awe_modAbsClass;
extern const int awe_modAbsFract32Class;
extern const int awe_modAcosClass;
extern const int awe_modAdderClass;
extern const int awe_modAdderFract32Class;
extern const int awe_modAdderInt32Class;
extern const int awe_modAsinClass;
extern const int awe_modAtanClass;
extern const int awe_modAtan2Class;
extern const int awe_modBalanceClass;
extern const int awe_modBalanceFract32Class;
extern const int awe_modBiquadClass;
extern const int awe_modBiquadCascadeClass;
extern const int awe_modBiquadCascadeFract32Class;
extern const int awe_modBiquadFract32Class;
extern const int awe_modBiquadNCascadeClass;
extern const int awe_modBiquadSmoothedClass;
extern const int awe_modBiquadSmoothedFract32Class;
extern const int awe_modBiquadV2Fract32Class;
extern const int awe_modBitsToIntClass;
extern const int awe_modBlockCounterClass;
extern const int awe_modBooleanInvertClass;
extern const int awe_modBooleanSinkClass;
extern const int awe_modBooleanSourceClass;
extern const int awe_modButterworthFilterClass;
extern const int awe_modButterworthFilterFract32Class;
extern const int awe_modCeilClass;
extern const int awe_modClipAsymClass;
extern const int awe_modClipAsymFract32Class;
extern const int awe_modClipAsymInt32Class;
extern const int awe_modClipIndicatorClass;
extern const int awe_modClipIndicatorFract32Class;
extern const int awe_modCopierClass;
extern const int awe_modCosClass;
extern const int awe_modCoshClass;
extern const int awe_modCounterClass;
extern const int awe_modCrossFaderClass;
extern const int awe_modDCSourceIntClass;
extern const int awe_modDCSourceV2Class;
extern const int awe_modDCSourceV2Fract32Class;
extern const int awe_modDb10Class;
extern const int awe_modDb10ApproxClass;
extern const int awe_modDb10Fract32Class;
extern const int awe_modDb20Class;
extern const int awe_modDb20ApproxClass;
extern const int awe_modDb20Fract32Class;
extern const int awe_modDcBlockClass;
extern const int awe_modDebounceClass;
extern const int awe_modDeinterleaveClass;
extern const int awe_modDelayClass;
extern const int awe_modDelayMsecClass;
extern const int awe_modDivideClass;
extern const int awe_modExpClass;
extern const int awe_modFIRClass;
extern const int awe_modFIRFract32Class;
extern const int awe_modFIRSmoothedClass;
extern const int awe_modFloorClass;
extern const int awe_modFmodClass;
extern const int awe_modFrexpClass;
extern const int awe_modGPIOClass;
extern const int awe_modGraphicEQBandClass;
extern const int awe_modGraphicEQBandFract32Class;
extern const int awe_modImpulseMsecSourceClass;
extern const int awe_modImpulseMsecSourceFract32Class;
extern const int awe_modImpulseSourceFract32Class;
extern const int awe_modIntToBitsClass;
extern const int awe_modInterleaveClass;
extern const int awe_modInvertClass;
extern const int awe_modInvertFract32Class;
extern const int awe_modLShiftFract32Class;
extern const int awe_modLdexpClass;
extern const int awe_modLinkwitzRileyFilterClass;
extern const int awe_modLogClass;
extern const int awe_modLog10Class;
extern const int awe_modLog10Fract32Class;
extern const int awe_modLog2Class;
extern const int awe_modLog2Fract32Class;
extern const int awe_modLogicAllClass;
extern const int awe_modLogicAndConstFract32Class;
extern const int awe_modLogicAndConstInt32Class;
extern const int awe_modLogicAnyClass;
extern const int awe_modLogicBinaryOpClass;
extern const int awe_modLogicCompareClass;
extern const int awe_modLogicCompareConstClass;
extern const int awe_modLogicCompareConstFract32Class;
extern const int awe_modLogicCompareConstInt32Class;
extern const int awe_modLogicCompareConstReplaceClass;
extern const int awe_modLogicCompareConstReplaceInt32Class;
extern const int awe_modLogicCompareFract32Class;
extern const int awe_modLogicCompareInt32Class;
extern const int awe_modMaxAbsClass;
extern const int awe_modMaxAbsFract32Class;
extern const int awe_modMeterClass;
extern const int awe_modMeterFract32Class;
extern const int awe_modMixerDenseFract32Class;
extern const int awe_modMixerFract32Class;
extern const int awe_modMixerSmoothedFract32Class;
extern const int awe_modMixerV3Class;
extern const int awe_modMixerV3Fract32Class;
extern const int awe_modModfClass;
extern const int awe_modMultiplexorClass;
extern const int awe_modMultiplexorV2Class;
extern const int awe_modMultiplexorV2Fract32Class;
extern const int awe_modMultiplierFract32Class;
extern const int awe_modMultiplierV2Class;
extern const int awe_modMultiplierV2Fract32Class;
extern const int awe_modMuteNSmoothedClass;
extern const int awe_modMuteNSmoothedFract32Class;
extern const int awe_modMuteSmoothedClass;
extern const int awe_modMuteSmoothedFract32Class;
extern const int awe_modMuteUnmuteClass;
extern const int awe_modMuteUnmuteFract32Class;
extern const int awe_modNullSinkClass;
extern const int awe_modNullSinkV2Class;
extern const int awe_modNullSourceClass;
extern const int awe_modParamGetClass;
extern const int awe_modParamSetClass;
extern const int awe_modPeriodicFunctionGenClass;
extern const int awe_modPeriodicSourceClass;
extern const int awe_modPeriodicSourceFract32Class;
extern const int awe_modPolynomialClass;
extern const int awe_modPolynomialFract32Class;
extern const int awe_modPowClass;
extern const int awe_modPow10toXClass;
extern const int awe_modPow2Fract32Class;
extern const int awe_modPowFract32Class;
extern const int awe_modPulseGenClass;
extern const int awe_modPulseGenFract32Class;
extern const int awe_modRMSClass;
extern const int awe_modRMSFract32Class;
extern const int awe_modRMSNClass;
extern const int awe_modRandClass;
extern const int awe_modReciprocalClass;
extern const int awe_modReciprocalFract32Class;
extern const int awe_modRemainderClass;
extern const int awe_modRoundClass;
extern const int awe_modRouterClass;
extern const int awe_modRouterSmoothedClass;
extern const int awe_modRouterSmoothedFract32Class;
extern const int awe_modSMixer2x1Class;
extern const int awe_modSMixer2x1Fract32Class;
extern const int awe_modSOFSmoothedCascadeFract32Class;
extern const int awe_modSawtoothFract32Class;
extern const int awe_modScaleOffsetClass;
extern const int awe_modScaleOffsetFract32Class;
extern const int awe_modScaleOffsetInt32Class;
extern const int awe_modScalerControlClass;
extern const int awe_modScalerControlBlockSmoothedFract32Class;
extern const int awe_modScalerControlSmoothedFract32Class;
extern const int awe_modScalerDBControlClass;
extern const int awe_modScalerDBFract32Class;
extern const int awe_modScalerDBSmoothedFract32Class;
extern const int awe_modScalerFract32Class;
extern const int awe_modScalerNControlClass;
extern const int awe_modScalerNDBSmoothedFract32Class;
extern const int awe_modScalerNFract32Class;
extern const int awe_modScalerNSmoothedFract32Class;
extern const int awe_modScalerNSymmetricFadingClass;
extern const int awe_modScalerNV2Class;
extern const int awe_modScalerSmoothedFract32Class;
extern const int awe_modScalerSymmetricFadingClass;
extern const int awe_modScalerSymmetricFadingV2Class;
extern const int awe_modScalerV2Class;
extern const int awe_modSecondOrderFilterSmoothedClass;
extern const int awe_modSecondOrderFilterSmoothedCascadeClass;
extern const int awe_modSecondOrderFilterSmoothedCascadeV2Class;
extern const int awe_modSecondOrderFilterSmoothedFract32Class;
extern const int awe_modShiftFract32Class;
extern const int awe_modSignClass;
extern const int awe_modSinClass;
extern const int awe_modSineGenControlFract32Class;
extern const int awe_modSineGenFract32Class;
extern const int awe_modSineSmoothedGenClass;
extern const int awe_modSineSmoothedGenFract32Class;
extern const int awe_modSinhClass;
extern const int awe_modSinkClass;
extern const int awe_modSinkFract32Class;
extern const int awe_modSinkFract32PropertiesClass;
extern const int awe_modSinkIntClass;
extern const int awe_modSinkIntPropertiesClass;
extern const int awe_modSinkPropertiesClass;
extern const int awe_modSofV2HpFract32Class;
extern const int awe_modSoftClipClass;
extern const int awe_modSoftClipFract32Class;
extern const int awe_modSoloMuteClass;
extern const int awe_modSoloMuteFract32Class;
extern const int awe_modSourceClass;
extern const int awe_modSourceFract32Class;
extern const int awe_modSourceFract32PropertiesClass;
extern const int awe_modSourceIntClass;
extern const int awe_modSourceIntPropertiesClass;
extern const int awe_modSourcePropertiesClass;
extern const int awe_modSqrtClass;
extern const int awe_modSqrtFract32Class;
extern const int awe_modSquareClass;
extern const int awe_modSquareAddClass;
extern const int awe_modSquareAddFract32Class;
extern const int awe_modSquareFract32Class;
extern const int awe_modSubtractClass;
extern const int awe_modSubtractFract32Class;
extern const int awe_modSubtractInt32Class;
extern const int awe_modSumDiffClass;
extern const int awe_modSumDiffFract32Class;
extern const int awe_modSumDiffInt32Class;
extern const int awe_modTableInterpClass;
extern const int awe_modTableInterpFract32Class;
extern const int awe_modTableLookupClass;
extern const int awe_modTableLookupV2Class;
extern const int awe_modTanClass;
extern const int awe_modTanhClass;
extern const int awe_modThreeBandToneControlClass;
extern const int awe_modThreeBandToneControlFract32Class;
extern const int awe_modToggleButtonClass;
extern const int awe_modTypeConversionClass;
extern const int awe_modUndb10Class;
extern const int awe_modUndb10ApproxClass;
extern const int awe_modUndb10Fract32Class;
extern const int awe_modUndb20Class;
extern const int awe_modUndb20ApproxClass;
extern const int awe_modUndb20Fract32Class;
extern const int awe_modVolumeControlClass;
extern const int awe_modVolumeControlFract32Class;
extern const int awe_modWetDryClass;
extern const int awe_modWhiteNoiseClass;
extern const int awe_modWhiteNoiseFract32Class;
extern const int awe_modWithinRangeClass;
extern const int awe_modWithinRangeFract32Class;
extern const int awe_modZeroCrossingDetectorClass;
extern const int awe_modZeroCrossingDetectorFract32Class;
extern const int awe_modZeroSourceClass;


// Advanced Modules
extern const int awe_modAGCAttackReleaseClass;
extern const int awe_modAGCAttackReleaseFract32Class;
extern const int awe_modAGCCompressorCoreClass;
extern const int awe_modAGCCompressorCoreFract32Class;
extern const int awe_modAGCCoreFract32Class;
extern const int awe_modAGCVariableAttackReleaseClass;
extern const int awe_modAllpassDelayClass;
extern const int awe_modAllpassDelay16Class;
extern const int awe_modAllpassDelay16Fract32Class;
extern const int awe_modAllpassDelayFract32Class;
extern const int awe_modAllpassDelayciClass;
extern const int awe_modAllpassDelayiClass;
extern const int awe_modArraysetClass;
extern const int awe_modAttackDecaySustainReleaseClass;
extern const int awe_modAudioWeightingClass;
extern const int awe_modAudioWeightingFract32Class;
extern const int awe_modAveragerClass;
extern const int awe_modAveragerFract32Class;
extern const int awe_modBandToBinClass;
extern const int awe_modBinToBandClass;
extern const int awe_modBiquadCascadeHPClass;
extern const int awe_modBiquadCascadeHPFract32Class;
extern const int awe_modBiquadLoadingClass;
extern const int awe_modBiquadLoadingFract32Class;
extern const int awe_modBiquadSmoothedHPClass;
extern const int awe_modBiquadSparseClass;
extern const int awe_modBiquadSparseHPClass;
extern const int awe_modBiquadSparseHPFract32Class;
extern const int awe_modBiquadSparseV2Class;
extern const int awe_modBiquadSparseV3Class;
extern const int awe_modBiquadSparseV4Class;
extern const int awe_modBiquadSparseV5Class;
extern const int awe_modBiquadSparseV6Class;
extern const int awe_modBlockConcatenateClass;
extern const int awe_modBlockDelayClass;
extern const int awe_modBlockDelayV2Class;
extern const int awe_modBlockExtractClass;
extern const int awe_modBlockFlipClass;
extern const int awe_modBlockMedianClass;
extern const int awe_modBlockStatisticsClass;
extern const int awe_modBlockStatisticsFract32Class;
extern const int awe_modBlockStatisticsNClass;
extern const int awe_modButterworthFilterHPClass;
extern const int awe_modCfftClass;
extern const int awe_modCfftFract32Class;
extern const int awe_modCoeffTableClass;
extern const int awe_modComplexAngleClass;
extern const int awe_modComplexConjugateClass;
extern const int awe_modComplexConjugateFract32Class;
extern const int awe_modComplexDivideClass;
extern const int awe_modComplexMagnitudeClass;
extern const int awe_modComplexMagnitudeFract32Class;
extern const int awe_modComplexMagnitudeSquaredClass;
extern const int awe_modComplexModulateClass;
extern const int awe_modComplexMultiplierClass;
extern const int awe_modComplexMultiplierFract32Class;
extern const int awe_modComplexMultiplierV2Class;
extern const int awe_modComplexToPolarClass;
extern const int awe_modComplexToRealImagClass;
extern const int awe_modComplexToRealImagFract32Class;
extern const int awe_modConsecutiveCountIntClass;
extern const int awe_modConvolveClass;
extern const int awe_modCorrelateClass;
extern const int awe_modCorrelationFract32Class;
extern const int awe_modCycleBurnerClass;
extern const int awe_modCycleBurnerSetClass;
extern const int awe_modDateTimeClass;
extern const int awe_modDeadBandClass;
extern const int awe_modDelay16Class;
extern const int awe_modDelayInterpClass;
extern const int awe_modDelayInterpFract32Class;
extern const int awe_modDelayMixerSmoothedClass;
extern const int awe_modDelayNChanClass;
extern const int awe_modDelayNChanMsecClass;
extern const int awe_modDelayNChanMsecSmoothedClass;
extern const int awe_modDelayNChanSmoothedClass;
extern const int awe_modDelayNChanV2Class;
extern const int awe_modDelayNTapClass;
extern const int awe_modDelayNTap16Fract32Class;
extern const int awe_modDelayReaderClass;
extern const int awe_modDelayStateWriterClass;
extern const int awe_modDelayStateWriter16Class;
extern const int awe_modDelayciFract32Class;
extern const int awe_modDerivativeClass;
extern const int awe_modDerivativeFract32Class;
extern const int awe_modDitherFract32Class;
extern const int awe_modDownsamplerClass;
extern const int awe_modDownwardExpanderCoreClass;
extern const int awe_modDownwardExpanderCoreFract32Class;
extern const int awe_modDuckerClass;
extern const int awe_modDuckerFract32Class;
extern const int awe_modEmphasisFilterClass;
extern const int awe_modEmphasisFilterFract32Class;
extern const int awe_modExpanderCoreClass;
extern const int awe_modFIRDecimatorClass;
extern const int awe_modFIRDecimatorFract32Class;
extern const int awe_modFIRInterpolatorClass;
extern const int awe_modFIRInterpolatorFract32Class;
extern const int awe_modFIRLoadingClass;
extern const int awe_modFIRLoadingFract32Class;
extern const int awe_modFIRNChanClass;
extern const int awe_modFIRReaderClass;
extern const int awe_modFIRSparseClass;
extern const int awe_modFIRSparseReaderClass;
extern const int awe_modFIRSparseReaderFract16Class;
extern const int awe_modFIRSparseReaderFract32Class;
extern const int awe_modFOFControlClass;
extern const int awe_modFOFControlFract32Class;
extern const int awe_modFftClass;
extern const int awe_modFftFract32Class;
extern const int awe_modFilterbankFIRNChanClass;
extern const int awe_modFilterbankFIRNChanV2Class;
extern const int awe_modFractionalDelayV2Class;
extern const int awe_modFractionalDelayV2Fract32Class;
extern const int awe_modGetWirePropertiesClass;
extern const int awe_modGraphicEQBandHPClass;
extern const int awe_modGraphicEQBandHpFract32Class;
extern const int awe_modHistogramClass;
extern const int awe_modHysteresisClass;
extern const int awe_modHysteresisFract32Class;
extern const int awe_modIfftClass;
extern const int awe_modIfftFract32Class;
extern const int awe_modIntegralClass;
extern const int awe_modInterleavedSinkClass;
extern const int awe_modInvertNClass;
extern const int awe_modLMSClass;
extern const int awe_modLMSNormFract32Class;
extern const int awe_modLPF1ControlClass;
extern const int awe_modMapperControlClass;
extern const int awe_modMathExceptionClass;
extern const int awe_modMatrixMultiplicationClass;
extern const int awe_modMeasurementClass;
extern const int awe_modMeasurementV2Class;
extern const int awe_modMemoryLoadingClass;
extern const int awe_modMultichannelMixerSmoothedClass;
extern const int awe_modMuteLinearFadeClass;
extern const int awe_modMuteSymmetricClass;
extern const int awe_modMuteSyncClass;
extern const int awe_modMuteSyncFract32Class;
extern const int awe_modOscillatorClass;
extern const int awe_modOverlapAddClass;
extern const int awe_modOverlapAddFract32Class;
extern const int awe_modOverwriteControlClass;
extern const int awe_modPDFClass;
extern const int awe_modParamSetTableV2Class;
extern const int awe_modPeakFinderClass;
extern const int awe_modPeriodicStreamingClass;
extern const int awe_modPolarToComplexClass;
extern const int awe_modPresetArrayHandlerClass;
extern const int awe_modPresetArrayHandlerV2Class;
extern const int awe_modProbeFract32Class;
extern const int awe_modRandiClass;
extern const int awe_modRandiFract32Class;
extern const int awe_modRealImagToComplexClass;
extern const int awe_modRealImagToComplexFract32Class;
extern const int awe_modRebufferClass;
extern const int awe_modRepWinOverlapClass;
extern const int awe_modRepWinOverlapFract32Class;
extern const int awe_modRunningMinMaxClass;
extern const int awe_modRunningMinMaxFract32Class;
extern const int awe_modRunningStatisticsClass;
extern const int awe_modRunningStatisticsFract32Class;
extern const int awe_modRunningStatisticsNClass;
extern const int awe_modSOFCascadeHPClass;
extern const int awe_modSOFCascadeHPFract32Class;
extern const int awe_modSOFControlClass;
extern const int awe_modSOFControlFract32Class;
extern const int awe_modSOFControlHPClass;
extern const int awe_modSOFControlV2Class;
extern const int awe_modSOFControlV3Class;
extern const int awe_modSampleAndHoldClass;
extern const int awe_modSampleAndHoldInt32Class;
extern const int awe_modSampleMultiplexorControlClass;
extern const int awe_modSampleRateClass;
extern const int awe_modSampleStatisticsClass;
extern const int awe_modSampleStatisticsFract32Class;
extern const int awe_modSbAttackReleaseClass;
extern const int awe_modSbComplexFIRClass;
extern const int awe_modSbComplexFIRvlClass;
extern const int awe_modSbDerivativeClass;
extern const int awe_modSbNLMSClass;
extern const int awe_modSbNLMSV2Class;
extern const int awe_modSbNLMSV3Class;
extern const int awe_modSbRMSClass;
extern const int awe_modSbSOFClass;
extern const int awe_modSbSmoothClass;
extern const int awe_modSbSmoothFract32Class;
extern const int awe_modSbSmoothV2Class;
extern const int awe_modSbSplitterClass;
extern const int awe_modSecondOrderFilterHPClass;
extern const int awe_modSetWirePropertiesClass;
extern const int awe_modShiftSamplesClass;
extern const int awe_modSlewRateLimiterClass;
extern const int awe_modStatusSetClass;
extern const int awe_modStatusSetV2Class;
extern const int awe_modSubblockStatisticsClass;
extern const int awe_modSubblockStatisticsFract32Class;
extern const int awe_modSweepGenClass;
extern const int awe_modSystemVariableClass;
extern const int awe_modTHDClass;
extern const int awe_modTableInterp2dClass;
extern const int awe_modTableLookupIntFloatClass;
extern const int awe_modTableLookupIntIntClass;
extern const int awe_modTableLookupLogXClass;
extern const int awe_modTableSourceV2Class;
extern const int awe_modTablesourceClass;
extern const int awe_modTimeOutClass;
extern const int awe_modTimeOutFract32Class;
extern const int awe_modTriggeredPulseClass;
extern const int awe_modTriggeredPulseV2Class;
extern const int awe_modTriggeredSinkClass;
extern const int awe_modTwoPieceInterpV2Class;
extern const int awe_modUnwrapClass;
extern const int awe_modUpFIRDnConverterClass;
extern const int awe_modUpdateSampleRateClass;
extern const int awe_modUpsamplerClass;
extern const int awe_modVolumeControlHPClass;
extern const int awe_modWAVInterp16OneShotSourceFract32Class;
extern const int awe_modWAVInterpOneShotSourceClass;
extern const int awe_modWindowClass;
extern const int awe_modWindowAliasClass;
extern const int awe_modWindowAliasFract32Class;
extern const int awe_modWindowCustomClass;
extern const int awe_modWindowFract32Class;
extern const int awe_modWindowOverlapClass;
extern const int awe_modWindowOverlapFract32Class;
extern const int awe_modWindowV2Class;
extern const int awe_modZeroPaddingClass;
extern const int awe_modZeroPaddingFract32Class;


// Deprecated Modules
extern const int awe_modAGCMultiplierClass;
extern const int awe_modApplyPresetClass;
extern const int awe_modDCSourceClass;
extern const int awe_modDCSourceControlClass;
extern const int awe_modDCSourceControlFract32Class;
extern const int awe_modDCSourceFract32Class;
extern const int awe_modDeMultiplexorClass;
extern const int awe_modDelayciClass;
extern const int awe_modDelayiClass;
extern const int awe_modFileStreamingClass;
extern const int awe_modFloatToFract32Class;
extern const int awe_modFloatToIntClass;
extern const int awe_modFract32ToFloatClass;
extern const int awe_modFract32ToIntClass;
extern const int awe_modImpulseSourceClass;
extern const int awe_modIntToFloatClass;
extern const int awe_modIntToFract32Class;
extern const int awe_modMatlabStreamingClass;
extern const int awe_modMixerClass;
extern const int awe_modMixerDenseClass;
extern const int awe_modMixerSmoothedClass;
extern const int awe_modMixerSmoothedV2bClass;
extern const int awe_modMixerSparseSmoothedClass;
extern const int awe_modMultiplexorControlClass;
extern const int awe_modMultiplexorFadeClass;
extern const int awe_modMultiplexorFadeFract32Class;
extern const int awe_modMultiplexorSmoothedClass;
extern const int awe_modMultiplexorSmoothedFract32Class;
extern const int awe_modMultiplierClass;
extern const int awe_modProbeClass;
extern const int awe_modRawStreamingSourceClass;
extern const int awe_modSawtoothClass;
extern const int awe_modScalerClass;
extern const int awe_modScalerDBClass;
extern const int awe_modScalerDBSmoothedClass;
extern const int awe_modScalerNClass;
extern const int awe_modScalerNDBSmoothedClass;
extern const int awe_modScalerNSmoothedClass;
extern const int awe_modScalerSmoothedClass;
extern const int awe_modSineGenClass;
extern const int awe_modTableInterpRuntimeClass;
extern const int awe_modTwoPieceInterpClass;


// Internal Modules
extern const int awe_modCompiledSubsystemClass;
extern const int awe_modFifoInClass;
extern const int awe_modFifoInV2Class;
extern const int awe_modFifoOutClass;
extern const int awe_modFifoOutV2Class;
extern const int awe_modIPCFifoInClass;
extern const int awe_modIPCFifoOutClass;
extern const int awe_modProxyGetSetClass;


// Additional Modules



// Edit this list to prune modules from target
#define STANDARD_LIST \
&awe_modAGCCoreClass, \
&awe_modAGCLimiterCoreClass, \
&awe_modAGCLimiterCoreFract32Class, \
&awe_modAGCLimiterCoreV2Class, \
&awe_modAGCLimiterCoreV2Fract32Class, \
&awe_modAGCMultiplierFract32Class, \
&awe_modAGCNoiseGateCoreClass, \
&awe_modAbsClass, \
&awe_modAbsFract32Class, \
&awe_modAcosClass, \
&awe_modAdderClass, \
&awe_modAdderFract32Class, \
&awe_modAdderInt32Class, \
&awe_modAsinClass, \
&awe_modAtanClass, \
&awe_modAtan2Class, \
&awe_modBalanceClass, \
&awe_modBalanceFract32Class, \
&awe_modBiquadClass, \
&awe_modBiquadCascadeClass, \
&awe_modBiquadCascadeFract32Class, \
&awe_modBiquadFract32Class, \
&awe_modBiquadNCascadeClass, \
&awe_modBiquadSmoothedClass, \
&awe_modBiquadSmoothedFract32Class, \
&awe_modBiquadV2Fract32Class, \
&awe_modBitsToIntClass, \
&awe_modBlockCounterClass, \
&awe_modBooleanInvertClass, \
&awe_modBooleanSinkClass, \
&awe_modBooleanSourceClass, \
&awe_modButterworthFilterClass, \
&awe_modButterworthFilterFract32Class, \
&awe_modCeilClass, \
&awe_modClipAsymClass, \
&awe_modClipAsymFract32Class, \
&awe_modClipAsymInt32Class, \
&awe_modClipIndicatorClass, \
&awe_modClipIndicatorFract32Class, \
&awe_modCopierClass, \
&awe_modCosClass, \
&awe_modCoshClass, \
&awe_modCounterClass, \
&awe_modCrossFaderClass, \
&awe_modDCSourceIntClass, \
&awe_modDCSourceV2Class, \
&awe_modDCSourceV2Fract32Class, \
&awe_modDb10Class, \
&awe_modDb10ApproxClass, \
&awe_modDb10Fract32Class, \
&awe_modDb20Class, \
&awe_modDb20ApproxClass, \
&awe_modDb20Fract32Class, \
&awe_modDcBlockClass, \
&awe_modDebounceClass, \
&awe_modDeinterleaveClass, \
&awe_modDelayClass, \
&awe_modDelayMsecClass, \
&awe_modDivideClass, \
&awe_modExpClass, \
&awe_modFIRClass, \
&awe_modFIRFract32Class, \
&awe_modFIRSmoothedClass, \
&awe_modFloorClass, \
&awe_modFmodClass, \
&awe_modFrexpClass, \
&awe_modGPIOClass, \
&awe_modGraphicEQBandClass, \
&awe_modGraphicEQBandFract32Class, \
&awe_modImpulseMsecSourceClass, \
&awe_modImpulseMsecSourceFract32Class, \
&awe_modImpulseSourceFract32Class, \
&awe_modIntToBitsClass, \
&awe_modInterleaveClass, \
&awe_modInvertClass, \
&awe_modInvertFract32Class, \
&awe_modLShiftFract32Class, \
&awe_modLdexpClass, \
&awe_modLinkwitzRileyFilterClass, \
&awe_modLogClass, \
&awe_modLog10Class, \
&awe_modLog10Fract32Class, \
&awe_modLog2Class, \
&awe_modLog2Fract32Class, \
&awe_modLogicAllClass, \
&awe_modLogicAndConstFract32Class, \
&awe_modLogicAndConstInt32Class, \
&awe_modLogicAnyClass, \
&awe_modLogicBinaryOpClass, \
&awe_modLogicCompareClass, \
&awe_modLogicCompareConstClass, \
&awe_modLogicCompareConstFract32Class, \
&awe_modLogicCompareConstInt32Class, \
&awe_modLogicCompareConstReplaceClass, \
&awe_modLogicCompareConstReplaceInt32Class, \
&awe_modLogicCompareFract32Class, \
&awe_modLogicCompareInt32Class, \
&awe_modMaxAbsClass, \
&awe_modMaxAbsFract32Class, \
&awe_modMeterClass, \
&awe_modMeterFract32Class, \
&awe_modMixerDenseFract32Class, \
&awe_modMixerFract32Class, \
&awe_modMixerSmoothedFract32Class, \
&awe_modMixerV3Class, \
&awe_modMixerV3Fract32Class, \
&awe_modModfClass, \
&awe_modMultiplexorClass, \
&awe_modMultiplexorV2Class, \
&awe_modMultiplexorV2Fract32Class, \
&awe_modMultiplierFract32Class, \
&awe_modMultiplierV2Class, \
&awe_modMultiplierV2Fract32Class, \
&awe_modMuteNSmoothedClass, \
&awe_modMuteNSmoothedFract32Class, \
&awe_modMuteSmoothedClass, \
&awe_modMuteSmoothedFract32Class, \
&awe_modMuteUnmuteClass, \
&awe_modMuteUnmuteFract32Class, \
&awe_modNullSinkClass, \
&awe_modNullSinkV2Class, \
&awe_modNullSourceClass, \
&awe_modParamGetClass, \
&awe_modParamSetClass, \
&awe_modPeriodicFunctionGenClass, \
&awe_modPeriodicSourceClass, \
&awe_modPeriodicSourceFract32Class, \
&awe_modPolynomialClass, \
&awe_modPolynomialFract32Class, \
&awe_modPowClass, \
&awe_modPow10toXClass, \
&awe_modPow2Fract32Class, \
&awe_modPowFract32Class, \
&awe_modPulseGenClass, \
&awe_modPulseGenFract32Class, \
&awe_modRMSClass, \
&awe_modRMSFract32Class, \
&awe_modRMSNClass, \
&awe_modRandClass, \
&awe_modReciprocalClass, \
&awe_modReciprocalFract32Class, \
&awe_modRemainderClass, \
&awe_modRoundClass, \
&awe_modRouterClass, \
&awe_modRouterSmoothedClass, \
&awe_modRouterSmoothedFract32Class, \
&awe_modSMixer2x1Class, \
&awe_modSMixer2x1Fract32Class, \
&awe_modSOFSmoothedCascadeFract32Class, \
&awe_modSawtoothFract32Class, \
&awe_modScaleOffsetClass, \
&awe_modScaleOffsetFract32Class, \
&awe_modScaleOffsetInt32Class, \
&awe_modScalerControlClass, \
&awe_modScalerControlBlockSmoothedFract32Class, \
&awe_modScalerControlSmoothedFract32Class, \
&awe_modScalerDBControlClass, \
&awe_modScalerDBFract32Class, \
&awe_modScalerDBSmoothedFract32Class, \
&awe_modScalerFract32Class, \
&awe_modScalerNControlClass, \
&awe_modScalerNDBSmoothedFract32Class, \
&awe_modScalerNFract32Class, \
&awe_modScalerNSmoothedFract32Class, \
&awe_modScalerNSymmetricFadingClass, \
&awe_modScalerNV2Class, \
&awe_modScalerSmoothedFract32Class, \
&awe_modScalerSymmetricFadingClass, \
&awe_modScalerSymmetricFadingV2Class, \
&awe_modScalerV2Class, \
&awe_modSecondOrderFilterSmoothedClass, \
&awe_modSecondOrderFilterSmoothedCascadeClass, \
&awe_modSecondOrderFilterSmoothedCascadeV2Class, \
&awe_modSecondOrderFilterSmoothedFract32Class, \
&awe_modShiftFract32Class, \
&awe_modSignClass, \
&awe_modSinClass, \
&awe_modSineGenControlFract32Class, \
&awe_modSineGenFract32Class, \
&awe_modSineSmoothedGenClass, \
&awe_modSineSmoothedGenFract32Class, \
&awe_modSinhClass, \
&awe_modSinkClass, \
&awe_modSinkFract32Class, \
&awe_modSinkFract32PropertiesClass, \
&awe_modSinkIntClass, \
&awe_modSinkIntPropertiesClass, \
&awe_modSinkPropertiesClass, \
&awe_modSofV2HpFract32Class, \
&awe_modSoftClipClass, \
&awe_modSoftClipFract32Class, \
&awe_modSoloMuteClass, \
&awe_modSoloMuteFract32Class, \
&awe_modSourceClass, \
&awe_modSourceFract32Class, \
&awe_modSourceFract32PropertiesClass, \
&awe_modSourceIntClass, \
&awe_modSourceIntPropertiesClass, \
&awe_modSourcePropertiesClass, \
&awe_modSqrtClass, \
&awe_modSqrtFract32Class, \
&awe_modSquareClass, \
&awe_modSquareAddClass, \
&awe_modSquareAddFract32Class, \
&awe_modSquareFract32Class, \
&awe_modSubtractClass, \
&awe_modSubtractFract32Class, \
&awe_modSubtractInt32Class, \
&awe_modSumDiffClass, \
&awe_modSumDiffFract32Class, \
&awe_modSumDiffInt32Class, \
&awe_modTableInterpClass, \
&awe_modTableInterpFract32Class, \
&awe_modTableLookupClass, \
&awe_modTableLookupV2Class, \
&awe_modTanClass, \
&awe_modTanhClass, \
&awe_modThreeBandToneControlClass, \
&awe_modThreeBandToneControlFract32Class, \
&awe_modToggleButtonClass, \
&awe_modTypeConversionClass, \
&awe_modUndb10Class, \
&awe_modUndb10ApproxClass, \
&awe_modUndb10Fract32Class, \
&awe_modUndb20Class, \
&awe_modUndb20ApproxClass, \
&awe_modUndb20Fract32Class, \
&awe_modVolumeControlClass, \
&awe_modVolumeControlFract32Class, \
&awe_modWetDryClass, \
&awe_modWhiteNoiseClass, \
&awe_modWhiteNoiseFract32Class, \
&awe_modWithinRangeClass, \
&awe_modWithinRangeFract32Class, \
&awe_modZeroCrossingDetectorClass, \
&awe_modZeroCrossingDetectorFract32Class, \
&awe_modZeroSourceClass

#define ADVANCED_LIST \
&awe_modAGCAttackReleaseClass, \
&awe_modAGCAttackReleaseFract32Class, \
&awe_modAGCCompressorCoreClass, \
&awe_modAGCCompressorCoreFract32Class, \
&awe_modAGCCoreFract32Class, \
&awe_modAGCVariableAttackReleaseClass, \
&awe_modAllpassDelayClass, \
&awe_modAllpassDelay16Class, \
&awe_modAllpassDelay16Fract32Class, \
&awe_modAllpassDelayFract32Class, \
&awe_modAllpassDelayciClass, \
&awe_modAllpassDelayiClass, \
&awe_modArraysetClass, \
&awe_modAttackDecaySustainReleaseClass, \
&awe_modAudioWeightingClass, \
&awe_modAudioWeightingFract32Class, \
&awe_modAveragerClass, \
&awe_modAveragerFract32Class, \
&awe_modBandToBinClass, \
&awe_modBinToBandClass, \
&awe_modBiquadCascadeHPClass, \
&awe_modBiquadCascadeHPFract32Class, \
&awe_modBiquadLoadingClass, \
&awe_modBiquadLoadingFract32Class, \
&awe_modBiquadSmoothedHPClass, \
&awe_modBiquadSparseClass, \
&awe_modBiquadSparseHPClass, \
&awe_modBiquadSparseHPFract32Class, \
&awe_modBiquadSparseV2Class, \
&awe_modBiquadSparseV3Class, \
&awe_modBiquadSparseV4Class, \
&awe_modBiquadSparseV5Class, \
&awe_modBiquadSparseV6Class, \
&awe_modBlockConcatenateClass, \
&awe_modBlockDelayClass, \
&awe_modBlockDelayV2Class, \
&awe_modBlockExtractClass, \
&awe_modBlockFlipClass, \
&awe_modBlockMedianClass, \
&awe_modBlockStatisticsClass, \
&awe_modBlockStatisticsFract32Class, \
&awe_modBlockStatisticsNClass, \
&awe_modButterworthFilterHPClass, \
&awe_modCfftClass, \
&awe_modCfftFract32Class, \
&awe_modCoeffTableClass, \
&awe_modComplexAngleClass, \
&awe_modComplexConjugateClass, \
&awe_modComplexConjugateFract32Class, \
&awe_modComplexDivideClass, \
&awe_modComplexMagnitudeClass, \
&awe_modComplexMagnitudeFract32Class, \
&awe_modComplexMagnitudeSquaredClass, \
&awe_modComplexModulateClass, \
&awe_modComplexMultiplierClass, \
&awe_modComplexMultiplierFract32Class, \
&awe_modComplexMultiplierV2Class, \
&awe_modComplexToPolarClass, \
&awe_modComplexToRealImagClass, \
&awe_modComplexToRealImagFract32Class, \
&awe_modConsecutiveCountIntClass, \
&awe_modConvolveClass, \
&awe_modCorrelateClass, \
&awe_modCorrelationFract32Class, \
&awe_modCycleBurnerClass, \
&awe_modCycleBurnerSetClass, \
&awe_modDateTimeClass, \
&awe_modDeadBandClass, \
&awe_modDelay16Class, \
&awe_modDelayInterpClass, \
&awe_modDelayInterpFract32Class, \
&awe_modDelayMixerSmoothedClass, \
&awe_modDelayNChanClass, \
&awe_modDelayNChanMsecClass, \
&awe_modDelayNChanMsecSmoothedClass, \
&awe_modDelayNChanSmoothedClass, \
&awe_modDelayNChanV2Class, \
&awe_modDelayNTapClass, \
&awe_modDelayNTap16Fract32Class, \
&awe_modDelayReaderClass, \
&awe_modDelayStateWriterClass, \
&awe_modDelayStateWriter16Class, \
&awe_modDelayciFract32Class, \
&awe_modDerivativeClass, \
&awe_modDerivativeFract32Class, \
&awe_modDitherFract32Class, \
&awe_modDownsamplerClass, \
&awe_modDownwardExpanderCoreClass, \
&awe_modDownwardExpanderCoreFract32Class, \
&awe_modDuckerClass, \
&awe_modDuckerFract32Class, \
&awe_modEmphasisFilterClass, \
&awe_modEmphasisFilterFract32Class, \
&awe_modExpanderCoreClass, \
&awe_modFIRDecimatorClass, \
&awe_modFIRDecimatorFract32Class, \
&awe_modFIRInterpolatorClass, \
&awe_modFIRInterpolatorFract32Class, \
&awe_modFIRLoadingClass, \
&awe_modFIRLoadingFract32Class, \
&awe_modFIRNChanClass, \
&awe_modFIRReaderClass, \
&awe_modFIRSparseClass, \
&awe_modFIRSparseReaderClass, \
&awe_modFIRSparseReaderFract16Class, \
&awe_modFIRSparseReaderFract32Class, \
&awe_modFOFControlClass, \
&awe_modFOFControlFract32Class, \
&awe_modFftClass, \
&awe_modFftFract32Class, \
&awe_modFilterbankFIRNChanClass, \
&awe_modFilterbankFIRNChanV2Class, \
&awe_modFractionalDelayV2Class, \
&awe_modFractionalDelayV2Fract32Class, \
&awe_modGetWirePropertiesClass, \
&awe_modGraphicEQBandHPClass, \
&awe_modGraphicEQBandHpFract32Class, \
&awe_modHistogramClass, \
&awe_modHysteresisClass, \
&awe_modHysteresisFract32Class, \
&awe_modIfftClass, \
&awe_modIfftFract32Class, \
&awe_modIntegralClass, \
&awe_modInterleavedSinkClass, \
&awe_modInvertNClass, \
&awe_modLMSClass, \
&awe_modLMSNormFract32Class, \
&awe_modLPF1ControlClass, \
&awe_modMapperControlClass, \
&awe_modMathExceptionClass, \
&awe_modMatrixMultiplicationClass, \
&awe_modMeasurementClass, \
&awe_modMeasurementV2Class, \
&awe_modMemoryLoadingClass, \
&awe_modMultichannelMixerSmoothedClass, \
&awe_modMuteLinearFadeClass, \
&awe_modMuteSymmetricClass, \
&awe_modMuteSyncClass, \
&awe_modMuteSyncFract32Class, \
&awe_modOscillatorClass, \
&awe_modOverlapAddClass, \
&awe_modOverlapAddFract32Class, \
&awe_modOverwriteControlClass, \
&awe_modPDFClass, \
&awe_modParamSetTableV2Class, \
&awe_modPeakFinderClass, \
&awe_modPeriodicStreamingClass, \
&awe_modPolarToComplexClass, \
&awe_modPresetArrayHandlerClass, \
&awe_modPresetArrayHandlerV2Class, \
&awe_modProbeFract32Class, \
&awe_modRandiClass, \
&awe_modRandiFract32Class, \
&awe_modRealImagToComplexClass, \
&awe_modRealImagToComplexFract32Class, \
&awe_modRebufferClass, \
&awe_modRepWinOverlapClass, \
&awe_modRepWinOverlapFract32Class, \
&awe_modRunningMinMaxClass, \
&awe_modRunningMinMaxFract32Class, \
&awe_modRunningStatisticsClass, \
&awe_modRunningStatisticsFract32Class, \
&awe_modRunningStatisticsNClass, \
&awe_modSOFCascadeHPClass, \
&awe_modSOFCascadeHPFract32Class, \
&awe_modSOFControlClass, \
&awe_modSOFControlFract32Class, \
&awe_modSOFControlHPClass, \
&awe_modSOFControlV2Class, \
&awe_modSOFControlV3Class, \
&awe_modSampleAndHoldClass, \
&awe_modSampleAndHoldInt32Class, \
&awe_modSampleMultiplexorControlClass, \
&awe_modSampleRateClass, \
&awe_modSampleStatisticsClass, \
&awe_modSampleStatisticsFract32Class, \
&awe_modSbAttackReleaseClass, \
&awe_modSbComplexFIRClass, \
&awe_modSbComplexFIRvlClass, \
&awe_modSbDerivativeClass, \
&awe_modSbNLMSClass, \
&awe_modSbNLMSV2Class, \
&awe_modSbNLMSV3Class, \
&awe_modSbRMSClass, \
&awe_modSbSOFClass, \
&awe_modSbSmoothClass, \
&awe_modSbSmoothFract32Class, \
&awe_modSbSmoothV2Class, \
&awe_modSbSplitterClass, \
&awe_modSecondOrderFilterHPClass, \
&awe_modSetWirePropertiesClass, \
&awe_modShiftSamplesClass, \
&awe_modSlewRateLimiterClass, \
&awe_modStatusSetClass, \
&awe_modStatusSetV2Class, \
&awe_modSubblockStatisticsClass, \
&awe_modSubblockStatisticsFract32Class, \
&awe_modSweepGenClass, \
&awe_modSystemVariableClass, \
&awe_modTHDClass, \
&awe_modTableInterp2dClass, \
&awe_modTableLookupIntFloatClass, \
&awe_modTableLookupIntIntClass, \
&awe_modTableLookupLogXClass, \
&awe_modTableSourceV2Class, \
&awe_modTablesourceClass, \
&awe_modTimeOutClass, \
&awe_modTimeOutFract32Class, \
&awe_modTriggeredPulseClass, \
&awe_modTriggeredPulseV2Class, \
&awe_modTriggeredSinkClass, \
&awe_modTwoPieceInterpV2Class, \
&awe_modUnwrapClass, \
&awe_modUpFIRDnConverterClass, \
&awe_modUpdateSampleRateClass, \
&awe_modUpsamplerClass, \
&awe_modVolumeControlHPClass, \
&awe_modWAVInterp16OneShotSourceFract32Class, \
&awe_modWAVInterpOneShotSourceClass, \
&awe_modWindowClass, \
&awe_modWindowAliasClass, \
&awe_modWindowAliasFract32Class, \
&awe_modWindowCustomClass, \
&awe_modWindowFract32Class, \
&awe_modWindowOverlapClass, \
&awe_modWindowOverlapFract32Class, \
&awe_modWindowV2Class, \
&awe_modZeroPaddingClass, \
&awe_modZeroPaddingFract32Class

#define DEPRECATED_LIST \
&awe_modAGCMultiplierClass, \
&awe_modApplyPresetClass, \
&awe_modDCSourceClass, \
&awe_modDCSourceControlClass, \
&awe_modDCSourceControlFract32Class, \
&awe_modDCSourceFract32Class, \
&awe_modDeMultiplexorClass, \
&awe_modDelayciClass, \
&awe_modDelayiClass, \
&awe_modFileStreamingClass, \
&awe_modFloatToFract32Class, \
&awe_modFloatToIntClass, \
&awe_modFract32ToFloatClass, \
&awe_modFract32ToIntClass, \
&awe_modImpulseSourceClass, \
&awe_modIntToFloatClass, \
&awe_modIntToFract32Class, \
&awe_modMatlabStreamingClass, \
&awe_modMixerClass, \
&awe_modMixerDenseClass, \
&awe_modMixerSmoothedClass, \
&awe_modMixerSmoothedV2bClass, \
&awe_modMixerSparseSmoothedClass, \
&awe_modMultiplexorControlClass, \
&awe_modMultiplexorFadeClass, \
&awe_modMultiplexorFadeFract32Class, \
&awe_modMultiplexorSmoothedClass, \
&awe_modMultiplexorSmoothedFract32Class, \
&awe_modMultiplierClass, \
&awe_modProbeClass, \
&awe_modRawStreamingSourceClass, \
&awe_modSawtoothClass, \
&awe_modScalerClass, \
&awe_modScalerDBClass, \
&awe_modScalerDBSmoothedClass, \
&awe_modScalerNClass, \
&awe_modScalerNDBSmoothedClass, \
&awe_modScalerNSmoothedClass, \
&awe_modScalerSmoothedClass, \
&awe_modSineGenClass, \
&awe_modTableInterpRuntimeClass, \
&awe_modTwoPieceInterpClass

#define INTERNAL_LIST \
&awe_modCompiledSubsystemClass, \
&awe_modFifoInClass, \
&awe_modFifoInV2Class, \
&awe_modFifoOutClass, \
&awe_modFifoOutV2Class, \
&awe_modIPCFifoInClass, \
&awe_modIPCFifoOutClass, \
&awe_modProxyGetSetClass

#define CUSTOM_LIST 


#define LISTOFCLASSOBJECTS \
STANDARD_LIST, \
ADVANCED_LIST, \
INTERNAL_LIST, \
DEPRECATED_LIST



#ifdef	__cplusplus
}
#endif

#endif	// _MODULELIST_H
