// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>
#include <stdio.h>
#include <print.h>
#include <quadflashlib.h>

#include "awe_xcore_internal.h"


// Ports for QuadSPI access on explorerKIT.
fl_QSPIPorts ports = {
  PORT_SQI_CS,
  PORT_SQI_SCLK,
  PORT_SQI_SIO,
  XS1_CLKBLK_1
};

///////SERVER SIDE STUFF/////
#define FFS_SERVER_BUFF_SIZE        256 // 32b words
#define FLASH_SCRATCH_BUFF_BYTES    4096 // Bytes
enum {
    FFS_RPC_INIT = 0,
    FFS_RPC_READ,
    FFS_RPC_WRITE,
    FFS_RPC_ERASE,
};

#define DEBUG       0

#if     DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

void ffs_server(chanend_t c_ffs_rpc){
    dprintf("ffs_server\n");
    uint32_t flash_buffer[FFS_SERVER_BUFF_SIZE];

    while(1){
        int ret = 0;
        uint32_t cmd = chan_in_word(c_ffs_rpc);
        switch(cmd){
            case FFS_RPC_INIT:
                dprintf("fl_connect\n");
                ret = fl_connect(&ports);
                chan_out_word(c_ffs_rpc, ret == 0);

                dprintf("0x%x\n", fl_getDataPartitionSize());
                dprintf("0x%u\n", fl_getDataSectorSize(0));
            break;

            case FFS_RPC_READ:
                (void) cmd; // Avoid compiler bug
                UINT32 nAddress = chan_in_word(c_ffs_rpc);
                UINT32 nDWordsToRead = chan_in_word(c_ffs_rpc);
                dprintf("FFS_RPC_READ: 0x%x, 0x%x\n", nAddress, nDWordsToRead);

                ret = 0;
                for(int i = 0; i < nDWordsToRead; i += FFS_SERVER_BUFF_SIZE){
                    uint32_t addr = nAddress + i * sizeof(uint32_t);
                    uint32_t chunk_size_words = (nDWordsToRead - i) > FFS_SERVER_BUFF_SIZE ? FFS_SERVER_BUFF_SIZE :  (nDWordsToRead - i);
                    dprintf("fl_readData: 0x%lx, 0x%lu, 0x%x\n", addr, chunk_size_words * sizeof(uint32_t), (UINT32)flash_buffer);
                    ret |= fl_readData(addr, chunk_size_words * sizeof(uint32_t), (uint8_t *)flash_buffer);
                    chan_out_word(c_ffs_rpc, chunk_size_words);
                    chan_out_buf_word(c_ffs_rpc, flash_buffer, chunk_size_words);                    
                }
                chan_out_word(c_ffs_rpc, 0);
                chan_out_word(c_ffs_rpc, ret == 0);
            break;

            case FFS_RPC_WRITE:
                (void) cmd; // Avoid compiler bug
                nAddress = chan_in_word(c_ffs_rpc);
                UINT32 nDWordsToWrite = chan_in_word(c_ffs_rpc);
                dprintf("FFS_RPC_WRITE: 0x%x, 0x%x\n", nAddress, nDWordsToWrite);

                uint8_t write_sratch_buffer[FLASH_SCRATCH_BUFF_BYTES];

                size_t min_scratch_size = fl_getWriteScratchSize(nAddress, nDWordsToWrite * sizeof(uint32_t)); 
                // printintln(min_scratch_size);
                // printintln(sizeof(write_sratch_buffer));

                xassert(min_scratch_size <= sizeof(write_sratch_buffer));

                int ret = 0;
                for(int i = 0; i < nDWordsToWrite; i += FFS_SERVER_BUFF_SIZE){
                    uint32_t addr = nAddress + i * sizeof(uint32_t);
                    uint32_t chunk_size_words = (nDWordsToWrite - i) > FFS_SERVER_BUFF_SIZE ? FFS_SERVER_BUFF_SIZE :  (nDWordsToWrite - i);
                    chan_out_word(c_ffs_rpc, chunk_size_words);
                    chan_in_buf_word(c_ffs_rpc, flash_buffer, chunk_size_words);
                    dprintf("fl_writeData: 0x%lx, 0x%lu, 0x%x 0x%x\n", addr, chunk_size_words * sizeof(uint32_t), (unsigned)flash_buffer, (unsigned)write_sratch_buffer);
                    ret |= fl_writeData(addr, chunk_size_words * sizeof(uint32_t), (const unsigned char *)flash_buffer, write_sratch_buffer);
                }
                chan_out_word(c_ffs_rpc, ret == 0);
            break;

            case FFS_RPC_ERASE:
                (void) cmd; // Avoid compiler bug
                UINT32 nStartingAddress = chan_in_word(c_ffs_rpc);
                UINT32 nNumberOfSectors = chan_in_word(c_ffs_rpc);

                (void)nNumberOfSectors;
                dprintf("FFS_RPC_ERASE: 0x%x, %u\n", nStartingAddress, nNumberOfSectors);

                ret = 0;
                size_t num_sectors = fl_getNumDataSectors();

                // work out which sectors need to be erased
                unsigned sector_base_addr = 0;
                unsigned curr_sector = 0;
                int start_sector_found = 0;
                while(curr_sector < num_sectors){
                    uint32_t curr_sector_size = fl_getDataSectorSize(curr_sector);
                    if(nStartingAddress >= sector_base_addr && nStartingAddress < sector_base_addr + curr_sector_size){
                        start_sector_found = 1;
                        break;
                    }
                    curr_sector++;
                }
                // Erase the sectors
                if(start_sector_found){
                    for(unsigned sector = curr_sector; sector < curr_sector + nNumberOfSectors; sector++){
                        ret |= fl_eraseDataSector(sector);
                    }
                } else {
                    xassert(0); // We didn't get an address hit
                }

                chan_out_word(c_ffs_rpc, ret == 0);
            break;
        }
    }
}

// Global channel end so we don't need to pass it in to these callbacks
chanend_t g_ffs_rpc_client = 0;

void init_ffs_rpc_client_chanend(chanend_t ffs_rpc_client){
    g_ffs_rpc_client = ffs_rpc_client;
}

BOOL usrInitFlashFileSystem(void)
{
    xassert(g_ffs_rpc_client != 0); // Ensure we have a channel connection
#if DEBUG
    printchar('.');
#endif
    chan_out_word(g_ffs_rpc_client, FFS_RPC_INIT);
    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
} 

BOOL usrReadFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToRead)
{
#if DEBUG
    printchar('@');
#endif
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
    chan_in_word(g_ffs_rpc_client); // Needed?

    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
}


BOOL usrWriteFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToWrite)
{
#if DEBUG
    printchar('+');
#endif
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
#if DEBUG
    printchar('-');
#endif
    chan_out_word(g_ffs_rpc_client, FFS_RPC_ERASE);
    chan_out_word(g_ffs_rpc_client, nStartingAddress);
    chan_out_word(g_ffs_rpc_client, nNumberOfSectors);

    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
}
