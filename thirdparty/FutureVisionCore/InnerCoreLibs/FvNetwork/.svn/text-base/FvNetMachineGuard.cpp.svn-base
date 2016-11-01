#include "FvNetMachineGuard.h"
#include "FvNetInterfaceMinder.h"
#include "FvNetMisc.h"
#include "FvNetPortMap.h"

#include <FvMemoryStream.h>
#include <FvTimestamp.h>
#include <FvDebug.h>

#include <sstream>

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 );

#ifdef PLAYSTATION3
#define select socketselect
#endif

FvNetMGMPacket::~FvNetMGMPacket()
{
	if (!m_bDontDeleteMessages)
	{
		for (unsigned i=0; i < m_kMessages.size(); i++)
			if (m_kDelInfo[i])
				delete m_kMessages[i];
	}
}

void FvNetMGMPacket::Read( FvMemoryIStream &is )
{
	is >> m_uiFlags >> m_uiBuddy;
	while (is.RemainingLength())
	{
		FvUInt16 msglen; is >> msglen;
		const char *msgdata = (const char *)is.Retrieve( msglen );
		if (is.Error())
		{
			FV_ERROR_MSG( "FvNetMGMPacket::Read: "
				"Not enough bytes on stream (wanted %d)\n",	msglen );
			return;
		}

		FvMemoryIStream msgstream( msgdata, msglen );

		FvNetMachineGuardMessage *pMgm = FvNetMachineGuardMessage::Create( msgstream );
		if (pMgm != NULL)
			this->Append( *pMgm, true );

		else
		{
			pMgm = new UnknownMessage();
			msgstream = FvMemoryIStream( msgdata, msglen );
			pMgm->Read( msgstream );
			this->Append( *pMgm, true );
			FV_ERROR_MSG( "FvNetMGMPacket::Read: "
				"Unknown message encountered: %s\n", pMgm->c_str() );
		}
	}
}


bool FvNetMGMPacket::Write( FvMemoryOStream &os ) const
{
	os << m_uiFlags << (ms_uiBuddy != FV_NET_BROADCAST ? ms_uiBuddy : m_uiBuddy);
	for (unsigned i=0; i < m_kMessages.size(); i++)
	{
		int offset = os.Size(); os.Reserve( sizeof( FvUInt16 ) );
		m_kMessages[i]->Write( os );
		FvUInt16 msglen = os.Size() - offset - sizeof( FvUInt16 );
		*(FvUInt16*)((char*)os.Data() + offset) = FV_HTONS( msglen );
	}

	bool isOversized = (os.Size() > FvNetMGMPacket::MAX_SIZE);
	if (isOversized)
	{
		FV_ERROR_MSG( "FvNetMGMPacket::Write: Wrote FvNetMGMPacket %d bytes long/%d "
			"messages. You need to implement MGM fragmentation\n",
			os.Size(), m_kMessages.size() );
	}

	return !isOversized;
}

void FvNetMGMPacket::Append( FvNetMachineGuardMessage &mgm, bool shouldDelete )
{
	m_kMessages.push_back( &mgm );
	m_kDelInfo.push_back( shouldDelete );
}

void FvNetMGMPacket::SetBuddy( FvUInt32 buddy )
{
	ms_uiBuddy = buddy;
}

FvUInt32 FvNetMGMPacket::ms_uiBuddy = FV_NET_BROADCAST;

FvNetMachineGuardMessage::FvNetMachineGuardMessage( FvUInt8 message, FvUInt8 flags, FvUInt16 seq ) :
	m_uiMessage( message ), m_uiFlags( flags ), m_uiSeq( seq ), m_bSeqSent( false )
{
	if (m_uiSeq == 0)
		this->RefreshSeq();
}

void FvNetMachineGuardMessage::RefreshSeq()
{
	if (ms_uiSeqTicker == 0)
	{
		srand( (int)Timestamp() );
		ms_uiSeqTicker = rand() % 0xffff;
	}

	ms_uiSeqTicker = (ms_uiSeqTicker + 1) % 0xffff;
	if (ms_uiSeqTicker == 0)
		ms_uiSeqTicker++;

	m_uiSeq = ms_uiSeqTicker;
	m_bSeqSent = false;
}

