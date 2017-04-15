/*
 * test-auth.cc
 *
 *  Created on: 22Jul.,2016
 *      Author: jackiez
 */

#include "gtest/gtest.h"

#include "network/net-types.h"
#include "test-msg-handlers.h"

struct cell
{
	static cell* instance_;
	static cell* instance()
	{
		if (instance_)
			return instance_;
		instance_ = new cell;
	}

	int invoke_entity_mtd(const CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgsType& invoke_entity_mtd_with_cbStructArgs)
	{
		network_logger()->debug("cell::client_invoke_entity_mtd(eid{},mtdid{},cbid{}",
			CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.eid,
			CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.mtdid,
			CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.cbmtdid);

		const int mtdargs_size = 2;
		const int int_type_id = 0;
		int mtdtypes[mtdargs_size] = { 0, 0 };
		void* mtdargvalues[mtdargs_size];
		for (int i = 0; i < mtdargs_size; i++)
		{
			if (mtdtypes[i] == int_type_id)
			{
				uint arg;
				CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.data->Read(arg);
				mtdargvalues[i] = &arg;
			}
			//..... more
		}

		// reuse stream 
		CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.data->reset();

		network_logger()->debug("callback client entity mtd ...");
		CLIENTAPP::client_invoke_entity_mtdStructArgs.eid = CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.eid;
		CLIENTAPP::client_invoke_entity_mtdStructArgs.mtdid = CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.cbmtdid;
		CLIENTAPP::client_invoke_entity_mtdStructArgs.ret = "ok";
		*CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.data << CLIENTAPP::client_invoke_entity_mtdStructArgs;

		return 0;
		// sendmsg(stream);
	}
};
cell* cell::instance_ = 0;

// this is handled msg
struct CellEntityMsgHandler : public GecoNetInputMessageHandler
{
	typedef int (cell::*impl)(const CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgsType&);
	impl mtd_;

	CellEntityMsgHandler(impl mtd = 0) : mtd_(mtd)
	{
	}
	~CellEntityMsgHandler()
	{
	}
	int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody)
	{
		msgbody >> CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs;
		assert(cell::instance() && mtd_);
		(cell::instance()->*mtd_)(CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs);
		return 0;
	}
};

//this is raw msg
class ClientEntityMsgHandler : public GecoNetInputMessageHandler
{
	int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody)
	{
		msgbody >> CLIENTAPP::client_invoke_entity_mtdStructArgs;
		network_logger()->debug("ClientEntityMsgHandle::HandleMessage()::ret {}", CLIENTAPP::client_invoke_entity_mtdStructArgs.ret.c_str());
		network_logger()->debug("done...");
		return 0;
	}
};

//CellEntityMsgHandler* invoke_entity_mtd_cb_handler = new CellEntityMsgHandler(&cell::client_invoke_entity_mtd);
ClientEntityMsgHandler* invoke_entity_mtd_handler = new ClientEntityMsgHandler;

// DEFINE_INTERFACE_HERE will set up handler in network element with NULL so
//#define DEFINE_INTERFACE_HERE
#define DEFINE_SERVER_HERE
#include "test-msg-handlers.h"

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

TEST(geco_engine_network, test_msg_handler_macros)
{
	// sctp receive() returns a complete msg at one time and so msg is the smallest logic unit.
	// call sctp send() to send a complete msg with reliable flg stream id and so on.
	// geco_bit_stream_t will help you handle the case where a single msg that is exceeding default max msg size 1MB
	// client script :: self.health = 12; // generate and send an set_entity_property_value_msg to server 
	// server c++ ::  set_entity_property_value_handler(set_entity_property_value_msg); // decode msg and call python-c-api::set_property_value(playerobj,"pname_health", newval12) 
	// in periodlly update, server will construct unpdate packets and thus the change will be also progatated other entities.
	geco_bit_stream_t stream(1024 * 1024 * 1024); // 1MB this is kept send buffer and wil be release when pragram exits so make it very big 1MB
	network_recv_stats_t recv_stats_;
	MyMsgFilter msg_filter_; /*bundle::*/
	uchar msgid;
	const GecoNetAddress from;

	// 1.caller inits a msg callee
	bool has_cb = true;
	if (has_cb)
	{
		CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.eid = 0;
		CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.mtdid = 0;
		CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs.cbmtdid = 1;
		stream << CELLAPP::cell_invoke_entity_mtd_with_cbidStructArgs;
	}
	uint mtdarg1 = 1;
	uint mtdarg2 = 2;
	stream.Write(mtdarg1);
	stream.Write(mtdarg2);

	network_logger()->debug("client calls cell::client_invoke_entity_mtd ...");
	// 2. assume caller sendmsg(stream);

	// 3. assume callee receive request
	stream.Read(msgid);

	// 4. callee find ie based on id here we just assign it as we are testing
	GecoNetInterfaceElementWithStats& ie = CELLAPP::cell_invoke_entity_mtd_with_cbid;
	if (g_enable_stats)
		startMessageHandling(recv_stats_, stream.get_bytes_length() + 1);
	bool passed = msg_filter_.filterMessage(from, ie, stream);
	if (!passed && ie.GetHandler())
	{
		if (g_enable_stats)
			ie.startProfile();
		ie.GetHandler()->HandleMessage(from, ie, stream);
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
		ie = CLIENTAPP::client_invoke_entity_mtd;
		CLIENTAPP::client_invoke_entity_mtd.GetHandler()->HandleMessage(from, ie, stream);
	}
}

