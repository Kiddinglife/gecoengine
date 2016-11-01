//{future header message}
#ifndef __FvProcessAffinity_H__
#define __FvProcessAffinity_H__

#include "FvKernel.h"
#include <FvPowerDefines.h>

namespace FvProcessorAffinity
{
	void FV_KERNEL_API Set(FvUInt32 uiProcessorHint = 0);
	FvUInt32 FV_KERNEL_API Get();
	void FV_KERNEL_API Update();
};

#endif // __FvProcessAffinity_H__
