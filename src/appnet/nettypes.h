/*
 * nettypes.h
 *
 *  Created on: 2Sep.,2016
 *      Author: jackiez
 */

#ifndef SRC_APPNET_NETTYPES_H_
#define SRC_APPNET_NETTYPES_H_

#include <map>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <sstream>
#include <memory>
#include <functional>
#include "../protocolstack/net_common.h"
#include "../plateform.h"
#include "../geco-engine-config.h"
#include "../common/ds/geco-bit-stream.h"
#include "../common/ultils/singleton_base.h"
struct selector;
struct watcher_value_query_t;

struct interface_cmp_functor  //比较函数 ==
{
        bool operator()(sockaddrunion a, sockaddrunion b) const
        {
            return saddr_equals(&a, &b);
        }
};

typedef std::map<sockaddrunion, std::string, interface_cmp_functor> interfaces_map_t;
/**
 *  This class provides a wrapper around a socket.
 *
 *  @ingroup network
 */
class geco_sock_base_t
{
    public:
        /// @name Construction/Destruction
        //@{
        geco_sock_base_t() :
                socket_(NO_SOCKET)
        {

        }
        ~geco_sock_base_t()
        {
            this->close();
        }
        //@}

        static const int NO_SOCKET = -1;

        /// @name File descriptor access
        //@{
        operator int() const
        {
            return socket_;
        }
        void setFileDescriptor(int fd)
        {
            socket_ = fd;
        }
        bool good() const
        {
            return (socket_ != NO_SOCKET);
        }
        //@}

        /// @name General Socket Methods
        //@{
        /**
         *  This method creates a socket of the requested type.
         *  It initialises Winsock if necessary.
         *
         *  @param type     Normally SOCK_STREAM or SOCK_DGRAM
         */
        void socket(int af = AF_INET, int type = SOCK_DGRAM, int protocol = 0)
        {
            this->setFileDescriptor(::socket(af, type, protocol));
#ifndef __unix__
#ifndef PLAYSTATION3
            if ((socket_ == INVALID_SOCKET) && (WSAGetLastError() == WSANOTINITIALISED))
            {
                // not initialised, so do so...
                //initNetwork();
                // ... and try it again
                this->setFileDescriptor(::socket(af, type, protocol));
            }
#endif
#endif
        }

        int setnonblocking(bool nonblocking)
        {
#ifdef __unix__
            int val = nonblocking ? O_NONBLOCK : 0;
            return ::fcntl(socket_, F_SETFL, val);
#elif defined( PLAYSTATION3 )
            int val = nonblocking ? 1 : 0;
            return setsockopt( socket_, SOL_SOCKET, SO_NBIO, &val, sizeof(int) );
#else
            u_long val = nonblocking ? 1 : 0;
            return ::ioctlsocket(socket_,FIONBIO,&val);
#endif
        }
        int setreuseaddr(bool reuseaddr)
        {
#ifdef __unix__
            int val;
#else
            bool val;
#endif
            val = reuseaddr ? 1 : 0;
            return ::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char*) &val, sizeof(val));
        }

        int bind(sockaddrunion& bindaddr)
        {
            return ::bind(socket_, &bindaddr.sa, sizeof(bindaddr.sa));
        }

        int close()
        {
            if (socket_ == NO_SOCKET)
            {
                return 0;
            }

#ifdef __unix__
            int ret = ::close(socket_);
#elif defined( PLAYSTATION3 )
            int ret = ::socketclose(socket_);
#else
            int ret = ::closesocket(socket_);
#endif
            if (ret == 0)
            {
                this->setFileDescriptor(NO_SOCKET);
            }
            return ret;
        }
        int detach()
        {
            int ret = socket_;
            this->setFileDescriptor(NO_SOCKET);
            return ret;
        }

        //  @return 0 if successful, or -1 if an error occurred
        int getlocaladdress(sockaddrunion& out_addr) const
        {
            static socklen_t sinLen = sizeof(out_addr.sa);
            return ::getsockname(socket_, &out_addr.sa, &sinLen);
        }

        sockaddrunion getLocalAddress() const;
        sockaddrunion getRemoteAddress() const;

        const char * c_str() const
        {
            sockaddrunion su;
            getlocaladdress(su);
            static char buf[MAX_IPADDR_STR_LEN + MAX_IPPORT_STR_LEN];
            ushort port;
            saddr2str(&su, buf, MAX_IPADDR_STR_LEN, &port);
            sprintf(buf + strlen(buf), ":%d", port);
            return buf;
        }

        bool getClosedPort(sockaddrunion& closedPort);
        //@}

        /// @name Network Interface Methods
        //@{
