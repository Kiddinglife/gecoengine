/*
 * nettypes.cpp
 *
 *  Created on: 2Sep.,2016
 *      Author: jackiez
 */
#include "nettypes.h"
#include "../common/debugging/gecowatchert.h"
#include "../protocolstack/transport_layer.h"

#ifdef __unix__
extern "C" {
#include <linux/errqueue.h>
#define get_if_nameindex if_nameindex
}
#include <net/if.h>
#include <signal.h>
#include <sys/uio.h>
#include <netinet/ip.h>
#else	// not unix
 // Need to implement if_nameindex functions on Windows
 /** @internal */
 struct if_nameindex
{
	unsigned int if_index;	/* 1, 2, ... */
	char *if_name;			/* null terminated name: "eth0", ... */
};
struct if_nameindex *get_if_nameindex(void)
{
	static struct if_nameindex staticIfList[3] =
	{ { 1, "eth0" },{ 2, "lo" },{ 0, 0 } };
	return staticIfList;
}

/** @internal */
inline void if_freenameindex(struct if_nameindex *)
{ }
#endif	// not unix

#ifdef __unix__
int geco_sock_base_t::getInterfaceFlags(char * name, int & flags)
{
	struct ifreq request;
	strncpy(request.ifr_name, name, IFNAMSIZ);
	if( ioctl(socket_, SIOCGIFFLAGS, &request) != 0 )
	{
		return -1;
	}

	flags = request.ifr_flags;
	return 0;
}
int geco_sock_base_t::getInterfaceAddress(const char * name, sockaddrunion& outaddress)
{
	struct ifreq request;
	strncpy(request.ifr_name, name, IFNAMSIZ);
	if( ioctl(socket_, SIOCGIFADDR, &request) != 0 )
	{
		return -1;
	}
	outaddress.sa = request.ifr_addr;
	return 0;
}
int geco_sock_base_t::getInterfaceNetmask(const char * name, sockaddrunion & netmask)
{
	struct ifreq request;
	memset(&request, 0, sizeof(request));
	strncpy(request.ifr_name, name, IFNAMSIZ);
	if( ioctl(socket_, SIOCGIFNETMASK, &request) != 0 )
	{
		return -1;
	}
	netmask.sa = request.ifr_netmask;
	return 0;
}
#else
// *    @return 0 if successful, 1 otherwise.
int geco_sock_base_t::getInterfaceFlags(char * name, int & flags)
{
	if( !strcmp(name, "eth0") )
	{
		flags = IFF_UP | IFF_BROADCAST | IFF_MULTICAST;
		return 0;
	} else if( !strcmp(name, "lo") )
	{
		flags = IFF_UP | IFF_LOOPBACK;
		return 0;
	}
	return -1;
}
/**
 *  This method returns the address to which an interface is bound.
 *
 *  @param name     Name of the interface.
 *  @param address  The address is returned here.
 *
 *  @return 0 if successful, 1 otherwise.
 */
int geco_sock_base_t::getInterfaceAddress(const char * name, sockaddrunion & address)
{
	if( !strcmp(name, "eth0") )
	{
#if defined( PLAYSTATION3 )
		CellNetCtlInfo netInfo;
		int ret = cellNetCtlGetInfo(CELL_NET_CTL_INFO_IP_ADDRESS, &netInfo);
		MF_ASSERT(ret == 0);
		int ip0, ip1, ip2, ip3;
		sscanf(netInfo.ip_address, "%d.%d.%d.%d", &ip0, &ip1, &ip2, &ip3);
		address = ( ip0 << 24 ) | ( ip1 << 16 ) | ( ip2 << 8 ) | ( ip3 << 0 );
#else
		char myname[256];
		::gethostname(myname, sizeof(myname));

		struct hostent * myhost = gethostbyname(myname);
		if( !myhost )
		{
			return -1;
		}
		memcpy(&address.sa.sa_data, myhost->h_addr_list[0], sizeof(address.sa.sa_data));
		//address = ((struct in_addr*)(myhost->h_addr_list[0]))->s_addr;
#endif
		return 0;
	} else if( !strcmp(name, "lo") )
	{
		address.sin.sin_addr.s_addr = htonl(0x7F000001);
		//address = htonl(0x7F000001);
		return 0;
	}

	return -1;
}
int geco_sock_base_t::getInterfaceNetmask(const char * name, sockaddrunion & netmask)
{
	return 0;
}
#endif

