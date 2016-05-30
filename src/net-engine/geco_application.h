/*!
 * \file ServerApplication.h
 * \author mengdi
 * \date Oct 18, 2015
 *\ Remember we never push null pointer to any kind of queue including lockfree queue
 *\ and normal queue so that we will never pop out a null pointer from any kind of queue

 1. Client pre - generats challenge then send connection request to server.
 + Header(1)   ID_OPEN_CONNECTION_REQUEST_1
 + OfflineMesageID(16)
 + ProtocolNumber(1)
 + Pad(toMTU)

 2. Server generates cookie and then send it to client to answer.
 + Header(1)   ID_OPEN_CONNECTION_REPLY_1
 + OfflineMesageID(16)
 + Server GUID(8)
 + HasCookieOn(1)
 + Cookie(4, if HasCookieOn) protect syn - floods - attack with faked ip adress
 + Server Public Key(if do security is true)
 + MTU(2)

 3. Client compare the received ServerPublicKey
 with locally - stored pre - given - one, if not equal,
 connection would fail. if equal, client send challenge
 for server to answer and reply the cookie.
 + Header(1)   ID_OPEN_CONNECTION_REQUEST_2
 + OfflineMesageID(16)
 + Cookie(4, if HasCookieOn is true on the server)
 + ClientSupportsSecurity(1 bit)
 + ClientHandshakeChallenge(if has security on both server and client)
 + RemoteBindingAddress(6)
 + MTU(2)
 + ClientGUID(8)

 4. server verify the blelow things :
 if cookie is incorrect, stop process.
 if client does not supports security connection,
 but this client secure is required by this server, stop process.
 if this client connects within 100ms since last connect,
 we regard it as connecion flooding and send msg to notify client.
 if process challenge incorrect, stop process.otherwise, write answer to client
 + Header(1)   ID_OPEN_CONNECTION_REPLY_2
 + OfflineMesageID(16)
 + ServerGUID(8)
 + ServerAddress(? )
 + MTU(2)
 + ClientSecureRequiredbyServer(1bit)
 + Answer(128)

 5. Client processes answer from server
 if incorrect, stop process
 + Header(1)   ID_OPEN_CONNECTION_REPLY_2
 + ClientGUID(8)
 + TimeMS(4)
 + proof(32)
 */

#ifndef __INCLUDE_SERVER_APPLICATION_H
#define __INCLUDE_SERVER_APPLICATION_H

#include "geco-export.h"
#include "transport_layer_t.h"
#include "geco-bit-stream.h"
#include "JackieSimpleMutex.h"
//#include "JackieString.h"
#include "JACKIE_Thread.h"
//#include "RakNetSmartPtr.h"
#include "JackieWaitEvent.h"
#include "geco-features.h"
#include "JACKIE_Atomic.h"
#include "JackieArraryQueue.h"
#include "JackieArrayList.h"
#include "JackieSPSCQueue.h"
#include "JackieMemoryPool.h"
#include "geco-random-seed-creator.h"
#include "network_socket_t.h"
#if ENABLE_SECURE_HAND_SHAKE == 1
#include "geco-secure-hand-shake.h"
#endif

using namespace geco::net;
using namespace geco::ultils;

GECO_NET_BEGIN_NSPACE

JACKIE_THREAD_DECLARATION(RunNetworkUpdateCycleLoop);
JACKIE_THREAD_DECLARATION(RunRecvCycleLoop);
JACKIE_THREAD_DECLARATION(UDTConnect);

struct network_plugin_t;
class GECO_EXPORT network_application_t  //: public IServerApplication
{
    private:
#if ENABLE_SECURE_HAND_SHAKE == 1
    // Encryption and security
    bool secureIncomingConnectionEnabled, _require_client_public_key;
    //char server_public_key_[cat::EasyHandshake::PUBLIC_KEY_BYTES];
    cat::TunnelPublicKey server_public_key_;
    cat::ServerEasyHandshake serverHandShaker;
    cat::CookieJar serverCookie;
    bool InitializeClientSecurity(connection_request_t *rcs, const char *public_key);
#endif

    RandomSeedCreator rnr;
    geco_bit_stream_t sendBitStream;

    guid_t myGuid;
    network_address_t localIPAddrs[MAX_COUNT_LOCAL_IP_ADDR];
    network_address_t firstExternalID;


