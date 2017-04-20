/*
 * test-endpoint.cc
 *
 *  Created on: 20Apr.,2017
 *      Author: jackiez
 */

#include "gtest/gtest.h"
#include "network/end-point.h"

TEST(endpoint, test_all_methods)
{
    GecoNetEndpoint ep;
    ep.Socket(AF_INET, SOCK_DGRAM);
    GecoNetAddress addr;
    ep.GetLocalAddress(&addr);
    network_logger()->debug("{}",addr.su.sin.sin_addr.s_addr);
}







