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
#define FILE_SYSTEM_START_OFFSET    0x00000     // Set to start of data partition which is always address 0 from the FFS point fo view
#define SECTOR_ERASE_TIME_MS        0           // The low level erase builds in the delays so this can be zeroed

#ifndef FFS_SERVER_BUFF_SIZE
#define FFS_SERVER_BUFF_SIZE        256         // In 32b words. Can be any non zero size but larger will improve performance slightly but cost 2x this in RAM.
#endif

#ifndef FLASH_SCRATCH_BUFF_BYTES
#define FLASH_SCRATCH_BUFF_BYTES    4096        // Bytes. This is used to support read modify-write operations smaller than a block size. If set too small it will hit an exception in write.
#endif

#ifndef __XC__
DECLARE_JOB(ffs_server, (chanend_t));
#endif

/* Task which handles the low level flash memory operations. It handles commands over a channel
 * end and provides the AWE FFS with access to the flash. It consumes a whole thread but can
 * be exited when non longer needed.
 * Used by lib_awe for the flash file system only.
 *
 * @param[in]  chanend_t  c_ffs_rpc_server - the channel end resource ID on the server side
 */
void ffs_server(chanend_t c_ffs_rpc_server);

/* Function to initialise the channel end use by the FFS RPC. Must be called before 
 * any of the following functions are used.
 * Used by lib_awe for the flash file system only.
 *
 * @param[in]  chanend_t  ffs_rpc_client - the channel end resource ID on the client side
 */
void init_ffs_rpc_client_chanend(chanend_t c_ffs_rpc_client);

/* Callback to initialize Flash device via RPC to ffs_server on different tile.
 * Used by lib_awe for the flash file system only.
 *
 * @retval     TRUE  - Initialization is success
 * @retval     FALSE - Initialization failed
 */
BOOL usrInitFlashFileSystem(void);

/** Read 4-byte words from flash memory
 * Used by lib_awe for the flash file system only.
 *
 * @param[in]  UINT32  nAddress - address in flash to start reading (byte address)
 * @param[in]  UINT32 *pBuffer  - buffer to read into
 * @param[in]  UINT32  nDWordsToRead - number of 4-bytes elements to read
 *
 * @retval     TRUE  - read succeeded
 * @retval     FALSE - read failed
 */
BOOL usrReadFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToRead);

/** Write 4-byte words to flash memory
 * Used by lib_awe for the flash file system only.
 *
 * @param[in]  UINT32  nAddress - address in flash to start writing (byte address)
 * @param[in]  UINT32 * pBuffer  - buffer to write into
 * @param[in]  UINT32  nDWordsToWrite - number of 4-bytes elements to write
 *
 * @retval     TRUE  - write succeeded
 * @retval     FALSE - write failed
 */
BOOL usrWriteFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToWrite);

/** Erase flash memory starting at address for number of sectors
 * Used by lib_awe for the flash file system only.
 *
 * @param[in]  UINT32  nStartingAddress - address in flash to start erasing (byte address)
 * @param[in]  UINT32  nNumberOfSectors  - number of flash memory sectors to erase
 *
 * @retval     TRUE  - erase succeeded
 * @retval     FALSE - erase failed
 */
BOOL usrEraseFlashSector(UINT32 nStartingAddress, UINT32 nNumberOfSectors);

/** Find the data partition size and erase sector size in bytes 
 * Used by lib_awe for the flash file system only.
 *
 * @param[out]  UINT32 * dp_size_bytes - the data partition size in bytes
 * @param[out]  UINT32 * sector_size_bytes
 *
 */
void ffs_rpc_get_flash_info(UINT32 *dp_size_bytes, UINT32 *sector_size_bytes);

/** Exit the FFS server thread. Can be used to dynamically free up resources at runtime
 * if the FFS server is no longer needed, for example, after loading an AWB image.
 * You will need to re-spawn ffs_server() if any flash commands are needed after that.
 * Used by lib_awe for the flash file system only.
 *
 * @retval     TRUE  - exit succeeded
 * @retval     FALSE - exit failed
 */
BOOL ffs_rpc_exit_server(void);

#endif
