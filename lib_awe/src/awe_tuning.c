// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include <xcore/select.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>
#include <stdio.h>
#include <print.h>
#include "awe_xcore_internal.h"
#include "ProxyIDs.h"
#include "AWECoreUtils.h"

#if AWE_DSP_MAX_THREAD_NUM > 5
#error "AWE_DSP_MAX_THREAD_NUM should be 5 or tuning needs to be extended"
#endif

#define AWE_AUDIO_STOP_TO_DESTROY_DELAY_MS  10 // 10ms. (6ms has been tested safe over 5000 iterations)

// TODO: cleanup stacksize
#pragma stackfunction 1024
void awe_tuning_thread(chanend_t c_control_from_host,
                       chanend_t c_control_to_host,
                       chanend_t c_deferred_work[AWE_DSP_MAX_THREAD_NUM]) {
    int input_message_word_count = 0;
    int full_packet_in_words = 0;
    int output_message_word_count = 0;
    int num_words, max_words;
    int deferred_thread_mask = 0;
    int bMoreProcessingRequired = 0;
    SELECT_RES(
        CASE_THEN(c_deferred_work[0],  deferred_0),
        CASE_THEN(c_deferred_work[1],  deferred_1),
        CASE_THEN(c_deferred_work[2],  deferred_2),
        CASE_THEN(c_deferred_work[3],  deferred_3),
        CASE_THEN(c_deferred_work[4],  deferred_4),
        DEFAULT_GUARD_THEN(deferred_thread_mask, deferred_work),
        CASE_THEN(c_control_from_host, control_from_host),
        CASE_THEN(c_control_to_host,   control_to_host)
        )
    {
    deferred_0:
        chanend_check_end_token(c_deferred_work[0]);
        deferred_thread_mask |= 1 << 0;
        continue;
    deferred_1:
        chanend_check_end_token(c_deferred_work[1]);
        deferred_thread_mask |= 1 << 1;
        continue;
    deferred_2:
        chanend_check_end_token(c_deferred_work[2]);
        deferred_thread_mask |= 1 << 2;
        continue;
    deferred_3:
        chanend_check_end_token(c_deferred_work[3]);
        deferred_thread_mask |= 1 << 3;
        continue;
    deferred_4:
        chanend_check_end_token(c_deferred_work[4]);
        deferred_thread_mask |= 1 << 4;
        continue;
    deferred_work:
        bMoreProcessingRequired = awe_deferredSetCall((AWEInstance *)&g_AWEInstance);
        if (!bMoreProcessingRequired) {
            for(int i = 0; i < AWE_DSP_MAX_THREAD_NUM; i++) {
                if ((deferred_thread_mask >> i) & 1) {
                    chanend_out_end_token(c_deferred_work[i]);
                }
            }
            deferred_thread_mask = 0;
        }
        continue;
    control_from_host:
        num_words = chanend_in_word(c_control_from_host);
        for(int i = 0; i < num_words; i++) {
            int x = chanend_in_word(c_control_from_host);
            if (input_message_word_count == 0) {
                full_packet_in_words = x >> 16;
            }
            AWE_Packet_Buffer[input_message_word_count++] = x;
        }
        chanend_out_end_token(c_control_from_host);
        chanend_check_end_token(c_control_from_host);
        if (input_message_word_count >= full_packet_in_words) {
            input_message_word_count = 0;
            awe_packetProcess((AWEInstance *)&g_AWEInstance);
            full_packet_in_words = AWE_Packet_Buffer[0]>>16;
            if (full_packet_in_words > AWE_HID_PACKET_BUFFER_SIZE) {
                full_packet_in_words = AWE_HID_PACKET_BUFFER_SIZE;
            }
            output_message_word_count = 0;
            chanend_out_end_token(c_control_to_host);
        }
        continue;
    control_to_host:
        max_words = chanend_in_word(c_control_to_host);
        chanend_check_end_token(c_control_to_host);
        num_words = full_packet_in_words - output_message_word_count;
        if (num_words > max_words) {
            num_words = max_words;
        }
        chanend_out_word(c_control_to_host, num_words);
        for(int i = 0; i < num_words; i++) {
            int x = AWE_Packet_Buffer[output_message_word_count++];
            chanend_out_word(c_control_to_host, x);
        }
        chanend_out_end_token(c_control_to_host);
        chanend_check_end_token(c_control_to_host);
        if (output_message_word_count < full_packet_in_words) {
            chanend_out_end_token(c_control_to_host);
        }
        continue;
    }
}

