/*
 * networkstats.cpp
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */

#include "msg-parser.h"
#include "networkstats.h"

#include "common/ds/eastl/EASTL/vector.h"

eastl::vector<GecoNetInterfaceElementWithStats> /*bundle::*/ table_;

network_recv_stats_t recv_stats_;
struct MyMsgFilter : public GecoMessageFilter
{
	bool filterMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody)
	{
		g_network_logger->debug("MyMsgFilter::filterMessage()::msg {} is filtered...", ie.c_str());
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

