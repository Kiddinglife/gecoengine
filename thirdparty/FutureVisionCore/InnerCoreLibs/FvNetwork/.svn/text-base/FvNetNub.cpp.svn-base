#include "FvNetNub.h"
#include "FvNetBundle.h"
#include "FvNetChannel.h"
#include "FvNetInterfaceMinder.h"
#include "FvNetInterfaceMinderMacros.h"

#include <FvProcess.h>
#include <FvMemoryStream.h>
#include <FvConcurrency.h>
#include <FvProfile.h>

#include <sstream>

#ifdef PLAYSTATION3
#include <netex/errno.h>
#define unix
#undef EAGAIN
#define EAGAIN SYS_NET_EAGAIN
#define ECONNREFUSED SYS_NET_ECONNREFUSED
#define EHOSTUNREACH SYS_NET_EHOSTUNREACH
#define ENOBUFS SYS_NET_ENOBUFS
#define select socketselect
#undef errno
#define errno sys_net_errno
#endif

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 );

const char *FvNetNub::USE_FVMACHINED = "fvmachined";

const unsigned int FvNetNub::ERROR_REPORT_MIN_PERIOD_MS = 2000; // 2 seconds

const unsigned int FvNetNub::ERROR_REPORT_COUNT_MAX_LIFETIME_MS = 10000; // 10 seconds

//! modify by Uman, 20100406
const int FvNetNub::RECV_BUFFER_SIZE = 128 * 1024 * 1024; // 64MB
//const int FvNetNub::RECV_BUFFER_SIZE = 16 * 1024 * 1024; // 16MB

FvNetNub::FvNetNub( FvUInt16 uiMachinePort, FvUInt16 uiUserID, FvUInt16 listeningPort, const char * listeningInterface ) :
	m_kSocket( /* useSyncHijack */ false ),
	m_kInterfaceName(),
	m_iInterfaceID( 0 ),
	m_kInterfaceTable( 256 ),
	m_kTimerQueue(),
	m_pkCurrentTimer( NULL ),
	m_iArtificialDropPerMillion( 0 ),
	m_iArtificialLatencyMin( 0 ),
	m_iArtificialLatencyMax( 0 ),
	m_bDropNextSend( false ),
	m_iNextReplyID( (FvUInt32(Timestamp())%100000) + 10101 ),
	m_uiNextSequenceID( 1 ),
	m_spNextPacket( NULL ),
	m_kClearFragmentedBundlesTimerID( FV_NET_TIMER_ID_NONE ),
	m_bBreakProcessing( false ),
	m_bDrainSocketInput( false ),
	m_iFDLargest( -1 ),
	m_iFDWriteCount( 0 ),
	m_pkBundleFinishHandler( NULL ),
	m_pkPacketMonitor( NULL ),
	m_kChannelMap(),
	m_pkChannelFinder( NULL ),
	m_uiLastStatisticsGathered( 0 ),
	m_iLastTxQueueSize( 0 ),
	m_iLastRxQueueSize( 0 ),
	m_iMaxTxQueueSize( 0 ),
	m_iMaxRxQueueSize( 0 ),
	m_uiNumBytesSent( 0 ),
	m_uiNumBytesResent( 0 ),
	m_uiNumBytesReceived( 0 ),
	m_uiNumPacketsSent( 0 ),
	m_uiNumPacketsResent( 0 ),
	m_uiNumPiggybacks( 0 ),
	m_uiNumPacketsSentOffChannel( 0 ),
	m_uiNumPacketsReceived( 0 ),
	m_uiNumDuplicatePacketsReceived( 0 ),
	m_uiNumPacketsReceivedOffChannel( 0 ),
	m_uiNumBundlesSent( 0 ),
	m_uiNumBundlesReceived( 0 ),
	m_uiNumMessagesSent( 0 ),
	m_uiNumReliableMessagesSent( 0 ),
	m_uiNumMessagesReceived( 0 ),
	m_uiNumOverheadBytesReceived( 0 ),
	m_uiNumFailedPacketSend( 0 ),
	m_uiNumFailedBundleSend( 0 ),
	m_uiNumCorruptedPacketsReceived( 0 ),
	m_uiNumCorruptedBundlesReceived( 0 ),
	m_uiLastNumBytesSent( 0 ),
	m_uiLastNumBytesReceived( 0 ),
	m_uiLastNumPacketsSent( 0 ),
	m_uiLastNumPacketsReceived( 0 ),
	m_uiLastNumBundlesSent( 0 ),
	m_uiLastNumBundlesReceived( 0 ),
	m_uiLastNumMessagesSent( 0 ),
	m_uiLastNumMessagesReceived( 0 ),
	m_uiSpareTime( 0 ),
	m_uiAccSpareTime( 0 ),
	m_uiOldSpareTime( 0 ),
	m_uiTotSpareTime( 0 ),
	m_pkOpportunisticPoller( 0 ),
	m_kOnceOffPacketCleaningTimerID( 0 ),
	m_iOnceOffMaxResends( FV_NET_DEFAULT_ONCEOFF_MAX_RESENDS ),
	m_iOnceOffResendPeriod( FV_NET_DEFAULT_ONCEOFF_RESEND_PERIOD ),
	m_bIsExternal( false ),
	m_kReportLimitTimerID( 0 ),
	m_pkMasterNub( NULL ),
	m_kTickChildNubsTimerID( FV_NET_TIMER_ID_NONE ),
	m_bShouldUseChecksums( false ),
	m_bIsVerbose( true ),
	m_kInterfaceStatsTimerID( FV_NET_TIMER_ID_NONE ),
	m_uiMachinePort(uiMachinePort),
	m_uiUserID(uiUserID)
{
	// FV_ASSERT( sizeof(InterfaceElement) == 8 );

	::StampsPerSecond();

	startupTime_ = Timestamp();
	for (int i = 0; i < LVT_END; i++)
	{
		lastVisitTime_[i] = startupTime_;
	}

	FD_ZERO( &m_kFDReadSet );
	FD_ZERO( &m_kFDWriteSet );
	m_iFDLargest = -1;

	this->RecreateListeningSocket( listeningPort, listeningInterface );

	this->ServeInterfaceElement( FvNetInterfaceElement::REPLY,
		FvNetInterfaceElement::REPLY.GetID(), this );

	m_spNextPacket = new FvNetPacket();
	memset( m_aiLoopStats, 0, sizeof(m_aiLoopStats) );

	m_kReportLimitTimerID = this->RegisterTimer( ERROR_REPORT_MIN_PERIOD_MS * 1000,
			this, NULL );

	m_kClearFragmentedBundlesTimerID = this->RegisterTimer(
		FragmentedBundle::MAX_AGE * 1000000, this, NULL );

	m_kInterfaceStatsTimerID = this->RegisterTimer( 1000000, this, NULL );
}


FvNetNub::~FvNetNub()
{
	if (!m_kReplyHandlerMap.empty())
	{
		FV_INFO_MSG( "FvNetNub::~FvNetNub: Num pending reply handlers = %u\n",
				m_kReplyHandlerMap.size() );

		ReplyHandlerMap::iterator iter = m_kReplyHandlerMap.begin();

		while (iter != m_kReplyHandlerMap.end())
		{
			delete iter->second;
			++iter;
		}

		m_kReplyHandlerMap.clear();
	}

	ChannelMap::iterator iter = m_kChannelMap.begin();
	while (iter != m_kChannelMap.end())
	{
		ChannelMap::iterator oldIter = iter++;
		FvNetChannel * pChannel = oldIter->second;

		if (pChannel->IsOwnedByNub())
		{
			// delete pChannel;
			pChannel->Destroy();
		}
		else
		{
			FV_WARNING_MSG( "Nub::~Nub: "
					"Channel to %s is still registered\n",
				pChannel->c_str() );
		}
	}

	this->DeregisterWithMachined();

	if (m_pkMasterNub)
	{
		m_pkMasterNub->DeregisterChildNub( this );
	}

	if (m_kSocket.Good())
	{
		m_kSocket.Close();
	}

	if (m_kOnceOffPacketCleaningTimerID)
	{
		this->CancelTimer( m_kOnceOffPacketCleaningTimerID );
		m_kOnceOffPacketCleaningTimerID = 0;
	}

	this->ReportPendingExceptions();
	if (m_kReportLimitTimerID)
	{
		this->CancelTimer( m_kReportLimitTimerID );
		m_kReportLimitTimerID = 0;
	}

	while (!m_kTimerQueue.empty())
	{
		TimerQueueElement * tqe = m_kTimerQueue.top();
		m_kTimerQueue.pop();

		this->CancelTimer( tqe );
		this->FinishProcessingTimerEvent( tqe );
	}
}


FvNetNub::QueryInterfaceHandler::QueryInterfaceHandler( int requestType ) :
	m_bHasResponded( false ),
	m_cRequest( requestType )
{
}


bool FvNetNub::QueryInterfaceHandler::OnQueryInterfaceMessage(
	FvNetQueryInterfaceMessage &qim, FvUInt32 addr )
{
	m_iAddress = qim.m_uiAddress;
	m_bHasResponded = true;

	return false;
}


bool FvNetNub::QueryMachinedInterface(u_int32_t& addr)
{
	FvNetEndpoint ep;
	u_int32_t ifaddr;

	ep.Socket( SOCK_DGRAM );

	if (ep.GetInterfaceAddress( FV_NET_LOCALHOSTNAME, ifaddr ) != 0)
	{
		FV_WARNING_MSG( "FvNetNub::QueryMachinedInterface: "
			"Could not get 'lo' by name, defaulting to 127.0.0.1\n" );
		ifaddr = FV_NET_LOCALHOST;
	}

	FvNetQueryInterfaceMessage qim;
	QueryInterfaceHandler qih( FvNetQueryInterfaceMessage::INTERNAL );

	//! TODO: 这个ep没有bind,可以使用吗?
	if (FV_NET_REASON_SUCCESS != qim.SendAndRecv( ep, ifaddr, m_uiMachinePort, &qih ))
	{
		FV_ERROR_MSG( "FvNetNub::QueryMachinedInterface: "
			"Failed to send interface discovery message to fvmachined.\n" );
		return false;
	}

	if (qih.m_bHasResponded)
	{
		addr = qih.m_iAddress;
		return true;
	}

	return false;
}

bool FvNetNub::RecreateListeningSocket( FvUInt16 listeningPort,
	const char * listeningInterface )
{
	if (m_kSocket.Good())
	{
		this->DeregisterWithMachined();

		this->DeregisterFileDescriptor( m_kSocket );
		m_kSocket.Close();
		m_kSocket.Detach();
	}


// 	FV_TRACE_MSG( "FvNetNub:RecreateListeningSocket: %s\n",
// 		listeningInterface ? listeningInterface : "NULL" );

	m_kAdvertisedAddress.m_uiIP = 0;
	m_kAdvertisedAddress.m_uiPort = 0;
	m_kAdvertisedAddress.m_uiSalt = 0;

	m_kSocket.Socket( SOCK_DGRAM );

	if (!m_kSocket.Good())
	{
		FV_ERROR_MSG( "FvNetNub::FvNetNub: couldn't create a socket\n" );
		return false;
	}

	this->RegisterFileDescriptor( m_kSocket, NULL );

	char ifname[IFNAMSIZ];
	u_int32_t ifaddr = INADDR_ANY;
	bool listeningInterfaceEmpty =
		(listeningInterface == NULL || listeningInterface[0] == 0);

	if (listeningInterface &&
		(strcmp(listeningInterface, USE_FVMACHINED) == 0))
	{
		FV_INFO_MSG( "FvNetNub::FvNetNub: Querying FvMachined for interface\n" );
		if (!this->QueryMachinedInterface( ifaddr ))
		{
			FV_WARNING_MSG( "FvNetNub::FvNetNub: No address received from machined so "
					"binding to all interfaces.\n" );
		}
	}
	else if (m_kSocket.FindIndicatedInterface( listeningInterface, ifname ) == 0)
	{
		FV_INFO_MSG( "FvNetNub::FvNetNub: creating on interface '%s' (= %s)\n",
			listeningInterface, ifname );
		if (m_kSocket.GetInterfaceAddress( ifname, ifaddr ) != 0)
		{
			FV_WARNING_MSG( "FvNetNub::FvNetNub: couldn't get addr of interface %s "
				"so using all interfaces\n", ifname );
		}
	}
	else if (!listeningInterfaceEmpty)
	{
		FV_WARNING_MSG( "FvNetNub::FvNetNub: couldn't parse interface spec '%s' "
			"so using all interfaces\n", listeningInterface );
	}

	if (m_kSocket.Bind( listeningPort, ifaddr ) != 0)
	{
		FV_ERROR_MSG( "FvNetNub::FvNetNub: couldn't bind the socket to %s (%s)\n",
			(char*)FvNetAddress( ifaddr, listeningPort ), strerror( errno ) );
		m_kSocket.Close();
		m_kSocket.Detach();
		return false;
	}

	m_kSocket.GetLocalAddress( (u_int16_t*)&m_kAdvertisedAddress.m_uiPort,
		(u_int32_t*)&m_kAdvertisedAddress.m_uiIP );

	if (m_kAdvertisedAddress.m_uiIP == 0)
	{
		if (m_kSocket.FindDefaultInterface( ifname ) != 0 ||
			m_kSocket.GetInterfaceAddress( ifname,
				(u_int32_t&)m_kAdvertisedAddress.m_uiIP ) != 0)
		{
			FV_ERROR_MSG( "FvNetNub::FvNetNub: "
				"couldn't determine ip addr of default interface\n" );

			m_kSocket.Close();
			m_kSocket.Detach();
			return false;
		}

		FV_INFO_MSG( "FvNetNub::FvNetNub: bound to all interfaces "
				"but advertising only %s ( %s )\n",
			ifname, (char*)m_kAdvertisedAddress );
	}

	FV_INFO_MSG( "Nub::recreateListeningSocket: Advertised address %s\n",
		(char*)m_kAdvertisedAddress );

	m_kSocket.SetNonblocking( true );

#if defined( unix ) && !defined( PLAYSTATION3 )
	int recverrs = true;
	setsockopt( m_kSocket, SOL_IP, IP_RECVERR, &recverrs, sizeof(int) );
#endif

#ifdef FV_SERVER
	if (!m_kSocket.SetBufferSize( SO_RCVBUF, RECV_BUFFER_SIZE ))
	{
		FV_WARNING_MSG( "Nub::Nub: Operating with a receive buffer of "
			"only %d bytes (instead of %d)\n",
			m_kSocket.GetBufferSize( SO_RCVBUF ), RECV_BUFFER_SIZE );
	}
	//! add by Uman, 20100406
	m_kSocket.SetBufferSize( SO_SNDBUF, RECV_BUFFER_SIZE );
	//! add end
#endif

	if (!m_kInterfaceName.empty())
	{
		this->RegisterWithMachined( m_kInterfaceName, m_iInterfaceID, m_uiMachinePort );
	}

	return true;
}


void FvNetNub::ServeInterfaceElement( const FvNetInterfaceElement & ie, FvNetMessageID id,
	FvNetInputMessageHandler * pHandler )
{
	FvNetInterfaceElement & element = m_kInterfaceTable[ id ];
	element	= ie;
	element.SetHandler( pHandler );
}


FvNetReason FvNetNub::RegisterBirthListener(FvNetBundle& kBundle, int iAddrStart,
											const char* pcIfName, FvUInt16 uiUserID)
{
	return this->RegisterListener(kBundle, iAddrStart, pcIfName, true, uiUserID);
}