FvNetMachineGuardMessage* FvNetMachineGuardMessage::Create( FvBinaryIStream &is )
{
	FvUInt8 message = (FvUInt8)is.Peek();
	FvNetMachineGuardMessage *pMgm = NULL;

	if (is.Error())
		return NULL;

	switch (message)
	{
	case FvNetMachineGuardMessage::HIGH_PRECISION_MACHINE_MESSAGE:
		pMgm = new FvNetHighPrecisionMachineMessage(); break;
	case FvNetMachineGuardMessage::WHOLE_MACHINE_MESSAGE:
		pMgm = new FvNetWholeMachineMessage(); break;
	case FvNetMachineGuardMessage::PROCESS_MESSAGE:
		pMgm = new FvNetProcessMessage(); break;
	case FvNetMachineGuardMessage::PROCESS_STATS_MESSAGE:
		pMgm = new FvNetProcessStatsMessage(); break;
	case FvNetMachineGuardMessage::LISTENER_MESSAGE:
		pMgm = new FvNetListenerMessage(); break;
	case FvNetMachineGuardMessage::CREATE_MESSAGE:
		pMgm = new FvNetCreateMessage(); break;
	case FvNetMachineGuardMessage::SIGNAL_MESSAGE:
		pMgm = new FvNetSignalMessage(); break;
	case FvNetMachineGuardMessage::TAGS_MESSAGE:
		pMgm = new FvNetTagsMessage(); break;
	case FvNetMachineGuardMessage::USER_MESSAGE:
		pMgm = new FvNetUserMessage(); break;
	case FvNetMachineGuardMessage::PID_MESSAGE:
		pMgm = new FvNetPidMessage(); break;
	case FvNetMachineGuardMessage::RESET_MESSAGE:
		pMgm = new FvNetResetMessage(); break;
	case FvNetMachineGuardMessage::ERROR_MESSAGE:
		pMgm = new FvNetErrorMessage(); break;
	case FvNetMachineGuardMessage::MACHINED_ANNOUNCE_MESSAGE:
		pMgm = new FvNetMachinedAnnounceMessage(); break;
	case FvNetMachineGuardMessage::QUERY_INTERFACE_MESSAGE:
		pMgm = new FvNetQueryInterfaceMessage(); break;
	case FvNetMachineGuardMessage::CREATE_WITH_ARGS_MESSAGE:
		pMgm = new FvNetCreateWithArgsMessage(); break;
	default:
		return NULL;
	}

	pMgm->Read( is );
	if (is.Error())
	{
		delete pMgm;
		return NULL;
	}
	else
		return pMgm;
}

FvNetMachineGuardMessage* FvNetMachineGuardMessage::Create( void *buf, int length )
{
	FvMemoryIStream is( buf, length );
	return Create( is );
}

const char *FvNetMachineGuardMessage::TypeStr() const
{
	static char buf[32];
	switch (m_uiMessage)
	{
		case HIGH_PRECISION_MACHINE_MESSAGE: strcpy_s( buf, 32, "HIGH_PRECISION" ); break;
		case WHOLE_MACHINE_MESSAGE: strcpy_s( buf, 32, "WHOLE_MACHINE" ); break;
		case PROCESS_MESSAGE: strcpy_s( buf, 32, "PROCESS" ); break;
		case PROCESS_STATS_MESSAGE: strcpy_s( buf, 32, "PROCESS_STATS" ); break;
		case LISTENER_MESSAGE: strcpy_s( buf, 32, "LISTENER" ); break;
		case CREATE_MESSAGE: strcpy_s( buf, 32, "CREATE" ); break;
		case SIGNAL_MESSAGE: strcpy_s( buf, 32, "SIGNAL" ); break;
		case TAGS_MESSAGE: strcpy_s( buf, 32, "TAGS" ); break;
		case USER_MESSAGE: strcpy_s( buf, 32, "USER" ); break;
		case PID_MESSAGE: strcpy_s( buf, 32, "PID" ); break;
		case RESET_MESSAGE: strcpy_s( buf, 32, "RESET" ); break;
		case MACHINED_ANNOUNCE_MESSAGE : strcpy_s( buf, 32, "ANNOUNCE" ); break;
		case QUERY_INTERFACE_MESSAGE: strcpy_s( buf, 32, "QUERY_INTERFACE" ); break;
		default: strcpy_s( buf, 32, "** UNKNOWN **" ); break;
	}

	return buf;
}

void FvNetMachineGuardMessage::Write( FvBinaryOStream &os )
{
	if (m_bSeqSent && !this->OutGoing())
		this->RefreshSeq();
	os << m_uiMessage << m_uiFlags << m_uiSeq;
	m_bSeqSent = true;
}

