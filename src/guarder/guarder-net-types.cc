#include "guarder-net-types.h"

guarder_packet_t::guarder_packet_t() :
	m_uiFlags(0), dont_delete_msgs_(false)
{
}
guarder_packet_t::guarder_packet_t(geco_bit_stream_t &is) :
	m_uiFlags(0), dont_delete_msgs_(false)
{
	geco_zero_mem(&m_uiBuddy, sizeof(sockaddrunion));
	this->read(is);
}
guarder_packet_t::~guarder_packet_t()
{
	if (!dont_delete_msgs_)
	{
		for (unsigned i = 0; i < m_kMessages.size(); i++)
			if (m_kDelInfo[i])
				delete m_kMessages[i];
	}
}
void guarder_packet_t::add(guarder_msg_t* msg, bool should_delete)
{
	m_kMessages.push_back(msg);
	m_kDelInfo.push_back(should_delete);
}
void guarder_packet_t::read(geco_bit_stream_t &is)
{
	is.Read(m_uiFlags);
	if (saddr_family(&ms_uiBuddy.su) == AF_INET)
	{
		is.ReadRaw((char*)&s4addr(&ms_uiBuddy.su), sizeof(uint));
	}
	else
	{
		is.ReadRaw((char*)&saddr_family(&m_uiBuddy.su), sizeof(in6_addr));
	}

	uint msglen;
	guarder_msg_t* msg;
	while (is.get_payloads() > 0)
	{
		is.Read(msglen);
		msg = guarder_msg_t::from(is);
		if (msg != NULL)
		{
			this->add(msg, true);
		}
		else
		{
			network_logger()->error("guarder_packet_t::read()::Unknown message encountered: {}\n", msg->c_str());
		}
	}
}

bool guarder_packet_t::write(geco_bit_stream_t &os) const
{
	os.Write(m_uiFlags);
	const char* addr;
	if (saddr_family(&ms_uiBuddy.su) == AF_INET)
	{
		ms_uiBuddy.su.sin.sin_addr.s_addr == INADDR_LOOPBACK ? addr = (const char*)&s4addr(&ms_uiBuddy.su) : addr =
			(const char*)&s4addr(&m_uiBuddy.su);
		os.WriteRaw(addr, sizeof(uint));
	}
	else
	{
		IN6_ADDR_EQUAL(&ms_uiBuddy.su.sin6.sin6_addr, &in6addr_loopback) ?
			addr = (const char*)&saddr_family(&m_uiBuddy.su) : addr = (const char*)&saddr_family(&ms_uiBuddy.su);
		os.WriteRaw(addr, sizeof(in6_addr));
	}
	uint* msglenpos;
	uint msglen;
	for (auto msg : m_kMessages)
	{
		msglenpos = (uint*)os.reserve(sizeof(uint));
		msglen = msg->write(os);
		*(msglenpos) = GECO_HTONS(msglen);
	}
	bool isOversized = (os.get_bytes_length() > guarder_packet_t::MAX_SIZE);
	if (isOversized)
	{
		network_logger()->error("guarder_packet_t::write:: Wrote {} bytes long/{} messages."
			"You need to implement MGM fragmentation", os.get_bytes_length(), m_kMessages.size());
	}
	return !isOversized;
}