FvNetReason FvNetNub::RegisterDeathListener(FvNetBundle& kBundle, int iAddrStart,
											const char* pcIfName, FvUInt16 uiUserID)
{
	return this->RegisterListener(kBundle, iAddrStart, pcIfName, false, uiUserID);
}


FvNetReason FvNetNub::RegisterBirthListener(const FvNetInterfaceElement& kIE,
											const char* pcIfName)
{
	FvNetBundle kBundle;

	kBundle.StartMessage( kIE, false );
	int iStartOfAddress = kBundle.Size();
	kBundle << InterfaceListenerMsg();

	return this->RegisterBirthListener( kBundle, iStartOfAddress, pcIfName, m_uiUserID );
}


FvNetReason FvNetNub::RegisterDeathListener(const FvNetInterfaceElement& kIE,
											const char* pcIfName)
{
	FvNetBundle kBundle;

	kBundle.StartMessage( kIE, false );
	int iStartOfAddress = kBundle.Size();
	kBundle << InterfaceListenerMsg();

	return this->RegisterDeathListener( kBundle, iStartOfAddress, pcIfName, m_uiUserID );
}

FvNetReason FvNetNub::RegisterBirthListener(const FvNetInterfaceElement& kIE,
											const char* pcIfName, FvUInt16 uiUserID)
{
	FvNetBundle kBundle;

	kBundle.StartMessage( kIE, false );
	int iStartOfAddress = kBundle.Size();
	kBundle << InterfaceListenerMsg();

	return this->RegisterBirthListener( kBundle, iStartOfAddress, pcIfName, uiUserID );
}


FvNetReason FvNetNub::RegisterDeathListener(const FvNetInterfaceElement& kIE,
											const char* pcIfName, FvUInt16 uiUserID)
{
	FvNetBundle kBundle;

	kBundle.StartMessage( kIE, false );
	int iStartOfAddress = kBundle.Size();
	kBundle << InterfaceListenerMsg();

	return this->RegisterDeathListener( kBundle, iStartOfAddress, pcIfName, uiUserID );
}

FvNetReason FvNetNub::RegisterListener(FvNetBundle& kBundle,
							 int iAddrStart, const char* pcIfName, bool bIsBirth, FvUInt16 uiUserID)
{
	kBundle.Finalise();
	const FvNetPacket* p = kBundle.m_spFirstPacket.Get();

	FV_ASSERT( p->GetFlags() == 0 );

	FvNetListenerMessage lm;
	lm.m_uiParam = (bIsBirth ? lm.ADD_BIRTH_LISTENER : lm.ADD_DEATH_LISTENER) |
		lm.PARAM_IS_MSGTYPE;
	lm.m_uiCategory = lm.SERVER_COMPONENT;
	lm.m_uiUID = uiUserID;
	lm.m_uiPID = FvGetPID();
	lm.m_uiPort = this->Address().m_uiPort;
	lm.m_kName = pcIfName ? pcIfName : "";

	unsigned int postSize = p->TotalSize() - iAddrStart - sizeof(InterfaceListenerMsg);

	lm.m_kPreAddr = FvString( p->Data(), iAddrStart );
	lm.m_kPostAddr = FvString( p->Data() + iAddrStart + sizeof(InterfaceListenerMsg), postSize );

	//FvUInt32 srcip = m_kAdvertisedAddress.m_uiIP, destip = FV_NET_LOCALHOST;
	FvUInt32 srcip = m_kAdvertisedAddress.m_uiIP, destip = m_kAdvertisedAddress.m_uiIP;//! modify by Uman, 20100613, win2008下127发不了
	return lm.SendAndRecv( srcip, destip, m_uiMachinePort, NULL );
}

namespace
{
class FindInterfaceHandler : public FvNetMachineGuardMessage::ReplyHandler
{
public:
	FindInterfaceHandler( FvNetAddress &address ) :
	  found_( false ), address_( address ) {}

	  virtual bool OnProcessStatsMessage( FvNetProcessStatsMessage &psm, FvUInt32 addr )
	  {
		  if (psm.m_uiPID != 0)
		  {
			  address_.m_uiIP = addr;
			  address_.m_uiPort = psm.m_uiPort;
			  address_.m_uiSalt = 0;
			  found_ = true;
			  FV_DEBUG_MSG( "Found interface %s at %s:%d\n",
				  psm.m_kName.c_str(), inet_ntoa( (in_addr&)addr ),
				  ntohs( address_.m_uiPort ) );
		  }
		  return true;
	  }

	  bool found_;
	  FvNetAddress &address_;
};

class FindInterfacesHandler : public FvNetMachineGuardMessage::ReplyHandler
{
public:
	FindInterfacesHandler( FvNetNub::InterfaceInfoVec& kInfos ):m_bFound(false), m_kInfos( kInfos ) {}

	virtual bool OnProcessStatsMessage( FvNetProcessStatsMessage &psm, FvUInt32 addr )
	{
		if(psm.m_uiPID != 0)
		{
			m_bFound = true;
			bool bAdd(false);
			FvNetAddress kAddr(addr, psm.m_uiPort);

			//! 检测重复性
			if(m_kInfos.empty())
			{
				bAdd = true;
				m_kInfos.push_back(FvNetNub::InterfaceInfo(psm.m_uiUID, psm.m_uiID, kAddr, psm.m_kName));
			}
			else
			{
				bool bExist(false);
				for(int i=0; i<(int)m_kInfos.size(); ++i)
				{
					if(m_kInfos[i].uiUserID == psm.m_uiUID &&
						m_kInfos[i].uiID == psm.m_uiID &&
						m_kInfos[i].kAddr == kAddr &&
						m_kInfos[i].kName == psm.m_kName)
					{
						bExist = true;
						break;
					}
				}

				if(!bExist)
				{
					bAdd = true;
					m_kInfos.push_back(FvNetNub::InterfaceInfo(psm.m_uiUID, psm.m_uiID, kAddr, psm.m_kName));
				}
			}

			if(bAdd)
			{
				FV_DEBUG_MSG( "Found Interface %s, UID:%d, ID:%d, at %s\n",
					psm.m_kName.c_str(), psm.m_uiUID, psm.m_uiID, kAddr.c_str() );
			}
		}
		return true;
	}

	bool m_bFound;
	FvNetNub::InterfaceInfoVec& m_kInfos;
};
}

FvNetReason FvNetNub::FindInterface( const char * name, int id,
		FvNetAddress & address, int retries, bool verboseRetry )
{
	FvNetProcessStatsMessage pm;
	pm.m_uiParam = pm.PARAM_USE_CATEGORY |
		pm.PARAM_USE_UID |
		pm.PARAM_USE_NAME |
		(id < 0 ? 0 : pm.PARAM_USE_ID);
	pm.m_uiCategory = pm.SERVER_COMPONENT;
	pm.m_uiUID = m_uiUserID;
	pm.m_kName = name;
	pm.m_uiID = id;

	int attempt = 0;
	FindInterfaceHandler handler( address );

	while (pm.SendAndRecv( 0, FV_NET_BROADCAST, m_uiMachinePort, &handler ) == FV_NET_REASON_SUCCESS)
	{
		if (handler.found_)
			return FV_NET_REASON_SUCCESS;

		if (verboseRetry)
		{
			FV_INFO_MSG( "FvNetNub::FindInterface: Failed to find %s on attempt %d.\n",
				name, attempt );
		}

		if (attempt++ >= retries)
			break;

#if defined( PLAYSTATION3 )
		sys_timer_sleep( 1 );
#elif !defined( _WIN32 )
		sleep( 1 );
#else
		Sleep( 1000 );
#endif
	}

	return FV_NET_REASON_TIMER_EXPIRED;
}

FvNetReason FvNetNub::FindInterfaces( FvUInt16 uiUserID, const char* pcInterfaceName, int iID, InterfaceInfoVec& kInfos,
						   int retries, bool verboseRetry )
{
	bool bAllInterface(false);
	if(!pcInterfaceName || !pcInterfaceName[0])
		bAllInterface = true;

	FvNetProcessStatsMessage pm;
	pm.m_uiParam = pm.PARAM_USE_CATEGORY |
		(uiUserID==0xFFFF ? 0 : pm.PARAM_USE_UID) |
		(bAllInterface ? 0 : pm.PARAM_USE_NAME) |
		(iID < 0 ? 0 : pm.PARAM_USE_ID);
	pm.m_uiCategory = pm.SERVER_COMPONENT;
	pm.m_uiUID = uiUserID;
	pm.m_kName = bAllInterface ? "" : pcInterfaceName;
	pm.m_uiID = iID;

	bool bFound(false);
	int attempt = 0;
	kInfos.clear();
	FindInterfacesHandler handler( kInfos );

	while(pm.SendAndRecv( 0, FV_NET_BROADCAST, m_uiMachinePort, &handler ) == FV_NET_REASON_SUCCESS)
	{
		if(!handler.m_bFound && verboseRetry)
		{
			FV_INFO_MSG( "FvNetNub::FindInterfaces: Failed to find %s on attempt %d.\n",
				pm.m_kName.c_str(), attempt );
		}

		bFound = bFound | handler.m_bFound;
		handler.m_bFound = false;

		if (attempt++ >= retries)
			break;

#if defined( PLAYSTATION3 )
		sys_timer_sleep( 1 );
#elif !defined( _WIN32 )
		sleep( 1 );
#else
		Sleep( 1000 );
#endif
	}

	if(bFound)
		return FV_NET_REASON_SUCCESS;
	else
		return FV_NET_REASON_TIMER_EXPIRED;
}

void FvNetNub::FinishProcessingTimerEvent( TimerQueueElement * pElement )
{
	m_pkCurrentTimer = NULL;

	if (pElement->m_iState == TimerQueueElement::STATE_CANCELLED)
	{
		// STATE_CANCELLED (=> deleted from timerMap_)
		delete pElement;
	}
	else
	{
		pElement->m_uiDeliveryTime += pElement->m_uiIntervalTime;
		pElement->m_iState = TimerQueueElement::STATE_PENDING;
		m_kTimerQueue.push( pElement );

		m_aiLoopStats[TIMER_RESCHEDS]++;
	}

	m_aiLoopStats[TIMER_CALLS]++;
}


bool FvNetNub::ProcessPendingEvents( bool expectingPacket )
{
	this->sendDelayedChannels();

	while ((!m_kTimerQueue.empty()) &&
		(m_kTimerQueue.top()->m_uiDeliveryTime <= Timestamp() ||
			m_kTimerQueue.top()->m_iState == TimerQueueElement::STATE_CANCELLED) &&
		!m_bDrainSocketInput)
	{
		TimerQueueElement * tqe = m_kTimerQueue.top();

		m_kTimerQueue.pop();
		if (tqe->m_iState != TimerQueueElement::STATE_CANCELLED)
		{
			tqe->m_iState = TimerQueueElement::STATE_EXECUTING;

			try
			{
				FV_ASSERT( m_pkCurrentTimer == NULL );
				m_pkCurrentTimer = tqe;

				tqe->handler->HandleTimeout( tqe, tqe->arg );
			}
			catch (...)
			{
				if (tqe->m_uiIntervalTime == 0)
					this->CancelTimer( tqe );

				this->FinishProcessingTimerEvent( tqe );

				throw;
			}

			if (tqe->m_uiIntervalTime == 0)
				this->CancelTimer( tqe );
		}

		this->FinishProcessingTimerEvent( tqe );
	}

	if (Timestamp() - m_uiLastStatisticsGathered >= StampsPerSecond())
	{
		m_kSocket.GetQueueSizes( m_iLastTxQueueSize, m_iLastRxQueueSize );

		if (m_iLastTxQueueSize > m_iMaxTxQueueSize && m_iLastTxQueueSize > 128*1024)
			FV_WARNING_MSG( "Transmit queue peaked at new max (%d bytes)\n",
				m_iLastTxQueueSize );
		if (m_iLastRxQueueSize > m_iMaxRxQueueSize && m_iLastRxQueueSize > 1024*1024)
			FV_WARNING_MSG( "Receive queue peaked at new max (%d bytes)\n",
				m_iLastRxQueueSize );

		m_iMaxTxQueueSize = std::max( m_iLastTxQueueSize, m_iMaxTxQueueSize );
		m_iMaxRxQueueSize = std::max( m_iLastRxQueueSize, m_iMaxRxQueueSize );

		m_uiOldSpareTime = m_uiTotSpareTime;
		m_uiTotSpareTime = m_uiAccSpareTime + m_uiSpareTime;

		m_uiLastStatisticsGathered = Timestamp();
	}

	m_kRecvMercuryTimer.start();
	m_kRecvSystemTimer.start();

	m_aiLoopStats[RECV_TRYS]++;

	FvNetAddress	srcAddr;
	int len = m_spNextPacket->RecvFromEndpoint( m_kSocket, srcAddr );

	m_kRecvSystemTimer.stop( len > 0 );

	if (len > 0)
	{
		m_aiLoopStats[RECV_GETS]++;

		m_uiNumPacketsReceived++;

		m_uiNumBytesReceived += len + FV_NET_UDP_OVERHEAD;
		m_uiNumOverheadBytesReceived += len + FV_NET_UDP_OVERHEAD;

		FvNetPacketPtr curPacket = m_spNextPacket;
		m_spNextPacket = new FvNetPacket();

		curPacket->MsgEndOffset( len );

		FvNetReason ret = this->ProcessPacket( srcAddr, curPacket.Get() );

		m_kRecvMercuryTimer.stop( true );

		if (ret != FV_NET_REASON_SUCCESS)
		{
			throw NubExceptionWithAddress( ret, srcAddr );
		}

		return true;
	}

	else
	{
		m_kRecvMercuryTimer.stop( false );

		if (len == 0 )
		{
			m_aiLoopStats[RECV_ERRS]++;

			FV_WARNING_MSG( "FvNetNub::ProcessPendingEvents: "
				"Throwing REASON_GENERAL_NETWORK (1)- %s\n",
				strerror( errno ) );

			throw FvNetNubException(FV_NET_REASON_GENERAL_NETWORK);
		}

#ifdef _WIN32
        DWORD wsaErr = WSAGetLastError();
#endif //def _WIN32

		if (
#ifdef _WIN32
			wsaErr == WSAEWOULDBLOCK
#else
			errno == EAGAIN && !expectingPacket
#endif
			)
		{
			return false;
		}

		m_aiLoopStats[RECV_ERRS]++;

#ifdef unix
		if (errno == EAGAIN ||
			errno == ECONNREFUSED ||
			errno == EHOSTUNREACH)
		{
#if defined( PLAYSTATION3 )
			throw FvNetNubException( FV_NET_REASON_NO_SUCH_PORT );
#else
			FvNetAddress offender;

			if (m_kSocket.GetClosedPort( offender ))
			{
				if (errno == ECONNREFUSED)
				{
					FvNetChannel * pDeadChannel = this->FindChannel( offender );

					if (pDeadChannel && pDeadChannel->IsInternal())
					{
						FV_INFO_MSG( "FvNetNub::ProcessPendingEvents: "
							"Marking channel to %s as dead (%s)\n",
							pDeadChannel->c_str(),
							NetReasonToString( FV_NET_REASON_NO_SUCH_PORT ) );

						pDeadChannel->HasRemoteFailed( true );
					}
				}

				throw NubExceptionWithAddress( FV_NET_REASON_NO_SUCH_PORT, offender );
			}
			else
			{
				FV_WARNING_MSG( "FvNetNub::ProcessPendingEvents: "
					"getClosedPort() failed\n" );
			}
#endif
		}
#else
        if (wsaErr == WSAECONNRESET)
        {
            return true;
			// throw NubException( REASON_NO_SUCH_PORT );
        }
#endif // unix

#ifdef _WIN32
		FV_WARNING_MSG( "FvNetNub::ProcessPendingEvents: "
					"Throwing REASON_GENERAL_NETWORK - %d\n",
					wsaErr );
#else
		FV_WARNING_MSG( "FvNetNub::ProcessPendingEvents: "
					"Throwing REASON_GENERAL_NETWORK - %s\n",
				strerror( errno ) );
#endif
		throw FvNetNubException(FV_NET_REASON_GENERAL_NETWORK);
	}
}


