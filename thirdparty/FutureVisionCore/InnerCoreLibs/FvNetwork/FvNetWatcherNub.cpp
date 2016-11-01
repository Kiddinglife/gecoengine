#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////#define FV_ENABLE_WATCHERS 1

#if FV_ENABLE_WATCHERS

#include "FvNetEndpoint.h"
#include "FvNetPortMap.h"
#include "FvNetMachineGuard.h"
#include "FvNetWatcherNub.h"
#include "FvNetWatcherPacketHandler.h"
#include "FvNetInterfaceMinder.h"
#include "FvNetMisc.h"

#include <FvProcess.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 )

FvNetWatcherNub::FvNetWatcherNub(FvUInt16 uiMachinePort) :
	m_iID(-1),
	m_bRegistered(false),
	m_pkWRHandler(NULL),
	m_bInsideReceiveRequest(false),
	m_pcRequestPacket(new char[FV_NET_WN_PACKET_SIZE]),
	m_bIsInitialised( false ),
	m_kSocket( /* useSyncHijack */ false ),
	m_uiMachinePort(uiMachinePort)
{

}

bool FvNetWatcherNub::Init( const char * listeningInterface, FvUInt16 listeningPort )
{
	FV_INFO_MSG( "FvNetWatcherNub::Init: listeningInterface = '%s', listeningPort = "
			"%hd\n", listeningInterface ? listeningInterface : "", listeningPort );
	if (m_bIsInitialised)
	{
		// WARNING_MSG( "WatcherNub::Init: Already initialised.\n" );
		return true;
	}

	m_bIsInitialised = true;

	m_kSocket.Socket(SOCK_DGRAM);
	if(!m_kSocket.Good())
	{
		FV_ERROR_MSG( "FvNetWatcherNub::Init: socket() failed\n" );
		return false;
	}

	if (m_kSocket.SetNonblocking(true))
	{
		FV_ERROR_MSG( "FvNetWatcherNub::Init: fcntl(O_NONBLOCK) failed\n" );
		return false;
	}

	u_int32_t ifaddr = INADDR_ANY;
#ifndef _WIN32
	if (inet_aton( listeningInterface, (struct in_addr *)&ifaddr ) == 0)
#endif
	{
		char ifname[IFNAMSIZ];
		bool listeningInterfaceEmpty =
			(listeningInterface == NULL || listeningInterface[0] == 0);
		if (m_kSocket.FindIndicatedInterface( listeningInterface, ifname ) == 0)
		{
			FV_INFO_MSG( "FvNetWatcherNub::Init: creating on interface '%s' (= %s)\n",
				listeningInterface, ifname );
			if (m_kSocket.GetInterfaceAddress( ifname, ifaddr ) != 0)
			{
				FV_WARNING_MSG( "FvNetWatcherNub::Init: couldn't get addr of interface %s "
					"so using all interfaces\n", ifname );
			}
		}
		else if (!listeningInterfaceEmpty)
		{
			FV_WARNING_MSG( "FvNetWatcherNub::Init: couldn't parse interface spec %s "
				"so using all interfaces\n", listeningInterface );
		}
	}

	if (m_kSocket.Bind( listeningPort, ifaddr ))
	{
		FV_ERROR_MSG( "FvNetWatcherNub::Init: bind() failed\n" );
		m_kSocket.Close();
		return false;
	}

	return true;
}

FvNetWatcherNub::~FvNetWatcherNub()
{
	if (m_bRegistered)
	{
		this->DeregisterWatcher();
	}

	if (m_kSocket.Good())
	{
		m_kSocket.Close();
	}

	if (m_pcRequestPacket != NULL)
	{
		delete [] m_pcRequestPacket;
		m_pcRequestPacket = NULL;
	}
}

int FvNetWatcherNub::RegisterWatcher( int id, const char *abrv, const char *longName,
	   const char * listeningInterface, FvUInt16 listeningPort )
{
	if (!this->Init( listeningInterface, listeningPort ))
	{
		FV_ERROR_MSG( "FvNetWatcherNub::RegisterWatcher: Init failed.\n" );
		return -1;
	}

	if (m_bRegistered)
		this->DeregisterWatcher();

	m_iID = id;

	strncpy( m_acABRV, abrv, sizeof(m_acABRV) );
	m_acABRV[sizeof(m_acABRV)-1]=0;

	strncpy( m_acName, longName, sizeof(m_acName) );
	m_acName[sizeof(m_acName)-1]=0;

	int ret = this->WatcherControlMessage(FV_NET_WATCHER_MSG_REGISTER,true);

	if (ret == 0)
	{
		m_bRegistered = true;
		this->NotifyMachineGuard();
	}
	return ret;
}

