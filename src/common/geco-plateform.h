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
#ifndef _SRC_GECO_ENGINE_PLATEFORM
#define _SRC_GECO_ENGINE_PLATEFORM

//#include <new>
//void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
//{
//	return 
//}
//void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
//{
//}

#ifdef CODE_INLINE
#define CODE_INLINE    inline
#else
#define CODE_INLINE
#endif

/* common includes */
#include <limits>
#include <math.h>

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
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef intptr_t intptr;
typedef uintptr_t uintptr;
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"
#define PRIu32 "lu"
#define PRId32 "ld"
#else
#ifdef _WIN32
typedef __int8 int8;
typedef unsigned __int8 uint8;
typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
/// This type is an integer with the size of a pointer.
typedef INT_PTR intptr;
/// This type is an unsigned integer with the size of a pointer.
typedef UINT_PTR uintptr;
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"
#define PRIu32 "lu"
#define PRId32 "ld"
#define GECO_FAST_CALL __fastcall
#else //unix or linux
#define GECO_FAST_CALL
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
#ifdef _LP64
typedef int64 intptr;
typedef uint64 uintptr;
#define PRId64 "ld"
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIX64 "lX"
#else
typedef int32 intptr;
typedef uint32 uintptr;
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"
#endif
#ifndef PRId32
#define PRId32 "zd"
#endif
#ifndef PRIu32
#define PRIu32 "zu"
#endif
#endif

#endif

struct uint24_t
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

/*-------------- Macros --------------*/
/* array & structure macros */
#define ARRAYSZ(v)					(sizeof(v) / sizeof(v[0]))
#define ARRAY_SIZE(v)				(sizeof(v) / sizeof(v[0]))

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

/* scalar types comparing */
#include <algorithm>
#if defined(_WIN32)
#undef min
#define min min
#undef max
#define max max
template <class T> inline const T & min(const T & a, const T & b)
{	return b < a ? b : a;}
template <class T> inline const T & max(const T & a, const T & b)
{	return a < b ? b : a;}
#define GECO_MIN min
#define GECO_MAX max
#define NOMINMAX
#undef geco_sleep
#define geco_sleep Sleep
#else
#define GECO_MIN std::min
#define GECO_MAX std::max
#undef geco_sleep
#define geco_sleep sleep
#endif

/*
 * Intel Architecture is little endian (low byte presented first)
 * Motorola Architecture is big endian (high byte first)
 */
/// The current architecture is Little Endian.
#define GECO_LITTLE_ENDIAN
/*#define GECO_BIG_ENDIAN*/

/* accessing individual bytes (int8) and words (int16) within
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
#define SWAP_DW(a)	\
((((a)&0xff000000)>>24)|(((a)&0xff0000)>>8)|(((a)&0xff00)<<8)|(((a)&0xff)<<24))
#else
/* big endian macros go here */
#endif

#define GECO_SAFE_DELETE(p) if(p) {delete (p); (p) = NULL;}
#define GECO_SAFE_DELETE_ARRAY(p) if(p) {delete [] (p); (p) = NULL;}

#ifndef GECO_DECLARE_COPY_CONSTRUCTOR
#define GECO_DECLARE_COPY_CONSTRUCTOR(classname)  classname(const classname & );
#endif
#ifndef GECO_DECLARE_COPY
#define GECO_DECLARE_COPY(classname)  void operator=( const classname& );
#endif
#ifndef GECO_DECLARE_COMPARE
#define GECO_DECLARE_COMPARE(classname)  bool operator==( const classname& );
#endif
#ifndef GECO_NOT_COPY
#define GECO_NOT_COPY(classname) \
	GECO_DECLARE_COPY(classname);\
	GECO_DECLARE_COPY_CONSTRUCTOR(classname);
#endif
#ifndef GECO_NOT_COPY_COMPARE
#define GECO_NOT_COPY_COMPARE(classname) \
	GECO_DECLARE_COPY(classname);\
	GECO_DECLARE_COMPARE(classname);\
	GECO_DECLARE_COPY_CONSTRUCTOR(classname);
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
#elif defined( _WIN32 )
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
 *	This function returns user id.
 */
