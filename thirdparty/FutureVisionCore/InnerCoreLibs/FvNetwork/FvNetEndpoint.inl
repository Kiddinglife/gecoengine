#ifdef PLAYSTATION3
#include <netex/libnetctl.h>
#endif

FV_INLINE FvNetEndpoint::~FvNetEndpoint()
{
	this->Close();
}

FV_INLINE FvNetEndpoint::operator int() const
{
	return (int)m_kSocket;
}

FV_INLINE void FvNetEndpoint::SetFileDescriptor( int fd )
{
	if ((m_kSocket != NO_SOCKET) && FvNetEndpoint::IsHijacked())
	{
		this->HijackSendClose();
	}

	m_kSocket = fd;
}

FV_INLINE bool FvNetEndpoint::Good() const
{
	return (m_kSocket != NO_SOCKET);
}

FV_INLINE void FvNetEndpoint::Socket(int type)
{
	this->SetFileDescriptor( (int)::socket( AF_INET, type, 0 ) );
#ifndef unix
#ifndef PLAYSTATION3
	if ((m_kSocket == INVALID_SOCKET) && (WSAGetLastError() == WSANOTINITIALISED))
	{
		InitNetwork();

		this->SetFileDescriptor( (int)::socket( AF_INET, type, 0 ) );
	}
#endif
#endif
}

FV_INLINE int FvNetEndpoint::SetNonblocking(bool nonblocking)
{
#ifdef unix
	int val = nonblocking ? O_NONBLOCK : 0;
	return ::fcntl(m_kSocket,F_SETFL,val);
#elif defined( PLAYSTATION3 )
	int val = nonblocking ? 1 : 0;
	return setsockopt( m_kSocket, SOL_SOCKET, SO_NBIO, &val, sizeof(int) );
#else
	u_long val = nonblocking ? 1 : 0;
	return ::ioctlsocket(m_kSocket,FIONBIO,&val);
#endif
}

FV_INLINE int FvNetEndpoint::SetBroadcast(bool broadcast)
{
#ifdef unix
	int val;
	if (broadcast)
	{
		val = 2;
		::setsockopt( m_kSocket, SOL_IP, IP_MULTICAST_TTL, &val, sizeof(int) );
	}
#else
	bool val;
#endif
	val = broadcast ? 1 : 0;
	return ::setsockopt(m_kSocket,SOL_SOCKET,SO_BROADCAST,
		(char*)&val,sizeof(val));
}

FV_INLINE int FvNetEndpoint::SetReuseaddr(bool reuseaddr)
{
#ifdef unix
	int val;
#else
	bool val;
#endif
	val = reuseaddr ? 1 : 0;
	return ::setsockopt(m_kSocket,SOL_SOCKET,SO_REUSEADDR,
		(char*)&val,sizeof(val));
}
FV_INLINE int FvNetEndpoint::SetKeepalive(bool keepalive)
{
#ifdef unix
	int val;
#else
	bool val;
#endif
	val = keepalive ? 1 : 0;
	return ::setsockopt(m_kSocket,SOL_SOCKET,SO_KEEPALIVE,
		(char*)&val,sizeof(val));
}

FV_INLINE int FvNetEndpoint::Bind(u_int16_t networkPort, u_int32_t networkAddr)
{
	if (FvNetEndpoint::IsHijacked())
	{
		this->HijackSendOpen( networkPort, networkAddr );

		sockaddr_in	sin;
		sin.sin_family = AF_INET;
		sin.sin_port = 0;
		sin.sin_addr.s_addr = htonl(0x7f000001u);
		FV_VERIFY(
			::bind( m_kSocket, (struct sockaddr*)&sin, sizeof(sin) ) == 0 );

		return 0;
	}

	sockaddr_in	sin;
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;
	return ::bind( m_kSocket, (struct sockaddr*)&sin, sizeof(sin) );
}

FV_INLINE int FvNetEndpoint::JoinMulticastGroup( u_int32_t networkAddr )
{
#ifdef unix
	struct ip_mreqn req;
	req.imr_multiaddr.s_addr = networkAddr;
	req.imr_address.s_addr = INADDR_ANY;
	req.imr_ifindex = 0;
	return ::setsockopt( m_kSocket, SOL_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req));
#else
	return -1;
#endif
}

FV_INLINE int FvNetEndpoint::QuitMulticastGroup( u_int32_t networkAddr )
{
#ifdef unix
	struct ip_mreqn req;
	req.imr_multiaddr.s_addr = networkAddr;
	req.imr_address.s_addr = INADDR_ANY;
	req.imr_ifindex = 0;
	return ::setsockopt( m_kSocket, SOL_IP, IP_DROP_MEMBERSHIP,&req, sizeof(req));
#else
	return -1;
#endif
}

