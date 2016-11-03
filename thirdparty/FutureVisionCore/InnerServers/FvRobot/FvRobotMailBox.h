//{future header message}
#ifndef __FvRobotMailBox_H__
#define __FvRobotMailBox_H__

#include "FvRobotDefines.h"
#include <FvMemoryStream.h>
#include <FvBaseTypes.h>
#include <FvNetTypes.h>


class FvEntity;
class FvServerConnection;


class FV_ROBOT_API FvMailBox
{
public:
	enum Destination
	{
		ToCLIENT = 0,
		ToCELL = 1,
		ToBASE = 2,
	};

	FvMailBox():m_pkEntity(NULL),m_pkServer(NULL) {}
	void				Init(FvEntity* pkEntity, FvServerConnection* pkServer);
	bool				IsValid() const;
	FvEntityID			GetEntityID() const;

protected:
	FvMailBox& operator=(const FvMailBox&);
	template<class T> friend FvBinaryIStream& operator>>(FvBinaryIStream&, T&);
	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);

	FvBinaryOStream*	GetStream(Destination eDest, FvInt32 iMethodIdx, FvUInt8 uiCBCnt);
	void				SendStream();

protected:
	FvEntity*				m_pkEntity;
	FvServerConnection*		m_pkServer;
	static FvMemoryOStream	ms_kStream;
	static FvUInt8			ms_uiState;
	static FvUInt16			ms_uiMethodID;
};

template<>
FV_ROBOT_API FvBinaryIStream& operator>>(FvBinaryIStream&, FvMailBox&);
template<>
FV_ROBOT_API FvBinaryOStream& operator<<(FvBinaryOStream&, const FvMailBox&);


#define FVMAILBOX_DECLARE(_PARENT, _MAILBOX)\
	_MAILBOX():_PARENT() {}					\
	_MAILBOX(const _MAILBOX& kSrc)			\
	:_PARENT() {}							\
	_MAILBOX& operator=(const _MAILBOX& val)\
	{										\
		return *this;						\
	}



#endif//__FvRobotMailBox_H__