//{future header message}
#ifndef __GecoNetEndpoint_H__
#define __GecoNetEndpoint_H__

#include "net-types.h"
#include "common/ds/eastl/EASTL/map.h"
#include "common/ds/eastl/EASTL/vector.h"

class geco_bit_stream_t;

/**
 *	This class provides a wrapper around a socket.
 *
 *	@ingroup network
 */
typedef eastl::hash_map<GecoNetAddress, eastl::string, geco_net_addr_hash_functor, geco_net_addr_cmp_functor> GecoNetAddStringInterfaces;

class GECOAPI GecoNetEndpoint
{
    private:
        /// This is internal socket representation of the Endpoint.
#if defined( __linux__ ) || defined( PLAYSTATION3 )
        int m_kSocket;
#else //ifdef __linux__
        SOCKET m_kSocket;
#endif //def _WIN32

        /// this for one-off socket
        bool m_bShouldSendClose;
        typedef eastl::map<eastl::string, GecoNetAddress> FrontEndInterfaces;
        static FrontEndInterfaces ms_kFrontEndInterfaces;
        static GecoNetAddStringInterfaces ms_kGecoNetAddStringInterfaces;

    public:
        /// @name Construction/Destruction
        //@{
        GecoNetEndpoint();
        ~GecoNetEndpoint();
        //@}

        static const int NO_SOCKET = -1;

        /// @name File descriptor access
        //@{
        operator int() const;
        void SetFileDescriptor(int fd);
        bool Good() const;
        //@}

        /// @name General Socket Methods
        //@{
        void Socket(ushort af, int type);

        int SetNonblocking(bool nonblocking);
        int set_sock_broadcast(bool broadcast);
        int SetReuseaddr(bool reuseaddr);
        int SetKeepalive(bool keepalive);

        int Bind(u_int16_t networkPort, const char* networkAddr);
        int Bind(GecoNetAddress& bindaddr);

        int JoinMulticastGroup(const GecoNetAddress& networkAddr);
        int QuitMulticastGroup(const GecoNetAddress& networkAddr);

        int Close();
        int Detach();

        int GetLocalAddress(GecoNetAddress* networkAddr) const;
        int GetRemoteAddress(GecoNetAddress* networkAddr) const;

        const char * c_str() const;
        int GetRemoteHostname(eastl::string * name) const;

        bool GetClosedPort(GecoNetAddress & closedPort);
        //@}

        /// @name Connectionless Socket Methods
        //@{
        int SendTo(void * gramData, int gramSize, GecoNetAddress& networkAddr)
        {
            return ::sendto(m_kSocket, (char*) gramData, gramSize, 0, &networkAddr.su.sa, sizeof(sockaddr));
        }
        int RecvFrom(void * gramData, int gramSize, GecoNetAddress& networkAddr)
        {
            socklen_t sinLen = sizeof(sockaddr);
            return ::recvfrom(m_kSocket, (char*) gramData, gramSize, 0, &networkAddr.su.sa, &sinLen);
        }
        //@}

        /// @name Connecting Socket Methods
        //@{
        int Listen(int backlog = 1024);
        int Connect(const GecoNetAddress& networkAddr);
        GecoNetEndpoint * Accept(GecoNetAddress& networkAddr);
        int Send(const void * gramData, int gramSize);
        int Recv(void * gramData, int gramSize);
        bool RecvAll(void * gramData, int gramSize);
        //@}

        /// @name Network Interface Methods
        //@{
        int GetInterfaceFlags(char * name, int & flags);
        int GetInterfaceAddress(const char * name, GecoNetAddress & address);
        int GetInterfaceNetmask(const char * name, GecoNetAddress& netmask);
        /**
         * Generate a address/name map of all network interfaces.
         *
         * @param	interfaces	The map to populate with the interface list.
         *
         * @returns true on success, false on error.
         */
        const GecoNetAddStringInterfaces* GetInterfaces();
        /**
         *  This function finds the default interface, i.e. the one to use if
         *	an IP address is required for a socket that is bound to all interfaces.
         *
         *	Currently, the first valid (non-loopback) interface is used, but this
         *	should really be changed to be whatever interface is used by a local
         *	network broadcast - i.e. the interface that the default route goes over
         */
        int FindDefaultInterface(char * name);
        /**
         *	This function finds the interfaced specified by a string. The
         *	specification may take the form of a straight interface name,
         *	a IP address (name/dotted decimal), or a netmask (IP/bits).
         */
        int FindIndicatedInterface(const char * spec, char * name);
        /**
         * 	This method converts a string containing an IP address into
         * 	a 32 integer in network byte order. It handles both numeric
         * 	and named addresses.
         *
         *	@param string	The address as a string
         *	@param address	The address is returned here as an integer.
         *
         *	@return 0 if successful, -1 otherwise.
         */
        static int ConvertAddress(GecoNetAddress& address, const char * string, ushort port = 0);
        //@}

