/*
 * test-endpoint.cc
 *
 *  Created on: 20Apr.,2017
 *      Author: jackiez
 */

#include "gtest/gtest.h"
#include "network/end-point.h"

TEST(network, test_geco_net_addr)
{
    // 1.test ctor
    GecoNetAddress addr(123, "127.0.0.1");
    const char* addrstr = addr.c_str();
    network_logger()->debug("{}", addrstr);
    ASSERT_EQ((size_t )13, strlen(addrstr));
    ASSERT_STREQ("127.0.0.1:123", addrstr);
    // 2.test methods
    const char* ipstr = addr.ip_str();
    network_logger()->debug("{}", ipstr);
    ASSERT_EQ((size_t )9, strlen(ipstr));
    ASSERT_STREQ("127.0.0.1", ipstr);
}

TEST(network, test_geco_endpoint)
{
    GecoNetAddress addr(0, "localhost");
    GecoNetEndpoint ep;
    ep.Socket(AF_INET, SOCK_DGRAM);
    ep.Bind(addr);
    ep.GetLocalAddress(&addr);
    network_logger()->debug("{}", ntohl(addr.su.sin.sin_addr.s_addr));
    ep.GetInterfaceAddress("lo", addr);
    const char* addrstr = addr.c_str();
    ASSERT_STREQ("127.0.0.1:0", addrstr);
    network_logger()->debug("{}", addrstr);

}

