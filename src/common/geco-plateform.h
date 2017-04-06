/*
* Geco Gaming Company
* All Rights Reserved.
* Copyright (c)  2016 GECOEngine.
*
* GECOEngine is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* GECOEngine is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public License
* along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*
*/

// created on 26-June-2016 by Jackie Zhang
#ifndef __GECO_KERNEL_H__
#define __GECO_KERNEL_H__

#include "geco-config.h"

/* common includes */
#include <climits>
#include <numeric>
#include <cmath>

/* 360 includes */

/* PS3 includes */
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
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <pwd.h>
#define geco_sleep sleep
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef struct passwd
{
	char    *pw_name;
	char    *pw_passwd;
	int		 pw_uid;
	int		 pw_gid;
	char    *pw_gecos;
	char    *pw_dir;
	char    *pw_shell;
};
#define geco_sleep Sleep
#endif

#if !defined( PLAYSTATION3 )
#include <malloc.h>
#endif

#ifdef __WATCOMC__
#ifndef false
#define false		0
#define true		1
#endif
#endif // __WATCOMC__

/*-------------- basic type defs -------------*/
/// This type is an unsigned character.
typedef unsigned char uchar;
/// This type is an unsigned short.
typedef unsigned short ushort;
/// This type is an unsigned integer.
typedef unsigned int uint;
/// This type is an unsigned longer.
typedef unsigned long ulong;

#if defined( PLAYSTATION3 )

typedef int8_t			int8;
typedef int16_t			int16;
typedef int32_t			int32;
typedef int64_t			int64;
typedef uint8_t			uint8;
typedef uint16_t		uint16;
typedef uint32_t		uint32;
typedef uint64_t		uint64;

typedef intptr_t		intptr;
typedef uintptr_t		uintptr;

#define PRI64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"
#define PRIzu "lu"
#define PRIzd "ld"

#else


#ifdef _WIN32
typedef __int8				int8;
typedef unsigned __int8		uint8;
typedef __int16				int16;
typedef unsigned __int16	uint16;
typedef __int32				int32;
typedef unsigned __int32	uint32;
typedef __int64				int64;
typedef unsigned __int64	uint64;
// This type is an integer with the size of a pointer.
typedef INT_PTR				intptr;
// This type is an unsigned integer with the size of a pointer.
typedef UINT_PTR        	uintptr;
#define PRI64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"
#define PRIzu "lu"
#define PRIzd "ld"
#else //unix
/// This type is an integer with a size of 8 bits.
typedef int8_t				int8;
/// This type is an unsigned integer with a size of 8 bits.
typedef uint8_t				uint8;
/// This type is an integer with a size of 16 bits.
typedef int16_t				int16;
/// This type is an unsigned integer with a size of 16 bits.
typedef uint16_t			uint16;
/// This type is an integer with a size of 32 bits.
typedef int32_t				int32;
/// This type is an unsigned integer with a size of 32 bits.
typedef uint32_t			uint32;
/// This type is an integer with a size of 64 bits.
typedef int64_t				int64;
/// This type is an unsigned integer with a size of 64 bits.
typedef uint64_t			uint64;
#ifdef _LP64
typedef int64				intptr;
typedef uint64				uintptr;
#define PRI64 "ld"
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIX64 "lX"
#else
typedef int32				intptr;
typedef uint32				uintptr;
#define PRI64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"
#endif
#ifndef PRIzd
#define PRIzd "zd"
#endif
#ifndef PRIzu
#define PRIzu "zu"
#endif
#endif
#endif

/// This type is used for a 4 byte file header descriptor.
typedef uint32			HdrID;		/* 4 byte (file) descriptor */
/// This type is used for a generic 4 byte descriptor.
typedef uint32			ID;			/* 4 byte generic descriptor */

/*-------------- Macros --------------*/
/* array & structure macros */
#define ARRAYSZ(v)					(sizeof(v) / sizeof(v[0]))
#define ARRAY_SIZE(v)				(sizeof(v) / sizeof(v[0]))

