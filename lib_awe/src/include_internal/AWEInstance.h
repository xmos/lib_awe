/*******************************************************************************
*
*				Audio Framework
*				---------------
*
********************************************************************************
*	  AWEInstance.h
********************************************************************************
*
*	  Description:	Public API AWEInstance structure and functions
*
*	  Copyright:	(c) 2007-2021 DSP Concepts, Inc. All rights reserved.
*					3235 Kifer Road
*					Santa Clara, CA 95054
*
*******************************************************************************/

/**
* @file 
* @brief The AWEInstance API Header File
*/

#ifndef _AWECOREINSTANCE_H
#define _AWECOREINSTANCE_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "TargetProcessor.h" //Include the TargetProcessor header that includes another processor specific header file. 

#ifdef	__cplusplus
extern "C" {
#endif

/** 
@ingroup AWEInstance
@brief The AWE instance. 
@details The AWE Instance struct must have its members/pointers assigned at init time. 
The AWEInstance is the most important structure, and it must be initialized properly. 
After assigning all of the required members, the BSP author will call the awe_init() function. 
All members are required to be assigned unless they are marked optional in the detailed description below..
*/
typedef struct _AWEInstance AWEInstance;
typedef struct _AWEInstance2
{
	/** The ID of this instance. Single instance systems should always be 0.
	The instanceID of the AWEInstance must match the instanceID of your desired AWE commands.
	For example, if your packet router is routing commands to this instance with an address of 16,
	the instance's instanceID must be 16. See Packet Routing pseudocode above*/
	UINT32 instanceId; 
	
	/*------------------HEAPS------------------*/
	/** Fast heap A. This is the main heap. AWE heaps are user allocated memory required by the AWE Instance.  */
	UINT32 *pFastHeapA; 

	/** The second fast heap, B . */
	UINT32 *pFastHeapB;

	/** The slow heap. */
	UINT32 *pSlowHeap;

	/** The fast heap A size. Heap sizes will vary from platform to platform, and also depend on the size of a layout.
		A good place to start for each heap is 1048576, however this will be too large for some platforms. 
	*/
	UINT32 fastHeapASize;

	/** The fast heap B size. */
	UINT32 fastHeapBSize;

	/** The slow heap size. */
	UINT32 slowHeapSize;

	/*------------------AUDIO CALLBACKS------------------*/
	
	/** OPTIONAL This callback is invoked when a layout is run or when a StartAudio command is sent.
	A user will define their own callback function and initialize this member with a pointer to their function.
	This callback can be used for whatever the BSP author pleases, including configuring audio devices based on layout properties, etc..
	*/
	INT32 (*cbAudioStart)(struct _AWEInstance *PAWE); 

	/** OPTIONAL. This callback is invoked when a layout is stopped or a StopAudio command is sent
	A user will define their own callback function and init this member with a pointer to their function.
	This callback can also be used for whatever the BSP author pleases, including halting audio streams, freeing buffers, etc...
	*/
	INT32 (*cbAudioStop)(struct _AWEInstance *pAWE);


	/*------------------IO PINS------------------*/
	
	/**A BSP author must define/allocate an input pin in their BSP and assign it to this member
	NOTE: AudioWeaver only supports single input/output pins, so the pin must have only ONE element 
	Example: static IOPinDescriptor s_InputPin[1];
	*/
	IOPinDescriptor *pInputPin;

	/**A BSP author must define/allocate an output pin in their BSP and assign it to this member
	NOTE: AudioWeaver only supports single input/output pins, so the pin must have only ONE element 
	Example: static IOPinDescriptor s_OutputPin[1];
	*/
	IOPinDescriptor *pOutputPin;
	
	/*------------------MODULES------------------*/

	/** Number of modules in module table. This can be manually set, 
	or it can be determined by getting the size of the module descriptor table.
	Example: 
			UINT32 module_descriptor_table_size = sizeof(g_module_descriptor_table) / sizeof(g_module_descriptor_table[0]); 
			AWEInstance1.numModules = module_descriptor_table_size;
	*/
	UINT32 numModules;

	/** Pointer to module table. The module table is the list of modules that will be included in the instance. 
	See the provided "ModuleList.h" header for syntax specifics. 
	Any set of available modules can be included/excluded from the instance by simply adding or removing them from the table. 
	Note: this is often used for reducing the memory usage in production. AudioWeaver Designer can generate a ModuleList.h from a layout
	that ONLY includes the modules used in that layout. 
	*/
	const ModClassModule **pModuleDescriptorTable;
	
	/*------------------PACKET BUFFERS------------------*/

    /** The Packet buffer pointer. A BSP author must allocate a buffer of length packetBufferSize and set pPacketBuffer to the address of that buffer. 
	Example:
			g_AWEInstance.packetBufferSize = packetBufferSize; 
			UINT32 AWE_Packet_Buffer[g_AWEInstance.packetBufferSize];
			g_AWEInstance.pPacketBuffer = AWE_Packet_Buffer;
	*/
    UINT32 *pPacketBuffer;

	/** Reply buffer pointer. Initialized the same way as pPacketBuffer.
		The pPacketBuffer will be processed and the reply will be inserted into pReplyBuffer. 
	Example:
			g_AWEInstance.packetBufferSize = packetBufferSize; 
			UINT32 AWE_Reply_Buffer[g_AWEInstance.packetBufferSize];
			g_AWEInstance.pReplyBuffer = AWE_Reply_Buffer;		
	*/
	UINT32 *pReplyBuffer;

    /** Packet buffer size. Must be initialized to the size of the BSP's packet buffers. 
		The recommended packet buffer size is 264 words. If you need to use a smaller packetBufferSize
		due to memory constraints on your target, please contact DSPC Engineering. 
		(The absolute minimum packetBufferSize is 16 and absolute max is 4105) 
	*/
    UINT32 packetBufferSize;
	
    /** User Version word. A BSP author can initialize this to any UINT32 value they would like. 
	    Should mean something to the BSP author, but no required meaning/syntax.
	*/
    UINT32 userVersion;
	
	/** A BSP author will set this to the speed of the CPU they are integrating into. */
    float coreSpeed;

	/** Profiling clock speed in Hz. */
    float profileSpeed;

/*The name of the AWE Instance that will be displayed in Server. Can be any string of XYZ length?*/
#ifdef __ADSP21000__ 
    /** Name of this instance as an int*/
    const UINT32 *pName;
#else
    /** Name of this instance as a string  */
    const char *pName;
#endif

	/** Number of threads supported for multithreaded systems(1-4). */
    UINT32 numThreads;
	
	/** Default sample rate of this instance. */
    float sampleRate;

	/** Base frame size of this instance. In order for a layout to run on an instance, it must be a multiple of the fundamental blocksize
	*/
    UINT32 fundamentalBlockSize;

    /** DSPC Flash file system instance. OPTIONAL if no FFS. 
	If implementing the optional flash file system, then assign this pointer to your properly initialized flash file system instance
	*/
    AWEFlashFSInstance *pFlashFileSystem;

#ifdef AWEINSTANCE_FRAMEWORK
    AWEINSTANCE_FRAMEWORK
#else
	/** Internal members. Reserved memory. */
    #ifdef BUILD64
	UINT32 _Reserved[564];
    #else
	UINT32 _Reserved[295];
    #endif
#endif

} AWEInstance2;


/*------------------------------------------Initialization----------------------------------------------------*/
/**
 * @brief Initialize the instance. Call this once you have assigned all of the required 
	members/pointers of the AWE Instance, and after calling awe_InitPin on both input and output pins. 
	Calling awe_init before assigning all members of the instance structure or initializing the pins
	will result in crashes/erratic behavior. 
 * @param pAWE						instance to initialize
 * @return 							@ref E_SUCCESS,  @ref E_NULL_PACKET_BUFFER_POINTER,  @ref E_NULL_REPLY_BUFFER_POINTER, 
 *  @ref E_ZERO_PACKET_BUFFER_LENGTH,  @ref E_EXCEPTION,  @ref E_PARAMETER_ERROR,  @ref E_NOT_CREATED
 */
INT32 awe_init(AWEInstance *pAWE);


/**
 * @brief Initialize an input or output pin. This function must be called for both the input and output pins.
 * Must be called BEFORE the awe_init API function.  
 * @param [in] pPin					the pin instance to initialize
 * @param [in] channels				number of channels
 * @param [in] name					optional pin name
 * @return							@ref E_SUCCESS,  @ref E_NOT_OBJECT
 */
INT32 awe_initPin(IOPinDescriptor *pPin, UINT32 channels, const char *name);


/**
 * @brief Initialize the file system.
 * @param pAWE						instance to initialize
 * @param pAWEFlashFSInstance       the file system instance
 */
void awe_initFlashFS(AWEInstance * pAWE, AWEFlashFSInstance * pAWEFlashFSInstance);

typedef INT32 packetProcessFunction(AWEInstance * pAWE);


/*------------------------------------------Packet----------------------------------------------------*/
/**
 * @brief Process an AWEInstance's newly received tuning packet. 
 * @param [in] pAWE			The AWE instance pointer to process
 * @return error			@ref E_SUCCESS,  @ref E_COMMAND_NOT_IMPLEMENTED,  @ref E_MESSAGE_LENGTH_TOO_LONG, 
 *  @ref E_CRC_ERROR,  @ref E_BADPACKET 
 */
INT32 awe_packetProcess(AWEInstance * pAWE);


/*------------------------------------------Audio----------------------------------------------------*/

/**
 * @brief Audio pump function. Call this in your audio thread once you have imported the data you wish to process.
 *  Before pumping, you must also check that the AudioWeaver buffers have been filled (awe_getPumpMask) 
	and that audio has been started (awe_audioIsStarted). 
	This function will return 1 if awe_deferredSetCall needs to be called. This is for modules that need to do more time consuming
	calculations, like calculating filter coefficients. For example, a SOF Control module needs to calculate new coeffs when the frequency is modified by the control input.
 * @param [in] pAWE					AWE instance pointer
 * @param [in] layoutIndex			the layout index to pump
 * @return							@ref E_SUCCESS,  @ref E_NO_LAYOUTS, or 1 (indicating that deferred set is needed, see documentation)
 */
INT32 awe_audioPump(AWEInstance *pAWE, UINT32 layoutIndex);

/**
 * @brief Import samples from a user buffer to a channel. The value of the fundamentalBlockSize member of the AWEInstance determines the number of samples that are imported with each call.
	Call this function once for each audio input channel on the hardware. 
	Channel matching between hardware capabilities and layout will be done automatically. For example, if the target has only 1 audio input channel, 
	and if a layout with 3 input channels is loaded, the second and third channels of the layout input will be all zeros. Inversely, if there are more
	HW input channels than the layout expects, then they are ignored.
	
	The value of inStride defines the number of samples to skip between each read, and should be the number of interleaved channels in the inSamples buffer being read from. 
	A typical usage for a single source, interleaved stereo input could look like:
	@code
	awe_audioImportSamples(pAWE, inSamples + 0, 2, 0, Sample32bit);
	awe_audioImportSamples(pAWE, inSamples + 1, 2, 1, Sample32bit);
	@endcode
	
 * @param [in] pAWE						AWE instance
 * @param [in] inSamples				pointer to first sample in buffer to import for current channel
 * @param [in] inStride					input buffer import stride
 * @param [in] channel					channel to write to
 * @param [in] inType					type of input data
 * @return								@ref E_SUCCESS,  @ref E_AUDIO_NOT_STARTED,  @ref E_PARAMETER_ERROR
 */
INT32 awe_audioImportSamples(const AWEInstance *pAWE, const void *inSamples, INT32 inStride, INT32 channel, SampleType inType);

/**
 * @brief Export samples to a user buffer from a channel. The value of the fundamentalBlockSize member of the AWEInstance determines the number of samples that are exported with each call.
	Call this function once for each audio output channel on the hardware. 
	Like awe_audioImportSamples, this will do channel matching automatically. For example, if the target has 3 audio output channels, 
	and if a layout with 1 output channel is loaded, the second and third channels of the exported output will be all zeros. Inversely, if
	the layout has more output channels than the target supports, they are ignored.

	The value of outStride defines the number of samples to skip between each write. It should be equal to the number of interleaved channels in the outSamples buffer being written to. 
	A typical usage for exporting to an interleaved 3 channel output could look like:
	@code
	awe_audioExportSamples(pAWE, &outSamples[0], 3, 0, Sample32bit);
	awe_audioExportSamples(pAWE, &outSamples[1], 3, 1, Sample32bit);
	awe_audioExportSamples(pAWE, &outSamples[2], 3, 2, Sample32bit);
	@endcode
	
 * @param [in] pAWE						AWE instance
 * @param [out] outSamples				pointer to first sample in buffer to export to
 * @param [in] outStride				output buffer export stride
 * @param [in] channel					channel to read from
 * @param [in] outType					type of output data
 * @return								@ref E_SUCCESS,  @ref E_AUDIO_NOT_STARTED,  @ref E_PARAMETER_ERROR
 */
INT32 awe_audioExportSamples(const AWEInstance *pAWE,  void *outSamples, INT32 outStride, INT32 channel, SampleType outType);

/**
 * @brief Check if this instance is running. 
   A BSP author can use this to determine if the instance is already running to avoid pumping audio through a dead layout.
 * @param pAWE						instance pointer
 * @return							0 if audio is not started, 1 if audio is started. No error conditions
 */
INT32 awe_audioIsStarted(const AWEInstance *pAWE);

/**
 * @brief Test if AWE is ready to run. This will return the "layout mask" which is a bit vector of threads to run.
   For a simple, single threaded system, the bitvector will be 0x1. 
   If a previous pump is not complete and the layout is ready to pump again, an overflow is detected by this function. 
   In this condition, the layoutMask bit for the overflowed layout will be zero, which prevents the layout from being pumped again.
   Example:
		int layoutMask = awe_audioGetPumpMask&g_AWEInstance);
		if (layoutMask > 0) 
		{
			if (layoutMask & 0x1) 
			{
				int ret = awe_audioPump(&g_AWEInstance, 0);
			}
		}
 * @param pAWE						AWE instance
 * @return							bit vector of threads to run, 0 if no threads,  @ref E_AUDIO_NOT_STARTED
 */
INT32 awe_audioGetPumpMask(AWEInstance *pAWE);

/*------------------------------------------Deferred Functions----------------------------------------------------*/
/**
 * @brief Perform deferred awe set on a module. Each call to awe_deferredSetCall processes a single module.
	Usually this function would be called repeatedly until the return value is 0. 
 * @param [in] pAWE					AWE instance pointer
   @return finishedProcessing 		Will return 1 when processing is incomplete, 0 when complete. No error codes.
 */
INT32 awe_deferredSetCall(AWEInstance * pAWE);


/*------------------------------------------Loader Functions----------------------------------------------------*/
/**
* @brief Executes packet commands from an in-memory array. Designer can generate AWB arrays directly from a layout. 
* @param[in] pAWE 			AWE instance pointer
* @param[in] pCommands  	Buffer with commands to execute
* @param[in] arraySize 		Number of DWords in command buffer
* @param[out] pPos			Report failing word index
* @return 					@ref E_SUCCESS
*                           @ref E_EXCEPTION
*                           @ref E_UNEXPECTED_EOF
*                           @ref E_END_OF_FILE
*                           @ref E_MESSAGE_LENGTH_TOO_LONG
*                           @ref E_BADPACKET
*/
INT32 awe_loadAWBfromArray(AWEInstance *pAWE, const UINT32 *pCommands, UINT32 arraySize, UINT32 *pPos);

/**
* @brief Load an AWB file from stdio.
* @param [in]  pAWE 		AWE instance pointer
* @param [in]  binaryFile	AWB file to be loaded
* @param [out] pPos			Report failing word index
* @return					@ref E_SUCCESS
*                           @ref E_EXCEPTION
*                           @ref E_UNEXPECTED_EOF
*                           @ref E_END_OF_FILE
*                           @ref E_MESSAGE_LENGTH_TOO_LONG
*                           @ref E_BADPACKET
*                           @ref E_CANTOPEN
*/
INT32 awe_loadAWBfromFile(AWEInstance *pAWE, const char *binaryFile, UINT32 *pPos);

/**
* @brief Load an AWB file from the flash.
* @param [in]  pAWE 		AWE instance pointer
* @param [in]  binaryFile	AWB file to be loaded
* @param [out] pPos			Report failing word index
* @return					@ref E_SUCCESS
*                           @ref E_EXCEPTION
*                           @ref E_UNEXPECTED_EOF
*                           @ref E_END_OF_FILE
*                           @ref E_MESSAGE_LENGTH_TOO_LONG
*                           @ref E_BADPACKET
*                           @ref E_CANTOPEN
*/
INT32 awe_loadAWBfromFlash(AWEInstance *pAWE, const char *binaryFile, UINT32 *pPos);


/*------------------------------------------Layout Functions----------------------------------------------------*/
/**
 * @brief Returns the number of channels in the Layout's input and output pins.
 * @param [in] pAWE					AWE instance pointer
 * @param [in] pinIdx				which input pin (always 0)
 * @param [out] inCount				channels in primary input pin
 * @param [out] outCount			channels in primary output pin
 */
void awe_layoutGetChannelCount(const AWEInstance *pAWE, UINT32 pinIdx, UINT32 *inCount, UINT32 *outCount);

/**
 * @brief Returns the block size of a pin. Because the block size of the output pin is inherited from the input pin,
	the user only needs to check the input pin.
   @param [in] pAWE				    The AWE Instance
   @param [in] pinIdx				which input pin (always 0)
   @param [out] blockSize			block size of input pin
   @return							@ref E_SUCCESS,  @ref E_NO_LAYOUTS, @ref E_PIN_ID_NOT_FOUND
 */
INT32 awe_layoutGetInputBlockSize(const AWEInstance *pAWE, UINT32 pinIdx, UINT32 *blockSize);

/**
 * @brief Returns the sample rate of an input pin. Because the sample rate of the output pin is inherited from the input pin,
   the user only needs to check the input pin.
   @param [in] pAWE				    The AWE Instance
 * @param [in] pinIdx				which input pin (always 0)
 * @param [out] sampleRate			the input pin sample rate
 * @return							@ref E_SUCCESS,  @ref E_NO_LAYOUTS, @ref E_PIN_ID_NOT_FOUND
 */
INT32 awe_layoutGetInputSampleRate(const AWEInstance *pAWE, UINT32 pinIdx, FLOAT32 *sampleRate);

/**
 * @brief Determines if a layout is loaded and valid.
   @param [in] pAWE				    The AWE Instance
 * @return							0 if layout is not valid/loaded, 1 if layout is valid. No error codes.					
 */
INT32 awe_layoutIsValid(const AWEInstance *pAWE);


/*------------------------------------------Control Interface Functions----------------------------------------------------*/

/**
 * @brief Set a scalar or array value of a module variable by handle
 * @param [in] pAWE						instance pointer
 * @param [in] handle					packed object handle
 * @param [in] value					value(s) to set
 * @param [in] arrayOffset				array index if array
 * @param [in] length					number of elements. 1 if scaler
 * @return								@ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED, 
 *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS   
 */
INT32 awe_ctrlSetValue(const AWEInstance *pAWE, UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length);

/**
 * @brief Get a scalar or array value of a module variable by handle
 * @param [in] pAWE						instance pointer
 * @param [in] handle					packed object handle
 * @param [out] value					value(s) to get
 * @param [in] arrayOffset				array index if array
 * @param [in] length					number of elements. 1 if scaler
 * @return								@ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED, 
 *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS   
 */ 
INT32 awe_ctrlGetValue(const AWEInstance *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length);

/**
 * @brief Set the runtime status of a module. 
 * 0 = Active,    1 = Bypass,    2 = Mute,    3 = Inactive
 * @param [in] pAWE						instance pointer
 * @param [in] handle					packed object handle
 * @param [in] status					status to set
 * @return								@ref E_SUCCESS,  @ref E_NOT_MODULE,  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS 
 */
INT32 awe_ctrlSetStatus(const AWEInstance *pAWE, UINT32 handle, UINT32 status);

/**
 * @brief Get the runtime status of a module.
 * 0 = Active,    1 = Bypass,    2 = Mute,    3 = Inactive
 * @param [in] pAWE						instance pointer
 * @param [in] handle					packed object handle
 * @param [out] status					status to get
 * @return								@ref E_SUCCESS,  @ref E_NOT_MODULE,  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS,  @ref E_PARAMETER_ERROR
 */
INT32 awe_ctrlGetStatus(const AWEInstance *pAWE, UINT32 handle, UINT32 *status);

/**
 * @brief Set a scalar or array value  of a module variable by handle with mask. A mask allows you to only call module's set function
 *      for a single variable.
 * @param [in] pAWE						instance pointer
 * @param [in] handle					packed object handle
 * @param [in] value					value(s) to set
 * @param [in] arrayOffset				array index if array
 * @param [in] length					number of elements if array. 1 if scaler
 * @param [in] mask						mask to use - 0 to not call set function
 * @return								@ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX, 
 *  @ref E_CLASS_NOT_SUPPORTED,  @ref E_OBJECT_ID_NOT_FOUND,  @ref E_NOT_MODULE  
 */
INT32 awe_ctrlSetValueMask(const AWEInstance *pAWE, UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length, UINT32 mask);

/**
 * @brief Get a scalar or array value of a module variable by handle with mask. A mask allows you to only call module's set function
 *      for a single variable.
 * @param [in] pAWE						instance pointer
 * @param [in] handle					packed object handle
 * @param [out] value					value(s) to get
 * @param [in] arrayOffset				array index if array
 * @param [in] length					number of elements if array. 1 if scaler
 * @param [in] mask						mask to use - 0 to not call get function
 * @return								@ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX, 
 *  @ref E_CLASS_NOT_SUPPORTED,  @ref E_OBJECT_ID_NOT_FOUND,  @ref E_NOT_MODULE  
 */
INT32 awe_ctrlGetValueMask(const AWEInstance *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length, UINT32 mask);

/**
 * @brief Get an object class from its handle.
 * @param pAWE						instance pointer
 * @param [in] handle				handle of object to find
 * @param [out] pClassID			pointer to found object class
 * @return							@ref E_SUCCESS,  @ref E_NO_MORE_OBJECTS,  @ref E_LINKEDLIST_CORRUPT
 */
INT32 awe_ctrlGetModuleClass(const AWEInstance *pAWE, UINT32 handle, UINT32 *pClassID);

/**
 * @brief Enable or disable the profiling ability of the AWECore. Both top and module level profiling enabled by default at awe_init. 
 * Use this if you wish to selectively enable or disable per pump profiling during runtime. 
 * Disabling profiling saves a small amount of cycles per pump. User can also enable or disable independently module level profiling and top level profiling.
 * @param pAWE						instance pointer
 * @param status					0 to disable both, 1 to enable both, 2 to enable module level only and 3 to enable top level only
 * @return							@ref E_SUCCESS, @ref E_NOT_OBJECT, @ref E_PARAMETER_ERROR
*/
INT32 awe_setProfilingStatus(AWEInstance *pAWE, UINT32 status);

/**
 * @brief Get the average cycles of a running layout, in units of cycles at profileSpeed. Returns cycles in 24.8 format, 
 * so shift right by 8 bits for integer value. To get CPU cycles, multiply by target cpuSpeed / profileSpeed.
 * If a previous pump is not complete and the layout is ready to pump again, an overflow is detected.
 * When in this state, the awe_getAverageLayoutCycles api will return the averageCycles = AWE_PUMP_OVF_MAX_AVG_CYCLES (0xFFFFFFFF).
 * @param pAWE						instance pointer
 * @param layoutIdx					Layout index (typically 0, except in advanced use cases)
 * @param averageCycles             Pointer the output (average layout cycles)
 * @return							@ref E_SUCCESS,  @ref E_PARAMETER_ERROR,  @ref E_NO_LAYOUTS
*/
INT32 awe_getAverageLayoutCycles(AWEInstance *pAWE, UINT32 layoutIdx, UINT32 * averageCycles);


#ifdef	__cplusplus
}
#endif

#endif	// _AWECOREINSTANCE_H
