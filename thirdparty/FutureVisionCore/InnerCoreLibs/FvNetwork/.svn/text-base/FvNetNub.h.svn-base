//{future header message}
#ifndef __FvNetNub_H__
#define __FvNetNub_H__

#include "FvNetwork.h"
#include "FvNetEndpoint.h"
#include "FvNetInterfaces.h"
#include "FvNetInterfaceElement.h"
#include "FvNetCondemnedChannels.h"
#include "FvNetIrregularChannels.h"
#include "FvNetKeepAliveChannels.h"
#include "FvNetMachineGuard.h"
#include "FvNetMisc.h"
#include "FvNetPacket.h"
#include "FvNetPortMap.h"

#include <FvTimestamp.h>

#include <list>
#include <map>
#include <queue>
#include <set>

class FvNetBundle;
class FvNetChannel;
class FvNetChannelFinder;
class FvNetInterfaceElement;
class FvNetInterfaceIterator;
class FvNetPacketFilter;
class FvNetPacketMonitor;

typedef FvSmartPointer< FvNetChannel > FvNetChannelPtr;

class FV_NETWORK_API FvNetNubException
{
public:
	FvNetNubException( FvNetReason reason );
	virtual ~FvNetNubException() {};
	FvNetReason Reason() const;
	virtual bool GetAddress( FvNetAddress & addr ) const;

private:
	FvNetReason m_eReason;
};


class FV_NETWORK_API NubExceptionWithAddress : public FvNetNubException
{
public:
	NubExceptionWithAddress( FvNetReason reason, const FvNetAddress & addr );
	virtual bool GetAddress( FvNetAddress & addr ) const;

private:
	FvNetAddress m_kAddress;
};

struct ErrorReportAndCount
{
	FvUInt64 m_uiLastReportStamps;
	FvUInt64 m_uiLastRaisedStamps;
	unsigned int m_uiCount;
};

typedef std::pair< FvNetAddress, FvString > AddressAndErrorString;

typedef std::map< AddressAndErrorString, ErrorReportAndCount >
	ErrorsAndCounts;



