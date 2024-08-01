/**
* @file 
* @brief A list of all AWE Server Commands
*/

/****************************************************************************
 *
 *		Command IDs
 *		-----------
 *
 ****************************************************************************
 *	ProxyIDs.h
 ****************************************************************************
 *
 *	Description:	Message command ID list
 *
 *	   Copyright:	 (c) 2007-2021 DSP Concepts, Inc. All rights reserved.
 *					 3235 Kifer Road
 *					 Santa Clara, CA 95054
 *
 *	Note: This file parsed using comma delimiters. Do not use commas in comments.
 *
 ***************************************************************************/
#ifndef PROXYIDS_H_INCLUDED
#define PROXYIDS_H_INCLUDED

/**
 * @brief Enumeration of Proxy function IDs.
 */
typedef enum
{
	/** Undefined ID - zero */
	PFID_Undefined							= 0,

	/** Function ID for awe_fwSetCall(). */
	PFID_SetCall							= 1, 

	/** Function ID for awe_fwGetCall(). */
	PFID_GetCall							= 2, 

	// Hole id=3

	/** Function ID for awe_fwGetClassType(). */
	PFID_GetClassType						= 4, 

	/** Function ID for awe_fwGetPinType(). */
	PFID_GetPinType						= 5, 

	/** Function ID for ClassWire_Constructor(). */
	PFID_ClassWire_Constructor				= 6, 

	/** Function ID for BindIOToWire(). */
	PFID_BindIOToWire						= 7, 

	/** Function ID for awe_fwFetchValue(). */
	PFID_FetchValue						= 8, 

	/** Function ID for awe_fwSetValue(). */
	PFID_SetValue							= 9, 

	/** Function ID for awe_fwGetHeapCount(). */
	PFID_GetHeapCount						= 10, 

	/** Function ID for awe_fwGetHeapSize(). */
	PFID_GetHeapSize						= 11, 

	/** Function ID for awe_fwDestroy(). */
	PFID_Destroy							= 12, 

	/** Function ID for awe_fwGetAweInstanceModuleCount(). */
	PFID_GetCIModuleCount					= 13, 

	/** Function ID for awe_fwGetAweInstanceModuleInfo(). */
	PFID_GetCIModuleInfo					= 14, 

	/** Function ID for ClassModule_Constructor(). */
	PFID_ClassModule_Constructor			= 15, 

	/** Function ID for awe_fwLayoutConstructor(). */
	PFID_ClassLayout_Constructor			= 16, 

	// Hole Id=17-18

	/** Function ID for awe_fwSetModuleState(). */
	PFID_SetModuleState					= 19, 

	/** Function ID for awe_fwGetModuleState(). */
	PFID_GetModuleState					= 20, 

	/** Function ID for awe_fwPumpModule(). */
	PFID_PumpModule						= 21, 

	/** Function ID for awe_fwLayoutProcess(). */
	PFID_ClassLayout_Process				= 22, 

	/** Function ID for awe_fwGetFirstObject(). */
	PFID_GetFirstObject					= 23, 

	/** Function ID for awe_fwGetNextObject(). */
	PFID_GetNextObject						= 24, 

	/** Function ID for awe_fwGetFirstIO(). */
	PFID_GetFirstIO						= 25, 

	/** Function ID for awe_fwGetNextIO(). */
	PFID_GetNextIO							= 26, 

	/** Function ID for StartAudio(). */
	PFID_StartAudio						= 27, 

	/** Function ID for StopAudio(). */
	PFID_StopAudio							= 28, 

	/** Function ID for awe_fwFetchValue(). */
	PFID_FetchValues						= 29, 

	/** Function ID for awe_fwSetValue(). */
	PFID_SetValues							= 30, 

	/** Function ID for sizeof(INT32). */
	PFID_GetSizeofInt						= 31, 

	/** Function ID for awe_GetFirstFile(). */
	PFID_GetFirstFile						= 32, 

	/** Function ID for awe_GetNextFile(). */
	PFID_GetNextFile						= 33, 

	/** Function ID for awe_OpenFile(). */
	PFID_OpenFile							= 34, 

	/** Function ID for awe_ReadFile(). */
	PFID_ReadFile							= 35, 

	/** Function ID for awe_WriteFile(). */
	PFID_WriteFile							= 36, 

	/** Function ID for awe_CloseFile(). */
	PFID_CloseFile							= 37, 

	/** Function ID for awe_DeleteFile(). */
	PFID_DeleteFile						= 38, 

	/** Function ID for awe_ExecuteFile(). */
	PFID_ExecuteFile						= 39, 

	/** Function ID for awe_EraseFlash(). */
	PFID_EraseFlash						= 40, 

	/** Function ID for awe_GetTargetInfo(). */
	PFID_GetTargetInfo						= 41, 

	/** Function ID for awe_GetFileSystemInfo(). */
	PFID_GetFileSystemInfo					= 42, 

	/** Function ID for awe_fwGetProfileValues(). */
	PFID_GetProfileValues					= 43, 

	/** Function ID for awe_FileSystemReset(). */
	PFID_FileSystemReset					= 44, 

	// Hole Id=45

	/** Function ID for awe_fwGetObjectByID(). */
	PFID_GetObjectByID						= 46, 

	/** Function ID for awe_fwAddModuleToLayout(). */
	PFID_AddModuleToLayout					= 47, 

	/** Function ID for awe_fwSetValue() awe_fwSetCall(). */
	PFID_SetValueCall						= 48, 

	// Hole Id=49-53

	/* Command ID to tick for deferred execution. */
	PFID_Tick								= 54, 

	// Hole Id=55

	/** Allocate framework heaps. */
	PFID_AllocateHeaps						= 56, 

	/** Destroy framework heaps. */
	PFID_DestroyHeaps						= 57, 

	/** Single message write-pump-read combo. */
	PFID_WritePumpRead						= 58, 

	// Hole Id=59

	/** Function ID for awe_fwSetValue() followed by awe_fwSetCall. */
	PFID_SetValueSetCall					= 60, 

	/** Function ID for awe_fwSetValues() followed by awe_fwSetCall. */
	PFID_SetValuesSetCall					= 61, 

	/** Function ID for awe_GetCall() followed by awe_fwFetchValue(). */
	PFID_GetCallFetchValue					= 62, 

	/** Function ID for awe_GetCall() followed by awe_fwFetchValues(). */
	PFID_GetCallFetchValues				= 63, 

	// Hole id=64-76

	/** Function ID for awe_fwSetPointer(). */
	PFID_SetPointer							= 77, 

	// Hole id=78-80

	/** Function ID for awe_fwcreateLookupTable(). */
	PFID_CreateLookupTable						= 81, 

	// Hole id=82-83

	/** Function ID for awe_fwDerefPointer(). */
	PFID_DerefPointer						= 84, 

	/** Function ID for awe_fwGetWireType(). */
	PFID_GetWireType						= 85, 

	/** Function ID for awe_fwSetInstanceID(). */
	PFID_SetInstanceID						= 86, 

	/** Function ID for awe_fwGetFlashEraseTime(). */
	PFID_Get_Flash_Erase_Time				= 87, 

	// Hole id=88-92

	/** Function ID for awe_fwDestroyAll(). */
	PFID_DestroyAll						= 93, 

	/** Function ID for report first core. */
	PFID_GetFirstCore						= 94, 

	/** Function ID for report next core. */
	PFID_GetNextCore						= 95, 

	// Hole id=96

	/** Function ID for GetCores(). */
	PFID_GetCores							= 97, 

	/** Function ID for awe_fwFetchValue() as float. */
	PFID_FetchValues_float					= 98, 

	/** Function ID for awe_GetCall() as float followed by awe_fwFetchValue(). */
	PFID_GetCallFetchValues_float			= 99, 

	/** Function ID for awe_fwSetValue() as float. */
	PFID_SetValues_float					= 100, 

	/** Function ID for awe_fwSetValues() as float followed by awe_fwSetCall. */
	PFID_SetValuesSetCall_float			= 101, 

	/** Function ID for awe_fwFetchValue() as float. */
	PFID_FetchValue_float					= 102, 

	/** Function ID for awe_GetCall() as float followed by awe_fwFetchValue(). */
	PFID_GetCallFetchValue_float			= 103, 

	/** Function ID for awe_fwSetValue() as float. */
	PFID_SetValue_float					= 104, 

	/** Function ID for awe_fwSetValue() followed by awe_fwSetCall. */
	PFID_SetValueSetCall_float				= 105, 

	/** Function ID for awe_fwSetValue(). */
	PFID_SetValuesPartial					= 106, 

	/** Function ID for awe_fwSetValue() as float. */
	PFID_SetValuesPartial_float			= 107, 

	// Hole id=108

	/** Function ID for GetCores(). */
	PFID_SetCores 	= 109, 

	// Hole id=110-112

	/** Special for target integrity bchecks. */
	PFID_CheckMemory						= 113,

	// Hole id=114-115
	
	/** Start audio without callback. */
	PFID_StartAudio2						= 116, 

	/** Stop audio without callback. */
	PFID_StopAudio2						= 117, 

	// Hole id=118-119

	/** Get value(s) by handle. */
	PFID_GetValueHandle					= 120, 

	/** Set value(s) by handle. */
	PFID_SetValueHandle					= 121, 

	/** Get module status by handle. */
	PFID_GetStatusHandle					= 122, 

	/** Set module status by handle. */
	PFID_SetStatusHandle					= 123, 

	/** Get value(s) by handle and mask. */
	PFID_GetValueHandleMask				= 124, 

	/** Set value(s) by handle and mask. */
	PFID_SetValueHandleMask				= 125, 

	/** Get extended target info. */
	PFID_GetExtendedInfo					= 126, 

	/** Function ID for GetCores(). */
	PFID_GetCores2 = 127,							

	PFID_GetInstanceTable = 127,            

	/** Function ID for awe_fwCreateWireBufferPool(). */
	PFID_CreateWireBufferPool = 128,					

	/** Function ID for awe_fwCreateWireInBufferPool(). */
	PFID_CreateWireInBufferPool = 129,
	
	/** Get the shared heap size. */
	PFID_GetSharedHeapSize = 130,

    /** Function ID for awe_fwGetLayoutCoreAffinity **/
    PFID_GetLayoutCoreAffinity = 131,	

	/** Function ID for awe_fwGetProfileValuesPreCalc(). */
	PFID_GetProfileValuesPreCalc = 132,
} tProxyFuncID;

#endif /* !PROXYIDS_H_INCLUDED */

/**
 * End of file.
 */