FV_INLINE int FvNetEndpoint::Close()
{
	if (m_kSocket == NO_SOCKET)
	{
		return 0;
	}

#ifdef unix
	int ret = ::close(m_kSocket);
#elif defined( PLAYSTATION3 )
	int ret = ::socketclose(m_kSocket);
#else
	int ret = ::closesocket(m_kSocket);
#endif
	if (ret == 0)
	{
		this->SetFileDescriptor( NO_SOCKET );
	}
	return ret;
}

FV_INLINE int FvNetEndpoint::Detach()
{
	int ret = (int)m_kSocket;
	this->SetFileDescriptor( NO_SOCKET );
	return ret;
}

FV_INLINE int FvNetEndpoint::GetLocalAddress(
	u_int16_t * networkPort, u_int32_t * networkAddr ) const
{
	if (m_pkHijackEndpoint)
	{
		FV_ASSERT( !m_bUseSyncHijack );

		if (networkPort != NULL) *networkPort = m_uiHijackPort;
		if (networkAddr != NULL) *networkAddr = m_uiHijackAddr;
		return 0;
	}

	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);
	int ret = ::getsockname( m_kSocket, (struct sockaddr*)&sin, &sinLen );
	if (ret == 0)
	{
		if (networkPort != NULL) *networkPort = sin.sin_port;
		if (networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
	}
	return ret;
}


FV_INLINE int FvNetEndpoint::GetRemoteAddress(
	u_int16_t * networkPort, u_int32_t * networkAddr ) const
{
	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);
	int ret = ::getpeername( m_kSocket, (struct sockaddr*)&sin, &sinLen );
	if (ret == 0)
	{
		if (networkPort != NULL) *networkPort = sin.sin_port;
		if (networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
	}
	return ret;
}


FV_INLINE const char * FvNetEndpoint::c_str() const
{
	FvNetAddress addr;
	this->GetLocalAddress( &(u_int16_t&)addr.m_uiPort, &(u_int32_t&)addr.m_uiIP );
	return addr.c_str();
}

FV_INLINE int FvNetEndpoint::GetRemoteHostname( FvString * host ) const
{
	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);
	int ret = ::getpeername( m_kSocket, (struct sockaddr*)&sin, &sinLen );
	if (ret == 0)
	{
		hostent* h = gethostbyaddr( (char*) &sin.sin_addr,
				sizeof( sin.sin_addr ), AF_INET);

		if (h)
			*host = h->h_name;
		else
			ret = -1;
	}
	return ret;
}


FV_INLINE void FvNetEndpoint::HijackSendOpen( u_int16_t port, u_int32_t addr )
{
	FV_ASSERT( FvNetEndpoint::IsHijacked());


	FV_ASSERT( !m_bShouldSendClose );
	m_bShouldSendClose = true;

	FvInt16 msgType = HIJACK_MSG_OPEN_UDP;
	FvInt32 fileDescriptor = int(m_kSocket);
	FvInt8 isSync = m_bUseSyncHijack;

	m_pkHijackEndpoint =
		m_bUseSyncHijack ?  ms_pkHijackEndpointSync : ms_pkHijackEndpointAsync;

	ms_pkHijackEndpointSync->Send( &msgType, sizeof( msgType ) );
	ms_pkHijackEndpointSync->Send( &fileDescriptor, sizeof( fileDescriptor ) );
	ms_pkHijackEndpointSync->Send( &isSync, sizeof( isSync ) );
	ms_pkHijackEndpointSync->Send( &port, sizeof( port ) );
	ms_pkHijackEndpointSync->Send( &addr, sizeof( addr ) );

	if (!m_bUseSyncHijack)
	{
		FvInt32 fd;

		if (!ms_pkHijackEndpointSync->RecvAll( &msgType, sizeof( msgType ) ) ||
			(msgType != HIJACK_MSG_OPEN_UDP) ||
			!ms_pkHijackEndpointSync->RecvAll( &fd, sizeof( fd ) ) ||
			(fd != m_kSocket) ||
			!ms_pkHijackEndpointSync->RecvAll( &m_uiHijackAddr, sizeof( m_uiHijackAddr ) ) ||
			!ms_pkHijackEndpointSync->RecvAll( &m_uiHijackPort, sizeof( m_uiHijackPort ) ) )
		{
		}
	}
}


