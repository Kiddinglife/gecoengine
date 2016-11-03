////#define FV_ENABLE_WATCHERS 1
#if FV_ENABLE_WATCHERS

#include "FvNetPortMap.h"
#include "FvNetBSDSNPrintf.h"
#include "FvNetWatcherGlue.h"
#include "FvNetLoggerMessageForwarder.h"

#include <FvProcess.h>

#include <cstring>
#include <time.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 );

FV_SINGLETON_STORAGE( FvNetLoggerMessageForwarder )

void FvNetLoggerComponentMessage::Write( FvBinaryOStream &os ) const
{
	os << m_uiVersion << m_uiLoggerID << m_uiUID << m_uiPID << m_kComponentName;
}

void FvNetLoggerComponentMessage::Read( FvBinaryIStream &is )
{
	is >> m_uiVersion >> m_uiLoggerID >> m_uiUID >> m_uiPID >> m_kComponentName;
}

FvNetSimpleLoggerMessageForwarder::FvNetSimpleLoggerMessageForwarder(
		FvString appName,
		FvNetEndpoint & endpoint,
		FvUInt16 uiMachinePort,
		FvUInt8 loggerID,
		bool enabled) :
	m_kAppName( appName ),
	m_uiLoggerID( loggerID ),
	m_iAppID( 0 ),
	m_bEnabled( enabled ),
	m_kEndpoint( endpoint )
{
	FV_TRACE_MSG( "Finding loggers ...\n" );
	this->FindLoggerInterfaces(uiMachinePort);
	FV_TRACE_MSG( "Total # loggers on network: %u\n", m_kLoggers.size() );

	FvDebugFilter::Instance().AddMessageCallback( this );
}

FvNetSimpleLoggerMessageForwarder::~FvNetSimpleLoggerMessageForwarder()
{
	FvDebugFilter::Instance().DeleteMessageCallback( this );

	HandlerCache::iterator cacheIter = m_kHandlerCache.begin();
	while (cacheIter != m_kHandlerCache.end())
	{
		delete cacheIter->second;
		cacheIter++;
	}
}

bool FvNetSimpleLoggerMessageForwarder::HandleMessage( int componentPriority,
	int messagePriority, const char * format, va_list argPtr )
{
	if (m_kLoggers.empty() || !m_bEnabled)
		return false;

	this->ParseAndSend(this->FindForwardingStringHandler( format ),
					   componentPriority, messagePriority, argPtr );

	return false;
}


bool FvNetSimpleLoggerMessageForwarder::IsSuppressible( const FvString & format ) const
{
	return false;
}



FvNetLoggerMessageForwarder::FvNetLoggerMessageForwarder(
		FvString appName,
		FvNetEndpoint & endpoint,
		FvNetNub & nub,
		FvUInt8 loggerID,
		bool enabled,
		unsigned spamFilterThreshold ) :
	FvNetSimpleLoggerMessageForwarder(appName, endpoint, loggerID, enabled ),
	m_kNub( nub ),
	m_kSpamTimerID( FV_NET_TIMER_ID_NONE ),
	m_uiSpamFilterThreshold( spamFilterThreshold ),
	m_kSpamHandler( "* Suppressed %d in last 1s: %s" )
{
	this->Init();
}

FvNetLoggerMessageForwarder::~FvNetLoggerMessageForwarder()
{
	if (m_kSpamTimerID != FV_NET_TIMER_ID_NONE)
	{
		m_kNub.CancelTimer( m_kSpamTimerID );
	}
}

void FvNetSimpleLoggerMessageForwarder::RegisterAppID( int id )
{
	m_iAppID = id;

	for (Loggers::iterator it = m_kLoggers.begin(); it != m_kLoggers.end(); ++it)
	{
		this->SendAppID( *it );
	}
}

void FvNetLoggerMessageForwarder::AddSuppressionPattern( FvString prefix )
{
	SuppressionPatterns::iterator iter = std::find(
		m_kSuppressionPatterns.begin(), m_kSuppressionPatterns.end(), prefix );

	if (iter == m_kSuppressionPatterns.end())
	{
		m_kSuppressionPatterns.push_back( prefix );
		this->UpdateSuppressionPatterns();
	}
	else
	{
		FV_WARNING_MSG( "FvNetLoggerMessageForwarder::AddSuppressionPattern: "
			"Not re-adding pattern '%s'\n",
			prefix.c_str() );
	}
}

void FvNetLoggerMessageForwarder::DelSuppressionPattern( FvString prefix )
{
	SuppressionPatterns::iterator iter = std::find(
		m_kSuppressionPatterns.begin(), m_kSuppressionPatterns.end(), prefix );

	if (iter != m_kSuppressionPatterns.end())
	{
		m_kSuppressionPatterns.erase( iter );
		this->UpdateSuppressionPatterns();
	}
	else
	{
		FV_ERROR_MSG( "FvNetLoggerMessageForwarder::DelSuppressionPattern: "
			"Tried to erase unknown suppression pattern '%s'\n",
			prefix.c_str() );
	}
}