int FvNetWatcherNub::DeregisterWatcher()
{
	if (!m_bRegistered)
		return 0;
	else
	{
		int ret = this->WatcherControlMessage( FV_NET_WATCHER_MSG_DEREGISTER, true );
		if (ret == 0)
		{
			m_bRegistered = false;
			this->NotifyMachineGuard();
		}

		return ret;
	}
}

void FvNetWatcherNub::NotifyMachineGuard()
{
	u_int16_t port = 0;
	m_kSocket.GetLocalAddress( &port, NULL );

	FvNetProcessMessage pm;
	pm.m_uiParam = pm.PARAM_IS_MSGTYPE |
		(m_bRegistered ? pm.REGISTER : pm.DEREGISTER);
	pm.m_uiCategory = pm.WATCHER_NUB;
	pm.m_uiID = //FvGetUserID();
	pm.m_uiPID = FvGetPID();
	pm.m_uiPort = port;
	pm.m_uiID = m_iID;
	pm.m_kName = m_acABRV;

	FvUInt32 destip = htonl( 0x7F000001U );
	int reason;
	if ((reason = pm.SendAndRecv( 0, destip, m_uiMachinePort )) != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG( "Couldn't register watcher nub with machined: %s\n",
			NetReasonToString( (FvNetReason)reason ) );
	}
}

int FvNetWatcherNub::ResetServer()
{
	if(m_bRegistered)
		return -1;
	else
		return this->WatcherControlMessage( FV_NET_WATCHER_MSG_FLUSHCOMPONENTS, true );
}

int FvNetWatcherNub::WatcherControlMessage(int message, bool withid)
{
    int ret = 0;

	if (m_kSocket.SetBroadcast(true))
	{
		perror( "FvNetWatcherNub::WatcherControlMessage: "
                "setsockopt(SO_BROADCAST) failed\n" );
        ret = -1;
    }
    else
    {
        FvNetWatcherRegistrationMsg  wrm;
        wrm.m_iVersion = 0;
        wrm.m_iUID = //FvGetUserID();
        wrm.m_iMessage = message;

        if(withid)
        {
            wrm.m_iID = m_iID;
            strcpy(wrm.m_acABRV,m_acABRV);
            strcpy(wrm.m_acName,m_acName);
        }
        else
        {
            wrm.m_iID = -1;
            wrm.m_acABRV[0] = 0;
            wrm.m_acName[0] = 0;
        }

        if (m_kSocket.SendTo( &wrm,sizeof(wrm),
					htons( FV_NET_PORT_WATCHER ) ) != sizeof(wrm))
        {
            perror( "FvNetWatcherNub::WatcherControlMessage: sendto failed\n" );
            ret = -1;
        }

        if (m_kSocket.SetBroadcast( false ))
        {
            perror( "FvNetWatcherNub::WatcherControlMessage: "
                    "setsockopt(-SO_BROADCAST) failed\n" );
            ret = -1;
        }

    }

    return ret;
}

void FvNetWatcherNub::SetRequestHandler( FvNetWatcherRequestHandler *wrh )
{
	if (m_bInsideReceiveRequest)
	{
		FV_ERROR_MSG( "FvNetWatcherNub::SetRequestHandler: "
			"Can't call me while inside receiveRequest!\n" );
		return;
	}

	m_pkWRHandler = wrh;
}

int FvNetWatcherNub::GetSocketDescriptor()
{
	return m_kSocket;
}