class FV_NETWORK_API FvNetNub :
	public FvNetInputMessageHandler,
	public FvNetTimerExpiryHandler,
	public FvNetInputNotificationHandler
{
public:
	static const int RECV_BUFFER_SIZE;

	FvNetNub( FvUInt16 uiMachinePort = FV_NET_PORT_MACHINED,
			FvUInt16 uiUserID = 1,
			FvUInt16 listeningPort = 0,
			const char * listeningInterface = 0 );
	virtual ~FvNetNub();

	bool RecreateListeningSocket( FvUInt16 listeningPort,
		const char * listeningInterface );

	void ServeInterfaceElement( const FvNetInterfaceElement & ie, FvNetMessageID id,
		FvNetInputMessageHandler * pHandler );

	FvNetReason RegisterWithMachined( const FvString & name, int id,
		bool isRegister = true );

	FvNetReason DeregisterWithMachined();

	FvNetReason RegisterBirthListener(FvNetBundle& kBundle, int iAddrStart,
								const char* pcIfName, FvUInt16 uiUserID);
	FvNetReason RegisterDeathListener(FvNetBundle& kBundle, int iAddrStart,
								const char* pcIfName, FvUInt16 uiUserID);

	FvNetReason RegisterBirthListener(const FvNetInterfaceElement& kIE,
								const char* pcIfName);
	FvNetReason RegisterDeathListener(const FvNetInterfaceElement& kIE,
								const char* pcIfName);

	//! kIE:		处理监听消息的接口
	//! pcIfName:	监听的消息,NULL/"":全部Interface, "abc":指定Interface
	//! uiUserID:	0:全局UserID, 1-65534:指定的UserID, 65535:全部UserID
	FvNetReason RegisterBirthListener(const FvNetInterfaceElement& kIE,
								const char* pcIfName, FvUInt16 uiUserID);
	FvNetReason RegisterDeathListener(const FvNetInterfaceElement& kIE,
								const char* pcIfName, FvUInt16 uiUserID);

	FvNetReason FindInterface( const char * name, int id, FvNetAddress & address,
		int retries = 0, bool verboseRetry = true );

	struct InterfaceInfo
	{
		FvUInt16		uiUserID;
		FvUInt16		uiID;
		FvNetAddress	kAddr;
		FvString		kName;

		InterfaceInfo(FvUInt16 uiUserID_, FvUInt16 uiID_, FvNetAddress& kAddr_, FvString& kName_)
		:uiUserID(uiUserID_), uiID(uiID_), kAddr(kAddr_), kName(kName_) {}
	};
	typedef std::vector<InterfaceInfo> InterfaceInfoVec;
	//! uiUserID:			0:全局UserID, 1-65534:指定的UserID, 65535:全部UserID
	//! pcInterfaceName:	NULL/"":全部Interface, "abc":指定Interface
	//! iID:				<0:全部id, >=0:指定id
	//! kInfos:				返回查到的Interface信息s
	FvNetReason FindInterfaces( FvUInt16 uiUserID, const char* pcInterfaceName, int iID, InterfaceInfoVec& kInfos,
		int retries = 0, bool verboseRetry = true );

	bool ProcessPendingEvents( bool expectingPacket = false );
	void ProcessContinuously();
	void ProcessUntilBreak();
	void ProcessUntilChannelsEmpty( float timeout = 10.f );
	void ProcessFrame();

	FvNetReason ReceivedCorruptedPacket();

	void HandleInputNotifications( int &countReady,
		fd_set &readFDs, fd_set &writeFDs );

	virtual int HandleInputNotification( int fd );

	void BreakProcessing( bool breakState = true );
	bool ProcessingBroken() const;
	void BreakBundleLoop();
	void DrainSocketInput();

	void Shutdown();

	const FvNetAddress & Address() const;
	int Socket() const;

	void Send( const FvNetAddress & address, FvNetBundle & bundle,
		FvNetChannel * pChannel = NULL );
	void SendPacket( const FvNetAddress & addr, FvNetPacket * p,
			FvNetChannel * pChannel, bool isResend );

	FvNetReason BasicSendWithRetries( const FvNetAddress & addr, FvNetPacket * p );
	FvNetReason BasicSendSingleTry( const FvNetAddress & addr, FvNetPacket * p );

	void DelayedSend( FvNetChannel * pChannel );

	FV_INLINE FvNetTimerID RegisterTimer( int microseconds,
					FvNetTimerExpiryHandler * handler, void* arg = NULL );
	FvNetTimerID RegisterCallback( int microseconds, FvNetTimerExpiryHandler * handler,
			void * arg = NULL );
	void CancelTimer( FvNetTimerID id );
	int CancelTimers( FvNetTimerExpiryHandler * pHandler );
	FvUInt64   TimerDeliveryTime( FvNetTimerID id ) const;
	FvUInt64 & TimerIntervalTime( FvNetTimerID id );

	bool RegisterFileDescriptor( int fd, FvNetInputNotificationHandler * handler );
	bool DeregisterFileDescriptor( int fd );
	bool RegisterWriteFileDescriptor( int fd, FvNetInputNotificationHandler * handler );
	bool DeregisterWriteFileDescriptor( int fd );
	void SetBundleFinishHandler( FvNetBundleFinishHandler * handler );
	void FindLargestFileDescriptor();

	bool RegisterChildNub( FvNetNub * pChildNub,
		FvNetInputNotificationHandler * pHandler = NULL );

	bool DeregisterChildNub( FvNetNub * pChildNub );

	void OnChannelGone( FvNetChannel * pChannel );
	void CancelRequestsFor( FvNetChannel * pChannel );

	FV_INLINE void PrepareToShutDown();

	FvNetCondemnedChannels &CondemnedChannels() { return m_kCondemnedChannels; }
	FvNetIrregularChannels &IrregularChannels()	{ return m_kIrregularChannels; }
	FvNetKeepAliveChannels &KeepAliveChannels()	{ return m_kKeepAliveChannels; }

	void SetIrregularChannelsResendPeriod( float seconds )
	{
		m_kIrregularChannels.SetPeriod( seconds, *this );
	}

	FvUInt64 GetSpareTime() const;
	void ClearSpareTime();
	double ProportionalSpareTime() const;

#if FV_ENABLE_WATCHERS
	static FvWatcherPtr GetWatcher();
#endif

	void SetLatency( float latencyMin, float latencyMax );
	void SetLossRatio( float lossRatio );

	bool HasArtificialLossOrLatency() const
	{
		return (m_iArtificialLatencyMin != 0) || (m_iArtificialLatencyMax != 0) ||
			(m_iArtificialDropPerMillion != 0);
	}

	void SetPacketMonitor( FvNetPacketMonitor* pPacketMonitor );

	bool RegisterChannel( FvNetChannel & channel );
	bool DeregisterChannel( FvNetChannel & channel );

	void RegisterChannelFinder( FvNetChannelFinder *pFinder );

	void DropNextSend( bool v = true ) { m_bDropNextSend = v; }

	void OnAddressDead( const FvNetAddress & addr );

	bool Rebind( const FvNetAddress & addr );

	void SwitchSockets( FvNetNub * pOtherNub );

	bool HandleHijackData( int fd, FvBinaryIStream & stream );

	FV_INLINE bool IsVerbose() const;
	FV_INLINE void IsVerbose( bool value );

	const char * c_str() const { return m_kSocket.c_str(); }

	const char * MsgName( FvNetMessageID msgID ) const
	{
		return m_kInterfaceTable[ msgID ].GetName();
	}

	static const char *USE_FVMACHINED;
private:
	FvNetEndpoint m_kSocket;

	class QueryInterfaceHandler : public FvNetMachineGuardMessage::ReplyHandler
	{
	public:
		QueryInterfaceHandler( int requestType );
		bool OnQueryInterfaceMessage( FvNetQueryInterfaceMessage &qim, FvUInt32 addr );

		bool m_bHasResponded;
		u_int32_t m_iAddress;
		char m_cRequest;
	};

	bool QueryMachinedInterface(u_int32_t& addr);

	class ProcessMessageHandler : public FvNetMachineGuardMessage::ReplyHandler
	{
	public:
		ProcessMessageHandler() { m_bHasResponded = false; }
		bool OnProcessMessage( FvNetProcessMessage &pm, FvUInt32 addr );

		bool m_bHasResponded;
	};

	FvNetReason RegisterListener(FvNetBundle& kBundle,
		int iAddrStart, const char* pcIfName, bool bIsBirth, FvUInt16 uiUserID);

	FvString m_kInterfaceName;

	int m_iInterfaceID;

	typedef std::vector< FvNetInterfaceElementWithStats > InterfaceTable;
	InterfaceTable m_kInterfaceTable;

	class TimerQueueElement
	{
	public:
		FvUInt64 m_uiDeliveryTime;
		FvUInt64 m_uiIntervalTime;
		int m_iState;
		void *arg;
		FvNetTimerExpiryHandler	*handler;

		enum
		{
			STATE_PENDING = 0,
			STATE_EXECUTING = 1,
			STATE_CANCELLED = 2
		};
	};

	class TimerQueueComparator
	{
	public:
		bool operator()
			( const TimerQueueElement * a, const TimerQueueElement * b )
		{
			return a->m_uiDeliveryTime > b->m_uiDeliveryTime;
		}
	};

	typedef std::priority_queue< TimerQueueElement*,
		std::vector<TimerQueueElement*>, TimerQueueComparator > TimerQueue;
	TimerQueue m_kTimerQueue;
	TimerQueueElement *m_pkCurrentTimer;

	bool RescheduleSend( const FvNetAddress & addr, FvNetPacket * packet, bool isResend );

	int m_iArtificialDropPerMillion;

	int m_iArtificialLatencyMin;
	int m_iArtificialLatencyMax;

	bool m_bDropNextSend;

	class RescheduledSender : public FvNetTimerExpiryHandler
	{
	public:
		RescheduledSender( FvNetNub & nub, const FvNetAddress & addr, FvNetPacket * pPacket,
			int latencyMilli, bool isResend );

		virtual int HandleTimeout( FvNetTimerID id, void * arg );

	private:
		FvNetNub &m_kNub;
		FvNetAddress m_kAddr;
		FvNetPacketPtr m_spPacket;
		bool m_bIsResend;
	};

	FvNetReason ProcessPacket( const FvNetAddress & addr, FvNetPacket * p );
	FvNetReason ProcessFilteredPacket( const FvNetAddress & addr, FvNetPacket * p );
	FvNetReason ProcessOrderedPacket( const FvNetAddress & addr, FvNetPacket * p,
		FvNetChannel * pChannel );

public:
	FvNetReason ProcessPacketFromStream( const FvNetAddress & addr, FvBinaryIStream & data );

private:
	void DumpBundleMessages( FvNetBundle & bundle );

	friend class FvNetPacketFilter;

	FvNetReplyID m_iNextReplyID;
	FvNetReplyID GetNextReplyID()
	{
		if (m_iNextReplyID > FV_NET_REPLY_ID_MAX)
			m_iNextReplyID = 1;

		return m_iNextReplyID++;
	}

	FvNetSeqNum m_uiNextSequenceID;
	FvNetSeqNum GetNextSequenceID();

	class ReplyHandlerElement : public FvNetTimerExpiryHandler
	{
	public:
		int m_iReplyID;
		FvNetTimerID m_kTimerID;
		FvNetReplyMessageHandler *m_pkHandler;
		void *m_pkArg;
		FvNetChannel *m_pkChannel;

		virtual int HandleTimeout( FvNetTimerID id, void * nubArg );
		void HandleFailure( FvNetNub & nub, FvNetReason reason );
	};
	friend class ReplyHandlerElement;

	typedef std::map<int, ReplyHandlerElement *> ReplyHandlerMap;
	ReplyHandlerMap m_kReplyHandlerMap;

public:
	void CancelReplyMessageHandler( FvNetReplyMessageHandler * pHandler,
		   FvNetReason reason = FV_NET_REASON_CHANNEL_LOST );

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data );
private:

	FvNetTimerID NewTimer( int microseconds,
		FvNetTimerExpiryHandler * handler,
		void * arg,
		bool recurrent );

	void FinishProcessingTimerEvent( TimerQueueElement * pElement );

	FvNetPacketPtr m_spNextPacket;
	FvNetAddress m_kAdvertisedAddress;

