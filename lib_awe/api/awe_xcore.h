// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef AWE_XCORE_H
#define AWE_XCORE_H

#ifdef __XC__
#define channend_t chanend
#else
#include <stdint.h>
#include <xcore/channel.h>
#include <xcore/lock.h>
#endif

/**
 * \addtogroup lib_awe lib_awe
 *
 * The public API for using the lib_awe.
 * @{
 */

/** Function that spawns all the child threads. This function does not return.
 * It always spawns at least one tuning thread, one data-transport
 * thread, and one DSP thread. Up to four additional DSP threads are spawned
 * for a maximum of seven threads.
 *
 * The data-transport thread has a channel to each DSP thread in order
 * to start them when data is available. A single channel-end is used to send
 * audio-samples to the data-transport thread and receive samples back.
 *
 * The tuning thread occupies another channel-end, it is used for sending
 * tuning packets to the AWE library. Part packets are sent over this channel
 * and assembled in the tuning thread; when a complete packet has arrived
 * the tuning packet is handled by AWE and a reply is sent back over the same
 * channel-end.
 *
 * You must create a awe_conf.h file to configure the AWE instantiation.
 * This defines the the number of channels that AWE expects
 * (AWE_INPUT_CHANNELS, AWE_OUTPUT_CHANNELS), and the number of threads that
 * should be used for DSP (AWE_DSP_THREAD_NUM).
 *
 * \param c_tuning_from_host
 *                    The channel end over which tuning packets arrive. Over
 *                    this channel you will send a sequence [N, N words, CT_END],
 *                    and then receive an [CT_END] empty message as ack. You do
 *                    this until all data has been sent through.
 * \param c_tuning_to_host
 *                    A [CT_END] empty message will be sent back over c_tuning,
 *                    which you must acknowledge with a word M for the maximum
 *                    packet size you are willing to accept, whereupon you will
 *                    be sent [M words, CT_END] that you must acknowledge with a
 *                    [CT_END]. This is repeated until the reply has been sent
 *                    back.
 *
 * \param c_data      The channelend over which audio is communicated. Either
 *                    use ``awe_offload_data_to_dsp_engine`` or output
 *                    [ AWE_INPUT_CHANNELS words, CT_END] over c_data, then
 *                    input [ AWE_OUTPUT_CHANNELS, CT_END ] from c_data.
 */
extern void awe_xcore_main(chanend_t c_tuning_from_host,
                           chanend_t c_tuning_to_host,
                           chanend_t c_data);

/** Convenience function that pushes an audio frame stored in an array to the
 * AWE stack. This function may be avoided and instead data can be pushed
 * directly onto the channelend.
 *
 * NOTE: THIS DESCRIPTION IS WRONG AT PRESENT
 *
 * \param c_data      Channelend for audio communication
 *
 * \param toAWE       frame of data destined for AWE. This frame should be
 *                    AWE_INPUT_CHANNELS in size
 *
 * \param fromAWE     frame of data from AWE. This array should be
 *                    AWE_OUTPUT_CHANNELS in size and will be filled by the
 *                    function
 */
extern void awe_offload_data_to_dsp_engine(chanend_t c_to_awe,
                                           unsigned toAWE[],
                                           unsigned fromAWE[]);

/**
 * @brief Initialise the client side of the tuning interface which will typically connect to the host.
 * May be on the same or a different tile from AWE.
 * All tuning clients must be on the same tile.
 * \param c_tuning_from_host       Channel end for tuning communication from host side (to AWE)
 * \param c_tuning_from_host       Channel end for tuning communication to host side (from AWE)

 */
void init_awe_tuning_instance(chanend_t c_tuning_from_host,
                               chanend_t c_tuning_to_host);

/** @brief Type definition to make the xawe API reflect the AWE API*/
#define INT32 int
/** @brief Type definition to make the xawe API reflect the AWE API*/
#define UINT32 unsigned int

/**
 * @brief Get a scalar or array value of a module variable by handle
 * @param [in] handle                   packed object handle
 * @param [out] value                   value(s) to get
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements. 1 if scaler
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED,
 *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS
 */
