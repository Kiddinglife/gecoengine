#include "guarder-net-types.h"

void guarder_packet_t::read(geco_bit_stream_t &is)
{
	is.Read(m_uiFlags);
	if (saddr_family(&ms_uiBuddy) == AF_INET)
	{
		is.ReadRaw((char*)&s4addr(&ms_uiBuddy), sizeof(uint));
	}
	else
	{
		is.ReadRaw((char*)&saddr_family(&m_uiBuddy), sizeof(in6_addr));
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
	if (saddr_family(&ms_uiBuddy) == AF_INET)
	{
		s4addr(&ms_uiBuddy) != in4addr_loopback.s_addr ?
			os.WriteRaw((const char*)&s4addr(&ms_uiBuddy), sizeof(uint)) :
			os.WriteRaw((const char*)&s4addr(&m_uiBuddy), sizeof(uint));
	}
	else
	{
		IN6_ADDR_EQUAL(&ms_uiBuddy.sin6.sin6_addr, &in6addr_loopback) ?
			os.WriteRaw((const char*)&saddr_family(&m_uiBuddy), sizeof(in6_addr)) :
			os.WriteRaw((const char*)&saddr_family(&ms_uiBuddy), sizeof(in6_addr));
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

guarder_msg_t::guarder_msg_t(uchar message, uchar flags /*= 0*/, ushort seq /*= 0*/)
{

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

}

const char* guarder_msg_t::c_str() const
{
	strcpy(ms_acBuf, this->msg_type2str());
	return ms_acBuf;
}

const char* guarder_msg_t::msg_type2str() const
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
	throw std::logic_error("The method or operation is not implemented.");
}