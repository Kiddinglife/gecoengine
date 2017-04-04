//{future header message}
#ifndef __GecoNetEndpoint_H__
#define __GecoNetEndpoint_H__


#include "net-types.h"
#include "common/ds/eastl/EASTL/map.h"
#include "common/ds/eastl/EASTL/vector.h"

#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>

#if defined(__linux__ ) || defined( PLAYSTATION3 )
#include <sys/time.h>
#include <sys/socket.h>
#ifndef PLAYSTATION3
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unieastl.h>
#else
#include <WINSOCK.H>
#endif

#ifndef __linux__

#ifdef PLAYSTATION3
typedef uint8_t 	u_int8_t;
typedef uint16_t 	u_int16_t;
typedef uint32_t 	u_int32_t;
#else
#ifndef socklen_t
typedef int socklen_t;
#endif
typedef u_short u_int16_t;
typedef u_long u_int32_t;
#endif

#ifdef _WIN32
#define IFNAMSIZ 256
#else
#define IFNAMSIZ 16
#endif

#ifndef IFF_UP
enum
{
	IFF_UP = 0x1,
	IFF_BROADCAST = 0x2,
	IFF_DEBUG = 0x4,
	IFF_LOOPBACK = 0x8,
	IFF_POINTOPOINT = 0x10,
	IFF_NOTRAILERS = 0x20,
	IFF_RUNNING = 0x40,
	IFF_NOARP = 0x80,
	IFF_PROMISC = 0x100,
	IFF_MULTICAST = 0x1000
};
#endif

#endif

class geco_bit_stream_t;

class GECOAPI GecoNetEndpoint
{
public:
	GecoNetEndpoint(bool useSyncHijack = true);
	~GecoNetEndpoint();

	static const int NO_SOCKET = -1;

	operator int() const;
	void SetFileDescriptor(int fd);
	bool Good() const;

	void Socket(int type);

	int SetNonblocking(bool nonblocking);
	int SetBroadcast(bool broadcast);
	int SetReuseaddr(bool reuseaddr);
	int SetKeepalive(bool keepalive);

	int Bind(u_int16_t networkPort = 0, u_int32_t networkAddr = INADDR_ANY);

	int JoinMulticastGroup(u_int32_t networkAddr);
	int QuitMulticastGroup(u_int32_t networkAddr);

	int Close();
	int Detach();

	int GetLocalAddress(
		u_int16_t * networkPort, u_int32_t * networkAddr) const;
	int GetRemoteAddress(
		u_int16_t * networkPort, u_int32_t * networkAddr) const;

	const char * c_str() const;
	int GetRemoteHostname(eastl::string * name) const;

	bool GetClosedPort(GecoNetAddress & closedPort);

	int SendTo(void * gramData, int gramSize,
		u_int16_t networkPort, u_int32_t networkAddr = GECO_NET_BROADCAST);
	int SendTo(void * gramData, int gramSize, struct sockaddr_in & sin);
	int RecvFrom(void * gramData, int gramSize,
		u_int16_t * networkPort, u_int32_t * networkAddr);
	int RecvFrom(void * gramData, int gramSize,
		struct sockaddr_in & sin);

	int Listen(int backlog = 5);
	int Connect(u_int16_t networkPort, u_int32_t networkAddr = GECO_NET_BROADCAST);
	GecoNetEndpoint * Accept(
		u_int16_t * networkPort = NULL, u_int32_t * networkAddr = NULL);

	int Send(const void * gramData, int gramSize);
	int Recv(void * gramData, int gramSize);
	bool RecvAll(void * gramData, int gramSize);

	int GetInterfaceFlags(char * name, int & flags);
	int GetInterfaceAddress(const char * name, u_int32_t & address);
	int GetInterfaceNetmask(const char * name, u_int32_t & netmask);
	bool GetInterfaces(eastl::map< u_int32_t, eastl::string > &interfaces);
	int FindDefaultInterface(char * name);
	int FindIndicatedInterface(const char * spec, char * name);
	static int ConvertAddress(const char * string, u_int32_t & address);