void FvNetNub::ProcessContinuously()
{
	fd_set	readFDs;
	fd_set	writeFDs;
	struct timeval	nextTimeout;
	struct timeval	*selectArg;

	m_bBreakProcessing = false;
	FD_ZERO( &readFDs );
	FD_ZERO( &writeFDs );

	bool expectPacket = false;

	while (!m_bBreakProcessing)
	{
		if (m_pkOpportunisticPoller)
			m_pkOpportunisticPoller->Poll();

		bool 	gotPacket;

		do
		{
			gotPacket = this->ProcessPendingEvents( expectPacket );
			expectPacket = false;

			if (m_bDrainSocketInput && !gotPacket)
			{
				m_bDrainSocketInput = false;
				gotPacket = true;
			}
		}
		while (gotPacket && !m_bBreakProcessing);

		if (m_bBreakProcessing)
		{
			break;
		}

		BeginThreadBlockingOperation();

		FvUInt64		startSelect = Timestamp();

		readFDs = m_kFDReadSet;
		writeFDs = m_kFDWriteSet;
		if (m_kTimerQueue.empty())
		{
			selectArg = NULL;
		}
		else
		{
			double maxWait = 0.0;
			FvUInt64 topTime = m_kTimerQueue.top()->m_uiDeliveryTime;

			if (topTime > startSelect)
			{
#ifdef _WIN32
				maxWait = (double)(FvInt64)(topTime - startSelect);
#else
				maxWait = topTime - startSelect;
#endif
				maxWait /= StampsPerSecondD();
			}

			FV_ASSERT( 0.0 <= maxWait && maxWait <= 36000.0);

			nextTimeout.tv_sec = (int)maxWait;
			nextTimeout.tv_usec =
				(int)( (maxWait - (double)nextTimeout.tv_sec) * 1000000.0 );

			selectArg = &nextTimeout;
		}

		int countReady = select( m_iFDLargest+1, &readFDs,
				m_iFDWriteCount ? &writeFDs : NULL, NULL, selectArg );

		FvUInt64 endofSelect = Timestamp();
		m_uiSpareTime += endofSelect - startSelect;
		m_aiLoopStats[RECV_SELS]++;

		CeaseThreadBlockingOperation();

		if (countReady > 0)
		{
			if (FD_ISSET( m_kSocket, &readFDs ))
			{
				expectPacket = true;
			}
			else
			{
				this->HandleInputNotifications( countReady, readFDs, writeFDs );
			}
		}

		else if (countReady == -1)
		{
			if (!m_bBreakProcessing)
			{
#ifdef _WIN32
				FV_WARNING_MSG( "FvNetNub::ProcessContinuously: "
					"error in select(): id(%d)= %s\n", WSAGetLastError(), "" );
#else
				FV_WARNING_MSG( "FvNetNub::ProcessContinuously: "
					"error in select(): id(%d)= %s\n", errno, strerror( errno ) );
#endif
			}
		}
	}
}


void FvNetNub::ProcessUntilBreak()
{
	for (;;)
	{
		try
		{
			this->ProcessContinuously();
			break;
		}
		catch (FvNetNubException & ne)
		{
			this->ReportException( ne );

			if (m_bBreakProcessing)
			{
				break;
			}
		}
	}
	this->ReportPendingExceptions( true /* reportBelowThreshold */ );
}


void FvNetNub::ProcessUntilChannelsEmpty( float timeout )
{
	bool done = false;
	FvUInt64 startTime = Timestamp();
	FvUInt64 endTime = startTime + FvUInt64( timeout * StampsPerSecondD() );

	while (!done && (Timestamp() < endTime))
	{
		try
		{
			while (this->ProcessPendingEvents())
			{
				;
			}
		}
		catch (FvNetNubException & ne)
		{
			this->ReportException( ne );
		}

		bool haveAnyUnackedPackets = false;
		ChannelMap::iterator iter = m_kChannelMap.begin();
		while (iter != m_kChannelMap.end() && !haveAnyUnackedPackets)
		{
			FvNetChannel & channel = *iter->second;

			if (channel.HasUnackedPackets())
			{
				haveAnyUnackedPackets = true;
			}

			++iter;
		}

		done = !haveAnyUnackedPackets;

		if (!m_kCondemnedChannels.DeleteFinishedChannels())
		{
			done = false;
		}

		// Wait 100ms
#if defined( PLAYSTATION3 )
		sys_timer_usleep( 100000 );
#elif !defined( _WIN32 )
		usleep( 100000 );
#else
		Sleep( 100 );
#endif
	}

	this->ReportPendingExceptions( true /* reportBelowThreshold */ );

	if (!done)
	{
		FV_WARNING_MSG( "Nub::processUntilChannelsEmpty: "
			"Timed out after %.1fs, unacked packets may have been lost\n",
			timeout );
	}
}

void FvNetNub::ProcessFrame()
{
	fd_set	readFDs;
	fd_set	writeFDs;
	struct timeval	nextTimeout;
	struct timeval	*selectArg;

	FD_ZERO( &readFDs );
	FD_ZERO( &writeFDs );

	bool expectPacket = false;

	if (m_pkOpportunisticPoller)
		m_pkOpportunisticPoller->Poll();

	bool gotPacket;

	do
	{
		gotPacket = this->ProcessPendingEvents( expectPacket );
		expectPacket = false;

		if (m_bDrainSocketInput && !gotPacket)
		{
			m_bDrainSocketInput = false;
			gotPacket = true;
		}
	}
	while (gotPacket);

	BeginThreadBlockingOperation();

	FvUInt64 startSelect = Timestamp();

	readFDs = m_kFDReadSet;
	writeFDs = m_kFDWriteSet;
	if (m_kTimerQueue.empty())
	{
		selectArg = NULL;
	}
	else
	{
		double maxWait = 0.0;
		FvUInt64 topTime = m_kTimerQueue.top()->m_uiDeliveryTime;

		if (topTime > startSelect)
		{
#ifdef _WIN32
			maxWait = (double)(FvInt64)(topTime - startSelect);
#else
			maxWait = topTime - startSelect;
#endif
			maxWait /= StampsPerSecondD();
		}

		FV_ASSERT( 0.0 <= maxWait && maxWait <= 36000.0);

		nextTimeout.tv_sec = (int)maxWait;
		nextTimeout.tv_usec =
			(int)( (maxWait - (double)nextTimeout.tv_sec) * 1000000.0 );

		selectArg = &nextTimeout;
	}

	int countReady = select( m_iFDLargest+1, &readFDs,
		m_iFDWriteCount ? &writeFDs : NULL, NULL, selectArg );

	FvUInt64 endofSelect = Timestamp();
	m_uiSpareTime += endofSelect - startSelect;
	m_aiLoopStats[RECV_SELS]++;

	CeaseThreadBlockingOperation();

	if (countReady > 0)
	{
		if (FD_ISSET( m_kSocket, &readFDs ))
		{
			expectPacket = true;
		}
		else
		{
			this->HandleInputNotifications( countReady, readFDs, writeFDs );
		}
	}

	else if (countReady == -1)
	{
		if (!m_bBreakProcessing)
		{
			FV_WARNING_MSG( "FvNetNub::ProcessContinuously: "
				"error in select(): id(%d)= %s\n", WSAGetLastError(), "" );
		}
	}
}

void FvNetNub::HandleInputNotifications( int &countReady,
	fd_set &readFDs, fd_set &writeFDs )
{
#ifdef _WIN32
	for (unsigned i=0; i < readFDs.fd_count; i++)
	{
		int fd = readFDs.fd_array[ i ];
		--countReady;
		FvNetInputNotificationHandler * pHandler = m_apFDHandlers[ fd ];
		if (pHandler)
			pHandler->HandleInputNotification( fd );
	}

	for (unsigned i=0; i < writeFDs.fd_count; i++)
	{
		int fd = writeFDs.fd_array[ i ];
		--countReady;
		FvNetInputNotificationHandler * pHandler = m_apFDWriteHandlers[ fd ];
		if (pHandler)
			pHandler->HandleInputNotification( fd );
	}

#else

	for (int fd = 0; fd <= m_iFDLargest && countReady > 0; ++fd)
	{
		if (FD_ISSET( fd, &readFDs ))
		{
			--countReady;
			FvNetInputNotificationHandler * pHandler = m_apFDHandlers[ fd ];
			if (pHandler)
				pHandler->HandleInputNotification( fd );
		}
		if (FD_ISSET( fd, &writeFDs ))
		{
			--countReady;
			FvNetInputNotificationHandler * pHandler = m_apFDWriteHandlers[ fd ];
			if (pHandler)
				pHandler->HandleInputNotification( fd );
		}
	}
#endif
}


int FvNetNub::HandleInputNotification( int fd )
{
	this->ProcessPendingEvents( /* expectingPacket: */ true );
	return 0;
}


void FvNetNub::BreakProcessing( bool breakState )
{
	m_bBreakProcessing = breakState;
}


bool FvNetNub::ProcessingBroken() const
{
	return m_bBreakProcessing;
}


void FvNetNub::BreakBundleLoop()
{
	m_bBreakBundleLoop = true;
}

void FvNetNub::DrainSocketInput()
{
	m_bDrainSocketInput = true;
}


void FvNetNub::Shutdown()
{
    this->BreakProcessing();
    m_kSocket.Close();
}


const FvNetAddress & FvNetNub::Address() const
{
	return m_kAdvertisedAddress;
}


bool FvNetNub::RegisterChannel( FvNetChannel & channel )
{
	FV_ASSERT( channel.addr() != FvNetAddress::NONE );
	FV_ASSERT( &channel.nub() == this );

	ChannelMap::iterator iter = m_kChannelMap.find( channel.addr() );
	FvNetChannel * pExisting = iter != m_kChannelMap.end() ? iter->second : NULL;

	FV_IF_NOT_ASSERT_DEV( !pExisting )
	{
		return false;
	}

	m_kChannelMap[ channel.addr() ] = &channel;

	return true;
}


bool FvNetNub::DeregisterChannel( FvNetChannel & channel )
{
	const FvNetAddress & addr = channel.addr();
	FV_ASSERT( addr != FvNetAddress::NONE );

	if (!m_kChannelMap.erase( addr ))
	{
		FV_CRITICAL_MSG( "Nub::deregisterChannel: Channel not found %s!\n",
			   (char *)addr );
		return false;
	}

	if (m_bIsExternal)
	{
		FvNetTimerID timeoutID = this->RegisterCallback( 60 * 1000000, this,
			(void*)TIMEOUT_RECENTLY_DEAD_CHANNEL );

		m_kRecentlyDeadChannels[ addr ] = timeoutID;
	}

	return true;
}


void FvNetNub::RegisterChannelFinder( FvNetChannelFinder *pFinder )
{
	FV_ASSERT( m_pkChannelFinder == NULL );
	m_pkChannelFinder = pFinder;
}


FvNetChannel * FvNetNub::FindChannel( const FvNetAddress & addr, bool createAnonymous )
{
	ChannelMap::iterator iter = m_kChannelMap.find( addr );
	FvNetChannel * pChannel = iter != m_kChannelMap.end() ? iter->second : NULL;

	FV_ASSERT( !pChannel || pChannel->id() == FV_NET_CHANNEL_ID_NULL );

	if (!pChannel && createAnonymous)
	{
		FV_ASSERT( !m_bIsExternal );

		FV_INFO_MSG( "FvNetNub::FindChannel: "
			"Creating anonymous channel to %s\n",
			addr.c_str() );

		pChannel = new FvNetChannel( *this, addr, FvNetChannel::INTERNAL );
		pChannel->IsAnonymous( true );
	}

	return pChannel;
}


void FvNetNub::DelAnonymousChannel( const FvNetAddress & addr )
{
	ChannelMap::iterator iter = m_kChannelMap.find( addr );

	if (iter != m_kChannelMap.end())
	{
		if (iter->second->IsAnonymous())
		{
			iter->second->Condemn();
		}
		else
		{
			FV_ERROR_MSG( "FvNetNub::DelAnonymousChannel: "
				"Channel to %s is not anonymous!\n",
				addr.c_str() );
		}
	}
	else
	{
		FV_ERROR_MSG( "FvNetNub::DelAnonymousChannel: "
			"Couldn't find channel for address %s\n",
			addr.c_str() );
	}
}


FV_INLINE FvNetSeqNum FvNetNub::GetNextSequenceID()
{
	FvNetSeqNum ret = m_uiNextSequenceID;
	m_uiNextSequenceID = FvNetChannel::SeqMask( m_uiNextSequenceID + 1 );
	return ret;
}


