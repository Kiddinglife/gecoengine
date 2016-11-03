//{future header message}
#ifndef __FvNetBSDSNPrintf_H__
#define __FvNetBSDSNPrintf_H__

#include "FvNetFormatStringHandler.h"

#include <FvPowerDefines.h>
#include <FvBaseTypes.h>

#ifdef FV_HAVE_LONG_DOUBLE
# define FV_NET_LDOUBLE long double
#else
# define FV_NET_LDOUBLE double
#endif

#define FV_NET_LLONG FvInt64
#define FV_NET_ULLONG FvUInt64

#define FV_NET_DP_C_SHORT   1
#define FV_NET_DP_C_LONG    2
#define FV_NET_DP_C_LDOUBLE 3
#define FV_NET_DP_C_LLONG   4

#define FV_NET_VARIABLE_MIN_WIDTH 1
#define FV_NET_VARIABLE_MAX_WIDTH 2
typedef short FvNetWidthType;

#include <string>

void HandleFormatString( const char *pcFormat, FvNetFormatStringHandler &kFSH );

void BSDFormatString( const char *pcValue, int iFlags, int iMin, int iMax,
	FvString &kOut );

void BSDFormatInt( FV_NET_LLONG iValue, FvUInt8 uiBase, int iMin, int iMax, int iFlags,
	FvString &kOut );

void BSDFormatFloat( FV_NET_LDOUBLE fValue, int iMin, int iMax, int iFlags,
	FvString &kOut );

#endif // __FvNetBSDSNPrintf_H__