	int TransmitQueueSize() const;
	int ReceiveQueueSize() const;
	int GetQueueSizes(int & tx, int & rx) const;

	int GetBufferSize(int optname) const;
	bool SetBufferSize(int optname, int size);

	enum
	{
		HIJACK_MSG_OPEN_TCP = 3,
		HIJACK_MSG_CLOSE_TCP = 4,
		HIJACK_MSG_OPEN_UDP = 5,
		HIJACK_MSG_CLOSE_UDP = 6,
		HIJACK_MSG_UDP = 7,
		HIJACK_MSG_TCP = 8,
		HIJACK_MSG_BWLOG = 9,
		HIJACK_MSG_INIT = 10,
	};

	static void SetHijackEndpoints(GecoNetEndpoint * pSync, GecoNetEndpoint * pAsync);
	static void SetHijackStream(geco_bit_stream_t * pNewStream)
	{
		ms_pkHijackStream = pNewStream;
	}

	void HijackSendOpen(u_int16_t port, u_int32_t addr);
	void HijackSendClose();
	bool HijackRecvAllFrom(void * gramData, int gramSize,
		u_int16_t * pPort, u_int32_t * pAddr);
	int HijackFD() const;

	static bool IsHijacked() { return ms_pkHijackEndpointAsync != NULL; }
	static void AddFrontEndInterface(const eastl::string & name,
		u_int32_t addr);

private:

#if defined( __linux__ ) || defined( PLAYSTATION3 )
	int	m_kSocket;
#else //ifdef __linux__
	SOCKET	m_kSocket;
#endif //def _WIN32

	bool m_bUseSyncHijack;

	bool m_bShouldSendClose;

	u_int16_t m_uiHijackPort;
	u_int32_t m_uiHijackAddr;

	GecoNetEndpoint *m_pkHijackEndpoint;

	static GecoNetEndpoint *ms_pkHijackEndpointAsync;
	static GecoNetEndpoint *ms_pkHijackEndpointSync;

	static geco_bit_stream_t *ms_pkHijackStream;

	typedef eastl::map< eastl::string, u_int32_t > FrontEndInterfaces;
	static FrontEndInterfaces ms_kFrontEndInterfaces;
};

extern void InitNetwork();

#ifdef PLAYSTATION3
#include <netex/libnetctl.h>
#endif

#include <algorithm>

INLINE GecoNetEndpoint::~GecoNetEndpoint()
{
	this->Close();
}

INLINE GecoNetEndpoint::operator int() const
{
	return (int)m_kSocket;
}

INLINE void GecoNetEndpoint::SetFileDescriptor(int fd)
{
	if ((m_kSocket != NO_SOCKET) && GecoNetEndpoint::IsHijacked())
	{
		this->HijackSendClose();
	}

	m_kSocket = fd;
}

INLINE bool GecoNetEndpoint::Good() const
{
	return (m_kSocket != NO_SOCKET);
}

INLINE void GecoNetEndpoint::Socket(int type)
{
	this->SetFileDescriptor((int)::socket(AF_INET, type, 0));
#ifndef __linux__
#ifndef PLAYSTATION3
	if ((m_kSocket == INVALID_SOCKET) && (WSAGetLastError() == WSANOTINITIALISED))
	{
		InitNetwork();

		this->SetFileDescriptor((int)::socket(AF_INET, type, 0));
	}
#endif
#endif
}

INLINE int GecoNetEndpoint::SetNonblocking(bool nonblocking)
{
#ifdef __linux__
	int val = nonblocking ? O_NONBLOCK : 0;
	return ::fcntl(m_kSocket, F_SETFL, val);
#elif defined( PLAYSTATION3 )
	int val = nonblocking ? 1 : 0;
	return setsockopt(m_kSocket, SOL_SOCKET, SO_NBIO, &val, sizeof(int));
#else
	u_long val = nonblocking ? 1 : 0;
	return ::ioctlsocket(m_kSocket, FIONBIO, &val);
#endif
}