FV_INLINE void FvNetEndpoint::HijackSendClose()
{
	FV_ASSERT( ms_pkHijackEndpointSync );
	if (m_bShouldSendClose)
	{
		m_bShouldSendClose = false;
		FvInt32 fileDescriptor = int(m_kSocket);
		FvInt16 msgType = HIJACK_MSG_CLOSE_UDP;
		ms_pkHijackEndpointSync->Send( &msgType, sizeof( msgType ) );
		ms_pkHijackEndpointSync->Send( &fileDescriptor, sizeof( fileDescriptor ) );
	}
}


FV_INLINE int FvNetEndpoint::SendTo( void * gramData, int gramSize,
	u_int16_t networkPort, u_int32_t networkAddr )
{
	sockaddr_in	sin;
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;

	return this->SendTo( gramData, gramSize, sin );
}

FV_INLINE int FvNetEndpoint::SendTo( void * gramData, int gramSize,
	struct sockaddr_in & sin )
{
	if (m_pkHijackEndpoint)
	{
		FvInt32 fileDescriptor = int(m_kSocket);
		FvInt16 msgType = HIJACK_MSG_UDP;
		m_pkHijackEndpoint->Send( &msgType, sizeof( msgType ) );
		m_pkHijackEndpoint->Send( &fileDescriptor, sizeof( fileDescriptor ) );
		m_pkHijackEndpoint->Send( &sin.sin_addr.s_addr, 4 );
		m_pkHijackEndpoint->Send( &sin.sin_port, 2 );
		m_pkHijackEndpoint->Send( &gramSize, sizeof( FvInt32 ) );
		return m_pkHijackEndpoint->Send( gramData, gramSize );
	}

	return ::sendto( m_kSocket, (char*)gramData, gramSize,
		0, (sockaddr*)&sin, sizeof(sin) );
}


