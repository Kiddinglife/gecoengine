
// must include <winsock2.h> before <windows.h>
#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#endif

#include "end-point.h"

GecoNetEndpoint * GecoNetEndpoint::ms_pkHijackEndpointSync = NULL;
GecoNetEndpoint * GecoNetEndpoint::ms_pkHijackEndpointAsync = NULL;
geco_bit_stream_t * GecoNetEndpoint::ms_pkHijackStream = NULL;
GecoNetEndpoint::FrontEndInterfaces GecoNetEndpoint::ms_kFrontEndInterfaces;

#ifdef ____linux____
extern "C" {
	/// 32 bit unsigned integer.
#define __u32 uint
/// 8 bit unsigned integer.
#define __u8 uchar
#include <linux/errqueue.h>
}
#include <sys/uio.h>
#include <netinet/ip.h>
#elif defined(_WIN32)//! win32
static bool s_bGetAdaptersAddresses = false;
struct AdapterInfo
{
	eastl::string	kName;
	u_int32_t	uiAddr;
	AdapterInfo(eastl::string& kName_, u_int32_t uiAddr_) :kName(kName_), uiAddr(uiAddr_) {}
};
static eastl::vector<AdapterInfo> s_kAdapterInfoList;
void GetAdaptersAddresses();
#else
struct if_nameindex
{

	unsigned int if_index;

	char *if_name;

};

struct if_nameindex *if_nameindex(void)
{
	static struct if_nameindex staticIfList[3] =
	{ { 1, "eth0" }, { 2, GECO_NET_LOCALHOSTNAME }, { 0, 0 } };

	return staticIfList;
}

INLINE void if_freenameindex(struct if_nameindex *)
{}
#endif	// !__linux__

GecoNetEndpoint::GecoNetEndpoint(bool useSyncHijack) :
	m_kSocket(NO_SOCKET),
	m_bUseSyncHijack(useSyncHijack),
	m_bShouldSendClose(false),
	m_uiHijackPort(0),
	m_uiHijackAddr(0),
	m_pkHijackEndpoint(NULL)
{
#ifdef _WIN32
	GetAdaptersAddresses();
#endif
}

bool GecoNetEndpoint::GetClosedPort(GecoNetAddress & closedPort)
{
	bool isResultSet = false;

#ifdef __linux__
	//	GECO_ASSERT( errno == ECONNREFUSED );

	struct sockaddr_in	offender;
	offender.sin_family = 0;
	offender.sin_port = 0;
	offender.sin_addr.s_addr = 0;

	struct msghdr	errHeader;
	struct iovec	errPacket;

	char data[256];
	char control[256];

	errHeader.msg_name = &offender;
	errHeader.msg_namelen = sizeof(offender);
	errHeader.msg_iov = &errPacket;
	errHeader.msg_iovlen = 1;
	errHeader.msg_control = control;
	errHeader.msg_controllen = sizeof(control);
	errHeader.msg_flags = 0;	// result only

	errPacket.iov_base = data;
	errPacket.iov_len = sizeof(data);

	int errMsgErr = recvmsg(*this, &errHeader, MSG_ERRQUEUE);
	if (errMsgErr < 0)
	{
		return false;
	}

	struct cmsghdr * ctlHeader;

	for (ctlHeader = CMSG_FIRSTHDR(&errHeader);
		ctlHeader != NULL;
		ctlHeader = CMSG_NXTHDR(&errHeader, ctlHeader))
	{
		if (ctlHeader->cmsg_level == SOL_IP &&
			ctlHeader->cmsg_type == IP_RECVERR) break;
	}


	if (ctlHeader != NULL)
	{
		struct sock_extended_err * extError =
			(struct sock_extended_err*)CMSG_DATA(ctlHeader);


		if (errHeader.msg_namelen == 0)
		{
			offender = *(sockaddr_in*)SO_EE_OFFENDER(extError);
			offender.sin_port = 0;

			printf("GecoNetNub::ProcessPendingEvents: "
				"Kernel has a bug: recv_msg did not set msg_name.\n");
		}

		closedPort.ip = offender.sin_addr.s_addr;
		closedPort.port = offender.sin_port;

		isResultSet = true;
	}
#endif // __linux__

	return isResultSet;
}

