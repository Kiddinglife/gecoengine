#include "FvProcessorAffinity.h"
#include "FvDebug.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvKernel", 0 )

namespace
{
	FvUInt32 ms_uiProcessor = 0;

	FvUInt32 AvailableProcessor( FvUInt32 uiProcessorHint )
	{
		FvUInt32 uiRet = uiProcessorHint;

		DWORD iProcessAffinity = 0;
		DWORD iSystemAffinity = 0;
		HRESULT hr = GetProcessAffinityMask( GetCurrentProcess(), 
			&iProcessAffinity, &iSystemAffinity );

		if (SUCCEEDED(hr) &&
			!(iProcessAffinity & (1 << uiProcessorHint)) )
		{
			for (FvUInt32 i = 0; i < 32; i++)
			{
				if (iProcessAffinity & (1 << i))
				{
					uiRet = i;
					break;
				}
			}
		}

		return uiRet;
	}
}

void FvProcessorAffinity::Set(FvUInt32 uiProcessorHint)
{
    ms_uiProcessor = AvailableProcessor(uiProcessorHint);

    SetThreadAffinityMask(GetCurrentThread(), 1 << ms_uiProcessor );

	if (ms_uiProcessor != uiProcessorHint)
	{
		FV_INFO_MSG( "FvProcessorAffinity::Set - unable to set processor "
			"affinity to %d setting to %d in stead\n", uiProcessorHint, ms_uiProcessor );
	}
}

FvUInt32 FvProcessorAffinity::Get()
{
    return ms_uiProcessor;
}

void FvProcessorAffinity::Update()
{
    Set(ms_uiProcessor);
}