inline int getUserId()
{
#ifdef _WIN32
	// VS2005:
#if _MSC_VER >= 1400
	char uid[16];
	size_t sz;
	return getenv_s(&sz, uid, sizeof(uid), "UID") == 0 ? atoi(uid) : 0;

	// VS2003:
#elif _MSC_VER < 1400
	char * uid = getenv("UID");
	return uid ? atoi(uid) : 0;
#endif
#elif defined( PLAYSTATION3 )
	return 123;
#else
	// Linux:
	char * uid = getenv("UID");
	return uid ? atoi(uid) : getuid();
#endif
}

/**
 *	This function returns the username the process is running under.
 */
inline const char * getUsername()
{
#ifdef _WIN32
	return "";
#else
	// Note: a string in a static area is returned. Do not store this pointer.
	// See cuserid for details.
	char * pUsername = cuserid(NULL);
	return pUsername ? pUsername : "";
#endif
}

/**
 *	This function returns the process id.
 */
inline int mf_getpid()
{
#if defined(__unix__) || defined(__linux__)
	return getpid();
#elif defined(_XBOX) || defined(_XBOX360) || defined( PLAYSTATION3 )
	return -1;
#else
	return (int)GetCurrentProcessId();
#endif
}
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

#include <numeric>
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

/*
 *	This function is used to strip the path and
 * return just the basename from a path string.
 *　sample base path: logger/cppThresholds/ENGINE-UNITTEST-LOGGER/test-auth.cc
 */
#include <string.h>
inline const char* get_base_path(const char * path, const char * module)
{
	static char staticSpace[128];
	const char * pResult = path;
	const char * pSeparator;

	pSeparator = strrchr(pResult, '\\');
	if (pSeparator != NULL)
	{
		pResult = pSeparator + 1;
	}

	pSeparator = strrchr(pResult, '/');
	if (pSeparator != NULL)
	{
		pResult = pSeparator + 1;
	}

	strcpy(staticSpace, "logger/cppThresholds/");

	if (module != NULL)
	{
		strcat(staticSpace, module);
		strcat(staticSpace, "/");
	}

	strcat(staticSpace, pResult);
	return staticSpace;
}
/**
 *	Static (i.e. compile-time) assert. Based off
 *	Modern C++ Design: Generic Programming and Design Patterns Applied
 *	Section 2.1
 *	by Andrei Alexandrescu
 */