        /// @name Queue Size Methods
        //@{
        int TransmitQueueSize() const;
        int ReceiveQueueSize() const;
        int GetQueueSizes(int & tx, int & rx) const;
        //@}

        /// @name Buffer Size Methods
        //@{
        int GetBufferSize(int optname) const;
        bool SetBufferSize(int optname, int size);
        //@}

        static void AddFrontEndInterface(const eastl::string & name, GecoNetAddress* addr);
        //@}
};

INLINE GecoNetEndpoint::~GecoNetEndpoint()
{
    this->Close();
}

INLINE GecoNetEndpoint::operator int() const
{
    return (int) m_kSocket;
}
INLINE void GecoNetEndpoint::SetFileDescriptor(int fd)
{
    m_kSocket = fd;
}
INLINE bool GecoNetEndpoint::Good() const
{
    return (m_kSocket != NO_SOCKET);
}
INLINE void GecoNetEndpoint::Socket(ushort af, int type)
{
    this->SetFileDescriptor((int) ::socket(af, type, 0));
#ifndef __linux__
#ifndef PLAYSTATION3
    if ((m_kSocket == INVALID_SOCKET) && (WSAGetLastError() == WSANOTINITIALISED))
    {
        InitNetwork();
        this->SetFileDescriptor((int)::socket(af, type, 0));
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

INLINE int GecoNetEndpoint::set_sock_broadcast(bool broadcast)
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
    return ::setsockopt(m_kSocket, SOL_SOCKET, SO_BROADCAST, (char*) &val, sizeof(val));
}
INLINE int GecoNetEndpoint::SetReuseaddr(bool reuseaddr)
{
#ifdef __linux__
    int val;
#else
    bool val;
#endif
    val = reuseaddr ? 1 : 0;
    return ::setsockopt(m_kSocket, SOL_SOCKET, SO_REUSEADDR, (char*) &val, sizeof(val));
}
INLINE int GecoNetEndpoint::SetKeepalive(bool keepalive)
{
#ifdef __linux__
    int val;
#else
    bool val;
#endif
    val = keepalive ? 1 : 0;
    return ::setsockopt(m_kSocket, SOL_SOCKET, SO_KEEPALIVE, (char*) &val, sizeof(val));
}
INLINE int GecoNetEndpoint::Bind(u_int16_t networkPort, const char* networkAddr)
{
    sockaddrunion su;
    str2saddr(&su, networkAddr, networkPort);
    return ::bind(m_kSocket, (struct sockaddr*) &su, sizeof(sockaddr));
}
INLINE int GecoNetEndpoint::Bind(GecoNetAddress& bindaddr)
{
    return ::bind(m_kSocket, (struct sockaddr*) &bindaddr.su, sizeof(sockaddr));
}
INLINE int GecoNetEndpoint::JoinMulticastGroup(const GecoNetAddress& networkAddr)
{
#ifdef __linux__
    if (networkAddr.su.sa.sa_family == AF_INET)
    {
        struct ip_mreqn req;
        req.imr_multiaddr.s_addr = s4addr(&networkAddr.su);
        req.imr_address.s_addr = INADDR_ANY;
        req.imr_ifindex = 0;
        return ::setsockopt(m_kSocket, SOL_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req));
    }
    else
    {
    }
#else
    return -1;
#endif
}
INLINE int GecoNetEndpoint::QuitMulticastGroup(const GecoNetAddress& networkAddr)
{
#ifdef __linux__
    if (networkAddr.su.sa.sa_family == AF_INET)
    {
        struct ip_mreqn req;
        req.imr_multiaddr.s_addr = s4addr(&networkAddr.su);
        req.imr_address.s_addr = INADDR_ANY;
        req.imr_ifindex = 0;
        return ::setsockopt(m_kSocket, SOL_IP, IP_DROP_MEMBERSHIP, &req, sizeof(req));
    }
    else
    {
    }
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
    int ret = (int) m_kSocket;
    this->SetFileDescriptor(NO_SOCKET);
    return ret;
}
INLINE int GecoNetEndpoint::GetLocalAddress(GecoNetAddress* networkAddr) const
{
    socklen_t sinLen = sizeof(sockaddr);
    int ret = ::getsockname(m_kSocket, (struct sockaddr*) &networkAddr->su.sa, &sinLen);
    return ret;
}
INLINE int GecoNetEndpoint::GetRemoteAddress(GecoNetAddress* networkAddr) const
{
    socklen_t sinLen = sizeof(sockaddr);
    int ret = ::getpeername(m_kSocket, (struct sockaddr*) &networkAddr->su.sa, &sinLen);
    return ret;
}
INLINE const char * GecoNetEndpoint::c_str() const
{
    GecoNetAddress addr;
    this->GetLocalAddress(&addr);
    return addr.c_str();
}
INLINE int GecoNetEndpoint::GetRemoteHostname(eastl::string * host) const
{
    sockaddr_in sin;
    socklen_t sinLen = sizeof(sin);
    int ret = ::getpeername(m_kSocket, (struct sockaddr*) &sin, &sinLen);
    if (ret == 0)
    {
        hostent* h = gethostbyaddr((char*) &sin.sin_addr, sizeof(sin.sin_addr), AF_INET);

        if (h)
            *host = h->h_name;
        else
            ret = -1;
    }
    return ret;
}

