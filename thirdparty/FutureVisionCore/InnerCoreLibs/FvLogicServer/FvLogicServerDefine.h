//{future header message}
#ifndef __FvLogicServerDefine_H__
#define __FvLogicServerDefine_H__


#include <FvKernel.h>

#ifdef FV_AS_STATIC_LIBS
#define FV_LOGIC_SERVER_API
#else
#ifdef FV_LOGIC_SERVER_EXPORT
#define FV_LOGIC_SERVER_API FV_Proper_Export_Flag
#else
#define FV_LOGIC_SERVER_API FV_Proper_Import_Flag
#endif
#endif

#include <FvLogicDebug.h>


#endif//__FvLogicCommonDefine_H__