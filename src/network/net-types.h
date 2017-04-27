//{future header message}
#ifndef __GecoNetTypes_H__
#define __GecoNetTypes_H__

// winsock2 must include before windows.h
#ifdef _WIN32
#include <winsock2.h>
#include <ws2ipdef.h>
#endif

#include "common/geco-plateform.h"
#include "common/ds/geco-bit-stream.h"
#include "common/ds/eastl/EASTL/string.h"
#include "common/debugging/spdlog/spdlog.h"

#include "math/ema.h"
#include "math/geco-math-vector.h"
#include "math/geco-math-direction.h"

#include <vector>
#include <limits>

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
#else
#include <netex/libnetctl.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#ifndef __linux__
#ifdef PLAYSTATION3
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
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
enum
{
	//IFF_UP = 0x1,
	//IFF_BROADCAST = 0x2,
	IFF_DEBUG = 0x4,
	//IFF_LOOPBACK = 0x8,
	IFF_POINTOPOINT = 0x10,
	IFF_NOTRAILERS = 0x20,
	IFF_RUNNING = 0x40,
	IFF_NOARP = 0x80,
	IFF_PROMISC = 0x100,
	//IFF_MULTICAST = 0x1000
};
#endif

#ifndef _WIN32
#include <sys/time.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netdb.h>
#include <arpa/inet.h>      /* for inet_ntoa() under both SOLARIS/LINUX */
#include <sys/errno.h>
#include <sys/uio.h>        /* for struct iovec */
#include <sys/param.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <net/if.h>
#ifdef USE_UDP
#include <netinet/udp.h>
#endif
#include <asm/types.h>
#include <linux/rtnetlink.h>
#else
//#include <Netioapi.h>
#include <ws2tcpip.h> // has #include <ws2ipdef.h> and #include <winsock2.h>
#include <ws2def.h>
#include <mstcpip.h>
#include <mswsock.h>
#include <iphlpapi.h>
#include <sys/timeb.h>
#endif

#if defined (__linux__)
#include <asm/types.h>
#include <linux/rtnetlink.h>
#else /* this may not be okay for SOLARIS !!! */
#ifndef _WIN32
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
#ifndef __sun
#include <net/if_var.h>
#include <machine/param.h>
#else
#include <sys/sockio.h>
#endif
#endif
#endif

#if defined( __linux__) || defined(__unix__)
#include <sys/poll.h>
#else
#define POLLIN     0x001 //2base    0001
#define POLLPRI    0x002 //2base    0010
#define POLLOUT    0x004 //2base  0100
#define POLLERR    0x008//2base    1000
#endif

#define IFA_BUFFER_LENGTH   1024
#define POLL_FD_UNUSED     -1
#define MAX_FD_SIZE     32
#define    EVENTCB_TYPE_SCTP       1
#define    EVENTCB_TYPE_UDP        2
#define    EVENTCB_TYPE_USER       3
#define    EVENTCB_TYPE_ROUTING    4
#define    EVENTCB_TYPE_STDIN          5
#define    EVENTCB_TYPE_TASK       6

#ifndef CMSG_ALIGN
#ifdef ALIGN
#define CMSG_ALIGN ALIGN
#else
#define CMSG_ALIGN(len) ( ((len)+sizeof(long)-1) & ~(sizeof(long)-1) )
#endif
#endif

#ifndef CMSG_SPACE
#define CMSG_SPACE(len) (CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(len))
#endif

#ifndef CMSG_LEN
#define CMSG_LEN(len) (CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))
#endif

#ifdef _WIN32
#define MY_CMSG_DATA WSA_CMSG_DATA
#else
#define MY_CMSG_DATA CMSG_DATA
#endif

#ifndef _WIN32
#define LINUX_PROC_IPV6_FILE "/proc/net/if_inet6"

#else
#define ADDRESS_LIST_BUFFER_SIZE        4096
struct iphdr
{
	uchar version_length;
	uchar typeofservice; /* type of service */
	ushort length; /* total length */
	ushort identification; /* identification */
	ushort fragment_offset; /* fragment offset field */
	uchar ttl; /* time to live */
	uchar protocol; /* protocol */
	ushort checksum; /* checksum */
	struct in_addr src_addr; /* source and dest address */
	struct in_addr dst_addr;
};

#define msghdr _WSAMSG
#define iovec _WSABUF
#endif

#ifndef _WIN32
//#define USES_BSD_4_4_SOCKET
#ifndef __sun
#define ROUNDUP(a, size) (((a) & ((size)-1)) ? (1 + ((a) | ((size)-1))) : (a))
#define NEXT_SA(ap) \
ap = (struct sockaddr *)((caddr_t) ap + (ap->sa_len ? \
ROUNDUP(ap->sa_len, sizeof (u_long)) : sizeof(u_long)))
inline bool IN6_ADDR_EQUAL(const in6_addr *x, const in6_addr *y)
{
	uint64_t* a = (uint64_t*)x;
	uint64_t* b = (uint64_t*)y;
	return (bool)((a[1] == b[1]) && (a[0] == b[0]));
}
#else
#define NEXT_SA(ap) ap = (struct sockaddr *) ((caddr_t) ap + sizeof(struct sockaddr))
#define RTAX_MAX RTA_NUMBITS
#define RTAX_IFA 5
#define _NO_SIOCGIFMTU_
#endif
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <sys/types.h>
#endif

#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x)       (x)
#define unlikely(x)    (x)
#endif

#ifdef __linux__
#include <endian.h>
# if __BYTE_ORDER == __LITTLE_ENDIAN
#endif
#endif

// for linux-kernal-systems
#ifdef __linux__
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#include <sys/types.h>
#endif

#if defined(__sun)
# if defined(__svr4__)
/* Solaris */
#include <netinet/in_systm.h>
#include <stdarg.h>
# else
/* SunOS */
// add more files needed
# endif
#endif

#ifdef _XBOX360
#include "GecoNet360.h"
#endif

#ifndef _WIN32
#define GECO_NET_LOCALHOSTNAME "lo"
#ifndef _LP64
#define FMT_DBID "lld"
#else
#define FMT_DBID "ld"
#endif
#else
#define GECO_NET_LOCALHOSTNAME "localhost"
#define FMT_DBID "I64d"
#endif

extern std::shared_ptr<spdlog::logger>& network_logger();
extern bool g_enable_stats;

#define s4addr(X)   (((struct sockaddr_in *)(X))->sin_addr.s_addr)
#define sin4addr(X)   (((struct sockaddr_in *)(X))->sin_addr)
#define s6addr(X)  (((struct sockaddr_in6 *)(X))->sin6_addr.s6_addr)
#define sin6addr(X)  (((struct sockaddr_in6 *)(X))->sin6_addr)
#define saddr_family(X)  (X)->sa.sa_family
/* union for handling either type of addresses: ipv4 and ipv6 */
union sockaddrunion
{
	struct sockaddr sa;
	struct sockaddr_in sin;
	struct sockaddr_in6 sin6;
};
/* converts address-string to network order endian
 * (hex for ipv6, dotted decimal for ipv4 to a sockaddrunion structure)
 *  str == NULL will bitzero saddr used as 'ANY ADRESS 0.0.0.0'
 *  port number will be always >0
 *  default  is IPv4
 *  @return 0 for success, else -1.*/