    /// Store the maximum number of peers allowed to connect
    uint maxConnections;
    ///Store the maximum incoming connection allowed 
    uint maxIncomingConnections;


    char incomingPassword[256];
    unsigned char incomingPasswordLength;


    /// This is an array of pointers to RemoteEndPoint
    /// This allows us to preallocate the list when starting,
    /// so we don't have to allocate or delete at runtime.
    /// Another benefit is that is lets us add and remove active
    /// players simply by setting systemAddress
    /// and moving elements in the list by copying pointers variables
    /// without affecting running threads, even if they are in the reliability layer
    remote_system_t* remoteSystemList;

    /// activeSystemList holds a list of pointers and is preallocated to be the same size as 
    /// remoteSystemList. It is updated only by the network thread, but read by both threads
    /// When the isActive member of RemoteEndPoint is set to true or false, that system is 
    /// added to this list of pointers. Threadsafe because RemoteEndPoint is preallocated, 
    /// and the list is only added to, not removed from
    remote_system_t** activeSystemList;
    uint activeSystemListSize;

    /// Use a hash, with binaryAddress plus port mod length as the index
    JackieRemoteIndex **remoteSystemLookup;

    public:
    bool(*recvHandler)(recv_params_t*);
    void(*userUpdateThreadPtr)(network_application_t *, void *);
    void *userUpdateThreadData;
    bool(*incomeDatagramEventHandler)(recv_params_t *);

    public:
    /// temporary variables used in IsOfflineRecvParams() in this class
    /// to improve efficiency and good structure because it is loop
    msg_id_t msgid;
    network_packet_t* packet;
    unsigned int index;
    bool isUnconnectedRecvPrrams;

    bool updateCycleIsRunning;
    volatile bool endThreads; ///Set this to true to terminate threads execution 
    volatile bool isNetworkUpdateThreadActive; ///true if the send thread is active. 
    atomic_long_t isRecvPollingThreadActive; ///true if the recv thread is active. 
    JackieWaitEvent quitAndDataEvents;
    /// default sleep 10 ms to wit more incoming data
    uint userThreadSleepTime;

    int defaultMTUSize;
    bool trackFrequencyTable;
    uint bytesSentPerSecond;
    uint  bytesReceivedPerSecond;
    /// Do we occasionally ping the other systems?
    bool occasionalPing;
    bool allowInternalRouting;
    /// How long it has been since things were updated by a call
    /// to receiveUpdate thread uses this to determine how long to sleep for
    /// uint32_t lastUserUpdateCycle;
    /// True to allow connection accepted packets from anyone. 
    /// False to only allow these packets from servers we requested a connection to.
    bool allowConnectionResponseIPMigration;
    int splitMessageProgressInterval;
    TimeMS unreliableTimeout;
    TimeMS defaultTimeoutTime;
    uint maxOutgoingBPS;
    bool limitConnFrequencyOfSameClient;

    // Nobody would use the internet simulator in a final build.
#ifdef _DEBUG
    double _packetloss;
    unsigned short _minExtraPing;
    unsigned short _extraPingVariance;
#endif


    /// This is used to return a number to the user 
    /// when they call Send identifying the message
    /// This number will be returned back with ID_SND_RECEIPT_ACKED
    /// or ID_SND_RECEIPT_LOSS and is only returned with the reliability 
    /// types that DOES NOT contain 'NOT' in the name
    JackieSimpleMutex sendReceiptSerialMutex;
    uint sendReceiptSerial;


    /// in Multi-threads app, used only by send thread to alloc packet
    JackieMemoryPool<network_packet_t> packetPool;
    /// used only by ? thread to alloc RemoteEndPointIndex
    JackieMemoryPool<JackieRemoteIndex> remoteSystemIndexPool;
    /// in single thread app, default JISRecvParams pool is JISRecvParamsPool
    /// in Multi-threads app, used only by recv thread to alloc and dealloc JISRecvParams
    /// via anpothe
    JackieMemoryPool<recv_params_t>* JISRecvParamsPool;
    //MemoryPool<JISRecvParams, 512, 8> JISRecvParamsPool;
    JackieMemoryPool<cmd_t> commandPool;