int geco_tcp_sock_t::getremotehostname(std::string * out_name) const
{
	sockaddrunion out_addr;
	socklen_t sinLen = sizeof(out_addr.sa);
	int ret = ::getpeername(socket_, &out_addr.sa, &sinLen);
	if( ret == 0 )
	{
		hostent* h;
		out_addr.sa.sa_family == AF_INET ?
			h = gethostbyaddr((char*) &out_addr.sin.sin_addr, sizeof(in_addr), AF_INET) : h =
			gethostbyaddr((char*) &out_addr.sin6.sin6_addr, sizeof(in6_addr), AF_INET6);

		if( h != 0 )
		{
			*out_name = h->h_name;
		} else
		{
			ret = -1;
		}
	}
	return ret;
}

bool geco_sock_base_t::getInterfaces(interfaces_map_t &interfaces)
{
	// Find a list of all of our interfaces
	struct if_nameindex* pIfInfo = get_if_nameindex();
	if( !pIfInfo )
	{
		//ERROR_MSG("Unable to discover network interfaces.\n");
		return false;
	}

	int flags = 0;
	struct if_nameindex* pIfInfoCur = pIfInfo;
	while( pIfInfoCur->if_name )
	{
		flags = 0;
		this->getInterfaceFlags(pIfInfoCur->if_name, flags);
		if( ( flags & IFF_UP ) )
		{
			sockaddrunion addr;
			if( this->getInterfaceAddress(pIfInfoCur->if_name, addr) == 0 )
			{
				interfaces[addr] = pIfInfoCur->if_name;
			}
		}
		++pIfInfoCur;
	}
	if_freenameindex(pIfInfo);

	return true;
}

/**
 *  This function finds the interfaced specified by a string. The
 *  specification may take the form of a straight interface name,
 *  a IP address (name/dotted decimal), or a netmask (IP/bits).
 */
int geco_sock_base_t::findIndicatedInterface(const char * spec, int af, char * name)
{
	// start with it cleared
	name[0] = 0;

	// make sure there's something there
	if( spec == NULL || spec[0] == 0 ) return -1;

	// set up some working vars
	char * slash;
	int netmaskbits = 32;
	char iftemp[IFNAMSIZ + 16];
	strncpy(iftemp, spec, IFNAMSIZ);
	iftemp[IFNAMSIZ] = 0;
	//uint addr = 0;
	sockaddrunion addr =
	{ 0 };
	// see if it's a netmask
	if( ( slash = const_cast<char *>( strchr(spec, '/') ) ) && slash - spec <= 16 )
	{
		// specified a netmask
		assert(IFNAMSIZ >= 16);
		iftemp[slash - spec] = 0;
		bool ok = str2saddr(&addr, iftemp, 0, af == AF_INET);

		netmaskbits = atoi(slash + 1);
		ok &= netmaskbits > 0 && netmaskbits <= 32;

		if( !ok )
		{
			ERROR_MSG("Endpoint::findIndicatedInterface: "
				"netmask match %s length %s is not valid.\n", iftemp, slash + 1);
			return -1;
		}
	} else if( this->getInterfaceAddress(iftemp, addr) == 0 )
	{
		// specified name of interface
		strncpy(name, iftemp, IFNAMSIZ);
	} else if( str2saddr(&addr, spec, 0, af == AF_INET) )
	{
		// specified ip address
		netmaskbits = 32;// redundant but instructive
	} else
	{
		ERROR_MSG("Endpoint::findIndicatedInterface: "
			"No interface matching interface spec '%s' found\n", spec);
		return -1;
	}

	// if we haven't set a name yet then we're supposed to
	// look up the ip address
	if( name[0] == 0 )
	{
		int netmaskshift = 32 - netmaskbits;
		uint netmaskmatch = ntohl(addr.sin.sin_addr.s_addr);

		std::vector<std::string> interfaceNames;

		struct if_nameindex* pIfInfo = get_if_nameindex();
		if( pIfInfo )
		{
			struct if_nameindex* pIfInfoCur = pIfInfo;
			while( pIfInfoCur->if_name )
			{
				interfaceNames.push_back(pIfInfoCur->if_name);
				++pIfInfoCur;
			}
			if_freenameindex(pIfInfo);
		}

		std::vector<std::string>::iterator iter = interfaceNames.begin();

		while( iter != interfaceNames.end() )
		{
			uint tip = 0;
			char * currName = (char *) iter->c_str();

			if( this->getInterfaceAddress(currName, addr) == 0 )
			{
				uint htip = ntohl(tip);

				if( ( htip >> netmaskshift ) == ( netmaskmatch >> netmaskshift ) )
				{
					//DEBUG_MSG("Endpoint::bind(): found a match\n");
					strncpy(name, currName, IFNAMSIZ);
					break;
				}
			}

			++iter;
		}

		if( name[0] == 0 )
		{
			uint8 * qik = (uint8*) &addr;
			ERROR_MSG("Endpoint::findIndicatedInterface: "
				"No interface matching netmask spec '%s' found "
				"(evals to %d.%d.%d.%d/%d)\n", spec, qik[0], qik[1], qik[2], qik[3],
				netmaskbits);

			return -2; // parsing ok, just didn't match
		}
	}

	return 0;
}

