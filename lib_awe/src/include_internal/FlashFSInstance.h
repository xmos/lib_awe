/*******************************************************************************
*
*               Audio Framework
*               ---------------
*
********************************************************************************
*     FlashFSInstance.h
********************************************************************************
*
*     Description:  Prototypes of AudioWeaver Flash File System
*
*     Copyright:    (c) 2007-2021 DSP Concepts, Inc. All rights reserved.
*                   3235 Kifer Road
*                   Santa Clara, CA 95054
*
*******************************************************************************/

/**
* @file 
* @brief The AWEFlashFSInstance API Header File
*/

#ifndef _FLASHFSINSTANCE_H
#define _FLASHFSINSTANCE_H

#include "FileInfo.h"

#ifdef	__cplusplus
extern "C" {
#endif

// Forward decl as AWEInstance and AWEFlashFSInstance reference each other
struct _AWEInstance;

/**
@brief The DSPC Flash File System Instance
@details Optional -- SINGLE INSTANCE ONLY
*/
typedef struct _AWEFlashFSInstance
{
    /** Size of flash memory - if non-zero, next two values must also be non-zero. */
    UINT32 flashSizeInBytes;

    /** Size of flash erase block. */
    UINT32 flashErasableBlockSizeInBytes;

    /** Offset into start of flash used for file system. */
    UINT32 flashStartOffsetInBytes;

    /** Flash erase time in milliseconds */
    UINT32 flashEraseTimeInMs;

    /** User function to initialize flash file system. */
    BOOL (*cbInit)(void);

    /** User callback function to erase one or more sectors. */
    BOOL (*cbEraseSector)(UINT32 nStartingAddress, UINT32 nNumberOfSectors);

    /** User callback function to write to flash. */
    BOOL (*cbFlashWrite)(UINT32 nFlashAddress, UINT32 * pBuffer, UINT32 nDWordsToWrite);

    /** User function to read from flash. */
    BOOL (*cbFlashRead)(UINT32 nFlashAddress, UINT32 * pBuffer, UINT32 nDWordsToRead);

    /** Optional user callback function to return the next command from an awb in flash. Only required for multi-instance BSPs.
    If not defined, awb commands are processed internally in the associated AWEInstance.
    The command to be processed is in pAWE->pPacketBuffer. Parse the instanceID/opcode with AWECoreUtils, and route/process it on the desired instance. */
    INT32 (*cbFlashProcessCmd)(struct _AWEInstance *pAWE);

#ifdef FILESYSTEM_FRAMEWORK
	FILESYSTEM_FRAMEWORK
#else
    /** Reserved member. */
    UINT32 _Reserved[59];
#endif

} AWEFlashFSInstance;

// Macro to indicate API's changed
#define FLASH_FS_API_CHANGED

#ifdef	__cplusplus
}
#endif


#endif	// _FLASHFSINSTANCE_H