public:
	class FragmentedBundle : public FvSafeReferenceCount
	{
	public:
		FragmentedBundle( FvNetSeqNum lastFragment, int remaining,
				FvUInt64 touched, FvNetPacket * firstPacket ) :
			m_uiLastFragment( lastFragment ),
			m_iRemaining( remaining ),
			m_uiTouched( touched ),
			m_spChain( firstPacket )
		{}

		static const FvUInt64 MAX_AGE = 10;

		bool IsOld() const;
		bool IsReliable() const;

		FvNetSeqNum m_uiLastFragment;
		int	m_iRemaining;
		FvUInt64 m_uiTouched;
		FvNetPacketPtr m_spChain;

		class Key
		{
		public:
			Key( const FvNetAddress & addr, FvNetSeqNum firstFragment ) :
				m_kAddr( addr ),
				m_uiFirstFragment( firstFragment )
			{}

			bool operator==(const FvNetNub::FragmentedBundle::Key& kOther) const;
			bool operator<(const FvNetNub::FragmentedBundle::Key& kOther) const;

			FvNetAddress m_kAddr;
			FvNetSeqNum m_uiFirstFragment;
		};
	};

	typedef FvSmartPointer< FragmentedBundle > FragmentedBundlePtr;

private:
	typedef std::map< FragmentedBundle::Key, FragmentedBundlePtr >
		FragmentedBundles;

	FragmentedBundles m_kFragmentedBundles;

	FvNetTimerID m_kClearFragmentedBundlesTimerID;

	bool m_bBreakProcessing;
	bool m_bBreakBundleLoop;
	bool m_bDrainSocketInput;

