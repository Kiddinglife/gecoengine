#include "net-types.h"
//#include "GecoWatcher.h"

#ifdef _WIN32
#ifndef _XBOX360
#include <Winsock.h>
#endif
#elif defined( PLAYSTATION3 )
#include <netinet/in.h>
#else
#include <arpa/inet.h>
#endif

bool g_enable_stats = true;

/*GecoNetAddress*/
char GecoNetAddress::ms_pcStringBuf[2][GecoNetAddress::MAX_STRLEN];
int GecoNetAddress::ms_iCurrStringBuf = 0;
const GecoNetAddress GecoNetAddress::NONE(0, 0);
bool WatcherStringToValue(const char * valueStr, GecoNetAddress & value)
{
	int a1, a2, a3, a4, a5;
	if (sscanf(valueStr, "%d.%d.%d.%d:%d",
		&a1, &a2, &a3, &a4, &a5) != 5)
	{
		network_logger()->error("WatcherStringToValue: Cannot convert '{}' to an Address", valueStr);
		return false;
	}
	value.m_uiIP = (a1 << 24) | (a2 << 16) | (a3 << 8) | a4;
	value.m_uiPort = ushort(a5);
	value.m_uiPort = ntohs(value.m_uiPort);
	value.m_uiIP = ntohl(value.m_uiIP);
	return true;
}
GecoNetAddress::GecoNetAddress(char* str)
	:m_uiIP(0)
	, m_uiPort(0)
	, m_uiSalt(0)
{
	SetFromString(str);
}
int GecoNetAddress::WriteToString(char * str, int length) const
{
	uint	hip = ntohl(m_uiIP);
	ushort	hport = ntohs(m_uiPort);

	return geco_snprintf(str, length,
		"%d.%d.%d.%d:%d",
		(int)(unsigned char)(hip >> 24),
		(int)(unsigned char)(hip >> 16),
		(int)(unsigned char)(hip >> 8),
		(int)(unsigned char)(hip),
		(int)hport);
}
char * GecoNetAddress::c_str() const
{
	char * buf = GecoNetAddress::NextStringBuf();
	this->WriteToString(buf, MAX_STRLEN);
	return buf;
}
const char * GecoNetAddress::IPAsString() const
{
	uint	hip = ntohl(m_uiIP);
	char * buf = GecoNetAddress::NextStringBuf();

	geco_snprintf(buf, MAX_STRLEN,
		"%d.%d.%d.%d",
		(int)(unsigned char)(hip >> 24),
		(int)(unsigned char)(hip >> 16),
		(int)(unsigned char)(hip >> 8),
		(int)(unsigned char)(hip));

	return buf;
}
bool GecoNetAddress::SetFromString(char* str)
{
	if (!str)
		return false;

	int a1, a2, a3, a4, a5;

	if (sscanf(str, "%d.%d.%d.%d:%d",
		&a1, &a2, &a3, &a4, &a5) != 5)
		return false;

	m_uiIP = (a1 << 24) | (a2 << 16) | (a3 << 8) | a4;

	m_uiPort = ushort(a5);
	m_uiPort = ntohs(m_uiPort);
	m_uiIP = ntohl(m_uiIP);

	return true;
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
static bool s_network_logger_inited = false;
static std::shared_ptr<spdlog::logger> g_network_logger;
std::shared_ptr<spdlog::logger>& network_logger()
{
	if (s_network_logger_inited)
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
	s_network_logger_inited = true;
	return g_network_logger;
}