bool GecoNetEndpoint::GetInterfaces(eastl::map< u_int32_t, eastl::string > &interfaces)
{
#ifdef _WIN32

	if (!s_kAdapterInfoList.empty())
	{
		for (int i = 0; i < (int)s_kAdapterInfoList.size(); ++i)
		{
			interfaces[s_kAdapterInfoList[i].uiAddr] = s_kAdapterInfoList[i].kName;
		}
	}

#else

	struct if_nameindex* pIfInfo = if_nameindex();
	if (!pIfInfo)
	{
		printf("Unable to discover network interfaces.\n");
		return false;
	}

	int		flags = 0;
	struct if_nameindex* pIfInfoCur = pIfInfo;
	while (pIfInfoCur->if_name)
	{
		flags = 0;
		this->GetInterfaceFlags(pIfInfoCur->if_name, flags);

		if ((flags & IFF_UP) && (flags & IFF_RUNNING))
		{
			u_int32_t	addr;
			if (this->GetInterfaceAddress(pIfInfoCur->if_name, addr) == 0)
			{
				interfaces[addr] = pIfInfoCur->if_name;
			}
		}
		++pIfInfoCur;
	}
	if_freenameindex(pIfInfo);

#endif

	return true;
}


int GecoNetEndpoint::FindDefaultInterface(char * name)
{
	if (GecoNetEndpoint::IsHijacked())
	{
		FrontEndInterfaces::iterator iter = ms_kFrontEndInterfaces.begin();

		while (iter != ms_kFrontEndInterfaces.end())
		{
			if (iter->first != GECO_NET_LOCALHOSTNAME)
			{
				strcpy(name, iter->first.c_str());
				return 0;
			}

			++iter;
		}

		return -1;
	}

#ifndef __linux__

#ifdef _WIN32

	if (!s_kAdapterInfoList.empty())
	{
		strcpy(name, s_kAdapterInfoList[0].kName.c_str());
		return 0;
	}
	else
	{
		return -1;
	}

#else

	strcpy(name, "eth0");
	return 0;

#endif

#else // __linux__
	int		ret = -1;

	struct if_nameindex* pIfInfo = if_nameindex();
	if (pIfInfo)
	{
		int		flags = 0;
		struct if_nameindex* pIfInfoCur = pIfInfo;
		while (pIfInfoCur->if_name)
		{
			flags = 0;
			this->GetInterfaceFlags(pIfInfoCur->if_name, flags);

			if ((flags & IFF_UP) && (flags & IFF_RUNNING))
			{
				u_int32_t	addr;
				if (this->GetInterfaceAddress(pIfInfoCur->if_name, addr) == 0)
				{
					strcpy(name, pIfInfoCur->if_name);
					ret = 0;

					if (!(flags & IFF_LOOPBACK)) break;
				}
			}
			++pIfInfoCur;
		}
		if_freenameindex(pIfInfo);
	}
	else
	{
		printf("GecoNetEndpoint::FindDefaultInterface: "
			"if_nameindex returned NULL (%s)\n",
			strerror(errno));
	}

	return ret;
#endif // !__linux__
}

int GecoNetEndpoint::FindIndicatedInterface(const char * spec, char * name)
{
	name[0] = 0;

	if (spec == NULL || spec[0] == 0)
		return -1;

	if (strlen(spec) > IFNAMSIZ)
	{
		g_network_logger->error("{}, InterfaceName:{} is Too long\n", __FUNCTION__, spec);
		return -1;
	}

	char* slash;
	int netmaskbits = 32;
	u_int32_t addr = 0;

	if (slash = const_cast<char *>(strchr(spec, '/')))
	{
		char iftemp[IFNAMSIZ] = { 0 };
		strncpy(iftemp, spec, slash - spec);
		iftemp[slash - spec] = 0;
		bool ok = GecoNetEndpoint::ConvertAddress(iftemp, addr) == 0;

		netmaskbits = atoi(slash + 1);
		ok &= netmaskbits > 0 && netmaskbits <= 32;

		if (!ok)
		{
			g_network_logger->error("GecoNetEndpoint::FindIndicatedInterface: "
				"netmask match {} length {} is not valid.\n", iftemp, slash + 1);
			return -1;
		}
	}
	else if (this->GetInterfaceAddress(spec, addr) == 0)
	{
		strcpy(name, spec);
	}
	else if (GecoNetEndpoint::ConvertAddress(spec, addr) == 0)
	{
		netmaskbits = 32;
	}
	else
	{
		g_network_logger->error("GecoNetEndpoint::FindIndicatedInterface: "
			"No interface matching interface spec '{}' found\n", spec);
		return -1;
	}

	if (name[0] == 0)
	{
		int netmaskshift = 32 - netmaskbits;
		u_int32_t netmaskmatch = ntohl(addr);

		eastl::vector< eastl::string > interfaceNames;

		if (GecoNetEndpoint::IsHijacked())
		{
			FrontEndInterfaces::iterator iter = ms_kFrontEndInterfaces.begin();
			while (iter != ms_kFrontEndInterfaces.end())
			{
				interfaceNames.push_back(iter->first);
				++iter;
			}
		}
		else
		{
			if (!s_kAdapterInfoList.empty())
			{
				for (int i = 0; i < (int)s_kAdapterInfoList.size(); ++i)
				{
					interfaceNames.push_back(s_kAdapterInfoList[i].kName);
				}
			}
		}

		eastl::vector< eastl::string >::iterator iter = interfaceNames.begin();

		while (iter != interfaceNames.end())
		{
			u_int32_t tip = 0;
			char * currName = (char *)iter->c_str();

			if (this->GetInterfaceAddress(currName, tip) == 0)
			{
				u_int32_t htip = ntohl(tip);

				if ((htip >> netmaskshift) == (netmaskmatch >> netmaskshift))
				{
					strncpy(name, currName, IFNAMSIZ);
					g_network_logger->debug("Endpoint::bind(): found a match address {}\n", name);
					break;
				}
			}

			++iter;
		}

		if (name[0] == 0)
		{
			uchar * qik = (uchar*)&addr;
			g_network_logger->error("GecoNetEndpoint::FindIndicatedInterface: "
				"No interface matching netmask spec '%s' found "
				"(evals to {}.{}.{}.{}:{})\n", spec,
				qik[0], qik[1], qik[2], qik[3], netmaskbits);

			return -2;
		}
	}

	return 0;
}