void FvNetMachineGuardMessage::Read( FvBinaryIStream &is )
{
	is >> m_uiMessage >> m_uiFlags >> m_uiSeq;
}

const char *FvNetMachineGuardMessage::c_str() const
{
	strcpy_s( ms_acBuf, 1024, this->TypeStr() );
	return ms_acBuf;
}

bool FvNetMachineGuardMessage::SendTo( FvNetEndpoint &ep, FvUInt16 port, FvUInt32 addr,
	FvUInt8 packFlags )
{
	FvNetMGMPacket packet;
	FvMemoryOStream os;

	packet.m_uiFlags = packFlags;
	packet.Append( *this );
	packet.Write( os );

	return ep.SendTo( os.Data(), os.Size(), port, addr ) == os.Size();
}

FvNetReason FvNetMachineGuardMessage::SendAndRecv( FvNetEndpoint &ep, FvUInt32 destaddr, FvUInt16 uiMachinePort,
	ReplyHandler *pHandler )
{
	if (destaddr == FV_NET_BROADCAST)
		ep.SetBroadcast( true );

	char recvbuf[ FvNetMGMPacket::MAX_SIZE ];

	std::set< FvUInt32 > replied, waiting;

	int countdown = 3;
	while (countdown--)
	{
		if (!this->SendTo( ep, htons( uiMachinePort ), destaddr,
				FvNetMGMPacket::PACKET_STAGGER_REPLIES ))
		{
			FV_ERROR_MSG( "Failed to send entire MGM (#%d tries left), Addr:%s, Port:%d, Err:%d\n",
				countdown,
				inet_ntoa((struct in_addr&)destaddr),
				uiMachinePort,
				WSAGetLastError() );
			continue;
		}

		timeval tv = { 1, 0 };
		fd_set fds;
		int fd = int( ep );
		FD_ZERO( &fds );
		FD_SET( fd, &fds );

		while (select( fd+1, &fds, NULL, NULL, &tv ) == 1)
		{
			FvUInt32 srcaddr, buddyaddr;

			int len = ep.RecvFrom( recvbuf, sizeof( recvbuf ), NULL,
				&(u_int32_t&)srcaddr );

			if (len == -1)
			{
				FV_WARNING_MSG( "FvNetMachineGuardMessage::sendAndRecv: "
						"recvfrom failed (%s)\n",
					strerror( errno ) );
				continue;
			}

			bool badseq = false;
			bool continueProcessing = true;

			FvMemoryIStream is( (const char *)recvbuf, len );
			FvNetMGMPacket packet( is );
			buddyaddr = packet.m_uiBuddy;

			if (is.Error())
				continue;

			for (unsigned i=0; i < packet.m_kMessages.size(); i++)
			{
				FvNetMachineGuardMessage &reply = *packet.m_kMessages[i];

				if (reply.m_uiSeq != m_uiSeq)
				{
					FV_WARNING_MSG( "MGM::SendAndRecv: "
						"Bad seq (%d, wanted %d) from %s: %s\n",
						reply.m_uiSeq, m_uiSeq, inet_ntoa( (in_addr&)srcaddr ),
						reply.c_str() );
					badseq = true;
					break;
				}

				if (reply.m_uiFlags & reply.MESSAGE_NOT_UNDERSTOOD)
				{
					FV_ERROR_MSG( "MGM::SendAndRecv: "
						"Machined on %s did not understand %s\n",
						inet_ntoa( (in_addr&)srcaddr ), reply.c_str() );
					continue;
				}

				if (pHandler)
				{
					continueProcessing = pHandler->handle( reply, srcaddr );
					if (!continueProcessing)
						break;
				}
			}

			if (badseq)
				continue;

			replied.insert( srcaddr );
			if (waiting.count( srcaddr ))
				waiting.erase( srcaddr );
			if (replied.count( buddyaddr ) == 0 && buddyaddr != 0)
				waiting.insert( buddyaddr );

			if (!continueProcessing || destaddr != FV_NET_BROADCAST ||
				(replied.size() && !waiting.size()))
				return FV_NET_REASON_SUCCESS;
		}
	}

	FV_ERROR_MSG( "MGM::SendAndReceiveMGM: timed out!\n" );
	return FV_NET_REASON_TIMER_EXPIRED;
}


FvNetReason FvNetMachineGuardMessage::SendAndRecv( FvUInt32 srcip, FvUInt32 destaddr, FvUInt16 uiMachinePort,
	ReplyHandler *pHandler )
{
	// Set up socket
	FvNetEndpoint ep;
	ep.Socket( SOCK_DGRAM );
	if (!ep.Good() || ep.Bind( 0, srcip ) != 0)
		return FV_NET_REASON_GENERAL_NETWORK;

	return this->SendAndRecv( ep, destaddr, uiMachinePort, pHandler );
}