extern int GECOAPI str2saddr(sockaddrunion *su, const char * str, ushort port = 0);
extern int GECOAPI saddr2str(sockaddrunion *su, char * buf, size_t len, ushort* portnum = NULL);
extern bool GECOAPI saddr_equals(const sockaddrunion *a, const sockaddrunion *b, bool ignore_port = false);
//! From http://www.azillionmonkeys.com/qed/hash.html
//! Author of main code is Paul Hsieh. I just added some convenience functions
//! Also note http://burtleburtle.net/bob/hash/doobs.html, which shows that this is 20%
//! faster than the one on that page but has more collisions
extern unsigned long SuperFastHash(const char * data, int length);
extern unsigned long SuperFastHashIncremental(const char * data, int len, unsigned int lastHash);
extern unsigned long SuperFastHashFile(const char * filename);
extern unsigned long SuperFastHashFilePtr(FILE *fp);
extern unsigned int transportaddr2hashcode(const sockaddrunion* local_sa, const sockaddrunion* peer_sa);
extern unsigned int sockaddr2hashcode(const sockaddrunion* sa);

struct sockaddr_hash_functor
{
	size_t operator()(const sockaddrunion& addr) const
	{
		return sockaddr2hashcode(&addr);
	}
};
struct sockaddr_cmp_functor
{
	bool operator()(const sockaddrunion& a, const sockaddrunion& b) const
	{
		return saddr_equals(&a, &b, true);
	}
};

class GecoWatcher;
class GecoNetBundle;
class GecoNetPacket;
class GecoNetworkInterface;

const uchar REPLY_MESSAGE_IDENTIFIER = 0xFF;

/**
 *	@internal
 *	This is the default request timeout in microseconds.
 */
const int DEFAULT_REQUEST_TIMEOUT = 5000000; // 5 SECS

/**
 * 	@internal
 * 	This constant indicates a fixed length message.
 * 	It is used as the lengthStyle in an InterfaceElement.
 * 	For this type of message, lengthParam is the number
 * 	of bytes in the message. This is constant for all
 * 	messages of this type.
 */
const char FIXED_LENGTH_MESSAGE = 0;
/**
 * 	@internal
 * 	This constant indicates a variable length message.
 * 	It is used as the lengthStyle in an InterfaceElement.
 * 	For this type of message, lengthParam is the number
 * 	of bytes used to store the message size. This is
 * 	constant for all messages of this type.
 */
const char VARIABLE_LENGTH_MESSAGE = 1;
/**
 * 	@internal
 * 	This constant indicates the InterfaceElement has not been initialised.
 */
const char INVALID_MESSAGE = 2;
const int PACKET_MAX_SIZE = 1472; // 1500 - iphdr 20 - updhdr 8 = 1472

/**
 *	The number of bytes in the header of a UDP packet.
 *
 * 	@ingroup common
 */
const int GECO_NET_UDP_OVERHEAD = 28;

/**
 *	The number of bits per byte.
 *
 * 	@ingroup common
 */
const int GECO_NET_BITS_PER_BYTE = 8;

/**
 *  Standard localhost address.  Assumes little endian host byte order.
 *
 * 	@ingroup common
 */
const uint GECO_NET_LOCALHOST = 0x0100007F;

/**
 *  Standard broadcast address.  Assumes little endian host byte order.
 *
 * 	@ingroup common
 */
const uint GECO_NET_BROADCAST = 0xFFFFFFFF;

/**
 * Watcher Nub packet size.
 *
 * @ingroup common
 */
static const int GECO_NET_WN_PACKET_SIZE = 0x10000;
static const int GECO_NET_WN_PACKET_SIZE_TCP = 0x1000000;

/**
 *	A game timestamp. This is synchronised between all clients and servers,
 *	and is incremented by one every game tick.
 *
 * 	@ingroup common
 */
typedef uint GecoTimeStamp;

/**
 *	A unique identifier representing an entity. An entity's ID is constant
 *	between all its instances on the baseapp, cellapp and client.
 *	EntityIDs are however not persistent and should be assumed to be different
 *	upon each run of the server.
 *
 *	A value of zero is considered a null value as defined by NULL_ENTITY.
 *	The range of IDs greater than 1<<30 is reserved for client only entities.
 *
 *	This type replaces the legacy ObjectID type.
 *
 * 	@ingroup common
 */
typedef int GecoEntityID;
const uchar GECO_ENTITY_DEF_INIT_FLG = 0;

/**
 *	The null value for an EntityID meaning that no entity is referenced.
 *
 * 	@ingroup common
 */
const GecoEntityID GECO_NULL_ENTITY = ((GecoEntityID)0);

//! Server (0, 0x20000000)
const GecoEntityID GECO_MIN_SERVER_ENTITY = ((GecoEntityID)0);
const GecoEntityID GECO_MAX_SERVER_ENTITY = ((GecoEntityID)0x20000000);

//! Client (0x1FFFFFFF, 0x40000000)
const GecoEntityID GECO_MIN_CLIENT_ENTITY = ((GecoEntityID)0x1FFFFFFF);
const GecoEntityID GECO_MAX_CLIENT_ENTITY = ((GecoEntityID)0x40000000);

//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const GecoEntityID GECO_MIN_GLOBAL_ENTITY = ((GecoEntityID)0x3FFFFFFF);
const GecoEntityID GECO_MAX_GLOBAL_ENTITY = ((GecoEntityID)0x7FFFFFFF);

/**
 *	A unique ID representing a space.
 *
 *	A value of zero is considered a null value as defined by NULL_SPACE.
 *
 * 	@ingroup common
 */
typedef int GecoSpaceID;

/**
 *	The null value for an SpaceID meaning that no space is referenced.
 *
 * 	@ingroup common
 */
const GecoSpaceID GECO_NULL_SPACE = ((GecoSpaceID)0);

//! Server (0, 0x40000000)
const GecoSpaceID GECO_MIN_SERVER_SPACE = ((GecoSpaceID)0);
const GecoSpaceID GECO_MAX_SERVER_SPACE = ((GecoSpaceID)0x40000000);

//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const GecoSpaceID GECO_MIN_GLOBAL_SPACE = ((GecoSpaceID)0x3FFFFFFF);
const GecoSpaceID GECO_MAX_GLOBAL_SPACE = ((GecoSpaceID)0x7FFFFFFF);

/**
 *	A unique ID representing a cell application.
 *
 * 	@ingroup common
 */
typedef int GecoCellAppID;

/**
 *	A unique ID representing a base application.
 *
 * 	@ingroup common
 */
typedef int GecoBaseAppID;
/**
 *	The authentication key used between ClientApp and BaseApp.
 */
typedef uint GecoSessionKey;

/**
 *	A 3D vector used for storing object positions.
 *
 * 	@ingroup common
 */
typedef GecoVector3 GecoPosition3D;

/**
 *	A unique ID representing a class of entity.
 *
 * 	@ingroup common
 */