template<bool> class BW_compile_time_check
{
public:
	BW_compile_time_check(...)
	{
	}
};
template<> class BW_compile_time_check<false>
{
};
#define BW_STATIC_ASSERT(test, errormsg)\
do {struct ERROR_##errormsg {};\
typedef BW_compile_time_check< (test) != 0 > TmplImpl;\
TmplImpl aTemp = TmplImpl( ERROR_##errormsg() );	\
size_t x = sizeof( aTemp ); x += 1;} while (0)

#include <stddef.h>
#include <stdint.h>
#include <emmintrin.h>
//---------------------------------------------------------------------
// force inline for compilers
//---------------------------------------------------------------------
#ifndef INLINE
#ifdef __GNUC__
#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif
#elif defined(_MSC_VER)
#define INLINE __forceinline
#elif (defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE
#endif
#endif

//---------------------------------------------------------------------
// fast copy for different sizes
//---------------------------------------------------------------------
static INLINE void memcpy_sse2_16(void *dst, const void *src)
{
	__m128i m0 = _mm_loadu_si128(((const __m128i *) src) + 0);
	_mm_storeu_si128(((__m128i *) dst) + 0, m0);
}

static INLINE void memcpy_sse2_32(void *dst, const void *src)
{
	__m128i m0 = _mm_loadu_si128(((const __m128i *) src) + 0);
	__m128i m1 = _mm_loadu_si128(((const __m128i *) src) + 1);
	_mm_storeu_si128(((__m128i *) dst) + 0, m0);
	_mm_storeu_si128(((__m128i *) dst) + 1, m1);
}

static INLINE void memcpy_sse2_64(void *dst, const void *src)
{
	__m128i m0 = _mm_loadu_si128(((const __m128i *) src) + 0);
	__m128i m1 = _mm_loadu_si128(((const __m128i *) src) + 1);
	__m128i m2 = _mm_loadu_si128(((const __m128i *) src) + 2);
	__m128i m3 = _mm_loadu_si128(((const __m128i *) src) + 3);
	_mm_storeu_si128(((__m128i *) dst) + 0, m0);
	_mm_storeu_si128(((__m128i *) dst) + 1, m1);
	_mm_storeu_si128(((__m128i *) dst) + 2, m2);
	_mm_storeu_si128(((__m128i *) dst) + 3, m3);
}

static INLINE void memcpy_sse2_128(void *dst, const void *src)
{
	__m128i m0 = _mm_loadu_si128(((const __m128i *) src) + 0);
	__m128i m1 = _mm_loadu_si128(((const __m128i *) src) + 1);
	__m128i m2 = _mm_loadu_si128(((const __m128i *) src) + 2);
	__m128i m3 = _mm_loadu_si128(((const __m128i *) src) + 3);
	__m128i m4 = _mm_loadu_si128(((const __m128i *) src) + 4);
	__m128i m5 = _mm_loadu_si128(((const __m128i *) src) + 5);
	__m128i m6 = _mm_loadu_si128(((const __m128i *) src) + 6);
	__m128i m7 = _mm_loadu_si128(((const __m128i *) src) + 7);
	_mm_storeu_si128(((__m128i *) dst) + 0, m0);
	_mm_storeu_si128(((__m128i *) dst) + 1, m1);
	_mm_storeu_si128(((__m128i *) dst) + 2, m2);
	_mm_storeu_si128(((__m128i *) dst) + 3, m3);
	_mm_storeu_si128(((__m128i *) dst) + 4, m4);
	_mm_storeu_si128(((__m128i *) dst) + 5, m5);
	_mm_storeu_si128(((__m128i *) dst) + 6, m6);
	_mm_storeu_si128(((__m128i *) dst) + 7, m7);
}

//---------------------------------------------------------------------
// tiny memory copy with jump table optimized
//---------------------------------------------------------------------
static INLINE void *memcpy_tiny(void *dst, const void *src, size_t size)
{
	unsigned char *dd = ((unsigned char*) dst) + size;
	const unsigned char *ss = ((const unsigned char*) src) + size;

	switch (size)
	{
	case 64:
		memcpy_sse2_64(dd - 64, ss - 64);
	case 0:
		break;

	case 65:
		memcpy_sse2_64(dd - 65, ss - 65);
	case 1:
		dd[-1] = ss[-1];
		break;

	case 66:
		memcpy_sse2_64(dd - 66, ss - 66);
	case 2:
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 67:
		memcpy_sse2_64(dd - 67, ss - 67);
	case 3:
		*((uint16_t*) (dd - 3)) = *((uint16_t*) (ss - 3));
		dd[-1] = ss[-1];
		break;

	case 68:
		memcpy_sse2_64(dd - 68, ss - 68);
	case 4:
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 69:
		memcpy_sse2_64(dd - 69, ss - 69);
	case 5:
		*((uint32_t*) (dd - 5)) = *((uint32_t*) (ss - 5));
		dd[-1] = ss[-1];
		break;

	case 70:
		memcpy_sse2_64(dd - 70, ss - 70);
	case 6:
		*((uint32_t*) (dd - 6)) = *((uint32_t*) (ss - 6));
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 71:
		memcpy_sse2_64(dd - 71, ss - 71);
	case 7:
		*((uint32_t*) (dd - 7)) = *((uint32_t*) (ss - 7));
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 72:
		memcpy_sse2_64(dd - 72, ss - 72);
	case 8:
		*((uint64_t*) (dd - 8)) = *((uint64_t*) (ss - 8));
		break;

	case 73:
		memcpy_sse2_64(dd - 73, ss - 73);
	case 9:
		*((uint64_t*) (dd - 9)) = *((uint64_t*) (ss - 9));
		dd[-1] = ss[-1];
		break;

	case 74:
		memcpy_sse2_64(dd - 74, ss - 74);
	case 10:
		*((uint64_t*) (dd - 10)) = *((uint64_t*) (ss - 10));
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 75:
		memcpy_sse2_64(dd - 75, ss - 75);
	case 11:
		*((uint64_t*) (dd - 11)) = *((uint64_t*) (ss - 11));
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 76:
		memcpy_sse2_64(dd - 76, ss - 76);
	case 12:
		*((uint64_t*) (dd - 12)) = *((uint64_t*) (ss - 12));
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 77:
		memcpy_sse2_64(dd - 77, ss - 77);
	case 13:
		*((uint64_t*) (dd - 13)) = *((uint64_t*) (ss - 13));
		*((uint32_t*) (dd - 5)) = *((uint32_t*) (ss - 5));
		dd[-1] = ss[-1];
		break;

	case 78:
		memcpy_sse2_64(dd - 78, ss - 78);
	case 14:
		*((uint64_t*) (dd - 14)) = *((uint64_t*) (ss - 14));
		*((uint64_t*) (dd - 8)) = *((uint64_t*) (ss - 8));
		break;

	case 79:
		memcpy_sse2_64(dd - 79, ss - 79);
	case 15:
		*((uint64_t*) (dd - 15)) = *((uint64_t*) (ss - 15));
		*((uint64_t*) (dd - 8)) = *((uint64_t*) (ss - 8));
		break;

	case 80:
		memcpy_sse2_64(dd - 80, ss - 80);
	case 16:
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 81:
		memcpy_sse2_64(dd - 81, ss - 81);
	case 17:
		memcpy_sse2_16(dd - 17, ss - 17);
		dd[-1] = ss[-1];
		break;

	case 82:
		memcpy_sse2_64(dd - 82, ss - 82);
	case 18:
		memcpy_sse2_16(dd - 18, ss - 18);
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 83:
		memcpy_sse2_64(dd - 83, ss - 83);
	case 19:
		memcpy_sse2_16(dd - 19, ss - 19);
		*((uint16_t*) (dd - 3)) = *((uint16_t*) (ss - 3));
		dd[-1] = ss[-1];
		break;

	case 84:
		memcpy_sse2_64(dd - 84, ss - 84);
	case 20:
		memcpy_sse2_16(dd - 20, ss - 20);
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 85:
		memcpy_sse2_64(dd - 85, ss - 85);
	case 21:
		memcpy_sse2_16(dd - 21, ss - 21);
		*((uint32_t*) (dd - 5)) = *((uint32_t*) (ss - 5));
		dd[-1] = ss[-1];
		break;

	case 86:
		memcpy_sse2_64(dd - 86, ss - 86);
	case 22:
		memcpy_sse2_16(dd - 22, ss - 22);
		*((uint32_t*) (dd - 6)) = *((uint32_t*) (ss - 6));
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 87:
		memcpy_sse2_64(dd - 87, ss - 87);
	case 23:
		memcpy_sse2_16(dd - 23, ss - 23);
		*((uint32_t*) (dd - 7)) = *((uint32_t*) (ss - 7));
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 88:
		memcpy_sse2_64(dd - 88, ss - 88);
	case 24:
		memcpy_sse2_16(dd - 24, ss - 24);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 89:
		memcpy_sse2_64(dd - 89, ss - 89);
	case 25:
		memcpy_sse2_16(dd - 25, ss - 25);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 90:
		memcpy_sse2_64(dd - 90, ss - 90);
	case 26:
		memcpy_sse2_16(dd - 26, ss - 26);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 91:
		memcpy_sse2_64(dd - 91, ss - 91);
	case 27:
		memcpy_sse2_16(dd - 27, ss - 27);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 92:
		memcpy_sse2_64(dd - 92, ss - 92);
	case 28:
		memcpy_sse2_16(dd - 28, ss - 28);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 93:
		memcpy_sse2_64(dd - 93, ss - 93);
	case 29:
		memcpy_sse2_16(dd - 29, ss - 29);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 94:
		memcpy_sse2_64(dd - 94, ss - 94);
	case 30:
		memcpy_sse2_16(dd - 30, ss - 30);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 95:
		memcpy_sse2_64(dd - 95, ss - 95);
	case 31:
		memcpy_sse2_16(dd - 31, ss - 31);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 96:
		memcpy_sse2_64(dd - 96, ss - 96);
	case 32:
		memcpy_sse2_32(dd - 32, ss - 32);
		break;

	case 97:
		memcpy_sse2_64(dd - 97, ss - 97);
	case 33:
		memcpy_sse2_32(dd - 33, ss - 33);
		dd[-1] = ss[-1];
		break;

	case 98:
		memcpy_sse2_64(dd - 98, ss - 98);
	case 34:
		memcpy_sse2_32(dd - 34, ss - 34);
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 99:
		memcpy_sse2_64(dd - 99, ss - 99);
	case 35:
		memcpy_sse2_32(dd - 35, ss - 35);
		*((uint16_t*) (dd - 3)) = *((uint16_t*) (ss - 3));
		dd[-1] = ss[-1];
		break;

	case 100:
		memcpy_sse2_64(dd - 100, ss - 100);
	case 36:
		memcpy_sse2_32(dd - 36, ss - 36);
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 101:
		memcpy_sse2_64(dd - 101, ss - 101);
	case 37:
		memcpy_sse2_32(dd - 37, ss - 37);
		*((uint32_t*) (dd - 5)) = *((uint32_t*) (ss - 5));
		dd[-1] = ss[-1];
		break;

	case 102:
		memcpy_sse2_64(dd - 102, ss - 102);
	case 38:
		memcpy_sse2_32(dd - 38, ss - 38);
		*((uint32_t*) (dd - 6)) = *((uint32_t*) (ss - 6));
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 103:
		memcpy_sse2_64(dd - 103, ss - 103);
	case 39:
		memcpy_sse2_32(dd - 39, ss - 39);
		*((uint32_t*) (dd - 7)) = *((uint32_t*) (ss - 7));
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 104:
		memcpy_sse2_64(dd - 104, ss - 104);
	case 40:
		memcpy_sse2_32(dd - 40, ss - 40);
		*((uint64_t*) (dd - 8)) = *((uint64_t*) (ss - 8));
		break;

	case 105:
		memcpy_sse2_64(dd - 105, ss - 105);
	case 41:
		memcpy_sse2_32(dd - 41, ss - 41);
		*((uint64_t*) (dd - 9)) = *((uint64_t*) (ss - 9));
		dd[-1] = ss[-1];
		break;

	case 106:
		memcpy_sse2_64(dd - 106, ss - 106);
	case 42:
		memcpy_sse2_32(dd - 42, ss - 42);
		*((uint64_t*) (dd - 10)) = *((uint64_t*) (ss - 10));
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 107:
		memcpy_sse2_64(dd - 107, ss - 107);
	case 43:
		memcpy_sse2_32(dd - 43, ss - 43);
		*((uint64_t*) (dd - 11)) = *((uint64_t*) (ss - 11));
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 108:
		memcpy_sse2_64(dd - 108, ss - 108);
	case 44:
		memcpy_sse2_32(dd - 44, ss - 44);
		*((uint64_t*) (dd - 12)) = *((uint64_t*) (ss - 12));
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 109:
		memcpy_sse2_64(dd - 109, ss - 109);
	case 45:
		memcpy_sse2_32(dd - 45, ss - 45);
		*((uint64_t*) (dd - 13)) = *((uint64_t*) (ss - 13));
		*((uint32_t*) (dd - 5)) = *((uint32_t*) (ss - 5));
		dd[-1] = ss[-1];
		break;

	case 110:
		memcpy_sse2_64(dd - 110, ss - 110);
	case 46:
		memcpy_sse2_32(dd - 46, ss - 46);
		*((uint64_t*) (dd - 14)) = *((uint64_t*) (ss - 14));
		*((uint64_t*) (dd - 8)) = *((uint64_t*) (ss - 8));
		break;

	case 111:
		memcpy_sse2_64(dd - 111, ss - 111);
	case 47:
		memcpy_sse2_32(dd - 47, ss - 47);
		*((uint64_t*) (dd - 15)) = *((uint64_t*) (ss - 15));
		*((uint64_t*) (dd - 8)) = *((uint64_t*) (ss - 8));
		break;

	case 112:
		memcpy_sse2_64(dd - 112, ss - 112);
	case 48:
		memcpy_sse2_32(dd - 48, ss - 48);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 113:
		memcpy_sse2_64(dd - 113, ss - 113);
	case 49:
		memcpy_sse2_32(dd - 49, ss - 49);
		memcpy_sse2_16(dd - 17, ss - 17);
		dd[-1] = ss[-1];
		break;

	case 114:
		memcpy_sse2_64(dd - 114, ss - 114);
	case 50:
		memcpy_sse2_32(dd - 50, ss - 50);
		memcpy_sse2_16(dd - 18, ss - 18);
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 115:
		memcpy_sse2_64(dd - 115, ss - 115);
	case 51:
		memcpy_sse2_32(dd - 51, ss - 51);
		memcpy_sse2_16(dd - 19, ss - 19);
		*((uint16_t*) (dd - 3)) = *((uint16_t*) (ss - 3));
		dd[-1] = ss[-1];
		break;

	case 116:
		memcpy_sse2_64(dd - 116, ss - 116);
	case 52:
		memcpy_sse2_32(dd - 52, ss - 52);
		memcpy_sse2_16(dd - 20, ss - 20);
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 117:
		memcpy_sse2_64(dd - 117, ss - 117);
	case 53:
		memcpy_sse2_32(dd - 53, ss - 53);
		memcpy_sse2_16(dd - 21, ss - 21);
		*((uint32_t*) (dd - 5)) = *((uint32_t*) (ss - 5));
		dd[-1] = ss[-1];
		break;

	case 118:
		memcpy_sse2_64(dd - 118, ss - 118);
	case 54:
		memcpy_sse2_32(dd - 54, ss - 54);
		memcpy_sse2_16(dd - 22, ss - 22);
		*((uint32_t*) (dd - 6)) = *((uint32_t*) (ss - 6));
		*((uint16_t*) (dd - 2)) = *((uint16_t*) (ss - 2));
		break;

	case 119:
		memcpy_sse2_64(dd - 119, ss - 119);
	case 55:
		memcpy_sse2_32(dd - 55, ss - 55);
		memcpy_sse2_16(dd - 23, ss - 23);
		*((uint32_t*) (dd - 7)) = *((uint32_t*) (ss - 7));
		*((uint32_t*) (dd - 4)) = *((uint32_t*) (ss - 4));
		break;

	case 120:
		memcpy_sse2_64(dd - 120, ss - 120);
	case 56:
		memcpy_sse2_32(dd - 56, ss - 56);
		memcpy_sse2_16(dd - 24, ss - 24);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 121:
		memcpy_sse2_64(dd - 121, ss - 121);
	case 57:
		memcpy_sse2_32(dd - 57, ss - 57);
		memcpy_sse2_16(dd - 25, ss - 25);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 122:
		memcpy_sse2_64(dd - 122, ss - 122);
	case 58:
		memcpy_sse2_32(dd - 58, ss - 58);
		memcpy_sse2_16(dd - 26, ss - 26);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 123:
		memcpy_sse2_64(dd - 123, ss - 123);
	case 59:
		memcpy_sse2_32(dd - 59, ss - 59);
		memcpy_sse2_16(dd - 27, ss - 27);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 124:
		memcpy_sse2_64(dd - 124, ss - 124);
	case 60:
		memcpy_sse2_32(dd - 60, ss - 60);
		memcpy_sse2_16(dd - 28, ss - 28);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 125:
		memcpy_sse2_64(dd - 125, ss - 125);
	case 61:
		memcpy_sse2_32(dd - 61, ss - 61);
		memcpy_sse2_16(dd - 29, ss - 29);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 126:
		memcpy_sse2_64(dd - 126, ss - 126);
	case 62:
		memcpy_sse2_32(dd - 62, ss - 62);
		memcpy_sse2_16(dd - 30, ss - 30);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 127:
		memcpy_sse2_64(dd - 127, ss - 127);
	case 63:
		memcpy_sse2_32(dd - 63, ss - 63);
		memcpy_sse2_16(dd - 31, ss - 31);
		memcpy_sse2_16(dd - 16, ss - 16);
		break;

	case 128:
		memcpy_sse2_128(dd - 128, ss - 128);
		break;
	}

	return dst;
}

//---------------------------------------------------------------------
// main routine
//---------------------------------------------------------------------
static void* memcpy_fast(void *destination, const void *source, size_t size)
{
	unsigned char *dst = (unsigned char*) destination;
	const unsigned char *src = (const unsigned char*) source;
	static size_t cachesize = 0x200000; // L2-cache size
	size_t padding;

	// small memory copy
	if (size <= 128)
	{
		return memcpy_tiny(dst, src, size);
	}

	// align destination to 16 bytes boundary
	padding = (16 - (((size_t) dst) & 15)) & 15;

	if (padding > 0)
	{
		__m128i head = _mm_loadu_si128((const __m128i *) src);
		_mm_storeu_si128((__m128i *) dst, head);
		dst += padding;
		src += padding;
		size -= padding;
	}

	// medium size copy
	if (size <= cachesize)
	{
		__m128i c0, c1, c2, c3, c4, c5, c6, c7;

		for (; size >= 128; size -= 128)
		{
			c0 = _mm_loadu_si128(((const __m128i *) src) + 0);
			c1 = _mm_loadu_si128(((const __m128i *) src) + 1);
			c2 = _mm_loadu_si128(((const __m128i *) src) + 2);
			c3 = _mm_loadu_si128(((const __m128i *) src) + 3);
			c4 = _mm_loadu_si128(((const __m128i *) src) + 4);
			c5 = _mm_loadu_si128(((const __m128i *) src) + 5);
			c6 = _mm_loadu_si128(((const __m128i *) src) + 6);
			c7 = _mm_loadu_si128(((const __m128i *) src) + 7);
			_mm_prefetch((const char* )(src + 256), _MM_HINT_NTA);
			src += 128;
			_mm_store_si128((((__m128i *) dst) + 0), c0);
			_mm_store_si128((((__m128i *) dst) + 1), c1);
			_mm_store_si128((((__m128i *) dst) + 2), c2);
			_mm_store_si128((((__m128i *) dst) + 3), c3);
			_mm_store_si128((((__m128i *) dst) + 4), c4);
			_mm_store_si128((((__m128i *) dst) + 5), c5);
			_mm_store_si128((((__m128i *) dst) + 6), c6);
			_mm_store_si128((((__m128i *) dst) + 7), c7);
			dst += 128;
		}
	}
	else
	{		// big memory copy
		__m128i c0, c1, c2, c3, c4, c5, c6, c7;

		_mm_prefetch((const char* )(src), _MM_HINT_NTA);

		if ((((size_t) src) & 15) == 0)
		{	// source aligned
			for (; size >= 128; size -= 128)
			{
				c0 = _mm_load_si128(((const __m128i *) src) + 0);
				c1 = _mm_load_si128(((const __m128i *) src) + 1);
				c2 = _mm_load_si128(((const __m128i *) src) + 2);
				c3 = _mm_load_si128(((const __m128i *) src) + 3);
				c4 = _mm_load_si128(((const __m128i *) src) + 4);
				c5 = _mm_load_si128(((const __m128i *) src) + 5);
				c6 = _mm_load_si128(((const __m128i *) src) + 6);
				c7 = _mm_load_si128(((const __m128i *) src) + 7);
				_mm_prefetch((const char* )(src + 256), _MM_HINT_NTA);
				src += 128;
				_mm_stream_si128((((__m128i *) dst) + 0), c0);
				_mm_stream_si128((((__m128i *) dst) + 1), c1);
				_mm_stream_si128((((__m128i *) dst) + 2), c2);
				_mm_stream_si128((((__m128i *) dst) + 3), c3);
				_mm_stream_si128((((__m128i *) dst) + 4), c4);
				_mm_stream_si128((((__m128i *) dst) + 5), c5);
				_mm_stream_si128((((__m128i *) dst) + 6), c6);
				_mm_stream_si128((((__m128i *) dst) + 7), c7);
				dst += 128;
			}
		}
		else
		{							// source unaligned
			for (; size >= 128; size -= 128)
			{
				c0 = _mm_loadu_si128(((const __m128i *) src) + 0);
				c1 = _mm_loadu_si128(((const __m128i *) src) + 1);
				c2 = _mm_loadu_si128(((const __m128i *) src) + 2);
				c3 = _mm_loadu_si128(((const __m128i *) src) + 3);
				c4 = _mm_loadu_si128(((const __m128i *) src) + 4);
				c5 = _mm_loadu_si128(((const __m128i *) src) + 5);
				c6 = _mm_loadu_si128(((const __m128i *) src) + 6);
				c7 = _mm_loadu_si128(((const __m128i *) src) + 7);
				_mm_prefetch((const char* )(src + 256), _MM_HINT_NTA);
				src += 128;
				_mm_stream_si128((((__m128i *) dst) + 0), c0);
				_mm_stream_si128((((__m128i *) dst) + 1), c1);
				_mm_stream_si128((((__m128i *) dst) + 2), c2);
				_mm_stream_si128((((__m128i *) dst) + 3), c3);
				_mm_stream_si128((((__m128i *) dst) + 4), c4);
				_mm_stream_si128((((__m128i *) dst) + 5), c5);
				_mm_stream_si128((((__m128i *) dst) + 6), c6);
				_mm_stream_si128((((__m128i *) dst) + 7), c7);
				dst += 128;
			}
		}
		_mm_sfence();
	}

	memcpy_tiny(dst, src, size);

	return destination;
}

#endif
