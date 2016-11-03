//{future header message}
#ifndef __FvPowerDefines_H__
#define __FvPowerDefines_H__

#include <string.h>

#include "FvConfig.h"
#include "FvExport.h"

#if defined(_MSC_VER)

#define FV_FAST_CALL __fastcall
#else // _MSC_VER
#define FV_FAST_CALL
#endif // !_MSC_VER

#ifdef FV_AS_STATIC_LIBS
#define FV_POWER_API FV_FAST_CALL
#define FV_POWER_EXTERN_VAR 
#else // FV_AS_STATIC_LIBS
#ifdef FV_POWER_EXPORT
#define FV_POWER_API FV_Proper_Export_Flag FV_FAST_CALL
#define FV_POWER_EXTERN_VAR FV_Proper_Export_Flag
#else // FV_POWER_EXPORT
#define FV_POWER_API FV_Proper_Import_Flag FV_FAST_CALL
#define FV_POWER_EXTERN_VAR FV_Proper_Import_Flag
#endif // !FV_POWER_EXPORT
#endif // FV_AS_STATIC_LIBS

#define FV_BOOL unsigned int
#define FV_TRUE 1
#define FV_FALSE 0

#define FV_CODE_INLINE 1

# if !defined (FV_CODE_INLINE)
#	define FV_INLINE 
# else
#	define FV_INLINE inline
# endif /* !defined (FV_CODE_INLINE) */

#define FV_ARRAYSZ(v)	 (sizeof(v) / sizeof(v[0]))
#define FV_ARRAY_SIZE(v) (sizeof(v) / sizeof(v[0]))

#define FV_ENTER_DEBUGGER() __asm { int 3 }

#if !defined (FV_HOST_NAME_MAX)
#  define FV_HOST_NAME_MAX 256
#endif /* !FV_HOST_NAME_MAX */

#if !defined (FV_PATH_MAX)
#  if defined (_MAX_PATH)
#    define FV_PATH_MAX _MAX_PATH
#  elif defined (MAX_PATH)
#    define FV_PATH_MAX MAX_PATH
#  else /* !_MAX_PATH */
#    define FV_PATH_MAX 1024
#  endif /* _MAX_PATH */
#endif /* !FV_PATH_MAX */

#define FV_MIN(a, b) (((b) < (a)) ? (b) : (a))
#define FV_MAX(a, b) (((a) < (b)) ? (b) : (a))

# include /**/ <limits.h>

// A char always has 1 byte, by definition.
# define FV_SIZEOF_CHAR 1

// The number of bytes in a short.
# if !defined (FV_SIZEOF_SHORT)
#   if (USHRT_MAX) == 255U
#     define FV_SIZEOF_SHORT 1
#   elif (USHRT_MAX) == 65535U
#     define FV_SIZEOF_SHORT 2
#   elif (USHRT_MAX) == 4294967295U
#     define FV_SIZEOF_SHORT 4
#   elif (USHRT_MAX) == 18446744073709551615U
#     define FV_SIZEOF_SHORT 8
#   else
#     error: unsupported short size, must be updated for this platform!
#   endif /* USHRT_MAX */
# endif /* !defined (FV_SIZEOF_SHORT) */

// The number of bytes in an int.
# if !defined (FV_SIZEOF_INT)
#   if (UINT_MAX) == 65535U
#     define FV_SIZEOF_INT 2
#   elif (UINT_MAX) == 4294967295U
#     define FV_SIZEOF_INT 4
#   elif (UINT_MAX) == 18446744073709551615U
#     define FV_SIZEOF_INT 8
#   else
#     error: unsupported int size, must be updated for this platform!
#   endif /* UINT_MAX */
# endif /* !defined (FV_SIZEOF_INT) */

// The number of bytes in a long.
# if !defined (FV_SIZEOF_LONG)
#   if (ULONG_MAX) == 65535UL
#     define FV_SIZEOF_LONG 2
#   elif ((ULONG_MAX) == 4294967295UL)
#     define FV_SIZEOF_LONG 4
#   elif ((ULONG_MAX) == 18446744073709551615UL)
#     define FV_SIZEOF_LONG 8
#   else
#     error: unsupported long size, must be updated for this platform!
#   endif /* ULONG_MAX */
# endif /* !defined (FV_SIZEOF_LONG) */

