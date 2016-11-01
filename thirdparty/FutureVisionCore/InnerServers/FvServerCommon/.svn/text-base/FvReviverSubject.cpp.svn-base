#include "FvReviverSubject.h"
#include "FvReviverCommon.h"
#include "FvServerConfig.h"

#include <FvTimestamp.h>
#include <FvNetBundle.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvServer", 0 );

FvReviverSubject FvReviverSubject::ms_kInstance;

FvReviverSubject::FvReviverSubject() :
	m_pkNub( NULL ),
	m_kReviverAddr( 0, 0 ),
	m_uiLastPingTime( 0 ),
	m_uiPriority( 0xff ),
	m_iMSTimeout( 0 )
{
}

void FvReviverSubject::Init( FvNetNub *pkNub, const char *pcComponentName )
{
	m_pkNub = pkNub;
	char buf[128];
#ifdef _WIN32 
	_snprintf( buf, sizeof(buf), "reviver/%s/subjectTimeout", pcComponentName );
#else  // WIN32 (this is linux)
	snprintf( buf, sizeof(buf), "reviver/%s/subjectTimeout", pcComponentName );
#endif // WIN32
	m_iMSTimeout = int(FvServerConfig::Get( buf,
				FvServerConfig::Get( "reviver/subjectTimeout", 0.2f ) ) * 1000);
	FV_INFO_MSG( "FvReviverSubject::Init: m_iMSTimeout = %d\n", m_iMSTimeout );
}

void FvReviverSubject::Fini()
{
	m_pkNub = NULL;
}

void FvReviverSubject::HandleMessage( const FvNetAddress &kSrcAddr,
		FvNetUnpackedMessageHeader &kHeader,
		FvBinaryIStream &kData )
{
	if (m_pkNub == NULL)
	{
		FV_ERROR_MSG( "FvReviverSubject::HandleMessage: "
						"FvReviverSubject not initialised\n" );
		return;
	}

	FvUInt64 currentPingTime = Timestamp();

	FvReviverPriority priority;
	kData >> priority;

	bool accept = (m_kReviverAddr == kSrcAddr);

	if (!accept)
	{
		if (priority < m_uiPriority)
		{
			if (m_uiPriority == 0xff)
			{
				FV_INFO_MSG( "FvReviverSubject::HandleMessage: "
							"Reviver is %s (Priority %d)\n",
						(char *)kSrcAddr, priority );
			}
			else
			{
				FV_INFO_MSG( "FvReviverSubject::HandleMessage: "
							"%s has a better priority (%d)\n",
						(char *)kSrcAddr, priority );
			}
			accept = true;
		}
		else
		{
			FvUInt64 delta = (currentPingTime - m_uiLastPingTime) * FvUInt64(1000);
			delta /= StampsPerSecond();
			int msBetweenPings = int(delta);

			if (msBetweenPings > m_iMSTimeout)
			{
				FvString oldAddr = (char *)m_kReviverAddr;
				FV_INFO_MSG( "FvReviverSubject::HandleMessage: "
								"%s timed out (%d ms). Now using %s\n",
							oldAddr.c_str(), msBetweenPings, (char *)kSrcAddr );
				accept = true;
			}
		}
	}


	FvNetBundle bundle;
	bundle.StartReply( kHeader.m_iReplyID );
	if (accept)
	{
		m_kReviverAddr = kSrcAddr;
		m_uiLastPingTime = currentPingTime;
		m_uiPriority = priority;
		bundle << REVIVER_PING_YES;
	}
	else
	{
		bundle << REVIVER_PING_NO;
	}
	m_pkNub->Send( kSrcAddr, bundle );
}