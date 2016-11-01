
#ifndef __FvLogicDebug_H__
#define __FvLogicDebug_H__


#include "FvLogBus.h"
#include <FvDebug.h>
#include <FvKernel.h>

#define FV_ASSERT_WARNING(exp)\
	if (!(exp))\
	{\
		FvLogBus::Warning("%s%d", __FILE__, int(__LINE__));\
	}

#define FV_ASSERT_ERROR(exp)\
	if (!(exp))\
	{\
		FvLogBus::Error("%s%d", __FILE__, int(__LINE__));\
	}\
	FV_ASSERT(exp)


#endif //__FvLogicDebug_H__