void FvNetNub::Send( const FvNetAddress & address, FvNetBundle & bundle, FvNetChannel * pChannel )
{
	FV_ASSERT( address != FvNetAddress::NONE );
	FV_ASSERT( !pChannel || pChannel->addr() == address );

	FV_ASSERT( !bundle.pChannel() || (bundle.pChannel() == pChannel) );

	m_kSendMercuryTimer.start();

	bundle.Finalise();

	for (FvNetBundle::ReplyOrders::iterator order = bundle.m_kReplyOrders.begin();
		order != bundle.m_kReplyOrders.end();
		order++)
	{
		ReplyHandlerElement * pRHE = new ReplyHandlerElement;

		int replyID = this->GetNextReplyID();
		pRHE->m_iReplyID = replyID;
		pRHE->m_kTimerID = FV_NET_TIMER_ID_NONE;
		pRHE->m_pkHandler = order->handler;
		pRHE->m_pkArg = order->arg;
		pRHE->m_pkChannel = pChannel;

		m_kReplyHandlerMap[ replyID ] = pRHE;

		*(order->pReplyID) = FV_HTONL( replyID );

		if (!pChannel)
		{
 			FV_ASSERT( order->microseconds > 0 );
			pRHE->m_kTimerID =
				this->RegisterCallback( order->microseconds, pRHE, this );
		}
	}

	FvNetPacket * pFirstOverflowPacket = NULL;

	int	numPackets = bundle.SizeInPackets();
	FvNetSeqNum firstSeq = 0;
	FvNetSeqNum lastSeq = 0;
	FvNetBundle::AckOrders::iterator ackIter = bundle.m_kAckOrders.begin();

	for (FvNetPacket * p = bundle.m_spFirstPacket.Get(); p; p = p->Next())
	{
		FV_ASSERT( p->MsgEndOffset() >= FvNetPacket::HEADER_SIZE );

		if (m_bShouldUseChecksums)
		{
			FV_ASSERT( !p->HasFlags( FvNetPacket::FLAG_HAS_CHECKSUM ) );
			p->ReserveFooter( sizeof( FvNetPacket::Checksum ) );
			p->EnableFlags( FvNetPacket::FLAG_HAS_CHECKSUM );
		}

		FV_ASSERT( !p->HasFlags( FvNetPacket::FLAG_ON_CHANNEL ) );
		if (pChannel)
		{
			p->EnableFlags( FvNetPacket::FLAG_ON_CHANNEL );
		}

		const int msgEndOffset = p->MsgEndOffset();
		p->Grow( p->FooterSize() );

		FvNetPacket::Checksum * pChecksum = NULL;

		if (p->HasFlags( FvNetPacket::FLAG_HAS_CHECKSUM ))
		{
			p->PackFooter( FvNetPacket::Checksum( 0 ) );
			pChecksum = (FvNetPacket::Checksum*)p->Back();
		}

		if (p->HasFlags( FvNetPacket::FLAG_HAS_PIGGYBACKS ))
		{
			FV_ASSERT( p->Next() == NULL );

			FvInt16 * lastLen = NULL;

			int backPiggyOffset = p->MsgEndOffset();

			for (FvNetBundle::Piggybacks::const_iterator it =
					bundle.m_kPiggybacks.begin();
				 it != bundle.m_kPiggybacks.end(); ++it)
			{
				const FvNetBundle::Piggyback &pb = **it;

				p->PackFooter( pb.m_iLen );
				lastLen = (FvInt16*)p->Back();

				p->Shrink( pb.m_iLen );
				char * pdata = p->Back();

				*(FvNetPacket::Flags*)pdata = FV_HTONS( pb.m_uiFlags );
				pdata += sizeof( FvNetPacket::Flags );

				for (FvReliableVector::const_iterator rvit = pb.m_kRVec.begin();
					 rvit != pb.m_kRVec.end(); ++rvit)
				{
					memcpy( pdata, rvit->m_uiSegBegin, rvit->m_uiSegLength );
					pdata += rvit->m_uiSegLength;
				}

				*(FvNetSeqNum*)pdata = FV_HTONL( pb.m_uiSeq );
				pdata += sizeof( FvNetSeqNum );

				if (pb.m_uiFlags & FvNetPacket::FLAG_HAS_PIGGYBACKS)
				{
					const FvNetDataField & subPiggies = pb.m_spPacket->PiggyFooters();
					memcpy( pdata, subPiggies.m_pcBeg, subPiggies.m_uiLength );
					pdata += subPiggies.m_uiLength;
				}

				FV_ASSERT( pdata == (char*)lastLen );

				++m_uiNumPiggybacks;

				if (m_bIsVerbose)
				{
					FV_DEBUG_MSG( "Nub::send( %s ): "
						"Piggybacked #%u (%d bytes) onto outgoing bundle\n",
						address.c_str(), pb.m_uiSeq, pb.m_iLen );
				}
			}

			*lastLen = FV_HTONS( ~FV_NTOHS( *lastLen ) );

			p->PiggyFooters().m_pcBeg = p->Back();
			p->PiggyFooters().m_uiLength =
				FvUInt16( backPiggyOffset - p->MsgEndOffset() );
		}

		if (p->HasFlags( FvNetPacket::FLAG_INDEXED_CHANNEL ))
		{
			FV_ASSERT( pChannel && pChannel->IsIndexed() );

			p->GetChannelID() = pChannel->id();
			p->PackFooter( p->GetChannelID() );

			p->GetChannelVersion() = pChannel->version();
			p->PackFooter( p->GetChannelVersion() );
		}

		if (p->HasFlags( FvNetPacket::FLAG_HAS_ACKS ))
		{
			p->PackFooter( (FvUInt8)p->GetNAcks() );

			int numAcks = 0;
			while (ackIter != bundle.m_kAckOrders.end() && ackIter->p == p)
			{
				p->PackFooter( ackIter->forseq );
				++ackIter;
				++numAcks;
			}

			FV_ASSERT( numAcks <= FvNetPacket::MAX_ACKS );
			FV_ASSERT( numAcks == p->GetNAcks() );
		}

		if (p->HasFlags( FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER ))
		{
			p->GetSeq() = (pChannel && p->HasFlags( FvNetPacket::FLAG_IS_RELIABLE )) ?
				pChannel->UseNextSequenceID() : this->GetNextSequenceID();

			p->PackFooter( p->GetSeq() );

			if (p == bundle.m_spFirstPacket)
			{
				firstSeq = p->GetSeq();
				lastSeq = p->GetSeq() + numPackets - 1;
			}
		}

		if (p->HasFlags( FvNetPacket::FLAG_HAS_REQUESTS ))
		{
			p->PackFooter( p->FirstRequestOffset() );
		}

		if (p->HasFlags( FvNetPacket::FLAG_IS_FRAGMENT ))
		{
			p->PackFooter( lastSeq );
			p->PackFooter( firstSeq );
		}

		FV_ASSERT( p->MsgEndOffset() == msgEndOffset );

		if (p->HasFlags( FvNetPacket::FLAG_HAS_CHECKSUM ))
		{
			FvNetPacket::Checksum sum = 0;
			for (FvNetPacket::Checksum * pData = (FvNetPacket::Checksum *)p->Data();
				 pData < pChecksum; pData++)
			{
				sum ^= FV_NTOHL( *pData );
			}

			*pChecksum = FV_HTONL( sum );
		}

		if (p->HasFlags( FvNetPacket::FLAG_IS_RELIABLE ))
		{
			if (pChannel)
			{
				const FvNetReliableOrder *roBeg, *roEnd;

				if (pChannel->IsInternal())
				{
					roBeg = roEnd = NULL;
				}
				else
				{
					bundle.ReliableOrders( p, roBeg, roEnd );
				}

				if (!pChannel->AddResendTimer( p->GetSeq(), p, roBeg, roEnd ))
				{
					if (pFirstOverflowPacket == NULL)
					{
						pFirstOverflowPacket = p;
					}
					// return REASON_WINDOW_OVERFLOW;
				}
				else
				{
					FV_ASSERT( pFirstOverflowPacket == NULL );
				}
			}
			else
			{
				this->AddOnceOffResendTimer( address, p->GetSeq(), p );
			}
		}
	}

	for (FvNetPacket * p = bundle.m_spFirstPacket.Get();
			p != pFirstOverflowPacket;
			p = p->Next())
	{
		this->SendPacket( address, p, pChannel, false );
	}

	m_kSendMercuryTimer.stop(true);
	m_uiNumBundlesSent++;
	m_uiNumMessagesSent += bundle.NumMessages();

	m_uiNumReliableMessagesSent += bundle.m_kReliableOrders.size();
}


void FvNetNub::SendPacket( const FvNetAddress & address,
						FvNetPacket * p,
						FvNetChannel * pChannel, bool isResend )
{
	if (isResend)
	{
		m_uiNumBytesResent += p->TotalSize();
		++m_uiNumPacketsResent;
	}
	else
	{
		if (!p->HasFlags( FvNetPacket::FLAG_ON_CHANNEL ))
		{
			++m_uiNumPacketsSentOffChannel;
		}
	}

	if (!this->RescheduleSend( address, p, /* isResend: */ false ))
	{
		FvNetPacketFilterPtr pFilter = pChannel ? pChannel->pFilter() : NULL;

		if(m_pkPacketMonitor)
		{
			m_pkPacketMonitor->PacketOut( address, *p );
		}

		if (pFilter)
		{
			pFilter->Send( *this, address, p );
		}
		else
		{
			this->BasicSendWithRetries( address, p );
		}
	}
}


FvNetReason FvNetNub::BasicSendWithRetries( const FvNetAddress & addr, FvNetPacket * p )
{
	int retries = 0;
	FvNetReason reason;

	while (1)
	{
		retries++;

		m_kSendSystemTimer.start();
		reason = this->BasicSendSingleTry( addr, p );
		m_kSendSystemTimer.stop(true);

		if (reason == FV_NET_REASON_SUCCESS)
			return reason;

		if (reason == FV_NET_REASON_NO_SUCH_PORT && retries <= 3)
		{
			continue;
		}

		if (reason == FV_NET_REASON_RESOURCE_UNAVAILABLE && retries <= 3)
		{
			fd_set	fds;
			struct timeval tv = { 0, 10000 };
			FD_ZERO(&fds);
			FD_SET(m_kSocket,&fds);

			FV_WARNING_MSG( "FvNetNub::Send: "
				"Transmit queue full, waiting for space... (%d)\n",
				retries );

			m_kSendSystemTimer.start();
			select(m_kSocket+1,NULL,&fds,NULL,&tv);
			m_kSendSystemTimer.stop(true);

			continue;
		}

		break;
	}

	FvNetAddress badAddress;

	while (m_kSocket.GetClosedPort( badAddress ))
	{
		FV_ERROR_MSG( "FvNetNub::Send: Bad address is %s (discarding)\n",
				badAddress.c_str() );
	}

	return reason;
}


FvNetReason FvNetNub::BasicSendSingleTry( const FvNetAddress & addr, FvNetPacket * p )
{
	int len = m_kSocket.SendTo( p->Data(), p->TotalSize(), addr.m_uiPort, addr.m_uiIP );

	if (len == p->TotalSize())
	{
		m_uiNumBytesSent += len + FV_NET_UDP_OVERHEAD;
		m_uiNumPacketsSent++;

		return FV_NET_REASON_SUCCESS;
	}
	else
	{
		int err;
		FvNetReason reason;

		m_uiNumFailedPacketSend++;

		#ifdef unix
			err = errno;

			switch (err)
			{
				case ECONNREFUSED:	reason = FV_NET_REASON_NO_SUCH_PORT; break;
				case EAGAIN:		reason = FV_NET_REASON_RESOURCE_UNAVAILABLE; break;
				case ENOBUFS:		reason = FV_NET_REASON_TRANSMIT_QUEUE_FULL; break;
				default:			reason = FV_NET_REASON_GENERAL_NETWORK; break;
			}
		#else
			err = WSAGetLastError();

			if (err == WSAEWOULDBLOCK || err == WSAEINTR)
			{
				reason = FV_NET_REASON_RESOURCE_UNAVAILABLE;
			}
			else
			{
				reason = FV_NET_REASON_GENERAL_NETWORK;
			}
		#endif

		if (len == -1)
		{
			if (reason != FV_NET_REASON_NO_SUCH_PORT)
			{
				this->ReportError( addr,
					"FvNetNub::BasicSendSingleTry( %s ): could not send packet: %s",
					addr.c_str(), strerror( err ) );
			}
		}
		else
		{
			FV_WARNING_MSG( "FvNetNub::BasicSendSingleTry( %s ): "
				"packet length %d does not match sent length %d (err = %s)\n",
				(char*)addr, p->TotalSize(), len, strerror( err ) );
		}

		return reason;
	}
}

bool FvNetNub::RescheduleSend( const FvNetAddress & addr, FvNetPacket * packet, bool isResend )
{
	if (m_bDropNextSend ||
		((m_iArtificialDropPerMillion != 0) &&
		rand() < FvInt64( m_iArtificialDropPerMillion ) * RAND_MAX / 1000000))
	{
		if (packet->GetSeq() != FvNetChannel::SEQ_NULL)
		{
			if (packet->GetChannelID() != FV_NET_CHANNEL_ID_NULL)
			{
				FV_DEBUG_MSG( "FvNetNub::RescheduleSend: "
					"dropped packet #%u to %s/%d due to artificial loss\n",
					packet->GetSeq(), addr.c_str(), packet->GetChannelID() );
			}
			else
			{
				FV_DEBUG_MSG( "FvNetNub::RescheduleSend: "
					"dropped packet #%u to %s due to artificial loss\n",
					packet->GetSeq(), addr.c_str() );
			}
		}
		else
		{
			FV_ASSERT( m_bIsExternal ||
				packet->MsgEndOffset() == FvNetPacket::HEADER_SIZE );

			FV_DEBUG_MSG( "FvNetNub::RescheduleSend: "
				"Dropped packet with %d ACKs to %s due to artificial loss\n",
				packet->GetNAcks(), addr.c_str() );
		}

		m_bDropNextSend = false;

		return true;
	}

	if (m_iArtificialLatencyMax == 0)
		return false;

	int latency = (m_iArtificialLatencyMax > m_iArtificialLatencyMin) ?
		m_iArtificialLatencyMin +
				rand() % (m_iArtificialLatencyMax - m_iArtificialLatencyMin) :
		m_iArtificialLatencyMin;

	if (latency < 2)
		return false;	// 2ms => send now

	new RescheduledSender( *this, addr, packet, latency, isResend );

	return true;
}


FvNetNub::RescheduledSender::RescheduledSender( FvNetNub & nub, const FvNetAddress & addr,
	FvNetPacket * pPacket, int latencyMilli, bool isResend ) :
	m_kNub( nub ),
	m_kAddr( addr ),
	m_spPacket( pPacket ),
	m_bIsResend( isResend )
{
	m_kNub.RegisterCallback( latencyMilli*1000, this );
}


int FvNetNub::RescheduledSender::HandleTimeout( FvNetTimerID, void * )
{
	FvNetChannel * pChannel = m_spPacket->HasFlags( FvNetPacket::FLAG_ON_CHANNEL ) ?
		m_kNub.FindChannel( m_kAddr ) : NULL;

	if (m_bIsResend)
	{
		m_kNub.SendPacket( m_kAddr, m_spPacket.Get(), pChannel, true );
	}
	else
	{
		FvNetPacketFilterPtr pFilter = pChannel ? pChannel->pFilter() : NULL;

		if(m_kNub.m_pkPacketMonitor)
		{
			m_kNub.m_pkPacketMonitor->PacketOut( m_kAddr, *m_spPacket );
		}

		if (pFilter)
		{
			pFilter->Send( m_kNub, m_kAddr, m_spPacket.Get() );
		}
		else
		{
			m_kNub.BasicSendWithRetries( m_kAddr, m_spPacket.Get() );
		}

	}

	delete this;

	return 0;
}


void FvNetNub::DelayedSend( FvNetChannel * pChannel )
{
	m_kDelayedChannels.insert( pChannel );
}


void FvNetNub::sendDelayedChannels()
{
	DelayedChannels::iterator iter = m_kDelayedChannels.begin();

	while (iter != m_kDelayedChannels.end())
	{
		FvNetChannel * pChannel = iter->Get();

		if (!pChannel->IsDead())
		{
			pChannel->Send();
		}

		++iter;
	}

	m_kDelayedChannels.clear();
}


void FvNetNub::OnAddressDead( const FvNetAddress & addr )
{
	OnceOffPackets::iterator iter = m_kOnceOffPackets.begin();
	int numRemoved = 0;

	while (iter != m_kOnceOffPackets.end())
	{
		if (iter->m_kAddr == addr)
		{
			OnceOffPackets::iterator deadIter = iter++;
			this->DelOnceOffResendTimer( deadIter );
			numRemoved++;
		}
		else
		{
			++iter;
		}
	}

	if (numRemoved)
	{
		FV_WARNING_MSG( "FvNetNub::OnAddressDead( %s ): "
			"Discarded %d unacked once-off sends\n",
			addr.c_str(), numRemoved );
	}

	FvNetChannel * pDeadChannel = this->FindChannel( addr );

	if (pDeadChannel && pDeadChannel->IsAnonymous())
	{
		pDeadChannel->HasRemoteFailed( true );
	}
}


bool FvNetNub::ProcessMessageHandler::OnProcessMessage(
		FvNetProcessMessage &pm, FvUInt32 addr )
{
	m_bHasResponded = true;
	return false;
}

class ProcessMessageHandler;