guarder_msg_t::guarder_msg_t(uchar message, uchar flags /*= 0*/, ushort seq /*= 0*/) :
	m_uiMessage(message), m_uiFlags(flags), m_uiSeq(seq), m_bSeqSent(false)
{
	if (m_uiSeq == 0)
		this->refresh_seq();
}
guarder_msg_t * guarder_msg_t::from(geco_bit_stream_t &is)
{
	uint message;
	is.Read(message);
	guarder_msg_t *pMgm = NULL;

	switch (message)
	{
	case guarder_msg_t::HIGH_PRECISION_MESSAGE:
		pMgm = new high_precision_msg_t();
		break;
	case guarder_msg_t::MACHINE_MESSAGE:
		pMgm = new machine_msg_t();
		break;
	case guarder_msg_t::PROCESS_MESSAGE:
		pMgm = new process_msg_t();
		break;
	case guarder_msg_t::PROCESS_STATS_MESSAGE:
		pMgm = new process_stats_msg_t();
		break;
	case guarder_msg_t::LISTENER_MESSAGE:
		pMgm = new listenner_msg_t();
		break;
	case guarder_msg_t::CREATE_MESSAGE:
		pMgm = new create_msg_t();
		break;
	case guarder_msg_t::SIGNAL_MESSAGE:
		pMgm = new signal_msg_t();
		break;
	case guarder_msg_t::TAGS_MESSAGE:
		pMgm = new tag_msg_t();
		break;
	case guarder_msg_t::USER_MESSAGE:
		pMgm = new user_msg_t();
		break;
	case guarder_msg_t::PID_MESSAGE:
		pMgm = new pid_msg_t();
		break;
	case guarder_msg_t::RESET_MESSAGE:
		pMgm = new reset_msg_t();
		break;
	case guarder_msg_t::ERROR_MESSAGE:
		pMgm = new error_msg_t();
		break;
	case guarder_msg_t::GUARDER_ANNOUNCE_MESSAGE:
		pMgm = new announce_msg_t();
		break;
	case guarder_msg_t::QUERY_INTERFACE_MESSAGE:
		pMgm = new query_interface_msg_t();
		break;
	case guarder_msg_t::CREATE_WITH_ARGS_MESSAGE:
		pMgm = new create_with_args_msg_t();
		break;
	default:
		return NULL;
	}
}
guarder_msg_t * guarder_msg_t::from(void *buf, int length)
{
	geco_bit_stream_t is((uchar*)buf, length, false);
	return from(is);
}
const char* guarder_msg_t::c_str() const
{
	strcpy(ms_acBuf, this->type2str());
	return ms_acBuf;
}
const char* guarder_msg_t::type2str() const
{
	static char buf[32];
	switch (m_uiMessage)
	{
	case HIGH_PRECISION_MESSAGE:
		strcpy(buf, "HIGH_PRECISION");
		break;
	case MACHINE_MESSAGE:
		strcpy(buf, "WHOLE_MACHINE");
		break;
	case PROCESS_MESSAGE:
		strcpy(buf, "PROCESS");
		break;
	case PROCESS_STATS_MESSAGE:
		strcpy(buf, "PROCESS_STATS");
		break;
	case LISTENER_MESSAGE:
		strcpy(buf, "LISTENER");
		break;
	case CREATE_MESSAGE:
		strcpy(buf, "CREATE");
		break;
	case SIGNAL_MESSAGE:
		strcpy(buf, "SIGNAL");
		break;
	case TAGS_MESSAGE:
		strcpy(buf, "TAGS");
		break;
	case USER_MESSAGE:
		strcpy(buf, "USER");
		break;
	case PID_MESSAGE:
		strcpy(buf, "PID");
		break;
	case RESET_MESSAGE:
		strcpy(buf, "RESET");
		break;
	case GUARDER_ANNOUNCE_MESSAGE:
		strcpy(buf, "ANNOUNCE");
		break;
	case QUERY_INTERFACE_MESSAGE:
		strcpy(buf, "QUERY_INTERFACE");
		break;
	default:
		strcpy(buf, "** UNKNOWN **");
		break;
	}
	return buf;
}
uint guarder_msg_t::write(geco_bit_stream_t & os)
{
	if (m_bSeqSent && !this->outgoing())
		this->refresh_seq();
	os.Write(m_uiMessage);
	os.Write(m_uiFlags);
	os.Write(m_uiSeq);
	m_bSeqSent = true;
	return sizeof(uint) * 3;
	// uint extrasize = write_extra(os);
	//return sizeof(uint) * 3 + extrasize;
}
void guarder_msg_t::read(geco_bit_stream_t &is)
{
	is.Read(m_uiMessage);
	is.Read(m_uiFlags);
	is.Read(m_uiSeq);
	//read_extra(is);
}
void guarder_msg_t::refresh_seq()
{
	if (ms_uiSeqTicker == 0)
	{
		srand(gettimestamp());
		ms_uiSeqTicker = rand() % 0xffff;
	}
	ms_uiSeqTicker = (ms_uiSeqTicker + 1) % 0xffff;
	if (ms_uiSeqTicker == 0)
		ms_uiSeqTicker++;
	m_uiSeq = ms_uiSeqTicker;
	m_bSeqSent = false;
}
void guarder_msg_t::copy_seq(const guarder_msg_t &mgm)
{
	m_uiSeq = mgm.m_uiSeq;
	m_bSeqSent = false;
}
bool guarder_msg_t::sendto(GecoNetEndpoint &ep, GecoNetAddress& addr, uint packFlags)
{
	guarder_packet_t packet;
	geco_bit_stream_t os;
	packet.m_uiFlags = packFlags;
	packet.add(this);
	packet.write(os);
	return ep.SendTo(os.uchar_data(), os.get_bytes_length(), addr);
}
GecoNetReason guarder_msg_t::send_and_recv(GecoNetEndpoint &ep, GecoNetAddress& dest,
	guarder_input_msg_handler_t *pHandler /*= NULL*/)
{
	ep.set_sock_broadcast(dest.is_broadcast_addr());
	fd_set fds;
	int fd = int(ep);
	timeval tv = { 1, 0 };
	GecoNetAddress srcaddr, buddyaddr;
	int recvlen;
	bool badseq;
	bool continueProcessing;
	int countdown = 3;
	while (countdown--)
	{
		if (!this->sendto(ep, dest, guarder_packet_t::PACKET_STAGGER_REPLIES))
		{
			network_logger()->error(
				"guarder_msg_t::send_and_recv(): Failed to send entire MGM ({} tries left), destAddr:{}, errorno:{}",
				countdown, dest.c_str(), strerror(errno));
			continue;
		}
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		while (select(fd + 1, &fds, NULL, NULL, &tv) == 1)
		{
			recvlen = ep.RecvFrom(guarder_packet_t::recvbuf, guarder_packet_t::MAX_SIZE, srcaddr);
			if (recvlen == -1)
			{
				network_logger()->warn("guarder_msg_t::send_and_recv(): recvfrom failed ({})\n", strerror(errno));
				continue;
			}
			badseq = false;
			continueProcessing = true;
			geco_bit_stream_t is((uchar*)guarder_packet_t::recvbuf, recvlen);
			guarder_packet_t packet(is);
			buddyaddr = packet.m_uiBuddy;
			for (auto replymsgptr : packet.m_kMessages)
			{
				guarder_msg_t& reply = *replymsgptr;
				if (reply.m_uiSeq != m_uiSeq)
				{
					network_logger()->warn("guarder_msg_t::send_and_recv(): Bad seq ({}, wanted {}) from {}:{}",
						reply.m_uiSeq, m_uiSeq, srcaddr.c_str(), reply.c_str());
					badseq = true;
					break;
				}
				if (reply.m_uiFlags & reply.MESSAGE_NOT_UNDERSTOOD)
				{
					network_logger()->error(
						"guarder_msg_t::send_and_recv(): Machined on [{}] did not understand msg type [{}]",
						srcaddr.c_str(), reply.c_str());
					continue;
				}
				if (pHandler)
				{
					continueProcessing = pHandler->handle(reply, srcaddr);
					if (!continueProcessing)
						break;
				}
			}

			if (badseq)
				continue;

			guarder_packet_t::replied.insert(srcaddr);
			if (guarder_packet_t::waiting.count(srcaddr))
				guarder_packet_t::waiting.erase(srcaddr);
			if (buddyaddr != GecoNetAddress::NONE && guarder_packet_t::replied.count(buddyaddr) == 0)
				guarder_packet_t::waiting.insert(buddyaddr); // store byddy addr if it is not replied

			if (!continueProcessing || !dest.is_broadcast_addr()
				|| (guarder_packet_t::replied.size() > 0 && guarder_packet_t::waiting.size() == 0))
				return GECO_NET_REASON_SUCCESS;
		}
	}
	guarder_packet_t::replied.clear();
	guarder_packet_t::waiting.clear();
	network_logger()->error("guarder_msg_t::send_and_recv(): timed out!");
	return GECO_NET_REASON_TIMER_EXPIRED;
}