/* string comparing */
/// Returns true if two strings are equal.
#define STR_EQ(s, t)		(!strcmp((s), (t)))
/// Returns true if two strings are the same, ignoring case.
#define STR_EQI(s, t)		(!geco_stricmp((s), (t)))
/// Returns true if the first sz byte of the input string are equal, ignoring case.
#define STRN_EQI(s, t, sz) 	(!geco_strnicmp((s), (t), (sz)))
/// Returns true if the first sz byte of the input string are equal, ignoring case.
#define STRN_EQ(s, t, sz) 	(!strncmp((s), (t), (sz)))
/// Returns true if all three input string are equal.
#define STR_EQ2(s, t1, t2)	(!(strcmp((s), (t1)) || strcmp((s), (t2))))

/// scalar types comparing 
#define GECO_MIN(a, b) (((b) < (a)) ? (b) : (a))
#define GECO_MAX(a, b) (((a) < (b)) ? (b) : (a))

/*
* Intel Architecture is little endian (low byte presented first)
* Motorola Architecture is big endian (high byte first)
*/
/// The current architecture is Little Endian.
//#define GECO_LITTLE_ENDIAN
/*#define GECO_BIG_ENDIAN*/

/*
* accessing individual bytes (int8) and words (int16) within
* words and long words (int32).
* Macros ending with W deal with words, L macros deal with longs
*/
#ifdef GECO_LITTLE_ENDIAN
/// Returns the high byte of a word.
#define HIBYTEW(b)		(((b) & 0xff00) >> 8)
/// Returns the low byte of a word.
#define LOBYTEW(b)		( (b) & 0xff)
/// Returns the high byte of a long.
#define HIBYTEL(b)		(((b) & 0xff000000L) >> 24)
/// Returns the low byte of a long.
#define LOBYTEL(b)		( (b) & 0xffL)
/// Returns byte 0 of a long.
#define BYTE0L(b)		( (b) & 0xffL)
/// Returns byte 1 of a long.
#define BYTE1L(b)		(((b) & 0xff00L) >> 8)
/// Returns byte 2 of a long.
#define BYTE2L(b)		(((b) & 0xff0000L) >> 16)
/// Returns byte 3 of a long. 0xff000000L = 1111111 - 3CHARS
#define BYTE3L(b)		(((b) & 0xff000000L) >> 24)
/// Returns the high word of a long.
#define HIWORDL(b)		(((b) & 0xffff0000L) >> 16)
/// Returns the low word of a long.
#define LOWORDL(b)		( (b) & 0xffffL)
/*This macro takes a dword ordered 0123 and reorder it to 3210.*/
#define SWAP_DW(a) ((((a)&0xff000000)>>24)|(((a)&0xff0000)>>8)|(((a)&0xff00)<<8)|(((a)&0xff)<<24)
#else
/* big endian macros go here */
#endif

// This macro is used to enter the debugger.
//how to complie asemble http://blog.csdn.net/robin__chou/article/details/50275061
#if defined( _XBOX360 )
#define ENTER_DEBUGGER() DebugBreak()
#elif defined( WIN32 )
extern "C" void _stdcall asm_int3();
#ifdef _AMD64_
#define ENTER_DEBUGGER() asm_int3()
#else
#define ENTER_DEBUGGER() __asm { int 3 }
#endif
#elif defined( PLAYSTATION3 )
#define ENTER_DEBUGGER() __asm__ volatile ( ".int 0" )
#else
#define ENTER_DEBUGGER() asm( "int $3" )
#endif

#ifndef _WIN32
#define GECO_SYSLOG_EMERG       0
#define GECO_SYSLOG_ALERT       1
#define GECO_SYSLOG_CRIT        2
#define GECO_SYSLOG_ERR         3
#define GECO_SYSLOG_WARNING     4
#define GECO_SYSLOG_NOTICE      5
#define GECO_SYSLOG_INFO        6
#define GECO_SYSLOG_DEBUG       7
#else // !_WIN32
#define GECO_SYSLOG_EMERG       EVENTLOG_SUCCESS
#define GECO_SYSLOG_ALERT       EVENTLOG_ERROR_TYPE
#define GECO_SYSLOG_CRIT        EVENTLOG_ERROR_TYPE
#define GECO_SYSLOG_ERR         EVENTLOG_ERROR_TYPE
#define GECO_SYSLOG_WARNING     EVENTLOG_WARNING_TYPE
#define GECO_SYSLOG_NOTICE      EVENTLOG_WARNING_TYPE
#define GECO_SYSLOG_INFO        EVENTLOG_INFORMATION_TYPE
#define GECO_SYSLOG_DEBUG       EVENTLOG_INFORMATION_TYPE
#endif // _WIN32
#define GECO_SYSLOG_KERN        (0<<3)
#define GECO_SYSLOG_USER        (1<<3)
#define GECO_SYSLOG_MAIL        (2<<3)
#define GECO_SYSLOG_DAEMON      (3<<3)
#define GECO_SYSLOG_AUTH        (4<<3)
#define GECO_SYSLOG_SYSLOG      (5<<3)
#define GECO_SYSLOG_LPR         (6<<3)
#define GECO_SYSLOG_NEWS        (7<<3)
#define GECO_SYSLOG_UUCP        (8<<3)
#define GECO_SYSLOG_CRON        (9<<3)
#define GECO_SYSLOG_AUTHPRIV    (10<<3)
#define GECO_SYSLOG_FTP         (11<<3)

