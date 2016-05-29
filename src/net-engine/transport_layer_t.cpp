#include "transport_layer_t.h"
#include "geco_application.h"
#include <iostream>

using namespace geco::net;

transport_layer_t::transport_layer_t()
{
}

transport_layer_t::~transport_layer_t()
{
}

void transport_layer_t::ApplyNetworkSimulator(double _packetloss, unsigned short _minExtraPing, unsigned short _extraPingVariance)
{
    std::cout << " JackieReliabler::ApplyNetworkSimulator is not implemented.";
}

bool transport_layer_t::ProcessOneConnectedRecvParams(network_application_t* serverApp, recv_params_t* recvParams, unsigned mtuSize)
{
    std::cout << " JackieReliabler::ProcessOneConnectedRecvParams is not implemented.";
    return true;
}

void transport_layer_t::Reset(bool param1, int MTUSize, bool client_has_security)
{
    std::cout << " JackieReliabler::Reset is not implemented.";
}

void transport_layer_t::SetSplitMessageProgressInterval(int splitMessageProgressInterval)
{
    std::cout << " JackieReliabler::SetSplitMessageProgressInterval is not implemented.";
}

void transport_layer_t::SetUnreliableTimeout(TimeMS unreliableTimeout)
{
    std::cout << " JackieReliabler::SetUnreliableTimeout is not implemented.";
}

void transport_layer_t::SetTimeoutTime(TimeMS defaultTimeoutTime)
{
    std::cout << " JackieReliabler::SetTimeoutTime is not implemented.";
}

bool transport_layer_t::Send(reliable_send_params_t& sendParams)
{
    //remoteSystemList[sendList[sendListIndex]].reliabilityLayer.Send(data, numberOfBitsToSend, priority, reliability, orderingChannel, useData == false, remoteSystemList[sendList[sendListIndex]].MTUSize, currentTime, receipt);
    std::cout <<
        "Not implemented JackieReliabler::Send(ReliableSendParams& sendParams)";
    return true;
}


