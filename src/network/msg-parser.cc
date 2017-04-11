/*
 * networkstats.cpp
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */

#include "msg-parser.h"

class change_health_handler_t : public GecoNetInputMessageHandler
{
	int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody,
		void* data /* = NULL */)
	{
		msgbody >> CELLAPP::change_healthArgs;
		network_logger()->debug("cell received {} (param status {}) from client ...", ie.c_str(), CELLAPP::change_healthArgs.health);

		CLIENTAPP::change_health_cbArgs.status = "OK";
		msgbody << CLIENTAPP::change_health_cbArgs;
		// sendmsg(encoder);
		network_logger()->debug("cell calls client::change_health_cb ...");

		delete this;
		return 0;
	}
};
class change_health_cb_handler_t : public GecoNetInputMessageHandler
{
	int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody,
		void* data /* = NULL */)
	{
		msgbody >> CLIENTAPP::change_health_cbArgs;
		network_logger()->debug("client received {} (param status {}) from cell ...", ie.c_str(), CLIENTAPP::change_health_cbArgs.status.c_str());
		network_logger()->debug("done...");
		delete this;
		return 0;
	}
};

change_health_handler_t* change_health_handler = new change_health_handler_t;
change_health_cb_handler_t* change_health_cb_handler = new change_health_cb_handler_t;

#define DEFINE_SERVER_HERE
#include "msg-parser.h"

eastl::vector<GecoNetInterfaceElementWithStats> /*bundle::*/table_;

struct MyMsgFilter : public GecoMessageFilter
{
	bool filterMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody)
	{
		bool filter = false;
		if (filter)
			network_logger()->debug("cell filtered out msg {} ...", ie.c_str());
		return filter;
	}
};

// As msg is the smallest logic unit,
// you must call syscall send() for each of msg with stream id, reliable flag
// geco_bit_stream_t will help you handle the case where a single msg that is exceeding default 1MB
geco_bit_stream_t send_buf_(1024 * 1024 * 1024); // 1MB

/**
 *	 This method is called just before a message is dispatched to its handler.
 */
void /*ProcessSocketStatsHelper::*/startMessageHandling(network_recv_stats_t& recv_stats, int messageLength)
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
void /*ProcessSocketStatsHelper::*/stopMessageHandling(network_recv_stats_t& recv_stats)
{
	recv_stats.mercuryTimer_.stop();
}

void msg_call()
{
	geco_bit_stream_t encoder;
	uchar msgid;

	network_recv_stats_t recv_stats_;

	MyMsgFilter msg_filter_; /*bundle::*/

	CELLAPP::gMinder.name_;
	CLIENTAPP::gMinder.name_;
	// 1.caller inits a msg callee
	CELLAPP::change_healthArgs.health = 12;
	encoder << CELLAPP::change_healthArgs;
	network_logger()->debug("client calls cell::change_health ...");
	// assume caller sendmsg(caller_encoder);

	const GecoNetAddress from;
	void* data = 0;
	// assume callee receive request
	encoder.Read(msgid);
	// find ie vased on id here we just assign it as we are testing
	GecoNetInterfaceElementWithStats& ie = CELLAPP::change_health;
	if (g_enable_stats)
		startMessageHandling(recv_stats_, encoder.get_bytes_length() + 1);
	bool passed = msg_filter_.filterMessage(from, ie, encoder);
	if (!passed)
	{
		if (g_enable_stats)
			ie.startProfile();
		ie.GetHandler()->HandleMessage(from, ie, encoder, data);
		if (g_enable_stats)
			ie.stopProfile(encoder.get_bytes_length());
	}
	else
	{
		++recv_stats_.numFilteredBundlesReceived_;
	}
	if (g_enable_stats)
		stopMessageHandling(recv_stats_);
	if (!passed)
	{
		// assume caller receive reply
		encoder.Read(msgid);
		// find ie vased on id here we just assign it as we are testing
		ie = CLIENTAPP::change_health_cb;
		CLIENTAPP::change_health_cb.GetHandler()->HandleMessage(from, ie, encoder, data);
	}
}

/// this function is used to parse a single msg (datagram)
/// @param[in] msg datagram when calling syscall receive(), it returns a complete msg
/// @param[in] recvlen the msg length travelling on internet. when compressed by peer,
/// it will be lwss than the actual msg length