#ifdef _WIN32
	std::map<int,FvNetInputNotificationHandler*> m_apFDHandlers;
	std::map<int,FvNetInputNotificationHandler*> m_apFDWriteHandlers;
#else
	FvNetInputNotificationHandler *m_apFDHandlers[FD_SETSIZE];
	FvNetInputNotificationHandler *m_apFDWriteHandlers[FD_SETSIZE];
#endif

	fd_set m_kFDReadSet;
	fd_set m_kFDWriteSet;
	int m_iFDLargest;
	int m_iFDWriteCount;

	FvNetBundleFinishHandler *m_pkBundleFinishHandler;

	FvNetPacketMonitor *m_pkPacketMonitor;

	typedef std::map< FvNetAddress, FvNetChannel * > ChannelMap;
	ChannelMap m_kChannelMap;

	FvNetChannelFinder *m_pkChannelFinder;

	FvUInt64 m_uiLastStatisticsGathered;
	int m_iLastTxQueueSize;
	int m_iLastRxQueueSize;
	int m_iMaxTxQueueSize;
	int m_iMaxRxQueueSize;

public:
	FvNetChannel * FindChannel( const FvNetAddress & addr, bool createAnonymous = false );

	FvNetChannel & FindOrCreateChannel( const FvNetAddress & addr )
	{
		return *this->FindChannel( addr, /* createAnonymous: */ true );
	}

	void DelAnonymousChannel( const FvNetAddress & addr );

	unsigned int NumPacketsReceived() const;
	unsigned int NumMessagesReceived() const;
	unsigned int NumBytesReceived() const;
	unsigned int NumOverheadBytesReceived() const;
	unsigned int NumBytesReceivedForMessage(FvUInt8 message) const;
	unsigned int NumMessagesReceivedForMessage(FvUInt8 message) const;

	double BytesSentPerSecondAvg() const;
	double BytesSentPerSecondPeak() const;
	double BytesReceivedPerSecondAvg() const;
	double BytesReceivedPerSecondPeak() const;
	double PacketsSentPerSecondAvg() const;
	double PacketsSentPerSecondPeak() const;
	double PacketsReceivedPerSecondAvg() const;
	double PacketsReceivedPerSecondPeak() const;
	double BundlesSentPerSecondAvg() const;
	double BundlesSentPerSecondPeak() const;
	double BundlesReceivedPerSecondAvg() const;
	double BundlesReceivedPerSecondPeak() const;
	double MessagesSentPerSecondAvg() const;
	double MessagesSentPerSecondPeak() const;
	double MessagesReceivedPerSecondAvg() const;
	double MessagesReceivedPerSecondPeak() const;

	void IncNumDuplicatePacketsReceived()
	{
		++m_uiNumDuplicatePacketsReceived;
	}

	struct MiniTimer
	{
		MiniTimer();
		void start();
		void stop( bool increment = false );
		void reset();

		double GetMinDurationSecs() const;
		double GetMaxDurationSecs() const;
		double GetAvgDurationSecs() const;

		FvUInt64 m_uiTotal;	///< The total time spent performing this operation.
		FvUInt64 m_uiLast;	///< The current time spent performing this operation.
		FvUInt64 m_uiSofar;	///< The time this operation last commenced.
		FvUInt64 m_uiMin; 	///< The minimum time taken to perform the operation.
		FvUInt64 m_uiMax; 	///< The maximum time taken to perform the operation.
		unsigned int m_uiCount;	///< The number of times this operation has occurred.
	};

	class TransientMiniTimer : public MiniTimer
	{
		FvUInt64 m_uiResetPeriodStamp;
		FvUInt64 m_uiResetTimeStamp;

	public:
		TransientMiniTimer(int resetPeriodSecs);

		FV_INLINE void start();
		FV_INLINE void stop();
		FV_INLINE void reset();

		double GetElapsedResetSecs() const;
		double GetCountPerSec()	const;

	public:
		template < class TIMERTYPE >
		class Op
		{
			TIMERTYPE& m_kTimer;
		public:
			Op(TIMERTYPE& timer)
				: m_kTimer(timer)
			{	m_kTimer.start();	}

			~Op()
			{	m_kTimer.stop();	}
		};
	};

	int * LoopStats()	{ return m_aiLoopStats; }
	enum LoopStat
	{
		RECV_TRYS = 0,
		RECV_GETS,
		RECV_ERRS,
		RECV_SELS,

		TIMER_CALLS = 8,
		TIMER_RESCHEDS
	};

