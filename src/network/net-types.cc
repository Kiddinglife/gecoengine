#include "net-types.h"
//#include "GecoWatcher.h"

#ifdef _WIN32
#elif defined( PLAYSTATION3 )
#include <netinet/in.h>
#else
#include <arpa/inet.h>
#endif

bool g_enable_stats = true;

bool saddr_equals(const sockaddrunion *a, const sockaddrunion *b, bool ignore_port)
{
	if (saddr_family(a) == AF_INET)
	{
		if (saddr_family(b) == AF_INET)
		{
			if (a->sin.sin_addr.s_addr == b->sin.sin_addr.s_addr)
			{
				if (ignore_port)
					return true;
				else if (a->sin.sin_port == b->sin.sin_port)
					return true;
				else
					return false;
			}
			return false;
		}
		return false;
	}
	else if (saddr_family(a) == AF_INET6)
	{
		if (saddr_family(b) == AF_INET6)
		{
			if (IN6_ADDR_EQUAL(&a->sin6.sin6_addr, &b->sin6.sin6_addr))
			{
				if (ignore_port)
					return true;
				else if (a->sin6.sin6_port == b->sin6.sin6_port)
					return true;
				else
					return false;
			}
			return false;
		}
		return false;
	}
	else
	{
		network_logger()->critical("saddr_equals()::no such af!!");
	}
}
int str2saddr(sockaddrunion *su, const char * str, ushort hs_port)
{
	int ret;
	memset((void*)su, 0, sizeof(union sockaddrunion));

	if (hs_port < 0)
	{
		network_logger()->error("Invalid port \n");
		return -1;
	}

	if (str != NULL && strlen(str) > 0)
	{
#ifndef WIN32
		ret = inet_aton(str, &su->sin.sin_addr);
#else
		(su->sin.sin_addr.s_addr = inet_addr(str)) == INADDR_NONE ? ret = 0 : ret = 1;
#endif
	}
	else
	{
		network_logger()->warn("no s_addr specified, set to all zeros\n");
		ret = 1;
	}

	if (ret > 0) /* Valid IPv4 address format. */
	{
		su->sin.sin_family = AF_INET;
		su->sin.sin_port = htons(hs_port);
		return 0;
	}

	if (str != NULL && strlen(str) > 0)
	{
		ret = inet_pton(AF_INET6, (const char *)str, &su->sin6.sin6_addr);
	}
	else
	{
		network_logger()->warn("no s_addr specified, set to all zeros\n");
		ret = 1;
	}

	if (ret > 0) /* Valid IPv6 address format. */
	{
		su->sin6.sin6_family = AF_INET6;
		su->sin6.sin6_port = htons(hs_port);
		su->sin6.sin6_scope_id = 0;
		su->sin6.sin6_flowinfo = 0;
		return 0;
	}
	return -1;
}
int saddr2str(sockaddrunion *su, char * buf, size_t len, ushort* portnum)
{

	if (su->sa.sa_family == AF_INET)
	{
		if (buf != NULL)
			strncpy(buf, inet_ntoa(su->sin.sin_addr), 16);
		if (portnum != NULL)
			*portnum = ntohs(su->sin.sin_port);
		return (1);
	}
	else if (su->sa.sa_family == AF_INET6)
	{
		if (buf != NULL)
		{
			char ifnamebuffer[IFNAMSIZ];
			const char* ifname = 0;

			if (inet_ntop(AF_INET6, &su->sin6.sin6_addr, buf, len) == NULL)
				return 0;
			if (IN6_IS_ADDR_LINKLOCAL(&su->sin6.sin6_addr))
			{
#ifdef _WIN32
				NET_LUID luid;
				ConvertInterfaceIndexToLuid(su->sin6.sin6_scope_id, &luid);
				ifname = (char*)ConvertInterfaceLuidToNameA(&luid, (char*)&ifnamebuffer, IFNAMSIZ);
#else
				ifname = if_indextoname(su->sin6.sin6_scope_id, (char*)&ifnamebuffer);
#endif
				if (ifname == NULL)
				{
					return (0); /* Bad scope ID! */
				}
				if (strlen(buf) + strlen(ifname) + 2 >= len)
				{
					return (0); /* Not enough space! */
				}
				strcat(buf, "%");
				strcat(buf, ifname);
			}

			if (portnum != NULL)
				*portnum = ntohs(su->sin6.sin6_port);
		}
		return (1);
	}
	return 0;
}
unsigned int sockaddr2hashcode(const sockaddrunion* sa)
{
	ushort local_saaf = saddr_family(sa);
	unsigned int lastHash = SuperFastHashIncremental((const char*)&sa->sin.sin_port,
		sizeof(sa->sin.sin_port), local_saaf);
	if (local_saaf == AF_INET)
	{
		lastHash = SuperFastHashIncremental((const char*)&sa->sin.sin_addr.s_addr, sizeof(in_addr),
			lastHash);
	}
	else if (local_saaf == AF_INET6)
	{
		lastHash = SuperFastHashIncremental((const char*)&sa->sin6.sin6_addr.s6_addr,
			sizeof(in6_addr), lastHash);
	}
	else
	{
		network_logger()->critical("sockaddr2hashcode()::no such af {}", local_saaf);
	}
	return lastHash;
}
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const unsigned short *) (d)))
#else
#define get16bits(d) ((((unsigned int)(((const unsigned char *)(d))[1])) << 8)\
	+(unsigned int)(((const unsigned char *)(d))[0]) )