FvNetReason FvNetMachineGuardMessage::SendAndRecvFromEndpointAddr(
	FvNetEndpoint & ep, FvUInt32 destaddr, FvUInt16 uiMachinePort, ReplyHandler * pHandler )
{
	FvUInt32 epAddr;
	if (ep.GetLocalAddress( NULL, (u_int32_t*)&epAddr ))
	{
		FV_ERROR_MSG( "FvNetMachineGuardMessage::sendAndRecvFromEndpointAddr: "
			"Couldn't get local address of provided endpoint\n" );

		return FV_NET_REASON_GENERAL_NETWORK;
	}

	return this->SendAndRecv( epAddr, destaddr, uiMachinePort, pHandler );
}


char FvNetMachineGuardMessage::ms_acBuf[ 1024 ];
FvUInt16 FvNetMachineGuardMessage::ms_uiSeqTicker = 0;


bool FvNetMachineGuardMessage::ReplyHandler::handle(
	FvNetMachineGuardMessage &mgm, FvUInt32 addr )
{
	switch (mgm.m_uiMessage)
	{
		case FvNetMachineGuardMessage::HIGH_PRECISION_MACHINE_MESSAGE:
			return OnHighPrecisionMachineMessage(
				static_cast< FvNetHighPrecisionMachineMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::WHOLE_MACHINE_MESSAGE:
			return OnWholeMachineMessage(
				static_cast< FvNetWholeMachineMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::PROCESS_MESSAGE:
			return OnProcessMessage(
				static_cast< FvNetProcessMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::PROCESS_STATS_MESSAGE:
			return OnProcessStatsMessage(
				static_cast< FvNetProcessStatsMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::LISTENER_MESSAGE:
			return OnListenerMessage(
				static_cast< FvNetListenerMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::CREATE_MESSAGE:
			return OnCreateMessage(
				static_cast< FvNetCreateMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::SIGNAL_MESSAGE:
			return OnSignalMessage(
				static_cast< FvNetSignalMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::TAGS_MESSAGE:
			return OnTagsMessage(
				static_cast< FvNetTagsMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::USER_MESSAGE:
			return OnUserMessage(
				static_cast< FvNetUserMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::PID_MESSAGE:
			return OnPidMessage(
				static_cast< FvNetPidMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::RESET_MESSAGE:
			return OnResetMessage(
				static_cast< FvNetResetMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::ERROR_MESSAGE:
			return OnErrorMessage(
				static_cast< FvNetErrorMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::MACHINED_ANNOUNCE_MESSAGE:
			return OnMachinedAnnounceMessage(
				static_cast< FvNetMachinedAnnounceMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::QUERY_INTERFACE_MESSAGE:
			return OnQueryInterfaceMessage(
				static_cast< FvNetQueryInterfaceMessage& >( mgm ), addr );
		case FvNetMachineGuardMessage::CREATE_WITH_ARGS_MESSAGE:
			return OnCreateWithArgsMessage(
				static_cast< FvNetCreateWithArgsMessage& >( mgm ), addr );
		default:
			return OnUnhandledMsg( mgm, addr );
	}
}

bool FvNetMachineGuardMessage::ReplyHandler::OnUnhandledMsg(
	FvNetMachineGuardMessage &mgm, FvUInt32 addr )
{
	FV_ERROR_MSG( "MGM::ReplyHandler: Unhandled message from %s: %s\n",
		inet_ntoa( (in_addr&)addr ), mgm.c_str() );
	return true;
}

bool FvNetMachineGuardMessage::ReplyHandler::OnHighPrecisionMachineMessage(
	FvNetHighPrecisionMachineMessage &hpmm, FvUInt32 addr ){
	return OnUnhandledMsg( hpmm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnWholeMachineMessage(
	FvNetWholeMachineMessage &wmm, FvUInt32 addr ){
	return OnUnhandledMsg( wmm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnProcessMessage(
	FvNetProcessMessage &pm, FvUInt32 addr ){
	return OnUnhandledMsg( pm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnProcessStatsMessage(
	FvNetProcessStatsMessage &psm, FvUInt32 addr ){
	return OnUnhandledMsg( psm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnListenerMessage(
	FvNetListenerMessage &lm, FvUInt32 addr ){
	return OnUnhandledMsg( lm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnCreateMessage(
	FvNetCreateMessage &cm, FvUInt32 addr ){
	return OnUnhandledMsg( cm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnSignalMessage(
	FvNetSignalMessage &sm, FvUInt32 addr ){
	return OnUnhandledMsg( sm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnTagsMessage(
	FvNetTagsMessage &tm, FvUInt32 addr ){
	return OnUnhandledMsg( tm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnUserMessage(
	FvNetUserMessage &um, FvUInt32 addr ){
	return OnUnhandledMsg( um, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnPidMessage(
	FvNetPidMessage &pm, FvUInt32 addr ){
	return OnUnhandledMsg( pm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnResetMessage(
	FvNetResetMessage &rm, FvUInt32 addr ){
	return OnUnhandledMsg( rm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnErrorMessage(
	FvNetErrorMessage &rm, FvUInt32 addr ){
	return OnUnhandledMsg( rm, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnMachinedAnnounceMessage(
	FvNetMachinedAnnounceMessage &mam, FvUInt32 addr ){
	return OnUnhandledMsg( mam, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnQueryInterfaceMessage(
	FvNetQueryInterfaceMessage &qim, FvUInt32 addr ){
	return OnUnhandledMsg( qim, addr ); }
bool FvNetMachineGuardMessage::ReplyHandler::OnCreateWithArgsMessage(
	FvNetCreateWithArgsMessage &cwam, FvUInt32 addr ){
	return OnUnhandledMsg( cwam, addr ); }

FvNetQueryInterfaceMessage::FvNetQueryInterfaceMessage() :
	FvNetMachineGuardMessage( FvNetMachineGuardMessage::QUERY_INTERFACE_MESSAGE ),
	m_uiAddress( INTERNAL )
{
}

FvNetQueryInterfaceMessage::~FvNetQueryInterfaceMessage()
{
}

void FvNetQueryInterfaceMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );

	os << m_uiAddress;
}

void FvNetQueryInterfaceMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );

	is >> m_uiAddress;
}

const char *FvNetQueryInterfaceMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf), "QueryInterfaceMessage" );
	return FvNetMachineGuardMessage::ms_acBuf;
}


FvNetHighPrecisionMachineMessage::FvNetHighPrecisionMachineMessage() :
	FvNetMachineGuardMessage( FvNetMachineGuardMessage::HIGH_PRECISION_MACHINE_MESSAGE ),
	m_uiNCPUs( 0 ), m_uiNInterfaces( 0 )
{
}

FvNetHighPrecisionMachineMessage::~FvNetHighPrecisionMachineMessage()
{
}

void FvNetHighPrecisionMachineMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );

	os << m_kHostname << m_uiCPUSpeed << m_uiNCPUs << m_uiNInterfaces
	   << m_uiMem << m_uiVersion << m_uiInDiscards << m_uiOutDiscards;

	for (unsigned int i=0; i < m_uiNCPUs; i++)
		os << m_kCPULoads[i];

	os << m_uiIOWaitTime;

	for (unsigned int i=0; i < m_uiNInterfaces; i++)
	{
		const FvNetHighPrecisionInterfaceStats &is = m_kIFStats[i];
		os << is.m_kName << is.m_uiBitsIn << is.m_uiBitsOut <<
			is.m_uiPackIn << is.m_uiPackOut;
	}
}

void FvNetHighPrecisionMachineMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );

	is >> m_kHostname >> m_uiCPUSpeed >> m_uiNCPUs >> m_uiNInterfaces
	   >> m_uiMem >> m_uiVersion >> m_uiInDiscards >> m_uiOutDiscards;

	m_kCPULoads.resize( m_uiNCPUs, 0 );

	m_kIFStats.resize( m_uiNInterfaces );

	for (unsigned int i=0; i < m_uiNCPUs; i++)
		is >> m_kCPULoads[i];

	is >> m_uiIOWaitTime;

	for (unsigned int i=0; i < m_uiNInterfaces; i++)
	{
		FvNetHighPrecisionInterfaceStats &ifs = m_kIFStats[i];
		is >> ifs.m_kName >> ifs.m_uiBitsIn >> ifs.m_uiBitsOut >>
			ifs.m_uiPackIn >> ifs.m_uiPackOut;
	}
}

const char *FvNetHighPrecisionMachineMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf,
				sizeof(FvNetMachineGuardMessage::ms_acBuf),
				"HighPrecisionMachineMessage" );
	return FvNetMachineGuardMessage::ms_acBuf;
}

FvNetWholeMachineMessage::FvNetWholeMachineMessage() :
	FvNetMachineGuardMessage( FvNetMachineGuardMessage::WHOLE_MACHINE_MESSAGE ),
	m_uiNCPUs( 0 ), m_uiNInterfaces( 0 )
{
}

FvNetWholeMachineMessage::~FvNetWholeMachineMessage()
{
}

void FvNetWholeMachineMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );

	os << m_kHostname << m_uiCPUSpeed << m_uiNCPUs << m_uiNInterfaces
	   << m_uiMem << m_uiVersion << m_uiInDiscards << m_uiOutDiscards;

	for (int i=0; i < m_uiNCPUs; i++)
		os << m_kCPULoads[i];

	for (int i=0; i < m_uiNInterfaces; i++)
	{
		const FvNetLowPrecisionInterfaceStats &is = m_kIFStats[i];
		os << is.m_kName << is.m_uiBitsIn << is.m_uiBitsOut <<
			is.m_uiPackIn << is.m_uiPackOut;
	}
}

void FvNetWholeMachineMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );

	is >> m_kHostname >> m_uiCPUSpeed >> m_uiNCPUs >> m_uiNInterfaces
	   >> m_uiMem >> m_uiVersion >> m_uiInDiscards >> m_uiOutDiscards;

	m_kCPULoads.resize( m_uiNCPUs, 0 );

	m_kIFStats.resize( m_uiNInterfaces );

	for (int i=0; i < m_uiNCPUs; i++)
		is >> m_kCPULoads[i];

	for (int i=0; i < m_uiNInterfaces; i++)
	{
		FvNetLowPrecisionInterfaceStats &ifs = m_kIFStats[i];
		is >> ifs.m_kName >> ifs.m_uiBitsIn >> ifs.m_uiBitsOut >>
			ifs.m_uiPackIn >> ifs.m_uiPackOut;
	}
}

const char *FvNetWholeMachineMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf,
				sizeof(FvNetMachineGuardMessage::ms_acBuf),
				"FvNetWholeMachineMessage" );
	return FvNetMachineGuardMessage::ms_acBuf;
}

void FvNetProcessMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );
	os << m_uiParam << m_uiCategory << m_uiUID << m_uiPID << m_uiPort << m_uiID << m_kName;
}

void FvNetProcessMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );
	is >> m_uiParam >> m_uiCategory >> m_uiUID >> m_uiPID >> m_uiPort >> m_uiID >> m_kName;
}

const char *FvNetProcessMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf),
		"%s (%s) (pid:%d) (uid:%d) (port:%d)",
		m_kName.c_str(),
		FvNetProcessMessage::categoryToString( m_uiCategory ),
		m_uiPID,
		m_uiUID,
		ntohs(m_uiPort));
	return FvNetMachineGuardMessage::ms_acBuf;
}

bool FvNetProcessMessage::matches( const FvNetProcessMessage &query ) const
{
	if (query.m_uiParam & query.PARAM_USE_UID && query.m_uiUID != m_uiUID)
		return false;
	if (query.m_uiParam & query.PARAM_USE_PID && query.m_uiPID != m_uiPID)
		return false;
	if (query.m_uiParam & query.PARAM_USE_ID && query.m_uiID != m_uiID)
		return false;
	if (query.m_uiParam & query.PARAM_USE_NAME && query.m_kName != m_kName)
		return false;
	if (query.m_uiParam & query.PARAM_USE_CATEGORY && query.m_uiCategory != m_uiCategory)
		return false;
	if (query.m_uiParam & query.PARAM_USE_PORT && query.m_uiPort != m_uiPort)
		return false;
	return true;
}


const char *FvNetProcessMessage::categoryToString( FvUInt8 category )
{
	static const FvString sc = "SERVER_COMPONENT", wn = "WATCHER_NUB",
		err = "UNKNOWN";
	if (category == SERVER_COMPONENT)
		return sc.c_str();
	else if (category == WATCHER_NUB)
		return wn.c_str();
	else
		return err.c_str();
}

void FvNetProcessStatsMessage::Write( FvBinaryOStream &os )
{
	FvNetProcessMessage::Write( os );
	os << m_uiCPU << m_uiMem;
}

void FvNetProcessStatsMessage::Read( FvBinaryIStream &is )
{
	FvNetProcessMessage::Read( is );
	is >> m_uiCPU >> m_uiMem;
}

const char *FvNetProcessStatsMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf),
		"%s (%d)",
		m_kName.c_str(), m_uiPID );
	return FvNetMachineGuardMessage::ms_acBuf;
}

void FvNetListenerMessage::Write( FvBinaryOStream &os )
{
	FvNetProcessMessage::Write( os );
	os << m_kPreAddr << m_kPostAddr;
}

void FvNetListenerMessage::Read( FvBinaryIStream &is )
{
	FvNetProcessMessage::Read( is );
	is >> m_kPreAddr >> m_kPostAddr;
}

const char *FvNetListenerMessage::c_str() const
{
	if (m_uiParam == (this->PARAM_IS_MSGTYPE | this->ADD_BIRTH_LISTENER))
	{
		FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf),
			"RegBirthL:%s",
			m_kName.c_str() );
	}
	else
	{
		FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf),
			"RegDeathL:%s",
			m_kName.c_str() );
	}

	return FvNetMachineGuardMessage::ms_acBuf;
}

void FvNetCreateMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );
	os << m_kName << m_kConfig << m_uiUID << m_uiRecover << m_uiFWDIP << m_uiFWDPort;
}

void FvNetCreateMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );
	is >> m_kName >> m_kConfig >> m_uiUID >> m_uiRecover >> m_uiFWDIP >> m_uiFWDPort;
}

const char *FvNetCreateMessage::c_str() const
{
	return this->c_str_name( "CreateMessage" );
}

const char *FvNetCreateMessage::c_str_name( const char * className ) const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf),
		"%s: %s/%s (uid:%d)",
		className, m_kConfig.c_str(), m_kName.c_str(), m_uiUID );
	return FvNetMachineGuardMessage::ms_acBuf;
}

