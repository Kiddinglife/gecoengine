//{future header message}
#ifndef __FvGlobalMailBox_H__
#define __FvGlobalMailBox_H__

#include "FvGlobalDefines.h"
#include <FvDataObj.h>
#include <FvMemoryStream.h>

class FvEntity;
class FvNetChannel;


class FV_GLOBAL_API FvMailBox : public FvObjMailBox
{
	template<class> friend class FvRPCCallback;
public:
	FvMailBox():FvObjMailBox() {}
	FvMailBox(const FvMailBox& kSrc):FvObjMailBox((const FvObjMailBox&)kSrc) {}

	enum Destination
	{
		ToCLIENT = 0,
		ToCELL = 1,
		ToBASE = 2,
		ToGLOBAL = 3,
	};

	FvMailBox& operator=(const FvMailBox&);

	void	Set(FvEntityID iEntityID, FvEntityTypeID uiEntityType, const FvNetAddress& kAddr, FvEntityMailBoxRef::Component eComponent);
	void	Set(const FvEntityMailBoxRef& kMBRef);
	void	SetComponent(FvEntityMailBoxRef::Component eComponent);
	void	SetType(FvEntityTypeID uiEntityType);
	void	SetID(FvEntityID iEntityID);
	void	SetAddr(const FvNetAddress& kAddr);

protected:
	FvBinaryOStream*	GetStream(Destination eDest, FvInt32 iMethodIdx, FvUInt8 uiCBCnt);
	void				SendStream();
	FvBinaryOStream*	RPCCallbackInvokeGetStream(FvUInt32 uiID);
	void				RPCCallbackInvokeSendStream();
	void				RPCCallbackException(FvUInt32 uiID);

protected:
	static FvUInt8			ms_uiState;
	static FvNetChannel*	ms_pkChannel;
	static FvNetChannel*	ms_pkChannelForCB;

	template<class T> friend FvBinaryIStream& operator>>(FvBinaryIStream&, T&);
	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
};

template<>
FV_GLOBAL_API FvBinaryIStream& operator>>(FvBinaryIStream&, FvMailBox&);
template<>
FV_GLOBAL_API FvBinaryOStream& operator<<(FvBinaryOStream&, const FvMailBox&);


#define FVMAILBOX_DECLARE(_PARENT, _MAILBOX)	\
	_MAILBOX():_PARENT() {}						\
	_MAILBOX(const _MAILBOX& kSrc)				\
	:_PARENT((const _PARENT&)kSrc) {}			\
	_MAILBOX& operator=(const _MAILBOX& val)	\
	{											\
		if(this==&val || m_kMBRef==val.m_kMBRef)\
			return *this;						\
		m_kMBRef = val.m_kMBRef;				\
		MailBoxChangNotify();					\
		return *this;							\
	}



#endif//__FvGlobalMailBox_H__