#ifdef _WIN32

int GecoNetEndpoint::GetInterfaceFlags(char * name, int & flags)
{
	if (_stricmp(name, GECO_NET_LOCALHOSTNAME))
	{
		flags = IFF_UP | IFF_LOOPBACK | IFF_RUNNING;
	}
	else
	{
		flags = IFF_UP | IFF_BROADCAST | IFF_NOTRAILERS |
			IFF_RUNNING | IFF_MULTICAST;
	}

	return 0;
}

int GecoNetEndpoint::GetInterfaceAddress(const char * name, u_int32_t & address)
{
	if (!s_kAdapterInfoList.empty())
	{
		for (int i = 0; i < (int)s_kAdapterInfoList.size(); ++i)
		{
			if (!_stricmp(s_kAdapterInfoList[i].kName.c_str(), name))
			{
				address = s_kAdapterInfoList[i].uiAddr;
				return 0;
			}
		}
	}

	return -1;
}

#endif

int GecoNetEndpoint::ConvertAddress(const char * string, u_int32_t & address)
{
	u_int32_t	trial;

#ifdef __linux__
	if (inet_aton(string, (struct in_addr*)&trial) != 0)
#else // __linux__
	if ((trial = inet_addr(string)) != INADDR_NONE)
#endif // !__linux__
	{
		address = trial;
		return 0;
	}

#ifdef _WIN32
	if (!s_kAdapterInfoList.empty())
	{
		for (int i = 0; i < (int)s_kAdapterInfoList.size(); ++i)
		{
			if (!_stricmp(s_kAdapterInfoList[i].kName.c_str(), string))
			{
				address = s_kAdapterInfoList[i].uiAddr;
				return 0;
			}
		}
	}
#else
	struct hostent * hosts = gethostbyname(string);
	if (hosts != NULL)
	{
		address = *(u_int32_t*)(hosts->h_addr_list[0]);
		return 0;
	}
#endif

	return -1;
}


#ifdef __linux__
int GecoNetEndpoint::GetQueueSizes(int & tx, int & rx) const
{
	int	ret = -1;

	u_int16_t	nport = 0;
	this->GetLocalAddress(&nport, NULL);

	char		match[16];
	GecoSNPrintf(match, sizeof(match), "%04X", (int)ntohs(nport));

	FILE * f = fopen("/proc/net/udp", "r");

	if (!f)
	{
		g_network_logger->error("GecoNetEndpoint::GetQueueSizes: "
			"could not open /proc/net/udp: {}\n",
			strerror(errno));
		return -1;
	}

	char	aline[256];
	fgets(aline, 256, f);

	while (fgets(aline, 256, f) != NULL)
	{
		if (!strncmp(aline + 4 + 1 + 1 + 8 + 1, match, 4))
		{
			char * start = aline + 4 + 1 + 1 + 8 + 1 + 4 + 1 + 8 + 1 + 4 + 1 + 2 + 1;
			start[8] = 0;
			tx = strtol(start, NULL, 16);

			start += 8 + 1;
			start[8] = 0;
			rx = strtol(start, NULL, 16);

			ret = 0;

			break;
		}
	}

	fclose(f);

	return ret;
}
#else // __linux__
int GecoNetEndpoint::GetQueueSizes(int &, int &) const
{
	return -1;
}
#endif // !__linux__