#ifdef __unix__

        /**
         *  This method returns the flags associated with the given interface.
         *
         *  @param name     Interface name for which flags are needed.
         *  @param flags    The flags are returned here.
         *
         *  @return 0 if successful, 1 otherwise.
         */
        int getInterfaceFlags(char * name, int& flags);
        /**
         *  This method returns the address to which an interface is bound.
         *
         *  @param name     Name of the interface.
         *  @param address  The address is returned here.
         *
         *  @return 0 if successful, 1 otherwise.
         */
        int getInterfaceAddress(const char * name, sockaddrunion& outaddress);
        /**
         *  This method returns the netmask for a local interface.
         *
         *  @param name     Name of the interface.
         *  @param netmask  The netmask is returned here.
         *
         *  @return 0 if successful, 1 otherwise.
         */
        int getInterfaceNetmask(const char * name, sockaddrunion& netmask);
#else
        int getInterfaceFlags(char * name, int & flags);
        int getInterfaceAddress(const char * name, sockaddrunion& outaddress);
        int getInterfaceNetmask(const char * name, sockaddrunion & netmask);
#endif
        /**
         * Generate a address/name map of all network interfaces.
         *
         * @param   interfaces  The map to populate with the interface list.
         *
         * @returns true on success, false on error.
         */
        bool getInterfaces(interfaces_map_t &interfaces);
        int findDefaultInterface(char * name);
        int findIndicatedInterface(const char * spec, int af, char * outname);
        static int convertAddress(const char * string, uint & address);
        //@}

        /// @name Queue Size Methods
        //@{
        int transmitQueueSize() const;
        int receiveQueueSize() const;
        int getQueueSizes(int & tx, int & rx) const;
        //@}

        /// @name Buffer Size Methods
        //@{
        /**
         *  This method returns either the send or receive buffer size for this socket,
         *  or -1 on error.  You should pass either SO_RCVBUF or SO_SNDBUF as the
         *  argument to this method.
         */
        int getBufferSize(int optname) const;
        /**
         *  This method sets either the send or receive buffer size for this socket.
         *  You should pass either SO_RCVBUF or SO_SNDBUF as the optname argument to
         *  this method.
         */
        bool setBufferSize(int optname, int size);
        //@}

    protected:

        /// This is internal socket representation of the Endpoint.
#if defined(__unix__) || defined(PLAYSTATION3)
        int socket_;
#else //ifdef unix
        SOCKET socket_;
#endif //def _WIN32
};
class geco_udp_sock_t: public geco_sock_base_t
{
    public:

        int joinMulticastGroup(uint ip4networkAddr)
        {
#ifdef __unix__
            struct ip_mreqn req;
            req.imr_multiaddr.s_addr = ip4networkAddr;
            req.imr_address.s_addr = INADDR_ANY;
            req.imr_ifindex = 0;
            return ::setsockopt(socket_, SOL_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req));
#else
            return -1;
#endif
        }
        int quitMulticastGroup(uint32 ip4aaddr)
        {
#ifdef __unix__
            struct ip_mreqn req;
            req.imr_multiaddr.s_addr = ip4aaddr;
            req.imr_address.s_addr = INADDR_ANY;
            req.imr_ifindex = 0;
            return ::setsockopt(socket_, SOL_IP, IP_DROP_MEMBERSHIP, &req, sizeof(req));
#else
            return -1;
#endif
        }
        int setbroadcast(bool broadcast);

        /// @name Connectionless Socket Methods
        //@{
        /**
         *  This method sends a packet to the given address.
         *
         *  @param gramData     Pointer to a data buffer containing the packet.
         *  @param gramSize     Number of bytes in the data buffer.
         *  @param sin          Destination address.
         */
        int sendto(void * gramData, int gramSize, sockaddrunion& sin)
        {
            return ::sendto(socket_, (char*) gramData, gramSize, 0, &sin.sa, sizeof(sin.sa));
        }
        /**
         *  This method attempts to receive a packet.
         *
         *  @param gramData     Pointer to a data buffer to receive the packet.
         *  @param gramSize     Number of bytes in the data buffer.
         *  @return The number of bytes received, or -1 if an error occurred.
         */
        int recvfrom(void * gramData, int gramSize, sockaddrunion& sin)
        {
            static socklen_t sinLen = sizeof(sin.sa);
            return ::recvfrom(socket_, (char*) gramData, gramSize, 0, &sin.sa, &sinLen);
        }
        //@}
};
class geco_tcp_sock_t: public geco_sock_base_t
{
    public:
        //@return 0 if successful, or -1 if an error occurred.
        int getremotehostname(std::string * out_name) const;

