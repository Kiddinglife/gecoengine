//{future header message}
#ifndef __FvGlobalRPCCallBack_H__
#define __FvGlobalRPCCallBack_H__

#include "FvGlobalMailbox.h"



template<class T>
class FvRPCCallback
{
	template<class> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const FvRPCCallback&);
	template<class> friend FvBinaryIStream& operator>>(FvBinaryIStream&, FvRPCCallback&);
public:
	void Invoke(const T& kVal) const
	{
		if(m_uiCBID == 0)
		{
			FV_ERROR_MSG("RPCCallback Invoke Invalid\n");
			return;
		}
		FvRPCCallback& kThis = const_cast<FvRPCCallback&>(*this);
		FvBinaryOStream* pkOS = kThis.m_kMB.RPCCallbackInvokeGetStream(m_uiCBID);
		if(!pkOS) return;
		*pkOS << kVal;
		kThis.m_kMB.RPCCallbackInvokeSendStream();
		kThis.m_uiCBID = 0;
	}
	void Exception() const
	{
		if(m_uiCBID == 0)
		{
			FV_ERROR_MSG("RPCCallback Exception Invalid\n");
			return;
		}
		FvRPCCallback& kThis = const_cast<FvRPCCallback&>(*this);
		kThis.m_kMB.RPCCallbackException(m_uiCBID);
		kThis.m_uiCBID = 0;
	}

	FvRPCCallback():m_uiCBID(0){}
	FvRPCCallback(const FvMailBox& kMB, FvUInt32 uiCBID):m_kMB(kMB),m_uiCBID(uiCBID){}
	FvRPCCallback(const FvRPCCallback& kSrc):m_kMB(kSrc.m_kMB),m_uiCBID(kSrc.m_uiCBID){ const_cast<FvRPCCallback&>(kSrc).m_uiCBID=0; }
	FvRPCCallback& operator=(const FvRPCCallback& kSrc){ m_kMB=kSrc.m_kMB; m_uiCBID=kSrc.m_uiCBID; const_cast<FvRPCCallback&>(kSrc).m_uiCBID=0; return *this; }
	~FvRPCCallback(){ if(m_uiCBID) Exception(); }
	FvUInt32 GetID() const { return m_uiCBID; }
	const FvMailBox& GetMB() const { return m_kMB; }
	void SetMB(const FvMailBox& kMB) { m_kMB=kMB; }
	void SetID(FvUInt32 uiCBID) { m_uiCBID=uiCBID; }

protected:
	FvMailBox	m_kMB;
	FvUInt32	m_uiCBID;
};

template<class T>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvRPCCallback<T>& kData)
{
	kOS << kData.m_kMB << kData.m_uiCBID;
	const_cast<FvRPCCallback<T>&>(kData).m_uiCBID = 0;
	return kOS;
}

template<class T>
FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvRPCCallback<T>& kData)
{
	if(kIS.Error()) return kIS;
	return kIS >> kData.m_kMB >> kData.m_uiCBID;
}

namespace FvDataPrint
{
	template<class T>
	void Print(const char* pcName, const FvRPCCallback<T>& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s<", pcName);
		FvDataPrint::Print("MB", static_cast<const FvObjMailBox&>(kVal.GetMB()), kMsg);
		FvDataPrint::Print(", ID", kVal.GetID(), kMsg);
		kMsg.append(">");
	}
	template<class T>
	void Print(const FvRPCCallback<T>& kVal, FvString& kMsg)
	{
		kMsg.append("<MB:");
		FvDataPrint::Print(static_cast<const FvObjMailBox&>(kVal.GetMB()), kMsg);
		kMsg.append("ID:");
		FvDataPrint::Print(kVal.GetID(), kMsg);
		kMsg.append(">");
	}
}

void CallRPCCallbackException(FvInt32 iMethodIdx, FvBinaryIStream& kIS);


#endif//__FvGlobalRPCCallBack_H__