// Sends a single packet. Adds CRC on end
void _send_packet_to_awe(chanend_t c_tuning_from_host, const unsigned int payload[], unsigned int num_words){
    chanend_out_word(c_tuning_from_host, num_words + 1); // + crc

    unsigned int crc = 0;
    for(int i = 0; i < num_words; i++) {
        chanend_out_word(c_tuning_from_host, payload[i]);
        // printhexln(payload[i]);
        crc ^= payload[i];
    }
    chanend_out_word(c_tuning_from_host, crc);
    // printhexln(crc);

    chanend_out_end_token(c_tuning_from_host);
    chanend_check_end_token(c_tuning_from_host);
    // printchar('\n');

}

// For debugging AWE tuning commands. Will print packets to and from AWE
#define DEBUG_PACKETS     0

// Sends a concatonated packet consisting of two single packets. Adds CRC on end
void _send_packet_to_awe_dual_array(chanend_t c_tuning_from_host, const unsigned int payload1[], unsigned int num_words1, const unsigned int payload2[], unsigned int num_words2){
    chanend_out_word(c_tuning_from_host, num_words1 + num_words2 + 1); // + crc

    unsigned int crc = 0;
    for(unsigned i = 0; i < num_words1; i++) {
        chanend_out_word(c_tuning_from_host, payload1[i]);
        if(DEBUG_PACKETS) printhexln(payload1[i]);
        crc ^= payload1[i];
    }
    if(DEBUG_PACKETS) printstr("split\n");
    for(unsigned i = 0; i < num_words2; i++) {
        chanend_out_word(c_tuning_from_host, payload2[i]);
        if(DEBUG_PACKETS) printhexln(payload2[i]);
        crc ^= payload2[i];
    }
    chanend_out_word(c_tuning_from_host, crc);
    if(DEBUG_PACKETS) printhexln(crc);

    chanend_out_end_token(c_tuning_from_host);
    chanend_check_end_token(c_tuning_from_host);
}

// Gets a whole packet (icnluding CRC) from awe
unsigned int _get_packet_from_awe(chanend_t c_tuning_to_host, unsigned int packet_buffer[], unsigned max_packet_words){
    chanend_check_end_token(c_tuning_to_host);
    chanend_out_word(c_tuning_to_host, max_packet_words);
    chanend_out_end_token(c_tuning_to_host);
    unsigned num_words = chanend_in_word(c_tuning_to_host);
    if(DEBUG_PACKETS) printintln(num_words);
    for(unsigned i = 0; i < num_words; i++) {
        packet_buffer[i] = chanend_in_word(c_tuning_to_host);
        if(DEBUG_PACKETS) printhexln(packet_buffer[i]);
    }
    chanend_out_end_token(c_tuning_to_host);
    chanend_check_end_token(c_tuning_to_host);

    return num_words;
}

#define NUM_WORDS(packet) (sizeof(packet) / sizeof(packet[0]))
#define PACKET_HEADER(num_words, core_id, command) ( ((num_words + 1) << 16) | ((core_id & 0xff) << 8) | (command & 0xff))
#if DEBUG_PACKETS
#define DEBUG_PRINT_RESPONSE(num_words, response_packet) {for(int i=0; i<num_words_rx; i++) {printstr("rx "); printint(i); printchar(' '); printhexln(response_packet[i]);}}
#else
#define DEBUG_PRINT_RESPONSE(num_words, response_packet) (void)num_words;
#endif

