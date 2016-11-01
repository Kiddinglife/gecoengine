#include "FvIdClient.h"
#include <FvDebug.h>
#include <FvTimestamp.h>

#include <FvNetBlockingReplyHandler.h>
#include <FvNetBundle.h>
#include <FvNetChannel.h>


FV_DECLARE_DEBUG_COMPONENT( 0 );

namespace
{
	const size_t FV_MAX_AUTO_INCREMENT_DESIRED_SIZE = 65535;
}


FvIDClient::FvIDClient()
:m_pkChannel( NULL )
,m_uiHighSize( 0 )
,m_uiDesiredSize( 0 )
,m_uiLowSize( 0 )
,m_uiCriticallyLowSize( 0 )
,m_bPendingRequest( false )
,m_bInEmergency( false )
,m_bInited(false)
,m_pkGetMoreMethod( NULL )
,m_pkPutBackMethod( NULL )
{
}


bool FvIDClient::Init(FvNetChannel * pChannel,
		const FvNetInterfaceElement& getMoreMethod,
		const FvNetInterfaceElement& putBackMethod,
		size_t criticallyLowSize,
		size_t lowSize,
		size_t desiredSize,
		size_t highSize,
		bool bBlockAtInit)
{
	m_bInited = true;
	m_pkChannel = pChannel;

	m_pkGetMoreMethod = &getMoreMethod;
	m_pkPutBackMethod = &putBackMethod;
	m_uiCriticallyLowSize = criticallyLowSize;
	m_uiLowSize = lowSize;
	m_uiDesiredSize = desiredSize;
	m_uiHighSize = highSize;
	m_bInEmergency = true;
	m_bPendingRequest = false;

	bool isSorted =
		(m_uiCriticallyLowSize < m_uiLowSize) &&
		(m_uiLowSize < m_uiDesiredSize) &&
		(m_uiDesiredSize < m_uiHighSize);

	if (!isSorted)
	{
		FV_ERROR_MSG( "IDClient::init: \n"
					"Configuration options must be in increasing order!\n"
					"\tcriticallyLowSize %lu\n"
					"\tlowSize %lu\n"
					"\tdesiredSize %lu\n"
					"\thighSize %lu\n",
				criticallyLowSize,
				lowSize,
				desiredSize,
				highSize );
	}

	if(bBlockAtInit)
	{
		return this->GetMoreIDsBlocking() && isSorted;
	}
	else
	{
		GetMoreIDs();
		return isSorted;
	}
}


FvIDClient::LockedID::LockedID( ID id )
{
	m_iId = id;
	m_uiUnlockTime = Timestamp() + FvUInt64(10) * StampsPerSecond();
}

void FvIDClient::PutUsedID( ID id )
{
#ifdef FV_ID_RECYCLING
	m_kLockedIDs.push( id );
#endif
	this->PerformUpdates( false );
}

FvIDClient::ID FvIDClient::GetID()
{
	if (m_kReadyIDs.empty())
	{
		this->PerformUpdates( true );
		if (m_kReadyIDs.empty())
		{
			FV_ERROR_MSG("IDClient::getID: no id's left (really bad)\n");
			return 0;
		}
	}
	ID id = m_kReadyIDs.front();
	m_kReadyIDs.pop();
	this->PerformUpdates( false );
	return id;
}


void FvIDClient::ReturnIDs()
{
	while (m_kLockedIDs.size())
	{
		m_kReadyIDs.push( m_kLockedIDs.front().m_iId );
		m_kLockedIDs.pop();
	}

	if (m_pkChannel != NULL)
	{
		FvNetBundle & bundle = m_pkChannel->Bundle();
		bundle.StartMessage( *m_pkPutBackMethod );
		this->PlaceIDsOntoStream( m_kReadyIDs.size(), m_kReadyIDs, bundle );
		m_pkChannel->Send();
	}
}