extern int GECO_EXTERN_VAR g_iBackEndUID;
extern int GECO_EXTERN_VAR g_iBackEndPID;
extern int GECO_EXTERN_VAR g_iIsBackEndProcess;

GECOAPI void GECO_FAST_CALL geco_open_syslog(const char *pcIdent, int iOption, int iFacility);
GECOAPI void GECO_FAST_CALL geco_syslog(int iPriority, const char *pcFormat, ...);

GECOAPI void GECO_FAST_CALL geco_init_back_end_process(int iUID, int iPID);
GECOAPI int GECO_FAST_CALL geco_get_user_id();
GECOAPI 
#ifdef _WIN32
const wchar_t*
#else
const char*
#endif
GECO_FAST_CALL getUsername();
GECOAPI int GECO_FAST_CALL geco_get_pid();
GECOAPI passwd* GECO_FAST_CALL  geco_get_pw_user_id(int iUID);
GECOAPI passwd*  GECO_FAST_CALL geco_get_pw_nam(const char *pcName);
/*
*	This function is used to strip the path and
* return just the basename from a path string.
*　sample base path: logger/cppThresholds/ENGINE-UNITTEST-LOGGER/test-auth.cc
*/
GECOAPI const char* GECO_FAST_CALL get_base_path(const char * path, const char * module);
GECOAPI void* GECO_FAST_CALL memcpy_fast(void *destination, const void *source, size_t size);

/*print out*/
#if defined(__unix__) || defined(__linux__) || defined( PLAYSTATION3 )
#define geco_isnan isnan
#define geco_isinf isinf
#define geco_snprintf snprintf
#define geco_vsnprintf vsnprintf
#define geco_vsnwprintf vsnwprintf
#define geco_snwprintf swprintf
#define geco_stricmp strcasecmp
#define geco_strnicmp strncasecmp
#define geco_fileno fileno
#define geco_va_copy va_copy
#else
#define geco_isnan _isnan
#define geco_isinf(x) (!_finite(x) && !_isnan(x))
#define geco_snprintf _snprintf
#define geco_vsnprintf _vsnprintf
#define geco_vsnwprintf _vsnwprintf
#define geco_snwprintf _snwprintf
#define geco_stricmp _stricmp
#define geco_strnicmp _strnicmp
#define geco_fileno _fileno
#define geco_va_copy( dst, src) dst = src
#endif // unix or linux

#define GECO_SAFE_DELETE(p) if(p) {delete (p); (p) = NULL;}
#define GECO_SAFE_DELETE_ARRAY(p) if(p) {delete [] (p); (p) = NULL;}
#define GECO_SAFE_FREE(p) if(p) {free(p); (p) = NULL;}
#define GECO_SAFE_FREE_ARRAY(p) if(p) {free(p); (p) = NULL;}
#define geco_zero_mem(dst,len) (memset((dst),0,(len)))
#define geco_zero_mem_ex(dst,size,count) (geco_zero_mem((dst),(size)*(count)))
#define geco_mem_cpy(dst,src,len) (memcpy((dst),(src),(len)))
#define geco_mem_cpy_ex(dst,src,size,count) (geco_mem_cpy((dst),(src),(size)*(count)))

