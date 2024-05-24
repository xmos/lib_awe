/*******************************************************************************
*
*				Audio Framework
*				---------------
*
********************************************************************************
*	  AWECore.h
********************************************************************************
*
*	  Description:	AudioWeaver Framework main header file
*
*	  Copyright:	(c) 2007-2021 DSP Concepts, Inc. All rights reserved.
*					3235 Kifer Road
*					Santa Clara, CA 95054
*
*******************************************************************************/

/**
* @file 
* @brief The AWECore API Header File
*/

#ifndef _FRAMEWORK_H
//Do nothing if Framework.h was previously included.

#ifndef _AWECORE_H
#define _AWECORE_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// Include the TargetProcessor header that includes another processor specific header file. 
#include "TargetProcessor.h"
#include "FlashFSInstance.h"
#include "Pin.h"
// Opaque declaration for framework structure ModClassModule
typedef void ModClassModule;
#include "AWEInstance.h"

#endif	// _AWECORE_H

#endif // _FRAMEWORK_H