int GecoNetEndpoint::GetBufferSize(int optname) const
{
#ifdef __linux__
	GECO_ASSERT(optname == SO_SNDBUF || optname == SO_RCVBUF);

	int recvbuf = -1;
	socklen_t rbargsize = sizeof(int);
	int rberr = getsockopt(m_kSocket, SOL_SOCKET, optname,
		(char*)&recvbuf, &rbargsize);

	if (rberr == 0 && rbargsize == sizeof(int))
	{
		return recvbuf;
	}
	else
	{
		g_network_logger->error("GecoNetEndpoint::GetBufferSize: "
			"Failed to read option {}: {}\n",
			optname == SO_SNDBUF ? "SO_SNDBUF" : "SO_RCVBUF",
			strerror(errno));

		return -1;
	}

#else // __linux__
	return -1;
#endif // !__linux__
}


bool GecoNetEndpoint::SetBufferSize(int optname, int size)
{
#ifdef __linux__
	setsockopt(m_kSocket, SOL_SOCKET, optname, (const char*)&size,
		sizeof(size));
#else
	//! add by Uman,2009/10/22
	return setsockopt(m_kSocket, SOL_SOCKET, optname, (const char*)&size,
		sizeof(size)) == 0;
#endif // __linux__

	return this->GetBufferSize(optname) >= size;
}


bool GecoNetEndpoint::RecvAll(void * gramData, int gramSize)
{
	while (gramSize > 0)
	{
		int len = this->Recv(gramData, gramSize);

		if (len <= 0)
		{
			if (len == 0)
			{
				g_network_logger->warn("GecoNetEndpoint::RecvAll: Connection lost\n");
			}
			else
			{
				g_network_logger->error("GecoNetEndpoint::RecvAll: Got error '{}'\n", strerror(errno));
			}

			return false;
		}
		gramSize -= len;
		gramData = ((char *)gramData) + len;
	}

	return true;
}


void GecoNetEndpoint::SetHijackEndpoints(GecoNetEndpoint * pSync, GecoNetEndpoint * pAsync)
{
	ms_pkHijackEndpointSync = pSync;
	ms_pkHijackEndpointAsync = pAsync;
}


bool GecoNetEndpoint::HijackRecvAllFrom(void * gramData, int gramSize,
	u_int16_t * pPort, u_int32_t * pAddr)
{
	if (m_pkHijackEndpoint == NULL)
	{
		return
			this->RecvFrom(gramData, gramSize, pPort, pAddr) == gramSize;
	}
	else
	{
		short msgType;
		int dstFD;
		int dataLen;
		u_int32_t addr;
		u_int16_t port;

		if (!m_pkHijackEndpoint->RecvAll(&msgType, sizeof(msgType)) ||
			(msgType != GecoNetEndpoint::HIJACK_MSG_UDP) ||
			!m_pkHijackEndpoint->RecvAll(&dstFD, sizeof(dstFD)) ||
			(dstFD != m_kSocket) ||
			!m_pkHijackEndpoint->RecvAll(&dataLen, sizeof(dataLen)) ||
			(dataLen != gramSize + 6) ||
			!m_pkHijackEndpoint->RecvAll(&addr, sizeof(addr)) ||
			!m_pkHijackEndpoint->RecvAll(&port, sizeof(port)))
		{
			return false;
		}

		if (pAddr)
			*pAddr = addr;
		if (pPort)
			*pPort = port;

		return m_pkHijackEndpoint->RecvAll(gramData, gramSize);
	}
}


int GecoNetEndpoint::HijackFD() const
{
	return m_pkHijackEndpoint ? m_pkHijackEndpoint->m_kSocket : m_kSocket;
}


void GecoNetEndpoint::AddFrontEndInterface(const eastl::string & name, u_int32_t addr)
{
	ms_kFrontEndInterfaces[name] = addr;
}