bool FvNetWatcherNub::ReceiveRequest()
{
	if (!m_bIsInitialised)
	{
		return false;
	}

	sockaddr_in		senderAddr;
	int				len;

	if (m_pkWRHandler == NULL)
	{
		FV_ERROR_MSG( "FvNetWatcherNub::ReceiveRequest: Can't call me before\n"
			"\tcalling setRequestHandler(WatcherRequestHandler*)\n" );
		return false;
	}

	if (m_bInsideReceiveRequest)
	{
		FV_ERROR_MSG( "FvNetWatcherNub::ReceiveRequest: BAD THING NOTICED:\n"
			"\tAttempted re-entrant call to receiveRequest\n" );
		return false;
	}

	m_bInsideReceiveRequest = true;

	len = m_kSocket.RecvFrom( m_pcRequestPacket, FV_NET_WN_PACKET_SIZE, senderAddr );

	if (len == -1)
	{
		// EAGAIN = no packets waiting, ECONNREFUSED = rejected outgoing packet

#ifdef _WIN32
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK && err != WSAECONNREFUSED && err != WSAECONNRESET)
#else
		int err = errno;
		if (err != EAGAIN && err != ECONNREFUSED)
#endif
		{
			FV_ERROR_MSG( "FvNetWatcherNub::ReceiveRequest: recvfrom failed\n" );
		}

		m_bInsideReceiveRequest = false;
		return false;
	}

	FvNetWatcherDataMsg * wdm = (FvNetWatcherDataMsg*)m_pcRequestPacket;

	if (len < (int)sizeof(FvNetWatcherDataMsg))
	{
		FV_ERROR_MSG( "FvNetWatcherNub::ReceiveRequest: Packet is too short\n" );
		m_bInsideReceiveRequest = false;
		return false;
	}

	if (! (wdm->m_iMessage == FV_NET_WATCHER_MSG_GET ||
		   wdm->m_iMessage == FV_NET_WATCHER_MSG_GET_WITH_DESC ||
		   wdm->m_iMessage == FV_NET_WATCHER_MSG_SET ||
		   wdm->m_iMessage == FV_NET_WATCHER_MSG_GET2 ||
		   wdm->m_iMessage == FV_NET_WATCHER_MSG_SET2
		) )
	{
		m_pkWRHandler->ProcessExtensionMessage( wdm->m_iMessage,
								m_pcRequestPacket + sizeof( wdm->m_iMessage ),
								len - sizeof( wdm->m_iMessage ),
								FvNetAddress( senderAddr.sin_addr.s_addr,
									senderAddr.sin_port ) );
		m_bInsideReceiveRequest = false;
		return true;
	}

	FvNetWatcherPacketHandler *packetHandler = NULL;

	switch (wdm->m_iMessage)
	{
		case FV_NET_WATCHER_MSG_GET:
		case FV_NET_WATCHER_MSG_GET_WITH_DESC:
		{
			packetHandler = new FvNetWatcherPacketHandler( m_kSocket, senderAddr,
					wdm->m_iCount, FvNetWatcherPacketHandler::WP_VERSION_1, false );

			char	*astr = wdm->m_acString;
			for(int i=0;i<wdm->m_iCount;i++)
			{
				m_pkWRHandler->ProcessWatcherGetRequest( *packetHandler, astr,
					   	(wdm->m_iMessage == FV_NET_WATCHER_MSG_GET_WITH_DESC) );
				astr += strlen(astr)+1;
			}
		}
		break;

		case FV_NET_WATCHER_MSG_GET2:
		{
			packetHandler = new FvNetWatcherPacketHandler( m_kSocket, senderAddr,
					wdm->m_iCount, FvNetWatcherPacketHandler::WP_VERSION_2, false );

			char	*astr = wdm->m_acString;
			for(int i=0;i<wdm->m_iCount;i++)
			{
				unsigned int & seqNum = (unsigned int &)*astr;
				astr += sizeof(unsigned int);
				m_pkWRHandler->ProcessWatcherGet2Request( *packetHandler, astr, seqNum );
				astr += strlen(astr)+1;
			}
		}
		break;


		case FV_NET_WATCHER_MSG_SET:
		{
			packetHandler = new FvNetWatcherPacketHandler( m_kSocket, senderAddr,
					wdm->m_iCount, FvNetWatcherPacketHandler::WP_VERSION_1, true );

			char	*astr = wdm->m_acString;
			for(int i=0;i<wdm->m_iCount;i++)
			{
				char	*bstr = astr + strlen(astr)+1;
				m_pkWRHandler->ProcessWatcherSetRequest( *packetHandler, astr,bstr );
				astr = bstr + strlen(bstr)+1;
			}
		}
		break;


		case FV_NET_WATCHER_MSG_SET2:
		{
			packetHandler = new FvNetWatcherPacketHandler( m_kSocket, senderAddr,
					wdm->m_iCount, FvNetWatcherPacketHandler::WP_VERSION_2, true );

			char	*astr = wdm->m_acString;
			for(int i=0;i<wdm->m_iCount;i++)
			{
				m_pkWRHandler->ProcessWatcherSet2Request( *packetHandler, astr );
			}
		}
		break;

		default:
		{
			FvNetAddress srcAddr( senderAddr.sin_addr.s_addr,
									senderAddr.sin_port );
			FV_WARNING_MSG( "FvNetWatcherNub::ReceiveRequest: "
						"Unknown message %d from %s\n",
					wdm->m_iMessage, srcAddr.c_str() );
		}
		break;
	}

	if (packetHandler)
	{
		packetHandler->Run();
		packetHandler = NULL;
	}

	m_bInsideReceiveRequest = false;

	return true;
}