#endif
static const int INCREMENTAL_READ_BLOCK = 65536;
unsigned long SuperFastHash(const char * data, int length)
{
	// All this is necessary or the hash does not match SuperFastHashIncremental
	int bytesRemaining = length;
	unsigned int lastHash = length;
	int offset = 0;
	while (bytesRemaining >= INCREMENTAL_READ_BLOCK)
	{
		lastHash = SuperFastHashIncremental(data + offset, INCREMENTAL_READ_BLOCK, lastHash);
		bytesRemaining -= INCREMENTAL_READ_BLOCK;
		offset += INCREMENTAL_READ_BLOCK;
	}
	if (bytesRemaining > 0)
	{
		lastHash = SuperFastHashIncremental(data + offset, bytesRemaining, lastHash);
	}
	return lastHash;

	//	return SuperFastHashIncremental(data,len,len);
}
unsigned long SuperFastHashIncremental(const char * data, int len, unsigned int lastHash)
{
	unsigned int hash = (unsigned int)lastHash;
	unsigned int tmp;
	int rem;

	if (len <= 0 || data == NULL) return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--)
	{
		hash += get16bits(data);
		tmp = (get16bits(data + 2) << 11) ^ hash;
		hash = (hash << 16) ^ tmp;
		data += 2 * sizeof(unsigned short);
		hash += hash >> 11;
	}

	/* Handle end cases */
	switch (rem)
	{
	case 3:
		hash += get16bits(data);
		hash ^= hash << 16;
		hash ^= data[sizeof(unsigned short)] << 18;
		hash += hash >> 11;
		break;
	case 2:
		hash += get16bits(data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1:
		hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return (unsigned int)hash;

}
unsigned long SuperFastHashFile(const char * filename)
{
	FILE *fp = fopen(filename, "rb");
	if (fp == 0) return 0;
	unsigned int hash = SuperFastHashFilePtr(fp);
	fclose(fp);
	return hash;
}
unsigned long SuperFastHashFilePtr(FILE *fp)
{
	fseek(fp, 0, SEEK_END);
	int length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int bytesRemaining = length;
	unsigned int lastHash = length;
	char readBlock[INCREMENTAL_READ_BLOCK];
	while (bytesRemaining >= (int) sizeof(readBlock))
	{
		fread(readBlock, sizeof(readBlock), 1, fp);
		lastHash = SuperFastHashIncremental(readBlock, (int) sizeof(readBlock), lastHash);
		bytesRemaining -= (int) sizeof(readBlock);
	}
	if (bytesRemaining > 0)
	{
		fread(readBlock, bytesRemaining, 1, fp);
		lastHash = SuperFastHashIncremental(readBlock, bytesRemaining, lastHash);
	}
	return lastHash;
}

/*GecoNetAddress*/
char GecoNetAddress::ms_pcStringBuf[2][GecoNetAddress::MAX_STRLEN];
int GecoNetAddress::ms_iCurrStringBuf = 0;
const GecoNetAddress GecoNetAddress::NONE;
bool WatcherStringToValue(const char * valueStr, GecoNetAddress & value)
{
	char ret[256];
	short port;
	if (sscanf(valueStr, "%s:%d",ret, &port) != 2)
	{
		network_logger()->error("WatcherStringToValue: Cannot convert '{}' to an Address", valueStr);
		return false;
	}
	value.su;
	str2saddr(&value.su, ret, port);
	return true;
}
int GecoNetAddress::WriteToString(char * str, int length)
{
	ushort port;
	saddr2str(&su, str, length, &port);
	return geco_snprintf(str, length - sizeof(sockaddr), ":%d", port);
}
char * GecoNetAddress::c_str()
{
	char * buf = GecoNetAddress::NextStringBuf();
	WriteToString(buf, MAX_STRLEN);
	return buf;
}
const char * GecoNetAddress::IPAsString()
{
	char * buf = GecoNetAddress::NextStringBuf();
	return buf;
}

// @TODO
GecoWatcher* GecoNetAddress::GetWatcher()
{
	//static GecoMemberWatcher<char *, GecoNetAddress>	* watchMe = NULL;
	//if (watchMe == NULL)
	//{
	//	watchMe = new GecoMemberWatcher<char *, GecoNetAddress>(
	//		*((GecoNetAddress*)NULL),
	//		&GecoNetAddress::operator char*,
	//		static_cast<void (GecoNetAddress::*)(char*)>(NULL)
	//		);
	//}
	//return *watchMe;
	return NULL;
}
char * GecoNetAddress::NextStringBuf()
{
	ms_iCurrStringBuf = (ms_iCurrStringBuf + 1) % 2;
	return ms_pcStringBuf[ms_iCurrStringBuf];
}



/*GecoNetInterfaceElement*/
class BundleDataPos
{
private:
	GecoNetPacket * pPacket_;
	uchar * pCurr_;
public:
	BundleDataPos(GecoNetPacket * pPacket, void * pCurr) :
		pPacket_(pPacket),
		pCurr_((uchar*)pCurr)
	{
		assert(pPacket->Body() <= pCurr && pCurr < pPacket->Back());
	}
	GecoNetPacket * pPacket() const { return pPacket_; }
	uchar * pData() const { return pCurr_; }
	bool Advance(unsigned int distance)
	{
		if ((pPacket_ == NULL) || (pCurr_ == NULL))
			return false;

		int remainingLength = distance;
		uchar * pEnd = (uchar*)pPacket_->Back();

		while (remainingLength >= (pEnd - pCurr_))
		{
			remainingLength -= (pEnd - pCurr_);
			pPacket_ = pPacket_->Next();
			if (pPacket_ == NULL)
			{
				network_logger()->error("BundleDataPos::AdvancedBy: Ran out of packets.\n");
				return false;
			}
			pCurr_ = (uchar*)pPacket_->Body();
			pEnd = (uchar*)pPacket_->Back();
		}
		pCurr_ += remainingLength;
		return true;
	}
};

const GecoNetInterfaceElement GecoNetInterfaceElement::REPLY("Reply", REPLY_MESSAGE_IDENTIFIER, VARIABLE_LENGTH_MESSAGE, 4);

int GecoNetInterfaceElement::HeaderSize() const
{
	int headerLen = sizeof(GecoNetMessageID);
	switch (m_uiLengthStyle)
	{
	case FIXED_LENGTH_MESSAGE:
		break;
	case VARIABLE_LENGTH_MESSAGE:
		headerLen += m_iLengthParam;
		break;
	default:
		abort();
		break;
	}
	return headerLen;
}
int GecoNetInterfaceElement::NominalBodySize() const
{
	return (m_uiLengthStyle == FIXED_LENGTH_MESSAGE) ? m_iLengthParam : 0;
}
GecoWatcher* GecoNetInterfaceElementWithStats::GetWatcher()
{
	//static FvDirectoryWatcherPtr spWatcher = NULL;
	//if (spWatcher == NULL)
	//{
	//	FvNetInterfaceElementWithStats * pNULL = NULL;
	//	spWatcher = new FvDirectoryWatcher();
	//	spWatcher->AddChild("name",
	//		MakeWatcher(pNULL->m_pcName));
	//	spWatcher->AddChild("id",
	//		MakeWatcher(*pNULL, &FvNetInterfaceElementWithStats::IDAsInt));
	//	spWatcher->AddChild("maxBytesReceived",
	//		MakeWatcher(pNULL->m_uiMaxBytesReceived));
	//	spWatcher->AddChild("bytesReceived",
	//		MakeWatcher(pNULL->m_uiNumBytesReceived));
	//	spWatcher->AddChild("messagesReceived",
	//		MakeWatcher(pNULL->m_uiNumMessagesReceived));
	//	spWatcher->AddChild("avgMessageLength",
	//		MakeWatcher(*pNULL,
	//			&FvNetInterfaceElementWithStats::AvgMessageLength));
	//	spWatcher->AddChild("avgBytesPerSecond",
	//		MakeWatcher(*pNULL,
	//			&FvNetInterfaceElementWithStats::AvgBytesReceivedPerSecond));
	//	spWatcher->AddChild("avgMessagesPerSecond",
	//		MakeWatcher(*pNULL,
	//			&FvNetInterfaceElementWithStats::
	//			AvgMessagesReceivedPerSecond));
	//}
	//return spWatcher;
	return NULL;
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
}

static std::shared_ptr<spdlog::logger> g_network_logger (NULL);
std::shared_ptr<spdlog::logger>& network_logger()
{
	if (g_network_logger.get())
		return g_network_logger;

	//This other example use a single logger with multiple sinks.
	//This means that the same log_msg is forwarded to multiple sinks;
	//Each sink can have it's own log level and a message will be logged.
	std::vector<spdlog::sink_ptr> sinks;
#ifdef _WIN32
	sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_st>());
#else
	sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_sink>(spdlog::sinks::stdout_sink_st::instance()));
#endif
	// Create a file rotating logger with 5mb size max and 3 rotated files
	//sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_st>(SPDLOG_FILENAME_T("logs/network"), SPDLOG_FILENAME_T("log"), 1048576 * 5, 3));
	sinks[0]->set_level(spdlog::level::trace);  // console. Allow everything.  Default value
	//sinks[1]->set_level(spdlog::level::info);  //  regular file. Allow everything.  Default value
	g_network_logger = std::make_shared<spdlog::logger>("g_network_logger", sinks.begin(), sinks.end());
	g_network_logger->set_level(spdlog::level::trace);
	spdlog::register_logger(g_network_logger);
	return g_network_logger;
}
