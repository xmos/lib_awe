/*******************************************************************************
*
*				AWE Utilities
*				---------------
*
********************************************************************************
*	  AWECoreUtils.h
********************************************************************************
*
*	  Description:	AudioWeaver Utilities and Helper Functions
*
*	  Copyright:	(c) 2007-2021 DSP Concepts, Inc. All rights reserved.
*					3235 Kifer Road
*					Santa Clara, CA 95054
*
*******************************************************************************/

/**
* @file
* @brief The AWECore Helper Functions File
*/

#ifndef AWECOREUTILS_H
#define AWECOREUTILS_H

#include "ProxyIDs.h"
#include "Errors.h"
#include "TargetProcessor.h"

#ifdef	__cplusplus
extern "C" {
#endif

/*------------------ PACKET ROUTING MACROS - MULTI-INSTANCE ONLY  ------------------*/

/* Maximum number of AWECore instances able to be configured on a single target */
#define MAX_PROCESSING_INSTANCES (256)

/**This will determine the length of a packet in words.*/
#define PACKET_LENGTH_WORDS(x) (x[0]>>16)

/**This will determine the length of a packet in bytes.*/
#define PACKET_LENGTH_BYTES(x) ((x[0]>>16) * sizeof(x[0]))

/**This will get the instance ID of a packet. Used by the tuning interface to determine the destination of an incoming packet */
#define PACKET_INSTANCEID(x) (x[0] >> 8) & 0xff

/** Get the OPCODE of the packet. All opcodes are defined in ProxyIDs.h.
Required when doing multi instance to detect the GetInstanceTable PFID and send an instanceTable reply.*/
#define PACKET_OPCODE(x) ((INT32)x[0] & 0xffU)

/** return value that indicates that the end of the AWB has been reached */
#define AWB_DONE 1

/** return value that indicates that the end of the AWB has been reached and another command should be processed */
#define AWB_NOT_DONE 0

/**
@private
State of the Messaging System.
*/
typedef enum
{
    /* --- States for receiving a command --- */

    tMS_Rx_CmdSTX,          /* Receive Command STX */
    tMS_Rx_CmdSeq,          /* Receive Command Sequence ID */
    tMS_Rx_CmdData,         /* Receive Command Data */

    /* --- States for sending a response --- */
    tMS_Tx_RspSTX,          /* Send Response STX */
    tMS_Tx_RspSeq,          /* Send Response Sequence ID */
    tMS_Tx_RspData,         /* Send Response Data */
	tMS_Tx_Done

} tMsgState;

/*-----------------------------SPI/UART HELPER FUNCTIONS------------------------------------- */
/*The following helper functions are currently not working, but they demonstrate how our UART/SPI state machine works*/

/* ----------------------------------------------------------------------------
 *   Types
 * ------------------------------------------------------------------------- */
#define INCOMPLETE_PACKET               0
#define COMPLETE_NEW_PACKET	    		1
#define COMPLETE_REPEATED_PACKET    	2

/**
 @private
 * @brief Utility function for resetting the UART byte processing state machine
 *
 * Note: Call this method if an error occurs during communication.
 *
 */
void tuningResetByteEncoderUART(void);

/**
 @private
 * @brief Utility function for decoding a received encoded byte for byte-by-byte tuning interface.
 * Provide a pointer to the buffer that will be ultimately processed by awe_packetProcess. Internal state
 * variables will keep track of packet indexing, so always pass in start address of packet buffer.
 * The passed in encoded byte 'ch' will be internally decoded and pushed into the passed in packetBuffer.
 * Keep calling this function until INCOMPLETE_PACKET is not returned.
 *
 * Note: The byte-by-byte transmission protocol is designed to increase the robustness of transmitted
 * packets by inserting a start byte (0x02) and an end byte (0x03) to the packets. Doing this also requires
 * that the data in the packets be encoded to avoid the possibility of false start or end bytes within the payload.
 * This encoding, plus the addition of a sequence number in the packet to detect repeat transmissions,
 * means that the total size of the encoded packets will be [(decodedPacketSizeBytes * (5/4)) + 3] bytes long.
 * If the application will reserve space for an entire encoded packet to be available, it must be
 * [MAX_BUFFER_SIZE_WORDS * 5 + 3] bytes. For MAX_BUFFER_SIZE_WORDS = 264, this equals 1323 bytes.
 *
 *
 * @param[out]  *packetBuffer       Receive packet buffer to be filled with decoded data
 * @param[in] ch                    Received encoded byte to be decoded
 * @return              INCOMPLETE_PACKET - message not complete. Continue passing in encoded bytes.
 * @return              COMPLETE_NEW_PACKET - message complete. Valid message now contained in packetBuffer.
 * @return              COMPLETE_REPEATED_PACKET - message complete, but sequence number identical to last packet.
 *                                                  Retransmit last replyBuffer without processing packet.
 * @return              E_BADPACKET - packet end byte recieved, but packet is invalid.
 *
 */
INT32 tuningDecodeByteUART(UINT32 *packetBuffer, UINT8 ch);

/**
 * @private
 * @brief Utility function for encoding reply packet buffer for byte-by-byte tuning interface.
 * Provide pointer to packet buffer that has been filled with the reply packet returned by
 * awe_packetProcess. Internal state variables will keep track of packet indexing, so always
 * pass in start address of packet buffer.
 * The value of ch is updated with an encoded byte each time the function is called. This encoded
 * byte is expected to be transmitted across the tuning interface one at a time. Continue calling
 * the function until it returns COMPLETE_NEW_PACKET, at which point the final encoded byte
 * (always 0x03) must be transmitted to complete the packet.
 *
 * Note: The byte-by-byte transmission protocol is designed to increase the robustness of transmitted
 * packets by inserting a start byte (0x02) and an end byte (0x03) to the packets. Doing this also requires
 * that the data in the packets be encoded to avoid the possibility of false start or end bytes within the payload.
 * This encoding, plus the addition of a sequence number in the packet to detect repeat transmissions,
 * means that the total size of the encoded packets will be [(decodedPacketBytes * (5/4)) + 3] bytes long.
 * If the application will reserve space for an entire encoded packet to be available, it must be
 * [MAX_BUFFER_SIZE_WORDS * 5 + 3] bytes long. For MAX_BUFFER_SIZE_WORDS = 264, this equals 1323 bytes.
 *
 *
 * @param[in]  *packetBuffer       Reply packet buffer of decoded data to transmit
 * @param[out] *ch                 Encoded character to transmit
 * @return              INCOMPLETE_PACKET - message not complete. Transmit ch and call function again.
 * @return              COMPLETE_NEW_PACKET - message complete. Transmit final ch to indicate end of packet.
 */
 INT32 tuningEncodeByteUART(UINT32 *packetBuffer, UINT8 * ch);

/**
 @private
 Insert a word into the packet buffer to build the send message
*/
//BOOL awe_tuningRxCmdWordSPI(AWEInstance *pAWE, UINT32 word);

/**
 @private
 Get the word from packet buffer to be sent as reply message
 */
//BOOL awe_tuningGetReplyWordSPI(AWEInstance *pAWE, UINT32 * word);

/*-----------------------------END SPI/UART HELPER FUNCTIONS------------------------------------- */

/**
 * @brief 							Compute the CheckSum of a generated reply before sending it back to the Server.
 * @param[out] pPacketBuffer		pointer to your instance's packet buffer
 */
void ComputeCheckSumPublic(UINT32 * pPacketBuffer);

/**
 * @brief 							Generate an instance table reply for Server based on the arguments.
 * @param[out] pPacketBuffer		pointer to your instance's packet buffer
 * @param[in] numInstances			number of instances on your system
 * @param[in] pInstanceTable		pointer to your instanceID's table
 */
void GenerateInstanceTableReply(UINT32 * pPacketBuffer, UINT32 numInstances, UINT32 * pInstanceTable);

/**
 * @brief 							Convert audio data from floating point to Fract32 sample by sample
 * @param[in] x						The floating point value that you wish to convert
 * @return							The converted value in fract32 format
 */
INT32 float_to_fract32(FLOAT32 x);

/**
 * @brief 							Convert audio data from fract32 to float sample by sample
 * @param[in] x						The fract32 point value that you wish to convert
 * @return							The converted value as a float
 */
FLOAT32 fract32_to_float(INT32 x);

/**
* @brief Get the next command from an array of AWB commands and write it into a packetBuffer to be processed.
* @param [in]  *pArray            The AWB array
* @param [in]  arraySize         The size of the AWB array
* @param [out] *pErrorOffset      current position in the array (index of command)
* @param [in]  *pPacketBuffer       The packetBuffer to write the next command into.
* @return	AWB_NOT_DONE if processing of array is not complete
* @return 	AWB_DONE if processing of array is complete
* @return	negative number if error encountered
*/
INT32 awe_getNextAWBCmd(const UINT32 *pArray, UINT32 arraySize, UINT32 * pErrorOffset, UINT32 *pPacketBuffer);

#ifdef	__cplusplus
}
#endif

#endif	// AWECOREUTILS_H

/**
 * @}
 * End of file
 */
