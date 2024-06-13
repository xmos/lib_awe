// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef AWE_XCORE_H
#define AWE_XCORE_H

#include <stdint.h>
#include <xcore/channel.h>

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
 * This defines the number of channels in the audio stack (AUDIO_INPUT_CHANNELS,
 * AUDIO_OUTPUT_CHANNELS), the number of channels that AWE expects
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
 *                    [ AUDIO_INPUT_CHANNELS words, CT_END] over c_data, then
 *                    input [ AUDIO_OUTPUT_CHANNELS, CT_END ] from c_data.
 */
extern void awe_xcore_main(chanend_t c_control_from_host,
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
 *                    AUDIO_OUTPUT_CHANNELS in size
 *
 * \param fromAWE     frame of data from AWE. This array should be
 *                    AUDIO_INPUT_CHANNELS in size and will be filled by the
 *                    function
 */
extern void awe_offload_data_to_dsp_engine(chanend_t c_to_awe,
                                           unsigned toAWE[],
                                           unsigned fromAWE[]);


/** 
@brief The XMOS AWE instance. 
@details For XMOS this is an array of channel ends which represent
the public API to the XMOS AWE instance.
*/
typedef struct xAWEInstance_t{
    chanend_t c_tuning_from_host;
    chanend_t c_tuning_to_host;
} xAWEInstance_t;

/** @brief Type definition to make the xawe API reflect the AWE API*/
#define INT32 int
/** @brief Type definition to make the xawe API reflect the AWE API*/
#define UINT32 unsigned int

/**
 * @brief Get a scalar or array value of a module variable by handle
 * @param [in] pAWE                     instance pointer
 * @param [in] handle                   packed object handle
 * @param [out] value                   value(s) to get
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements. 1 if scaler
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED, 
 *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS   
 */ 
INT32 xawe_ctrlGetValue(const xAWEInstance_t *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length);

/**
 * @brief Set a scalar or array value of a module variable by handle
 * @param [in] pAWE                     instance pointer
 * @param [in] handle                   packed object handle
 * @param [in] value                    value(s) to set
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements. 1 if scaler
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED, 
 *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS   
 */
INT32 xawe_ctrlSetValue(const xAWEInstance_t *pAWE, UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length);

/**
 * @brief Set the runtime status of a module. 
 * 0 = Active,    1 = Bypass,    2 = Mute,    3 = Inactive
 * @param [in] pAWE                     instance pointer
 * @param [in] handle                   packed object handle
 * @param [in] status                   status to set
 * @return                              @ref E_SUCCESS,  @ref E_NOT_MODULE,  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS 
 */
INT32 xawe_ctrlSetStatus(const xAWEInstance_t *pAWE, UINT32 handle, UINT32 status);

/**
 * @brief Get the runtime status of a module.
 * 0 = Active,    1 = Bypass,    2 = Mute,    3 = Inactive
 * @param [in] pAWE                     instance pointer
 * @param [in] handle                   packed object handle
 * @param [out] status                  status to get
 * @return                              @ref E_SUCCESS,  @ref E_NOT_MODULE,  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS,  @ref E_PARAMETER_ERROR
 */
INT32 xawe_ctrlGetStatus(const xAWEInstance_t *pAWE, UINT32 handle, UINT32 *status);

/**
 * @brief Set a scalar or array value  of a module variable by handle with mask. A mask allows you to only call module's set function
 *      for a single variable.
 * @param [in] pAWE                     instance pointer
 * @param [in] handle                   packed object handle
 * @param [in] value                    value(s) to set
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements if array. 1 if scaler
 * @param [in] mask                     mask to use - 0 to not call set function
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX, 
 *  @ref E_CLASS_NOT_SUPPORTED,  @ref E_OBJECT_ID_NOT_FOUND,  @ref E_NOT_MODULE  
 */
INT32 xawe_ctrlSetValueMask(const xAWEInstance_t *pAWE, UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length, UINT32 mask);

/**
 * @brief Get a scalar or array value of a module variable by handle with mask. A mask allows you to only call module's set function
 *      for a single variable.
 * @param [in] pAWE                     instance pointer
 * @param [in] handle                   packed object handle
 * @param [out] value                   value(s) to get
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements if array. 1 if scaler
 * @param [in] mask                     mask to use - 0 to not call get function
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX, 
 *  @ref E_CLASS_NOT_SUPPORTED,  @ref E_OBJECT_ID_NOT_FOUND,  @ref E_NOT_MODULE  
 */
INT32 xawe_ctrlGetValueMask(const xAWEInstance_t *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length, UINT32 mask);


/*------------------------------------------Loader Functions----------------------------------------------------*/
/**
* @brief Executes packet commands from an in-memory array. Designer can generate AWB arrays directly from a layout. 
* @param[in] pAWE           AWE instance pointer
* @param[in] pCommands      Buffer with commands to execute
* @param[in] arraySize      Number of DWords in command buffer
* @param[out] pPos          Report failing word index
* @return                   @ref E_SUCCESS
*                           @ref E_EXCEPTION
*                           @ref E_UNEXPECTED_EOF
*                           @ref E_END_OF_FILE
*                           @ref E_MESSAGE_LENGTH_TOO_LONG
*                           @ref E_BADPACKET
*/
INT32 xawe_loadAWBfromArray(xAWEInstance_t *pAWE, const UINT32 *pCommands, UINT32 arraySize, UINT32 *pPos);


/** @brief The maximum number of xcore processor threads supported by lib_awe. Cannot be changed by the user. */
#define AWE_DSP_MAX_THREAD_NUM        5


#if defined(__awe_conf_h_exists__)
#include "awe_conf.h"
#endif

/** @brief The number of xcore threads used by lib_awe. Modifiable by the user per project. */
#ifndef AWE_DSP_THREAD_NUM
#define AWE_DSP_THREAD_NUM              2
#endif

#if AWE_DSP_THREAD_NUM > AWE_DSP_MAX_THREAD_NUM
#error "AWE_DSP_THREAD_NUM too high, max " ## AWE_DSP_MAX_THREAD_NUM
#endif

#ifndef AWE_INPUT_CHANNELS
#error "Must define AWE_INPUT_CHANNELS"
#endif

#ifndef AWE_OUTPUT_CHANNELS
#error "Must define AWE_OUTPUT_CHANNELS"
#endif

#ifndef AUDIO_INPUT_CHANNELS
#error "Must define AUDIO_INPUT_CHANNELS"
#endif

#ifndef AUDIO_OUTPUT_CHANNELS
#error "Must define AUDIO_OUTPUT_CHANNELS"
#endif

/** @brief The size of the packet buffer used for USB/HID transfer. */
#define AWE_HID_PACKET_BUFFER_SIZE 264

/** @brief The number of audio samples per block processed by AWE. */
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

#endif