FvNetReason FvNetNub::RegisterWithMachined( const FvString & name, int id,
	bool isRegister )
{
	FvNetProcessMessage pm;
	ProcessMessageHandler pmh;
	FvNetReason response;

	pm.m_uiParam = (isRegister ? pm.REGISTER : pm.DEREGISTER) |
		pm.PARAM_IS_MSGTYPE;
	pm.m_uiCategory = FvNetProcessMessage::SERVER_COMPONENT;
	pm.m_uiUID = m_uiUserID;
	pm.m_uiPID = FvGetPID();
	pm.m_uiPort = this->Address().m_uiPort;
	pm.m_kName = name;
	pm.m_uiID = id;

	//FvUInt32 src = m_kAdvertisedAddress.m_uiIP, dest = htonl( 0x7F000001U );
	FvUInt32 src = m_kAdvertisedAddress.m_uiIP, dest = m_kAdvertisedAddress.m_uiIP;//! modify by Uman, 20100613, win2008下127发不了
	response = pm.SendAndRecv( src, dest, m_uiMachinePort, &pmh );

	if (response == FV_NET_REASON_SUCCESS)
	{
		if (pmh.m_bHasResponded)
		{
			m_kInterfaceName = name;
			m_iInterfaceID = id;
		}
		else
		{
			response = FV_NET_REASON_TIMER_EXPIRED;
		}
	}

	return response;
}


FvNetReason FvNetNub::DeregisterWithMachined()
{
	if (!m_kInterfaceName.empty())
	{
		return this->RegisterWithMachined(
			m_kInterfaceName, m_iInterfaceID, false );
	}
	else
	{
		return FV_NET_REASON_SUCCESS;
	}
}


FvNetReason FvNetNub::ProcessPacket( const FvNetAddress & addr, FvNetPacket * p )
{
	//! add by Uman, 20100415, 这里可以优化,idx channel不需要查channel
	FvNetChannel * pChannel = this->FindChannel( addr,
			!m_bIsExternal && p->ShouldCreateAnonymous() );

	if (pChannel != NULL)
	{
		pChannel->OnPacketReceived( p->TotalSize() );

		if (pChannel->pFilter() && !pChannel->HasRemoteFailed())
		{
			return pChannel->pFilter()->Recv( *this, addr, p );
		}
	}

	if (m_bIsExternal &&
		m_bIsVerbose &&
		(m_kRecentlyDeadChannels.find( addr ) != m_kRecentlyDeadChannels.end()))
	{
		FV_DEBUG_MSG( "FvNetNub::ProcessPacket( %s ): "
			"Ignoring incoming packet on recently dead channel\n",
			addr.c_str() );

		return FV_NET_REASON_SUCCESS;
	}

	return this->ProcessFilteredPacket( addr, p );
}


#define RETURN_FOR_CORRUPTED_PACKET()										\
	++m_uiNumCorruptedPacketsReceived;											\
	return FV_NET_REASON_CORRUPTED_PACKET;											\


FvNetReason FvNetNub::ProcessFilteredPacket( const FvNetAddress & addr, FvNetPacket * p )
{
	if (p->TotalSize() < int( sizeof( FvNetPacket::Flags ) ))
	{
		if (!m_bIsExternal)
		{
			FV_WARNING_MSG( "Nub::processFilteredPacket( %s ): "
					"received undersized packet\n",
				addr.c_str() );
		}

		RETURN_FOR_CORRUPTED_PACKET();
	}

	if (m_pkPacketMonitor)
	{
		m_pkPacketMonitor->PacketIn( addr, *p );
	}

	// p->debugDump();

	if (p->GetFlags() & ~FvNetPacket::KNOWN_FLAGS)
	{
		if (!m_bIsExternal)
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"received packet with bad flags %x\n",
				addr.c_str(), p->GetFlags() );
		}

		RETURN_FOR_CORRUPTED_PACKET();
	}

	if (!p->HasFlags( FvNetPacket::FLAG_ON_CHANNEL ))
	{
		++m_uiNumPacketsReceivedOffChannel;
	}

	if (m_bIsExternal && p->HasFlags( FvNetPacket::FLAG_CREATE_CHANNEL ))
	{
		FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
			"Got FLAG_CREATE_CHANNEL on external nub\n",
			addr.c_str() );

		RETURN_FOR_CORRUPTED_PACKET();
	}

	if (p->TotalSize() <= FvNetPacket::HEADER_SIZE)
	{
		FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
			"received undersize packet (%d bytes)\n",
			addr.c_str(), p->TotalSize() );

		RETURN_FOR_CORRUPTED_PACKET();
	}

	if (p->HasFlags( FvNetPacket::FLAG_HAS_CHECKSUM ))
	{
		if (!p->StripFooter( p->GetChecksum() ))
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Packet too short (%d bytes) for checksum!\n",
				addr.c_str(), p->TotalSize() );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		*(FvNetPacket::Checksum*)p->Back() = 0;

		FvNetPacket::Checksum sum = 0;

		for (const FvNetPacket::Checksum * pData = (FvNetPacket::Checksum*)p->Data();
			 pData < (FvNetPacket::Checksum*)p->Back(); pData++)
		{
			sum ^= FV_NTOHL( *pData );
		}

		*(FvNetPacket::Checksum*)p->Back() = FV_HTONL( p->GetChecksum() );

		if (sum != p->GetChecksum())
		{
			FV_ERROR_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Packet (flags %hx, size %d) failed checksum "
				"(wanted %08x, got %08x)\n",
				addr.c_str(), p->GetFlags(), p->TotalSize(), sum, p->GetChecksum() );

			RETURN_FOR_CORRUPTED_PACKET();
		}
	}

	if (p->HasFlags( FvNetPacket::FLAG_HAS_PIGGYBACKS ))
	{
		bool done = false;

		while (!done)
		{
			FvInt16 len;

			if (!p->StripFooter( len ))
			{
				FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
					"Not enough data for piggyback length (%d bytes left)\n",
					addr.c_str(), p->BodySize() );

				RETURN_FOR_CORRUPTED_PACKET();
			}

			if (len < 0)
			{
				len = ~len;
				done = true;
			}

			if (p->BodySize() < len)
			{
				FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
					"Packet too small to contain piggyback message of "
					"length %d (only %d bytes remaining)\n",
					addr.c_str(), len, p->BodySize() );

				RETURN_FOR_CORRUPTED_PACKET();
			}

			FvNetPacketPtr piggyPack = new FvNetPacket();
			p->Shrink( len );
			memcpy( piggyPack->Data(), p->Back(), len );
			piggyPack->MsgEndOffset( len );

			try
			{
				this->ProcessFilteredPacket( addr, piggyPack.Get() );
			}
			catch (FvNetNubException & ne)
			{
				FV_ERROR_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
					"Got an exception whilst processing piggyback packet: %s\n",
					addr.c_str(), NetReasonToString( ne.Reason() ) );
			}
		}
	}

	FvNetChannelPtr pChannel = NULL;
	bool shouldSendChannel = false;

	if (p->HasFlags( FvNetPacket::FLAG_INDEXED_CHANNEL ))
	{
		if (m_pkChannelFinder == NULL)
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Got indexed channel packet with no finder registered\n",
				(char*)addr );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		if (!p->StripFooter( p->GetChannelID() ) ||
			!p->StripFooter( p->GetChannelVersion() ))
		{
			FV_WARNING_MSG( "Nub::processFilteredPacket( %s ): "
				"Not enough data for indexed channel footer (%d bytes left)\n",
				addr.c_str(), p->BodySize() );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		bool hasBeenHandled = false;
		pChannel = m_pkChannelFinder->Find( p->GetChannelID(), p, hasBeenHandled );

		if (hasBeenHandled)
		{
			return FV_NET_REASON_SUCCESS;
		}

		if (!pChannel)
		{
			pChannel = m_kCondemnedChannels.Find( p->GetChannelID() );

			if (pChannel)
			{
				FV_NOTICE_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
						"Received packet for condemned channel.\n",
					pChannel->c_str() );
			}
		}

		if (pChannel)
		{
			pChannel->OnPacketReceived( p->TotalSize() );
		}
		else
		{
			FV_ERROR_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Couldn't get indexed channel for id %d\n",
				addr.c_str(), p->GetChannelID() );

			RETURN_FOR_CORRUPTED_PACKET();
		}
	}

	if (!pChannel && p->HasFlags( FvNetPacket::FLAG_ON_CHANNEL ))
	{
		pChannel = this->FindChannel( addr );

		if (!pChannel)
		{
			FV_ASSERT_DEV( !p->ShouldCreateAnonymous() );

			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Dropping packet due to absence of local channel\n",
				addr.c_str() );

			return FV_NET_REASON_GENERAL_NETWORK;
		}
	}

	if (pChannel)
	{
		if (pChannel->HasRemoteFailed())
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Dropping packet due to remote process failure\n",
				pChannel->c_str() );

			return FV_NET_REASON_GENERAL_NETWORK;
		}

		else if (pChannel->WantsFirstPacket())
		{
			if (p->HasFlags( FvNetPacket::FLAG_CREATE_CHANNEL ))
			{
				pChannel->GotFirstPacket();
			}
			else
			{
				FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
					"Dropping packet on "
					"channel wanting FLAG_CREATE_CHANNEL (flags: %x)\n",
					pChannel->c_str(), p->GetFlags() );

				return FV_NET_REASON_GENERAL_NETWORK;
			}
		}
	}

	if (p->HasFlags( FvNetPacket::FLAG_HAS_ACKS ))
	{
		if (!p->StripFooter( p->GetNAcks() ))
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Not enough data for ack count footer (%d bytes left)\n",
				addr.c_str(), p->BodySize() );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		if (p->GetNAcks() == 0)
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Packet with FLAG_HAS_ACKS had 0 acks\n",
				addr.c_str() );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		int ackSize = p->GetNAcks() * sizeof( FvNetSeqNum );

		if (p->BodySize() < ackSize)
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Not enough footers for %d acks "
				"(have %d bytes but need %d)\n",
				addr.c_str(), p->GetNAcks(), p->BodySize(), ackSize );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		if (pChannel)
		{
			for (unsigned int i=0; i < p->GetNAcks(); i++)
			{
				FvNetSeqNum seq;
				p->StripFooter( seq );

				if (!pChannel->DelResendTimer( seq ))
				{
					FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
						"delResendTimer() failed for #%u\n",
						addr.c_str(), seq );

					RETURN_FOR_CORRUPTED_PACKET();
				}
			}
		}
		else if (!p->HasFlags( FvNetPacket::FLAG_ON_CHANNEL ))
		{
			for (unsigned int i=0; i < p->GetNAcks(); i++)
			{
				FvNetSeqNum seq;
				p->StripFooter( seq );
				this->DelOnceOffResendTimer( addr, seq );
			}
		}
		else
		{
			p->Shrink( ackSize );

			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Got %d acks without a channel\n",
				addr.c_str(), p->GetNAcks() );
		}
	}

	if (p->HasFlags( FvNetPacket::FLAG_HAS_SEQUENCE_NUMBER ))
	{
		if (!p->StripFooter( p->GetSeq() ))
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
				"Not enough data for sequence number footer (%d bytes left)\n",
				addr.c_str(), p->BodySize() );

			RETURN_FOR_CORRUPTED_PACKET();
		}
	}

	if (p->HasFlags( FvNetPacket::FLAG_IS_RELIABLE ))
	{
		if (p->GetSeq() == FvNetChannel::SEQ_NULL)
		{
			FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): Dropping packet "
				"due to illegal request for reliability "
				"without related sequence number\n", addr.c_str() );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		if (pChannel)
		{
			std::pair< FvNetPacket*, bool > result =
				pChannel->QueueAckForPacket( p, p->GetSeq(), addr );

			if (pChannel->IsIrregular())
			{
				shouldSendChannel = true;
			}

			if (result.first == NULL)
			{
				if (result.second)
				{
					++m_uiNumDuplicatePacketsReceived;

					if (pChannel->IsEstablished() && shouldSendChannel)
						pChannel->Send();

					return FV_NET_REASON_SUCCESS;
				}
				else
				{
					RETURN_FOR_CORRUPTED_PACKET();
				}
			}
		}
		else
		{
			if (m_bIsExternal)
			{
				FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
					"Dropping illegal once-off-reliable packet\n",
					addr.c_str() );

				RETURN_FOR_CORRUPTED_PACKET();
			}

			FvNetBundle backBundle;
			backBundle.AddAck( p->GetSeq() );
			this->Send( addr, backBundle );

			if (this->OnceOffReliableReceived( addr, p->GetSeq() ))
			{
				return FV_NET_REASON_SUCCESS;
			}
		}
	}
	else // if !RELIABLE
	{
		if ((pChannel != NULL) && (pChannel->IsExternal()))
		{
			if (!pChannel->validateUnreliableSeqNum( p->GetSeq() ))
			{
				FV_WARNING_MSG( "FvNetNub::ProcessFilteredPacket( %s ): "
					"Dropping packet due to invalid unreliable seqNum\n",
					addr.c_str() );
				RETURN_FOR_CORRUPTED_PACKET();
			}
		}

	}

	FvNetReason oret = FV_NET_REASON_SUCCESS;
	FvNetPacketPtr pCurrPacket = p;
	FvNetPacketPtr pNextPacket = NULL;

	while (pCurrPacket &&
		((pChannel == NULL) || !pChannel->IsCondemned()))
	{
		pNextPacket = pCurrPacket->Next();
		pCurrPacket->Chain( NULL );

		FvNetReason ret = this->ProcessOrderedPacket(
			addr, pCurrPacket.Get(), pChannel.Get() );

		if (oret == FV_NET_REASON_SUCCESS)
		{
			oret = ret;
		}

		pCurrPacket = pNextPacket;
	}

	if (pChannel &&
		!pChannel->IsDead() &&
		shouldSendChannel &&
		pChannel->IsEstablished() &&
		(pChannel->Bundle().m_spFirstPacket->GetNAcks() > 0))
	{
		pChannel->Send();
	}

	return oret;
}

