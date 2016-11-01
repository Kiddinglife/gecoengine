//{future header message}
#ifndef __FvPrintf_H__
#define __FvPrintf_H__

#include "FvPowerDefines.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef FV_ENABLE_DPRINTF
#define FV_ENABLE_DPRINTF 1
#endif // FV_ENABLE_DPRINTF

	FV_POWER_EXTERN_VAR extern int g_iShouldWriteToConsole;

#if FV_ENABLE_DPRINTF
#ifndef _WIN32
	void FV_POWER_API FvDPrintfNormal( const char *pcFormat, ... )
		__attribute__ ( (pcFormat (printf, 1, 2 ) ) );
	void FV_POWER_API FvDPrintfPriority( int iComponentPriority, int iMessagePriority,
		const char *pcFormat, ...)
		__attribute__ ( (pcFormat (printf, 3, 4 ) ) );
#else // !_WIN32
	void FV_POWER_API FvDPrintfNormal( const char *pcFormat, ... );
	void FV_POWER_API FvDPrintfPriority( int iComponentPriority, int iMessagePriority,
		const char *pcFormat, ...);
#endif // _WIN32

	void FV_POWER_API FvVDPrintfNormal( const char *pcFormat, va_list kArgPtr,
		const char *pcPrefix );
	void FV_POWER_API FvVDPrintfPriority( int iComponentPriority, int iMessagePriority,
		const char *pcFormat, va_list kArgPtr, const char *pcPrefix );
#else // FV_ENABLE_DPRINTF
	void FV_POWER_API FvDPrintfNormal( const char *pcFormat, ... ) {}
	void FV_POWER_API FvDPrintfPriority( int iComponentPriority, int iMessagePriority,
						const char *pcFormat, ...) {}
	void FV_POWER_API FvVDPrintfNormal( const char *pcFormat, va_list kArgPtr,
						 const char *pcPrefix ) {}
	void FV_POWER_API FvVDPrintfPriority( int iComponentPriority, int iMessagePriority,
						 const char *pcFormat, va_list kArgPtr, const char *pcPrefix ) {}
#endif  // !FV_ENABLE_DPRINTF

#ifdef __cplusplus
}
#endif // __cplusplus

#if defined( unix ) || defined( PLAYSTATION3 )
#define FvIsNAN isnan
#define FvIsINF isinf
#define FvSNPrintf snprintf
#define FvVSNPrintf vsnprintf
#define FvSNWPrintf snwprintf
#define FvStriCMP strcasecmp
#define FvStrNiCMP strncasecmp
#define FvFileNo fileno
#else // (unix || PLAYSTATION3)
#define FvIsNAN _isnan
#define FvIsINF(x) (!_finite(x) && !_isnan(x))
#define FvSNPrintf _snprintf
#define FvVSNPrintf _vsnprintf
#define FvSNWPrintf _snwprintf
#define FvStriCMP _stricmp
#define FvStrNiCMP _strnicmp
#define FvFileNo _fileno
#endif // !(unix || PLAYSTATION3)

#endif // __FvPrintf_H__
