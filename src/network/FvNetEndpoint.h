//{future header message}
#ifndef __FvNetEndpoint_H__
#define __FvNetEndpoint_H__

#include "../common/geco-plateform.h"

//#include "FvNetTypes.h"

//#include <FvDebug.h>
//#include <FvBinaryStream.h>

#include <map>

#include <sys/types.h>
#if defined( unix ) || defined( PLAYSTATION3 )
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
	#include <unistd.h>
#else
	#include <WINSOCK.H>
#endif
#include <errno.h>
#include <stdlib.h>

#ifndef unix

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

class FvBinaryIStream;

class GECOAPI FvNetEndpoint
{
public:
	FvNetEndpoint( bool useSyncHijack = true );
	~FvNetEndpoint();

	static const int NO_SOCKET = -1;

	operator int() const;
	void SetFileDescriptor(int fd);
	bool Good() const;

	void Socket( int type );

	int SetNonblocking( bool nonblocking );
	int SetBroadcast( bool broadcast );
	int SetReuseaddr( bool reuseaddr );
	int SetKeepalive( bool keepalive );

	int Bind( u_int16_t networkPort = 0, u_int32_t networkAddr = INADDR_ANY );

	int JoinMulticastGroup( u_int32_t networkAddr );
	int QuitMulticastGroup( u_int32_t networkAddr );

	int Close();
	int Detach();

	int GetLocalAddress(
		u_int16_t * networkPort, u_int32_t * networkAddr ) const;
	int GetRemoteAddress(
		u_int16_t * networkPort, u_int32_t * networkAddr ) const;

	const char * c_str() const;
	int GetRemoteHostname( FvString * name ) const;

	bool GetClosedPort( FvNetAddress & closedPort );

	int SendTo( void * gramData, int gramSize,
		u_int16_t networkPort, u_int32_t networkAddr = FV_NET_BROADCAST);
	int SendTo( void * gramData, int gramSize, struct sockaddr_in & sin );
	int RecvFrom( void * gramData, int gramSize,
		u_int16_t * networkPort, u_int32_t * networkAddr );
	int RecvFrom( void * gramData, int gramSize,
		struct sockaddr_in & sin );

	int Listen( int backlog = 5 );
	int Connect( u_int16_t networkPort, u_int32_t networkAddr = FV_NET_BROADCAST );
	FvNetEndpoint * Accept(
		u_int16_t * networkPort = NULL, u_int32_t * networkAddr = NULL );

	int Send( const void * gramData, int gramSize );
	int Recv( void * gramData, int gramSize );
	bool RecvAll( void * gramData, int gramSize );

	int GetInterfaceFlags( char * name, int & flags );
	int GetInterfaceAddress( const char * name, u_int32_t & address );
	int GetInterfaceNetmask( const char * name, u_int32_t & netmask );
	bool GetInterfaces( std::map< u_int32_t, FvString > &interfaces );
	int FindDefaultInterface( char * name );
	int FindIndicatedInterface( const char * spec, char * name );
	static int ConvertAddress( const char * string, u_int32_t & address );

	int TransmitQueueSize() const;
	int ReceiveQueueSize() const;
	int GetQueueSizes( int & tx, int & rx ) const;

	int GetBufferSize( int optname ) const;
	bool SetBufferSize( int optname, int size );

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

	static void SetHijackEndpoints( FvNetEndpoint * pSync, FvNetEndpoint * pAsync );
	static void SetHijackStream( FvBinaryIStream * pNewStream )
			{ ms_pkHijackStream = pNewStream; }

	void HijackSendOpen( u_int16_t port, u_int32_t addr );
	void HijackSendClose();
	bool HijackRecvAllFrom( void * gramData, int gramSize,
		u_int16_t * pPort, u_int32_t * pAddr );
	int HijackFD() const;

	static bool IsHijacked()	{ return ms_pkHijackEndpointAsync != NULL; }
	static void AddFrontEndInterface( const FvString & name,
			u_int32_t addr );

private:

#if defined( unix ) || defined( PLAYSTATION3 )
	int	m_kSocket;
#else //ifdef unix
	SOCKET	m_kSocket;
#endif //def _WIN32

	bool m_bUseSyncHijack;

	bool m_bShouldSendClose;

	u_int16_t m_uiHijackPort;
	u_int32_t m_uiHijackAddr;

	FvNetEndpoint *m_pkHijackEndpoint;

	static FvNetEndpoint *ms_pkHijackEndpointAsync;
	static FvNetEndpoint *ms_pkHijackEndpointSync;

	static FvBinaryIStream *ms_pkHijackStream;

	typedef std::map< FvString, u_int32_t > FrontEndInterfaces;
	static FrontEndInterfaces ms_kFrontEndInterfaces;
};

extern void InitNetwork();

#include "FvNetEndpoint.inl"

#endif // __FvNetEndpoint_H__
