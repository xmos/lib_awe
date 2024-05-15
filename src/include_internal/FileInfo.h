/**
*	@addtogroup Framework
*	@{
*/

/****************************************************************************
*
*		Audio Framework
*		---------------
*
****************************************************************************
*	 FileInfo.h
****************************************************************************
*
*	Description:	Compiled in File System
*
*     Copyright:    (c) 2007-2021 DSP Concepts, Inc. All rights reserved.
*                   3235 Kifer Road
*                   Santa Clara, CA 95054
*	
***************************************************************************/

/*
* @file  FileInfo.h
*/

#ifndef _FileInfo_H
#define _FileInfo_H

#include "TargetProcessor.h" // for UINT32

#ifdef __cplusplus
extern "C" {
#endif

#define ALLOCATION_BLOCKSIZE_DWORDS 16
#define MAX_FILENAME_LENGTH 56
#define MAX_FILENAME_LENGTH_IN_DWORDS (MAX_FILENAME_LENGTH >> 2)

// Definitions for file attribute byte
#define LOAD_IMAGE              0x01
#define STARTUP_FILE            0x02
#define DATA_FILE               0x04    // Any file of type "Other"
#define COMPILED_SCRIPT         0x08
#define COMMAND_SCRIPT          0x10
#define PRESET_SCRIPT           0x20
#define COMPILED_PRESET_SCRIPT  0x28
#define LOADER_FILE             0x40
#define FILE_DELETED            0x80

// @brief Flash file system directory entry data structure.
//
// All structure elements are 4 byte words to accommodate SHARC processors
//
// File Info UINT32 layout
// ---------------------------------------------------------
// | Attribute bits | File Data CRC | Block Offset to Data |
// ---------------------------------------------------------
// | 1 byte			| 1 byte		| 2 bytes			   |
// ---------------------------------------------------------
// | bit 31...........................................bit 0|
// ---------------------------------------------------------
//
// The layout for nFileInfo field in a file directory entry is:
// ---------------------------------------------------------
// | Attribute bits |         Block Offset to Data         |
// ---------------------------------------------------------
// | 1 byte			|               3 bytes	     		   |
// ---------------------------------------------------------
// | bit 31...........................................bit 0|
// ---------------------------------------------------------
//
// Note that the file name must fit in one allocation block
// This inherently limits the file name to 55 bytes plus zero terminator byte (56 bytes)

typedef struct _DIRECTORY_ENTRY
{
	UINT32 nFileInfo;
	UINT32 nDataDWordCnt;
	UINT32 nFilename[MAX_FILENAME_LENGTH_IN_DWORDS];

} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;

#ifdef __cplusplus
}
#endif

#endif	// _ FileInfo_H

/**
* @}
*
* End of file.
*/