void FvNetCreateWithArgsMessage::Write( FvBinaryOStream &os )
{
	FvNetCreateMessage::Write( os );

	os << FvUInt32( m_kArgs.size() );
	for ( Args::const_iterator i = m_kArgs.begin(); i != m_kArgs.end(); ++i )
	{
		os << *i;
	}
}

void FvNetCreateWithArgsMessage::Read( FvBinaryIStream &is )
{
	FvNetCreateMessage::Read( is );

	FvUInt32 numArgs;
	is >> numArgs;
	m_kArgs.resize( numArgs );
	for ( FvUInt32 i = 0; i < numArgs; ++i )
	{
		is >> m_kArgs[i];
	}
}

const char *FvNetCreateWithArgsMessage::c_str() const
{
	std::stringstream ss;
	ss << FvNetCreateMessage::c_str_name( "CreateWithArgsMessage" );
	ss << " args=[";
	Args::const_iterator i = m_kArgs.begin();
	if (i != m_kArgs.end())
	{
		ss << *i;
		for ( ++i; i != m_kArgs.end(); ++i )
		{
			ss << ',' << *i;
		}
	}
	ss << ']';

	strncpy_s( ms_acBuf, 1024, ss.str().c_str(),
			sizeof(FvNetMachineGuardMessage::ms_acBuf) - 1 );
	ms_acBuf[ sizeof(FvNetMachineGuardMessage::ms_acBuf) - 1 ] = '\0';

	return FvNetMachineGuardMessage::ms_acBuf;
}

