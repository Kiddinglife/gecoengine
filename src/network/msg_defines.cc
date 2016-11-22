#include "msg_defines.h"

DECLARE_DEBUG_COMPONENT2("Network", 0);


inline geco_engine_reason packet_recv_t::process_packet()
{
	geco_engine_reason ret = geco_engine_reason::SUCCESS;
	curr_packet_->m_uiIdentifier;
}

geco_bundle_t::geco_bundle_t(uchar spareSize, geco_channel_t* pChannel) :
	m_spFirstPacket(NULL),
	m_pkCurrentPacket(NULL),
	m_bFinalised(false),
	m_uiExtraSize(spareSize),
	m_pkChannel(pChannel)
{
	this->clear(true/*do_not_dispose*/);
}

geco_bundle_t::geco_bundle_t(uchar* packetChain) :
	m_spFirstPacket(packetChain),
	m_pkCurrentPacket(packetChain),
	m_bFinalised(true),
	m_uiExtraSize(0),
	m_pkChannel(NULL)
{
	this->clear(true/*do_not_dispose*/);
}


geco_bundle_t::~geco_bundle_t()
{

}

inline void geco_bundle_t::dispose()
{
	m_spFirstPacket = NULL;
	m_pkCurrentPacket = NULL;

	m_kReplyOrders.clear();
	m_kReliableOrders.clear();

	for (piggy_backs_t::iterator iter = m_kPiggybacks.begin();iter != m_kPiggybacks.end(); ++iter) delete *iter;
	m_kPiggybacks.clear();

	m_kAckOrders.clear();
}

inline void geco_bundle_t::clear(bool do_not_dispose)
{
	if (!do_not_dispose)
	{
		this->dispose();
		m_bFinalised = false;
	}

	m_bReliableDriver = false;
	m_iReliableOrdersExtracted = 0;
	m_bIsCritical = false;
	m_pkCurIE = NULL;
	m_iMsgLen = 0;
	m_puiMsgBeg = NULL;
	m_uiMsgChunkOffset = 0;
	m_bMsgReliable = false;
	m_bMsgRequest = false;
	m_iNumMessages = 0;
	m_iNumReliableOrderedMessages = 0;
	m_iNumReliableUnOrderedMessages = 0;
	m_iNumUnReliableOrderedMessages = 0;
	m_iNumUnReliableUnOrderedMessages = 0;
	m_iNumMessagesTotalBytes = 0;

	if (m_spFirstPacket == NULL)
	{
		//m_spFirstPacket = new FvNetPacket();
		//this->StartPacket(m_spFirstPacket.GetObject());
	}
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

inline uchar* geco_bundle_t::reserve(int extra)
{
	m_ucSendBuffers[m_pkCurIE->ro_].AppendBitsCouldRealloc(extra * 8);
	return m_ucSendBuffers[m_pkCurIE->ro_].get_written_bytes()+ m_ucSendBuffers[m_pkCurIE->ro_].uchar_data();
}

uchar* geco_bundle_t::new_message(uint extra)
{
	// figure the length of the header
	m_uiHeaderLen = m_pkCurIE->hdr_len_;
	if (m_uiHeaderLen < 0)
	{
		CRITICAL_MSG("geco_bundle_t::new_message(extra=%d)::"
			"tried to add a message with an unknown length format %d\n",
			extra, (int)m_pkCurIE->lengthStyle_);
	}

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

	m_pHeader = reserve(m_uiHeaderLen + extra);
	m_puiMsgBeg = m_pHeader; 		// set the start of this msg
	m_ucSendBuffers[m_pkCurIE->ro_].Write(m_pkCurIE->id_);
	m_iMsgLen = 0; 	// set the length to zero as we do not know it is vari or fix msg
	m_iMsgExtra = extra;
	return (m_pHeader + m_uiHeaderLen);  //return a pointer to the extra data
}

inline void geco_bundle_t::start_request_message(const interface_element_t& ie)
{
	if (m_pkChannel == NULL)
	{
		WARNING_MSG("geco_bundle_t::start_request_message(): no channel set !\n");
		return;
	}
	this->end_message();
	m_pkCurIE = &ie;
	m_bMsgRequest = true;
	this->new_message(ie.lengthParam_);
}

inline void geco_bundle_t::start_request_message(const interface_element_t& ie, response_handler_t& response_handler, void * arg, uint timeout)
{
	if (m_pkChannel == NULL)
	{
		WARNING_MSG("geco_bundle_t::start_request_message(): no channel set !\n");
		return;
	}
	if (ie.ro_ != RELIABLE_ORDER && ie.ro_ != RELIABLE_UNORDER)
	{
		WARNING_MSG("geco_bundle_t::start_request_message(): UNRELIABLE NOT ALLOWED FOR REQUEST MSG WITH RESPONSE EXPECTED!\n");
		return;
	}
	if (timeout <= 0)
	{
		// Requests never timeout on channels.
		WARNING_MSG("geco_bundle_t::start_request_message(channel: %s): no timeout set !\n", m_pkChannel->c_str());
		return;
	}
	this->end_message();
	m_pkCurIE = &ie;
	m_bMsgRequest = true;

	// Start a new message, and set bit for the reply flag 
	this->new_message(/*extra=*/0);

	// now make and add a response order
	response_order_t& ro = m_kReplyOrders.push_back();
	ro.response_handler = response_handler;
	ro.timeout_ms = timeout;
	ro.arg = arg;
}


inline void geco_bundle_t::start_response_message(const interface_element_t& ie)
{
	if (m_pkChannel == NULL)
	{
		WARNING_MSG("geco_bundle_t::start_response_message(): no channel set !\n");
		return;
	}
	if (ie.ro_ != RELIABLE_ORDER && ie.ro_ != RELIABLE_UNORDER)
	{
		WARNING_MSG("geco_bundle_t::start_response_message(): UNRELIABLE NOT ALLOWED FOR RESPONSE MSG!\n");
		return;
	}
	this->end_message();
	m_pkCurIE = &ie;
	m_bMsgRequest = false;
	this->new_message(/*extra=*/0);
}

inline void geco_bundle_t::end_message()
{
	// nothing to do if no message yet
	if (m_puiMsgBeg == NULL) return;
	// fill in msg hdr including msgid, msglen and msgflags
	m_pkCurIE->compress_length(m_puiMsgBeg, m_iMsgLen, *this, m_bMsgRequest);
	m_puiMsgBeg = NULL;
}



