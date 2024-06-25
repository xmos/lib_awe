// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef __FFS_RPC_H__
#define __FFS_RPC_H__


#ifdef __XC__
#define chanend_t chanend
#else
#include "awe_xcore_internal.h"
#include "xcore/channel.h"
#include "xcore/parallel.h"
#endif

#include <stdio.h>
#include "StandardDefs.h"


/* ----------------------------------------------------------------------
** Specify flash memory available for flash file system
** ------------------------------------------------------------------- */
#define FLASH_MEMORY_SIZE_IN_BYTES 0x200000
#define ERASEABLE_SECTOR_SIZE 0x1000
#define FILE_SYSTEM_START_OFFSET 0x00000
#define SECTOR_ERASE_TIME_MS    400

#ifndef __XC__
DECLARE_JOB(ffs_server, (chanend_t));
#endif
void ffs_server(chanend_t c_ffs_rpc);

void init_ffs_rpc_client_chanend(chanend_t ffs_rpc_client);
BOOL usrInitFlashFileSystem(void);
BOOL usrReadFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToRead);
BOOL usrWriteFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToWrite);
BOOL usrEraseFlashSector(UINT32 nStartingAddress, UINT32 nNumberOfSectors);


#endif