GecoNetReason guarder_msg_t::send_and_recv(const char* bindipaddr, GecoNetAddress& dest,
	guarder_input_msg_handler_t *pHandler /*= NULL*/)
{
	GecoNetEndpoint ep;
	ep.Socket(AF_INET, SOCK_DGRAM);
	if (!ep.Good() || ep.Bind(0, bindipaddr) != 0)
		return GECO_NET_REASON_GENERAL_NETWORK;
}
GecoNetReason guarder_msg_t::send_and_recv_with_endpoint_address(GecoNetEndpoint & ep, GecoNetAddress& dest,
	guarder_input_msg_handler_t * pHandler /*= NULL*/)
{
	GecoNetAddress tmp;
	if (ep.GetLocalAddress(&tmp))
	{
		network_logger()->error(
			"guarder_msg_t::send_and_recv_with_endpoint_address(): Couldn't get local address of provided endpoint");
		return GECO_NET_REASON_GENERAL_NETWORK;
	}
	char ipstr[256];
	saddr2str(&tmp.su, ipstr, 256);
	return this->send_and_recv(ipstr, dest, pHandler);
}
bool guarder_input_msg_handler_t::handle(guarder_msg_t& reply, GecoNetAddress& srcaddr)
{
	throw std::logic_error("The method or operation is not implemented.");
}


query_interface_msg_t::query_interface_msg_t() : guarder_msg_t(guarder_msg_t::QUERY_INTERFACE_MESSAGE) {}
query_interface_msg_t::~query_interface_msg_t() {}
uint query_interface_msg_t::write(geco_bit_stream_t & os)
{
	uint size = guarder_msg_t::write(os);
	address_.su.sa.sa_family = htons(address_.su.sa.sa_family);
	os.WriteRaw((uchar*)&address_.su, sizeof(sockaddr));
	return size + sizeof(sockaddrunion);
}
void query_interface_msg_t::read(geco_bit_stream_t &is)
{
	guarder_msg_t::read(is);
	is.ReadRaw((uchar*)&address_.su, sizeof(sockaddr));
	address_.su.sa.sa_family = ntohs(address_.su.sa.sa_family);
}
const char* query_interface_msg_t::c_str() const
{
	geco_snprintf(guarder_msg_t::ms_acBuf, sizeof(guarder_msg_t::ms_acBuf), "QueryInterfaceMessage");
	return guarder_msg_t::ms_acBuf;
}


