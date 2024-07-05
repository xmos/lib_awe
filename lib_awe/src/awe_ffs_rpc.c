// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>
#include <stdio.h>
#include <print.h>
#include <quadflashlib.h>

#if AWE_USE_FLASH_FILE_SYSTEM

#define DEBUG_PRINT_ENABLEAWE_FFS_SERVER 0
#define DEBUG_UNIT AWE_FFS_SERVER
#include "debug_print.h"

#include "awe_xcore_internal.h"
#include "awe_ffs_rpc.h"

// Ports for QuadSPI access
fl_QSPIPorts ports = {
  PORT_SQI_CS,
  PORT_SQI_SCLK,
  PORT_SQI_SIO,
  XS1_CLKBLK_1
};

/* These are the commands for the FFS RPC */
enum {
    FFS_RPC_INIT = 0,
    FFS_RPC_READ,
    FFS_RPC_WRITE,
    FFS_RPC_ERASE,
    FFS_RPC_GET_FLASH_INFO,
    FFS_RPC_EXIT
};


void ffs_server(chanend_t c_ffs_rpc_server){
    debug_printf("ffs_server\n");
    uint32_t flash_buffer[FFS_SERVER_BUFF_SIZE];
    char flash_initialised = 0;
    char ffs_server_active = 1;

    while(ffs_server_active){
        int ret = 0;
        uint32_t cmd = chan_in_word(c_ffs_rpc_server);
        switch(cmd){
            case FFS_RPC_INIT:
                debug_printf("FFS_RPC_INIT\n");
                if(!flash_initialised){
                    ret = fl_connect(&ports);
                } else {
                    ret = 0;
                }
                chan_out_word(c_ffs_rpc_server, ret == 0);
                if(ret == 0){
                    flash_initialised = 1;
                }
            break;

            case FFS_RPC_GET_FLASH_INFO:
                xassert(flash_initialised);
                debug_printf("FFS_RPC_GET_FLASH_INFO\n");

                UINT32 dp_size = fl_getDataPartitionSize();
                UINT32 sector_size = fl_getDataSectorSize(0); // Makes assumption that sectors all the same size. AWE FFS also does this
                chan_out_word(c_ffs_rpc_server, dp_size);
                chan_out_word(c_ffs_rpc_server, sector_size);
            break;

            case FFS_RPC_READ:
                (void) cmd; // http://bugzilla/show_bug.cgi?id=18883
                xassert(flash_initialised);
                UINT32 nAddress = chan_in_word(c_ffs_rpc_server);
                UINT32 nDWordsToRead = chan_in_word(c_ffs_rpc_server);
                debug_printf("FFS_RPC_READ: 0x%x, 0x%x\n", nAddress, nDWordsToRead);

                ret = 0;
                for(int i = 0; i < nDWordsToRead; i += FFS_SERVER_BUFF_SIZE){
                    uint32_t addr = nAddress + i * sizeof(uint32_t);
                    uint32_t chunk_size_words = (nDWordsToRead - i) > FFS_SERVER_BUFF_SIZE ? FFS_SERVER_BUFF_SIZE :  (nDWordsToRead - i);
                    debug_printf("fl_readData: 0x%lx, 0x%lx, 0x%x\n", addr, chunk_size_words * sizeof(uint32_t), (UINT32)flash_buffer);
                    ret |= fl_readData(addr, chunk_size_words * sizeof(uint32_t), (uint8_t *)flash_buffer);
                    chan_out_word(c_ffs_rpc_server, chunk_size_words);
                    chan_out_buf_word(c_ffs_rpc_server, flash_buffer, chunk_size_words);                    
                }
                chan_out_word(c_ffs_rpc_server, 0);
                chan_out_word(c_ffs_rpc_server, ret == 0);
            break;

            case FFS_RPC_WRITE:
                (void) cmd; // http://bugzilla/show_bug.cgi?id=18883
                xassert(flash_initialised);
                nAddress = chan_in_word(c_ffs_rpc_server);
                UINT32 nDWordsToWrite = chan_in_word(c_ffs_rpc_server);
                debug_printf("FFS_RPC_WRITE: 0x%x, 0x%x\n", nAddress, nDWordsToWrite);

                uint8_t write_sratch_buffer[FLASH_SCRATCH_BUFF_BYTES];
                size_t min_scratch_size = fl_getWriteScratchSize(nAddress, nDWordsToWrite * sizeof(uint32_t)); 
                (void)min_scratch_size;
                xassert(min_scratch_size <= sizeof(write_sratch_buffer)); // If you hit this please increase FLASH_SCRATCH_BUFF_BYTES

                int ret = 0;
                for(int i = 0; i < nDWordsToWrite; i += FFS_SERVER_BUFF_SIZE){
                    uint32_t addr = nAddress + i * sizeof(uint32_t);
                    uint32_t chunk_size_words = (nDWordsToWrite - i) > FFS_SERVER_BUFF_SIZE ? FFS_SERVER_BUFF_SIZE :  (nDWordsToWrite - i);
                    chan_out_word(c_ffs_rpc_server, chunk_size_words);
                    chan_in_buf_word(c_ffs_rpc_server, flash_buffer, chunk_size_words);
                    debug_printf("fl_writeData: 0x%lx, 0x%lx, 0x%x 0x%x\n", addr, chunk_size_words * sizeof(uint32_t), (unsigned)flash_buffer, (unsigned)write_sratch_buffer);
                    ret |= fl_writeData(addr, chunk_size_words * sizeof(uint32_t), (const unsigned char *)flash_buffer, write_sratch_buffer);
                }
                chan_out_word(c_ffs_rpc_server, ret == 0);
            break;

            case FFS_RPC_ERASE:
                (void) cmd; // http://bugzilla/show_bug.cgi?id=18883
                xassert(flash_initialised);
                UINT32 nStartingAddress = chan_in_word(c_ffs_rpc_server);
                UINT32 nNumberOfSectors = chan_in_word(c_ffs_rpc_server);

                (void)nNumberOfSectors;
                debug_printf("FFS_RPC_ERASE: 0x%x, %u\n", nStartingAddress, nNumberOfSectors);

                ret = 0;
                size_t num_sectors = fl_getNumDataSectors();

                // work out which sectors need to be erased
                unsigned sector_base_addr = 0;
                unsigned curr_sector = 0;
                int start_sector_found = 0;
                while(curr_sector < num_sectors){
                    uint32_t curr_sector_size = fl_getDataSectorSize(curr_sector);
                    debug_printf("Found sector: %u, size %lu\n", curr_sector, curr_sector_size);

                    if(nStartingAddress >= sector_base_addr && nStartingAddress < sector_base_addr + curr_sector_size){
                        start_sector_found = 1;
                        break;
                    }
                    curr_sector++;
                    sector_base_addr += curr_sector_size;
                }
                // Erase the sectors
                if(start_sector_found){
                    for(unsigned sector = curr_sector; sector < curr_sector + nNumberOfSectors; sector++){
                        debug_printf("Erasing sector: %u\n", sector);
                        ret |= fl_eraseDataSector(sector);
                    }
                } else {
                    xassert(0); // We didn't get an address hit
                }

                chan_out_word(c_ffs_rpc_server, ret == 0);
            break;

            case FFS_RPC_EXIT:
                (void) cmd; // http://bugzilla/show_bug.cgi?id=18883
                flash_initialised = 0;
                ret = fl_disconnect();
                chan_out_word(c_ffs_rpc_server, ret == 0);
                ffs_server_active = 0;
            break;
        } // switch
    } // while ffs_server_active
}