FvNetReason FvNetNub::ProcessOrderedPacket( const FvNetAddress & addr, FvNetPacket * p,
	FvNetChannel * pChannel )
{
#	define SOURCE_STR (pChannel ? pChannel->c_str() : addr.c_str())

	if (p->HasFlags( FvNetPacket::FLAG_HAS_REQUESTS ))
	{
		if (!p->StripFooter( p->FirstRequestOffset() ))
		{
			FV_WARNING_MSG( "FvNetNub::ProcessPacket( %s ): "
				"Not enough data for first request offset (%d bytes left)\n",
				SOURCE_STR, p->BodySize() );

			RETURN_FOR_CORRUPTED_PACKET();
		}
	}

	FvNetPacketPtr pChain = NULL;

	if (p->HasFlags( FvNetPacket::FLAG_IS_FRAGMENT ))
	{
		if (p->BodySize() < int( sizeof( FvNetSeqNum ) * 2 ))
		{
			FV_WARNING_MSG( "FvNetNub::ProcessPacket( %s ): "
				"Not enough footers for fragment spec "
				"(have %d bytes but need %u)\n",
				SOURCE_STR, p->BodySize(), 2 * sizeof( FvNetSeqNum ) );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		p->StripFooter( p->FragEnd() );
		p->StripFooter( p->FragBegin() );

		const int numFragmentsInBundle = p->FragEnd() - p->FragBegin() + 1;

		if (numFragmentsInBundle < 2)
		{
			FV_WARNING_MSG( "FvNetNub::ProcessPacket( %s ): "
				"Dropping fragment due to illegal bundle fragment count (%d)\n",
				SOURCE_STR, numFragmentsInBundle );

			RETURN_FOR_CORRUPTED_PACKET();
		}

		FragmentedBundle::Key key( addr, p->FragBegin() );

		FragmentedBundlePtr pFragments = NULL;
		FragmentedBundles::iterator fragIter = m_kFragmentedBundles.end();

		bool isOnChannel = pChannel && p->HasFlags( FvNetPacket::FLAG_IS_RELIABLE );

		if (isOnChannel)
		{
			pFragments = pChannel->pFragments();
		}
		else
		{
			fragIter = m_kFragmentedBundles.find( key );
			pFragments = (fragIter != m_kFragmentedBundles.end()) ?
				fragIter->second : NULL;
		}

		if (pFragments && pFragments->IsOld() && !isOnChannel)
		{
			FV_WARNING_MSG( "FvNetNub::ProcessPacket( %s ): "
				"Discarding abandoned stale overlapping fragmented bundle "
				"from seq %u to %u\n",
				SOURCE_STR, p->FragBegin(), pFragments->m_uiLastFragment );

			pFragments = NULL;

			m_kFragmentedBundles.erase( fragIter );
		}

		if (pFragments == NULL)
		{
			if (pChannel && p->GetSeq() != p->FragBegin())
			{
				FV_ERROR_MSG( "Nub::processOrderedPacket( %s ): "
					"Bundle (#%u,#%u) is missing packets before #%u\n",
					SOURCE_STR, p->FragBegin(), p->FragEnd(), p->GetSeq() );

				RETURN_FOR_CORRUPTED_PACKET();
			}

			pFragments = new FragmentedBundle(
				p->FragEnd(),
				numFragmentsInBundle - 1,
				Timestamp(),
				p );

			if (isOnChannel)
			{
				pChannel->pFragments( pFragments );
			}
			else
			{
				m_kFragmentedBundles[ key ] = pFragments;
			}

			return FV_NET_REASON_SUCCESS;
		}

		if (pFragments->m_uiLastFragment != p->FragEnd())
		{
			if (!p->HasFlags( FvNetPacket::FLAG_IS_RELIABLE ))
			{
				FV_ASSERT( pFragments->IsReliable() || m_bIsExternal );

				FV_WARNING_MSG( "FvNetNub::ProcessPacket( %s ): "
					"Discarding unreliable fragment #%u (#%u,#%u) while "
					"waiting for reliable chain (#%u,#%u) to complete\n",
					SOURCE_STR, p->GetSeq(), p->FragBegin(), p->FragEnd(),
					pFragments->m_spChain->GetSeq(), pFragments->m_uiLastFragment );

				return FV_NET_REASON_SUCCESS;
			}

			if (m_bIsExternal)
			{
				FV_WARNING_MSG( "FvNetNub::ProcessPacket( %s ): "
					"Mangled fragment footers, "
					"lastFragment(%u) != p->fragEnd()(%u)\n",
					SOURCE_STR, pFragments->m_uiLastFragment, p->FragEnd() );

				RETURN_FOR_CORRUPTED_PACKET();
			}

			else
			{
				FV_CRITICAL_MSG( "FvNetNub::ProcessPacket( %s ): "
					"Mangled fragment footers, "
					"lastFragment(%u) != p->fragEnd()(%u)\n",
					(char*)addr, pFragments->m_uiLastFragment, p->FragEnd() );
			}
		}

		pFragments->m_uiTouched = Timestamp();

		FvNetPacket * pre = NULL;
		FvNetPacket * walk;

		for (walk = pFragments->m_spChain.Get(); walk; walk = walk->Next())
		{
			if (walk->GetSeq() == p->GetSeq())
			{
				FV_WARNING_MSG( "FvNetNub::ProcessPacket( %s ): "
					"Discarding duplicate fragment #%u\n",
					SOURCE_STR, p->GetSeq() );

				return FV_NET_REASON_SUCCESS;
			}

			if (FvNetChannel::SeqLessThan( p->GetSeq(), walk->GetSeq() ))
			{
				break;
			}

			pre = walk;
		}

		p->Chain( walk );

		if (pre == NULL)
		{
			pFragments->m_spChain = p; // pFragments -> chain -> p -> walk
		}
		else
		{
			pre->Chain( p ); // pFragments -> chain -> ... -> pre -> p -> walk
		}

		if (--pFragments->m_iRemaining > 0)
		{
			return FV_NET_REASON_SUCCESS;
		}
		else
		{
			pChain = pFragments->m_spChain;
			p = pChain.Get();

			if (isOnChannel)
			{
				pChannel->pFragments( NULL );
			}
			else
			{
				m_kFragmentedBundles.erase( fragIter );
			}
		}
	}

	FvNetBundle outputBundle( p );
	pChain = NULL;

	m_bBreakBundleLoop = false;
	FvNetReason ret = FV_NET_REASON_SUCCESS;

	FvNetMessageFilterPtr pMessageFilter =
		pChannel ? pChannel->pMessageFilter() : NULL;

	FvNetBundle::iterator iter	= outputBundle.Begin();
	FvNetBundle::iterator end	= outputBundle.End();
	while (iter != end && !m_bBreakBundleLoop)
	{
		FvNetInterfaceElementWithStats & ie = m_kInterfaceTable[ iter.MsgID() ];
		if (ie.GetHandler() == NULL)
		{
			FV_ERROR_MSG( "FvNetNub::ProcessOrderedPacket( %s ): "
				"Discarding bundle after hitting unhandled message id %d\n",
				SOURCE_STR, (int)iter.MsgID() );

			ret = FV_NET_REASON_NONEXISTENT_ENTRY;
			break;
		}

		FvNetUnpackedMessageHeader & header = iter.Unpack( ie );
		header.m_pkNub = this;
		header.m_pkChannel = pChannel;
		if (header.m_cFlags & FvNetPacket::FLAG_IS_FRAGMENT)
		{
			FV_ERROR_MSG( "FvNetNub::ProcessOrderedPacket( %s ): "
				"Discarding bundle due to corrupted header for message id %d\n",
				SOURCE_STR, (int)iter.MsgID() );

			// dumpBundleMessages( outputBundle );
			m_uiNumCorruptedPacketsReceived++;
			ret = FV_NET_REASON_CORRUPTED_PACKET;
			break;
		}

		const char * msgData = iter.data();
		if (msgData == NULL)
		{
			FV_ERROR_MSG( "FvNetNub::ProcessOrderedPacket( %s ): "
				"Discarding rest of bundle since chain too short for data of "
				"message id %d length %d\n",
				SOURCE_STR, (int)iter.MsgID(), header.m_iLength );

			// dumpBundleMessages( outputBundle );
			m_uiNumCorruptedPacketsReceived++;
			ret = FV_NET_REASON_CORRUPTED_PACKET;
			break;
		}

		FvMemoryIStream mis = FvMemoryIStream( msgData, header.m_iLength );

		m_uiNumMessagesReceived++;
		ie.MessageReceived( header.m_iLength );
		m_uiNumOverheadBytesReceived -= header.m_iLength;

		m_kRecvMercuryTimer.stop();

		if (!pMessageFilter)
		{
			ie.GetHandler()->HandleMessage( addr, header, mis );
		}
		else
		{
			pMessageFilter->FilterMessage( addr, header, mis, ie.GetHandler() );
		}

		m_kRecvMercuryTimer.start();

		iter++;

		if (mis.RemainingLength() != 0)
		{
			if (header.m_uiIdentifier == FV_NET_REPLY_MESSAGE_IDENTIFIER)
			{
				FV_WARNING_MSG( "FvNetNub::ProcessOrderedPacket( %s ): "
					"Handler for reply left %d bytes\n",
					SOURCE_STR, mis.RemainingLength() );

			}
			else
			{
				FV_WARNING_MSG( "FvNetNub::ProcessOrderedPacket( %s ): "
					"Handler for message %s (id %d) left %d bytes\n",
					SOURCE_STR, ie.GetName(),
					header.m_uiIdentifier, mis.RemainingLength() );
			}
		}
	}

	if (iter != end && !m_bBreakBundleLoop)
	{
		m_uiNumCorruptedBundlesReceived++;
	}
	else
	{
		m_uiNumBundlesReceived++;
	}

	if (m_pkBundleFinishHandler)
	{
		m_pkBundleFinishHandler->OnBundleFinished();
	}

	return ret;
#	undef SOURCE_STR
}


FvNetReason FvNetNub::ProcessPacketFromStream( const FvNetAddress & addr, FvBinaryIStream & data )
{
	FvNetPacketPtr pPacket = new FvNetPacket();
	int len = data.RemainingLength();

	memcpy( pPacket->Data(), data.Retrieve( len ), len );
	pPacket->MsgEndOffset( len );

	return this->ProcessPacket( addr, pPacket.Get() );
}


bool FvNetNub::FragmentedBundle::IsOld() const
{
	return Timestamp() - m_uiTouched > StampsPerSecond() * MAX_AGE;
}


bool FvNetNub::FragmentedBundle::IsReliable() const
{
	return m_spChain->HasFlags( FvNetPacket::FLAG_IS_RELIABLE );
}


void FvNetNub::DumpBundleMessages( FvNetBundle & outputBundle )
{
	FvNetBundle::iterator iter	= outputBundle.Begin();
	FvNetBundle::iterator end	= outputBundle.End();
	int count = 0;

	while (iter != end && count < 1000)
	{
		FvNetInterfaceElement & ie = m_kInterfaceTable[ iter.MsgID() ];

		if (ie.GetHandler() != NULL)
		{
			FvNetUnpackedMessageHeader & header =
				iter.Unpack( ie );

			FV_WARNING_MSG( "\tMessage %d: ID %d, len %d\n",
					count, header.m_uiIdentifier, header.m_iLength );
		}

		iter++;
		count++;
	}
}


void FvNetNub::HandleMessage( const FvNetAddress & source,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & data )
{
	if (header.m_uiIdentifier != FV_NET_REPLY_MESSAGE_IDENTIFIER)
	{
		FV_ERROR_MSG( "FvNetNub::FvNetNub::HandleMessage( %s ): "
			"received the wrong kind of message!\n",
			source.c_str() );

		return;
	}

	if (header.m_iLength < (int)(sizeof(int)))
	{
		FV_ERROR_MSG( "FvNetNub::FvNetNub::HandleMessage( %s ): "
			"received a corrupted reply message (length %d)!\n",
			source.c_str(), header.m_iLength );

		return;
	}

	int		inReplyTo;
	data >> inReplyTo;
	header.m_iLength -= sizeof(int);

	ReplyHandlerMap::iterator rheIterator = m_kReplyHandlerMap.find( inReplyTo );
	if (rheIterator == m_kReplyHandlerMap.end())
	{
		FV_WARNING_MSG( "FvNetNub::FvNetNub::HandleMessage( %s ): "
			"Couldn't find handler for reply id 0x%08x (maybe it timed out?)\n",
			source.c_str(), inReplyTo );

		data.Finish();
		return;
	}

	ReplyHandlerElement	* pRHE = rheIterator->second;

	if (m_bIsExternal &&
			((pRHE->m_pkChannel == NULL) || (source != pRHE->m_pkChannel->addr())))
	{
		FV_WARNING_MSG( "FvNetNub::FvNetNub::HandleMessage: "
			"Got reply to request %d from unexpected source %s\n",
			inReplyTo, source.c_str() );

		return;
	}

	if (pRHE->m_kTimerID != FV_NET_TIMER_ID_NONE)
	{
		this->CancelTimer( pRHE->m_kTimerID );
		pRHE->m_kTimerID = FV_NET_TIMER_ID_NONE;
	}

	m_kReplyHandlerMap.erase( rheIterator );

	pRHE->m_pkHandler->HandleMessage( source, header, data, pRHE->m_pkArg );

	delete pRHE;
}


FvNetTimerID FvNetNub::NewTimer( int microseconds,
	FvNetTimerExpiryHandler * handler,
	void * arg,
	bool recurrent )
{
	FV_ASSERT( handler );

	if (microseconds <= 0) return FV_NET_TIMER_ID_NONE;
	FvInt64 interval = FvInt64(
		( ((double)microseconds)/1000000.0 ) * StampsPerSecondD());

	TimerQueueElement *tqe = new TimerQueueElement;
	tqe->m_uiDeliveryTime = Timestamp() + interval;
	tqe->m_uiIntervalTime = recurrent ? interval : 0;
	tqe->m_iState = TimerQueueElement::STATE_PENDING;
	tqe->arg = arg;
	tqe->handler = handler;

	m_kTimerQueue.push( tqe );

	return FvNetTimerID( tqe );
}


void FvNetNub::CancelTimer( FvNetTimerID id )
{
	TimerQueueElement * tqe = (TimerQueueElement*)id;

	tqe->m_iState = TimerQueueElement::STATE_CANCELLED;
}

int FvNetNub::CancelTimers( FvNetTimerExpiryHandler * pHandler )
{
	int numRemoved = 0;

	typedef TimerQueueElement * const * Iter;

	Iter begin = &m_kTimerQueue.top();
	Iter end = begin + m_kTimerQueue.size();
	for (Iter iter = begin; iter != end; iter++)
	{
		if ((*iter)->handler == pHandler)
		{
			(*iter)->m_iState = TimerQueueElement::STATE_CANCELLED;
			numRemoved++;
		}
	}

	if (m_pkCurrentTimer && (m_pkCurrentTimer->handler == pHandler))
	{
		if (numRemoved == 0)
		{
			FV_NOTICE_MSG( "FvNetNub::CancelTimers: It is more efficient to use "
					"Nub::cancelTimer to cancel the current timer\n" );
		}

		m_pkCurrentTimer->m_iState = TimerQueueElement::STATE_CANCELLED;
		numRemoved++;
	}

	return numRemoved;
}


void FvNetNub::CancelReplyMessageHandler( FvNetReplyMessageHandler * pHandler,
	   FvNetReason reason )
{
	ReplyHandlerMap::iterator iter = m_kReplyHandlerMap.begin();

	while (iter != m_kReplyHandlerMap.end())
	{
		ReplyHandlerElement * pElement = iter->second;

		++iter;

		if (pElement->m_pkHandler == pHandler)
		{
			pElement->HandleFailure( *this, reason );
		}
	}

}


FvUInt64 FvNetNub::TimerDeliveryTime( FvNetTimerID id ) const
{
	TimerQueueElement * tqe = (TimerQueueElement*)id;
	return (tqe->m_iState == TimerQueueElement::STATE_EXECUTING) ?
			(tqe->m_uiDeliveryTime + tqe->m_uiIntervalTime) : tqe->m_uiDeliveryTime;
}

FvUInt64 & FvNetNub::TimerIntervalTime( FvNetTimerID id )
{
	TimerQueueElement * tqe = (TimerQueueElement*)id;
	return tqe->m_uiIntervalTime;
}


bool FvNetNub::RegisterFileDescriptor( int fd,
	FvNetInputNotificationHandler * handler )
{
#ifndef _WIN32
	if ((fd < 0) || (FD_SETSIZE <= fd))
	{
		FV_ERROR_MSG( "Nub::registerFileDescriptor: "
			"Tried to register invalid fd %d. FD_SETSIZE (%d)\n",
			fd, FD_SETSIZE );

		return false;
	}
#endif

	if (FD_ISSET( fd, &m_kFDReadSet ))
		return false;

	FD_SET( fd, &m_kFDReadSet );
	m_apFDHandlers[fd] = handler;

	if (fd > m_iFDLargest) m_iFDLargest = fd;

	return true;
}


bool FvNetNub::RegisterWriteFileDescriptor( int fd,
	FvNetInputNotificationHandler * handler )
{
#ifndef _WIN32
	if ((fd < 0) || (FD_SETSIZE <= fd))
	{
		FV_ERROR_MSG( "Nub::registerWriteFileDescriptor: "
			"Tried to register invalid fd %d. FD_SETSIZE (%d)\n",
			fd, FD_SETSIZE );

		return false;
	}
#endif

	if(FD_ISSET( fd, &m_kFDWriteSet )) return false;

	FD_SET( fd, &m_kFDWriteSet );
	m_apFDWriteHandlers[fd] = handler;

	if (fd > m_iFDLargest) m_iFDLargest = fd;

	++m_iFDWriteCount;
	return true;
}



bool FvNetNub::DeregisterFileDescriptor( int fd )
{
#ifndef _WIN32
	if ((fd < 0) || (FD_SETSIZE <= fd))
	{
		return false;
	}
#endif

	if(!FD_ISSET( fd, &m_kFDReadSet )) return false;

	FD_CLR( fd, &m_kFDReadSet );
	m_apFDHandlers[fd] = NULL;

	if (fd == m_iFDLargest)
	{
		this->FindLargestFileDescriptor();
	}

	return true;
}


bool FvNetNub::DeregisterWriteFileDescriptor( int fd )
{
#ifndef _WIN32
	if ((fd < 0) || (FD_SETSIZE <= fd))
	{
		return false;
	}
#endif

	if(!FD_ISSET( fd, &m_kFDWriteSet )) return false;

	FD_CLR( fd, &m_kFDWriteSet );
	m_apFDWriteHandlers[fd] = NULL;

	if (fd == m_iFDLargest)
	{
		this->FindLargestFileDescriptor();
	}

	--m_iFDWriteCount;
	return true;
}

void FvNetNub::SetBundleFinishHandler( FvNetBundleFinishHandler * pHandler )
{
	m_pkBundleFinishHandler = pHandler;
}


void FvNetNub::FindLargestFileDescriptor()
{
#ifdef _WIN32
	m_iFDLargest = 0;

	for (unsigned i=0; i < m_kFDReadSet.fd_count; ++i)
		if ((int)m_kFDReadSet.fd_array[i] > m_iFDLargest)
			m_iFDLargest = m_kFDReadSet.fd_array[i];

	for (unsigned i=0; i < m_kFDWriteSet.fd_count; ++i)
		if ((int)m_kFDWriteSet.fd_array[i] > m_iFDLargest)
			m_iFDLargest = m_kFDWriteSet.fd_array[i];

#else
	while (m_iFDLargest > 0 &&
		!FD_ISSET( m_iFDLargest, &m_kFDReadSet ) &&
		!FD_ISSET( m_iFDLargest, &m_kFDWriteSet ))
	{
		m_iFDLargest--;
	}
#endif
}


bool FvNetNub::DeregisterChildNub( FvNetNub * pChildNub )
{
	if (pChildNub->m_pkMasterNub != this)
	{
		FV_WARNING_MSG( "Nub::deregisterChildNub: "
				"Input nub is not a child of this nub.\n" );

		return false;
	}

	this->DeregisterFileDescriptor( pChildNub->m_kSocket );

	ChildNubs::iterator iter = std::find(
		m_kChildNubs.begin(), m_kChildNubs.end(), pChildNub );

	FV_ASSERT( iter != m_kChildNubs.end() );
	m_kChildNubs.erase( iter );

	if (m_kChildNubs.empty())
	{
		this->CancelTimer( m_kTickChildNubsTimerID );
		m_kTickChildNubsTimerID = FV_NET_TIMER_ID_NONE;
	}

	pChildNub->m_pkMasterNub = NULL;
	return true;
}


void FvNetNub::OnChannelGone( FvNetChannel * pChannel )
{
	this->CancelRequestsFor( pChannel );
}


void FvNetNub::CancelRequestsFor( FvNetChannel * pChannel )
{
	ReplyHandlerMap::iterator iter = m_kReplyHandlerMap.begin();

	while (iter != m_kReplyHandlerMap.end())
	{
		ReplyHandlerElement * pElement = iter->second;

		++iter;

		if (pElement->m_pkChannel == pChannel)
		{
			pElement->HandleFailure( *this, FV_NET_REASON_CHANNEL_LOST );
		}
	}
}


bool FvNetNub::RegisterChildNub( FvNetNub * pChildNub,
	FvNetInputNotificationHandler * pHandler )
{
	if (pChildNub->m_pkMasterNub)
	{
		pChildNub->m_pkMasterNub->DeregisterChildNub( pChildNub );
	}

	if (!pHandler)
	{
		pHandler = pChildNub;
	}

	bool ret = this->RegisterFileDescriptor( pChildNub->m_kSocket, pHandler );

	if (ret)
	{
		if (m_kChildNubs.empty())
		{
			m_kTickChildNubsTimerID =
				this->RegisterTimer( CHILD_NUB_TICK_PERIOD, this );
		}

		m_kChildNubs.push_back( pChildNub );
		pChildNub->m_pkMasterNub = this;
	}

	return ret;
}


FvUInt64 FvNetNub::GetSpareTime() const
{
	return m_uiSpareTime;
}

void FvNetNub::ClearSpareTime()
{
	m_uiAccSpareTime += m_uiSpareTime;
	m_uiSpareTime = 0;
}

double FvNetNub::ProportionalSpareTime() const
{
	double ret = (double)(FvInt64)(m_uiTotSpareTime - m_uiOldSpareTime);
	return ret/StampsPerSecondD();
}


double FvNetNub::PeakCalculator( LastVisitTime idx, const unsigned int & now,
		unsigned int & last ) const
{
	FvUInt64 currTime = Timestamp();
	double delta = double(currTime - lastVisitTime_[idx]) / StampsPerSecondD();
	lastVisitTime_[idx] = currTime;
	double items = now - last;
	last = now;
	return items / delta;
}


void FvNetNub::SwitchSockets( FvNetNub * pOtherNub )
{
	int tempFD = (int)m_kSocket;
	FvNetAddress tempAddr = m_kAdvertisedAddress;

	this->DeregisterFileDescriptor( m_kSocket );
	this->RegisterFileDescriptor( pOtherNub->m_kSocket, NULL );
	pOtherNub->DeregisterFileDescriptor( pOtherNub->m_kSocket );
	pOtherNub->RegisterFileDescriptor( m_kSocket, NULL );

	m_kSocket.SetFileDescriptor( pOtherNub->m_kSocket );
	m_kAdvertisedAddress = pOtherNub->m_kAdvertisedAddress;

	pOtherNub->m_kSocket.SetFileDescriptor( tempFD );
	pOtherNub->m_kAdvertisedAddress = tempAddr;

	FvUInt16 tempPort = m_uiMachinePort;
	m_uiMachinePort = pOtherNub->m_uiMachinePort;
	pOtherNub->m_uiMachinePort = tempPort;

	FvUInt16 tempUserID = m_uiUserID;
	m_uiUserID = pOtherNub->m_uiUserID;
	pOtherNub->m_uiUserID = tempUserID;
}

namespace
{
	class Hijacker
	{
	public:
		Hijacker( FvBinaryIStream & stream )
		{
			FvNetEndpoint::SetHijackStream( &stream );
		}

		~Hijacker()
		{
			FvNetEndpoint::SetHijackStream( NULL );
		}
	};
}


bool FvNetNub::HandleHijackData( int fd, FvBinaryIStream & stream )
{
	Hijacker hijacker( stream );

	if (fd == m_kSocket)
	{
		this->ProcessPendingEvents( /* expectingPacket: */ true );
		return true;
	}
	else
	{
		FvNetInputNotificationHandler *	pHandler = m_apFDHandlers[fd];

		if (pHandler)
		{
			pHandler->HandleInputNotification( fd );
			return true;
		}
		else
		{
			FV_ERROR_MSG( "FvNetNub::HandleHijackData: No handler for %d\n", fd );
		}
	}

	return false;
}


#if FV_ENABLE_WATCHERS

FvWatcherPtr FvNetNub::GetWatcher()
{
	static FvDirectoryWatcherPtr watchMe = NULL;

	if (watchMe == NULL)
	{
		watchMe = new FvDirectoryWatcher();

		FvNetNub		*pNull = NULL;

		watchMe->AddChild( "address", &FvNetAddress::GetWatcher(),
			(void*)&pNull->m_kAdvertisedAddress );

		{
			FvSequenceWatcher< InterfaceTable > * pWatcher =
				new FvSequenceWatcher< InterfaceTable >( pNull->m_kInterfaceTable );
			pWatcher->AddChild( "*", FvNetInterfaceElementWithStats::GetWatcher() );

			watchMe->AddChild( "interfaceByID", pWatcher );
		}

		{
			FvSequenceWatcher< InterfaceTable > * pWatcher =
				new FvSequenceWatcher< InterfaceTable >( pNull->m_kInterfaceTable );
			pWatcher->SetLabelSubPath( "name" );
			pWatcher->AddChild( "*", FvNetInterfaceElementWithStats::GetWatcher() );

			watchMe->AddChild( "interfaceByName", pWatcher );
		}

#ifdef unix
		watchMe->AddChild( "socket/transmitQueue",
			MakeWatcher( pNull->m_iLastTxQueueSize ) );

		watchMe->AddChild( "socket/receiveQueue",
			MakeWatcher( pNull->m_iLastRxQueueSize ) );

		watchMe->AddChild( "socket/maxTransmitQueue",
			MakeWatcher( pNull->m_iMaxTxQueueSize ) );

		watchMe->AddChild( "socket/maxReceiveQueue",
			MakeWatcher( pNull->m_iMaxRxQueueSize ) );
#endif

		watchMe->AddChild( "timing/spareTime",
			MakeWatcher( *pNull, &FvNetNub::ProportionalSpareTime ) );
		watchMe->AddChild( "timing/totalSpareTime",
				MakeWatcher( pNull->m_uiTotSpareTime ) );

		watchMe->AddChild( "socket/socket",
			new FvMemberWatcher<int,FvNetEndpoint>(
				pNull->m_kSocket,
				&FvNetEndpoint::operator int,
				static_cast< void (FvNetEndpoint::*)( int ) >( NULL ) ) );
		// timerMap_
		// replyHandlerMap_

		watchMe->AddChild( "artificialLoss/dropPerMillion",
			MakeWatcher( pNull->m_iArtificialDropPerMillion,
				FvWatcher::WT_READ_WRITE ) );

		watchMe->AddChild( "artificialLoss/minLatency",
			MakeWatcher( pNull->m_iArtificialLatencyMin,
				FvWatcher::WT_READ_WRITE ) );

		watchMe->AddChild( "artificialLoss/maxLatency",
			MakeWatcher( pNull->m_iArtificialLatencyMax,
				FvWatcher::WT_READ_WRITE ) );

		watchMe->AddChild( "misc/nextReplyID",
			MakeWatcher( pNull->m_iNextReplyID ) );

		watchMe->AddChild( "misc/nextSequenceID",
			MakeWatcher( pNull->m_uiNextSequenceID ) );

		watchMe->AddChild( "misc/breakProcessing",
			MakeWatcher( pNull->m_bBreakProcessing, FvWatcher::WT_READ_WRITE ) );

		watchMe->AddChild( "misc/largestFD",
			MakeWatcher( pNull->m_iFDLargest ) );

		watchMe->AddChild( "timing/mercurySend",
				MakeWatcher( pNull->m_kSendMercuryTimer ) );
		watchMe->AddChild( "timing/systemSend",
				MakeWatcher( pNull->m_kSendSystemTimer ) );

		watchMe->AddChild( "timing/mercuryRecv",
				MakeWatcher( pNull->m_kRecvMercuryTimer ) );
		watchMe->AddChild( "timing/systemRecv",
				MakeWatcher( pNull->m_kRecvSystemTimer ) );

		watchMe->AddChild( "totals/failedPacketSends",
			MakeWatcher( pNull->m_uiNumFailedPacketSend ) );
		watchMe->AddChild( "totals/failedBundleSends",
			MakeWatcher( pNull->m_uiNumFailedBundleSend ) );
		watchMe->AddChild( "totals/corruptedPacketsReceived",
			MakeWatcher( pNull->m_uiNumCorruptedPacketsReceived ) );
		watchMe->AddChild( "totals/corruptedBundlesReceived",
			MakeWatcher( pNull->m_uiNumCorruptedBundlesReceived ) );

		watchMe->AddChild( "totals/bytesSent",
			MakeWatcher( pNull->m_uiNumBytesSent ) );
		watchMe->AddChild( "averages/bytesSentPerSecond",
			MakeWatcher( *pNull, &FvNetNub::BytesSentPerSecondAvg ) );
		watchMe->AddChild( "lastVisit/bytesSentPerSecond",
			MakeWatcher( *pNull, &FvNetNub::BytesSentPerSecondPeak ) );
		watchMe->AddChild( "totals/bytesResent",
			MakeWatcher( pNull->m_uiNumBytesResent ) );

		watchMe->AddChild( "totals/bytesReceived",
			MakeWatcher( pNull->m_uiNumBytesReceived ) );
		watchMe->AddChild( "averages/bytesReceivedPerSecond",
			MakeWatcher( *pNull, &FvNetNub::BytesReceivedPerSecondAvg ) );
		watchMe->AddChild( "lastVisit/bytesReceivedPerSecond",
			MakeWatcher( *pNull, &FvNetNub::BytesReceivedPerSecondPeak ) );

		watchMe->AddChild( "totals/packetsSentOffChannel",
			MakeWatcher( pNull->m_uiNumPacketsSentOffChannel ) );
		watchMe->AddChild( "totals/packetsSent",
			MakeWatcher( pNull->m_uiNumPacketsSent ) );
		watchMe->AddChild( "averages/packetsSentPerSecond",
			MakeWatcher( *pNull, &FvNetNub::PacketsSentPerSecondAvg ) );
		watchMe->AddChild( "lastVisit/packetsSentPerSecond",
			MakeWatcher( *pNull, &FvNetNub::PacketsSentPerSecondPeak ) );
		watchMe->AddChild( "totals/packetsResent",
			MakeWatcher( pNull->m_uiNumPacketsResent ) );

		watchMe->AddChild( "totals/numPiggybacks",
			MakeWatcher( pNull->m_uiNumPiggybacks ) );

		watchMe->AddChild( "totals/packetsReceivedOffChannel",
			MakeWatcher( pNull->m_uiNumPacketsReceivedOffChannel ) );
		watchMe->AddChild( "totals/packetsReceived",
			MakeWatcher( pNull->m_uiNumPacketsReceived ) );
		watchMe->AddChild( "totals/duplicatePacketsReceived",
			MakeWatcher( pNull->m_uiNumDuplicatePacketsReceived ) );
		watchMe->AddChild( "averages/packetsReceivedPerSecond",
			MakeWatcher( *pNull, &FvNetNub::PacketsReceivedPerSecondAvg ) );
		watchMe->AddChild( "lastVisit/packetsReceivedPerSecond",
			MakeWatcher( *pNull, &FvNetNub::PacketsReceivedPerSecondPeak ) );

		watchMe->AddChild( "totals/bundlesSent",
			MakeWatcher( pNull->m_uiNumBundlesSent ) );
		watchMe->AddChild( "averages/bundlesSentPerSecond",
			MakeWatcher( *pNull, &FvNetNub::BundlesSentPerSecondAvg ) );
		watchMe->AddChild( "lastVisit/bundlesSentPerSecond",
			MakeWatcher( *pNull, &FvNetNub::BundlesSentPerSecondPeak ) );

		watchMe->AddChild( "totals/bundlesReceived",
			MakeWatcher( pNull->m_uiNumBundlesReceived ) );
		watchMe->AddChild( "averages/bundlesReceivedPerSecond",
			MakeWatcher( *pNull, &FvNetNub::BundlesReceivedPerSecondAvg ) );
		watchMe->AddChild( "lastVisit/bundlesReceivedPerSecond",
			MakeWatcher( *pNull, &FvNetNub::BundlesReceivedPerSecondPeak ) );

		watchMe->AddChild( "totals/messagesSent",
			MakeWatcher( pNull->m_uiNumMessagesSent ) );
		watchMe->AddChild( "totals/messagesSentReliableExt",
			MakeWatcher( pNull->m_uiNumReliableMessagesSent ) );
		watchMe->AddChild( "averages/messagesSentPerSecond",
			MakeWatcher( *pNull, &FvNetNub::MessagesSentPerSecondAvg ) );
		watchMe->AddChild( "lastVisit/messagesSentPerSecond",
			MakeWatcher( *pNull, &FvNetNub::MessagesSentPerSecondPeak ) );

		watchMe->AddChild( "totals/messagesReceived",
			MakeWatcher( pNull->m_uiNumMessagesReceived ) );
		watchMe->AddChild( "averages/messagesReceivedPerSecond",
			MakeWatcher( *pNull, &FvNetNub::MessagesReceivedPerSecondAvg ) );
		watchMe->AddChild( "lastVisit/messagesReceivedPerSecond",
			MakeWatcher( *pNull, &FvNetNub::MessagesReceivedPerSecondPeak ) );
	}

	return watchMe;
}
#endif /* #if FV_ENABLE_WATCHER */


void FvNetNub::InitOnceOffPacketCleaning()
{
	if (m_kOnceOffPacketCleaningTimerID == 0)
	{
		long onceOffPacketCleaningPeriod = (int)( 1.1 *
			m_iOnceOffMaxResends * m_iOnceOffResendPeriod );
		m_kOnceOffPacketCleaningTimerID =
				this->RegisterTimer( onceOffPacketCleaningPeriod, this );
	}
}


int FvNetNub::HandleTimeout( FvNetTimerID id, void * arg )
{
	if (id == m_kOnceOffPacketCleaningTimerID)
	{
		this->OnceOffReliableCleanup();
		return 0;
	}

	else if (id == m_kReportLimitTimerID)
	{
		this->ReportPendingExceptions();
		return 0;
	}

	else if (id == m_kTickChildNubsTimerID)
	{
		ChildNubs::iterator iter = m_kChildNubs.begin();

		while (iter != m_kChildNubs.end())
		{
			FvNetNub * pChild = *iter++;

			pChild->ProcessPendingEvents( /* expectingPacket: */ false );
		}

		return 0;
	}

	else if (id == m_kClearFragmentedBundlesTimerID)
	{
		FragmentedBundles::iterator iter = m_kFragmentedBundles.begin();
		FvUInt64 now = ::Timestamp();

		while (iter != m_kFragmentedBundles.end())
		{
			FragmentedBundles::iterator oldIter = iter++;
			const FragmentedBundle::Key & key = oldIter->first;
			FragmentedBundlePtr pFragments = oldIter->second;
			FvUInt64 age = now - pFragments->m_uiTouched;

			if (age > FragmentedBundle::MAX_AGE)//! add by Uman, 20100304, 问题记录:单位不对,但该网络模式不会被使用,暂不改
			{
				FV_WARNING_MSG( "FvNetNub::HandleTimeout: "
					"Discarded stale fragmented bundle from %s "
					"(%.1fs old, %d packets)\n",
					key.m_kAddr.c_str(), age / StampsPerSecondD(),
					pFragments->m_spChain->ChainLength() );

				m_kFragmentedBundles.erase( oldIter );
			}
		}
	}
	else if (id == m_kInterfaceStatsTimerID)
	{
		for (unsigned int i = 0; i < m_kInterfaceTable.size(); ++i)
		{
			m_kInterfaceTable[i].Tick();
		}
	}

	else if (arg == (void*)TIMEOUT_RECENTLY_DEAD_CHANNEL)
	{
		for (RecentlyDeadChannels::iterator iter = m_kRecentlyDeadChannels.begin();
			 iter != m_kRecentlyDeadChannels.end(); ++iter)
		{
			if (iter->second == id)
			{
				m_kRecentlyDeadChannels.erase( iter );
				break;
			}
		}
	}

	return 1;
}


void FvNetNub::OnceOffReliableCleanup()
{
	m_kPrevOnceOffReceipts.clear();
	m_kCurrOnceOffReceipts.swap( m_kPrevOnceOffReceipts );
}



void FvNetNub::AddOnceOffResendTimer( const FvNetAddress & addr, int seq, FvNetPacket * p )
{
//	FV_TRACE_MSG( "addOnceOffResendTimer(%s,%d)\n", (char*)addr, seq );
	OnceOffPacket temp( addr, seq, p );

	OnceOffPackets::iterator actualIter = m_kOnceOffPackets.insert( temp ).first;
	OnceOffPacket * pOnceOffPacket =
		const_cast< OnceOffPacket * > (&(*actualIter) );

	pOnceOffPacket->RegisterTimer( this );
}

void FvNetNub::ExpireOnceOffResendTimer( OnceOffPacket & oop )
{
	if (++oop.m_iRetries <= m_iOnceOffMaxResends)
	{
		if (this->RescheduleSend( oop.m_kAddr, oop.m_spPacket.Get(),
				/* isResend: */ true ))
		{
			return;
		}

		this->SendPacket( oop.m_kAddr, oop.m_spPacket.Get(), NULL, true );
	}
	else
	{
		OnceOffPackets::iterator iter = m_kOnceOffPackets.find( oop );

		if (iter != m_kOnceOffPackets.end())
		{
			FV_DEBUG_MSG( "FvNetNub::ExpOnceOffResendTimer( %s ): "
				"Discarding #%d after %d retries\n",
				oop.m_kAddr.c_str(), oop.m_iFooterSequence, m_iOnceOffMaxResends );

			this->DelOnceOffResendTimer( iter );
		}
		else
		{
			FV_CRITICAL_MSG( "FvNetNub::ExpOnceOffResendTimer( %s ): "
				"Called for #%d that we haven't got!\n",
				oop.m_kAddr.c_str(), oop.m_iFooterSequence );
		}
	}
}


void FvNetNub::DelOnceOffResendTimer( const FvNetAddress & addr, int seq )
{
	OnceOffPacket oop( addr, seq );
	OnceOffPackets::iterator found = m_kOnceOffPackets.find( oop );

	if (found != m_kOnceOffPackets.end())
	{
		this->DelOnceOffResendTimer( found );
	}
	else
	{
		FV_DEBUG_MSG( "FvNetNub::DelOnceOffResendTimer( %s ): "
			"Called for #%d that we no longer have (usually ok)\n",
			addr.c_str(), seq );
	}
}


void FvNetNub::DelOnceOffResendTimer( OnceOffPackets::iterator & iter )
{
	this->CancelTimer( iter->m_kTimerID );
	m_kOnceOffPackets.erase( iter );
}


bool FvNetNub::OnceOffReliableReceived( const FvNetAddress & addr, int seq )
{
	this->InitOnceOffPacketCleaning();

	OnceOffReceipt oor( addr, seq );

	if (m_kCurrOnceOffReceipts.find( oor ) != m_kCurrOnceOffReceipts.end() ||
		m_kPrevOnceOffReceipts.find( oor ) != m_kPrevOnceOffReceipts.end())
	{
		// ++numOnceOffReliableDupesReceived_;
		FV_TRACE_MSG( "FvNetNub::OnceOffReliableReceived( %s ): #%d already received\n",
			addr.c_str(), seq );

		return true;
	}

	// ++numOnceOffReliableReceived_;
	m_kCurrOnceOffReceipts.insert( oor );

	return false;
}


FV_NETWORK_API std::ostream& operator<<( std::ostream &s, const FvNetNub::MiniTimer &v )
{
	s << FvNiceTime( v.m_uiTotal ) << ", min ";
	s << FvNiceTime( v.m_uiMin ) << ", max ";
	s << FvNiceTime( v.m_uiMax ) << ", avg ";
	s << (v.m_uiCount ? FvNiceTime( v.m_uiTotal/v.m_uiCount ) : FvNiceTime(0));
	return s;
}

FV_NETWORK_API std::istream& operator>>( std::istream &s, FvNetNub::MiniTimer & )
{
	return s;
}

int FvNetNub::ReplyHandlerElement::HandleTimeout( FvNetTimerID /*id*/, void * nubArg )
{
	this->HandleFailure( *static_cast< FvNetNub * >( nubArg ),
			FV_NET_REASON_TIMER_EXPIRED );
	return 0;
}


void FvNetNub::ReplyHandlerElement::HandleFailure( FvNetNub & nub, FvNetReason reason )
{
	nub.m_kReplyHandlerMap.erase( m_iReplyID );

	if (m_kTimerID != FV_NET_TIMER_ID_NONE)
	{
		nub.CancelTimer( m_kTimerID );
		m_kTimerID = FV_NET_TIMER_ID_NONE;
	}

	FvNetNubException e( reason );
	m_pkHandler->HandleException( e, m_pkArg );

	delete this;
}

FvString FvNetNub::AddressErrorToString( const FvNetAddress & address,
		const FvString & errorString )
{
	std::ostringstream out;
	out << ((char*) address) << ": " << errorString;
	return out.str();
}


FvString FvNetNub::AddressErrorToString(
		const FvNetAddress & address,
		const FvString & errorString,
		const ErrorReportAndCount & reportAndCount,
		const FvUInt64 & now )
{
	FvInt64 deltaStamps = now - reportAndCount.m_uiLastReportStamps;
	double deltaMillis = 1000 * deltaStamps / StampsPerSecondD();

	char * buf = NULL;
	int bufLen = 64;
	int strLen = bufLen;
	do
	{
		bufLen = strLen + 1;
		delete [] buf;
		buf = new char[ bufLen ];
#ifdef _WIN32
		strLen = _snprintf( buf, bufLen, "%d reports of '%s' "
				"in the last %.00fms",
			reportAndCount.m_uiCount,
			AddressErrorToString( address, errorString ).c_str(),
			deltaMillis );
		if (strLen == -1) strLen = (bufLen - 1) * 2;
#else
		strLen = snprintf( buf, bufLen, "%d reports of '%s' "
				"in the last %.00fms",
			reportAndCount.m_uiCount,
			AddressErrorToString( address, errorString ).c_str(),
			deltaMillis );
#endif
	} while (strLen >= bufLen);

	FvString out( buf );
	delete [] buf;
	return out;
}


void FvNetNub::ReportError(
		const FvNetAddress & address, const char* format, ... )
{
	//! modify by Uman, 2010/11/28
#ifdef _WIN32
	static char s_acErrBuf[2048];
	va_list va;
	va_start( va, format );
	vsnprintf_s(s_acErrBuf, sizeof(s_acErrBuf), _TRUNCATE, format, va);
	va_end( va );
	this->AddReport( address, s_acErrBuf );
#else
	char * buf = NULL;
	int bufLen = 32;
	int strLen = bufLen;
	do
	{
		delete [] buf;
		bufLen = strLen + 1;
		buf = new char[ bufLen ];

		va_list va;
		va_start( va, format );
		strLen = vsnprintf( buf, bufLen, format, va );
		va_end( va );
		buf[bufLen -1] = '\0';

	} while (strLen >= bufLen );

	FvString error( buf );

	delete [] buf;

	this->AddReport( address, error );
#endif

/**	old code
	char * buf = NULL;
	int bufLen = 32;
	int strLen = bufLen;
	do
	{
		delete [] buf;
		bufLen = strLen + 1;
		buf = new char[ bufLen ];

		va_list va;
		va_start( va, format );
#ifdef _WIN32
		strLen = _vsnprintf_s( buf, bufLen, bufLen, format, va );
		if (strLen == -1) strLen = (bufLen - 1) * 2;
#else
		strLen = vsnprintf( buf, bufLen, format, va );
#endif
		va_end( va );
		buf[bufLen -1] = '\0';

	} while (strLen >= bufLen );

	FvString error( buf );

	delete [] buf;

	this->AddReport( address, error );
**/
}


void FvNetNub::ReportException( FvNetNubException & ne, const char* prefix )
{
	FvNetAddress offender( 0, 0 );
	ne.GetAddress( offender );
	if (prefix)
	{
		this->ReportError( offender,
			"%s: Exception was thrown: %s",
			prefix, NetReasonToString( ne.Reason() ) );
	}
	else
	{
		this->ReportError( offender, "Exception was thrown: %s",
				NetReasonToString( ne.Reason() ) );
	}
}


void FvNetNub::AddReport( const FvNetAddress & address, const FvString & errorString )
{
	AddressAndErrorString addressError( address, errorString );
	ErrorsAndCounts::iterator searchIter =
		this->m_kErrorsAndCounts.find( addressError );

	FvUInt64 now = Timestamp();
	if (searchIter != this->m_kErrorsAndCounts.end())
	{
		ErrorReportAndCount & reportAndCount = searchIter->second;
		reportAndCount.m_uiCount++;

		FvInt64 millisSinceLastReport = 1000 *
			(now - reportAndCount.m_uiLastReportStamps) /
			StampsPerSecond();

		reportAndCount.m_uiLastRaisedStamps = now;

		if (millisSinceLastReport >= ERROR_REPORT_MIN_PERIOD_MS)
		{
			FV_ERROR_MSG( "%s\n",
				AddressErrorToString( address, errorString,
					reportAndCount, now ).c_str() );
			reportAndCount.m_uiCount = 0;
			reportAndCount.m_uiLastReportStamps = now;
		}

	}
	else
	{
		FV_ERROR_MSG( "%s\n",
			AddressErrorToString( address, errorString ).c_str() );

		ErrorReportAndCount reportAndCount = {
			now, 	// lastReportStamps,
			now, 	// lastRaisedStamps,
			0,		// count
		};

		this->m_kErrorsAndCounts[ addressError ] = reportAndCount;
	}
}


void FvNetNub::ReportPendingExceptions( bool reportBelowThreshold )
{
	FvUInt64 now = Timestamp();

	ErrorsAndCounts::iterator staleIter = this->m_kErrorsAndCounts.end();

	for (	ErrorsAndCounts::iterator exceptionCountIter =
				this->m_kErrorsAndCounts.begin();
			exceptionCountIter != this->m_kErrorsAndCounts.end();
			++exceptionCountIter )
	{
		if (staleIter != this->m_kErrorsAndCounts.end())
		{
			this->m_kErrorsAndCounts.erase( staleIter );
			staleIter = this->m_kErrorsAndCounts.end();
		}

		const AddressAndErrorString & addressError =
			exceptionCountIter->first;
		ErrorReportAndCount & reportAndCount = exceptionCountIter->second;

		FvInt64 millisSinceLastReport = 1000 *
			(now - reportAndCount.m_uiLastReportStamps) / StampsPerSecond();
		if (reportBelowThreshold ||
				millisSinceLastReport >= ERROR_REPORT_MIN_PERIOD_MS)
		{
			if (reportAndCount.m_uiCount)
			{
				FV_ERROR_MSG( "%s\n",
					AddressErrorToString(
						addressError.first, addressError.second,
						reportAndCount, now ).c_str()
				);
				reportAndCount.m_uiCount = 0;
				reportAndCount.m_uiLastReportStamps = now;

			}
		}

		FvUInt64 sinceLastRaisedMillis = 1000 * (now - reportAndCount.m_uiLastRaisedStamps) /
			StampsPerSecond();
		if (sinceLastRaisedMillis > ERROR_REPORT_COUNT_MAX_LIFETIME_MS)
		{
			staleIter = exceptionCountIter;
		}
	}

	if (staleIter != this->m_kErrorsAndCounts.end())
	{
		this->m_kErrorsAndCounts.erase( staleIter );
	}
}