INLINE int GecoNetEndpoint::SetBroadcast(bool broadcast)
{
#ifdef __linux__
	int val;
	if (broadcast)
	{
		val = 2;
		::setsockopt(m_kSocket, SOL_IP, IP_MULTICAST_TTL, &val, sizeof(int));
	}
#else
	bool val;
#endif
	val = broadcast ? 1 : 0;
	return ::setsockopt(m_kSocket, SOL_SOCKET, SO_BROADCAST,
		(char*)&val, sizeof(val));
}

INLINE int GecoNetEndpoint::SetReuseaddr(bool reuseaddr)
{
#ifdef __linux__
	int val;
#else
	bool val;
#endif
	val = reuseaddr ? 1 : 0;
	return ::setsockopt(m_kSocket, SOL_SOCKET, SO_REUSEADDR,
		(char*)&val, sizeof(val));
}
INLINE int GecoNetEndpoint::SetKeepalive(bool keepalive)
{
#ifdef __linux__
	int val;
#else
	bool val;
#endif
	val = keepalive ? 1 : 0;
	return ::setsockopt(m_kSocket, SOL_SOCKET, SO_KEEPALIVE,
		(char*)&val, sizeof(val));
}

INLINE int GecoNetEndpoint::Bind(u_int16_t networkPort, u_int32_t networkAddr)
{
	if (GecoNetEndpoint::IsHijacked())
	{
		this->HijackSendOpen(networkPort, networkAddr);

		sockaddr_in	sin;
		sin.sin_family = AF_INET;
		sin.sin_port = 0;
		sin.sin_addr.s_addr = htonl(0x7f000001u);
		assert(::bind(m_kSocket, (struct sockaddr*)&sin, sizeof(sin)) == 0);

		return 0;
	}

	sockaddr_in	sin;
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;
	return ::bind(m_kSocket, (struct sockaddr*)&sin, sizeof(sin));
}

INLINE int GecoNetEndpoint::JoinMulticastGroup(u_int32_t networkAddr)
{
#ifdef __linux__
	struct ip_mreqn req;
	req.imr_multiaddr.s_addr = networkAddr;
	req.imr_address.s_addr = INADDR_ANY;
	req.imr_ifindex = 0;
	return ::setsockopt(m_kSocket, SOL_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req));
#else
	return -1;
#endif
}

INLINE int GecoNetEndpoint::QuitMulticastGroup(u_int32_t networkAddr)
{
#ifdef __linux__
	struct ip_mreqn req;
	req.imr_multiaddr.s_addr = networkAddr;
	req.imr_address.s_addr = INADDR_ANY;
	req.imr_ifindex = 0;
	return ::setsockopt(m_kSocket, SOL_IP, IP_DROP_MEMBERSHIP, &req, sizeof(req));
#else
	return -1;
#endif
}

INLINE int GecoNetEndpoint::Close()
{
	if (m_kSocket == NO_SOCKET)
	{
		return 0;
	}

#ifdef __linux__
	int ret = ::close(m_kSocket);
#elif defined( PLAYSTATION3 )
	int ret = ::socketclose(m_kSocket);
#else
	int ret = ::closesocket(m_kSocket);
#endif
	if (ret == 0)
	{
		this->SetFileDescriptor(NO_SOCKET);
	}
	return ret;
}

INLINE int GecoNetEndpoint::Detach()
{
	int ret = (int)m_kSocket;
	this->SetFileDescriptor(NO_SOCKET);
	return ret;
}

