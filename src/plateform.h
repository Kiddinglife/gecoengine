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

#ifdef CODE_INLINE
#define INLINE    inline
#else
#define INLINE
#endif

/* common includes */
#include <limits>
#include <math.h>
/* 360 includes */

/* PS3 includes */
#if defined( PLAYSTATION3 )
// _BIG_ENDIAN is a built in define
#include <stdlib.h>
#include <sys/sys_time.h>
#include <sys/timer.h>
#include <sys/time_util.h>
#include <sys/select.h>
#endif

#ifdef _WIN32
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
#define PRId64 "%lld"
#define PRIu64 "%llu"
#define PRIx64 "%llx"
#define PRIX64 "%llX"
#define PRIu32 "%lu"
#define PRId32 "%ld"
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
#define PRId64 "%lld"
#define PRIu64  "%llu" 
#define PRIx64 "%llx"
#define PRIX64 "%llX"
#define PRIu32 "%lu"
#define PRId32 "%ld"
#else //unix or linux
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
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
#define PRId64 "%ld"
#define PRIu64 "%lu"
#define PRIx64 "%lx"
#define PRIX64 "%lX"
#else
typedef int32 intptr;
typedef uint32 uintptr;
#define PRId64 "%lld"
#define PRIu64 "%llu"
#define PRIx64 "%llx"
#define PRIX64 "%llX"
#endif
#ifndef PRId32
#define PRId32 "%zd"
#endif
#ifndef PRIu32
#define PRIu32 "%zu"
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
{   return b < a ? b : a;}
template <class T> inline const T & max(const T & a, const T & b)
{   return a < b ? b : a;}
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
inline bool almost_equal(const float f1, const float f2, const float epsilon = 0.0004f)
{
    return fabsf(f1 - f2) < epsilon;
}
inline bool almost_equal(const double d1, const double d2, const double epsilon = 0.0004)
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
inline bool almost_equal(const T& c1, const T& c2, const float epsilon = 0.0004f)
{
    if (c1.size() != c2.size()) return false;
    typename T::const_iterator iter1 = c1.begin();
    typename T::const_iterator iter2 = c2.begin();
    for (; iter1 != c1.end(); ++iter1, ++iter2)
        if (!almost_equal(*iter1, *iter2, epsilon)) return false;
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

#endif