void FvNetLoggerMessageForwarder::UpdateSuppressionPatterns()
{
	for (HandlerCache::iterator iter = m_kHandlerCache.begin();
		 iter != m_kHandlerCache.end(); ++iter)
	{
		FvNetForwardingStringHandler & handler = *iter->second;
		handler.IsSuppressible( this->IsSuppressible( handler.fmt() ) );
	}
}


void FvNetLoggerMessageForwarder::Init()
{
	FV_WATCH( "logger/add", *this,
			&FvNetLoggerMessageForwarder::WatcherHack,
			&FvNetLoggerMessageForwarder::WatcherAddLogger,
			"Used by logger to add itself as a logging destination" );
	FV_WATCH( "logger/del", *this,
			&FvNetLoggerMessageForwarder::WatcherHack,
			&FvNetLoggerMessageForwarder::WatcherDelLogger,
			"Used by logger to remove itself as a logging destination" );
	FV_WATCH( "logger/size", *this, &FvNetLoggerMessageForwarder::size,
		   "The number of loggers this process is sending to" );
	FV_WATCH( "logger/enabled", m_bEnabled, FvWatcher::WT_READ_WRITE,
		   "Whether or not to forward messages to attached logs" );

	FV_WATCH( "logger/filterThreshold", FvDebugFilter::Instance(),
			FV_ACCESSORS( int, FvDebugFilter, FilterThreshold ),
	   "Controls the level at which messages are sent to connected loggers.\n"
	   "A higher value reduces the volume of messages sent." );

	FV_WATCH( "logger/spamThreshold", m_uiSpamFilterThreshold,
		FvWatcher::WT_READ_WRITE,
		"The maximum number of a particular message that will be sent to the "
		"logs each second." );

	FV_WATCH( "logger/addSuppressionPattern", *this,
		&FvNetLoggerMessageForwarder::SuppressionWatcherHack,
		&FvNetLoggerMessageForwarder::AddSuppressionPattern,
		"Adds a new spam suppression pattern to this logger" );

	FV_WATCH( "logger/delSuppressionPattern", *this,
		&FvNetLoggerMessageForwarder::SuppressionWatcherHack,
		&FvNetLoggerMessageForwarder::DelSuppressionPattern,
		"Removes a spam suppression pattern from this logger" );

	FV_WATCH( "debug/hasDevelopmentAssertions", FvDebugFilter::Instance(),
			FV_ACCESSORS( bool, FvDebugFilter, HasDevelopmentAssertions ),
	   "If true, the process will be stopped when a development-time "
		   "assertion fails" );

	m_kSpamTimerID = m_kNub.RegisterTimer( 1000000 /* 1s */, this );
}

void FvNetSimpleLoggerMessageForwarder::AddLogger( const FvNetAddress & addr )
{
	Loggers::iterator iter =
		std::find( m_kLoggers.begin(), m_kLoggers.end(), addr );

	if (iter != m_kLoggers.end())
	{
		FV_WARNING_MSG( "LoggerMessageForwarder::addLogger: Re-adding %s\n",
				(char*)addr );
	}
	else
	{
		m_kLoggers.push_back( addr );
	}

	FvMemoryOStream os;
	os << (int)FV_NET_MESSAGE_LOGGER_REGISTER;

	FvNetLoggerComponentMessage lrm;
	lrm.m_uiVersion = FV_NET_MESSAGE_LOGGER_VERSION;
	lrm.m_uiLoggerID = m_uiLoggerID;
	lrm.m_uiPID = FvGetPID();
	lrm.m_uiUID = //FvGetUserID();
	lrm.m_kComponentName = m_kAppName;
	lrm.Write( os );

	m_kEndpoint.SendTo( os.Data(), os.Size(), addr.m_uiPort, addr.m_uiIP );

	FV_INFO_MSG( "FvNetLoggerMessageForwarder::AddLogger: Added %s. # loggers = %u\n",
			(char *)addr, m_kLoggers.size() );

	if (m_iAppID > 0)
		this->SendAppID( addr );
}

void FvNetSimpleLoggerMessageForwarder::DelLogger( const FvNetAddress & addr )
{
	Loggers::iterator iter =
		std::find( m_kLoggers.begin(), m_kLoggers.end(), addr );

	if (iter != m_kLoggers.end())
	{
		m_kLoggers.erase( iter );
		FV_INFO_MSG( "FvNetLoggerMessageForwarder::DelLogger: "
				"Removed %s. # loggers = %u\n",
			(char *)addr, m_kLoggers.size() );
	}
}

void FvNetSimpleLoggerMessageForwarder::SendAppID( const FvNetAddress &addr )
{
	FV_ASSERT( m_iAppID != 0 );

	FvMemoryOStream os;
	os << (int)FV_NET_MESSAGE_LOGGER_APP_ID << m_iAppID;

	if (m_kEndpoint.SendTo( os.Data(), os.Size(), addr.m_uiPort, addr.m_uiIP ) !=
		os.Size())
	{
		FV_ERROR_MSG( "FvNetLoggerMessageForwarder::RegisterAppID: "
			"Failed to send app ID to %s\n", (char*)addr );
	}
}