FV_INLINE int FvNetEndpoint::RecvFrom( void * gramData, int gramSize,
	u_int16_t * networkPort, u_int32_t * networkAddr )
{
	sockaddr_in sin;
	int result = this->RecvFrom( gramData, gramSize, sin );

	if (result >= 0)
	{
		if (networkPort != NULL) *networkPort = sin.sin_port;
		if (networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
	}

	return result;
}


FV_INLINE int FvNetEndpoint::RecvFrom( void * gramData, int gramSize,
	struct sockaddr_in & sin )
{
	if (ms_pkHijackStream)
	{
		// If there was an artificial packet waiting for us, use it instead.
		u_int32_t addr;
		u_int16_t port;
		(*ms_pkHijackStream) >> (FvUInt32&)addr >> (FvUInt16&)port;

		sin.sin_port = port;
		sin.sin_addr.s_addr = addr;

		if (ms_pkHijackStream->Error())
		{
			return -1;
		}

		int len = std::min( gramSize, ms_pkHijackStream->RemainingLength() );

		::memcpy( gramData, ms_pkHijackStream->Retrieve( len ), len );

		FV_ASSERT( len >= 0 );

		return len;
	}
	else if (ms_pkHijackEndpointSync)
	{
#ifdef _WIN32
		WSASetLastError( WSAEWOULDBLOCK );
#else
		errno = EAGAIN;
#endif
		return -1;
	}

	socklen_t		sinLen = sizeof(sin);
	int ret = ::recvfrom( m_kSocket, (char*)gramData, gramSize,
		0, (sockaddr*)&sin, &sinLen);

	return ret;
}

FV_INLINE int FvNetEndpoint::Listen( int backlog )
{
	return ::listen( m_kSocket, backlog );
}

FV_INLINE int FvNetEndpoint::Connect( u_int16_t networkPort, u_int32_t networkAddr )
{
	sockaddr_in	sin;
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;

	return ::connect( m_kSocket, (sockaddr*)&sin, sizeof(sin) );
}

FV_INLINE FvNetEndpoint * FvNetEndpoint::Accept(
	u_int16_t * networkPort, u_int32_t * networkAddr )
{
	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);
	int ret = (int)::accept( m_kSocket, (sockaddr*)&sin, &sinLen);
#if defined( unix ) || defined( PLAYSTATION3 )
	if (ret < 0) return NULL;
#else
	if (ret == INVALID_SOCKET) return NULL;
#endif

	FvNetEndpoint * pNew = new FvNetEndpoint();
	pNew->SetFileDescriptor( ret );

	if (networkPort != NULL) *networkPort = sin.sin_port;
	if (networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;

	return pNew;
}


FV_INLINE int FvNetEndpoint::Send( const void * gramData, int gramSize )
{
	return ::send( m_kSocket, (char*)gramData, gramSize, 0 );
}


FV_INLINE int FvNetEndpoint::Recv( void * gramData, int gramSize )
{
	if (ms_pkHijackStream)
	{
		int len = std::min( gramSize, ms_pkHijackStream->RemainingLength() );
		::memcpy( gramData, ms_pkHijackStream->Retrieve( len ), len );

		return len;
	}

	return ::recv( m_kSocket, (char*)gramData, gramSize, 0 );
}


#ifdef unix
FV_INLINE int FvNetEndpoint::GetInterfaceFlags( char * name, int & flags )
{
	FV_ASSERT( !FvNetEndpoint::IsHijacked() );

	struct ifreq	request;

	strncpy( request.ifr_name, name, IFNAMSIZ );
	if (ioctl( m_kSocket, SIOCGIFFLAGS, &request ) != 0)
	{
		return -1;
	}

	flags = request.ifr_flags;
	return 0;
}

FV_INLINE int FvNetEndpoint::GetInterfaceAddress( const char * name, u_int32_t & address )
{
	if (FvNetEndpoint::IsHijacked())
	{
		FrontEndInterfaces::iterator iter = ms_kFrontEndInterfaces.find( name );

		if (iter != ms_kFrontEndInterfaces.end())
		{
			address = iter->second;
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		struct ifreq	request;

		strncpy( request.ifr_name, name, IFNAMSIZ );
		if (ioctl( m_kSocket, SIOCGIFADDR, &request ) != 0)
		{
			return -1;
		}

		if (request.ifr_addr.sa_family == AF_INET)
		{
			address = ((sockaddr_in*)&request.ifr_addr)->sin_addr.s_addr;
			return 0;
		}
		else
		{
			return -1;
		}
	}
}

FV_INLINE int FvNetEndpoint::GetInterfaceNetmask( const char * name,
	u_int32_t & netmask )
{
	struct ifreq request;
	strncpy( request.ifr_name, name, IFNAMSIZ );

	if (ioctl( m_kSocket, SIOCGIFNETMASK, &request ) != 0)
	{
		return -1;
	}

	netmask = ((sockaddr_in&)request.ifr_netmask).sin_addr.s_addr;

	return 0;
}

#else

#ifndef _WIN32

FV_INLINE int FvNetEndpoint::GetInterfaceFlags( char * name, int & flags )
{
	if (!strcmp(name,"eth0"))
	{
		flags = IFF_UP | IFF_BROADCAST | IFF_NOTRAILERS |
			IFF_RUNNING | IFF_MULTICAST;
		return 0;
	}
	else if (!strcmp(name, FV_NET_LOCALHOSTNAME))
	{
		flags = IFF_UP | IFF_LOOPBACK | IFF_RUNNING;
		return 0;
	}
	return -1;
}

FV_INLINE int FvNetEndpoint::GetInterfaceAddress( const char * name, u_int32_t & address )
{
	if (!strcmp(name,"eth0"))
	{
#if defined( PLAYSTATION3 )
		CellNetCtlInfo netInfo;
		int ret = cellNetCtlGetInfo( CELL_NET_CTL_INFO_IP_ADDRESS, &netInfo );
		FV_ASSERT( ret == 0 );
		int ip0, ip1, ip2, ip3;
		sscanf( netInfo.ip_address, "%d.%d.%d.%d", &ip0, &ip1, &ip2, &ip3 );
		address = ( ip0 << 24 ) | ( ip1 << 16 ) | ( ip2 << 8 ) | ( ip3 << 0 );
#else
		char	myname[256];
		::gethostname(myname,sizeof(myname));

		struct hostent * myhost = gethostbyname(myname);
		if (!myhost)
		{
			return -1;
		}

		address = ((struct in_addr*)(myhost->h_addr_list[0]))->s_addr;
#endif
		return 0;
	}
	else if (!strcmp(name, FV_NET_LOCALHOSTNAME))
	{
		address = htonl(0x7F000001);
		return 0;
	}

	return -1;
}

#endif

#endif

FV_INLINE int FvNetEndpoint::TransmitQueueSize() const
{
	int tx=0, rx=0;
	this->GetQueueSizes( tx, rx );
	return tx;
}

FV_INLINE int FvNetEndpoint::ReceiveQueueSize() const
{
	int tx=0, rx=0;
	this->GetQueueSizes( tx, rx );
	return rx;
}
