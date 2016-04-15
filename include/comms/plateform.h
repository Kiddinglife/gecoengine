/*
 * Copyright (c) 2016
 * Geco Gaming Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for GECO purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  GECO makes no representations about
 * the suitability of this software for GECO purpose.
 * It is provided "as is" without express or implied warranty.
 *
 */

/*
 * plateform.h
 *
 *  Created on: 15 Apr 2016
 *      Author: jakez
 *
 *      library wide definitions
 */

#ifndef INCLUDE_COMMS_PLATEFORM_H_
#define INCLUDE_COMMS_PLATEFORM_H_

#ifdef CODE_INLINE
#define INLINE    inline
#else
#define INLINE
#endif

#if defined( PLAYSTATION3 )
// _BIG_ENDIAN is a built in define
#include <stdlib.h>
#include <stdint.h>
#include <sys/sys_time.h>
#include <sys/timer.h>
#include <sys/time_util.h>
#include <sys/select.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if !defined( PLAYSTATION3 )
#include <malloc.h>
#endif
#include <limits>
#include <math.h>

#ifdef __WATCOMC__
#ifndef false
#define false       0
#define true        1
#endif
#endif // __WATCOMC__
#endif /* INCLUDE_COMMS_COMM_DEFS_H_ */

/* basic types
 * */
/// This type is an unsigned character.
typedef unsigned char uchar;
/// This type is an unsigned short.
typedef unsigned short ushort;
/// This type is an unsigned integer.
typedef unsigned int uint;
/// This type is an unsigned longer.
typedef unsigned long ulong;

#ifdef _WIN32
typedef __int8 int8;
typedef unsigned __int8 uint8;
typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
// This type is an integer with the size of a pointer.
typedef INT_PTR intptr;
// This type is an unsigned integer with the size of a pointer.
typedef UINT_PTR uintptr;
#define PRIu64 "%llu"
#else //unix
/// This type is an integer with a size of 8 bits.
typedef int8_t int8;
/// This type is an unsigned integer with a size of 8 bits.
typedef uint8_t uint8;
/// This type is an integer with a size of 16 bits.
typedef int16_t int16;
/// This type is an unsigned integer with a size of 16 bits.
typedef uint16_t uint16;
/// This type is an integer with a size of 32 bits.
typedef int32_t int32;
/// This type is an unsigned integer with a size of 32 bits.
typedef uint32_t uint32;
/// This type is an integer with a size of 64 bits.
typedef int64_t int64;
/// This type is an unsigned integer with a size of 64 bits.
typedef uint64_t uint64;
typedef intptr_t intptr;
typedef uintptr_t uintptr;
#if __WORDSIZE == 64 //64bits plateform
#define PRIu64 "%lu"
#else
#define PRIu64 "%llu"
#endif

/*
 * array & structure macros
 * */
#define ARRAYSZ(v)                  (sizeof(v) / sizeof(v[0]))
#define ARRAY_SIZE(v)              (sizeof(v) / sizeof(v[0]))

/*
 * string comparing
 * */
/// Returns true if two strings are equal.
#define STR_EQ(s, t)        (!strcmp((s), (t)))
/// Returns true if two strings are the same, ignoring case.
#define STR_EQI(s, t)       (!bw_stricmp((s), (t)))
/// Returns true if the first sz byte of the input string are equal, ignoring
/// case.
#define STRN_EQI(s, t, sz)  (!bw_strnicmp((s), (t), (sz)))
/// Returns true if the first sz byte of the input string are equal, not ignoring
/// case.
#define STRN_EQ(s, t, sz)   (!strncmp((s), (t), (sz)))
/// Returns true if all three input string are equal.
#define STR_EQ2(s, t1, t2)  (!(strcmp((s), (t1)) || strcmp((s), (t2))))

/// Returns the high byte of a word.
#define HIBYTEW(b)      (((b) & 0xff00) >> 8)
/// Returns the low byte of a word.
#define LOBYTEW(b)      ( (b) & 0xff)
/// Returns the high byte of a long.
#define HIBYTEL(b)      (((b) & 0xff000000L) >> 24)
/// Returns the low byte of a long.
#define LOBYTEL(b)      ( (b) & 0xffL)
/// Returns the high word of a long.
#define HIWORDL(b)      (((b) & 0xffff0000L) >> 16)
/// Returns the low word of a long.
#define LOWORDL(b)      ( (b) & 0xffffL)
/**
 *  This macro takes a dword ordered 0123 and reorder it to 3210.
 */
#define SWAP_DW(a)    ( (((a) & 0xff000000)>>24) |  \
                        (((a) & 0xff0000)>>8) |     \
                        (((a) & 0xff00)<<8) |       \
                        (((a) & 0xff)<<24) )

#ifdef GECO_LITTLE_ENDIAN
/// Returns byte 0 of a long.
#define BYTE0L(b)       ( (b) & 0xffL)
/// Returns byte 1 of a long.
#define BYTE1L(b)       (((b) & 0xff00L) >> 8)
/// Returns byte 2 of a long.
#define BYTE2L(b)       (((b) & 0xff0000L) >> 16)
/// Returns byte 3 of a long.
#define BYTE3L(b)       (((b) & 0xff000000L) >> 24)
#else
/// Returns byte 0 of a long.
#define BYTE4L(b)       ( (b) & 0xffL)
/// Returns byte 1 of a long.
#define BYTE3L(b)       (((b) & 0xff00L) >> 8)
/// Returns byte 2 of a long.
#define BYTE2L(b)       (((b) & 0xff0000L) >> 16)
/// Returns byte 3 of a long.
#define BYTE1L(b)       (((b) & 0xff000000L) >> 24)
#endif

/// This macro is used to enter the debugger.
#if defined( _XBOX360 )
#define ENTER_DEBUGGER() DebugBreak()
#elif defined( _WIN32 )
#define ENTER_DEBUGGER() __asm { int 3 }
#elif defined( PLAYSTATION3 )
#define ENTER_DEBUGGER() __asm__ volatile ( ".int 0" )
#else
#define ENTER_DEBUGGER() asm( "int $3" )
#endif

#endif

