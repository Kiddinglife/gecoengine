#include "msg_defines.h"

DECLARE_DEBUG_COMPONENT2("Network", 0);


geco_engine_reason packet_recv_t::process_packet()
{
	geco_engine_reason ret = geco_engine_reason::SUCCESS;
	curr_packet_->m_uiIdentifier;
}

void geco_bundle_t::dispose()
{
	m_spFirstPacket = NULL;
	m_pkCurrentPacket = NULL;

	m_kReplyOrders.clear();
	m_kReliableOrders.clear();

	for (piggy_backs_t::iterator iter = m_kPiggybacks.begin();iter != m_kPiggybacks.end(); ++iter) delete *iter;
	m_kPiggybacks.clear();

	m_kAckOrders.clear();
}

void geco_bundle_t::clear(bool do_not_dispose)
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
	m_iNumReliableMessages = 0;
	m_iNumMessagesTotalBytes_ = 0;

	if (m_spFirstPacket == NULL)
	{
		//m_spFirstPacket = new FvNetPacket();
		//this->StartPacket(m_spFirstPacket.GetObject());
	}
}

/// returns true if the bundle is empty of messages.
inline bool geco_bundle_t::empty() const
{
	return (m_iNumMessages == 0);
}

void geco_bundle_t::cancel_requests()
{
	reply_orders_t::iterator iter = m_kReplyOrders.begin();
	while (iter != m_kReplyOrders.end())
	{
		iter->response_exception_handler_("requests are cancelled for eneral network reason !", iter->arg);
		++iter;
	}
	m_kReplyOrders.clear();
}