void FvNetSignalMessage::Write( FvBinaryOStream &os )
{
	FvNetProcessMessage::Write( os );
	os << m_uiSignal;
}

void FvNetSignalMessage::Read( FvBinaryIStream &is )
{
	FvNetProcessMessage::Read( is );
	is >> m_uiSignal;
}

const char *FvNetSignalMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf), "FvNetSignalMessage: %d", m_uiSignal );
	return FvNetMachineGuardMessage::ms_acBuf;
}

void FvNetTagsMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );
	os << (FvUInt8)m_kTags.size() << m_uiExists;
	for (unsigned i=0; i < m_kTags.size(); i++)
		os << m_kTags[i];
}

void FvNetTagsMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );
	FvUInt8 tagCount;
	is >> tagCount >> m_uiExists;

	FvString s;
	for (FvUInt8 i=0; i < tagCount; i++)
	{
		is >> s;
		m_kTags.push_back( s );
	}
}

const char *FvNetTagsMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf), "FvNetTagsMessage" );
	return FvNetMachineGuardMessage::ms_acBuf;
}

FvBinaryIStream& operator>>( FvBinaryIStream &is, FvNetUserMessage::CoreDump &cd )
{
	is >> cd.m_kFilename >> cd.m_kAssert >> cd.m_uiTime;
	return is;
}