INLINE int GecoNetEndpoint::Listen(int backlog)
{
    return ::listen(m_kSocket, backlog);
}

INLINE int GecoNetEndpoint::Connect(const GecoNetAddress& saddr)
{
    return ::connect(m_kSocket, (sockaddr*) &saddr.su.sa, sizeof(sockaddr));
}

INLINE GecoNetEndpoint * GecoNetEndpoint::Accept(GecoNetAddress& networkAddr)
{
    socklen_t sinLen = sizeof(sockaddr);
    int ret = (int) ::accept(m_kSocket, &networkAddr.su.sa, &sinLen);
#if defined( __linux__ ) || defined( PLAYSTATION3 )
    if (ret < 0)
        return NULL;
#else
    if (ret == INVALID_SOCKET) return NULL;
#endif
    GecoNetEndpoint * pNew = new GecoNetEndpoint();
    pNew->SetFileDescriptor(ret);
    return pNew;
}

INLINE int GecoNetEndpoint::Send(const void * gramData, int gramSize)
{
    return ::send(m_kSocket, (char*) gramData, gramSize, 0);
}

INLINE int GecoNetEndpoint::Recv(void * gramData, int gramSize)
{
    return ::recv(m_kSocket, (char*) gramData, gramSize, 0);
}

#ifdef __linux__
INLINE int GecoNetEndpoint::GetInterfaceFlags(char * name, int & flags)
{
    struct ifreq request;
    strncpy(request.ifr_name, name, IFNAMSIZ);
    if (ioctl(m_kSocket, SIOCGIFFLAGS, &request) != 0)
    {
        return -1;
    }

    flags = request.ifr_flags;
    return 0;
}

INLINE int GecoNetEndpoint::GetInterfaceAddress(const char * name, GecoNetAddress& address)
{
    struct ifreq request;
    strncpy(request.ifr_name, name, IFNAMSIZ);
    if (ioctl(m_kSocket, SIOCGIFADDR, &request) != 0)
    {
        return -1;
    }

    if (request.ifr_addr.sa_family == AF_INET)
    {
        s4addr(&address.su) = ((sockaddr_in*) &request.ifr_addr)->sin_addr.s_addr;
        return 0;
    }
    else if (request.ifr_addr.sa_family == AF_INET6)
    {
        memcpy_fast(address.su.sin6.sin6_addr.s6_addr, ((sockaddr_in6*) &request.ifr_addr)->sin6_addr.s6_addr,
                sizeof(in6_addr));
        return 0;
    }
    else
    {
        return -1;
    }
}

INLINE int GecoNetEndpoint::GetInterfaceNetmask(const char * name, GecoNetAddress& netmask)
{
    struct ifreq request;
    strncpy(request.ifr_name, name, IFNAMSIZ);

    if (ioctl(m_kSocket, SIOCGIFNETMASK, &request) != 0)
    {
        return -1;
    }
    if (request.ifr_addr.sa_family == AF_INET)
        netmask.su.sin.sin_addr.s_addr = ((sockaddr_in&) request.ifr_netmask).sin_addr.s_addr;
    else if (request.ifr_addr.sa_family == AF_INET6)
        memcpy_fast(netmask.su.sin6.sin6_addr.s6_addr, ((sockaddr_in6&) request.ifr_netmask).sin6_addr.s6_addr,
                sizeof(in6_addr));
    else
        network_logger()->critical("GecoNetEndpoint::GetInterfaceNetmask(): no such sa family !");
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
        char myname[256];
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