// Global channel end so we don't need to pass this var in to the callbacks
chanend_t g_ffs_rpc_client = 0;

void init_ffs_rpc_client_chanend(chanend_t c_ffs_rpc_client){
    g_ffs_rpc_client = c_ffs_rpc_client;
}


BOOL usrInitFlashFileSystem(void)
{
    xassert(g_ffs_rpc_client != 0); // Ensure we have a channel connection. If you hit this then you need to call init_ffs_rpc_client_chanend() on the client side at startup first.
    chan_out_word(g_ffs_rpc_client, FFS_RPC_INIT);
    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
}


BOOL usrReadFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToRead)
{
    chan_out_word(g_ffs_rpc_client, FFS_RPC_READ);
    chan_out_word(g_ffs_rpc_client, nAddress);
    chan_out_word(g_ffs_rpc_client, nDWordsToRead);

    uint32_t total_words_read = 0;
    while(total_words_read < nDWordsToRead){
        uint32_t chunk_size_words = chan_in_word(g_ffs_rpc_client);
        UINT32 *addr = pBuffer + total_words_read;
        chan_in_buf_word(g_ffs_rpc_client, (uint32_t *)addr, chunk_size_words);  
        total_words_read += chunk_size_words;                  
    }
    chan_in_word(g_ffs_rpc_client);

    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
}


BOOL usrWriteFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToWrite)
{
    chan_out_word(g_ffs_rpc_client, FFS_RPC_WRITE);
    chan_out_word(g_ffs_rpc_client, nAddress);
    chan_out_word(g_ffs_rpc_client, nDWordsToWrite);

    uint32_t total_words_written = 0;
    while(total_words_written < nDWordsToWrite){
        uint32_t chunk_size_words = chan_in_word(g_ffs_rpc_client);
        UINT32 *addr = pBuffer + total_words_written;
        chan_out_buf_word(g_ffs_rpc_client, (const uint32_t *)addr, chunk_size_words);  
        total_words_written += chunk_size_words;                  
    }

    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;

}


BOOL usrEraseFlashSector(UINT32 nStartingAddress, UINT32 nNumberOfSectors)
{
    chan_out_word(g_ffs_rpc_client, FFS_RPC_ERASE);
    chan_out_word(g_ffs_rpc_client, nStartingAddress);
    chan_out_word(g_ffs_rpc_client, nNumberOfSectors);

    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
}


void ffs_rpc_get_flash_info(UINT32 *dp_size_bytes, UINT32 *sector_size_bytes){
    chan_out_word(g_ffs_rpc_client, FFS_RPC_GET_FLASH_INFO);
    *dp_size_bytes = chan_in_word(g_ffs_rpc_client);
    *sector_size_bytes = chan_in_word(g_ffs_rpc_client);
}


BOOL ffs_rpc_exit_server(void)
{
    chan_out_word(g_ffs_rpc_client, FFS_RPC_EXIT);
    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
} 

#endif // AWE_USE_FLASH_FILE_SYSTEM