void FvNetWatcherRequestHandler::ProcessExtensionMessage( int messageID,
				char * data, int dataLen, const FvNetAddress & addr )
{
	FV_ERROR_MSG( "FvNetWatcherRequestHandler::ProcessExtensionMessage: "
							"Unknown message %d from %s. Message len = %d\n",
						messageID, (char *)addr, dataLen );
}

FvNetStandardWatcherRequestHandler::FvNetStandardWatcherRequestHandler(
		FvNetWatcherNub & nub ) : m_kNub( nub )
{
}

void FvNetStandardWatcherRequestHandler::ProcessWatcherGetRequest(
	FvNetWatcherPacketHandler & packetHandler, const char * path, bool withDesc )
{
#if FV_ENABLE_WATCHERS
	std::string newPath( path );
	FvWatcherPathRequestV1 *pRequest =
			(FvWatcherPathRequestV1 *)packetHandler.NewRequest( newPath );
	pRequest->UseDescription( withDesc );
#endif // FV_ENABLE_WATCHERS
}


void FvNetStandardWatcherRequestHandler::ProcessWatcherGet2Request(
	FvNetWatcherPacketHandler & packetHandler, const char * path, FvUInt32 seqNum )
{
#if FV_ENABLE_WATCHERS
	std::string newPath( path );
	FvWatcherPathRequestV2 *pRequest = (FvWatcherPathRequestV2 *)
								packetHandler.NewRequest( newPath );
	pRequest->SetSequenceNumber( seqNum );
#endif // FV_ENABLE_WATCHERS
}

void FvNetStandardWatcherRequestHandler::ProcessWatcherSetRequest(
		FvNetWatcherPacketHandler & packetHandler, const char * path,
		const char * valueString )
{
#if FV_ENABLE_WATCHERS
	std::string newPath( path );
	FvWatcherPathRequestV1 *pRequest =
			(FvWatcherPathRequestV1 *)packetHandler.NewRequest( newPath );

	pRequest->SetValueData( valueString );
#endif // FV_ENABLE_WATCHERS
}


void FvNetStandardWatcherRequestHandler::ProcessWatcherSet2Request(
		FvNetWatcherPacketHandler & packetHandler, char* & packet )
{
#if FV_ENABLE_WATCHERS
	FvUInt32 & seqNum = (FvUInt32 &)*packet;
	const char *path = packet + sizeof(FvUInt32);
	char *curr = (char *)path + strlen(path)+1;

	FvUInt32 size = 0;
	FvUInt8 sizeHint = (FvUInt8)*(curr+1); 
	if (sizeHint == 0xff)
	{
		size = FV_UNPACK3( (curr+2) );
		size += 5; 
	}
	else
	{
		size = sizeHint + 2;
	}

	std::string newPath( path );

	FvWatcherPathRequestV2 *pRequest = (FvWatcherPathRequestV2 *)
								packetHandler.NewRequest( newPath );
	pRequest->SetSequenceNumber( seqNum );

	pRequest->SetPacketData( size, curr );

	packet = curr + size;
#endif // FV_ENABLE_WATCHERS
}

#endif // FV_ENABLE_WATCHERS

