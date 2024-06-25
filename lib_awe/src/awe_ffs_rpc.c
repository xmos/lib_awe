// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xcore/channel.h>
#include <xcore/hwtimer.h>
#include <stdio.h>
#include <quadflashlib.h>

#include "assert.h"
#include "awe_xcore_internal.h"
#include "StandardDefs.h"


// Ports for QuadSPI access on explorerKIT.
fl_QSPIPorts ports = {
  PORT_SQI_CS,
  PORT_SQI_SCLK,
  PORT_SQI_SIO,
  XS1_CLKBLK_1
};

///////SERVER SIDE STUFF/////
#define FFS_SERVER_BUFF_SIZE        256 // 32b words
#define FLASH_SCRATCH_BUFF_BYTES    1024 // Bytes
enum {
    FFS_RPC_INIT = 0,
    FFS_RPC_READ,
    FFS_RPC_WRITE,
    FFS_RPC_ERASE,
};

#define DEBUG       0 // Prints for debugging. Turn off for actual test

#if     DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

void ffs_server(chanend_t c_ffs_rpc){
    while(1){
        int ret = 0;
        uint32_t cmd = chan_in_word(c_ffs_rpc);
        switch(cmd){
            case FFS_RPC_INIT:
                dprintf("fl_connect\n");
                ret = fl_connect(&ports);
                chan_out_word(c_ffs_rpc, ret == 0);                
            break;
            case FFS_RPC_READ:
                (void) cmd; // Avoid compiler bug
                UINT32 nAddress = chan_in_word(c_ffs_rpc);
                UINT32 * pBuffer = (UINT32 *)chan_in_word(c_ffs_rpc);
                UINT32 nDWordsToRead = chan_in_word(c_ffs_rpc);

                uint32_t flash_read_buffer[FFS_SERVER_BUFF_SIZE];
                ret = 0;
                for(int i = 0; i < nDWordsToRead; i += FFS_SERVER_BUFF_SIZE){
                    uint32_t addr = nAddress + i * sizeof(uint32_t);
                    uint32_t chunk_size_words = (nDWordsToRead - i) > FFS_SERVER_BUFF_SIZE ? FFS_SERVER_BUFF_SIZE :  (nDWordsToRead - i);
                    dprintf("fl_readData: 0x%x, 0x%u, 0x%x\n", addr, chunk_size_words * sizeof(uint32_t), flash_read_buffer);
                    // ret |= fl_readData(addr, chunk_size_words * sizeof(uint32_t), (uint8_t *)flash_read_buffer);
                    chan_out_word(c_ffs_rpc, chunk_size_words);
                    chan_out_buf_word(c_ffs_rpc, flash_read_buffer, chunk_size_words);                    
                }
                chan_out_word(c_ffs_rpc, 0);
                chan_out_word(c_ffs_rpc, ret == 0);
            break;
            case FFS_RPC_WRITE:
                (void) cmd; // Avoid compiler bug
                nAddress = chan_in_word(c_ffs_rpc);
                pBuffer = (UINT32 *)chan_in_word(c_ffs_rpc);
                UINT32 nDWordsToWrite = chan_in_word(c_ffs_rpc);

                uint32_t flash_write_buffer[FFS_SERVER_BUFF_SIZE];
                uint8_t write_sratch_buffer[FLASH_SCRATCH_BUFF_BYTES];

                size_t min_scratch_size = fl_getWriteScratchSize(nAddress, nDWordsToWrite * sizeof(uint32_t)); 
                assert( min_scratch_size >= sizeof(write_sratch_buffer));

                int ret = 0;
                for(int i = 0; i < nDWordsToWrite; i += FFS_SERVER_BUFF_SIZE){
                    uint32_t addr = nAddress + i * sizeof(uint32_t);
                    uint32_t chunk_size_words = (nDWordsToWrite - i) > FFS_SERVER_BUFF_SIZE ? FFS_SERVER_BUFF_SIZE :  (nDWordsToWrite - i);
                    // chan_out_word(chunk_size_words)
                    chan_in_buf_word(c_ffs_rpc, flash_write_buffer, chunk_size_words);
                    dprintf("fl_writeData: 0x%x, 0x%u, 0x%x 0x%x\n", addr, chunk_size_words * sizeof(uint32_t), flash_write_buffer, write_sratch_buffer);
                    // ret != fl_writeData(addr, chunk_size_words * sizeof(uint32_t), flash_write_buffer, write_sratch_buffer);
                }
                chan_out_word(c_ffs_rpc, ret == 0);
            break;
            case FFS_RPC_ERASE:
                (void) cmd; // Avoid compiler bug
                UINT32 nStartingAddress = chan_in_word(c_ffs_rpc);
                UINT32 nNumberOfSectors = chan_in_word(c_ffs_rpc);

                dprintf("Erase: 0x%x, %u\n", nStartingAddress, nNumberOfSectors);

                ret = 0;

                #if 0
                size_t num_sectors = fl_getNumDataSectors();
                uint32_t sector_base_addr = 0;
                int sector_num = 0;
                while(sector_base_addr < nStartingAddress){
                    sector_base_addr += fl_getDataSectorSize(sector_num);
                    if(++sector_num >= num_sectors){
                        assert 0; // We didn't get an address hit
                    }
                }
                // Double check we have an exact match
                assert(sector_base_addr == nStartingAddress);
                sector_num -= 1; // Because we incremented one last time
                ret = fl_eraseDataSector(sector_num);
                #endif

                chan_out_word(c_ffs_rpc, ret == 0);
            break;
        }
    }
}