private:
	unsigned int m_uiNumBytesSent;
	unsigned int m_uiNumBytesResent;
	unsigned int m_uiNumBytesReceived;
	unsigned int m_uiNumPacketsSent;
	unsigned int m_uiNumPacketsResent;
	unsigned int m_uiNumPiggybacks;
	unsigned int m_uiNumPacketsSentOffChannel;
	unsigned int m_uiNumPacketsReceived;
	unsigned int m_uiNumDuplicatePacketsReceived;
	unsigned int m_uiNumPacketsReceivedOffChannel;
	unsigned int m_uiNumBundlesSent;
	unsigned int m_uiNumBundlesReceived;
	unsigned int m_uiNumMessagesSent;
	unsigned int m_uiNumReliableMessagesSent;
	unsigned int m_uiNumMessagesReceived;
	unsigned int m_uiNumOverheadBytesReceived;
	unsigned int m_uiNumFailedPacketSend;
	unsigned int m_uiNumFailedBundleSend;
	unsigned int m_uiNumCorruptedPacketsReceived;
	unsigned int m_uiNumCorruptedBundlesReceived;

	mutable unsigned int m_uiLastNumBytesSent;
	mutable unsigned int m_uiLastNumBytesReceived;
	mutable unsigned int m_uiLastNumPacketsSent;
	mutable unsigned int m_uiLastNumPacketsReceived;
	mutable unsigned int m_uiLastNumBundlesSent;
	mutable unsigned int m_uiLastNumBundlesReceived;
	mutable unsigned int m_uiLastNumMessagesSent;
	mutable unsigned int m_uiLastNumMessagesReceived;

	MiniTimer m_kSendMercuryTimer;
	MiniTimer m_kSendSystemTimer;
	MiniTimer m_kRecvMercuryTimer;
	MiniTimer m_kRecvSystemTimer;

	FvUInt64 m_uiSpareTime, m_uiAccSpareTime;
	FvUInt64 m_uiOldSpareTime, m_uiTotSpareTime;

	int		m_aiLoopStats[16];

	enum LastVisitTime
	{
		LVT_BytesSent,
		LVT_BytesReceived,
		LVT_PacketsSent,
		LVT_PacketsReceived,
		LVT_BundlesSent,
		LVT_BundlesReceived,
		LVT_MessagesSent,
		LVT_MessagesReceived,
		LVT_END
	};
	mutable FvUInt64 lastVisitTime_[LVT_END];
	FvUInt64 startupTime_;

	double Delta() const { return (double)( Timestamp() - startupTime_ ) / StampsPerSecondD(); };
	double PeakCalculator( LastVisitTime, const unsigned int &, unsigned int & ) const;