#ifndef GECO_DECLARE_COPY_CTOR
#define GECO_DECLARE_COPY_CTOR(classname)  classname(const classname & );
#endif
#ifndef GECO_DECLARE_COPY_OPT
#define GECO_DECLARE_COPY_OPT(classname)  void operator=( const classname& );
#endif
#ifndef GECO_DECLARE_COMPARE_OPT
#define GECO_DECLARE_COMPARE_OPT(classname)  bool operator==( const classname& );
#endif
#ifndef GECO_NOT_COPY
#define GECO_NOT_COPY(classname) \
	GECO_DECLARE_COPY_OPT(classname);\
	GECO_DECLARE_COPY_CTOR(classname);
#endif
#ifndef GECO_NOT_COPY_COMPARE
#define GECO_NOT_COPY_COMPARE(classname) \
	GECO_DECLARE_COPY_OPT(classname);\
	GECO_DECLARE_COMPARE_OPT(classname);\
	GECO_DECLARE_COPY_CTOR(classname);
#endif

#define GECO_MASK(location) (0x01 << (location))
#define GECO_MASK_HAS_ANY(flag,mask) (((flag) & (mask)) != 0)
#define GECO_MASK_HAS_ALL(flag,mask) (((flag) & (mask)) == (mask))
#define GECO_MASK_ADD(flag,mask) ((flag) |= (mask))
#define GECO_MASK_DEL(flag,mask) ((flag) &= (~(mask)))
#define GECO_MASK_REMOVE(flag,mask) ((flag) & (~(mask)))
#define GECO_MASK_CHANGE(location,flag)									\
if(GECO_MASK_HAS_ANY(flag,GECO_MASK(location)))							\
GECO_MASK_DEL(flag,GECO_MASK(location));							\
else																\
GECO_MASK_ADD(flag,GECO_MASK(location));

//how to complie asemble http://blog.csdn.net/robin__chou/article/details/50275061
/// This macro is used to enter the debugger.
#if defined( _XBOX360 )
#define ENTER_DEBUGGER() DebugBreak()
#elif defined( WIN32 )
extern "C" void _stdcall asm_int3();
#ifdef _AMD64_
#define ENTER_DEBUGGER() asm_int3()
#else
#define ENTER_DEBUGGER() __asm { int 3 }
#endif
#elif defined( PLAYSTATION3 )
#define ENTER_DEBUGGER() __asm__ volatile ( ".int 0" )
#else
#define ENTER_DEBUGGER() asm( "int $3" )
#endif

/**
*  This class helps with using internal STL implementation details in
*  different compilers.
*/
template<class MAP> struct MapTypes
{
#ifdef _WIN32
#if _MSC_VER>=1300 // VC7
	typedef typename MAP::mapped_type & _Tref;
#else
	typedef typename MAP::_Tref _Tref;
#endif
#else
	typedef typename MAP::mapped_type & _Tref;
#endif
};

#define IS_EQUAL_FLOAT(value1, value2) \
(abs(value1 - value2) < std::numeric_limits<float>::epsilon())
// use 0.0004 because most existing functions are using it
inline bool almost_equal(const float f1, const float f2, const float epsilon =
	0.0004f)
{
	return fabsf(f1 - f2) < epsilon;
}
inline bool almost_equal(const double d1, const double d2,
	const double epsilon = 0.0004)
{
	return fabs(d1 - d2) < epsilon;
}
inline bool almost_zero(const float f, const float epsilon = 0.0004f)
{
	return f < epsilon && f > -epsilon;
}
inline bool almost_zero(const double d, const double epsilon = 0.0004)
{
	return d < epsilon && d > -epsilon;
}
template<typename T>
inline bool almost_equal(const T& c1, const T& c2,
	const float epsilon = 0.0004f)
{
	if (c1.size() != c2.size())
		return false;
	typename T::const_iterator iter1 = c1.begin();
	typename T::const_iterator iter2 = c2.begin();
	for (; iter1 != c1.end(); ++iter1, ++iter2)
		if (!almost_equal(*iter1, *iter2, epsilon))
			return false;
	return true;
}

/**
*  This function determines if a float is a valid and finite
*/
union IntFloat
{
	float f;
	uint32 ui32;
};

inline bool isFloatValid(float f)
{
	IntFloat intFloat;
	intFloat.f = f;
	return (intFloat.ui32 & 0x7f800000) != 0x7f800000;
}

