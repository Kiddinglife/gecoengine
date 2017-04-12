/*
 * networkstats.cpp
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */

#include "msg-parser.h"

class change_health_handler_t: public GecoNetInputMessageHandler
{
        int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody,
                void* data /* = NULL */)
        {
            msgbody >> CELLAPP::change_healthArgs;
            network_logger()->debug("cell received {} (param status {}) from client ...", ie.c_str(),
                    CELLAPP::change_healthArgs.health);

            CLIENTAPP::change_health_cbArgs.status = "OK";
            msgbody << CLIENTAPP::change_health_cbArgs;
            // sendmsg(stream);
            network_logger()->debug("cell calls client::change_health_cb ...");

            delete this;
            return 0;
        }
};
class change_health_cb_handler_t: public GecoNetInputMessageHandler
{
        int HandleMessage(const GecoNetAddress & from, GecoNetInterfaceElement& ie, geco_bit_stream_t & msgbody,
                void* data /* = NULL */)
        {
            msgbody >> CLIENTAPP::change_health_cbArgs;
            network_logger()->debug("client received {} (param status {}) from cell ...", ie.c_str(),
                    CLIENTAPP::change_health_cbArgs.status.c_str());
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

struct MyMsgFilter: public GecoMessageFilter
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
    CELLAPP::change_healthArgs.health = 12;
    stream << CELLAPP::change_healthArgs;
    network_logger()->debug("client calls cell::change_health ...");
    // 2. assume caller sendmsg(stream);

    // 3. assume callee receive request
    stream.Read(msgid);
    // 4. find ie vased on id here we just assign it as we are testing
    GecoNetInterfaceElementWithStats& ie = CELLAPP::change_health;
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
        ie = CLIENTAPP::change_health_cb;
        CLIENTAPP::change_health_cb.GetHandler()->HandleMessage(from, ie, stream, data);
    }
}