void WatcherPacketHandler::sendReply()
{
	g_watcher_module_send_params.chunk = packet_.uchar_data();
	g_watcher_module_send_params.length = packet_.get_written_bytes();
	VERBOSE_MSG("WatcherPacketHandler::sendReply() CALLED");
	my_send(g_watcher_module_send_params);
}

GECO_SINGLETON_DEFI(watcher_connection_acceptor_t);

/**
 *  This method is called by Mercury when there is data to read on the
 *  watcher socket. It calls the receiveRequest method to actually
 *  process the request.
 */
static void on_msg_arrived(int fd, short int revents, int* settled_events, void* usrdata)
{
	watcher_connection_acceptor_t* me = (watcher_connection_acceptor_t*) usrdata;
	if( fd == me->geco_tcp_sock() )
	{

	}
}

void watcher_connection_acceptor_t::attachTo(selector* reactor)
{
	cbunion_t cbunion_;
	cbunion_.user_cb_fun = &on_msg_arrived;
	reactor->set_expected_event_on_fd(udpSocket_, EVENTCB_TYPE_USER,
		POLLIN | POLLPRI, cbunion_, this);
	reactor->set_expected_event_on_fd(tcpSocket_, EVENTCB_TYPE_USER,
		POLLIN | POLLPRI, cbunion_, this);
	reactor_ = reactor;
}

bool watcher_connection_acceptor_t::init(const char * listeningInterface, ushort listeningPort,
	int af)
{
	if( isInitialised_ )
	{
		WARNING_MSG("WatcherNub::init: Already initialised.\n");
		return true;
	}

	INFO_MSG("WatcherNub::init: "
		"listeningInterface = '%s', listeningPort = %hd\n",
		listeningInterface ? listeningInterface : "", listeningPort);

	// open the socket
	udpSocket_.socket(af, SOCK_DGRAM);
	if( !udpSocket_.good() )
	{
		ERROR_MSG("WatcherNub::init: socket() failed\n");
		return false;
	}

	// ask endpoint to parse the interface specification into a name
	if( ( listeningInterface == NULL || listeningInterface[0] == 0 ) )
	{
		DEBUG_MSG("WatcherNub::init: ip null or empty. use any\n");
	}

	sockaddrunion su =
	{ 0 };
	bool useany = false;
	if( !str2saddr(&su, listeningInterface, listeningPort, af == AF_INET) )
	{
		ERROR_MSG("WatcherNub::init: str2saddr() failed use any\n");
		useany = true;
	}

	char ifname[IFNAMSIZ];
	if( udpSocket_.findIndicatedInterface(listeningInterface, af, ifname) >= 0 )
	{
		INFO_MSG("WatcherNub::init: creating on interface '%s' (= %s)\n", listeningInterface,
			ifname);
		if( udpSocket_.getInterfaceAddress(ifname, su) < 0 )
		{
			WARNING_MSG("WatcherNub::init: getInterfaceAddress(%s) failed"
				"so using any addr\n", ifname);
			useany = true;
		}
	} else
	{
		WARNING_MSG("WatcherNub::init: findIndicatedInterface failed use any addr\n");
		useany = true;
	}

	if( useany )
	{
		su =
		{ 0 };
		su.sa.sa_family = af;
		af == AF_INET ? su.sin.sin_port = htons(listeningPort) : su.sin6.sin6_port = htons(listeningPort);
	}

	bool socketsBound = false;
	const int MAX_RETRIES = 5;
	int count = 0;

	while( !socketsBound && count < MAX_RETRIES )
	{
		socketsBound = this->bindSockets(su);
		++count;
	}

	if( !socketsBound )
	{
		ERROR_MSG("WatcherNub::init: Failed to bind sockets to port\n");
		return false;
	}

	if( tcpSocket_.listen() != 0 )
	{
		ERROR_MSG("WatcherNub::init: listen failed\n");
		return false;
	}

	isInitialised_ = true;
	return isInitialised_;
}