static bool s_networkInitted = false;
void InitNetwork()
{
	if (s_networkInitted) return;
	s_networkInitted = true;
#if !defined( PLAYSTATION3 )
#ifndef __linux__
	WSAData wsdata;
	WSAStartup(0x202, &wsdata);
#endif // !__linux__
#endif // __linux__

	// This other example use a single logger with multiple sinks.
	// This means that the same log_msg is forwarded to multiple sinks;
	// Each sink can have it's own log level and a message will be logged.
	std::vector<spdlog::sink_ptr> sinks;
#ifdef _WIN32
	sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_st>());
#else
	sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_sink>(spdlog::sinks::stdout_sink_st::instance()));
#endif
	// Create a file rotating logger with 5mb size max and 3 rotated files
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_st>("logs/network", SPDLOG_FILENAME_T("log"), 1048576 * 5, 3));
	sinks[0]->set_level(spdlog::level::trace);  // console. Allow everything.  Default value
	sinks[1]->set_level(spdlog::level::info);  //  regular file. Allow everything.  Default value
	g_network_logger = std::make_shared<spdlog::logger>("g_network_logger", sinks.begin(), sinks.end());
	g_network_logger->set_level(spdlog::level::info);
	spdlog::register_logger(g_network_logger);
}

#ifdef _WIN32

void WCharToSTLStr(WCHAR* pSrc, eastl::string& des)
{
	if (!pSrc)
		return;

	int wLen = (int)wcslen(pSrc);
	int desLen(0);
	desLen = WideCharToMultiByte(CP_ACP, 0, pSrc, wLen, NULL, 0, NULL, NULL);
	des.assign(desLen, 0);
	WideCharToMultiByte(CP_ACP, 0, pSrc, wLen, const_cast<char*>(des.data()), desLen, NULL, NULL);
}

void GetAdaptersAddresses()
{
	if (s_bGetAdaptersAddresses)
		return;
	s_bGetAdaptersAddresses = true;
	s_kAdapterInfoList.clear();


	DWORD dwRetVal = 0;
	PIP_ADAPTER_ADDRESSES pkAddresses = NULL;
	ULONG uiOutBufLen = 15000;
	ULONG uiIterations = 0;


	do
	{
		pkAddresses = (IP_ADAPTER_ADDRESSES *)new char[uiOutBufLen];
		if (!pkAddresses)
			return;

		dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pkAddresses, &uiOutBufLen);

		if (dwRetVal == ERROR_BUFFER_OVERFLOW)
		{
			delete[] pkAddresses;
			pkAddresses = NULL;
		}
		else
		{
			break;
		}

		uiIterations++;

	} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (uiIterations < 3));


	if (dwRetVal != NO_ERROR)
	{
		printf("Call to GetAdaptersAddresses failed with error: %d\n", dwRetVal);

		if (dwRetVal == ERROR_NO_DATA)
		{
			g_network_logger->error("No addresses were found in GetAdaptersAddresses\n");
		}
		else
		{
			LPVOID lpMsgBuf = NULL;
			if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)& lpMsgBuf, 0, NULL))
			{
				printf("GetAdaptersAddresses Error: %s\n", lpMsgBuf);
				LocalFree(lpMsgBuf);
			}
		}

		if (pkAddresses)
			delete[](char*)pkAddresses;

		return;
	}


	PIP_ADAPTER_ADDRESSES pkCurrAddresses = pkAddresses;
	while (pkCurrAddresses)
	{
		if (pkCurrAddresses->FirstUnicastAddress)
		{
			eastl::string kFriendlyName;
			WCharToSTLStr(pkCurrAddresses->FriendlyName, kFriendlyName);
			sockaddr_in* pAddr = (sockaddr_in*)pkCurrAddresses->FirstUnicastAddress->Address.lpSockaddr;

			//! 判断是否是127.0.0.1
			if (pAddr->sin_addr.s_addr == GECO_NET_LOCALHOST)
				kFriendlyName = GECO_NET_LOCALHOSTNAME;

			s_kAdapterInfoList.push_back(AdapterInfo(kFriendlyName, pAddr->sin_addr.s_addr));
		}

		pkCurrAddresses = pkCurrAddresses->Next;
	}

	//! 交换位置,第一个放非localhost地址
	if (s_kAdapterInfoList.size() > 1)
	{
		if (s_kAdapterInfoList[0].kName == GECO_NET_LOCALHOSTNAME)
		{
			AdapterInfo kTmp = s_kAdapterInfoList.back();
			s_kAdapterInfoList.back() = s_kAdapterInfoList.front();
			s_kAdapterInfoList.front() = kTmp;
		}
	}

	if (pkAddresses)
		delete[](char*)pkAddresses;
}

#endif