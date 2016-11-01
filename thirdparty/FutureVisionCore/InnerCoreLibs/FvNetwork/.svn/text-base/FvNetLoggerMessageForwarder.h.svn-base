//{future header message}
#ifndef __FvNetLoggerMessageForwarder_H__
#define __FvNetLoggerMessageForwarder_H__

////#define FV_ENABLE_WATCHERS 1
#if FV_ENABLE_WATCHERS

#include "FvNetwork.h"
#include "FvNetTypes.h"
#include "FvNetChannel.h"
#include "FvNetEndpoint.h"
#include "FvNetInterfaces.h"
#include "FvNetWatcherNub.h"
#include "FvNetWatcherGlue.h"
#include "FvNetForwardingStringHandler.h"

#include <FvDebugFilter.h>

#define FV_NET_MESSAGE_LOGGER_VERSION 	6
#define FV_NET_MESSAGE_LOGGER_NAME 	"message_logger"

enum
{
	FV_NET_MESSAGE_LOGGER_MSG = FV_NET_WATCHER_MSG_EXTENSION_START,
	FV_NET_MESSAGE_LOGGER_REGISTER,
	FV_NET_MESSAGE_LOGGER_PROCESS_BIRTH,
	FV_NET_MESSAGE_LOGGER_PROCESS_DEATH,
	FV_NET_MESSAGE_LOGGER_APP_ID
};

const int FV_NET_LOGGER_MSG_SIZE = 2048;

#pragma pack( push, 1 )

struct FvNetLoggerMessageHeader
{
	FvUInt8 m_uiComponentPriority;
	FvUInt8 m_uiMessagePriority;
};
#pragma pack( pop )

#pragma pack( push, 1 )

class FV_NETWORK_API FvNetLoggerComponentMessage
{
public:
	FvUInt8 m_uiVersion;
	FvUInt8 m_uiLoggerID;
	FvUInt16 m_uiUID;
	FvUInt32 m_uiPID;
	FvString m_kComponentName;

	void Write( FvBinaryOStream &os ) const;
	void Read( FvBinaryIStream &is );
};
#pragma pack( pop )

class FV_NETWORK_API FvNetSimpleLoggerMessageForwarder :
	public FvDebugMessageCallback
{
public:
	FvNetSimpleLoggerMessageForwarder(
		FvString appName,
		FvNetEndpoint & endpoint,
		FvUInt16 uiMachinePort,
		FvUInt8 loggerID = 0,
		bool enabled = true );

	virtual ~FvNetSimpleLoggerMessageForwarder();

	void RegisterAppID( int id );

protected:

	virtual bool HandleMessage( int componentPriority,
		int messagePriority, const char * format, va_list argPtr );

	void AddLogger( const FvNetAddress & addr );
	void DelLogger( const FvNetAddress & addr );
	void SendAppID( const FvNetAddress & addr );

	void FindLoggerInterfaces(FvUInt16 uiMachinePort);

	class FindLoggerHandler : public FvNetMachineGuardMessage::ReplyHandler
	{
	public:
		FindLoggerHandler( FvNetSimpleLoggerMessageForwarder &lmf ) : m_kLoggerMessageForwarder( lmf ) {}
		virtual bool OnProcessStatsMessage(
			FvNetProcessStatsMessage &psm, FvUInt32 addr );

	private:
		FvNetSimpleLoggerMessageForwarder &m_kLoggerMessageForwarder;
	};

	void ParseAndSend( FvNetForwardingStringHandler * pHandler,
		int componentPriority, int messagePriority, va_list argPtr );

	void ParseAndSend( FvNetForwardingStringHandler * pHandler,
		int componentPriority, int messagePriority, ... );

	typedef std::vector< FvNetAddress > Loggers;
	Loggers m_kLoggers;

	FvString m_kAppName;

	FvUInt8 m_uiLoggerID;
	
	int m_iAppID;
	bool m_bEnabled;

	FvNetEndpoint &m_kEndpoint;

	typedef std::map< FvString, FvNetForwardingStringHandler* > HandlerCache;
	HandlerCache m_kHandlerCache;

	FvNetForwardingStringHandler *FindForwardingStringHandler( const char * format );

	virtual bool IsSuppressible( const FvString & format ) const;
};

class FV_NETWORK_API FvNetLoggerMessageForwarder :
	public FvNetSimpleLoggerMessageForwarder,
	public FvNetTimerExpiryHandler,
	public FvSingleton< FvNetLoggerMessageForwarder >
{
public:
	FvNetLoggerMessageForwarder(
		FvString appName,
		FvNetEndpoint & endpoint,
		FvNetNub & nub,
		FvUInt8 loggerID = 0,
		bool enabled = true,
		unsigned spamFilterThreshold = 10 );

	virtual ~FvNetLoggerMessageForwarder();

	FvString SuppressionWatcherHack() const { return FvString(); }
	void AddSuppressionPattern( FvString prefix );
	void DelSuppressionPattern( FvString prefix );

protected:
	virtual bool HandleMessage( int componentPriority,
		int messagePriority, const char * format, va_list argPtr );

	virtual int HandleTimeout( FvNetTimerID id, void * arg );

private:
	void Init();

	FvNetAddress WatcherHack() const;

	void WatcherAddLogger( FvNetAddress addr ) { this->AddLogger( addr ); }
	void WatcherDelLogger( FvNetAddress addr ) { this->DelLogger( addr ); }

	int size() const	{ return m_kLoggers.size(); }

	bool IsSuppressible( const FvString & format ) const;

	bool IsSpamming( FvNetForwardingStringHandler * pHandler ) const
	{
		return (m_uiSpamFilterThreshold > 0) &&
			pHandler->IsSuppressible() &&
			(pHandler->NumRecentCalls() > m_uiSpamFilterThreshold);
	}

	void UpdateSuppressionPatterns();

	FvNetNub &m_kNub;

	typedef std::vector< FvString > SuppressionPatterns;
	SuppressionPatterns m_kSuppressionPatterns;

	FvNetTimerID m_kSpamTimerID;

	unsigned m_uiSpamFilterThreshold;

	FvNetForwardingStringHandler m_kSpamHandler;

	typedef std::vector< FvNetForwardingStringHandler* > RecentlyUsedHandlers;
	RecentlyUsedHandlers m_kRecentlyUsedHandlers;
};