typedef ushort GecoEntityTypeID;
const GecoEntityTypeID GECO_INVALID_ENTITY_TYPE_ID = GecoEntityTypeID(-1);

/**
 *  An ID representing an entities position in the database
 */
typedef int64 GecoDatabaseID;

/**
 *  printf format string for DatabaseID e.g.
 *		DatabaseID dbID = &lt;something>;
 *		printf( "Entity %"FMT_DBID" is not valid!", dbID );
 */
#ifndef _WIN32
#ifndef _LP64
#define FMT_DBID "lld"
#else
#define FMT_DBID "ld"
#endif
#else
#define FMT_DBID "I64d"
#endif

 /**
  *	This is a 32 bit ID that represents a grid square. The most significant
  *	16 bits is the signed X position. The least significant 16 bits is the
  *	signed Y position.
  */
typedef uint GecoGridID;

/**
 *	The type used as a sequence number for the events of an entity.
 */
typedef uint GecoEventNumber;
const GecoEventNumber GECO_INITIAL_EVENT_NUMBER = 1;
typedef std::vector<GecoEventNumber> GecoCacheStamps;

/**
 *	The type used as a sequence number for volatile updates.
 */
typedef ushort GecoVolatileNumber;

typedef uchar GecoDetailLevel;
const int GECO_MAX_DATA_LOD_LEVELS = 6;

typedef int GecoNetSeqNum;
typedef uchar GecoNetMessageID;
typedef void * GecoNetTimerID;
const GecoNetTimerID GECO_NET_TIMER_ID_NONE = 0;
typedef int GecoNetChannelID;
const GecoNetChannelID GECO_NET_CHANNEL_ID_NULL = 0;
typedef GecoNetSeqNum GecoNetChannelVersion;
typedef int GecoNetReplyID;
const GecoNetReplyID GECO_NET_REPLY_ID_NONE = -1;
const GecoNetReplyID GECO_NET_REPLY_ID_MAX = 1000000;
const int GECO_NET_DEFAULT_ONCEOFF_RESEND_PERIOD = 200 * 1000; /* 200 ms */
const int GECO_NET_DEFAULT_ONCEOFF_MAX_RESENDS = 50;

enum GecoNetReason
{
	GECO_NET_REASON_SUCCESS = 0,
	GECO_NET_REASON_TIMER_EXPIRED = -1,
	GECO_NET_REASON_NO_SUCH_PORT = -2,
	GECO_NET_REASON_GENERAL_NETWORK = -3,
	GECO_NET_REASON_CORRUPTED_PACKET = -4,
	GECO_NET_REASON_NONEXISTENT_ENTRY = -5,
	GECO_NET_REASON_WINDOW_OVERFLOW = -6,
	GECO_NET_REASON_INACTIVITY = -7,
	GECO_NET_REASON_RESOURCE_UNAVAILABLE = -8,
	GECO_NET_REASON_CLIENT_DISCONNECTED = -9,
	GECO_NET_REASON_TRANSMIT_QUEUE_FULL = -10,
	GECO_NET_REASON_CHANNEL_LOST = -11
};
INLINE const char * NetReasonToString(GecoNetReason reason)
{
	static const char * reasons[] = { "GECO_NET_REASON_SUCCESS", "GECO_NET_REASON_TIMER_EXPIRED",
			"GECO_NET_REASON_NO_SUCH_PORT", "GECO_NET_REASON_GENERAL_NETWORK", "GECO_NET_REASON_CORRUPTED_PACKET",
			"GECO_NET_REASON_NONEXISTENT_ENTRY", "GECO_NET_REASON_WINDOW_OVERFLOW", "GECO_NET_REASON_INACTIVITY",
			"GECO_NET_REASON_RESOURCE_UNAVAILABLE", "GECO_NET_REASON_CLIENT_DISCONNECTED",
			"GECO_NET_REASON_TRANSMIT_QUEUE_FULL", "GECO_NET_REASON_CHANNEL_LOST" };
	unsigned int index = -reason;
	if (index < sizeof(reasons) / sizeof(reasons[0]))
	{
		return reasons[index];
	}
	return "GECO_NET_REASON_UNKNOWN";
}

class GECOAPI GecoNetAddress
{
private:
	static const int MAX_STRLEN = 256;
	static char ms_pcStringBuf[2][MAX_STRLEN];
	static int ms_iCurrStringBuf;
	static char * NextStringBuf();

public:
	sockaddrunion su;
	ushort m_uiSalt;

public:
	GecoNetAddress();
	GecoNetAddress(sockaddr* saddr) :
		m_uiSalt(0)
	{
		su.sa = *saddr;
	}
	GecoNetAddress(ushort port, const char* addrstr);

	int WriteToString(char * str, int length);
	operator char*()
	{
		return c_str();
	}
	char* c_str();
	const char* ip_str();

	bool IsNone() const
	{
		return ntohl(su.sin.sin_addr.s_addr) == INADDR_ANY || IN6_IS_ADDR_UNSPECIFIED(&su.sin6.sin6_addr);
	}
	// Broadcast only supoorts ip4
	bool is_broadcast_addr() const
	{
		return su.sa.sa_family == AF_INET && ntohl(su.sin.sin_addr.s_addr) == GECO_NET_BROADCAST;
	}
	static GecoWatcher* GetWatcher();
	static const GecoNetAddress NONE;
};

INLINE bool operator==(const GecoNetAddress & a, const GecoNetAddress & b);
INLINE bool operator!=(const GecoNetAddress & a, const GecoNetAddress & b);
INLINE geco_bit_stream_t& operator <<(geco_bit_stream_t& os, GecoNetAddress & a)
{
	a.su.sa.sa_family = htons(a.su.sa.sa_family);
	os.WriteRaw((uchar*)&a.su, sizeof(sockaddr));
	return os;
}
INLINE geco_bit_stream_t& operator >>(geco_bit_stream_t& os, GecoNetAddress & a)
{
	os.ReadRaw((uchar*)&a.su, sizeof(sockaddr));
	a.su.sa.sa_family = ntohs(a.su.sa.sa_family);
	return os;
}
GECOAPI bool WatcherStringToValue(const char *, GecoNetAddress &);
GECOAPI eastl::string WatcherValueToString(const GecoNetAddress &);

struct geco_net_addr_hash_functor
{
	size_t operator()(const GecoNetAddress& addr) const
	{
		return sockaddr2hashcode(&addr.su);
	}
};
struct geco_net_addr_cmp_functor
{
	bool operator()(const GecoNetAddress& a, const GecoNetAddress& b) const
	{
		return saddr_equals(&a.su, &b.su);
	}
};

/**
 *	This structure is a packed version of a mailbox for an entity
 */
struct GECOAPI GecoEntityMailBoxRef
{
	GecoEntityID m_iID;
	GecoNetAddress m_kAddr;

	enum Component
	{
		CELL = 0, BASE = 1, GLOBAL = 2,
	};

	Component GetComponent() const
	{
		return (Component)(m_kAddr.m_uiSalt >> 13);
	}
	void SetComponent(Component c)
	{
		m_kAddr.m_uiSalt = GetType() | (ushort(c) << 13);
	}

