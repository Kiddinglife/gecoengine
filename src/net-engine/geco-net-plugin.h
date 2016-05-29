#ifndef PluginReceiveResult_H_
#define PluginReceiveResult_H_

#include <iostream>
#include "geco-export.h"
#include "geco-net-type.h"
#include "geco-features.h"
#include "geco_application.h"

GECO_NET_BEGIN_NSPACE
class TCPInterface;
//class JISRecvParams;

/// For each message that arrives on an instance of RakPeer, the plugins get an 
/// opportunity to process them first. This enumeration represents what to do 
/// with the message
enum plugin_action_to_take_t
{
    /// The plugin used this message and it shouldn't be given to the user.
    PROCESSED_BY_ME_THEN_DEALLOC = 0, //RR_STOP_PROCESSING_AND_DEALLOCATE
    /// This message will be processed by other plugins, and at last by the user.
    PROCESSED_BY_OTHERS, 	//	RR_CONTINUE_PROCESSING,
    /// The plugin is going to hold on to this message.  
    /// Do not deallocate it and do not pass it to other plugins either.
    HOLD_ON_BY_ME_NOT_DEALLOC 	//	RR_STOP_PROCESSING
};

/// Reasons why a connection was lost
enum connection_lost_reason_t
{
    /// Called RakPeer::CloseConnection()
    LCR_CLOSED_BY_USER,

    /// Got ID_DISCONNECTION_NOTIFICATION
    LCR_DISCONNECTION_NOTIFICATION,

    /// GOT ID_CONNECTION_LOST
    LCR_CONNECTION_LOST
};

/// Reasons why a connection attempt failed
enum connection_attempt_fail_reason_t
{
    CAFR_CONNECTION_ATTEMPT_FAILED,
    CAFR_ALREADY_CONNECTED,
    CAFR_NO_FREE_INCOMING_CONNECTIONS,
    CAFR_SECURITY_PUBLIC_KEY_MISMATCH,
    CAFR_CONNECTION_BANNED,
    CAFR_INVALID_PASSWORD,
    CAFR_INCOMPATIBLE_PROTOCOL,
    CAFR_IP_RECENTLY_CONNECTED,
    CAFR_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY,
    CAFR_OUR_SYSTEM_REQUIRES_SECURITY,
    CAFR_PUBLIC_KEY_MISMATCH
};

struct GECO_EXPORT network_plugin_t
{
    // Filled automatically in when attached
    network_application_t* serverApplication;

#if JackieNet_SUPPORT_PacketizedTCP==1 && JackieNet_SUPPORT_TCPInterface==1
    TCPInterface *tcpInterface;
#endif

    network_plugin_t();
    virtual ~network_plugin_t() { }


    network_application_t *GetServerApplication(void) const { return serverApplication; }
    guid_t GetMyGUIDUnified(void) const
    {
        if (serverApplication != 0)
            return serverApplication->GetMyGuid();
        return JACKIE_NULL_GUID;
    }

    /// Called when the interface is attached
    virtual void OnAttach(void)
    {
        std::cout << "USER THREAD OnAttach()";
    }

    /// Called when the interface is detached
    virtual void OnDetach(void)
    {
        std::cout << "USER THREAD OnDetach()";
    }


    /// Called when serverApplication is initialized
    virtual void OnStartup()
    {
        std::cout << " OnRakPeerStartup()";
    }
    /// Called when serverApplication is shutdown
    virtual void OnShutdown(void)
    {
        std::cout << "OnRakPeerShutdown()";
    }


    /// Update is called every time a packet is checked for 
    virtual void Update(void) { }

    /// OnReceive is called for every packet.
    /// @param[in] packet the packet that is being returned to the user
    /// @return True to allow the game and other plugins to get this message, 
    /// false to absorb it
    virtual plugin_action_to_take_t OnRecvPacket(network_packet_t *packet)
    {
        return PROCESSED_BY_OTHERS;
    }

    /// Called when a connection is dropped because the user called RakPeer::CloseConnection() for a particular system
    /// @param[in] systemAddress The system whose connection was closed
    /// @param[in] rakNetGuid The guid of the specified system
    /// @param[in] lostConnectionReason How the connection was closed: manually, connection lost, or notification of disconnection
    virtual void OnClosedConnection(const network_address_t &systemAddress,
        guid_t& guid, connection_lost_reason_t lostConnectionReason)
    {
    }