INT32 xawe_ctrlGetValue(UINT32 handle, void *value, INT32 arrayOffset, UINT32 length);

/**
 * @brief Set a scalar or array value of a module variable by handle
 * @param [in] handle                   packed object handle
 * @param [in] value                    value(s) to set
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements. 1 if scaler
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED,
 *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS
 */
INT32 xawe_ctrlSetValue(UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length);

/**
 * @brief Set the runtime status of a module.
 * 0 = Active,    1 = Bypass,    2 = Mute,    3 = Inactive
 * @param [in] handle                   packed object handle
 * @param [in] status                   status to set
 * @return                              @ref E_SUCCESS,  @ref E_NOT_MODULE,  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS
 */
INT32 xawe_ctrlSetStatus(UINT32 handle, UINT32 status);

/**
 * @brief Get the runtime status of a module.
 * 0 = Active,    1 = Bypass,    2 = Mute,    3 = Inactive
 * @param [in] handle                   packed object handle
 * @param [out] status                  status to get
 * @return                              @ref E_SUCCESS,  @ref E_NOT_MODULE,  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS,  @ref E_PARAMETER_ERROR
 */
INT32 xawe_ctrlGetStatus(UINT32 handle, UINT32 *status);

/**
 * @brief Set a scalar or array value  of a module variable by handle with mask. A mask allows you to only call module's set function
 *      for a single variable.
 * @param [in] handle                   packed object handle
 * @param [in] value                    value(s) to set
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements if array. 1 if scaler
 * @param [in] mask                     mask to use - 0 to not call set function
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,
 *  @ref E_CLASS_NOT_SUPPORTED,  @ref E_OBJECT_ID_NOT_FOUND,  @ref E_NOT_MODULE
 */
INT32 xawe_ctrlSetValueMask(UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length, UINT32 mask);

/**
 * @brief Get a scalar or array value of a module variable by handle with mask. A mask allows you to only call module's set function
 *      for a single variable.
 * @param [in] handle                   packed object handle
 * @param [out] value                   value(s) to get
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements if array. 1 if scaler
 * @param [in] mask                     mask to use - 0 to not call get function
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,
 *  @ref E_CLASS_NOT_SUPPORTED,  @ref E_OBJECT_ID_NOT_FOUND,  @ref E_NOT_MODULE
 */
INT32 xawe_ctrlGetValueMask(UINT32 handle, void *value, INT32 arrayOffset, UINT32 length, UINT32 mask);


/**
 * @brief Get the profiling info of the signal processing.
 * Returns cycles in 24.8 format, so shift right by 8 bits for integer value. To get CPU cycles, multiply by target cpuSpeed / profileSpeed.
 * If a previous pump is not complete and the layout is ready to pump again, an overflow is detected.
 * In when in this state, the awe_getAverageLayoutCycles api will return the averageCycles = AWE_PUMP_OVF_MAX_AVG_CYCLES (0xFFFFFFFF).
 * @param [in] average_cycles           Pointer the output (average layout cycles)
 * @return                              @ref E_SUCCESS,
 *                                      @ref E_BADPACKET
 */
INT32 xawe_getAverageLayoutCycles(UINT32 *average_cycles);

/**
 * @brief Get the amount of main heap free.
 * Returns the heap size in 32 bit words.
 * @param [in] heap_free                Pointer the output (heap free in 32 bit words)
 * @return                              @ref E_SUCCESS,
 *                                      @ref E_BADPACKET
 */
INT32 xawe_GetHeapSize(UINT32 *heap_free);

/*------------------------------------------Loader Functions----------------------------------------------------*/
/**
* @brief Executes packet commands from an in-memory array. Designer can generate AWB arrays directly from a layout.
* Effectively this loads an AWB array and checks that it is valid. It automatically destroys any exitsing layout.
* @param[in] pCommands      Buffer with commands to execute
* @param[in] arraySize      Number of DWords in command buffer
* @param[out] pPos          Report failing word index
* @return                   @ref E_SUCCESS
*                           @ref E_EXCEPTION
*                           @ref E_UNEXPECTED_EOF
*                           @ref E_END_OF_FILE
*                           @ref E_MESSAGE_LENGTH_TOO_LONG
*                           @ref E_BADPACKET
*                           @ref E_NO_CORE
*/
INT32 xawe_loadAWBfromArray(const UINT32 *pCommands, UINT32 arraySize, UINT32 *pPos);