	GecoEntityTypeID GetType() const
	{
		return m_kAddr.m_uiSalt & 0x1FFF;
	}
	void SetType(GecoEntityTypeID t)
	{
		m_kAddr.m_uiSalt = (m_kAddr.m_uiSalt & 0xE000) | t;
	}

	void Init()
	{
		m_iID = 0;
		m_kAddr.su =
		{ 0 };
		m_kAddr.m_uiSalt = 0;
	}
	void Init(GecoEntityID i, const GecoNetAddress & a, Component c, GecoEntityTypeID t)
	{
		m_iID = i;
		m_kAddr = a;
		m_kAddr.m_uiSalt = (ushort(c) << 13) | t;
	}

	GecoEntityMailBoxRef()
	{
		SetType(GECO_INVALID_ENTITY_TYPE_ID);
	}
};
INLINE bool operator==(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b);
INLINE bool operator!=(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b);

/**
 *	This class maintains a set of 32 boolean capabilities.
 *
 *	@ingroup common
 */
class GECOAPI GecoCapabilities
{
	typedef uint CapsType;
public:
	GecoCapabilities();
	void Add(unsigned int cap);
	bool Has(unsigned int cap) const;
	bool Empty() const;
	bool Match(const GecoCapabilities& on, const GecoCapabilities& off) const;
	bool MatchAny(const GecoCapabilities& on, const GecoCapabilities& off) const;

	static const unsigned int ms_uiMaxCap = std::numeric_limits<CapsType>::digits - 1;
private:
	CapsType m_uiCaps;
};

/// This type identifies an entry in the data for a space
class GECOAPI GecoSpaceEntryID : public GecoNetAddress
{
};
INLINE bool operator==(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);
INLINE bool operator!=(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);
INLINE bool operator<(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);

struct GECOAPI InterfaceListenerMsg
{
	uint uiIP;
	ushort uiPort;
	ushort uiUserID;
	char kName[32];

	InterfaceListenerMsg(uint uiIP_, ushort uiPort_, ushort uiUserID_, const eastl::string& kName_) :
		uiIP(uiIP_), uiPort(uiPort_), uiUserID(uiUserID_)
	{
		strncpy(kName, kName_.c_str(), sizeof(kName));
	}
	InterfaceListenerMsg() :
		uiIP(0), uiPort(0), uiUserID(1)
	{
		kName[0] = 0;
	}
};
INLINE geco_bit_stream_t& operator >>(geco_bit_stream_t& kIS, InterfaceListenerMsg& kDes)
{
	if (kIS.is_compression_mode())
	{
		kIS.ReadMini(kDes.uiIP);
		kIS.ReadMini(kDes.uiPort);
		kIS.ReadMini(kDes.uiUserID);
		kIS.ReadMini(kDes.kName);
	}
	else
	{
		kIS.Read(kDes.uiIP);
		kIS.Read(kDes.uiPort);
		kIS.Read(kDes.uiUserID);
		kIS.Read(kDes.kName);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator <<(geco_bit_stream_t& kOS, const InterfaceListenerMsg& kDes)
{
	if (kOS.is_compression_mode())
	{
		kOS.WriteMini(kDes.uiIP);
		kOS.WriteMini(kDes.uiPort);
		kOS.WriteMini(kDes.uiUserID);
		kOS.WriteMini(kDes.kName);
	}
	else
	{
		kOS.Write(kDes.uiIP);
		kOS.Write(kDes.uiPort);
		kOS.Write(kDes.uiUserID);
		kOS.Write(kDes.kName);
	}
	return kOS;
}

/**
 * 	This class is used to pack a 3d vector for network transmission.
 *
 * 	@ingroup network
 */
typedef GecoVector3 GecoCoord;

struct GecoNetInterfaceElement;
/**
 *	This class declares an interface for receiving reply messages.
 *	When a client issues a request, an interface of this type should
 *	be provided to handle the reply.
 *
 *	@see Bundle::startRequest
 *	@see Bundle::startReply
 *
 *	@ingroup mercury
 */
#include <exception>
struct GecoNetInputMessageHandler
{
	virtual ~GecoNetInputMessageHandler()
	{
	}
	;

	/**
	 * 	This method is called to hndle a request,reply or normal message.
	 *
	 * 	@param from	The address at which the message originated.
	 * 	@param ier	 This contains the message type, size, and flags.
	 * 	@param msgbody		The actual message data.
	 * 	@param arg		This is user-defined data that was passed in with the request that generated this reply.
	 */
	virtual int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie,
		geco_bit_stream_t & msgbody) = 0;

	/**
	 * 	This method is called by Mercury when the request fails. The
	 * 	normal reason for this happening is a timeout.
	 *
	 * 	@param exception	The reason for failure.
	 * 	@param arg			The user-defined data associated with the request.
	 */
	virtual void handleException(const char* exception, void * arg = NULL)
	{
		throw std::logic_error("not inmplemented");
	}
	virtual void handleShuttingDown(const char* exception, void * arg = NULL)
	{
		throw std::logic_error("not inmplemented");
	}
};

/**
 *	Abstract class for a message filter.
 *
 *	A message filter has first-pass handling of messages before their
 *	destination message handlers. They are associated with channels.
 *
 *	They may or may not decide to pass, alter the message to the handler. One
 *	example of use is to implement rate-limiting of message dispatches, where
 *	message handling can be deferred.
 *
 *	Message filters are reference-counted.
 */
struct GecoMessageFilter
{
	/** Constructor. */
	GecoMessageFilter()
	{
	}

	/** Destructor. */
	virtual ~GecoMessageFilter()
	{
	}

	/**
	 *	Override this method to implement the message filter.
	 *
	 *	@param srcAddr 		The source address of the message.
	 *	@param header 		The message header.
	 *	@param data 		The message data stream.
	 *	@param pHandler 	The destination message handler.
	 *  @return true filtered otherwise flase
	 */
	virtual bool filterMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie,
		geco_bit_stream_t & msgbody)
	{
		return false;
	}
};

struct GecoNetPacket
{
	typedef ushort Flags;
	enum
	{
		FLAG_HAS_REQUESTS = 0x0001,
		FLAG_HAS_PIGGYBACKS = 0x0002,
		FLAG_HAS_ACKS = 0x0004,
		FLAG_ON_CHANNEL = 0x0008,
		FLAG_IS_RELIABLE = 0x0010,
		FLAG_IS_FRAGMENT = 0x0020,
		FLAG_HAS_SEQUENCE_NUMBER = 0x0040,
		FLAG_INDEXED_CHANNEL = 0x0080,
		FLAG_HAS_CHECKSUM = 0x0100,
		FLAG_CREATE_CHANNEL = 0x0200,
		KNOWN_FLAGS = 0x03FF
	};

	typedef ushort Offset;
	GecoNetPacket* m_spNext;
	int m_iMsgEndOffset;
	int m_iFooterSize;
	int m_iExtraFilterSize;
	Offset m_uiFirstRequestOffset;
	Offset *m_puiLastRequestOffset;
	char m_acData[PACKET_MAX_SIZE];

