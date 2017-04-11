/*
 * networkstats.cpp
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */


#include "msg-parser.h"

class change_health_handler_t : public GecoNetInputMessageHandler
{
	int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody, void* data /* = NULL */)
	{
		msgbody >> CELLAPP::change_healthArgs;
		//g_network_logger->debug("cell received request msg {} {} (param health {}) from client ...", ie.c_str(), ie.GetID(), CELLAPP::change_healthArgs.health);
		printf("cell received {%s} (param status {%d}) from client ...\n", ie.c_str(), CELLAPP::change_healthArgs.health);

		CLIENTAPP::change_health_cbArgs.status = "OK";
		msgbody << CLIENTAPP::change_health_cbArgs;
		// sendmsg(encoder);
		printf("cell calls change_health_cb() on client ...\n");

		delete this;
		return 0;
	}
};
class change_health_cb_handler_t: public GecoNetInputMessageHandler
{
	int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody, void* data /* = NULL */)
	{
		msgbody >> CLIENTAPP::change_health_cbArgs;
		//g_network_logger->debug("client received response msg {} {} (param health {}) from cell ...", ie.c_str(), ie.GetID(), CLIENTAPP::change_health_cbArgs.status);
		printf("client received %s (param status {%s}) from cell ...\n", ie.c_str(), CLIENTAPP::change_health_cbArgs.status.c_str());
		//g_network_logger->debug("done...");

		printf("done...\n");
		delete this;
		return 0;
	}
};

change_health_handler_t* change_health_handler = new change_health_handler_t;
change_health_cb_handler_t* change_health_cb_handler = new change_health_cb_handler_t;

#define DEFINE_SERVER_HERE
#include "msg-parser.h"

void msg_call()
{
	geco_bit_stream_t encoder;
	uchar msgid;
	CELLAPP::gMinder.name_;
	CLIENTAPP::gMinder.name_;
	// 1.caller inits a msg callee
	CELLAPP::change_healthArgs.health = 12;
	encoder << CELLAPP::change_healthArgs;
	//g_network_logger->debug("client calls change_health() on cell ...");
	printf("client calls change_health() on cell ...\n");
	// assume caller sendmsg(caller_encoder);

	const GecoNetAddress from;
	void* data = 0;

	// assume callee receive request
	encoder.ReadMini(msgid);
	// find ie vased on id here we just assign it as we are testing
	GecoNetInterfaceElement& ie = CELLAPP::change_health;
	ie.GetHandler()->HandleMessage(from, ie, encoder, data);

	// assume caller receive reply
	encoder.ReadMini(msgid);
	// find ie vased on id here we just assign it as we are testing
	ie = CLIENTAPP::change_health_cb;
	CLIENTAPP::change_health_cb.GetHandler()->HandleMessage(from, ie, encoder, data);
}

eastl::vector<GecoNetInterfaceElementWithStats> /*bundle::*/ table_;

network_recv_stats_t recv_stats_;
struct MyMsgFilter : public GecoMessageFilter
{
	bool filterMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody)
	{
		//g_network_logger->debug("MyMsgFilter::filterMessage()::msg {} is filtered...", ie.c_str());
		return true;
	}
};
GecoMessageFilter /*bundle::*/ msg_filter_;

// As msg is the smallest logic unit,
// you must call syscall send() for each of msg with stream id, reliable flag
// geco_bit_stream_t will help you handle the case where a single msg that is exceeding default 1MB
geco_bit_stream_t send_buf_(1024 * 1024 * 1024); // 1MB

/**
*	 This method is called just before a message is dispatched to its handler.
*/
void /*ProcessSocketStatsHelper::*/ startMessageHandling(network_recv_stats_t& recv_stats, int messageLength)
{
	++recv_stats.numMessagesReceivedPerSecond_;
	// Do not account for it in the overhead anymore.
	recv_stats.numOverheadBytesReceived_ -= messageLength;
	recv_stats.mercuryTimer_.start();
}
/**
*	 This method is called just after a message has been processed by its
*	 handler.
*/
void /*ProcessSocketStatsHelper::*/ stopMessageHandling(network_recv_stats_t& recv_stats)
{
	recv_stats.mercuryTimer_.stop();
}

/// this function is used to parse a single msg (datagram)
/// @param[in] msg datagram when calling syscall receive(), it returns a complete msg
/// @param[in] recvlen the msg length travelling on internet. when compressed by peer,
/// it will be lwss than the actual msg length
void parse_msg(uchar* msg, uint recvlen, GecoNetAddress& from, network_recv_stats_t& recv_stats)
{
	// msg = id+flag+payload
	uchar msgid;
	uchar msgflag;

	geco_bit_stream_t msg_reader(msg, recvlen, false);
	msg_reader.ReadMini(msgid);
	msg_reader.ReadMini(msgflag);

	GecoNetInterfaceElementWithStats& ie = table_[msgid];

	if (g_enable_stats) startMessageHandling(recv_stats, recvlen);
	bool passed = msg_filter_.filterMessage(from, ie, msg_reader);
	if (!passed)
	{
		if (g_enable_stats) ie.startProfile();
		uint actual_msg_body_len = ie.GetHandler()->HandleMessage(from, ie, msg_reader, 0);
		if (g_enable_stats) ie.stopProfile(actual_msg_body_len);
	}
	else
	{
		++recv_stats.numFilteredBundlesReceived_;
	}
	if (g_enable_stats) stopMessageHandling(recv_stats);
}
