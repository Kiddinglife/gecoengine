////#define FV_ENABLE_WATCHERS 1
#if FV_ENABLE_WATCHERS

#include "FvNetWatcherPacketHandler.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 );

const FvString FvNetWatcherPacketHandler::v1ErrorIdentifier = "<Err>";
const FvString FvNetWatcherPacketHandler::v1ErrorPacketLimit =
	"Exceeded maximum packet size";

FvNetWatcherPacketHandler::FvNetWatcherPacketHandler( FvNetEndpoint & socket,
	const sockaddr_in & addr, FvInt32 numPaths,
	WatcherProtocolVersion version, bool isSet ) :
		m_bCanDelete ( false ),
		m_kEndpoint( socket ),
		m_kDestAddr( addr ),
		m_kVersion( version ),
		m_bIsSet( isSet ),
		m_iOutgoingRequests( numPaths ),
		m_iAnsweredRequests( 0 ),
		m_bIsExecuting( false ),
		m_iMaxPacketSize( 0 ),
		m_bReachedPacketLimit( false )
{
	switch (m_kVersion)
	{
	case WP_VERSION_1:
		m_kPacket << (FvInt32)FV_NET_WATCHER_MSG_TELL;
		m_iMaxPacketSize = FV_NET_WN_PACKET_SIZE -
						(v1ErrorIdentifier.size()  + 1) -
						(v1ErrorPacketLimit.size() + 1);
		break;

	case WP_VERSION_2:
		m_kPacket << (FvInt32)(m_bIsSet ? FV_NET_WATCHER_MSG_SET2_TELL2 : FV_NET_WATCHER_MSG_TELL2);
		m_iMaxPacketSize = FV_NET_WN_PACKET_SIZE;
		break;

	default:
		FV_ERROR_MSG( "FvWatcherPacketHandler: Unknown version %d\n", m_kVersion );
		m_kVersion = WP_VERSION_UNKNOWN;
		break;
	}

	m_kPacket << (FvInt32)0;
}

FvNetWatcherPacketHandler::~FvNetWatcherPacketHandler()
{
	FvPathRequestList::iterator iter = m_kPathRequestList.begin();

	while (iter != m_kPathRequestList.end())
	{
		delete *iter;
		iter++;
	}

	m_kPathRequestList.clear();
}

FvWatcherPathRequest * FvNetWatcherPacketHandler::NewRequest( FvString & path )
{
	FvWatcherPathRequest *pathRequest = NULL;

	if (m_bIsExecuting)
	{
		FV_ERROR_MSG( "FvWatcherPacketHandler::NewRequest: Attempt to create a new "
					"path request after packet handler told disallowed new "
					"requests." );
		return NULL;
	}

	switch (m_kVersion)
	{
	case WP_VERSION_1:
		pathRequest = new FvWatcherPathRequestV1( path );
		break;
	case WP_VERSION_2:
		pathRequest = new FvWatcherPathRequestV2( path );
		break;
	default:
		FV_ERROR_MSG( "FvWatcherPacketHandler::NewRequest: "
				"Invalid watcher protocol version %d\n", m_kVersion );
	}

	if (pathRequest)
	{
		pathRequest->SetParent( this );
		m_kPathRequestList.push_back( pathRequest );
	}

	return pathRequest;
}

void FvNetWatcherPacketHandler::Run()
{
	m_bIsExecuting = true;

	FvPathRequestList::iterator iter = m_kPathRequestList.begin();

	this->DoNotDelete( true );

	while (iter != m_kPathRequestList.end())
	{
		if (m_bIsSet)
		{
			(*iter)->SetWatcherValue();
		}
		else
		{
			(*iter)->FetchWatcherValue();
		}

		iter++;
	}

	this->DoNotDelete( false );
}

void FvNetWatcherPacketHandler::NotifyComplete( FvWatcherPathRequest & pathRequest,
	FvInt32 count )
{
	if (!m_bReachedPacketLimit)
	{
		if ((m_kPacket.Size() + pathRequest.GetDataSize()) > m_iMaxPacketSize)
		{
			FV_ERROR_MSG( "FvNetWatcherPacketHandler::NotifyComplete: Can't add reply "
						"from WatcherPathRequest( '%s' ) due to packet size "
						"limit.\n", pathRequest.GetPath().c_str() );

			m_bReachedPacketLimit = true;

			if (m_kVersion == WP_VERSION_1)
			{
				m_kPacket.AddBlob( v1ErrorIdentifier.c_str(),
								v1ErrorIdentifier.size() + 1 );
				m_kPacket.AddBlob( v1ErrorPacketLimit.c_str(),
								v1ErrorPacketLimit.size() + 1 );
			}

		}
		else
		{
			m_kPacket.AddBlob( pathRequest.GetData(), pathRequest.GetDataSize() );

			FvInt32 *pReplyCount = (FvInt32 *)(m_kPacket.Data());
			pReplyCount[1] += count;
		}
	}

	m_iAnsweredRequests++;
	this->CheckSatisfied();
}

void FvNetWatcherPacketHandler::SendReply()
{
	m_kEndpoint.SendTo( m_kPacket.Data(), m_kPacket.Size(), m_kDestAddr );
	return;
}

void FvNetWatcherPacketHandler::CheckSatisfied()
{
	if (m_iOutgoingRequests == m_iAnsweredRequests)
	{
		if (m_bIsExecuting)
		{
			this->SendReply();
			m_bIsExecuting = false;
		}

		if (m_bCanDelete)
		{
			delete this;
		}
	}

	return;
}

void FvNetWatcherPacketHandler::DoNotDelete( bool shouldNotDelete )
{
	m_bCanDelete = !shouldNotDelete;

	if (m_bCanDelete)
	{
		this->CheckSatisfied();
	}
}

#endif // FV_ENABLE_WATCHERS
