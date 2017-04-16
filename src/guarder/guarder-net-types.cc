#include "guarder-net-types.h"

guarder_packet_t::guarder_packet_t() : m_uiFlags(0), dont_delete_msgs_(false) {}
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
		is.ReadRaw((char*)&s4addr(&ms_uiBuddy, su), sizeof(uint));
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
		IN4_ADDR_EQUAL(&ms_uiBuddy.su.sin.sin_addr, &in4addr_loopback) ?
			addr = (const char*)&s4addr(&ms_uiBuddy.su) : addr = (const char*)&s4addr(&m_uiBuddy.su);
		os.WriteRaw(addr, sizeof(uint));
	}
	else
	{
		IN6_ADDR_EQUAL(&ms_uiBuddy.su.sin6.sin6_addr, &in6addr_loopback) ?
			addr = (const char*)&saddr_family(&m_uiBuddy.su) :
			addr = (const char*)&saddr_family(&ms_uiBuddy.su);
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
	if (m_uiSeq == 0) this->refresh_seq();
}
guarder_msg_t * guarder_msg_t::from(geco_bit_stream_t &is)
{
	uint message;
	is.Read(message);
	guarder_msg_t *pMgm = NULL;

	switch (message)
	{
	case guarder_msg_t::HIGH_PRECISION_MESSAGE:
		pMgm = new high_precision_msg_t(); break;
	case guarder_msg_t::MACHINE_MESSAGE:
		pMgm = new machine_msg_t(); break;
	case guarder_msg_t::PROCESS_MESSAGE:
		pMgm = new process_msg_t(); break;
	case guarder_msg_t::PROCESS_STATS_MESSAGE:
		pMgm = new process_stats_msg_t(); break;
	case guarder_msg_t::LISTENER_MESSAGE:
		pMgm = new listenner_msg_t(); break;
	case guarder_msg_t::CREATE_MESSAGE:
		pMgm = new create_msg_t(); break;
	case guarder_msg_t::SIGNAL_MESSAGE:
		pMgm = new signal_msg_t(); break;
	case guarder_msg_t::TAGS_MESSAGE:
		pMgm = new tag_msg_t(); break;
	case guarder_msg_t::USER_MESSAGE:
		pMgm = new user_msg_t(); break;
	case guarder_msg_t::PID_MESSAGE:
		pMgm = new pid_msg_t(); break;
	case guarder_msg_t::RESET_MESSAGE:
		pMgm = new reset_msg_t(); break;
	case guarder_msg_t::ERROR_MESSAGE:
		pMgm = new error_msg_t(); break;
	case guarder_msg_t::ANNOUNCE_MESSAGE:
		pMgm = new announce_msg_t(); break;
	case guarder_msg_t::QUERY_INTERFACE_MESSAGE:
		pMgm = new query_interface_msg_t(); break;
	case guarder_msg_t::CREATE_WITH_ARGS_MESSAGE:
		pMgm = new create_with_args_msg_t(); break;
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
	case HIGH_PRECISION_MESSAGE: strcpy(buf, "HIGH_PRECISION"); break;
	case MACHINE_MESSAGE: strcpy(buf, "WHOLE_MACHINE"); break;
	case PROCESS_MESSAGE: strcpy(buf, "PROCESS"); break;
	case PROCESS_STATS_MESSAGE: strcpy(buf, "PROCESS_STATS"); break;
	case LISTENER_MESSAGE: strcpy(buf, "LISTENER"); break;
	case CREATE_MESSAGE: strcpy(buf, "CREATE"); break;
	case SIGNAL_MESSAGE: strcpy(buf, "SIGNAL"); break;
	case TAGS_MESSAGE: strcpy(buf, "TAGS"); break;
	case USER_MESSAGE: strcpy(buf, "USER"); break;
	case PID_MESSAGE: strcpy(buf, "PID"); break;
	case RESET_MESSAGE: strcpy(buf, "RESET"); break;
	case ANNOUNCE_MESSAGE: strcpy(buf, "ANNOUNCE"); break;
	case QUERY_INTERFACE_MESSAGE: strcpy(buf, "QUERY_INTERFACE"); break;
	default: strcpy(buf, "** UNKNOWN **"); break;
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
}
void guarder_msg_t::Read(geco_bit_stream_t &is)
{
	is.Read(m_uiMessage);
	is.Read(m_uiFlags);
	is.Read(m_uiSeq);
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