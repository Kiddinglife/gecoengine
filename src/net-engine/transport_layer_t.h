#ifndef __ReliabilityLayer_H
#define __ReliabilityLayer_H

#include "geco-export.h"
#include "geco-namesapces.h"
#include "geco-features.h"
#include "geco-basic-type.h"
#include "geco-time.h"

#if ENABLE_SECURE_HAND_SHAKE==1
#include "geco-secure-hand-shake.h"
#endif

GECO_NET_BEGIN_NSPACE

class remote_system_t;
class recv_params_t;
class network_application_t;
class geco_bit_stream_t;
struct reliable_send_params_t;

class GECO_EXPORT transport_layer_t
{
    private:
    remote_system_t* remoteEndpoint;

#if ENABLE_SECURE_HAND_SHAKE == 1
    public:
    cat::AuthenticatedEncryption* GetAuthenticatedEncryption(void) { return &auth_enc; }
    protected:
    cat::AuthenticatedEncryption auth_enc;
    bool useSecurity;
#endif // ENABLE_SECURE_HAND_SHAKE

    public:
    transport_layer_t();
    ~transport_layer_t();

    void ApplyNetworkSimulator(double _packetloss, unsigned short _minExtraPing, unsigned short _extraPingVariance);

    // Packets are read directly from the socket layer and skip the reliability
    //layer  because unconnected players do not use the reliability layer
    // This function takes packet data after a player has been confirmed as
    //connected.  The game should not use that data directly
    // because some data is used internally, such as packet acknowledgment and
    //split packets
    bool ProcessOneConnectedRecvParams(network_application_t* serverApp,
        recv_params_t* recvParams, unsigned mtuSize);
    void Reset(bool param1, int MTUSize, bool client_has_security);
    void SetSplitMessageProgressInterval(int splitMessageProgressInterval);
    void SetUnreliableTimeout(TimeMS unreliableTimeout);
    void SetTimeoutTime(TimeMS defaultTimeoutTime);
    bool Send(reliable_send_params_t& sendParams);
};

GECO_NET_END_NSPACE
#endif