const unsigned coreID = 0;


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
INT32 xawe_ctrlSetValue(const xAWEInstance_t *pAWE, UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length){
    UINT32 payload1[] = {PACKET_HEADER(4 + length, coreID, PFID_SetValueHandle), handle, length, arrayOffset};
    _send_packet_to_awe_dual_array(pAWE->c_tuning_from_host, payload1, NUM_WORDS(payload1), (const unsigned int *)value, length);
    const unsigned response_packet_len = 3;
    unsigned int response_packet[response_packet_len] = {0};
    unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, response_packet, response_packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, response_packet);

    return response_packet[1];
}


/**
 * @brief Get a scalar or array value of a module variable by handle
 * @param [in] pAWE                     instance pointer
 * @param [out] value                   value(s) to get
 * @param [in] arrayOffset              array index if array
 * @param [in] length                   number of elements. 1 if scaler
 * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED,
 *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS
 */
INT32 xawe_ctrlGetValue(const xAWEInstance_t *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length){
    UINT32 payload[] = {PACKET_HEADER(4, coreID, PFID_GetValueHandle), handle, length, arrayOffset};
    _send_packet_to_awe(pAWE->c_tuning_from_host, payload, NUM_WORDS(payload));
    const unsigned response_packet_len = 16;
    unsigned int response_packet[response_packet_len] = {0};
    unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, response_packet, response_packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, response_packet);

    for(int i = 0; i < length; i++){
        unsigned int *ptr = value;
        ptr[i] = response_packet[2 + i];
    }
    return response_packet[1];
}


/**
 * @brief Set the runtime status of a module.
 * 0 = Active,    1 = Bypass,    2 = Mute,    3 = Inactive
 * @param [in] pAWE                     instance pointer
 * @param [in] handle                   packed object handle
 * @param [in] status                   status to set
 * @return                              @ref E_SUCCESS,  @ref E_NOT_MODULE,  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS
 */
INT32 xawe_ctrlSetStatus(const xAWEInstance_t *pAWE, UINT32 handle, UINT32 status){
    UINT32 payload[] = {PACKET_HEADER(3, coreID, PFID_SetStatusHandle), handle, status};
    _send_packet_to_awe(pAWE->c_tuning_from_host, payload, NUM_WORDS(payload));
    const unsigned response_packet_len = 3;
    unsigned int response_packet[response_packet_len] = {0};
    unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, response_packet, response_packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, response_packet);

    return response_packet[1];
}


/**
 * @brief Get the runtime status of a module.
 * 0 = Active,    1 = Bypass,    2 = Mute,    3 = Inactive
 * @param [in] pAWE                     instance pointer
 * @param [in] handle                   packed object handle
 * @param [out] status                  status to get
 * @return                              @ref E_SUCCESS,  @ref E_NOT_MODULE,  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS,  @ref E_PARAMETER_ERROR
 */
INT32 xawe_ctrlGetStatus(const xAWEInstance_t *pAWE, UINT32 handle, UINT32 *status){
    UINT32 payload[] = {PACKET_HEADER(2, coreID, PFID_GetStatusHandle), handle};
    _send_packet_to_awe(pAWE->c_tuning_from_host, payload, NUM_WORDS(payload));
    const unsigned response_packet_len = 3;
    unsigned int response_packet[response_packet_len] = {0};
    unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, response_packet, response_packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, response_packet);
    *status = response_packet[1];

    return ((int)(*status) < 0 ? E_NOT_OBJECT : 0);
}