listenner_msg_t::listenner_msg_t()
{
	m_uiMessage = guarder_msg_t::LISTENER_MESSAGE;
}
listenner_msg_t::~listenner_msg_t() {}
uint listenner_msg_t::write(geco_bit_stream_t & os)
{
	uint size = guarder_msg_t::write(os);
	os.Write((uint)(strlen(m_kPreAddr) + 1));
	os.WriteRaw((uchar*)m_kPreAddr, strlen(m_kPreAddr) + 1);
	os.Write((uint)(strlen(m_kPostAddr) + 1));
	os.WriteRaw((uchar*)m_kPostAddr, strlen(m_kPostAddr) + 1);
	return size + strlen(m_kPreAddr) + strlen(m_kPostAddr) + 2;
}
void listenner_msg_t::read(geco_bit_stream_t &is)
{
	guarder_msg_t::read(is);
	uint len;
	is.Read(len);
	is.ReadRaw((uchar*)m_kPreAddr, len);
	is.Read(len);
	is.ReadRaw((uchar*)m_kPostAddr, len);
}
const char *listenner_msg_t::c_str() const
{
	geco_snprintf(guarder_msg_t::ms_acBuf, sizeof(guarder_msg_t::ms_acBuf), "QueryInterfaceMessage");
	return guarder_msg_t::ms_acBuf;
}


process_msg_t::process_msg_t() : guarder_msg_t(guarder_msg_t::PROCESS_MESSAGE) {}
process_msg_t::~process_msg_t() {}
uint process_msg_t::write(geco_bit_stream_t & os)
{
	uint size = guarder_msg_t::write(os);
	os.Write(this->m_uiParam);
	os.Write(this->m_uiCategory);
	os.Write(this->m_uiUID);
	os.Write(this->m_uiPID);
	os.Write(this->m_uiPort);
	os.Write(this->m_uiID);
	os.Write((uint)(strlen(m_kName) + 1));
	os.WriteRaw((uchar*)m_kName, strlen(m_kName) + 1);
}
void process_msg_t::read(geco_bit_stream_t & is)
{
	guarder_msg_t::read(is);
	is.Read(this->m_uiParam);
	is.Read(this->m_uiCategory);
	is.Read(this->m_uiUID);
	is.Read(this->m_uiPID);
	is.Read(this->m_uiPort);
	is.Read(this->m_uiID);
	uint len;
	is.Read(len);
	is.ReadRaw((uchar*)m_kName, len);
}
const char *process_msg_t::cate2str(uint category) const
{
	static const char* sc = "SERVER_COMPONENT", *wn = "WATCHER_NUB", *err = "UNKNOWN";
	if (category == SERVER_COMPONENT)
		return sc;
	else if (category == WATCHER_NUB)
		return wn;
	else
		return err;
}
const char * process_msg_t::c_str() const
{
	geco_snprintf(guarder_msg_t::ms_acBuf, sizeof(guarder_msg_t::ms_acBuf), "%s (%s) (pid:%d) (uid:%d) (port:%d)",
		m_kName, process_msg_t::cate2str(m_uiCategory), m_uiPID, m_uiUID, m_uiPort);
	return guarder_msg_t::ms_acBuf;
}
bool process_msg_t::matches(const process_msg_t & query) const
{
	if (query.m_uiParam & query.PARAM_USE_UID && query.m_uiUID != m_uiUID)
		return false;
	if (query.m_uiParam & query.PARAM_USE_PID && query.m_uiPID != m_uiPID)
		return false;
	if (query.m_uiParam & query.PARAM_USE_ID && query.m_uiID != m_uiID)
		return false;
	if (query.m_uiParam & query.PARAM_USE_NAME && query.m_kName != m_kName)
		return false;
	if (query.m_uiParam & query.PARAM_USE_CATEGORY && query.m_uiCategory != m_uiCategory)
		return false;
	if (query.m_uiParam & query.PARAM_USE_PORT && query.m_uiPort != m_uiPort)
		return false;
	return true;
}
process_msg_t & process_msg_t::operator<<(const process_msg_t &pm)
{
	m_uiParam = pm.m_uiParam;
	m_uiCategory = pm.m_uiCategory;
	m_uiUID = pm.m_uiUID;
	m_uiPID = pm.m_uiPID;
	m_uiPort = pm.m_uiPort;
	m_uiID = pm.m_uiID;
	memcpy_fast(m_kName, pm.m_kName, strlen(pm.m_kName) + 1);
	return *this;
}