    struct PacketFollowedByData { network_packet_t p; unsigned char data[1]; };
    //struct SocketQueryOutput{	RingBufferQueue<JACKIE_INet_Socket*> sockets;};
    JackieMemoryPool <JackieArraryQueue<network_socket_t*>, 8, 4> socketQueryOutput;


#if USE_SINGLE_THREAD == 0
    // it works in this way:
    // recv thread uses pool to allocate JISRecvParams* ptr and push it to  
    // bufferedAllocatedRecvParamQueue while send thread pops ptr out from 
    // bufferedAllocatedRecvParamQueue and use it do something, when finished, 
    // send thread . then push this ptr into bufferedDeallocatedRecvParamQueue
    // then recv thread will pop this ptr out from bufferedDeallocatedRecvParamQueue
    // and use this ptr to deallocate. In this way, we do not need locks the pool but using 
    // lockfree queue

    /// shared between recv and send thread to store allocated JISRecvParams PTR
    JackieSPSCQueue<recv_params_t*>* allocRecvParamQ;
    /// shared by recv and send thread to store JISRecvParams PTR that is being dellacated
    JackieSPSCQueue<recv_params_t*>* deAllocRecvParamQ;

    JackieSPSCQueue<cmd_t*> allocCommandQ;
    JackieSPSCQueue<cmd_t*> deAllocCommandQ;

    /// shared by recv thread and send thread
    JackieSPSCQueue<network_packet_t*> allocPacketQ;
    /// shared by recv thread and send thread
    JackieSPSCQueue<network_packet_t*> deAllocPacketQ;
#else

    /// shared between recv and send thread
    JackieArraryQueue<recv_params_t*>* allocRecvParamQ;
    /// shared by recv and send thread to  store JISRecvParams PTR that is being dellacated
    JackieArraryQueue<recv_params_t*>* deAllocRecvParamQ;

    JackieArraryQueue<cmd_t*> allocCommandQ;
    JackieArraryQueue<cmd_t*> deAllocCommandQ;

    /// shared by recv thread and send thread
    JackieArraryQueue<network_packet_t*> allocPacketQ;
    /// shared by recv thread and send thread
    JackieArraryQueue<network_packet_t*> deAllocPacketQ;

#endif


    JackieArraryQueue<network_address_t, 8> connReqCancelQ;
    JackieSimpleMutex connReqCancelQLock;
    JackieArraryQueue<connection_request_t*, 8> connReqQ;
    JackieSimpleMutex connReqQLock;

    struct Banned
    {
        char IP[65];
        TimeMS firstTimeBanned;
        TimeMS timeout; /*0 for none*/
        ushort bannedTImes;
    };
    public:
    JackieArrayList<Banned*> banList;

    private:
    // Threadsafe, and not thread safe
    JackieArrayList<network_plugin_t*> pluginListTS;
    JackieArrayList<network_plugin_t*> pluginListNTS;

    //public:
    /// only user thread pushtail into the queue, 
    /// other threads only read it so no need lock
    JackieArrayList<network_socket_t*, 8 > bindedSockets;

    private:
    void ClearAllCommandQs(void);
    void ClearSocketQueryOutputs(void);
    void ClearAllRecvParamsQs(void);

    void ProcessOneRecvParam(recv_params_t* recvParams);
    void IsOfflineRecvParams(recv_params_t* recvParams,
        bool* isOfflinerecvParams);
    void ProcessBufferedCommand(recv_params_t* recvParams, geco_bit_stream_t &updateBitStream);
    void ProcessConnectionRequestCancelQ(void); /// @Done
    void ProcessAllocJISRecvParamsQ(void);
    void ProcessAllocCommandQ(TimeUS& timeUS, TimeMS& timeMS);
    void ProcessConnectionRequestQ(TimeUS& timeUS, TimeMS& timeMS);	/// @Done
    void AdjustTimestamp(network_packet_t*& incomePacket) const;

    /// In multi-threads app and single- thread app,these 3 functions
    /// are called only  by recv thread. the recvStruct will be obtained from 
    /// bufferedDeallocatedRecvParamQueue, so it is thread safe
    /// It is Caller's responsibility to make sure s != 0
    void ReclaimOneJISRecvParams(recv_params_t *s, uint index);
    void ReclaimAllJISRecvParams(uint deAlloclJISRecvParamsQIndex);
    recv_params_t * AllocJISRecvParams(uint deAlloclJISRecvParamsQIndex);

    /// send thread will push trail this packet to buffered alloc queue in multi-threads env
    /// for the furture use of recv thread by popout
    network_packet_t* AllocPacket(uint dataSize);
    network_packet_t* AllocPacket(uint dataSize, uchar *data);
    /// send thread will take charge of dealloc packet in multi-threads env
    void ReclaimAllPackets(void);