	char *Back()
	{
		return m_acData + m_iMsgEndOffset;
	}
	const char *Body() const
	{
		return m_acData + 2;
	} // start of first msg hdr

	GecoNetPacket* Next()
	{
		return m_spNext;
	}
	const GecoNetPacket* Next() const
	{
		return m_spNext;
	}
};

/**
 * 	@internal
 *	This structure describes a single message within an
 *	interface. It describes how to encode and decode the
 *	header of the message, including whether the message
 *	is fixed length or variable.  For fixed length messages, it
 *	defines the length of the message. For variable length
 *	messages, it defines the number of bytes needed to
 *	express the length.
 */
struct GECOAPI GecoNetInterfaceElement
{
	/**
	 * 	@internal
	 *	This structure contains the standard interface definition for a reply
	 *	message. It is used internally by Mercury.
	 */
	static const GecoNetInterfaceElement REPLY;

	/*below are all variables that are assigned values read from config file*/
	GecoNetMessageID id; ///< Unique message ID
	uchar m_uiLengthStyle; ///< Fixed or variable length
	int m_iLengthParam; ///< This depends on lengthStyle
	const char *m_pcName; ///< The name of the interface method
	bool m_ubVoidMsg; ///> when not void, m_ubReliable IS USUALLY true but can be false

	uchar m_uiFlag; ///> Indicates this is req or res message. Write bit 0 for req or 1 for res after msgid field to ostream
	ushort m_usStreamID;
	ushort m_usStreamIdentity;
	bool m_ubReliable;
	bool m_ubOrderable;

	GecoNetInputMessageHandler *m_pkHandler; ///< decode args carried in the msg body and call real function

	int SpecialExpandLength(void * header, GecoNetPacket * pPacket) const;
	int SpecialCompressLength(void * header, int length, GecoNetBundle & bundle) const;

	GecoNetInterfaceElement(const char * name = "", GecoNetMessageID id = 0, uchar lengthStyle = INVALID_MESSAGE,
		int lengthParam = 0, GecoNetInputMessageHandler * pHandler = NULL) :
		id(id), m_uiLengthStyle(lengthStyle), m_iLengthParam(lengthParam), m_pcName(name), m_pkHandler(pHandler)
	{
	}

	void Set(const char * name, GecoNetMessageID idd, uchar lengthStyle, int lengthParam)
	{
		id = idd;
		m_uiLengthStyle = lengthStyle;
		m_iLengthParam = lengthParam;
		m_pcName = name;
	}
	/**
	 * 	This method returns the number of bytes occupied by a header
	 * 	for this type of message.
	 *
	 * 	@return Number of bytes needed for this header.
	 */
	int HeaderSize() const;
	/**
	 * 	This method returns the number of bytes nominally occupied by the body
	 * 	of this type of message. never guesses for variable-length messages
	 *
	 * 	@return Number of bytes lengthParam_ if fixed msg otherwise 0.
	 */
	int NominalBodySize() const;
	/**
	 * 	This method compresses a length into the given header.
	 *
	 *	@param[in] data		Pointer to the header
	 *	@param[in] length	Length to compress
	 *
	 *	@return 0 if successful.
	 */
	int CompressLength(void * header, int length, GecoNetBundle & bundle) const;
	int ExpandLength(void * header, GecoNetPacket * pPacket) const;
	/**
	 * This method returns bool that indicates if  m_iLengthParam field has enough bytes to
	 * represent the length of msg body.
	 *
	 * @param[in] length: the length of message body in bytes
	 */
	bool CanHandleLength(int length) const
	{
		return (
			/*true if it is fix length msg */
			m_uiLengthStyle != VARIABLE_LENGTH_MESSAGE) ||
			/*true if  it is variable length msg and its m_iLengthParam is big enough(greater than sizeof(int32))*/
			(m_iLengthParam >= int(sizeof(int32))) ||
			/*true if it is variable length msg and its m_iLengthParam is big enough(greater than @param[in] length)*/
			(length < (1 << (8 * m_iLengthParam)) - 1);
	}

	GecoNetMessageID GetID() const
	{
		return id;
	}
	void SetID(GecoNetMessageID id)
	{
		this->id = id;
	}

	uchar LengthStyle() const
	{
		return m_uiLengthStyle;
	}
	int LengthParam() const
	{
		return m_iLengthParam;
	}

	const char *GetName() const
	{
		return m_pcName;
	}
	const char *c_str() const
	{
		static char buf[256];
		geco_snprintf(buf, sizeof(buf), "%s/%d", m_pcName, id);
		return buf;
	}

	GecoNetInputMessageHandler *GetHandler() const
	{
		return m_pkHandler;
	}
	void SetHandler(GecoNetInputMessageHandler * pHandler)
	{
		m_pkHandler = pHandler;
	}
};
INLINE int GecoNetInterfaceElement::CompressLength(void * header, int length, GecoNetBundle & bundle) const
{
	switch (m_uiLengthStyle)
	{
	case FIXED_LENGTH_MESSAGE:
		if (length != m_iLengthParam)
		{
			//network_logger()()->critical("GecoNetInterfaceElement::CompressLength( {} ):" "Fixed length message has wrong length ({} instead of {})",this->c_str(), length, m_iLengthParam);
		}
		break;
	case VARIABLE_LENGTH_MESSAGE:
		// Beware of overflow in length, not sure if this actually happens in
		// practice, but it definitely happens on the expandLength() side of
		// things so it's probably worth having this check here
		if (length < 0)
		{
			//network_logger()()->critical("GecoNetInterfaceElement::CompressLength({}): " "Possible overflow in length ({} bytes) for " "variable length message",this->c_str(), length);
		}
		char *pLen = ((char*)header) + 1; // advance to the start position of msg body
		int len = length;
		bool oversize = false; // false means len is lee than max value that m_iLengthParam can represent
		switch (m_iLengthParam)
		{
		case 1:
			*(uchar*)pLen = (uchar)len;
			if (len >= UINT8_MAX)
				oversize = true;
			break;
		case 2:
			*(ushort*)pLen = GECO_HTONS(len);
			if (len >= UINT16_MAX)
				oversize = true;
			break;
		case 3:
			GECO_PACK3(pLen, len);
			if (len >= 0xffffff)
				oversize = true;
			break;
		case 4:
			*(uint*)pLen = GECO_HTONL((uint32)len);
			break;
		default:
			//network_logger()()->critical("InterfaceElement::compressLength( {} ): " "Unsupported variable length width: {}", this->c_str(), m_iLengthParam);
			break;
		}
		// If the message length could not fit into a standard length field, we
		// need to handle this as a special case.
		if (oversize)
		{
			//return this->SpecialCompressLength(data, length, bundle);
		}
		break;
	}
}

#include "networkstats.h"

