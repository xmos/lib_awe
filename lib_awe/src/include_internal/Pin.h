/*******************************************************************************
*
*				Audio Framework
*				---------------
*
********************************************************************************
*	  Pin.h
********************************************************************************
*
*	  Description:	AudioWeaver public pin header file
*
*	  Copyright:	(c) 2007-2021 DSP Concepts, Inc. All rights reserved.
*					3235 Kifer Road
*					Santa Clara, CA 95054
*
*******************************************************************************/
#ifndef _PIN_H
#define _PIN_H

#ifdef	__cplusplus
extern "C" {
#endif

/** @brief AWE IO Pin type.  */
/**@details The AWE typedef for input/output pins. A BSP author will allocate their input/output pins of this type.
*/
typedef struct _IOPinDescriptor IOPinDescriptor;
typedef struct _IOPinDescriptor2
{
#ifdef IOPINDESCRIPTOR_FRAMEWORK
    IOPINDESCRIPTOR_FRAMEWORK
#else
    /** Reserved member. */
    #ifdef BUILD64
    UINT32 _Reserved[22];
    #else
    UINT32 _Reserved[15];
    #endif
#endif
} IOPinDescriptor2;

#ifdef	__cplusplus
}
#endif

#endif	/* _PIN_H */