        //@return 0 if successful, or -1 if an error occurred.
        int getremoteaddress(sockaddrunion& out_addr) const
        {
            static socklen_t sinLen = sizeof(out_addr.sa);
            return ::getpeername(socket_, &out_addr.sa, &sinLen);
        }
        /// @name Connecting Socket Methods
        //@{
        int listen(int backlog = 1024)
        {
            return ::listen(socket_, backlog);
        }
        int connect(sockaddrunion& sin)
        {
            return ::connect(socket_, &sin.sa, sizeof(sin));
        }
        /**
         *  This method accepts a connection on the socket listen queue, returning
         *  a new endpoint if successful, or NULL if not. The remote port and
         *  address are set into the pointers passed in if not NULL.
         */
        geco_tcp_sock_t* accept(sockaddrunion& sin)
        {
            static socklen_t sinLen = sizeof(sin.sa);
            int ret = ::accept(socket_, &sin.sa, &sinLen);
#if defined( __unix__ ) || defined( PLAYSTATION3 )
            if (ret < 0) return NULL;
#else
            if (ret == INVALID_SOCKET) return NULL;
#endif
            geco_tcp_sock_t* pNew = new geco_tcp_sock_t();
            pNew->setFileDescriptor(ret);
            return pNew;
        }
        int send(const void * gramData, int gramSize)
        {
            return ::send(socket_, (char*) gramData, gramSize, 0);
        }
        int recv(void * gramData, int gramSize)
        {
            return ::recv(socket_, (char*) gramData, gramSize, 0);
        }
        /**
         *  This helper method wait until exactly gramSize data has been read.
         *
         *  True if gramSize was read, otherwise false (usually indicating the
         *  connection was lost.
         */
        bool recv_all(void * gramData, int gramSize);
        //@}

        int setkeepalive(bool keepalive)
        {
#ifdef __unix__
            int val;
#else
            bool val;
#endif
            val = keepalive ? 1 : 0;
            return ::setsockopt(socket_, SOL_SOCKET, SO_KEEPALIVE, (char*) &val, sizeof(val));
        }
};

/**
 *  This enumeration contains the possible watcher message IDs.
 *
 *  @ingroup watcher
 */
enum WatcherMsg
{
    // Deprecated messages used by old http watcher process.
    // WATCHER_MSG_REGISTER = 0,
    // WATCHER_MSG_DEREGISTER = 1,
    // WATCHER_MSG_FLUSHCOMPONENTS = 2,

    WATCHER_MSG_GET2 = 26,
    WATCHER_MSG_SET2 = 27,
    WATCHER_MSG_TELL2 = 28,
    WATCHER_MSG_SET2_TELL2 = 29,

    WATCHER_MSG_EXTENSION_START = 107
};

/**
 *  This structure is the network message used to register a watcher.
 *  @ingroup watcher
 */
struct WatcherRegistrationMsg
{
        int uid;        // your uid
        int message;    // WATCHER_...
        int id;         // e.g. 14
        char abrv[32];  // e.g. "cell14"
        char name[64];  // e.g. "Cell 14"
};

#ifdef _WIN32
#pragma warning(disable: 4200)
#endif

/**
 *  This structure is used to send watcher data across the network.
 *  @ingroup watcher
 */
struct WatcherDataMsg
{
        int message;
        int count;
        char string[0];
};

/* Packets:
 Reg and Dereg are just a WatcherRegistrationMsg Get and Set are a
 WatcherDataMsg followed by 'count' strings (for get) or string pairs (for
 set and tell). Every get/set packet is replied to with a tell packet.
 */

/**
 * This class is responsible for managing individual watcher path requests,
 * waiting for their responses, and then replying to the requesting process
 * with the collection of watcher path responses.
 */
typedef std::vector<watcher_value_query_t*> WatcherQueries;
class WatcherPacketHandler
{
    private:
        static const std::string v1ErrorIdentifier; //!< Error prefix used in v1 watcher responses when responses will exceed UDP packet size.
        static const std::string v1ErrorPacketLimit; //!< Error message used in v1 watcher responses when responses will exceed UDP packet size.

        bool canDelete_; //!< Set to true when we aren't initiating path requests.

        int channelid; //!< The channel we should communicate through.

        bool isSet_; //!< Is the current watcher request a set operation.