// The number of bytes in a long long.
# if !defined (FV_SIZEOF_LONG_LONG)
#   if defined (FV_LACKS_LONGLONG_T)
#     define FV_SIZEOF_LONG_LONG 8
#   elif defined (ULLONG_MAX)
#     if ((ULLONG_MAX) == 4294967295ULL)
#       define FV_SIZEOF_LONG_LONG 4
#     elif ((ULLONG_MAX) == 18446744073709551615ULL)
#       define FV_SIZEOF_LONG_LONG 8
#     endif
#   elif defined (ULONGLONG_MAX)
#     if ((ULONGLONG_MAX) == 4294967295ULL)
#       define FV_SIZEOF_LONG_LONG 4
#     elif ((ULONGLONG_MAX) == 18446744073709551615ULL)
#       define FV_SIZEOF_LONG_LONG 8
#     endif
#   endif
#   // If we can't determine the size of long long, assume it is 8
#   // instead of erroring out.  (Either ULLONG_MAX and ULONGLONG_MAX
#   // may not be supported; or an extended C/C++ dialect may need to
#   // be selected.  If this assumption is wrong, it can be addressed
#   // in the platform-specific config header.
#   if !defined (FV_SIZEOF_LONG_LONG)
#     define FV_SIZEOF_LONG_LONG 8
#   endif
# endif /* !defined (FV_SIZEOF_LONG_LONG) */

#if defined (FV_INT8_TYPE)
typedef FV_INT8_TYPE          FvInt8;
#elif defined (FV_HAS_INT8_T)
typedef int8_t                FvInt8;
#elif !defined (FV_LACKS_SIGNED_CHAR)
typedef signed char           FvInt8;
#else
typedef char                  FvInt8;
#endif /* defined (FV_INT8_TYPE) */

#if defined (FV_UINT8_TYPE)
typedef FR_UINT8_TYPE         FvUInt8;
#elif defined (FR_HAS_UINT8_T)
typedef uint8_t               FvUInt8;
#else
typedef unsigned char         FvUInt8;
#endif /* defined (FV_UINT8_TYPE) */

#if defined (FV_INT16_TYPE)
typedef FV_INT16_TYPE         FvInt16;
#elif defined (FV_HAS_INT16_T)
typedef int16_t               FvInt16;
#elif FV_SIZEOF_SHORT == 2
typedef short                 FvInt16;
#elif FV_SIZEOF_INT == 2
typedef int                   FvInt16;
#else
# error Have to add to the FvInt16 type setting
#endif  /* defined (FV_INT16_TYPE) */

#if defined (FV_UINT16_TYPE)
typedef FV_UINT16_TYPE        FvUInt16;
#elif defined (FV_HAS_UINT16_T)
typedef uint16_t              FvUInt16;
#elif FV_SIZEOF_SHORT == 2
typedef unsigned short        FvUInt16;
#elif FV_SIZEOF_INT == 2
typedef unsigned int          FvUInt16;
#else
# error Have to add to the FvUInt16 type setting
#endif /* defined (FV_UINT16_TYPE) */

#if defined (FV_INT32_TYPE)
typedef FV_INT32_TYPE         FvInt32;
#elif defined (FV_HAS_INT32_T)
typedef int32_t               FvInt32;
#elif FV_SIZEOF_INT == 4
typedef int                   FvInt32;
#elif FV_SIZEOF_LONG == 4
typedef long                  FvInt32;
#else
# error Have to add to the FvInt32 type setting
#endif /* defined (FV_INT32_TYPE) */

#if defined (FV_UINT32_TYPE)
typedef FV_UINT32_TYPE        FvUInt32;
#elif defined (FV_HAS_UINT32_T)
typedef uint32_t              FvUInt32;
#elif FV_SIZEOF_INT == 4
typedef unsigned int          FvUInt32;
#elif FV_SIZEOF_LONG == 4
typedef unsigned long         FvUInt32;
#else
# error Have to add to the FvUInt32 type setting
#endif /* defined (FV_UINT32_TYPE) */