FvBinaryOStream& operator<<( FvBinaryOStream &os, const FvNetUserMessage::CoreDump &cd )
{
	os << cd.m_kFilename << cd.m_kAssert << cd.m_uiTime;
	return os;
}

void FvNetUserMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );
	os << m_uiParam << m_uiUID << m_kUsername << m_kFullname <<
		m_kHome << m_kRoot << m_kResPath;

	static std::vector< int > empty;
	if (m_uiParam & PARAM_CHECK_COREDUMPS)
		os << m_kCoreDumps;
	else
		os << empty;
}

void FvNetUserMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );
	is >> m_uiParam >> m_uiUID >> m_kUsername >> m_kFullname >>
		m_kHome >> m_kRoot >> m_kResPath >> m_kCoreDumps;
}

const char *FvNetUserMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf), "FvNetUserMessage: %s:%d",
		m_kUsername.c_str(), m_uiUID );
	return FvNetMachineGuardMessage::ms_acBuf;
}

#if !defined( _WIN32 ) && !defined( PLAYSTATION3 )
void FvNetUserMessage::Init( const struct passwd &pwent )
{
	m_uiUID = pwent.pw_uid;
	m_uiGID = pwent.pw_gid;
	m_kUsername = pwent.pw_name;
	m_kFullname = pwent.pw_gecos;
	m_kHome = pwent.pw_dir;
}
#endif

