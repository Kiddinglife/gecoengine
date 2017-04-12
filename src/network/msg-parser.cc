/*
 * networkstats.cpp
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */

#include "msg-parser.h"

class invoke_entity_mtd_handler_with_cbid_t : public GecoNetInputMessageHandler
{
	int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody,
		void* data /* = NULL */)
	{
		msgbody >> CELLAPP::invoke_entity_mtd_with_cbStructArgs;
		network_logger()->debug("cell invoke_entity_mtd_handler_with_cbid_t({},{}) with cbid={}", 
			CELLAPP::invoke_entity_mtd_with_cbStructArgs.eid, CELLAPP::invoke_entity_mtd_with_cbStructArgs.mtdid, CELLAPP::invoke_entity_mtd_with_cbStructArgs.cbmtdid);

		const int mtdargs_size = 2;
		const int int_type_id = 0;
		int mtdtypes[mtdargs_size] = {0,0};
		void* mtdargvalues[mtdargs_size];
		for (int i = 0; i < mtdargs_size; i++)
		{
			if (mtdtypes[i] == int_type_id)
			{
				uint arg;
				msgbody.Read(arg);
				mtdargvalues[i] = &arg;
			}
			//..... more 
		}

		network_logger()->debug(" invokes script method and return value is ok");
		// call script mthod call_script_method(scriptobj, "mtdname", mtdargvalues);

		network_logger()->debug("call back client ...");
		CLIENTAPP::invoke_entity_mtdStructArgs.eid = CELLAPP::invoke_entity_mtd_with_cbStructArgs.eid;
		CLIENTAPP::invoke_entity_mtdStructArgs.mtdid = CELLAPP::invoke_entity_mtd_with_cbStructArgs.cbmtdid;
		msgbody << CLIENTAPP::invoke_entity_mtdStructArgs;
		std::string result = "OK";
		msgbody.Write(result);
		// sendmsg(stream);

		delete this;
		return 0;
	}
};
class invoke_entity_mtd_handler_t : public GecoNetInputMessageHandler
{
	int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody,
		void* data /* = NULL */)
	{
		msgbody >> CLIENTAPP::invoke_entity_mtdStructArgs;

		std::string ret;
		msgbody.ReadMini(ret);
		network_logger()->debug("client invokes script method cb {}", ret.c_str());
		network_logger()->debug("done...");

		delete this;
		return 0;
	}
};

invoke_entity_mtd_handler_with_cbid_t* invoke_entity_mtd_cb_handler = new invoke_entity_mtd_handler_with_cbid_t;
invoke_entity_mtd_handler_t* invoke_entity_mtd_handler = new invoke_entity_mtd_handler_t;

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

/// this function is used to parse a single msg (datagram) and call  handler
/// @param[in] msg datagram when calling syscall receive(), it returns a complete msg
/// @param[in] recvlen the msg length travelling on internet. when compressed by peer,
/// it will be lwss than the actual msg length
void msg_call()
{
	// As msg is the smallest logic unit,
	// you must call syscall send() for each of msg with stream id, reliable flag
	// geco_bit_stream_t will help you handle the case where a single msg that is exceeding default 1MB
	geco_bit_stream_t stream(1024 * 1024 * 1024); // 1MB
	network_recv_stats_t recv_stats_;
	MyMsgFilter msg_filter_; /*bundle::*/
	uchar msgid;
	const GecoNetAddress from;
	void* data = 0;

	// 1.caller inits a msg callee
	bool has_cb = true;
	if (has_cb)
	{
		CELLAPP::invoke_entity_mtd_with_cbStructArgs.eid = 0;
		CELLAPP::invoke_entity_mtd_with_cbStructArgs.mtdid = 0;
		CELLAPP::invoke_entity_mtd_with_cbStructArgs.cbmtdid = 1;
		stream << CELLAPP::invoke_entity_mtd_with_cbStructArgs;
	}
	uint mtdarg1 = 1;
	uint mtdarg2 = 2;
	stream.Write(mtdarg1);
	stream.Write(mtdarg2);

	network_logger()->debug("client calls cell::invoke_entity_mtd ...");
	// 2. assume caller sendmsg(stream);

	// 3. assume callee receive request
	stream.Read(msgid);

	// 4. callee find ie based on id here we just assign it as we are testing
	GecoNetInterfaceElementWithStats& ie = CELLAPP::invoke_entity_mtd_with_cb;
	if (g_enable_stats)
		startMessageHandling(recv_stats_, stream.get_bytes_length() + 1);
	bool passed = msg_filter_.filterMessage(from, ie, stream);
	if (!passed && ie.GetHandler())
	{
		if (g_enable_stats)
			ie.startProfile();
		ie.GetHandler()->HandleMessage(from, ie, stream, data);
		if (g_enable_stats)
			ie.stopProfile(stream.get_bytes_length());
	}
	else
	{
		++recv_stats_.numFilteredBundlesReceived_;
	}
	if (g_enable_stats)
		stopMessageHandling(recv_stats_);

	// 5. assume caller receive reply
	if (!passed)
	{
		stream.Read(msgid);
		// 6. find ie vased on id here we just assign it as we are testing
		ie = CLIENTAPP::invoke_entity_mtd;
		CLIENTAPP::invoke_entity_mtd.GetHandler()->HandleMessage(from, ie, stream, data);
	}
}