int watcher_connection_acceptor_t::registerWatcher(int id, const char *abrv, const char *longName,
	const char * listeningInterface, uint16 listeningPort, int af)
{
	if( !this->isInitialised_ && !this->init(listeningInterface, listeningPort, af) )
	{
		ERROR_MSG("WatcherNub::registerWatcher: init failed.\n");
		return -1;
	}

	// make sure we're not already registered...
	if( registered_ ) this->deregisterWatcher();

	// set up a few things
	id_ = id;
	strncpy(abrv_, abrv, sizeof(abrv_));
	abrv_[sizeof(abrv_) - 1] = 0;
	strncpy(name_, longName, sizeof(name_));
	name_[sizeof(name_) - 1] = 0;
	registered_ = true;

	this->notifyMachineGuard();
	return 0;
}

int watcher_connection_acceptor_t::deregisterWatcher()
{
	if( !registered_ )
	{
		return 0;
	}
	registered_ = false;
	this->notifyMachineGuard();
	return 0;
}

void watcher_connection_acceptor_t::notifyMachineGuard()
{
	sockaddrunion bindsu;
	this->udpSocket_.getlocaladdress(bindsu);

}
bool watcher_connection_acceptor_t::bindSockets(sockaddrunion& su)
{
	if( udpSocket_.good() )
	{
		udpSocket_.close();
		udpSocket_.detach();
	}

	if( tcpSocket_.good() )
	{
		tcpSocket_.close();
		tcpSocket_.detach();
	}

	char buf[MAX_IPADDR_STR_LEN];
	ushort port;
	udpSocket_.socket(su.sa.sa_family, SOCK_DGRAM);

	if( udpSocket_.bind(su) < 0 )
	{
		saddr2str(&su, buf, MAX_IPADDR_STR_LEN, &port);
		WARNING_MSG("WatcherNub::init: udpSocket_.bind() to %s:%d failed\n", buf, port);
		udpSocket_.close();
		return false;
	}

	tcpSocket_.socket(su.sa.sa_family, SOCK_STREAM);

	if( tcpSocket_.bind(su) < 0 )
	{
		saddr2str(&su, buf, MAX_IPADDR_STR_LEN, &port);
		WARNING_MSG("WatcherNub::init: tcpSocket_.bind() to %s:%d failed\n", buf, port);
		udpSocket_.close();
		return false;
	}
	udpSocket_.getlocaladdress(su);
	return true;
}

void WatcherPacketHandler::run()
{
	isExecuting_ = true;
	this->doNotDelete(true);
	for( auto& ptr : pathRequestList_ )
	{
		isSet_ ? ptr->set_watcher_value() : ptr->get_watcher_value();
	}
	this->doNotDelete(false);
}

void WatcherPacketHandler::checkSatisfied()
{
	// cleanup any completed requests
	if( outgoingRequests_ == answeredRequests_ )
	{
		if( isExecuting_ )
		{
			// we are done... send packet out
			this->sendReply();
			isExecuting_ = false;
		}

		if( canDelete_ ) delete this;
	}
}

void WatcherPacketHandler::doNotDelete(bool shouldNotDelete)
{
	canDelete_ = !shouldNotDelete;
	if( canDelete_ )
	{
		this->checkSatisfied();
	}
}

watcher_value_query_t* WatcherPacketHandler::newRequest(std::string & path)
{
	if( isExecuting_ )
	{
		ERROR_MSG("WatcherPacketHandler::newRequest: Attempt to create a new "
			"path request after packet handler told disallowed new requests.\n");
		return NULL;
	}

	watcher_value_query_t* pathRequest = new watcher_value_query_t(path.c_str(),
		std::bind(&WatcherPacketHandler::notifyComplete, this, std::placeholders::_1, std::placeholders::_2));
	if( pathRequest != NULL )
	{
		pathRequest->setParent(this);
		pathRequestList_.push_back(pathRequest);
		return pathRequest;
	}
	return NULL;
}