chanend_t g_ffs_rpc_client = 0;

void init_ffs_rpc_client_chanend(chanend_t ffs_rpc_client){
    g_ffs_rpc_client = ffs_rpc_client;
}

BOOL usrInitFlashFileSystem(void)
{
    printchar('.');
    chan_out_word(g_ffs_rpc_client, FFS_RPC_INIT);
    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
} 

BOOL usrReadFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToRead)
{
    printchar('@');
    chan_out_word(g_ffs_rpc_client, nAddress);
    chan_out_word(g_ffs_rpc_client, (UINT32)pBuffer);
    chan_out_word(g_ffs_rpc_client, nDWordsToRead);

    uint32_t total_words_read = 0;
    while(total_words_read < nDWordsToRead){
        uint32_t chunk_size_words = chan_in_word(g_ffs_rpc_client);
        uint32_t *addr = pBuffer + total_words_read;
        chan_in_buf_word(g_ffs_rpc_client, addr, chunk_size_words);  
        total_words_read += chunk_size_words;                  
    }
    chan_in_word(g_ffs_rpc_client); // Needed?

    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
}


BOOL usrWriteFlashMemory(UINT32 nAddress, UINT32 * pBuffer, UINT32 nDWordsToWrite)
{
    printchar('+');
    chan_out_word(g_ffs_rpc_client, nAddress);
    chan_out_word(g_ffs_rpc_client, pBuffer);
    chan_out_word(g_ffs_rpc_client, nDWordsToWrite);

    uint32_t total_words_written = 0;
    while(total_words_written < nDWordsToWrite){
        uint32_t chunk_size_words = chan_in_word(g_ffs_rpc_client);
        uint32_t *addr = pBuffer + total_words_written;
        chan_out_buf_word(g_ffs_rpc_client, addr, chunk_size_words);  
        total_words_written += chunk_size_words;                  
    }
    chan_in_word(g_ffs_rpc_client); // Needed?

    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;

}


BOOL usrEraseFlashSector(UINT32 nStartingAddress, UINT32 nNumberOfSectors)
{
    printchar('-');
    chan_out_word(g_ffs_rpc_client, nStartingAddress);
    chan_out_word(g_ffs_rpc_client, nNumberOfSectors);

    BOOL bSuccess = chan_in_word(g_ffs_rpc_client);
    return bSuccess;
}