    /// recv thread will reclaim all commands  into command pool in multi-threads env
    void ReclaimAllCommands();

    public:
    void RunNetworkUpdateCycleOnce(void);
    void RunRecvCycleOnce(uint in = 0);
    network_packet_t* RunGetPacketCycleOnce(void);

    /// function  CreateRecvPollingThread 
    /// Access  private  
    /// Param [in] [int threadPriority]
    /// Returns int
    /// Remarks this function is not used because because , a thread 
    /// that calls the startup() func plays recv thread
    /// author mengdi[Jackie]
    int CreateRecvPollingThread(int threadPriority, uint index);
    int CreateNetworkUpdateThread(int threadPriority);

    void PacketGoThroughPluginCBs(network_packet_t*& incomePacket);
    void PacketGoThroughPlugins(network_packet_t*& incomePacket);
    void UpdatePlugins(void);

    public:
    //STATIC_FACTORY_DECLARATIONS(JackieApplication);
    static network_application_t* get_instance(void);
    static void reclaim_instance(network_application_t *i);
    network_application_t();
    virtual ~network_application_t();

    void InitIPAddress(void);
    void DeallocBindedSockets(void);
    void ResetSendReceipt(void);
    network_packet_t* fetch_packet(void);

    virtual startup_result_t startup(socket_binding_params_t *socketDescriptors,
        uint maxConnections = 8, uint socketDescriptorCount = 1,
        int threadPriority = -99999);
    void End(uint blockDuration, unsigned char orderingChannel = 0,
        packet_send_priority_t disconnectionNotificationPriority = BUFFERED_THIRDLY_SEND);

    /// public: Generate and store a unique GUID
    void GenerateGUID(void) { myGuid.g = CreateUniqueRandness(); }
    /// Mac address is a poor solution because 
    /// you can't have multiple connections from the same system
    ulonglong CreateUniqueRandness(void);
    uint GetSystemIndexFromGuid(const guid_t& input) const;
    const guid_t& GetGuidFromSystemAddress(const network_address_t input) const;

    uint MaxConnections() const { return maxConnections; }
    /// return how many client initiats a connection to us
    uint GetIncomingConnectionsCount(void) const;
    bool CanAcceptIncomingConnection(void) const
    {
        return GetIncomingConnectionsCount() < maxConnections;
    }
    uint set_sleep_time() const { return userThreadSleepTime; }
    void set_sleep_time(uint val) { userThreadSleepTime = val; }
    /// to check if this is loop back address of local host
    bool IsLoopbackAddress(const guid_address_wrapper_t &systemIdentifier,
        bool matchPort) const;

    /// @Function CancelConnectionRequest 
    /// @Brief  
    /// Cancel a pending connection attempt
    //  If we are already connected, the connection stays open
    /// @Access  public  
    /// @Param [in] [const JackieAddress & target] 
    ///  Which remote server the connection being cancelled to
    /// @Author mengdi[Jackie]
    void CancelConnectionRequest(const network_address_t& target);

    bool GenerateConnectionRequestChallenge(connection_request_t *connectionRequest, key_pair_t *jackiePublicKey);

    /// Returns the number of IP addresses we have
    unsigned int GetLocalIPAddrCount(void)
    {
        if (!active())
        {
            InitIPAddress();
        }
        int i = 0;
        while (localIPAddrs[i] != JACKIE_NULL_ADDRESS)
            i++;
        return i;
    }

    // Returns an IP address at index 0 to GetNumberOfAddresses-1
    // \param[in] index index into the list of IP addresses
    // \return The local IP address at this index
    const char* GetLocalIPAddr(unsigned int index)
    {
        if (!active())
        {
            InitIPAddress();
        }
        static char str[65];
        localIPAddrs[index].ToString(false, str);
        return str;
    }