INLINE int GecoNetEndpoint::GetLocalAddress(
	u_int16_t * networkPort, u_int32_t * networkAddr) const
{
	if (m_pkHijackEndpoint)
	{
		assert(!m_bUseSyncHijack);

		if (networkPort != NULL) *networkPort = m_uiHijackPort;
		if (networkAddr != NULL) *networkAddr = m_uiHijackAddr;
		return 0;
	}

	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);
	int ret = ::getsockname(m_kSocket, (struct sockaddr*)&sin, &sinLen);
	if (ret == 0)
	{
		if (networkPort != NULL) *networkPort = sin.sin_port;
		if (networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
	}
	return ret;
}


INLINE int GecoNetEndpoint::GetRemoteAddress(
	u_int16_t * networkPort, u_int32_t * networkAddr) const
{
	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);
	int ret = ::getpeername(m_kSocket, (struct sockaddr*)&sin, &sinLen);
	if (ret == 0)
	{
		if (networkPort != NULL) *networkPort = sin.sin_port;
		if (networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
	}
	return ret;
}


INLINE const char * GecoNetEndpoint::c_str() const
{
	GecoNetAddress addr;
	this->GetLocalAddress(&(u_int16_t&)addr.m_uiPort, &(u_int32_t&)addr.m_uiIP);
	return addr.c_str();
}

INLINE int GecoNetEndpoint::GetRemoteHostname(eastl::string * host) const
{
	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);
	int ret = ::getpeername(m_kSocket, (struct sockaddr*)&sin, &sinLen);
	if (ret == 0)
	{
		hostent* h = gethostbyaddr((char*)&sin.sin_addr,
			sizeof(sin.sin_addr), AF_INET);

		if (h)
			*host = h->h_name;
		else
			ret = -1;
	}
	return ret;
}

INLINE void GecoNetEndpoint::HijackSendOpen(u_int16_t port, u_int32_t addr)
{
	assert(GecoNetEndpoint::IsHijacked());


	assert(!m_bShouldSendClose);
	m_bShouldSendClose = true;

	short msgType = HIJACK_MSG_OPEN_UDP;
	int fileDescriptor = int(m_kSocket);
	char isSync = m_bUseSyncHijack;

	m_pkHijackEndpoint =
		m_bUseSyncHijack ? ms_pkHijackEndpointSync : ms_pkHijackEndpointAsync;

	ms_pkHijackEndpointSync->Send(&msgType, sizeof(msgType));
	ms_pkHijackEndpointSync->Send(&fileDescriptor, sizeof(fileDescriptor));
	ms_pkHijackEndpointSync->Send(&isSync, sizeof(isSync));
	ms_pkHijackEndpointSync->Send(&port, sizeof(port));
	ms_pkHijackEndpointSync->Send(&addr, sizeof(addr));

	if (!m_bUseSyncHijack)
	{
		int fd;

		if (!ms_pkHijackEndpointSync->RecvAll(&msgType, sizeof(msgType)) ||
			(msgType != HIJACK_MSG_OPEN_UDP) ||
			!ms_pkHijackEndpointSync->RecvAll(&fd, sizeof(fd)) ||
			(fd != m_kSocket) ||
			!ms_pkHijackEndpointSync->RecvAll(&m_uiHijackAddr, sizeof(m_uiHijackAddr)) ||
			!ms_pkHijackEndpointSync->RecvAll(&m_uiHijackPort, sizeof(m_uiHijackPort)))
		{
		}
	}
}


INLINE void GecoNetEndpoint::HijackSendClose()
{
	assert(ms_pkHijackEndpointSync);
	if (m_bShouldSendClose)
	{
		m_bShouldSendClose = false;
		int fileDescriptor = int(m_kSocket);
		short msgType = HIJACK_MSG_CLOSE_UDP;
		ms_pkHijackEndpointSync->Send(&msgType, sizeof(msgType));
		ms_pkHijackEndpointSync->Send(&fileDescriptor, sizeof(fileDescriptor));
	}
}