bool FvNetUserMessage::Matches( const FvNetUserMessage &query ) const
{
	if (query.m_uiParam & query.PARAM_USE_UID && query.m_uiUID != m_uiUID)
		return false;
	if (query.m_uiParam & query.PARAM_USE_NAME && query.m_kUsername != m_kUsername)
		return false;
	return true;
}

const char * FvNetUserMessage::GetConfFilename() const
{
	static char buf[ 256 ];
	FvSNPrintf( buf, sizeof( buf ), "%s/.fvmachined.conf", m_kHome.c_str() );;
	return buf;
}

void FvNetErrorMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );
	os << m_uiSeverity << m_kMsg;
}

void FvNetErrorMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );
	is >> m_uiSeverity >> m_kMsg;
}

const char *FvNetErrorMessage::c_str() const
{
	FvSNPrintf( ms_acBuf, sizeof(ms_acBuf), "%s: %s", MessagePrefix( (FvDebugMessagePriority)m_uiSeverity ),
		m_kMsg.c_str() );

	return ms_acBuf;
}

void FvNetMachinedAnnounceMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );
	os << m_uiType << m_uiCount;
}

void FvNetMachinedAnnounceMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );
	is >> m_uiType >> m_uiCount;
}

const char *FvNetMachinedAnnounceMessage::c_str() const
{
	if (m_uiType == this->ANNOUNCE_BIRTH)
	{
		FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf), "MachinedBirth: %d", m_uiCount );
	}
	else
	{
		FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf), "MachinedDeath: %s",
			inet_ntoa( (in_addr&)m_uiAddr ) );
	}

	return FvNetMachineGuardMessage::ms_acBuf;
}

void FvNetPidMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );
	os << m_uiPID << m_uiRunning;
}

void FvNetPidMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );
	is >> m_uiPID >> m_uiRunning;
}

const char *FvNetPidMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf), "FvNetPidMessage: %d %d", m_uiPID,
		m_uiRunning );
	return FvNetMachineGuardMessage::ms_acBuf;
}

UnknownMessage::~UnknownMessage()
{
	delete [] m_pcData;
}

void UnknownMessage::Write( FvBinaryOStream &os )
{
	FvNetMachineGuardMessage::Write( os );
	os.AppendString( m_pcData, m_uiLen );
}

void UnknownMessage::Read( FvBinaryIStream &is )
{
	FvNetMachineGuardMessage::Read( is );
	m_uiLen = is.RemainingLength();
	delete [] m_pcData;
	m_pcData = new char[ m_uiLen ];
	strncpy_s( m_pcData, m_uiLen, (char*)is.Retrieve( m_uiLen ), m_uiLen );
	m_uiFlags |= MESSAGE_NOT_UNDERSTOOD;
}

const char *UnknownMessage::c_str() const
{
	FvSNPrintf( FvNetMachineGuardMessage::ms_acBuf, sizeof(FvNetMachineGuardMessage::ms_acBuf),
		"id %d, %d bytes long", m_uiMessage, m_uiLen );
	return FvNetMachineGuardMessage::ms_acBuf;
}
