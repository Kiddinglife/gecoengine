//{future header message}
#ifndef __FvRobotRPCCallBack_H__
#define __FvRobotRPCCallBack_H__

#include "FvRobotMailbox.h"


template<class T>
class FvRPCCallback
{
	template<class> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const FvRPCCallback&);
public:
	void Exception() const
	{
		if(m_uiCBID == 0)
		{
			FV_ERROR_MSG("RPCCallback Exception Invalid\n");
			return;
		}
		CallRPCCallbackException(m_uiCBID);
		FvRPCCallback& kThis = const_cast<FvRPCCallback&>(*this);
		kThis.m_uiCBID = 0;
	}

	FvRPCCallback():m_uiCBID(0){}
	FvRPCCallback(FvUInt32 uiCBID):m_uiCBID(uiCBID){}
	FvRPCCallback(const FvRPCCallback& kSrc):m_uiCBID(kSrc.m_uiCBID){ const_cast<FvRPCCallback&>(kSrc).m_uiCBID=0; }
	FvRPCCallback& operator=(const FvRPCCallback& kSrc){ m_uiCBID=kSrc.m_uiCBID; const_cast<FvRPCCallback&>(kSrc).m_uiCBID=0; return *this; }
	~FvRPCCallback(){ if(m_uiCBID) Exception(); }
	FvUInt8 GetID() const { return m_uiCBID; }

protected:
	FvUInt8	m_uiCBID;
};

template<class T>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvRPCCallback<T>& kData)
{
	kOS << kData.m_uiCBID;
	const_cast<FvRPCCallback<T>&>(kData).m_uiCBID = 0;
	return kOS;
}

namespace FvDataPrint
{
	template<class T>
	void Print(const char* pcName, const FvRPCCallback<T>& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s<", pcName);
		FvDataPrint::Print("ID", kVal.GetID(), kMsg);
		kMsg.append(">");
	}
	template<class T>
	void Print(const FvRPCCallback<T>& kVal, FvString& kMsg)
	{
		kMsg.append("<ID:");
		FvDataPrint::Print(kVal.GetID(), kMsg);
		kMsg.append(">");
	}
}

void CallRPCCallbackException(FvUInt8 uiCBID);



#endif//__FvRobotRPCCallBack_H__