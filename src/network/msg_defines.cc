#include "msg_defines.h"

DECLARE_DEBUG_COMPONENT2("Network", 0);


geco_engine_reason packet_recv_t::process_packet()
{
	geco_engine_reason ret = geco_engine_reason::SUCCESS;
	curr_packet_->m_uiIdentifier;
}

geco_bundle_t::geco_bundle_t(uchar spareSize = 0, geco_channel_t* pChannel = NULL) :
	m_spFirstPacket(NULL),
	m_pkCurrentPacket(NULL),
	m_bFinalised(false),
	m_uiExtraSize(spareSize),
	m_pkChannel(pChannel),
	m_sb_free_space_(sizeof(m_ucSendBuffer)),
	m_curr_stream_id(-1)
{
	this->clear(true/*do_not_dispose*/);
}

geco_bundle_t::geco_bundle_t(uchar* packetChain) :
	m_spFirstPacket(packetChain),
	m_pkCurrentPacket(packetChain),
	m_bFinalised(true),
	m_uiExtraSize(0),
	m_pkChannel(NULL),
	m_sb_free_space_(sizeof(m_ucSendBuffer)),
	m_curr_stream_id(-1)
{
	this->clear( /* firstTime: */ true);
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
	m_iNumMessagesTotalBytes = 0;

	if (m_spFirstPacket == NULL)
	{
		//m_spFirstPacket = new FvNetPacket();
		//this->StartPacket(m_spFirstPacket.GetObject());
	}
}

inline bool geco_bundle_t::empty() const{return (m_iNumMessages == 0);}
inline int geco_bundle_t::size() const { return m_iNumMessagesTotalBytes; }
inline bool geco_bundle_t::is_external_channel() const {return m_pkChannel && m_pkChannel->m_uiType == geco_channel_t::WAN;}

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

char* geco_bundle_t::new_message( int extra )
{

}

void geco_bundle_t::start_message_without_response( const interface_element_t& ie,int stream_id)
{
	if(m_pkChannel == NULL) return;
	this->end_message();
	m_pkCurIE = &ie;
	if(stream_id < 0)
	{
		m_curr_stream_id++;
		m_curr_stream_id &= m_pkChannel->m_ostreams_avg_size;
	}
}

void geco_bundle_t::end_message()
{
	// nothing to do if no message yet
	if (m_puiMsgBeg == NULL) return;
	// fill in msg hdr including msgid, msglen and msgflags
	m_pkCurIE->compress_length(msgBeg_,msgLen_,*this, msgIsRequest_);
	msgBeg_ = NULL;
	msgIsRequest_ = false;
}

