#include "msg_defines.h"

DECLARE_DEBUG_COMPONENT2("Network", 0);


inline geco_engine_reason packet_recv_t::process_packet()
{
	geco_engine_reason ret = geco_engine_reason::SUCCESS;
	curr_packet_->m_uiIdentifier;
}

geco_bundle_t::geco_bundle_t(uchar spareSize, geco_channel_t* pChannel) :
	m_bFinalised(false),
	m_uiExtraSize(spareSize),
	m_pkChannel(pChannel),
	m_CurrBuf(NULL),
	m_isExternal(m_pkChannel && m_pkChannel->m_uiType == geco_channel_t::WAN)
{
	this->clear(true/*do_not_dispose*/);
}

geco_bundle_t::geco_bundle_t(uchar* packetChain) :
	m_bFinalised(true),
	m_uiExtraSize(0),
	m_pkChannel(NULL),
	m_CurrBuf(NULL),
	m_isExternal(m_pkChannel && m_pkChannel->m_uiType == geco_channel_t::WAN)
{
	this->clear(true/*do_not_dispose*/);
}

geco_bundle_t::~geco_bundle_t()
{
	this->dispose();
}

inline void geco_bundle_t::dispose()
{
	m_kReplyOrders.clear();
}

inline void geco_bundle_t::clear(bool do_not_dispose)
{
	if (!do_not_dispose)
	{
		this->dispose();
		m_bFinalised = false;
	}
	m_pkCurIE = NULL;
	m_iMsgLen = 0;
	m_puiMsgBeg = NULL;
	m_bMsgRequest = false;
	m_iNumMessages = 0;
	m_iNumReliableOrderedMessages = 0;
	m_iNumReliableUnOrderedMessages = 0;
	m_iNumUnReliableOrderedMessages = 0;
	m_iNumUnReliableUnOrderedMessages = 0;
	m_iNumMessagesTotalBytes = 0;
}

inline bool geco_bundle_t::empty() const { return (m_iNumMessages == 0); }
inline uint geco_bundle_t::size() const { return m_iNumMessagesTotalBytes; }
inline bool geco_bundle_t::is_external_channel() const { return m_pkChannel && m_pkChannel->m_uiType == geco_channel_t::WAN; }


void geco_bundle_t::send(const sockaddrunion& address, geco_network_interface_t& networkInterface, geco_channel_t* pChannel)
{
	//@TODO
}

void geco_bundle_t::cancel_requests()
{
	response_orders_t::iterator iter = m_kReplyOrders.begin();
	response_orders_t::iterator endd = m_kReplyOrders.end();
	while (iter != endd)
	{
		iter->response_handler.exception_handler_("requests are cancelled for eneral network reason !", iter->arg);
		++iter;
	}
	m_kReplyOrders.clear();
}

inline uchar* geco_bundle_t::reserve(uint extra)
{
	m_CurrBuf->AppendBitsCouldRealloc(extra << 3);
	return m_CurrBuf->get_written_bytes() + m_CurrBuf->uchar_data();
}

inline void geco_bundle_t::new_message()
{
	++m_iNumMessages;
	switch (m_pkCurIE->ro_)
	{
	case RELIABLE_ORDER:
		++m_iNumReliableOrderedMessages;
		break;
	case RELIABLE_UNORDER:
		++m_iNumReliableUnOrderedMessages;
		break;
	case UNRELIABLE_ORDER:
		++m_iNumUnReliableOrderedMessages;
		break;
	case UNRELIABLE_UNORDER:
		++m_iNumUnReliableUnOrderedMessages;
		break;
	default:
		WARNING_MSG("geco_bundle_t::new_message(): no such m_pkCurIE->ro_(%d) !\n", m_pkCurIE->ro_);
		break;
	}

	m_CurrBuf = &m_ucSendBuffers[m_pkCurIE->ro_];
	if (is_external_channel())
	{
		m_CurrBuf->WriteMini(m_pkCurIE->id_);
		m_CurrBuf->WriteMini(m_pkCurIE->flag_);
		// compression do NOT need align
		//m_MsgLenPtr = m_CurrBuf->uchar_data() + m_CurrBuf->get_written_bytes();
		//m_CurrBuf->write_two_aligned_bytes((const char*)&m_pkCurIE->lengthParam_);
	}
	else
	{
		m_CurrBuf->Write(m_pkCurIE->id_);
		m_CurrBuf->Write(m_pkCurIE->flag_);
		//m_MsgLenPtr = m_CurrBuf->uchar_data() + m_CurrBuf->get_written_bytes();
		// non-compression DO need align
		m_CurrBuf->Write((ushort)0);
	}
}

geco_bit_stream_t* geco_bundle_t::start_request_message(interface_element_t& ie)
{
	if (m_pkChannel == NULL)
		WARNING_MSG("geco_bundle_t::start_request_message(): no channel set !\n");

	m_pkCurIE = &ie;
	m_pkCurIE->flag_ |= MSG_REQUEST;
	this->new_message();
	return m_CurrBuf;
}

geco_bit_stream_t* geco_bundle_t::start_request_message(interface_element_t& ie, response_handler_t& response_handler, void * arg, uint timeout)
{
	if (m_pkChannel == NULL)
		WARNING_MSG("geco_bundle_t::start_request_message(): no channel set !\n");

	if (ie.ro_ != RELIABLE_ORDER && ie.ro_ != RELIABLE_UNORDER)
		WARNING_MSG("geco_bundle_t::start_request_message(): UNRELIABLE NOT ALLOWED FOR REQUEST MSG WITH RESPONSE EXPECTED!\n");

	if (timeout <= 0)
		// Requests never timeout on channels.
		WARNING_MSG("geco_bundle_t::start_request_message(channel: %s): no timeout set !\n", m_pkChannel->c_str());

	m_pkCurIE = &ie;
	m_pkCurIE->flag_ |= MSG_REQUEST;
	this->new_message();
	// now make and add a response order
	response_order_t& ro = m_kReplyOrders.push_back();
	ro.response_handler = response_handler;
	ro.timeout_ms = timeout;
	ro.arg = arg;
	return m_CurrBuf;
}


geco_bit_stream_t* geco_bundle_t::start_response_message(interface_element_t& ie)
{
	if (m_pkChannel == NULL)
		WARNING_MSG("geco_bundle_t::start_response_message(): no channel set !\n");

	if (ie.ro_ != RELIABLE_ORDER && ie.ro_ != RELIABLE_UNORDER)
		WARNING_MSG("geco_bundle_t::start_response_message(): UNRELIABLE NOT ALLOWED FOR RESPONSE MSG!\n");

	m_pkCurIE = &ie;
	m_pkCurIE->flag_ |= MSG_RESPONSE;
	this->new_message();
	return m_CurrBuf;
}

unpacked_msg_hdr_t* geco_bundle_t::decode_message(interface_element_t& ie)
{
	m_unpacked_msg_hdr.m_uiIdentifier = ie.id_;
	m_unpacked_msg_hdr.
}


