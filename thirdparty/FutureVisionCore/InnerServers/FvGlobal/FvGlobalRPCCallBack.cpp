#include "FvGlobalRPCCallBack.h"


void CallRPCCallbackException(FvInt32 iMethodIdx, FvBinaryIStream& kIS)
{
	FvUInt8 uiCBCnt(iMethodIdx>>24);
	if(uiCBCnt)
	{
		FvRPCCallback<int> kCB;
		for(FvUInt8 i=0; i<uiCBCnt; ++i)
		{
			kIS >> kCB;
			kCB.Exception();
		}
	}
}