void FvIDClient::PerformUpdates( bool isEmergency )
{
	isEmergency = isEmergency || m_kReadyIDs.size() < m_uiCriticallyLowSize;

	if (isEmergency && !m_bInEmergency)
	{
		if (m_uiDesiredSize < FV_MAX_AUTO_INCREMENT_DESIRED_SIZE/2)
		{
			m_uiHighSize *= 2;
			m_uiDesiredSize *= 2;
			m_uiLowSize *= 2;
			FV_TRACE_MSG( "IDClient::performUpdates: "
						"increased sizes to (%lu, %lu, %lu)\n",
					m_uiLowSize, m_uiDesiredSize, m_uiHighSize );
		}
		else
		{
			// Adjust to maximum size
			float growRatio =
					float(FV_MAX_AUTO_INCREMENT_DESIRED_SIZE)/m_uiDesiredSize;
			m_uiHighSize = int( growRatio * m_uiHighSize );
			m_uiDesiredSize = FV_MAX_AUTO_INCREMENT_DESIRED_SIZE;
			m_uiLowSize = int( growRatio * m_uiLowSize );
			FV_WARNING_MSG( "IDClient::performUpdates: Sizes are at maximum "
					"(%lu, %lu, %lu). Please slow down entity creation.\n",
					m_uiLowSize, m_uiDesiredSize, m_uiHighSize );
		}
		m_bInEmergency = true;
	}

#if FV_ID_RECYCLING
	FvUInt64 now = Timestamp();

	while (m_kLockedIDs.size() &&
			(m_kLockedIDs.front().m_uiUnlockTime < now || isEmergency))
	{
		m_kReadyIDs.push( m_kLockedIDs.front().m_iId );
		m_kLockedIDs.pop();
		isEmergency = false;
	}

	if (m_kReadyIDs.size() > m_uiHighSize && !m_bInEmergency)
		this->PutBackIDs();
	else
#endif
	if (((m_kReadyIDs.size() < m_uiLowSize) || m_bInEmergency) &&
			!m_bPendingRequest)
	{
		this->GetMoreIDs();
	}
}

void FvIDClient::PutBackIDs()
{
#ifdef FV_ID_RECYCLING
	if ((m_pkChannel != NULL) && (m_kReadyIDs.size() > m_uiDesiredSize))
	{
		FvNetBundle & bundle = m_pkChannel->Bundle();
		bundle.StartMessage( *m_pkPutBackMethod );
		int numIDs = m_kReadyIDs.size() - m_uiDesiredSize;
		this->PlaceIDsOntoStream( numIDs, m_kReadyIDs, bundle );
		m_pkChannel->Send();
	}
#else
	FV_ERROR_MSG( "IDClient::putBackIDs: Recycling has been disabled\n" );
#endif
}


void FvIDClient::GetMoreIDs()
{
	this->GetMoreIDs( this );
}


bool FvIDClient::GetMoreIDsBlocking()
{
	FvNetBlockingReplyHandler handler( m_pkChannel->nub(), this );
	this->GetMoreIDs( &handler );
	return handler.WaitForReply( m_pkChannel ) == FV_NET_REASON_SUCCESS;
}


void FvIDClient::GetMoreIDs( FvNetReplyMessageHandler * pHandler )
{
	if (!m_pkChannel)
	{
		FV_INFO_MSG( "IDClient::getMoreIDs: No server yet.\n" );
		return;
	}

	FV_ASSERT( !m_bPendingRequest );
	if ((m_pkChannel != NULL) && (m_kReadyIDs.size() < m_uiDesiredSize))
	{
		FvNetBundle & bundle = m_pkChannel->Bundle();

		bundle.StartRequest( *m_pkGetMoreMethod, pHandler );

		int numIDs = m_uiDesiredSize - m_kReadyIDs.size();
		bundle << numIDs;
		m_pkChannel->Send();
		m_bPendingRequest = true;
	}
}


void FvIDClient::HandleMessage( const FvNetAddress& source,
		FvNetUnpackedMessageHeader& header, FvBinaryIStream& data,
		void * arg )
{
	FV_ASSERT( m_bPendingRequest );
	size_t oldSize = m_kReadyIDs.size();
	this->RetrieveIDsFromStream( m_kReadyIDs, data );
	FV_INFO_MSG( "IDClient::handleMessage: "
				"Number of ids increased from %lu to %lu\n",
			oldSize, m_kReadyIDs.size() );

	m_bPendingRequest = false;
	m_bInEmergency = false;
	this->PerformUpdates( false );
}

void FvIDClient::HandleException( const FvNetNubException& exception,
		void * arg )
{
	FV_ASSERT( m_bPendingRequest );
	FV_ERROR_MSG( "IDClient::handleException: failed to fetch more ID's\n" );
	m_bPendingRequest = false;
	this->PerformUpdates( false );
}

void FvIDClient::PlaceIDsOntoStream( size_t n, IDQueue& stk,
		FvBinaryOStream& stream )
{
	FV_ASSERT( n <= stk.size() );
	while (n--)
	{
		stream << stk.front();
		stk.pop();
	}
}


void FvIDClient::RetrieveIDsFromStream( IDQueue& stk,
		FvBinaryIStream& stream )
{
	while (stream.RemainingLength())
	{
		ID id;
		stream >> id;
		stk.push( id );
	}
}