#if defined (FV_INT64_TYPE)
typedef FV_INT64_TYPE         FvInt64;
#elif defined (FV_HAS_INT64_T)
typedef int64_t               FvInt64;
#elif FV_SIZEOF_LONG == 8
typedef long                  FvInt64;
#elif !defined (FV_LACKS_LONGLONG_T) && FV_SIZEOF_LONG_LONG == 8
# ifdef __GNUC__
// Silence g++ "-pedantic" warnings regarding use of "long long"
// type.
__extension__
# endif  /* __GNUC__ */
typedef long long             FvInt64;
#endif /* defined (FV_INT64_TYPE) */

#if !(defined (FV_LACKS_LONGLONG_T) || defined (FV_LACKS_UNSIGNEDLONGLONG_T))
/* See matching #if around FV_U_LongLong class declaration below */

#  if defined (FV_UINT64_TYPE)
typedef FV_UINT64_TYPE        FvUInt64;
#  elif defined (FV_HAS_UINT64_T)
typedef uint64_t              FvUInt64;
#  elif FV_SIZEOF_LONG == 8
typedef unsigned long         FvUInt64;
#  elif FV_SIZEOF_LONG_LONG == 8
# ifdef __GNUC__
// Silence g++ "-pedantic" warnings regarding use of "long long"
// type.
__extension__
# endif  /* __GNUC__ */
typedef unsigned long long    FvUInt64;
#  endif /* defined (FV_UINT64_TYPE) */
#endif /* !(FV_LACKS_LONGLONG_T || FV_LACKS_UNSIGNEDLONGLONG_T) */

typedef unsigned char FvByte;

/// 下面代码可能考虑移动到其他头文件

#if defined( PLAYSTATION3 )
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

#ifdef _WIN32
typedef INT_PTR	FvIntPtr;
typedef UINT_PTR FvUIntPtr;
#define PRIu64 "%llu"
#else // _WIN32
#	ifdef _LP64
typedef FvInt64	FvIntPtr;
typedef FvUInt64 FvUIntPtr;
#define PRIu64 "%lu"
#	else // _LP64
typedef FvInt32	FvIntPtr;
typedef FvUInt32 FvUIntPtr;
#define PRIu64 "%llu"
#	endif // !_LP64
#endif // !_WIN32

#ifdef FV_USE_ASSERTS
#	ifndef NDEBUG
#		undef NDEBUG
#		include <assert.h>
#		define NDEBUG
#	else
#		include <assert.h>
#	endif
#	define FV_CASSERT assert
#else // FV_USE_ASSERTS
#	define FV_CASSERT
#endif // !FV_USE_ASSERTS

#define FvZeroMemory(dst,len) (memset((dst),0,(len)))
#define FvZeroMemoryEx(dst,size,count) (FvZeroMemory((dst),(size)*(count)))

#define FvMemoryCopy(dst,src,len) (memcpy_s((dst),(len),(src),(len)))

#define FvCrazyCopy(dst,src,len) (memcpy_s((dst),(len),(src),(len)))
#define FvCrazyCopyEx(dst,src,size,count) (FvCrazyCopy((dst),(src),(size)*(count)))

#define FV_MASK(location) (0x01 << (location))
#define FV_MASK_HAS_ANY(flag,mask) (((flag) & (mask)) != 0)
#define FV_MASK_HAS_ALL(flag,mask) (((flag) & (mask)) == (mask))
#define FV_MASK_ADD(flag,mask) ((flag) |= (mask))
#define FV_MASK_DEL(flag,mask) ((flag) &= (~(mask)))
#define FV_MASK_REMOVE(flag,mask) ((flag) & (~(mask)))
#define FV_MASK_CHANGE(location,flag)									\
	if(FV_MASK_HAS_ANY(flag,FV_MASK(location)))							\
		FV_MASK_DEL(flag,FV_MASK(location));							\
	else																\
		FV_MASK_ADD(flag,FV_MASK(location));


#endif /* __FvPowerDefines_H__ */