    /// Sets the password incoming connections must match in the call to Connect
    /// (defaults to none) Pass 0 to passwordData to specify no password
    /// passwd: A data block that incoming connections must match.
    /// This can be just a password, or can be a stream of data.
    /// Specify 0 for no password data
    /// passwdLength: The length in bytes of passwordData
    void set_inbound_connection_pwd(const char passwd[255], int passwdLength)
    {
        //if (passwordDataLength > MAX_OFFLINE_DATA_LENGTH)
        //	passwordDataLength=MAX_OFFLINE_DATA_LENGTH;

        if (passwdLength > 255)
            passwdLength = 255;

        if (passwd == 0)
            passwdLength = 0;

        // Not threadsafe but it's not important enough to lock.  
        // Who is going to change the password a lot during runtime?
        // It won't overflow because @incomingPasswordLength is an unsigned char
        if (passwd != 0 && passwdLength > 0)
            strcpy(incomingPassword, passwd);
        incomingPasswordLength = (unsigned char)passwdLength;
    }

    ///  Must be called while offline
    /// If you are allowed to be connected by others,
    /// you must call this or else security will not be enabled for incoming connections.
    /// This feature requires more round trips, bandwidth, and CPU time for the connection
    /// handshake.  x64 builds require under 25% of the CPU time of other builds
    /// Parameters:
    /// publicKey = A pointer to the public key for accepting new connections
    /// privateKey = A pointer to the private key for accepting new connections
    /// If the private keys are 0, then a new key will be generated when this function is called
    /// bRequireClientKey: Should be set to false for most servers.  Allows the server to accept
    /// a public key from connecting clients as a proof of identity but eats twice as much CPU time as a normal connection
    bool enable_secure_inbound_connections(cat::TunnelKeyPair& key_pair, bool requireClientPublicKey);

    /// recv thread will push tail this packet to buffered dealloc queue in multi-threads env
    void reclaim_packet(network_packet_t *packet);
    /// only recv thread will take charge of alloc packet in multi-threads env
    cmd_t* alloc_cmd();
    void run_cmd(cmd_t* cmd) { allocCommandQ.PushTail(cmd); };


    /// @Function Connect  Connect_
    /// @Brief Connect to a remote host called from user thread Connect() 
    /// and connect_() is used internally by network update thread as AMI(asynchrnious Method Invokation)
    /// @Access  public  
    /// @Param [in] [const char * host] Either a dotted IP address or a domain name
    /// @Param [in] [uint16_t port] Which port to connect to on the remote machine.
    /// @Param [in] [const char * passwd]  
    /// 1.Must match the passwd on the server.  
    /// 2.This can be just a password, or can be a stream of data
    /// @Param [in] [uint32_t passwdLength]  The length in bytes
    /// @Param [in] [JACKIE_Public_Key * jackiePublicKey]  
    /// @Param [in] [uint32_t localSocketIndex]  
    /// @Param [in] [uint32_t attemptTimes]  
    /// @Param [in] [uint32_t attemptIntervalMS]  
    /// @Param [in] [TimeMS timeout]  
    /// @Returns [JACKIE_INET::ConnectionAttemptResult]
    /// 1.True on successful initiation. 
    /// 2.False on incorrect parameters, internal error, or too many existing peers
    /// @Remarks None
    /// @Notice
    /// 1.Calling Connect and not calling @SetMaxPassiveConnections() 
    /// acts as a dedicated client.  Calling both acts as a hyrid;
    /// 2.This is a non-blocking connection. So the connection gets successful
    /// when IsConnected() returns true or getting a packet with the type identifier 
    /// ID_CONNECTION_REQUEST_ACCEPTED. 
    /// @Author mengdi[Jackie]
    void Connect_(const char* host,
        ushort port, const char *passwd = 0, uchar passwdLength = 0,
        key_pair_t *jackiePublicKey = 0, uchar localSocketIndex = 0,
        uchar attemptTimes = 6, ushort attemptIntervalMS = 100000,
        TimeMS timeout = 0, uint extraData = 0);
    connection_attempt_result_t Connect(const char* host,
        ushort port, const char *passwd = 0, uchar passwdLength = 0,
        key_pair_t *jackiePublicKey = 0, uchar localSocketIndex = 0,
        uchar attemptTimes = 6, ushort attemptIntervalMS = 100000,
        TimeMS timeout = 0, uint extraData = 0);

    /// function  StopRecvPollingThread 
    /// Access  public  
    /// Param [in] [void]  
    /// Returns void
    /// Remarks: because we use app main thread as recv thread, it 
    /// will never block so no need to use this
    /// author mengdi[Jackie]
    void stop_recv_thread(void);
    void stop_network_update_thread(void);
    bool active(void) const { return endThreads == false; }