const float INTERFACE_ELEMENT_STAT_AVERAGE_BIAS = -2.f / (5 + 1);
class GECOAPI GecoNetInterfaceElementWithStats : public GecoNetInterfaceElement
{
	/// The maximum bytes received for a single message for this interface element.
	uint64 maxBytesReceived_;
	/// The number of bytes received over all messages for this interface element.
	uint64 numBytesReceived_;
	/// The number of messages received for this interface element.
	uint64 numMessagesReceived_;
	/// The per-second exponentially weighted moving average for bytes receivedfor this interface element./
	cumulative_ema_t<uint> avgBytesReceivedPerSecond_;
	/// The per-second exponentially weighted moving average for messages received for this interface element.
	cumulative_ema_t<uint> avgMessagesReceivedPerSecond_;
	ProfileVal profile_;

public:
	GecoNetInterfaceElementWithStats(const char * name = "", GecoNetMessageID id = 0, uchar lengthStyle =
		INVALID_MESSAGE, int lengthParam = 0, GecoNetInputMessageHandler * pHandler = NULL) :
		GecoNetInterfaceElement(name, id, lengthStyle, lengthParam, pHandler), maxBytesReceived_(0), numBytesReceived_(
			0), numMessagesReceived_(0), avgBytesReceivedPerSecond_(INTERFACE_ELEMENT_STAT_AVERAGE_BIAS), avgMessagesReceivedPerSecond_(
				INTERFACE_ELEMENT_STAT_AVERAGE_BIAS)
	{
	}
	//  called every second.
	void tick()
	{
		avgBytesReceivedPerSecond_.sample();
		avgMessagesReceivedPerSecond_.sample();
	}
	uint64 MaxBytesReceived() const
	{
		return maxBytesReceived_;
	}
	uint64 NumBytesReceived() const
	{
		return numBytesReceived_;
	}
	uint64 NumMessagesReceived() const
	{
		return numMessagesReceived_;
	}
	float AvgBytesReceivedPerSecond() const
	{
		return avgBytesReceivedPerSecond_.average();
	}
	float AvgMessagesReceivedPerSecond() const
	{
		return avgMessagesReceivedPerSecond_.average();
	}
	float AvgMessageLength() const
	{
		return numMessagesReceived_ ? float(numBytesReceived_) / float(numMessagesReceived_) : 0.0f;
	}
	void startProfile()
	{
		profile_.start();
	}

	void stopProfile(uint32 msgLen)
	{
		profile_.stop(msgLen);

		++avgMessagesReceivedPerSecond_.value();
		avgBytesReceivedPerSecond_.value() += msgLen;

		++numMessagesReceived_;
		numBytesReceived_ += msgLen;
		maxBytesReceived_ = msgLen > maxBytesReceived_ ? msgLen : maxBytesReceived_;
	}
	GecoWatcher* GetWatcher();
};

/**
 *	This class is used to pack a yaw and pitch value for network transmission.
 *
 *	@ingroup network
 */