        int32 outgoingRequests_; //!< The number of separate watcher path requests we have initiated.
        int32 answeredRequests_; //!< The number of path requests that have replied to us.
        bool isExecuting_; //!< True if we are active and expecting responses.

        geco_bit_stream_t packet_; //!< The raw packet data we will respond with.
        WatcherQueries pathRequestList_; //!< List of all path requests belonging to the current watcher query.

        uint maxPacketSize_; //!< The maximum size of packet we should fill with watcher response data.
        bool reachedPacketLimit_; //!< Have we already filled the outgoing packet.

    public:
        WatcherPacketHandler(int channelid, int32 numPaths, bool isSet = false) :
                canDelete_(false), channelid(-1), isSet_(isSet), outgoingRequests_(numPaths), answeredRequests_(
                        0), isExecuting_(false), maxPacketSize_(0), reachedPacketLimit_(false)
        {
            packet_.Write((uchar) (isSet_ ? WATCHER_MSG_SET2_TELL2 : WATCHER_MSG_TELL2));
            // Reserve 4 bytes for the count of the number of replies
            // so we can modify it immediately before dispatching the response.
            packet_.Write((ushort) 0);
            maxPacketSize_ = 65536;
        }
        virtual ~WatcherPacketHandler()
        {
            auto iter = pathRequestList_.begin();
            while (iter != pathRequestList_.end())
            {
                delete *iter;
                iter++;
            }
            pathRequestList_.clear();
        }

        /** Start all the watcher path request operations. */
        void run();

        /**
         * Check whether all outgoing path requests have responded.
         *
         * When all requests have responded we can finalise the last outgoing
         * packet and then notify our owner for deletion.
         */
        void checkSatisfied();

        void sendReply();

        /**
         * Set a flag to prevent this object from being deleted until it's safe.
         *
         * @param shouldNotDelete  Should the packet handler be undeleteable?
         */
        void doNotDelete(bool shouldNotDelete);

        watcher_value_query_t* newRequest(std::string & path);
        void notifyComplete(watcher_value_query_t& pathRequest, int32 count);
};

const int WN_PACKET_SIZE = 0x10000;
const int WN_PACKET_SIZE_TCP = 0x1000000;

class watcher_connection_acceptor_t: public base_singleton_t<watcher_connection_acceptor_t>
{
    private:
        char abrv_[32];
        char name_[64];

        //WatcherRequestHandler * pExtensionHandler_;
        int id_;
        bool registered_;
        bool insideReceiveRequest_;
        char *requestPacket_;
        bool isInitialised_;

        geco_udp_sock_t udpSocket_;
        geco_tcp_sock_t tcpSocket_;
        selector* reactor_;

    public:
        watcher_connection_acceptor_t() :
                id_(-1), registered_(false), insideReceiveRequest_(false), requestPacket_(
                        new char[WN_PACKET_SIZE]), isInitialised_(false), reactor_( NULL)
        {

        }
        ~watcher_connection_acceptor_t()
        {
            if (registered_)
            {
                this->deregisterWatcher();
            }

            if (udpSocket_.good())
            {
                udpSocket_.close();
            }

            if (requestPacket_ != NULL)
            {

                delete[] requestPacket_;
                requestPacket_ = NULL;
            }
        }
        /**
         *  This method initialises the watcher nub.
         */
        bool init(const char * listeningInterface, ushort listeningPort, int af);

        /**
         *  This method associates this with the given dispatcher.
         */
        void attachTo(selector* reactor);

        /**
         *  This method broadcasts a watcher register message for this watcher.
         *
         *  @param id           Numeric id for this watcher.
         *  @param abrv         Short name for this watcher.
         *  @param longName     Long name for this watcher.
         *  @param listeningInterface   The name of the network interface to listen on.
         *  @param listeningPort        The port to listen on.
         */
        int registerWatcher(int id, const char *abrv, const char *longName,
                const char * listeningInterface, uint16 listeningPort, int af);
        /**
         *  This method broadcasts a watcher deregister message for this watcher.
         */
        int deregisterWatcher();
        /**
         *  This method sends a message to the machined process.
         */
        void notifyMachineGuard();

        /**
         *  This method attempts to bind the UDP and TCP sockets to the same port.
         */
        bool bindSockets(sockaddrunion& su);

        geco_tcp_sock_t& geco_tcp_sock()
        {
            return tcpSocket_;
        }
        geco_udp_sock_t& geco_udp_sock()
        {
            return udpSocket_;
        }
};

#endif /* SRC_APPNET_NETTYPES_H_ */