public:
	struct IOpportunisticPoller
	{
		virtual ~IOpportunisticPoller() {};

		virtual void Poll() = 0;
	};

	void SetOpportunisticPoller(IOpportunisticPoller* pPoller)	{	m_pkOpportunisticPoller = pPoller;	}
	IOpportunisticPoller* GetOpportunisticPoller() const		{	return m_pkOpportunisticPoller;	}

private:
	IOpportunisticPoller* m_pkOpportunisticPoller;

public:
	class OnceOffReceipt
	{
	public:
		OnceOffReceipt( const FvNetAddress & addr, int footerSequence ) :
			m_kAddr( addr ),
			m_iFooterSequence( footerSequence )
		{
		}

		FV_INLINE bool
			operator<(const FvNetNub::OnceOffReceipt &kOther) const
		{
			return (this->m_iFooterSequence < kOther.m_iFooterSequence) ||
				((this->m_iFooterSequence == kOther.m_iFooterSequence) && (this->m_kAddr < kOther.m_kAddr));
		}

		FV_INLINE bool
			operator==(const FvNetNub::OnceOffReceipt &kOther) const
		{
			return (this->m_iFooterSequence == kOther.m_iFooterSequence) && (this->m_kAddr == kOther.m_kAddr);
		}

		FvNetAddress m_kAddr;
		int m_iFooterSequence;
	};

	void * pExtensionData() const			{ return m_pkExtensionData; }
	void pExtensionData( void * pData )		{ m_pkExtensionData = pData; }

private:
	typedef std::set< OnceOffReceipt > OnceOffReceipts;
	OnceOffReceipts m_kCurrOnceOffReceipts;
	OnceOffReceipts m_kPrevOnceOffReceipts;

	class OnceOffPacket : public FvNetTimerExpiryHandler, public OnceOffReceipt
	{
	public:
		OnceOffPacket( const FvNetAddress & addr, int footerSequence,
						FvNetPacket * pPacket = NULL ) :
			OnceOffReceipt( addr, footerSequence ),
			m_spPacket( pPacket ),
			m_kTimerID( 0 ),
			m_iRetries( 0 )
		{
		}

		void RegisterTimer( FvNetNub * pNub )
		{
			m_kTimerID = pNub->RegisterTimer(
				pNub->OnceOffResendPeriod() /* microseconds */,
				this /* handler */,
				pNub /* arg */ );
		}

		virtual int HandleTimeout( FvNetTimerID id, void * arg )
		{
			static_cast< FvNetNub * >( arg )->ExpireOnceOffResendTimer( *this );
			return 0;
		}

		FvNetPacketPtr m_spPacket;
		FvNetTimerID m_kTimerID;
		int m_iRetries;
	};
	typedef std::set< OnceOffPacket > OnceOffPackets;
	OnceOffPackets m_kOnceOffPackets;