class GECOAPI GecoYawPitch
{
public:
	GecoYawPitch(float yaw, float pitch)
	{
		this->Set(yaw, pitch);
	}
	GecoYawPitch()
	{
	}
	;
	void Set(float yaw, float pitch)
	{
		m_uiYaw = AngleToInt8(yaw);
		m_uiPitch = HalfAngleToInt8(pitch);
	}
	void Get(float & yaw, float & pitch) const
	{
		yaw = Int8ToAngle(m_uiYaw);
		pitch = Int8ToHalfAngle(m_uiPitch);
	}
	uchar m_uiYaw;
	uchar m_uiPitch;
};
INLINE geco_bit_stream_t & operator >>(geco_bit_stream_t& kIS, GecoYawPitch& kDes)
{
	if (kIS.is_compression_mode())
	{
		kIS.ReadMini(kDes.m_uiYaw);
		kIS.ReadMini(kDes.m_uiPitch);
	}
	else
	{
		kIS.Read(kDes.m_uiYaw);
		kIS.Read(kDes.m_uiPitch);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator <<(geco_bit_stream_t& kOS, const GecoYawPitch& kDes)
{
	if (kOS.is_compression_mode())
	{
		kOS.WriteMini(kDes.m_uiYaw);
		kOS.WriteMini(kDes.m_uiPitch);
	}
	else
	{
		kOS.Write(kDes.m_uiYaw);
		kOS.Write(kDes.m_uiPitch);
	}
	return kOS;
}

class GECOAPI GecoYawPitchRoll
{
public:
	GecoYawPitchRoll(float yaw, float pitch, float roll)
	{
		this->Set(yaw, pitch, roll);
	}

	GecoYawPitchRoll()
	{
	}
	;

	bool operator==(const GecoYawPitchRoll & oth) const
	{
		return m_uiYaw == oth.m_uiYaw && m_uiPitch == oth.m_uiPitch && m_uiRoll == oth.m_uiRoll;
	}

	void Set(float yaw, float pitch, float roll)
	{
		m_uiYaw = AngleToInt8(yaw);
		m_uiPitch = HalfAngleToInt8(pitch);
		m_uiRoll = AngleToInt8(roll);
	}

	void Get(float & yaw, float & pitch, float & roll) const
	{
		yaw = Int8ToAngle(m_uiYaw);
		pitch = Int8ToHalfAngle(m_uiPitch);
		roll = Int8ToAngle(m_uiRoll);
	}

	bool NearTo(const GecoYawPitchRoll & oth) const
	{
		uint val = (m_uiYaw - oth.m_uiYaw + 1) | (m_uiPitch - oth.m_uiPitch + 1) | (m_uiRoll - oth.m_uiRoll + 1);
		return val <= 3;
	}

	uchar m_uiYaw;
	uchar m_uiPitch;
	uchar m_uiRoll;
};
INLINE geco_bit_stream_t & operator >>(geco_bit_stream_t& kIS, GecoYawPitchRoll& kDes)
{
	if (kIS.is_compression_mode())
	{
		kIS.ReadMini(kDes.m_uiYaw);
		kIS.ReadMini(kDes.m_uiPitch);
		kIS.ReadMini(kDes.m_uiRoll);
	}
	else
	{
		kIS.Read(kDes.m_uiYaw);
		kIS.Read(kDes.m_uiPitch);
		kIS.Read(kDes.m_uiRoll);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator <<(geco_bit_stream_t& kOS, const GecoYawPitchRoll& kDes)
{
	if (kOS.is_compression_mode())
	{
		kOS.WriteMini(kDes.m_uiYaw);
		kOS.WriteMini(kDes.m_uiPitch);
		kOS.WriteMini(kDes.m_uiRoll);
	}
	else
	{
		kOS.Write(kDes.m_uiYaw);
		kOS.Write(kDes.m_uiRoll);
		kOS.Write(kDes.m_uiRoll);
	}
	return kOS;
}

class GecoPackedXY
{
protected:
	union MultiType
	{
		float asFloat;
		unsigned int asUint;
		int asInt;
	};

public:
	GecoPackedXY()
	{
	}
	;
	GecoPackedXY(float x, float y)
	{
		this->PackXY(x, y);
	}

	/**
	 *	This function compresses the two input floats into the low 3 bytes of the
	 *	return value. It does this as follows:
	 *
	 *	Bits 12-23 can be thought of as a 12-bit float storing the x-value.
	 *	Bits 0-11 can be thought of as a 12-bit float storing the z-value.
	 *	Bits 0-7 are the mantissa, bits 8-10 are the unsigned exponent and bit 11 is
	 *	the sign bit.
	 *
	 *	The input values must be in the range (-510, 510).
	 */
	void PackXY(float xValue, float yValue)
	{
		const static float addValues[] = { 2.f, -2.f };
		const uint xCeilingValues[] = { 0, 0x7ff000 };
		const uint yCeilingValues[] = { 0, 0x0007ff };

		GecoPackedXY::MultiType x;
		x.asFloat = xValue;
		GecoPackedXY::MultiType y;
		y.asFloat = yValue;

		// We want the value to be in the range (-512, -2], [2, 512). Take 2 from
		// negative numbers and add 2 to positive numbers. This is to make the
		// exponent be in the range [2, 9] which for the exponent of the float in
		// IEEE format is between 10000000 and 10000111.
		x.asFloat += addValues[x.asInt < 0];
		y.asFloat += addValues[y.asInt < 0];

		unsigned int result = 0;

		// Here we check if the input values are out of range. The exponent is meant
		// to be between 10000000 and 10000111. We check that the top 5 bits are
		// 10000.
		// We also need to check for the case that the rounding causes an overflow.
		// This occurs when the bits of the exponent and mantissa we are interested
		// in are all 1 and the next significant bit in the mantissa is 1.
		// If an overflow occurs, we set the result to the maximum result.
		result |= xCeilingValues[((x.asUint & 0x7c000000) != 0x40000000) || ((x.asUint & 0x3ffc000) == 0x3ffc000)];
		result |= yCeilingValues[((y.asUint & 0x7c000000) != 0x40000000) || ((y.asUint & 0x3ffc000) == 0x3ffc000)];

		// Copy the low 3 bits of the exponent and the high 8 bits of the mantissa.
		// These are the bits 15 - 25. We then add one to this value if the high bit
		// of the remainder of the mantissa is set. It magically works that if the
		// mantissa wraps around, the exponent is incremented by one as required.
		result |= ((x.asUint >> 3) & 0x7ff000) + ((x.asUint & 0x4000) >> 2);
		result |= ((y.asUint >> 15) & 0x0007ff) + ((y.asUint & 0x4000) >> 14);

		// We only need this for values in the range [509.5, 510.0). For these
		// values, the above addition overflows to the sign bit.
		result &= 0x7ff7ff;

		result |= (x.asUint >> 8) & 0x800000;
		result |= (y.asUint >> 20) & 0x000800;

		GECO_PACK3((char*)m_acData, result);
	}
	;

	/**
	 *	This function uncompresses the values that were created by pack.
	 */
	void UnpackXY(float & xarg, float & yarg) const
	{
		unsigned int data = GECO_UNPACK3((const char*)m_acData);

		MultiType & xu = (MultiType&)xarg;
		MultiType & yu = (MultiType&)yarg;

		// The high 5 bits of the exponent are 10000. The low 17 bits of the
		// mantissa are all clear.
		xu.asUint = 0x40000000;
		yu.asUint = 0x40000000;

		// Copy the low 3 bits of the exponent and the high 8 bits of the mantissa
		// into the results.
		xu.asUint |= (data & 0x7ff000) << 3;
		yu.asUint |= (data & 0x0007ff) << 15;

		// The produced floats are in the range (-512, -2], [2, 512). Change this
		// back to the range (-510, 510). [Note: the sign bit is not on yet.]
		xu.asFloat -= 2.0f;
		yu.asFloat -= 2.0f;

		// Copy the sign bit across.
		xu.asUint |= (data & 0x800000) << 8;
		yu.asUint |= (data & 0x000800) << 20;
	}

	/**
	 *	This method returns the maximum error in the x and z components caused by
	 *	compression.
	 */
	void GetXYError(float & xError, float & yError) const
	{
		uint data = GECO_UNPACK3((const char*)m_acData);

		// The exponents are the 3 bits from bit 8.
		int xExp = (data >> 20) & 0x7;
		int yExp = (data >> 8) & 0x7;

		// The first range is 2 metres and the mantissa is 8 bits. This makes each
		// step 2/(2**8). The x and z compression rounds and so the error can be
		// up to half step size. The error doubles for each additional range.
		xError = (1.f / 256.f) * (1 << xExp);
		yError = (1.f / 256.f) * (1 << yExp);
	}

	unsigned char m_acData[3];
};

INLINE geco_bit_stream_t& operator >>(geco_bit_stream_t& is, GecoPackedXY &xy)
{
	assert(sizeof(xy.m_acData) == 3);
	is.ReadThreeAlignedBytes(reinterpret_cast<char*>(xy.m_acData));
	return is;
}
INLINE geco_bit_stream_t& operator<<(geco_bit_stream_t& os, const GecoPackedXY &xy)
{
	assert(sizeof(xy.m_acData) == 3);
	os.WriteThreeAlignedBytes(reinterpret_cast<const char*>(xy.m_acData));
	return os;
}

// -----------------------------------------------------------------------------
// Section: class PackedXHZ
// -----------------------------------------------------------------------------

/**
 *  This is just a placeholder at the moment. The idea is that we send down
 *  a message that identifies the chunk that the entity is in and the approx.
 *  height within that chunk so that the client can find the correct level.
 */
class GecoPackedXHZ : public GecoPackedXY
{
public:
	GecoPackedXHZ() :
		height_(0)
	{
	}
	;
private:
	int8 height_;
};

/**
 *	This class is used to store a packed x, y, z coordinate. At the moment, it
 *	stores the z value as a 16 bit float with a 4 bit exponent and 11 bit
 *	mantissa. This is a bit of overkill. It can handle values in the range
 *	(-131070.0, 131070.0)
 */
class GecoPackedXYZ : public GecoPackedXY
{
public:
	GecoPackedXYZ()
	{
	}
	GecoPackedXYZ(float x, float y, float z)
	{
		this->PackXY(x, y);
		this->SetZ(z);
	}
	/**
	 *	This method packs the three input floats into this object.
	 */
	void PackXYZ(float x, float y, float z)
	{
		this->SetZ(z);
		this->PackXY(x, y);
	}
	/**
	 *	This method unpacks this object into the three input floats.
	 */
	void UnpackXYZ(float & x, float & y, float & z) const
	{
		z = this->GetZ();
		this->UnpackXY(x, y);
	}

	/**
	 *	This method returns the maximum error in the x, y and z components caused by
	 *	compression.
	 */
	float ZError() const
	{
		ushort& zDataInt = *(ushort*)m_acZData;

		int zExp = (zDataInt >> 11) & 0xf;

		return (2.f / 2048.f) * (1 << zExp);
	}

	/**
	 *	This method sets the packed y value from a float.
	 */
	void SetZ(float fZ)
	{
		// range checking is done on the input value or rounding of the result. (-131070.0, 131070.0)
		assert(fZ > -131070.0f && fZ < 131070.0f);

		// Add bias to the value to force the floating point exponent to be in the
		// range [2, 15], which translates to an IEEE754 exponent in the range
		// 10000000 to 1000FFFF (which contains a +127 bias).
		// Thus only the least significant 4 bits of the exponent need to be
		// stored.
		const static float addValues[] = { 2.f, -2.f };
		MultiType z;
		z.asFloat = fZ;
		z.asFloat += addValues[z.asInt < 0];

		ushort& zDataInt = *(ushort*)m_acZData;

		// Extract the lower 4 bits of the exponent, and the 11 most significant
		// bits of the mantissa (15 bits all up).
		zDataInt = (z.asUint >> 12) & 0x7fff;

		// Transfer the sign bit.
		zDataInt |= ((z.asUint >> 16) & 0x8000);
	}

	union MultiType
	{
		float asFloat;
		unsigned int asUint;
		int asInt;
	};
	/**
	 *	This method returns the packed z value as a float.
	 */
	float GetZ() const
	{
		// Preload output value with 0(sign) 10000000(exponent) 00..(mantissa).
		MultiType z;
		z.asUint = 0x40000000;

		ushort& zDataInt = *(ushort*)m_acZData;

		// Copy the 4-bit lower 4 bits of the exponent and the
		// 11 most significant bits of the mantissa.
		z.asUint |= (zDataInt & 0x7fff) << 12;

		// Remove value bias.
		z.asFloat -= 2.f;

		// Copy the sign bit.
		z.asUint |= (zDataInt & 0x8000) << 16;

		return z.asFloat;
	}
	union
	{
		unsigned char m_acZData[2];
		ushort m_acZShort;
	};
};
INLINE geco_bit_stream_t& operator >>(geco_bit_stream_t& is, GecoPackedXYZ &xy)
{
	assert(sizeof(xy.m_acData) == 3);
	is.ReadThreeAlignedBytes(reinterpret_cast<char*>(xy.m_acData));
	is.Read(xy.m_acZShort);
	return is;
}
INLINE geco_bit_stream_t& operator<<(geco_bit_stream_t& os, const GecoPackedXYZ &xy)
{
	assert(sizeof(xy.m_acData) == 3);
	os.WriteThreeAlignedBytes(reinterpret_cast<const char*>(xy.m_acData));
	os.Write(xy.m_acZShort);
	return os;
}

INLINE GecoNetAddress::GecoNetAddress()
{
	geco_zero_mem(&su, sizeof(sockaddrunion));
	m_uiSalt = 0;
}
INLINE GecoNetAddress::GecoNetAddress(ushort port, const char* addrstr) :
	m_uiSalt(0)
{
	str2saddr(&su, addrstr, port);
}
INLINE bool operator==(const GecoNetAddress & a, const GecoNetAddress & b)
{
	return saddr_equals(&a.su, &b.su);
}
INLINE bool operator!=(const GecoNetAddress & a, const GecoNetAddress & b)
{
	return !saddr_equals(&a.su, &b.su);
}
INLINE bool operator==(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b)
{
	return (a.m_iID == b.m_iID) && (a.m_kAddr == b.m_kAddr) && (a.m_kAddr.m_uiSalt == b.m_kAddr.m_uiSalt);
}
INLINE bool operator!=(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b)
{
	return (a.m_iID != b.m_iID) || (a.m_kAddr != b.m_kAddr) || (a.m_kAddr.m_uiSalt != b.m_kAddr.m_uiSalt);
}
INLINE GecoCapabilities::GecoCapabilities() :
	m_uiCaps(0)
{
}
INLINE void GecoCapabilities::Add(unsigned int cap)
{
	m_uiCaps |= 1 << cap;
}
INLINE bool GecoCapabilities::Has(unsigned int cap) const
{
	return !!(m_uiCaps & (1 << cap));
}
INLINE bool GecoCapabilities::Empty() const
{
	return m_uiCaps == 0;
}
INLINE bool GecoCapabilities::Match(const GecoCapabilities& on, const GecoCapabilities& off) const
{
	return (on.m_uiCaps & m_uiCaps) == on.m_uiCaps && (off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
}
INLINE bool GecoCapabilities::MatchAny(const GecoCapabilities& on, const GecoCapabilities& off) const
{
	return !!(on.m_uiCaps & m_uiCaps) && (off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
}
INLINE bool operator==(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b)
{
	return operator==((GecoNetAddress)a, (GecoNetAddress)b) && a.m_uiSalt == b.m_uiSalt;
}
INLINE bool operator!=(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b)
{
	return operator!=((GecoNetAddress)a, (GecoNetAddress)b) || a.m_uiSalt != b.m_uiSalt;
}

class GecoNetworkInterface
{
public:
	INLINE const GecoNetAddress & address() const
	{
		return GecoNetAddress();
	}
};

/**
 * 	The InterfaceMinder class manages a set of interface elements. It provides
 * 	an iterator for iterating over this set.
 *
 * 	@ingroup mercury
 */
struct GecoInterfaceMinder
{
	eastl::vector<GecoNetInterfaceElementWithStats> elements_;
	const char * name_;

	/**
	 * 	This is the default constructor.
	 * 	It initialises the InterfaceMinder without any interfaces.
	 *
	 * 	@param name	Name of the interface.
	 */
	GecoInterfaceMinder(const char * name) :
		name_(name)
	{
		elements_.reserve(256);
	}

	/**
	 * 	This method adds an msg handler to the interface minder.
	 *  @param name             Name of the interface element.
	 * 	@param lengthStyle		Specifies whether the message is fixed or variable.
	 *	@param lengthParam		This depends on lengthStyle.
	 *	@param pHandler			The message handler for this msg.
	 */
	GecoNetInterfaceElementWithStats & add(const char * name, int8 lengthStyle, int lengthParam,
		GecoNetInputMessageHandler * pHandler = NULL)
	{
		// Set up the new bucket and add it to the list
		void* dest = elements_.push_back_uninitialized();
		return *(new (dest) GecoNetInterfaceElementWithStats(name, elements_.size() - 1, lengthStyle, lengthParam,
			pHandler));
	}

	/**
	 * 	This method returns the handler for the given interface.
	 *
	 * 	@param index	Index of the interface required.
	 *
	 * 	@return The handler for the given interface index.
	 */
	GecoNetInputMessageHandler * handler(int index)
	{
		return elements_[index].GetHandler();
	}

	/**
	 * 	This method sets the handler for a given interface.
	 *
	 * 	@param index	The index for which to set a handler
	 * 	@param pHandler	A message handler object
	 */
	void handler(int index, GecoNetInputMessageHandler * pHandler)
	{
		elements_[index].SetHandler(pHandler);
	}

	/**
	 *	This method returns the InterfaceElement with the input id.
	 */
	const GecoNetInterfaceElement & interfaceElement(uint8 id) const
	{
		return elements_[id];
	}

	void registerWithInterface(GecoNetworkInterface & networkInterface)
	{

	}
	GecoNetReason registerWithMachined(const GecoNetAddress & addr, int id) const
	{
		return GecoNetReason::GECO_NET_REASON_CHANNEL_LOST;
	}
};

extern void InitNetwork();

#endif // __GecoNetTypes_H__