    /// Called when we got a new connection
    /// @param[in] systemAddress Address of the new connection
    /// @param[in] rakNetGuid The guid of the specified system
    /// @param[in] isIncoming If true, this is ID_NEW_INCOMING_CONNECTION, or the equivalent
    virtual void OnNewConnection(const network_address_t &systemAddress,
        guid_t& guid, bool isIncoming)
    {
        std::cout << "NEW CONNECTION FROM " << systemAddress.ToString();
    }

    /// Called when a connection attempt fails
    /// @param[in] packet Packet to be returned to the user
    /// @param[in] failedConnectionReason Why the connection failed
    virtual void OnFailedConnectionAttempt(network_packet_t *packet,
        connection_attempt_fail_reason_t failedConnectionAttemptReason)
    {
        std::cout << "OnFailedConnectionAttempt reason is " << packet->data[0];
    }


    /// Called on a send or receive of a message within the reliability layer
    /// @pre To be called, UsesReliabilityLayer() must return true
    /// @param[in] internalPacket The user message, along with all send data.
    /// @param[in] frameNumber The number of frames sent or received so far for this player depending on @a isSend .  Indicates the frame of this user message.
    /// @param[in] remoteSystemAddress The player we sent or got this packet from
    /// @param[in] time The current time as returned by RakNet::GetTimeMS()
    /// @param[in] isSend Is this callback representing a send event or receive event?
    virtual void OnInternalPacket(internal_packet_t *internalPacket, unsigned frameNumber,
        network_address_t& remoteSystemAddress, TimeMS time, int isSend) { }

    /// Called when we get an ack for a message we reliably sent
    /// @pre To be called, UsesReliabilityLayer() must return true
    /// @param[in] messageNumber The numerical identifier for which message this is
    /// @param[in] remoteSystemAddress The player we sent or got this packet from
    /// @param[in] time The current time as returned by RakNet::GetTimeMS()
    virtual void OnAck(unsigned int messageNumber, network_address_t& remoteSystemAddress, TimeMS time) { }

    /// System called IServerApplication::PushBackPacket()
    /// @param[in] data The data being sent
    /// @param[in] bitsUsed How many bits long @a data is
    /// @param[in] remoteSystemAddress The player we sent or got this packet from
    virtual void OnPushBackPacket(const char *data, const unsigned int bitsUsed,
        network_address_t& remoteSystemAddress) { }

    /// Queried when attached to RakPeer
    /// Return true to call OnDirectSocketSend(), OnDirectSocketReceive(), OnReliabilityLayerNotification(), OnInternalPacket(), and OnAck()
    /// If true, then you cannot call RakPeer::AttachPlugin() or RakPeer::DetachPlugin() for this plugin, while RakPeer is active
    virtual bool UsesReliabilityLayer(void) const { return true; }

    /// Called on a send to the socket, per datagram, that does not go through the reliability layer
    /// @pre To be called, UsesReliabilityLayer() must return true
    /// @param[in] data The data being sent
    /// @param[in] bitsUsed How many bits long @a data is
    /// @param[in] remoteSystemAddress Which system this message is being sent to
    //virtual void OnDirectSocketSend(const char *data, const unsigned int bitsUsed, JackieAddress& remoteSystemAddress) { }
    virtual void OnDirectSocketSend(const send_params_t* param)
    {
        std::cout << "OnDirectSocketSend():: send " << param->bytesWritten
            << " bytes to " << param->receiverINetAddress.ToString()
            << ", msg id = " << (int)param->data[0];
    }

    /// Called on a receive from the socket, per datagram, that does not go through the reliability layer
    /// @pre To be called, UsesReliabilityLayer() must return true
    /// @param[in] data The data being sent
    /// @param[in] bitsUsed How many bits long @a data is
    /// @param[in] remoteSystemAddress Which system this message is being sent to
    //virtual void OnDirectSocketReceive(const char *data, const unsigned int bitsUsed, JackieAddress& remoteSystemAddress) { }
    virtual void OnDirectSocketReceive(const recv_params_t* param)
    {
        std::cout << "OnDirectSocketReceive()::recv from  " << param->senderINetAddress.ToString() << ", bytes "
            << param->bytesRead << ", msg id = " << (int)param->data[0];
    }

    /// Called when the reliability layer rejects a send or receive
    /// @pre To be called, UsesReliabilityLayer() must return true
    /// @param[in] bitsUsed How many bits long @a data is
    /// @param[in] remoteSystemAddress Which system this message is being sent to
    virtual void OnReliabilityLayerNotification(const char *errorMessage, const unsigned int
        bitsUsed, network_address_t& remoteSystemAddress, bool isError) { }

};

GECO_NET_END_NSPACE
#endif