    const guid_t& GetMyGuid(void) const { return myGuid; }
    remote_system_t* GetRemoteSystem(const network_address_t& sa,
        bool neededBySendThread, bool onlyWantActiveEndPoint) const;
    remote_system_t* GetRemoteSystem(const network_address_t& sa)
        const;
    remote_system_t* GetRemoteSystem(const guid_address_wrapper_t&
        senderWrapper, bool neededBySendThread,
        bool onlyWantActiveEndPoint) const;
    remote_system_t* GetRemoteSystem(const guid_t& senderGUID,
        bool onlyWantActiveEndPoint) const;
    int GetRemoteSystemIndex(const network_address_t &sa) const;
    void RefRemoteEndPoint(const network_address_t &sa, uint index);
    void DeRefRemoteSystem(const network_address_t &sa);

    /// \brief Given \a systemAddress, returns its index into remoteSystemList.
    /// \details Values range from 0 to the maximum number of players allowed-1.
    /// This includes systems which were formerly connected, but are now not connected.
    /// \param[in] systemAddress The SystemAddress we are referring to
    /// \return The index of this SystemAddress or -1 on system not found.
    int GetRemoteSystemIndexGeneral(const network_address_t& systemAddress,
        bool calledFromNetworkThread = false) const;
    int GetRemoteSystemIndexGeneral(const guid_t& jackieGuid,
        bool calledFromNetworkThread = false) const;

    bool SendRightNow(TimeUS currentTime, bool useCallerAlloc,
        cmd_t* bufferedCommand);


    void CloseConnectionInternally(bool sendDisconnectionNotification,
        bool performImmediate, cmd_t* bufferedCommand);

    /// \brief Attaches a Plugin interface to an get_instance of the base class (RakPeer or PacketizedTCP) to run code automatically on message receipt in the Receive call.
    /// If the plugin returns false from PluginInterface::UsesReliabilityLayer(), which is the case for all plugins except PacketLogger, you can call AttachPlugin() and DetachPlugin() for this plugin while RakPeer is active.
    /// \param[in] messageHandler Pointer to the plugin to attach.
    void set_plugin(network_plugin_t *plugin);
    bool SendImmediate(reliable_send_params_t& sendParams);

    void AddToActiveSystemList(uint index2use);
    bool IsInSecurityExceptionList(network_address_t& jackieAddr);
    void Add2RemoteSystemList(recv_params_t* recvParams, remote_system_t*& free_rs, bool& thisIPFloodsConnRequest, uint mtu, network_address_t& recvivedBoundAddrFromClient, guid_t& guid,
        bool clientSecureRequiredbyServer);
    void AddToSecurityExceptionList(const char *ip);

    /// @brief Bans an IP from connecting. 
    /// Banned IPs persist between connections 
    /// but are not saved on shutdown nor loaded on startup.
    /// @param[in] IP Dotted IP address. 
    /// Can use * as a wildcard, such as 128.0.0.* will ban all IP
    /// addresses starting with 128.0.0
    /// @param[in] milliseconds 
    /// how many ms for a temporary ban.  Use 0 for a permanent ban
    /// @Caution 
    /// BanRemoteSystem() is used by user thread
    /// AddtoBanLst() is used by network thread
    /// Call this method from user user thread, it will post a cmd to cmd q for
    /// network to process in the future. so it is a asynchronous invokation to avoid 
    /// adding locks on @banlist 
    /// @!you can only call this from user thread after Startup() that clear cmd q
    void ban_remote_system(const char IP[32], TimeMS milliseconds = 0);
    bool IsBanned(network_address_t& senderINetAddress);
    private:
    void AddToBanList(const char IP[32], TimeMS milliseconds = 0);
    void OnConnectionFailed(recv_params_t* recvParams, bool* isOfflinerecvParams);
    void OnConnectionRequest1(recv_params_t* recvParams,
        bool* isOfflinerecvParams);
    void OnConnectionReply1(recv_params_t* recvParams,
        bool* isOfflinerecvParams);
    void OnConnectionRequest2(recv_params_t* recvParams,
        bool* isOfflinerecvParams);
    void OnConnectionReply2(recv_params_t* recvParams,
        bool* isOfflinerecvParams);

    public:
    friend JACKIE_THREAD_DECLARATION(RunNetworkUpdateCycleLoop);
    friend JACKIE_THREAD_DECLARATION(RunRecvCycleLoop);
    friend JACKIE_THREAD_DECLARATION(UDTConnect);
};

GECO_NET_END_NSPACE
#endif 

