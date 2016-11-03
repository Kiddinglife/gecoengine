//{future header message}
#ifndef __FvReviverSubject_H__
#define __FvReviverSubject_H__

#include "FvServerCommon.h"
#include "FvReviverCommon.h"

#include <FvNetNub.h>

class FV_SERVERCOMMON_API FvReviverSubject : public FvNetInputMessageHandler
{
public:
	FvReviverSubject();
	void Init( FvNetNub *pkNub, const char *pcComponentName );
	void Fini();

	static FvReviverSubject &Instance() { return ms_kInstance; }

private:
	virtual void HandleMessage( const FvNetAddress &kSrcAddr,
			FvNetUnpackedMessageHeader &kHeader,
			FvBinaryIStream &kData );

	FvNetNub *m_pkNub;
	FvNetAddress m_kReviverAddr;
	FvUInt64 m_uiLastPingTime;
	FvReviverPriority m_uiPriority;

	int m_iMSTimeout;

	static FvReviverSubject ms_kInstance;
};

#define FV_REVIVER_PING_MSG()	\
		MERCURY_VARIABLE_MESSAGE( ReviverPing, 2, &FvReviverSubject::Instance() )

#endif // REVIVER_SUBJECT_HPP