#define FV_NET_MESSAGE_FORWARDER2( NAME, CONFIG_PATH, ENABLED, NUB )			\
	FvString kMonitoringInterfaceName =											\
		FvServerConfig::Get( #CONFIG_PATH "/monitoringInterface",				\
				FvServerConfig::Get( "monitoringInterface", "" ) );				\
																				\
	NUB.SetLossRatio( FvServerConfig::Get( #CONFIG_PATH "/internalLossRatio",	\
				FvServerConfig::Get( "internalLossRatio", 0.f ) ) );			\
	NUB.SetLatency(																\
			FvServerConfig::Get( #CONFIG_PATH "/internalLatencyMin",			\
				FvServerConfig::Get( "internalLatencyMin", 0.f ) ),				\
			FvServerConfig::Get( #CONFIG_PATH "/internalLatencyMax",			\
				FvServerConfig::Get( "internalLatencyMax", 0.f ) ) );			\
																				\
	NUB.SetIrregularChannelsResendPeriod(										\
			FvServerConfig::Get( #CONFIG_PATH "/irregularResendPeriod",			\
				FvServerConfig::Get( "irregularResendPeriod",					\
					1.5f / FvServerConfig::Get( "gameUpdateHertz", 10.f ) ) ) );\
																				\
	NUB.ShouldUseChecksums(														\
		FvServerConfig::Get( #CONFIG_PATH "/shouldUseChecksums",				\
			FvServerConfig::Get( "shouldUseChecksums", true ) ) );				\
																				\
	FvNetChannel::SetInternalMaxOverflowPackets(								\
		FvServerConfig::Get( "maxChannelOverflow/internal",						\
		FvNetChannel::GetInternalMaxOverflowPackets() ));						\
																				\
	FvNetChannel::SetIndexedMaxOverflowPackets(									\
		FvServerConfig::Get( "maxChannelOverflow/indexed",						\
		FvNetChannel::GetIndexedMaxOverflowPackets() ));						\
																				\
	FvNetChannel::SetExternalMaxOverflowPackets(								\
		FvServerConfig::Get( "maxChannelOverflow/external",						\
		FvNetChannel::GetExternalMaxOverflowPackets() ));						\
																				\
	FvNetChannel::AssertOnMaxOverflowPackets(									\
		FvServerConfig::Get( "maxChannelOverflow/isAssert",						\
		FvNetChannel::AssertOnMaxOverflowPackets() ));							\
																				\
	if (kMonitoringInterfaceName == "")											\
	{																			\
		kMonitoringInterfaceName =												\
						inet_ntoa( (struct in_addr &)NUB.Address().m_uiIP );	\
	}																			\
																				\
	FvNetWatcherGlue kWatcherGlue(FvServerConfig::Get( "machinePort",			\
									FvUInt16(FV_NET_PORT_MACHINED) ));			\
	kWatcherGlue.Init( kMonitoringInterfaceName.c_str(), 0 );					\
																				\
	unsigned int uiSpamFilterThreshold =										\
		FvServerConfig::Get( #CONFIG_PATH "/logSpamThreshold",					\
			FvServerConfig::Get( "logSpamThreshold", 20 ) );					\
																				\
	FvNetLoggerMessageForwarder lForwarder( #NAME,								\
		kWatcherGlue.Socket(), NUB,												\
		FvServerConfig::Get( "loggerID", 0 ), ENABLED, uiSpamFilterThreshold );	\
																				\
	FvXMLSectionPtr spSuppressionPatterns =										\
		FvServerConfig::GetSection( #CONFIG_PATH "/logSpamPatterns",			\
			FvServerConfig::GetSection( "logSpamPatterns" ) );					\
																				\
	if (spSuppressionPatterns)													\
	{																			\
		for (FvXMLSectionIterator kIter = spSuppressionPatterns->Begin();		\
			 kIter != spSuppressionPatterns->End(); ++kIter)					\
		{																		\
				lForwarder.AddSuppressionPattern( (*kIter)->AsString() );		\
			}																	\
		}																		\
																				\
		if (FvServerConfig::IsBad())											\
	{																			\
		return 0;																\
	}																			\
	(void)0

#define FV_NET_MESSAGE_FORWARDER( NAME, CONFIG_PATH, NUB )						\
	FV_NET_MESSAGE_FORWARDER2( NAME, CONFIG_PATH, true, NUB )

#endif // FV_ENABLE_WATCHERS

#endif // __FvNetLoggerMessageForwarder_H__