FvNetAddress FvNetLoggerMessageForwarder::WatcherHack() const
{
	return FvNetAddress( 0, 0 );
}

bool FvNetLoggerMessageForwarder::FindLoggerHandler::OnProcessStatsMessage(
	FvNetProcessStatsMessage &psm, FvUInt32 addr )
{
	if (psm.m_uiPID != 0)
	{
		FvNetAddress address( addr, psm.m_uiPort );
		m_kLoggerMessageForwarder.AddLogger( address );
	}
	return true;
}

void FvNetSimpleLoggerMessageForwarder::FindLoggerInterfaces(FvUInt16 uiMachinePort)
{
	FvNetProcessStatsMessage psm;
	psm.m_uiParam = psm.PARAM_USE_CATEGORY | psm.PARAM_USE_NAME;
	psm.m_uiCategory = psm.WATCHER_NUB;
	psm.m_kName = FV_NET_MESSAGE_LOGGER_NAME;

	FindLoggerHandler handler( *this );
	int reason;
	if ((reason = psm.SendAndRecv( m_kEndpoint, FV_NET_BROADCAST, uiMachinePort, &handler )) !=
		FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG( "FvNetLoggerMessageForwarder::FindLoggerInterfaces: "
			"MGM::SendAndRecv() failed (%s)\n",
			NetReasonToString( (FvNetReason&)reason ) );
	}
}

FvNetForwardingStringHandler *
FvNetSimpleLoggerMessageForwarder::FindForwardingStringHandler(
	const char * format )
{
	HandlerCache::iterator it = m_kHandlerCache.find( format );
	if (it != m_kHandlerCache.end())
	{
		return it->second;
	}
	FvNetForwardingStringHandler * pHandler =
		new FvNetForwardingStringHandler( format, this->IsSuppressible( format ) );
	m_kHandlerCache[ format ] = pHandler;
	return pHandler;
}

bool FvNetLoggerMessageForwarder::HandleMessage( int componentPriority,
	int messagePriority, const char * format, va_list argPtr )
{
	if (m_kLoggers.empty() || !m_bEnabled)
		return false;

	FvNetForwardingStringHandler * pHandler =
		this->FindForwardingStringHandler( format );

	pHandler->AddRecentCall();

	if (!this->IsSpamming( pHandler ))
	{
		this->ParseAndSend(
			pHandler, componentPriority, messagePriority, argPtr );

		if (pHandler->NumRecentCalls() == 1)
		{
			m_kRecentlyUsedHandlers.push_back( pHandler );
		}
	}

	return false;
}

int FvNetLoggerMessageForwarder::HandleTimeout( FvNetTimerID id, void * arg )
{
	for (RecentlyUsedHandlers::iterator iter = m_kRecentlyUsedHandlers.begin();
		 iter != m_kRecentlyUsedHandlers.end(); ++iter)
	{
		FvNetForwardingStringHandler * pHandler = *iter;

		if (this->IsSpamming( pHandler ))
		{
			this->ParseAndSend( &m_kSpamHandler, 0, FV_DEBUG_MESSAGE_PRIORITY_DEBUG,
				pHandler->NumRecentCalls() - m_uiSpamFilterThreshold,
				pHandler->fmt().c_str() );
		}

		pHandler->ClearRecentCalls();
	}

	m_kRecentlyUsedHandlers.clear();

	return 0;
}

bool FvNetLoggerMessageForwarder::IsSuppressible( const FvString & format ) const
{
	for (SuppressionPatterns::const_iterator iter = m_kSuppressionPatterns.begin();
		 iter != m_kSuppressionPatterns.end(); ++iter)
	{
		const FvString & patt = *iter;

		if (patt.empty())
		{
			return true;
		}

		if (format.size() >= patt.size() &&
			std::mismatch( patt.begin(), patt.end(), format.begin() ).first ==
				patt.end())
		{
			return true;
		}
	}

	return false;
}

void FvNetSimpleLoggerMessageForwarder::ParseAndSend( FvNetForwardingStringHandler * pHandler,
	int componentPriority, int messagePriority, va_list argPtr )
{
	FvMemoryOStream os;
	FvNetLoggerMessageHeader hdr;

	hdr.m_uiComponentPriority = componentPriority;
	hdr.m_uiMessagePriority = messagePriority;

	os << (int)FV_NET_MESSAGE_LOGGER_MSG <<
		hdr.m_uiComponentPriority << hdr.m_uiMessagePriority << pHandler->fmt();

	pHandler->ParseArgs( argPtr, os );

	for (Loggers::const_iterator iter = m_kLoggers.begin();
		 iter != m_kLoggers.end(); ++iter)
	{
		m_kEndpoint.SendTo( os.Data(), os.Size(), iter->m_uiPort, iter->m_uiIP );
	}
}

void FvNetSimpleLoggerMessageForwarder::ParseAndSend( FvNetForwardingStringHandler * pHandler,
	int componentPriority, int messagePriority, ... )
{
	va_list argPtr;
	va_start( argPtr, messagePriority );

	this->ParseAndSend( pHandler, componentPriority, messagePriority, argPtr );

	va_end( argPtr );
}


#endif // FV_ENABLE_WATCHERS