/**
 * @brief Set a scalar or array value of a module variable by handle with mask. A mask allows you to only call module's set function
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
INT32 xawe_ctrlSetValueMask(const xAWEInstance_t *pAWE, UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length, UINT32 mask){
    UINT32 payload1[] = {PACKET_HEADER(5 + length, coreID, PFID_SetValueHandleMask), handle, length, arrayOffset, mask};
    _send_packet_to_awe_dual_array(pAWE->c_tuning_from_host, payload1, NUM_WORDS(payload1), (const unsigned int *)value, length);
    const unsigned response_packet_len = 3;
    unsigned int response_packet[response_packet_len] = {0};
    unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, response_packet, response_packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, response_packet);

    return response_packet[1];
}

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
INT32 xawe_ctrlGetValueMask(const xAWEInstance_t *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length, UINT32 mask){
    UINT32 payload[] = {PACKET_HEADER(5, coreID, PFID_GetValueHandleMask), handle, length, arrayOffset, mask};
    _send_packet_to_awe(pAWE->c_tuning_from_host, payload, NUM_WORDS(payload));
    const unsigned response_packet_len = 16;
    unsigned int response_packet[response_packet_len] = {0};
    unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, response_packet, response_packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, response_packet);

    for(int i = 0; i < length; i++){
        unsigned int *ptr = value;
        ptr[i] = response_packet[2 + i];
    }

    return response_packet[1];
}


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
INT32 xawe_loadAWBfromArray(xAWEInstance_t *pAWE, const UINT32 *pCommands, UINT32 arraySize, UINT32 *pPos){
    // Zero the position pointer
    *pPos = 0;

    const unsigned response_packet_len = 16;
    unsigned int response_packet[response_packet_len] = {0};

    // Send audio stop command
    const unsigned len = 2; // Whole packet length inc CRC
    const unsigned int stop_audio = (len << 16) + PFID_StopAudio;
    _send_packet_to_awe(pAWE->c_tuning_from_host, &stop_audio, len - 1); // -1 because CRC appended
    unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, response_packet, response_packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, response_packet);
    int err = response_packet[1];
    if(err != E_SUCCESS){
        return err;
    }

    // Required to allow audio to stop before issuing destroy as part of AWB load
    hwtimer_t tmr = hwtimer_alloc();
    hwtimer_delay(tmr, AWE_AUDIO_STOP_TO_DESTROY_DELAY_MS * XS1_TIMER_KHZ);
    hwtimer_free(tmr);

    // Send AWB file commands
    unsigned int cmd_idx = 0;
    while(cmd_idx < arraySize){
        unsigned int *msg_payload = (unsigned int *)&pCommands[cmd_idx];
        if(*msg_payload == PFID_Undefined){
            break;
        }

        unsigned int num_words_tx = PACKET_LENGTH_WORDS(msg_payload);
        *pPos += num_words_tx - 1;
        _send_packet_to_awe(pAWE->c_tuning_from_host, msg_payload, num_words_tx - 1); // -1 because CRC appended
        unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, response_packet, response_packet_len);
        DEBUG_PRINT_RESPONSE(num_words_rx, response_packet);
        if(num_words_rx == 4){
            err = response_packet[2];
        } else {
            err = response_packet[1];
        }
        if(err != E_SUCCESS){
            return err;
        }
        cmd_idx += num_words_tx - 1;
    }

    return E_SUCCESS;
}

/**
* @brief Executes packet commands from a stored file in the FFS. Designer can generate AWB arrays directly from a
* layout and add using AWE server -> Flash menu.
* @param[in] pAWE           AWE instance pointer
* @param[in] fileName       The ASCII filename of the file to be loaded
* @return                   @ref E_SUCCESS
*                           @ref E_INVALID_FILE
*                           @ref E_NOSUCHFILE
*                           @ref E_BADPACKET
*/
INT32 xawe_loadAWBfromFFS(xAWEInstance_t *pAWE, const char *fileName){
    const unsigned packet_len = 16;
    xassert(MAX_FILENAME_LENGTH_IN_DWORDS <= packet_len);
    unsigned int packet[packet_len] = {0};

    // Send audio stop command
    const unsigned len = 2; // Whole packet length inc CRC
    unsigned int stop_audio = (len << 16) + PFID_StopAudio;
    _send_packet_to_awe(pAWE->c_tuning_from_host, &stop_audio, len - 1); // -1 because CRC appended
    unsigned num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, packet, packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, packet);

    int err = packet[1];
    if(err != E_SUCCESS){
        return err;
    }

    // Required to allow audio to stop before issuing destroy as part of AWB load
    hwtimer_t tmr = hwtimer_alloc();
    hwtimer_delay(tmr, AWE_AUDIO_STOP_TO_DESTROY_DELAY_MS * XS1_TIMER_KHZ);
    hwtimer_free(tmr);

    // Check for FFS being enabled
    const unsigned int target_info = (len << 16) + PFID_GetTargetInfo;
    _send_packet_to_awe(pAWE->c_tuning_from_host, &target_info, len - 1); // -1 because CRC appended
    num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, packet, packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, packet);

    int isFlashSupported = (packet[5] & 0b10000) != 0;
    if(!isFlashSupported){
        return E_NOSUCHFILE;
    }

    // Now iterate through file names to find a filename match
    // store commands about to be used
    const unsigned int get_first_file = (len << 16) + PFID_GetFirstFile;
    const unsigned int get_next_file = (len << 16) + PFID_GetNextFile;
    const unsigned int execute_file = (len << 16) + PFID_ExecuteFile;

    // Get first filename
    _send_packet_to_awe(pAWE->c_tuning_from_host, &get_first_file, len - 1); // -1 because CRC appended
    num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, packet, packet_len);
    DEBUG_PRINT_RESPONSE(num_words_rx, packet);
    
    err = packet[1];
    if(err != E_SUCCESS){
        return err;
    }

    // Pointer into the string section of the respone. Only valid directly after get_xxxx_file as we re-use packet for Tx
    char *found_file_name = (char *)&packet[4];

    char last_filename[MAX_FILENAME_LENGTH + 1] = {0};
    strcpy(last_filename, found_file_name);

    while(1){
        // Extract the filename
        int attribute = packet[2];
        
        // Check against desired filename
        if(strcmp(found_file_name, fileName) == 0){
            // Check attribute
            if(attribute != (COMPILED_SCRIPT | COMMAND_SCRIPT)){
                return E_INVALID_FILE;
            }

            // Build command packet to load AWB from FFS
            memset(packet, 0, sizeof(packet)); // always starts with a zero word and is zero padded to a word boundary so zero all
            int char_count = strlen(fileName);
            strcpy((char*)&packet[1], fileName);
            int num_words = (char_count >> 2) + 1; // CMD, string + at least one byte (up to 4) zero padding to a word

            _send_packet_to_awe_dual_array(pAWE->c_tuning_from_host, &execute_file, 1, packet, num_words - 1); // -1 because CRC appended
            num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, packet, packet_len);
            DEBUG_PRINT_RESPONSE(num_words_rx, packet);

            err = packet[1];
            if(err != E_SUCCESS){
                return err;
            }

            return E_SUCCESS;
        }

        // Not found, try again. Get next filename
        _send_packet_to_awe(pAWE->c_tuning_from_host, &get_next_file, len - 1); // -1 because CRC appended
        num_words_rx = _get_packet_from_awe(pAWE->c_tuning_to_host, packet, packet_len);
        DEBUG_PRINT_RESPONSE(num_words_rx, packet);
        
        err = packet[1];
        if(err != E_SUCCESS){
            return err;
        }

        // Same file found again. Out of files in the FFS and desired one not found.
        if(strcmp(found_file_name, last_filename) == 0){
            return E_NOSUCHFILE;
        }

        strcpy(last_filename, found_file_name);
    }

    return E_SUCCESS; // Unreachable but keep compiler happy
}