void WatcherPacketHandler::notifyComplete(watcher_value_query_t& pathRequest, int32 count)
{
	if( !reachedPacketLimit_ )
	{
		// Check we haven't reached the packet size limit (UDP 64K)
		if( ( packet_.get_written_bytes() + pathRequest.get_data_size() ) > maxPacketSize_ )
		{
			ERROR_MSG("WatcherPacketHandler::notifyComplete: Can't add reply "
				"from WatcherPathRequest( '%s' ) due to packet size \n"
				"limit.\n", pathRequest.get_request_path().c_str());
			reachedPacketLimit_ = true;
		} else
		{
			packet_.Write(pathRequest.get_result_stream());
			// Now go back to the reserved reply count location.
			*(ushort*) &packet_.uchar_data()[1] += count;
		}
	}
	answeredRequests_++;
	this->checkSatisfied();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
// 　　　　　　　　　　　　Section: watcher_value_query_t impls
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
void watcher_value_query_t::get_watcher_value()
{
	int ret;
	if( is_dir_watcher_ ) ret = geco_watcher_base_t::get_root_watcher().visit_children(0,
		request_path_.c_str(), *this);
	else ret = geco_watcher_base_t::get_root_watcher().get_as_stream(NULL, request_path_.c_str(),
		*this);

	if( !ret )
	{
		result_.WriteMini(ret);
		// And notify the parent of our failure.
		this->notify(-1);
	}
}
void watcher_value_query_t::set_watcher_value()
{
	if( is_dir_watcher_ ) return;
	if( !geco_watcher_base_t::get_root_watcher().set_from_stream(NULL, request_path_.c_str(), *this) )
	{
		//result_.WriteMini((uchar)WVT_UNKNOWN);
		//result_.WriteMini((uchar)WT_READ_ONLY);
		result_.WriteMini(false);
		// We're pretty much done setting watcher value (failed)
		// Ready to fill in the packet now 0 means operation failed
		this->notify(-1);
	}
}
bool watcher_value_query_t::add_watcher_path(const void *base, const char *path,
	std::string & label, geco_watcher_base_t &watcher)
{
	std::string desc;
	origin_request_path_.size() > 0 ?
		request_path_ = origin_request_path_ + "/" + label : request_path_ = label;

	// Push the directory entry onto the result stream
	// We are using a reference to the correct watcher now, so no need
	// to pass in the path to search for.
	bool status = watcher.get_as_stream(base, NULL, *this);

	if( !status )
	{
		ERROR_MSG("watcher_path_request_v2::add_watcher_path::!status error\n");
		// If the get operation failed, we still need to notify the parent
		// so it can continue with its response.
		result_.WriteMini((uchar) WVT_UNKNOWN);
		result_.WriteMini((uchar) WT_READ_ONLY);
		result_.WriteMini(status);
		return false;
	}
	// Always need to return true from the asyn version 2 protocol
	// otherwise the stream won't be completed.
	return true;
}

bool geco_tcp_sock_t::recv_all(void * gramData, int gramSize)
{
	while( gramSize > 0 )
	{
		int len = this->recv(gramData, gramSize);

		if( len <= 0 )
		{
			if( len == 0 )
			{
				WARNING_MSG("Endpoint::recvAll: Connection lost\n");
			} else
			{
				WARNING_MSG("Endpoint::recvAll: Got error '%s'\n",
					strerror(errno));
			}

			return false;
		}
		gramSize -= len;
		gramData = ( (char *) gramData ) + len;
	}

	return true;
}

int geco_sock_base_t::getBufferSize(int optname) const
{
#ifdef __unix__
	assert(optname == SO_SNDBUF || optname == SO_RCVBUF);
	int recvbuf = -1;
	socklen_t rbargsize = sizeof(int);
	int rberr = getsockopt(socket_, SOL_SOCKET, optname, (char*) &recvbuf, &rbargsize);

	if( rberr == 0 && rbargsize == sizeof(int) )
	{
		return recvbuf;
	} else
	{
		ERROR_MSG("Endpoint::getBufferSize: "
			"Failed to read option %s: %s\n",
			optname == SO_SNDBUF ? "SO_SNDBUF" : "SO_RCVBUF",
			strerror(errno));
		return -1;
	}
#else
	return -1;
#endif
}

bool geco_sock_base_t::setBufferSize(int optname, int size)
{
#ifdef __unix__
	setsockopt(socket_, SOL_SOCKET, optname, (const char*) &size, sizeof(size));
#endif
	return this->getBufferSize(optname) >= size;
}