/**
*	Static (i.e. compile-time) assert. Based off
*	Modern C++ Design: Generic Programming and Design Patterns Applied
*	Section 2.1
*	by Andrei Alexandrescu
*/
template<bool> class geco_compile_time_check
{
public:
	geco_compile_time_check(...) {}
};
template<> class geco_compile_time_check<false>
{
};
#define GECO_STATIC_ASSERT(test, errormsg)						\
	do {														\
		struct ERROR_##errormsg {};								\
		typedef geco_compile_time_check< (test) != 0 > TmplImpl;	\
		TmplImpl aTemp = TmplImpl( ERROR_##errormsg() );		\
		size_t x = sizeof( aTemp );								\
		x += 1;													\
	} while (0)

struct   uint24_t
{
	unsigned int val;
	uint24_t() :
		val(0)
	{
	}
	operator unsigned int()
	{
		return val;
	}
	operator unsigned int() const
	{
		return val;
	}

	uint24_t(const uint24_t& a)
	{
		val = a.val;
	}
	uint24_t operator++()
	{
		++val;
		val &= 0x00FFFFFF;
		return *this;
	}
	uint24_t operator--()
	{
		--val;
		val &= 0x00FFFFFF;
		return *this;
	}
	uint24_t operator++(int)
	{
		uint24_t temp(val);
		++val;
		val &= 0x00FFFFFF;
		return temp;
	}
	uint24_t operator--(int)
	{
		uint24_t temp(val);
		--val;
		val &= 0x00FFFFFF;
		return temp;
	}
	uint24_t operator&(const uint24_t& a)
	{
		return uint24_t(val & a.val);
	}
	uint24_t& operator=(const uint24_t& a)
	{
		val = a.val;
		return *this;
	}
	uint24_t& operator+=(const uint24_t& a)
	{
		val += a.val;
		val &= 0x00FFFFFF;
		return *this;
	}
	uint24_t& operator-=(const uint24_t& a)
	{
		val -= a.val;
		val &= 0x00FFFFFF;
		return *this;
	}
	bool operator==(const uint24_t& right) const
	{
		return val == right.val;
	}
	bool operator!=(const uint24_t& right) const
	{
		return val != right.val;
	}
	bool operator >(const uint24_t& right) const
	{
		return val > right.val;
	}
	bool operator <(const uint24_t& right) const
	{
		return val < right.val;
	}
	const uint24_t operator+(const uint24_t &other) const
	{
		return uint24_t(val + other.val);
	}
	const uint24_t operator-(const uint24_t &other) const
	{
		return uint24_t(val - other.val);
	}
	const uint24_t operator/(const uint24_t &other) const
	{
		return uint24_t(val / other.val);
	}
	const uint24_t operator*(const uint24_t &other) const
	{
		return uint24_t(val * other.val);
	}

	uint24_t(const unsigned int& a)
	{
		val = a;
		val &= 0x00FFFFFF;
	}
	uint24_t operator&(const unsigned int& a)
	{
		return uint24_t(val & a);
	}
	uint24_t& operator=(const unsigned int& a)
	{
		val = a;
		val &= 0x00FFFFFF;
		return *this;
	}
	uint24_t& operator+=(const unsigned int& a)
	{
		val += a;
		val &= 0x00FFFFFF;
		return *this;
	}
	uint24_t& operator-=(const unsigned int& a)
	{
		val -= a;
		val &= 0x00FFFFFF;
		return *this;
	}
	bool operator==(const unsigned int& right) const
	{
		return val == (right & 0x00FFFFFF);
	}
	bool operator!=(const unsigned int& right) const
	{
		return val != (right & 0x00FFFFFF);
	}
	bool operator >(const unsigned int& right) const
	{
		return val > (right & 0x00FFFFFF);
	}
	bool operator <(const unsigned int& right) const
	{
		return val < (right & 0x00FFFFFF);
	}
	const uint24_t operator+(const unsigned int &other) const
	{
		return uint24_t(val + other);
	}
	const uint24_t operator-(const unsigned int &other) const
	{
		return uint24_t(val - other);
	}
	const uint24_t operator/(const unsigned int &other) const
	{
		return uint24_t(val / other);
	}
	const uint24_t operator*(const unsigned int &other) const
	{
		return uint24_t(val * other);
	}
};
#endif
