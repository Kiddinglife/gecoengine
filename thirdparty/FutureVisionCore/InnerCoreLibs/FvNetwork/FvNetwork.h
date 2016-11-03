//{future header message}
#ifndef __FvNetwork_H__
#define __FvNetwork_H__

#include <FvPowerDefines.h>

#ifdef FV_AS_STATIC_LIBS
#define FV_NETWORK_API
#else
#ifdef FV_NETWORK_EXPORT
#define FV_NETWORK_API FV_Proper_Export_Flag
#else
#define FV_NETWORK_API FV_Proper_Import_Flag
#endif
#endif

#endif // __FvNetwork_H__