private:

	virtual int HandleTimeout( FvNetTimerID id, void * arg );
	void InitOnceOffPacketCleaning();
	void OnceOffReliableCleanup();

	void AddOnceOffResendTimer( const FvNetAddress & addr, int seq, FvNetPacket * p );
	void ExpireOnceOffResendTimer( OnceOffPacket & packet );
	void DelOnceOffResendTimer( const FvNetAddress & addr, int seq );
	void DelOnceOffResendTimer( OnceOffPackets::iterator & iter );

	bool OnceOffReliableReceived( const FvNetAddress & addr, int seq );
public:
	int OnceOffResendPeriod() const
	{	return m_iOnceOffResendPeriod; }

	void OnceOffResendPeriod( int microseconds )
	{	m_iOnceOffResendPeriod = microseconds; }

	int OnceOffMaxResends() const
	{	return m_iOnceOffMaxResends; }

	void OnceOffMaxResends( int retries )
	{	m_iOnceOffMaxResends = retries; }

	void IsExternal( bool state )
	{
		m_bIsExternal = state;
	}

	void ShouldUseChecksums( bool b )
	{	m_bShouldUseChecksums = b; }

	bool ShouldUseChecksums() const
	{	return m_bShouldUseChecksums; }

	FvUInt16 GetMachinePort() { return m_uiMachinePort; }
	FvUInt16 GetUserID() { return m_uiUserID; }

private:
	FvNetTimerID m_kOnceOffPacketCleaningTimerID;
	int m_iOnceOffPacketCleaningPeriod;
	int m_iOnceOffMaxResends;
	int m_iOnceOffResendPeriod;
	void *m_pkExtensionData;

	bool m_bIsExternal;

public:
	void ReportException( FvNetNubException & ne,
			const char * prefix = NULL);
	void ReportError( const FvNetAddress & address,
			const char* format, ... );
	void ReportPendingExceptions( bool reportBelowThreshold = false );

private:
	static const unsigned int ERROR_REPORT_MIN_PERIOD_MS;

	static const unsigned int ERROR_REPORT_COUNT_MAX_LIFETIME_MS;

	void AddReport( const FvNetAddress & address, const FvString & error );

	static FvString AddressErrorToString(
			const FvNetAddress & address,
			const FvString & errorString );

	static FvString AddressErrorToString(
			const FvNetAddress & address,
			const FvString & errorString,
			const ErrorReportAndCount & reportAndCount,
			const FvUInt64 & now );

	FvNetTimerID m_kReportLimitTimerID;
	ErrorsAndCounts m_kErrorsAndCounts;

	FvNetNub *m_pkMasterNub;

	typedef std::list< FvNetNub* > ChildNubs;
	ChildNubs m_kChildNubs;

	FvNetTimerID m_kTickChildNubsTimerID;

	static const int CHILD_NUB_TICK_PERIOD = 50000;

	typedef std::map< FvNetAddress, FvNetTimerID > RecentlyDeadChannels;
	RecentlyDeadChannels m_kRecentlyDeadChannels;

	void sendDelayedChannels();

	typedef std::set< FvNetChannelPtr > DelayedChannels;
	DelayedChannels m_kDelayedChannels;

	enum TimeoutType
	{
		TIMEOUT_DEFAULT = 0,
		TIMEOUT_RECENTLY_DEAD_CHANNEL
	};

	FvNetIrregularChannels m_kIrregularChannels;
	FvNetCondemnedChannels m_kCondemnedChannels;
	FvNetKeepAliveChannels m_kKeepAliveChannels;

	bool m_bShouldUseChecksums;
	bool m_bIsVerbose;

	FvNetTimerID m_kInterfaceStatsTimerID;

	FvUInt16 m_uiMachinePort;
	FvUInt16 m_uiUserID;
};

FV_NETWORK_API std::ostream& operator <<( std::ostream &s, const FvNetNub::MiniTimer &v );
FV_NETWORK_API std::istream& operator >>( std::istream &s, FvNetNub::MiniTimer & );

#include "FvNetNub.inl"

#endif // __FvNetNub_H__
