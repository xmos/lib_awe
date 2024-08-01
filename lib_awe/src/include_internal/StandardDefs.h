/*******************************************************************************
*
*               Audio Framework
*               ---------------
*
********************************************************************************
*     StandardDefs.h
********************************************************************************
*
*     Description:  Standard Audio Weaver data types
*
*     Copyright:    (c) 2007-2021 DSP Concepts, Inc. All rights reserved.
*                   3235 Kifer Road
*                   Santa Clara, CA 95054
*
*******************************************************************************/
/**
* @file 
*/

#ifndef STANDARD_DEFS_H_
#define STANDARD_DEFS_H_

// Do not define BOOL for these processor types
#if defined(HMD) || defined(HIFI4) || defined(HIFIMINI)
#define BOOL_DEFINED
#endif

#ifndef UINT32_DEFINED
#define UINT32_DEFINED
#ifndef UINT32
//#define UINT32 unsigned int
typedef unsigned int UINT32;
#endif
#endif

#ifndef INT32_DEFINED
#define INT32_DEFINED
#ifndef INT32
//#define INT32 int
typedef int INT32;
#endif
#endif

#if defined(__ADSP215xx__) && defined(__ADSPSHARC__)
#ifndef BOOL
#define BOOL bool
#endif
#else
#ifndef BOOL
#define BOOL INT32
#endif
#endif

#ifndef MAXINT32
#define MAXINT32 ((INT32)0x7fffffffUL)
#endif

#ifndef MININT32
#define MININT32 ((INT32)~MAXINT32)
#endif

#ifndef UINT64_DEFINED
#define UINT64_DEFINED
#ifndef UINT64
//#define UINT64 unsigned long long
typedef unsigned long long UINT64;
#endif
#endif

#ifndef INT64_DEFINED
#define INT64_DEFINED
#ifndef INT64
//#define INT64 long long
typedef long long INT64;
#endif
#endif

#ifndef MAXINT64
#define MAXINT64 ((INT64)0x7ffffffffffffffffULL)
#endif

#ifndef MININT64
#define MININT64 ((INT64)~MAXINT64)
#endif

#ifndef FLOAT32_DEFINED
#define FLOAT32_DEFINED
#ifndef FLOAT32
//#define FLOAT32 float
typedef float FLOAT32;
#endif
#endif

#ifndef USE_SINGLE_PREC
#ifndef FLOAT64_DEFINED
#define FLOAT64_DEFINED
#ifndef FLOAT64
#if defined(__ADSP21000__)
typedef long double FLOAT64;
#else
typedef double FLOAT64;
#endif
#endif
#endif
#endif

#ifndef INT16_DEFINED
#define INT16_DEFINED
#ifndef INT16
//#define INT16 short
typedef short INT16;
#endif
#endif

#ifndef UINT16_DEFINED
#define UINT16_DEFINED
#ifndef UINT16
//#define UINT16 unsigned short
typedef unsigned short UINT16;
#endif
#endif

#ifndef INT8_DEFINED
#define INT8_DEFINED
#ifndef INT8
//#define INT8 signed char
typedef signed char INT8;
#endif
#endif

#ifndef UINT8_DEFINED
#define UINT8_DEFINED
#ifndef UINT8
//#define UINT8 unsigned char
typedef unsigned char UINT8;
#endif
#endif

/* These definitions are needed for Linux */
#ifndef INT_DEFINED
#define INT_DEFINED
#ifndef INT
//#define INT int
typedef int INT;
#endif
#endif

#ifndef UINT
#define UINT unsigned int
#endif

#ifndef BYTE_DEFINED
#define BYTE_DEFINED
#ifndef BYTE
//#define BYTE unsigned char
typedef unsigned char BYTE;
#endif
#endif

#ifndef PBYTE
#define PBYTE unsigned char *
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef PWORD
#define PWORD unsigned short *
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef PDWORD
#define PDWORD unsigned long *
#endif

#ifndef NUMOF
#define NUMOF(x)                    (sizeof(x) / sizeof(*x))
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef SUCCESS
#define SUCCESS 1
#endif

#ifndef FAILURE
#define FAILURE 0
#endif

#define PASTE(x, y)     x ## y
#define EVAL(x, y)      PASTE(x, y)

#ifndef FIXNAME
#define FIXNAME(x)      EVAL(MODULE_PREFIX, x)
#endif

#define AWE_UNUSED_VARIABLE(x)  ((void)(x))

#ifndef SAMPLETYPE_DEFINED
#define SAMPLETYPE_DEFINED
/**
 * @brief Type of data for I/O
*/
typedef enum _SampleType
{
	/** Data is 16 bit PCM. */
	Sample16bit,

	/** Data is 24 bit PCM aligned to the low bit in 32 bit buffers. */
	Sample24bit_low,

	/** Data is 24 bit PCM aligned to the high bit in 32 bit buffers. */
	Sample24bit_high,

	/** Data is 32 bit PCM . */
	Sample32bit
} SampleType;
#endif

#ifndef DLLSYMBOL
#define DLLSYMBOL
#endif

#ifndef CXX_NOWARN_DLLCLASSIF
#define CXX_NOWARN_DLLCLASSIF
#endif

#ifndef CXX_RESTORE_DLLCLASSIF
#define CXX_RESTORE_DLLCLASSIF
#endif

#endif // STANDARD_DEFS_H_