INLINE int GecoNetEndpoint::SendTo(void * gramData, int gramSize,
	u_int16_t networkPort, u_int32_t networkAddr)
{
	sockaddr_in	sin;
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;

	return this->SendTo(gramData, gramSize, sin);
}

INLINE int GecoNetEndpoint::SendTo(void * gramData, int gramSize,
	struct sockaddr_in & sin)
{
	if (m_pkHijackEndpoint)
	{
		int fileDescriptor = int(m_kSocket);
		short msgType = HIJACK_MSG_UDP;
		m_pkHijackEndpoint->Send(&msgType, sizeof(msgType));
		m_pkHijackEndpoint->Send(&fileDescriptor, sizeof(fileDescriptor));
		m_pkHijackEndpoint->Send(&sin.sin_addr.s_addr, 4);
		m_pkHijackEndpoint->Send(&sin.sin_port, 2);
		m_pkHijackEndpoint->Send(&gramSize, sizeof(int));
		return m_pkHijackEndpoint->Send(gramData, gramSize);
	}

	return ::sendto(m_kSocket, (char*)gramData, gramSize,
		0, (sockaddr*)&sin, sizeof(sin));
}


INLINE int GecoNetEndpoint::RecvFrom(void * gramData, int gramSize,
	u_int16_t * networkPort, u_int32_t * networkAddr)
{
	sockaddr_in sin;
	int result = this->RecvFrom(gramData, gramSize, sin);

	if (result >= 0)
	{
		if (networkPort != NULL) *networkPort = sin.sin_port;
		if (networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
	}

	return result;
}


INLINE int GecoNetEndpoint::RecvFrom(void * gramData, int gramSize,
	struct sockaddr_in & sin)
{
	if (ms_pkHijackStream)
	{
		// If there was an artificial packet waiting for us, use it instead.
		u_int32_t addr;
		u_int16_t port;
		(*ms_pkHijackStream) >> (uint&)addr >> (ushort&)port;

		sin.sin_port = port;
		sin.sin_addr.s_addr = addr;

		int len = GECO_MIN(gramSize, BITS_TO_BYTES(ms_pkHijackStream->get_payloads()));
		ms_pkHijackStream->ReadRaw((uchar*)gramData, len);

		return len;
	}
	else if (ms_pkHijackEndpointSync)
	{
#ifdef _WIN32
		WSASetLastError(WSAEWOULDBLOCK);
#else
		errno = EAGAIN;
#endif
		return -1;
	}

	socklen_t		sinLen = sizeof(sin);
	int ret = ::recvfrom(m_kSocket, (char*)gramData, gramSize,
		0, (sockaddr*)&sin, &sinLen);

	return ret;
}

INLINE int GecoNetEndpoint::Listen(int backlog)
{
	return ::listen(m_kSocket, backlog);
}

INLINE int GecoNetEndpoint::Connect(u_int16_t networkPort, u_int32_t networkAddr)
{
	sockaddr_in	sin;
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;

	return ::connect(m_kSocket, (sockaddr*)&sin, sizeof(sin));
}

INLINE GecoNetEndpoint * GecoNetEndpoint::Accept(
	u_int16_t * networkPort, u_int32_t * networkAddr)
{
	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);
	int ret = (int)::accept(m_kSocket, (sockaddr*)&sin, &sinLen);
#if defined( __linux__ ) || defined( PLAYSTATION3 )
	if (ret < 0) return NULL;
#else
	if (ret == INVALID_SOCKET) return NULL;
#endif

	GecoNetEndpoint * pNew = new GecoNetEndpoint();
	pNew->SetFileDescriptor(ret);

	if (networkPort != NULL) *networkPort = sin.sin_port;
	if (networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;

	return pNew;
}


INLINE int GecoNetEndpoint::Send(const void * gramData, int gramSize)
{
	return ::send(m_kSocket, (char*)gramData, gramSize, 0);
}


INLINE int GecoNetEndpoint::Recv(void * gramData, int gramSize)
{
	if (ms_pkHijackStream)
	{
		int len = GECO_MIN(gramSize, BITS_TO_BYTES(ms_pkHijackStream->get_payloads()));
		ms_pkHijackStream->ReadRaw((uchar*)gramData, len);
		return len;
	}

	return ::recv(m_kSocket, (char*)gramData, gramSize, 0);
}


#ifdef __linux__
INLINE int GecoNetEndpoint::GetInterfaceFlags(char * name, int & flags)
{
	assert(!GecoNetEndpoint::IsHijacked());

	struct ifreq	request;

	strncpy(request.ifr_name, name, IFNAMSIZ);
	if (ioctl(m_kSocket, SIOCGIFFLAGS, &request) != 0)
	{
		return -1;
	}

	flags = request.ifr_flags;
	return 0;
}

INLINE int GecoNetEndpoint::GetInterfaceAddress(const char * name, u_int32_t & address)
{
	if (GecoNetEndpoint::IsHijacked())
	{
		FrontEndInterfaces::iterator iter = ms_kFrontEndInterfaces.find(name);

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

		strncpy(request.ifr_name, name, IFNAMSIZ);
		if (ioctl(m_kSocket, SIOCGIFADDR, &request) != 0)
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

INLINE int GecoNetEndpoint::GetInterfaceNetmask(const char * name,
	u_int32_t & netmask)
{
	struct ifreq request;
	strncpy(request.ifr_name, name, IFNAMSIZ);

	if (ioctl(m_kSocket, SIOCGIFNETMASK, &request) != 0)
	{
		return -1;
	}

	netmask = ((sockaddr_in&)request.ifr_netmask).sin_addr.s_addr;

	return 0;
}

#else

#ifndef _WIN32

INLINE int GecoNetEndpoint::GetInterfaceFlags(char * name, int & flags)
{
	if (!strcmp(name, "eth0"))
	{
		flags = IFF_UP | IFF_BROADCAST | IFF_NOTRAILERS |
			IFF_RUNNING | IFF_MULTICAST;
		return 0;
	}
	else if (!strcmp(name, GECO_NET_LOCALHOSTNAME))
	{
		flags = IFF_UP | IFF_LOOPBACK | IFF_RUNNING;
		return 0;
	}
	return -1;
}

INLINE int GecoNetEndpoint::GetInterfaceAddress(const char * name, u_int32_t & address)
{
	if (!strcmp(name, "eth0"))
	{
#if defined( PLAYSTATION3 )
		CellNetCtlInfo netInfo;
		int ret = cellNetCtlGetInfo(CELL_NET_CTL_INFO_IP_ADDRESS, &netInfo);
		assert(ret == 0);
		int ip0, ip1, ip2, ip3;
		sscanf(netInfo.ip_address, "%d.%d.%d.%d", &ip0, &ip1, &ip2, &ip3);
		address = (ip0 << 24) | (ip1 << 16) | (ip2 << 8) | (ip3 << 0);
#else
		char	myname[256];
		::gethostname(myname, sizeof(myname));

		struct hostent * myhost = gethostbyname(myname);
		if (!myhost)
		{
			return -1;
		}

		address = ((struct in_addr*)(myhost->h_addr_list[0]))->s_addr;
#endif
		return 0;
	}
	else if (!strcmp(name, GECO_NET_LOCALHOSTNAME))
	{
		address = htonl(0x7F000001);
		return 0;
	}

	return -1;
}

#endif

#endif

INLINE int GecoNetEndpoint::TransmitQueueSize() const
{
	int tx = 0, rx = 0;
	this->GetQueueSizes(tx, rx);
	return tx;
}

INLINE int GecoNetEndpoint::ReceiveQueueSize() const
{
	int tx = 0, rx = 0;
	this->GetQueueSizes(tx, rx);
	return rx;
}

#endif // __GecoNetEndpoint_H__