/**
* @brief Executes packet commands from a stored file in the FFS. Designer can generate AWB arrays directly from a
* layout and add using AWE server -> Flash menu.
* Effectively this loads an AWB array and checks that it is valid. It automatically destroys any exitsing layout.
* Only available when AWE_USE_FLASH_FILE_SYSTEM is enabled and a valid .awb file has been pre-written into the FFS.
* @param[in] pAWE           AWE instance pointer
* @param[in] fileName       The ASCII filename of the file to be loaded
* @return                   @ref E_SUCCESS
*                           @ref E_INVALID_FILE
*                           @ref E_NOSUCHFILE
*                           @ref E_BADPACKET
*                           @ref E_NO_CORE
*/
INT32 xawe_loadAWBfromFFS(const char *fileName);



/** @brief The maximum number of xcore processor threads supported by lib_awe which is set to 5. Cannot be changed by the user. */
#define AWE_DSP_MAX_THREAD_NUM        5

#if defined(__awe_conf_h_exists__)
#include "awe_conf.h"
#endif

/** @brief The number of xcore threads used by lib_awe. Modifiable by the user per project between 1 and 5. */
#ifndef AWE_DSP_THREAD_NUM
#define AWE_DSP_THREAD_NUM              2
#endif

#if AWE_DSP_THREAD_NUM > AWE_DSP_MAX_THREAD_NUM
#error "AWE_DSP_THREAD_NUM too high, max " ## AWE_DSP_MAX_THREAD_NUM
#endif

/** @brief The amound of heap memory in long words (32 bit) that can be used by lib_awe. Modifiable by the user per project. */
#ifndef AWE_HEAP_SIZE_LONG_WORDS
#error "Please define AWE_HEAP_SIZE_LONG_WORDS to allocate the number of long words (32 bit) for AWE main heap"
#define AWE_HEAP_SIZE_LONG_WORDS // For doxygen only
#endif

/** @brief Enables use of the AWE Flash File System. Note this will consume in the order of 10 kB of memory on the AWE core and a similar amount for the code that handles the low-level flash accesses. */
#ifndef AWE_USE_FLASH_FILE_SYSTEM
#define AWE_USE_FLASH_FILE_SYSTEM   0
#endif

#ifndef AWE_INPUT_CHANNELS
#error "Must define AWE_INPUT_CHANNELS"
#endif

#ifndef AWE_OUTPUT_CHANNELS
#error "Must define AWE_OUTPUT_CHANNELS"
#endif

/** @brief The size of the packet buffer in 32b words used for communicating with AWE over tuning interface. This must be set to 264 normally but may be lowered in certain cases where long commands (tuning and or flash file system) are not used. Please see DSP Concepts documentation for further details. */
#define AWE_HID_PACKET_BUFFER_SIZE 264

/** @brief The number of audio samples per block processed by AWE. Normally set to 32. Please see DSP Concepts documentation for further details. */
#define AWE_BLOCK_SIZE              32

#ifndef FAST_HEAP_A_SIZE
/** @brief The amount of heap in bytes allocated for FAST_HEAP_A usage. Please see DSP Concepts documentation for further details. */
#define FAST_HEAP_A_SIZE          (5*1024)
#endif

#ifndef FAST_HEAP_B_SIZE
/** @brief The amount of heap in bytes allocated for FAST_HEAP_B usage. Please see DSP Concepts documentation for further details. */
#define FAST_HEAP_B_SIZE          (5*1024)
#endif

#ifndef SLOW_HEAP_SIZE
/** @brief The amount of heap in bytes allocated for SLOW_HEAP usage. Please see DSP Concepts documentation for further details. */
#define SLOW_HEAP_SIZE            (5*1024)
#endif

/**@}*/ // END: addtogroup lib_awe

#undef INT32
#undef UINT32

#ifdef __XC__
#undef channend_t
#endif

#endif
