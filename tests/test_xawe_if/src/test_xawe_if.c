// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <print.h>
#include <stdlib.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/select.h>
#include <xcore/parallel.h>
#include <xclib.h>
#include "awe_xcore.h"

#include "../../awb_files/playBasic_3thread_InitAWB.h"


DECLARE_JOB(awe_test, (chanend_t, chanend_t, chanend_t));
void awe_test(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    
    xAWEInstance_t xAWEInstance = {c_tuning_from_host, c_tuning_to_host};

    uint32_t msg_from_awe_buffer[16] = {0};
    int num_words_rx = 0;

    uint32_t pPos = 0;

    printstr("Loading AWB\n");
    xawe_loadAWBfromArray(&xAWEInstance, Core0_InitCommands, Core0_InitCommands_Len, &pPos);
    printstr("SUCCESS\n");


   // // Does the current AWE model have a SinkInt module with this control object ID?
   // if (xawe_ctrlGetModuleClass(&xAWEInstance, AWE_SinkInt1_value_HANDLE, &classID) == OBJECT_FOUND)
   // {
   //     // Check that module assigned this object ID is of module class SinkInt
   //     if (classID == AWE_SinkInt1_classID)
   //     {
   //         // SinkInt module (value is an array)
   //         xawe_ctrlGetValue(&xAWEInstance, AWE_SinkInt1_value_HANDLE, &nValue, 0, 1);
   //     }
   // }

    //  * @brief Get a scalar or array value of a module variable by handle
    //  * @param [in] pAWE                     instance pointer
    //  * @param [in] handle                   packed object handle
    //  * @param [out] value                   value(s) to get
    //  * @param [in] arrayOffset              array index if array
    //  * @param [in] length                   number of elements. 1 if scaler
    //  * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED, 
    //  *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS   
    //  */ 
    // INT32 xawe_ctrlGetValue(const xAWEInstance_t *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length);

    // /**
    //  * @brief Set a scalar or array value of a module variable by handle
    //  * @param [in] pAWE                     instance pointer
    //  * @param [in] handle                   packed object handle
    //  * @param [in] value                    value(s) to set
    //  * @param [in] arrayOffset              array index if array
    //  * @param [in] length                   number of elements. 1 if scaler
    //  * @return                              @ref E_SUCCESS,  @ref E_ARGUMENT_ERROR,  @ref E_BAD_MEMBER_INDEX,  @ref E_CLASS_NOT_SUPPORTED, 
    //  *  @ref E_LINKEDLIST_CORRUPT,  @ref E_NO_MORE_OBJECTS   
    //  */
    // INT32 xawe_ctrlSetValue(const xAWEInstance_t *pAWE, UINT32 handle, const void *value, INT32 arrayOffset, UINT32 length);

}

DECLARE_JOB(awe_main_wrapper, (chanend_t, chanend_t, chanend_t));
#pragma stackfunction 5000
void awe_main_wrapper(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data);
}

int main(int argc, char** argv){
    channel_t c_tuning_from_host = chan_alloc();
    channel_t c_tuning_to_host = chan_alloc();
    channel_t c_data = chan_alloc();

    PAR_JOBS(
        PJOB(awe_main_wrapper, (c_tuning_from_host.end_a, c_tuning_to_host.end_a, c_data.end_a)),
        PJOB(awe_test, (
            c_tuning_from_host.end_b, c_tuning_to_host.end_b, c_data.end_b)));

}
