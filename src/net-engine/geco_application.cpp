#include "geco_application.h"
#include "geco-wsa-singleton.h"
#include "geco-msg-ids.h"
#include "JackieINetVersion.h"
#include "geco-sliding-windows.h"
#include "geco-net-plugin.h"

//#include "JackieString.h"

#if !defined ( __APPLE__ ) && !defined ( __APPLE_CC__ )
#include <stdlib.h> // malloc
#endif

using namespace geco::net;
using namespace geco::ultils;

#define UNCONNETED_RECVPARAMS_HANDLER0 \
if (recvParams->bytesRead >= sizeof(msg_id_t) + sizeof(OFFLINE_MESSAGE_DATA_ID) + guid_t::size())\
{*isOfflinerecvParams = memcmp(recvParams->data + sizeof(msg_id_t),\
OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;}

#define UNCONNETED_RECVPARAMS_HANDLER1 \
if (recvParams->bytesRead >=sizeof(msg_id_t) + sizeof(Time) + sizeof(OFFLINE_MESSAGE_DATA_ID))\
{*isOfflinerecvParams =memcmp(recvParams->data + sizeof(msg_id_t) + \
sizeof(Time), OFFLINE_MESSAGE_DATA_ID,sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;}

#define UNCONNECTED_RECVPARAMS_HANDLER2 \
if (*isOfflinerecvParams) {\
for (index = 0; index < pluginListNTS.Size(); index++)\
pluginListNTS[index]->OnDirectSocketReceive(recvParams);}

////////////////////////////////////////////////// STATICS /////////////////////////////////////
static const uint RemoteEndPointLookupHashMutiple = 8;
static const int mtuSizesCount = 3;
static const int mtuSizes[mtuSizesCount] =
{ MAXIMUM_MTU_SIZE, 1200, 576 };
/// I set this because I limit ID_CONNECTION_REQUEST to 512 bytes, 
/// and the password is appended to that *incomePacket.
static const uint MAX_OFFLINE_DATA_LENGTH = 400;
/// Used to distinguish between offline messages with data, 
/// and messages from the reliability layer. Should be different 
/// than any message that could result from messages from the reliability layer
/// Make sure highest bit is 0, so isValid in DatagramHeaderFormat is false
static const unsigned char OFFLINE_MESSAGE_DATA_ID[16] =
{ 0x00, 0xFF, 0xFF, 0x00, 0xFE, 0xFE, 0xFE, 0xFE, 0xFD, 0xFD, 0xFD, 0xFD, 0x12,
0x34, 0x56, 0x78 };

network_application_t::network_application_t() :
sendBitStream(MAXIMUM_MTU_SIZE
#if ENABLE_SECURE_HAND_SHAKE==1
+ cat::AuthenticatedEncryption::OVERHEAD_BYTES
#endif
)
{
#if ENABLE_SECURE_HAND_SHAKE == 1
    // Encryption and security
    std::cout
        << "Initializing Jackie Application security flags: using_security = false, server_handshake = null, cookie_jar = null";
    secureIncomingConnectionEnabled = false;
#endif

    // Dummy call to PacketLogger to ensure it's included in exported symbols.
    //PacketLogger::BaseIDTOString(0);
    //JackieStringCompressor::AddReference();
    //RakNet::StringTable::AddReference();
    WSAStartupSingleton::AddRef();

    defaultMTUSize = mtuSizes[mtuSizesCount - 1];
    trackFrequencyTable = false;
    maxIncomingConnections = maxConnections = 0;
    bytesSentPerSecond = bytesReceivedPerSecond = 0;

    remoteSystemList = 0;
    remoteSystemLookup = 0;
    activeSystemList = 0;
    activeSystemListSize = 0;

    recvHandler = 0;
    userUpdateThreadPtr = 0;
    userUpdateThreadData = 0;
    incomeDatagramEventHandler = 0;
    endThreads = true;
    userThreadSleepTime = 10; // default sleep 10 ms to wit more incoming data

    allowInternalRouting = false;
    allowConnectionResponseIPMigration = false;
    incomingPasswordLength = 0;
    splitMessageProgressInterval = 0;
    unreliableTimeout = 1000;
    maxOutgoingBPS = 0;

    myGuid = JACKIE_NULL_GUID;
    firstExternalID = JACKIE_NULL_ADDRESS;

    for (uint index = 0; index < MAX_COUNT_LOCAL_IP_ADDR; index++)
    {
        localIPAddrs[index] = JACKIE_NULL_ADDRESS;
    }

#ifdef _DEBUG
    // Wait longer to disconnect in debug so I don't get disconnected while tracing
    defaultTimeoutTime = 30000;
    _packetloss = 0.0;
    _minExtraPing = 0;
    _extraPingVariance = 0;
#else
    defaultTimeoutTime = 10000;
#endif

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
    occasionalPing = true;
#else
    occasionalPing = false;
#endif

    limitConnFrequencyOfSameClient = false;

#if USE_SINGLE_THREAD == 0
    quitAndDataEvents.Init();
#endif

    GenerateGUID();
    ResetSendReceipt();
}
network_application_t::~network_application_t()
{
    OP_DELETE_ARRAY(JISRecvParamsPool, TRACKE_MALLOC);
}

startup_result_t network_application_t::startup(socket_binding_params_t *bindLocalSockets,
    uint maxConn, uint bindLocalSocketsCount,
    int threadPriority /*= -99999*/)
{
    if (active())
        return startup_result_t::ALREADY_STARTED;

    // If getting the guid failed in the constructor, try again
    if (myGuid.g == 0)
    {
        GenerateGUID();
        if (myGuid.g == 0)
            return startup_result_t::COULD_NOT_GENERATE_GUID;
    }

    if (myGuid == JACKIE_NULL_GUID)
    {
        rnr.SeedMT((uint)((myGuid.g >> 32) ^ myGuid.g));
        myGuid.g = rnr.RandomMT();
    }

    if (threadPriority == -99999)
    {
#if  defined(_WIN32)
        threadPriority = 0;
#else
        threadPriority = 1000;
#endif
    }

    InitIPAddress();

    assert(bindLocalSockets && bindLocalSocketsCount >= 1);
    if (bindLocalSockets == 0 || bindLocalSocketsCount < 1)
        return INVALID_JACKIE_LOCAL_SOCKET;

    assert(maxConn > 0);
    if (maxConn <= 0)
        return INVALID_MAX_CONNECTIONS;

    /// startup to bind given sockets
    uint index;
    network_socket_t* sock;
    berkley_socket_binding_params_t berkleyBindParams;
    socket_binding_result_t bindResult;
    DeallocBindedSockets();
    for (index = 0; index < bindLocalSocketsCount; index++)
    {
        do
        {
            sock = network_socket_alloc_t::AllocJIS();
        } while (sock == 0);
        //GECO_ASSERT(bindedSockets.PushTail(sock), true);
        bindedSockets.InsertAtLast(sock);

#if defined(__native_client__)
        NativeClientBindParameters ncbp;
        RNS2_NativeClient * nativeClientSocket = (RNS2_NativeClient*)r2;
        ncbp.eventHandler = this;
        ncbp.forceHostAddress = (char*)bindLocalSockets[index].hostAddress;
        ncbp.is_ipv6 = bindLocalSockets[index].socketFamily == AF_INET6;
        ncbp.nativeClientInstance = bindLocalSockets[index].chromeInstance;
        ncbp.port = bindLocalSockets[index].port;
        nativeClientSocket->Bind(&ncbp, _FILE_AND_LINE_);
#elif defined(WINDOWS_STORE_RT)
        RNS2BindResult br;
        ((RNS2_WindowsStore8*)r2)->SetRecvEventHandler(this);
        br = ((RNS2_WindowsStore8*)r2)->Bind(ref new Platform::String());
        if (br != BR_SUCCESS)
        {
            RakNetSocket2Allocator::DeallocRNS2(r2);
            DerefAllSockets();
            return SOCKET_FAILED_TO_BIND;
        }
#else
        if (sock->IsBerkleySocket())
        {
            berkleyBindParams.port = bindLocalSockets[index].port;
            berkleyBindParams.hostAddress =
                (char*)bindLocalSockets[index].hostAddress;
            berkleyBindParams.addressFamily =
                bindLocalSockets[index].socketFamily;
            berkleyBindParams.type = SOCK_DGRAM;
            berkleyBindParams.protocol =
                bindLocalSockets[index].extraSocketOptions;
            berkleyBindParams.isBroadcast = true;
            berkleyBindParams.setIPHdrIncl = false;
            berkleyBindParams.doNotFragment = false;
            berkleyBindParams.pollingThreadPriority = threadPriority;
            berkleyBindParams.eventHandler = this;
            berkleyBindParams.remotePortJackieNetWasStartedOn_PS3_PS4_PSP2 =
                bindLocalSockets[index].remotePortWasStartedOn_PS3_PSP2;

#if USE_SINGLE_THREAD == 0
            /// multi-threads app can use either non-blobk or blobk socket
            /// false = blobking, true = non-blocking
            berkleyBindParams.isNonBlocking = bindLocalSockets[index].blockingSocket;
#else
            ///  single thread app will always use non-blobking socket
            berkleyBindParams.isNonBlocking = true;
#endif

            bindResult = ((berkley_socket_t*)sock)->Bind(&berkleyBindParams,
                TRACKE_MALLOC);

            if (
#if NET_SUPPORT_IPV6 ==0
                bindLocalSockets[index].socketFamily != AF_INET
                ||
#endif
                bindResult
                == JISBindResult_REQUIRES_NET_SUPPORT_IPV6_DEFINED)
            {
                network_socket_alloc_t::DeallocJIS(sock);
                DeallocBindedSockets();
                std::cout
                    << "Bind Failed (REQUIRES_NET_SUPPORT_IPV6_DEFINED) ! ";
                return SOCKET_FAMILY_NOT_SUPPORTED;
            }

            switch (bindResult)
            {
                case JISBindResult_FAILED_BIND_SOCKET:
                    DeallocBindedSockets();
                    std::cout << "Bind Failed (FAILED_BIND_SOCKET) ! ";
                    return SOCKET_PORT_ALREADY_IN_USE;
                    break;

                case JISBindResult_FAILED_SEND_RECV_TEST:
                    DeallocBindedSockets();
                    std::cout << "Bind Failed (FAILED_SEND_RECV_TEST) ! ";
                    return SOCKET_FAILED_TEST_SEND_RECV;
                    break;

                default:
                    assert(bindResult == JISBindResult_SUCCESS);
                    std::cout << "Bind [" << sock->GetBoundAddress().ToString()
                        << "] Successfully";
                    sock->SetUserConnectionSocketIndex(index);
                    break;
            }
        }
        else
        {
            std::cout << "Bind Failed (@TO-DO UNKNOWN BERKELY SOCKET) ! ";
            assert("@TO-DO UNKNOWN BERKELY SOCKET" && 0);
        }
#endif
    }

    assert(bindedSockets.Size() == bindLocalSocketsCount);

    /// after binding, assign IPAddress port number
#if !defined(__native_client__) && !defined(WINDOWS_STORE_RT)
    if (bindedSockets[0]->IsBerkleySocket())
    {
        for (index = 0; index < MAX_COUNT_LOCAL_IP_ADDR; index++)
        {
            if (localIPAddrs[index] == JACKIE_NULL_ADDRESS)
                break;
            localIPAddrs[index].SetPortHostOrder(
                ((berkley_socket_t*)bindedSockets[0])->GetBoundAddress().GetPortHostOrder());
        }
    }
#endif

    JISRecvParamsPool = OP_NEW_ARRAY<JackieMemoryPool<recv_params_t>>(
        bindedSockets.Size(), TRACKE_MALLOC);
#if USE_SINGLE_THREAD == 0
    deAllocRecvParamQ = OP_NEW_ARRAY < JackieSPSCQueue <
        recv_params_t* >> (bindedSockets.Size(), TRACKE_MALLOC);
    allocRecvParamQ = OP_NEW_ARRAY < JackieSPSCQueue <
        recv_params_t* >> (bindedSockets.Size(), TRACKE_MALLOC);
#else
    deAllocRecvParamQ = OP_NEW_ARRAY<JackieArraryQueue<recv_params_t*>>(
        bindedSockets.Size(), TRACKE_MALLOC);
    allocRecvParamQ = OP_NEW_ARRAY<JackieArraryQueue<recv_params_t*>>(
        bindedSockets.Size(), TRACKE_MALLOC);
#endif

    /// setup connections list
    if (maxConnections == 0)
    {
        // Don't allow more incoming connections than we have peers.
        if (maxIncomingConnections > maxConn)
            maxIncomingConnections = maxConn;
        maxConnections = maxConn;

        remoteSystemList = OP_NEW_ARRAY<remote_system_t>(maxConnections,
            TRACKE_MALLOC);

        // All entries in activeSystemList have valid pointers all the time.
        activeSystemList = OP_NEW_ARRAY<remote_system_t*>(maxConnections,
            TRACKE_MALLOC);

        // decrease the collision chance by increasing the hashtable size
        index = maxConnections * RemoteEndPointLookupHashMutiple;
        remoteSystemLookup = OP_NEW_ARRAY<JackieRemoteIndex*>(index,
            TRACKE_MALLOC);
        memset((void**)remoteSystemLookup, 0,
            index * sizeof(JackieRemoteIndex*));

        for (index = 0; index < maxConnections; index++)
        {
            // remoteSystemList in Single thread
            remoteSystemList[index].isActive = false;
            remoteSystemList[index].systemAddress = JACKIE_NULL_ADDRESS;
            remoteSystemList[index].guid = JACKIE_NULL_GUID;
            remoteSystemList[index].myExternalSystemAddress =
                JACKIE_NULL_ADDRESS;
            remoteSystemList[index].connectMode = remote_system_t::NO_ACTION;
            remoteSystemList[index].MTUSize = defaultMTUSize;
            remoteSystemList[index].remoteSystemIndex = (system_index_t)index;
#ifdef _DEBUG
            remoteSystemList[index].reliabilityLayer.ApplyNetworkSimulator(_packetloss, _minExtraPing, _extraPingVariance);
#endif
            activeSystemList[index] = &remoteSystemList[index];
        }
    }

    /// Setup Plugins
    for (index = 0; index < pluginListTS.Size(); index++)
    {
        pluginListTS[index]->OnStartup();
    }

    for (index = 0; index < pluginListNTS.Size(); index++)
    {
        pluginListNTS[index]->OnStartup();
    }

    ///  setup thread things
    endThreads = true;
    isNetworkUpdateThreadActive = false;
    if (endThreads)
    {
        // no need to clear this
        //ClearAllCommandQs();
        //ClearSocketQueryOutputs();
        //ClearAllRecvParamsQs();

        firstExternalID = JACKIE_NULL_ADDRESS;
        updateCycleIsRunning = false;
        endThreads = false;

        /// Create recv threads
#if !defined(__native_client__) && !defined(WINDOWS_STORE_RT)
#if USE_SINGLE_THREAD == 0
        /// this will create @bindLocalSocketsCount number of of recv threads
        /// That is if you have two NICs, will create two recv threads to handle
        /// each of socket
        for (index = 0; index < bindLocalSocketsCount; index++)
        {
            if (bindedSockets[index]->IsBerkleySocket())
            {
                if (CreateRecvPollingThread(threadPriority, index) != 0)
                {
                    End(0);
                    return FAILED_TO_CREATE_RECV_THREAD;
                }
            }
        }

        /// Wait for the threads to activate. When they are active they will set these variables to true
        while (!isRecvPollingThreadActive.GetValue()) GecoSleep(10);
#else
        /// we handle recv in this thread, that is we only have two threads in the app this recv thread and th other send thread
        isRecvPollingThreadActive.Increment();
#endif
#endif

        /// use another thread to charge of sending
        if (!isNetworkUpdateThreadActive)
        {
#if USE_SINGLE_THREAD == 0
            if (CreateNetworkUpdateThread(threadPriority) != 0)
            {
                End(0);
                std::cout << "ServerApplication::startup() Failed (FAILED_TO_CREATE_SEND_THREAD) ! ";
                return FAILED_TO_CREATE_NETWORK_UPDATE_THREAD;
            }
            /// Wait for the threads to activate. When they are active they will set these variables to true
            while (!isNetworkUpdateThreadActive) GecoSleep(10);
#else
            /// we only have one thread to handle recv and send so just simply set it to true
            isNetworkUpdateThreadActive = true;
#endif
        }
    }

    //#ifdef USE_THREADED_SEND
    //		RakNet::SendToThread::AddRef();
    //#endif

    std::cout << "Startup Application Succeeds....";
    return START_SUCCEED;
}

void network_application_t::End(uint blockDuration, unsigned char orderingChannel,
    packet_send_priority_t disconnectionNotificationPriority)
{
}

inline void network_application_t::ReclaimOneJISRecvParams(recv_params_t *s,
    uint index)
{
    //std::cout << "Network Thread Reclaims One JISRecvParams";
    bool ret = deAllocRecvParamQ[index].PushTail(s);
    assert(ret == true);
}
inline void network_application_t::ReclaimAllJISRecvParams(uint Index)
{
    //std::cout << "Recv thread " << Index << " Reclaim All JISRecvParams";

    recv_params_t* recvParams = 0;
    for (uint index = 0; index < deAllocRecvParamQ[Index].Size(); index++)
    {
        bool ret = deAllocRecvParamQ[Index].PopHead(recvParams);
        assert(ret == true);
        JISRecvParamsPool[Index].Reclaim(recvParams);
    }
}
inline recv_params_t * network_application_t::AllocJISRecvParams(uint Index)
{
    //std::cout << "Recv Thread" << Index << " Alloc An JISRecvParams";
    recv_params_t* ptr = 0;
    do
    {
        ptr = JISRecvParamsPool[Index].Allocate();
    } while (ptr == 0);
    ptr->localBoundSocket = bindedSockets[Index];
    return ptr;
}
void network_application_t::ClearAllRecvParamsQs()
{
    for (uint index = 0; index < bindedSockets.Size(); index++)
    {
        recv_params_t *recvParams = 0;
        for (uint i = 0; i < allocRecvParamQ[index].Size(); i++)
        {
            bool ret = allocRecvParamQ[index].PopHead(recvParams);
            assert(ret == true);
            if (recvParams->data != 0)
                gFreeEx(recvParams->data, TRACKE_MALLOC);
            JISRecvParamsPool[index].Reclaim(recvParams);
        }
        for (uint i = 0; i < deAllocRecvParamQ[index].Size(); i++)
        {
            bool ret = deAllocRecvParamQ[index].PopHead(recvParams);
            assert(ret == true);
            if (recvParams->data != 0)
                gFreeEx(recvParams->data, TRACKE_MALLOC);
            JISRecvParamsPool[index].Reclaim(recvParams);
        }
        allocRecvParamQ[index].Clear();
        deAllocRecvParamQ[index].Clear();
        JISRecvParamsPool[index].Clear();
    }
}

inline void network_application_t::ReclaimAllCommands()
{
    //std::cout << "User Thread Reclaims All Commands";

    cmd_t* bufferedCommand = 0;
    for (uint index = 0; index < deAllocCommandQ.Size(); index++)
    {
        deAllocCommandQ.PopHead(bufferedCommand);
        assert(bufferedCommand != NULL);
        commandPool.Reclaim(bufferedCommand);
    }
}
inline cmd_t* network_application_t::alloc_cmd()
{
    //std::cout << "User Thread Alloc An cmd_t";
    cmd_t* ptr = 0;
    do
    {
        ptr = commandPool.Allocate();
    } while (ptr == 0);
    return ptr;
}
void network_application_t::ClearAllCommandQs(void)
{
    cmd_t *bcs = 0;

    /// first reclaim the elem in
    for (uint i = 0; i < allocCommandQ.Size(); i++)
    {
        allocCommandQ.PopHead(bcs);
        assert(bcs != NULL);
        if (bcs->data != 0)
            gFreeEx(bcs->data, TRACKE_MALLOC);
        commandPool.Reclaim(bcs);
    }
    for (uint i = 0; i < deAllocCommandQ.Size(); i++)
    {
        deAllocCommandQ.PopHead(bcs);
        assert(bcs != NULL);
        if (bcs->data != 0)
            gFreeEx(bcs->data, TRACKE_MALLOC);
        commandPool.Reclaim(bcs);
    }
    deAllocCommandQ.Clear();
    allocCommandQ.Clear();
    commandPool.Clear();
}

void network_application_t::InitIPAddress(void)
{
    assert(localIPAddrs[0] == JACKIE_NULL_ADDRESS);
    network_socket_t::GetMyIP(localIPAddrs);

    // Sort the addresses from lowest to highest
    int startingIdx = 0;
    while (startingIdx < MAX_COUNT_LOCAL_IP_ADDR - 1
        && localIPAddrs[startingIdx] != JACKIE_NULL_ADDRESS)
    {
        int lowestIdx = startingIdx;
        for (int curIdx = startingIdx + 1;
            curIdx < MAX_COUNT_LOCAL_IP_ADDR - 1
            && localIPAddrs[curIdx] != JACKIE_NULL_ADDRESS;
        curIdx++)
        {
            if (localIPAddrs[curIdx] < localIPAddrs[startingIdx])
            {
                lowestIdx = curIdx;
            }
        }
        if (startingIdx != lowestIdx)
        {
            network_address_t temp = localIPAddrs[startingIdx];
            localIPAddrs[startingIdx] = localIPAddrs[lowestIdx];
            localIPAddrs[lowestIdx] = temp;
        }
        ++startingIdx;
    }
}

inline void network_application_t::DeallocBindedSockets(void)
{
    for (uint index = 0; index < bindedSockets.Size(); index++)
    {
        if (bindedSockets[index] != 0)
            network_socket_alloc_t::DeallocJIS(bindedSockets[index]);
    }
}
inline void network_application_t::ClearSocketQueryOutputs(void)
{
    socketQueryOutput.Clear();
}

network_packet_t* network_application_t::AllocPacket(uint dataSize)
{
    //std::cout << "Network Thread Alloc One Packet";
    network_packet_t *p = 0;
    do
    {
        p = packetPool.Allocate();
    } while (p == 0);

    //p = new ( (void*) p ) Packet; we do not need call default ctor
    p->data = (unsigned char*)gMallocEx(dataSize, TRACKE_MALLOC);
    p->length = dataSize;
    p->bitSize = BYTES_TO_BITS(dataSize);
    p->freeInternalData = true;
    p->guid = JACKIE_NULL_GUID;
    p->wasGeneratedLocally = false;

    return p;
}

/// default 	p->freeInternalData = false;
network_packet_t* network_application_t::AllocPacket(uint dataSize,
    unsigned char *data)
{
    //std::cout << "Network Thread Alloc One Packet";
    network_packet_t *p = 0;
    do
    {
        p = packetPool.Allocate();
    } while (p == 0);

    //p = new ( (void*) p ) Packet; no custom ctor so no need to call default ctor
    p->data = (unsigned char*)data;
    p->length = dataSize;
    p->bitSize = BYTES_TO_BITS(dataSize);
    p->freeInternalData = false;
    p->guid = JACKIE_NULL_GUID;
    p->wasGeneratedLocally = false;

    return p;
}
void network_application_t::ReclaimAllPackets()
{
    //std::cout << "Network Thread Reclaims All Packets";

    network_packet_t* packet;
    for (uint index = 0; index < deAllocPacketQ.Size(); index++)
    {
        deAllocPacketQ.PopHead(packet);
        assert(packet != NULL);
        if (packet->freeInternalData)
        {
            //packet->~Packet(); no custom dtor so no need to call default dtor
            gFreeEx(packet->data, TRACKE_MALLOC);
        }
        packetPool.Reclaim(packet);
    }
}
inline void network_application_t::reclaim_packet(network_packet_t *packet)
{
    std::cout << "User Thread Reclaims One Packet";
    bool ret = deAllocPacketQ.PushTail(packet);
    assert(ret == true);
}

int network_application_t::CreateRecvPollingThread(int threadPriority, uint index)
{
    char* arg = (char*)gMallocEx(sizeof(network_application_t*) + sizeof(index),
        TRACKE_MALLOC);
    network_application_t* serv = this;
    memcpy(arg, &serv, sizeof(network_application_t*));
    memcpy(arg + sizeof(network_application_t*), (char*)&index, sizeof(index));
    return JACKIE_Thread::Create(RunRecvCycleLoop, arg, threadPriority);
}
int network_application_t::CreateNetworkUpdateThread(int threadPriority)
{
    return JACKIE_Thread::Create(RunNetworkUpdateCycleLoop, this,
        threadPriority);
}
void network_application_t::stop_recv_thread()
{
    endThreads = true;
#if USE_SINGLE_THREAD == 0
    for (uint i = 0; i < bindedSockets.Size(); i++)
    {
        if (bindedSockets[i]->IsBerkleySocket())
        {
            berkley_socket_t* sock = (berkley_socket_t*)bindedSockets[i];
            if (sock->GetBindingParams()->isNonBlocking == USE_BLOBKING_SOCKET)
            {
                /// try to send 0 data to let recv thread keep running
                /// to detect the isRecvPollingThreadActive === false so that stop the thread
                char zero[] = "This is used to Stop Recv Thread";
                send_params_t sendParams =
                { zero, sizeof(zero), 0, sock->GetBoundAddress(), 0 };
                sock->Send(&sendParams, TRACKE_MALLOC);
                TimeMS timeout = Get32BitsTimeMS() + 1000;
                while (isRecvPollingThreadActive.GetValue() > 0 && Get32BitsTimeMS() < timeout)
                {
                    sock->Send(&sendParams, TRACKE_MALLOC);
                    GecoSleep(100);
                }
            }
        }
    }
#endif
}
void network_application_t::stop_network_update_thread()
{
    endThreads = true;
    isNetworkUpdateThreadActive = false;
}

void network_application_t::ProcessOneRecvParam(recv_params_t* recvParams)
{
    //std::cout << "Process One RecvParam";

#if ENABLE_SECURE_HAND_SHAKE==1
#ifdef CAT_AUDIT
    printf("AUDIT: RECV ");
    for (int index = 0; index < recvParams->bytesRead; ++index)
    {
        printf("%02x", (cat::u8) recvParams->data[index]);
    }
    printf("\n");
#endif
#endif // ENABLE_SECURE_HAND_SHAKE

    // 1.process baned client's recv
    if (IsBanned(recvParams->senderINetAddress))
    {
        for (index = 0; index < pluginListNTS.Size(); index++)
            this->pluginListNTS[index]->OnDirectSocketReceive(recvParams);

        geco_bit_stream_t bs;
        bs.Write((msg_id_t)ID_CONNECTION_BANNED);
        bs.Write(OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
        bs.WriteMini(myGuid);

        send_params_t bsp;
        bsp.data = bs.char_data();
        bsp.length = bs.get_written_bytes();
        bsp.receiverINetAddress = recvParams->senderINetAddress;
        if (recvParams->localBoundSocket->Send(&bsp, TRACKE_MALLOC) > 0)
        {
            for (index = 0; index < this->pluginListNTS.Size(); index++)
                this->pluginListNTS[index]->OnDirectSocketSend(&bsp);
        }
        return;
    }

    assert(recvParams->senderINetAddress.GetPortHostOrder() != 0);

    // 2. Try to process this recv params as unconnected
    //bool isUnconnectedRecvPrrams;
    IsOfflineRecvParams(recvParams, &this->isUnconnectedRecvPrrams);

    /// no need to use return vlue
    //bool notSend2ReliabilityLayer = ProcessOneUnconnectedRecvParams(recvParams, &isUnconnectedRecvPrrams);
    //if (!notSend2ReliabilityLayer)

    if (!this->isUnconnectedRecvPrrams)
    {
        /// See if this datagram came from a connected system
        remote_system_t* remoteEndPoint = GetRemoteSystem(
            recvParams->senderINetAddress, true, true);
        if (remoteEndPoint != 0) // if this datagram comes from connected system
        {
            remoteEndPoint->reliabilityLayer.ProcessOneConnectedRecvParams(this,
                recvParams, remoteEndPoint->MTUSize);
        }
        else
        {
            char str[256];
            recvParams->senderINetAddress.ToString(true, str);
            std::cout << "Network Thread Says Packet from unconnected sender "
                << str;
        }
    }
}

void network_application_t::IsOfflineRecvParams(recv_params_t* recvParams,
    bool* isOfflinerecvParams)
{
    // The reason for all this is that the reliability layer has no way to tell between offline
    // messages that arrived late for a player that is now connected,
    // and a regular encoding. So I insert OFFLINE_MESSAGE_DATA_ID into the stream,
    // the encoding of which is essentially impossible to hit by chance
    if (recvParams->bytesRead <= 2)
    {
        *isOfflinerecvParams = true;
    }
    else
    {
        switch ((msg_id_t)recvParams->data[0])
        {
            case ID_UNCONNECTED_PING:
                if (recvParams->bytesRead
                    >= sizeof(msg_id_t) + sizeof(Time)
                    + sizeof(OFFLINE_MESSAGE_DATA_ID))
                {
                    *isOfflinerecvParams = memcmp(
                        recvParams->data + sizeof(msg_id_t) + sizeof(Time),
                        OFFLINE_MESSAGE_DATA_ID,
                        sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
                }
                if (*isOfflinerecvParams)
                {
                    for (index = 0; index < pluginListNTS.Size(); index++)
                        pluginListNTS[index]->OnDirectSocketReceive(recvParams);
                    // TO-DO
                }
                break;
            case ID_UNCONNECTED_PING_OPEN_CONNECTIONS:
                if (recvParams->bytesRead
                    >= sizeof(msg_id_t) + sizeof(Time)
                    + sizeof(OFFLINE_MESSAGE_DATA_ID))
                {
                    *isOfflinerecvParams = memcmp(
                        recvParams->data + sizeof(msg_id_t) + sizeof(Time),
                        OFFLINE_MESSAGE_DATA_ID,
                        sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
                }
                if (*isOfflinerecvParams)
                {
                    for (index = 0; index < pluginListNTS.Size(); index++)
                        pluginListNTS[index]->OnDirectSocketReceive(recvParams);
                    // TO-DO
                }
                break;
            case ID_UNCONNECTED_PONG:
                if (recvParams->bytesRead
                    > sizeof(msg_id_t) + sizeof(TimeMS) + guid_t::size()
                    + sizeof(OFFLINE_MESSAGE_DATA_ID))
                {
                    *isOfflinerecvParams = memcmp(
                        recvParams->data + sizeof(msg_id_t) + sizeof(Time)
                        + guid_t::size(), OFFLINE_MESSAGE_DATA_ID,
                        sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
                }
                else
                {
                    *isOfflinerecvParams = memcmp(
                        recvParams->data + sizeof(msg_id_t) + sizeof(TimeMS)
                        + guid_t::size(), OFFLINE_MESSAGE_DATA_ID,
                        sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
                }
                if (*isOfflinerecvParams)
                {

                    for (index = 0; index < pluginListNTS.Size(); index++)
                        pluginListNTS[index]->OnDirectSocketReceive(recvParams);
                    ///TO-DO
                }
                break;
            case ID_OUT_OF_BAND_INTERNAL:
                if (recvParams->bytesRead
                    >= sizeof(msg_id_t) + guid_t::size()
                    + sizeof(OFFLINE_MESSAGE_DATA_ID))
                {
                    *isOfflinerecvParams = memcmp(
                        recvParams->data + sizeof(msg_id_t)
                        + guid_t::size(), OFFLINE_MESSAGE_DATA_ID,
                        sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
                }
                if (*isOfflinerecvParams)
                {
                    for (index = 0; index < pluginListNTS.Size(); index++)
                        pluginListNTS[index]->OnDirectSocketReceive(recvParams);
                    // to-do
                }
                break;
            case ID_OPEN_CONNECTION_REPLY_1:
                OnConnectionReply1(recvParams, isOfflinerecvParams);
                break;
            case ID_OPEN_CONNECTION_REPLY_2:
                OnConnectionReply2(recvParams, isOfflinerecvParams);
                break;
            case ID_OPEN_CONNECTION_REQUEST_1:
                OnConnectionRequest1(recvParams, isOfflinerecvParams);
                break;
            case ID_OPEN_CONNECTION_REQUEST_2:
                OnConnectionRequest2(recvParams, isOfflinerecvParams);
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                OnConnectionFailed(recvParams, isOfflinerecvParams);
                break;
            case ID_CANNOT_ACCEPT_INCOMING_CONNECTIONS:
                OnConnectionFailed(recvParams, isOfflinerecvParams);
                break;
            case ID_CONNECTION_BANNED:
                OnConnectionFailed(recvParams, isOfflinerecvParams);
                break;
            case ID_ALREADY_CONNECTED:
                OnConnectionFailed(recvParams, isOfflinerecvParams);
                break;
            case ID_YOU_CONNECT_TOO_OFTEN:
                OnConnectionFailed(recvParams, isOfflinerecvParams);
                break;
            case ID_INCOMPATIBLE_PROTOCOL_VERSION:
                /// msg layout: MessageID MessageID OFFLINE_MESSAGE_DATA_ID JackieGUID
                OnConnectionFailed(recvParams, isOfflinerecvParams);
                break;
            default:
                *isOfflinerecvParams = false;
                break;
        }
    }
}

void network_application_t::OnConnectionFailed(recv_params_t* recvParams,
    bool* isOfflinerecvParams)
{
    if (recvParams->bytesRead
                    > sizeof(msg_id_t) + sizeof(OFFLINE_MESSAGE_DATA_ID)
                    && recvParams->bytesRead
                    <= sizeof(msg_id_t) + sizeof(OFFLINE_MESSAGE_DATA_ID)
                    + guid_t::size())
    {
        *isOfflinerecvParams = memcmp(recvParams->data + sizeof(msg_id_t),
            OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
    }

    if (*isOfflinerecvParams)
    {
        for (index = 0; index < pluginListNTS.Size(); index++)
            pluginListNTS[index]->OnDirectSocketReceive(recvParams);

        geco_bit_stream_t reader((uchar*)recvParams->data, recvParams->bytesRead);
        msg_id_t msgid;
        reader.Read(msgid);
        std::cout << "client receives msg with " << "msgid " << (int)msgid;
        if ((msg_id_t)recvParams->data[0] == ID_INCOMPATIBLE_PROTOCOL_VERSION)
        {
            msg_id_t update_protocol_version;
            reader.Read(update_protocol_version);
            std::cout << "update_protocol_version "
                << (int)update_protocol_version;
        }
        reader.skip_read_bytes(sizeof(OFFLINE_MESSAGE_DATA_ID));

        guid_t guid;
        reader.Read(guid);
        std::cout << "guid " << guid.g;

        connection_request_t *connectionRequest;
        bool connectionAttemptCancelled = false;

        connReqQLock.Lock();
        for (index = 0; index < connReqQ.Size(); index++)
        {
            connectionRequest = connReqQ[index];
            if (connectionRequest->actionToTake == connection_request_t::CONNECT
                && connectionRequest->receiverAddr
                == recvParams->senderINetAddress)
            {
                connectionAttemptCancelled = true;
                connReqQ.RemoveAtIndex(index);

#if ENABLE_SECURE_HAND_SHAKE==1
                std::cout
                    << "AUDIT: Connection attempt canceled so deleting connectionRequest->client_handshake object"
                    << connectionRequest->client_handshake;
                OP_DELETE(connectionRequest->client_handshake, TRACKE_MALLOC);
#endif // ENABLE_SECURE_HAND_SHAKE

                OP_DELETE(connectionRequest, TRACKE_MALLOC);
                break;
            }
        }
        connReqQLock.Unlock();

        if (connectionAttemptCancelled)
        {
            /// Tell user connection attempt failed
            network_packet_t* packet = AllocPacket(sizeof(unsigned char),
                (unsigned char*)recvParams->data);
            packet->systemAddress = recvParams->senderINetAddress;
            packet->guid = guid;
            bool ret = allocPacketQ.PushTail(packet);
            assert(ret == true);

        }
        //return true;
    }
}
void network_application_t::OnConnectionRequest2(recv_params_t* recvParams,
    bool* isOfflinerecvParams)
{
    if (recvParams->bytesRead
        >= sizeof(msg_id_t) + sizeof(OFFLINE_MESSAGE_DATA_ID)
        + guid_t::size())
    {
        *isOfflinerecvParams = memcmp(recvParams->data + sizeof(msg_id_t),
            OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
    }
    if (*isOfflinerecvParams)
    {
        std::cout << "Server handles ID_OPEN_CONNECTION_REQUEST_2 STARTS";
        for (index = 0; index < pluginListNTS.Size(); index++)
        {
            pluginListNTS[index]->OnDirectSocketReceive(recvParams);
        }

        geco_bit_stream_t fromClientReader((uchar*)recvParams->data,
            recvParams->bytesRead);
        fromClientReader.skip_read_bytes(sizeof(msg_id_t));
        fromClientReader.skip_read_bytes(sizeof(OFFLINE_MESSAGE_DATA_ID));

        bool clientSecureConnectionEnabled = false;
        bool clientSecureRequiredbyServer = false;

#if ENABLE_SECURE_HAND_SHAKE == 1
        char receivedChallengeFromClient[cat::EasyHandshake::CHALLENGE_BYTES];

        // prepare
        if (this->secureIncomingConnectionEnabled)
        {
            char str1[65];
            recvParams->senderINetAddress.ToString(false, str1);
            clientSecureRequiredbyServer = this->IsInSecurityExceptionList(
                recvParams->senderINetAddress) == false;

            uint cookie;
            fromClientReader.Read(cookie);
            if (this->serverCookie.Verify(
                &recvParams->senderINetAddress.address,
                sizeof(recvParams->senderINetAddress.address), cookie)
                == false)
            {
                std::cout << "Server NOT verifies Cookie" << cookie
                    << " from client of "
                    << recvParams->senderINetAddress.ToString();
                return;
            }
            std::cout << "Server verified Cookie from client !";

            fromClientReader.Read(clientSecureConnectionEnabled);
            if (clientSecureRequiredbyServer && !clientSecureConnectionEnabled)
            {
                std::cout
                    << "Fail, This client doesn't enable security connection, but server says this client is needing secure connect";
                return;
            }

            if (clientSecureConnectionEnabled)
            {
                fromClientReader.ReadAlignedBytes(
                    (unsigned char*)receivedChallengeFromClient,
                    cat::EasyHandshake::CHALLENGE_BYTES);
            }
        }
#endif // ENABLE_SECURE_HAND_SHAKE

        network_address_t recvivedBoundAddrFromClient;
        fromClientReader.Read(recvivedBoundAddrFromClient);
        std::cout << "serverReadMini(server_bound_addr) "
            << recvivedBoundAddrFromClient.ToString();
        ushort mtu;
        fromClientReader.Read(mtu);
        std::cout << "server ReadMini(mtu) " << mtu;
        guid_t guid;
        fromClientReader.Read(guid);
        std::cout << "server ReadMini(client guid) " << guid.g;

        int outcome;
        remote_system_t* rsysaddr = GetRemoteSystem(
            recvParams->senderINetAddress, true, true);
        bool usedAddr = rsysaddr != 0 && rsysaddr->isActive;

        remote_system_t* rsysguid = GetRemoteSystem(guid, true);
        bool usedGuid = rsysguid != 0 && rsysguid->isActive;

        // usedAddr, usedGuid, outcome
        // TRUE,	  , TRUE	 , ID_OPEN_CONNECTION_REPLY if they are the same, 1, else ID_ALREADY_CONNECTED, 2;
        // FALSE,   , TRUE  , ID_ALREADY_CONNECTED (someone else took this guid), 3;
        // TRUE,	  , FALSE	 , ID_ALREADY_CONNECTED (silently disconnected, restarted rakNet), 4;
        // FALSE	  , FALSE	 , Allow connection, 0;
        if (usedAddr && usedGuid)
        {
            if (rsysaddr == rsysguid
                && rsysaddr->connectMode
                == remote_system_t::UNVERIFIED_SENDER)
            {
                // @return ID_OPEN_CONNECTION_REPLY to client if they are the same
                outcome = 1;

                // Note to self:
                // If  rsysaddr->connectMode == REQUESTED_CONNECTION,
                // this means two systems attempted to connect to each other at the same time, and one finished first.
                // Returns ID)_CONNECTION_REQUEST_ACCEPTED to one system, and ID_ALREADY_CONNECTED followed by ID_NEW_INCOMING_CONNECTION to another
            }
            else
            {
                // @remarks we take all cases below as same connected client
                // @case client restarted jackie application and do not call disconnect() as normal,
                // but silently disconnected in server. so same  @rsysaddr but differnet  @rsysguid
                // @case client connects again from same ip with open another  jackie application
                // to connect to us and so same @rsysaddr but differnet @rsysguid
                // @case someone else took this guid and use it to connect to us
                // and so different @rsysaddr but same @rsysguid
                // @return 	ID_ALREADY_CONNECTED to client
                outcome = 2;
            }
        }
        else if (!usedAddr && usedGuid)
        {
            // @remarks we take all cases below as same connected client
            // @case someone else took this guid and use it to connect to us
            // and so different @rsysaddr but same @rsysguid
            // @return ID_ALREADY_CONNECTED to client
            outcome = 3;
        }
        else if (usedAddr && !usedGuid)
        {
            // @remarks we take all cases below as same connected client
            // @case client restarted jackie application and do not call disconnect() as normal,
            // but silently disconnected in server. so same  @rsysaddr but differnet  @rsysguid
            // @return ID_ALREADY_CONNECTED to client
            outcome = 4;
        }
        else
        {
            // this client is totally new client and  new connection is allowed
            outcome = 0;
            std::cout << " sever allowed new connection for this client";
        }

        geco_bit_stream_t toClientReplay2Writer;
        toClientReplay2Writer.Write(ID_OPEN_CONNECTION_REPLY_2);
        toClientReplay2Writer.Write(OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
        toClientReplay2Writer.WriteMini(myGuid);
        toClientReplay2Writer.WriteMini(recvParams->senderINetAddress);
        toClientReplay2Writer.WriteMini(mtu);
        toClientReplay2Writer.WriteMini(clientSecureRequiredbyServer);

        //return ID_OPEN_CONNECTION_REPLY if they are the same
        if (outcome == 1)
        {
            std::cout << "Server return ID_OPEN_CONNECTION_REPLY_2 to client";
            // Duplicate connection request packet from packetloss
            // Send back the same answer
#if ENABLE_SECURE_HAND_SHAKE==1
            if (clientSecureRequiredbyServer)
            {
                std::cout
                    << "AUDIT: Resending public key and answer from packetloss.  Sending ID_OPEN_CONNECTION_REPLY_2";
                toClientReplay2Writer.write_aligned_bytes(
                    (const unsigned char *)rsysaddr->answer,
                    sizeof(rsysaddr->answer));
            }
#endif // ENABLE_SECURE_HAND_SHAKE

            send_params_t bsp;
            bsp.data = toClientReplay2Writer.char_data();
            bsp.length = toClientReplay2Writer.get_written_bytes();
            bsp.receiverINetAddress = recvParams->senderINetAddress;
            recvParams->localBoundSocket->Send(&bsp, TRACKE_MALLOC);

            for (index = 0; index < pluginListNTS.Size(); index++)
                pluginListNTS[index]->OnDirectSocketSend(&bsp);
        }
        // return ID_ALREADY_CONNECTED
        else if (outcome != 0)
        {
            std::cout << "Server return ID_ALREADY_CONNECTED to client";
            geco_bit_stream_t toClientAlreadyConnectedWriter;
            toClientAlreadyConnectedWriter.Write(ID_ALREADY_CONNECTED);
            toClientAlreadyConnectedWriter.Write(OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
            toClientAlreadyConnectedWriter.Write(myGuid);

            send_params_t bsp;
            bsp.data = toClientAlreadyConnectedWriter.char_data();
            bsp.length = toClientAlreadyConnectedWriter.get_written_bytes();
            bsp.receiverINetAddress = recvParams->senderINetAddress;
            recvParams->localBoundSocket->Send(&bsp, TRACKE_MALLOC);

            for (index = 0; index < pluginListNTS.Size(); index++)
                pluginListNTS[index]->OnDirectSocketSend(&bsp);
        }
        /// start to handle new connection from client
        else if (outcome == 0)
        {
            geco_bit_stream_t toClientWriter;

            // no more incoming conn accepted
            if (!CanAcceptIncomingConnection())
            {
                std::cout
                    << "Server return ID_CANNOT_ACCEPT_INCOMING_CONNECTIONS to client";
                toClientWriter.Write(ID_CANNOT_ACCEPT_INCOMING_CONNECTIONS);
                toClientWriter.Write(OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                toClientWriter.WriteMini(myGuid);

                send_params_t bsp;
                bsp.data = toClientWriter.char_data();
                bsp.length = toClientWriter.get_written_bytes();
                bsp.receiverINetAddress = recvParams->senderINetAddress;
                recvParams->localBoundSocket->Send(&bsp, TRACKE_MALLOC);

                for (index = 0; index < pluginListNTS.Size(); index++)
                    pluginListNTS[index]->OnDirectSocketSend(&bsp);
            }
            // Assign this client to Remote System List
            else
            {
                assert(recvParams->senderINetAddress != JACKIE_NULL_ADDRESS);
                TimeMS time = GetTimeMS();
                uint i, j, index2use;

                // test if this client is flooding connection
                // Attackers can flood ID_OPEN_CONNECTION_REQUEST_1 or 2
                // by openning many new jackie application instances to connect to us
                // in order to use up all available connection slots of us
                // Ignore connection attempts if this IP address (port exclusive ) connected within the last 100 ms
                bool thisIPFloodsConnRequest = false;
                remote_system_t* free_rs;

                Add2RemoteSystemList(recvParams, free_rs,
                    thisIPFloodsConnRequest, mtu,
                    recvivedBoundAddrFromClient, guid,
                    clientSecureRequiredbyServer);

                if (thisIPFloodsConnRequest)
                {
                    toClientWriter.Write(ID_YOU_CONNECT_TOO_OFTEN);
                    toClientWriter.Write(OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                    toClientWriter.WriteMini(myGuid);
                    //SocketLayer::SendTo( rakNetSocket, (const char*) bsOut.GetData(), bsOut.GetNumberOfBytesUsed(), systemAddress, _FILE_AND_LINE_ );

                    send_params_t bsp;
                    bsp.data = toClientWriter.char_data();
                    bsp.length = toClientWriter.get_written_bytes();
                    bsp.receiverINetAddress = recvParams->senderINetAddress;
                    recvParams->localBoundSocket->Send(&bsp, TRACKE_MALLOC);

                    for (index = 0; index < pluginListNTS.Size(); index++)
                        pluginListNTS[index]->OnDirectSocketSend(&bsp);
                } // thisIPFloodsConnRequest == true

#if ENABLE_SECURE_HAND_SHAKE==1
                if (clientSecureRequiredbyServer)
                {
                    if (this->serverHandShaker.ProcessChallenge(
                        receivedChallengeFromClient, free_rs->answer,
                        free_rs->reliabilityLayer.GetAuthenticatedEncryption()))
                    {
                        std::cout << "AUDIT: Challenge good!\n";
                        // Keep going to OK block
                    }
                    else
                    {
                        std::cout
                            << "AUDIT: Challenge is BAD! Unassign this remote system";
                        // Unassign this remote system
                        DeRefRemoteSystem(recvParams->senderINetAddress);
                        return;
                    }

                    toClientReplay2Writer.write_aligned_bytes(
                        (const unsigned char *)free_rs->answer,
                        sizeof(free_rs->answer));
                }
#endif // ENABLE_SECURE_HAND_SHAKE

                send_params_t bsp;
                bsp.data = toClientReplay2Writer.char_data();
                bsp.length = toClientReplay2Writer.get_written_bytes();
                bsp.receiverINetAddress = recvParams->senderINetAddress;
                recvParams->localBoundSocket->Send(&bsp, TRACKE_MALLOC);

                for (index = 0; index < pluginListNTS.Size(); index++)
                    pluginListNTS[index]->OnDirectSocketSend(&bsp);

            }  // 	CanAcceptIncomingConnection() == true
        } // outcome == 0

        std::cout << "Server handles ID_OPEN_CONNECTION_REQUEST_2 ENDS";
    }
}
void network_application_t::OnConnectionRequest1(recv_params_t* recvParams,
    bool* isOfflinerecvParams)
{
    if ((uint)recvParams->bytesRead >= sizeof(msg_id_t) + sizeof(OFFLINE_MESSAGE_DATA_ID) + guid_t::size())
    {
        *isOfflinerecvParams = memcmp(recvParams->data + sizeof(msg_id_t),
            OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
    }
    if (*isOfflinerecvParams == true)
    {
        std::cout << "server start to handle ID_OPEN_CONNECTION_REQUEST_1";
        for (index = 0; index < pluginListNTS.Size(); index++)
            pluginListNTS[index]->OnDirectSocketReceive(recvParams);

        geco_bit_stream_t writer;
        unsigned char remote_system_protcol =
            (unsigned char)recvParams->data[sizeof(msg_id_t)
            + sizeof(OFFLINE_MESSAGE_DATA_ID)];

        // see if the protocol is up-to-date
        if (remote_system_protcol != (msg_id_t)JACKIE_INET_PROTOCOL_VERSION)
        {
            //test_sendto(*this);
            writer.Write(ID_INCOMPATIBLE_PROTOCOL_VERSION);
            writer.Write((msg_id_t)JACKIE_INET_PROTOCOL_VERSION);
            writer.Write(OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
            writer.WriteMini(myGuid);

            send_params_t data2send;
            data2send.data = writer.char_data();
            data2send.length = writer.get_written_bytes();
            data2send.receiverINetAddress = recvParams->senderINetAddress;

            /// we do not need test 10040 error because it is only 24 bytes length
            /// impossible to exceed the max mtu
            if (recvParams->localBoundSocket->Send(&data2send, TRACKE_MALLOC)
        > 0)
            {
                for (index = 0; index < pluginListNTS.Size(); index++)
                    pluginListNTS[index]->OnDirectSocketSend(&data2send);
            }
        }
        else
        {
#if !defined(__native_client__) && !defined(WINDOWS_STORE_RT)
            if (recvParams->localBoundSocket->IsBerkleySocket())
                ((berkley_socket_t*)recvParams->localBoundSocket)->SetDoNotFragment(
                1);
#endif
            writer.Write(ID_OPEN_CONNECTION_REPLY_1);
            writer.Write((unsigned char*)OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
            writer.WriteMini(myGuid);

#if ENABLE_SECURE_HAND_SHAKE==1
            if (secureIncomingConnectionEnabled)
            {
                writer.WriteMini(true);  // HasCookie on
                std::cout
                    << "AUDIT: server WriteMini(HasCookie On true) to client";
                uint cookie = serverCookie.Generate(&recvParams->senderINetAddress.address, sizeof(recvParams->senderINetAddress.address));
                writer.Write(cookie); // Write cookie
                std::cout << "AUDIT: server WriteMini(cookie " << cookie
                    << ") to client";
                // @Important !!!
                // this is dangeous to send public key to remote because,
                // legal client is pre-given a server public key and client uses this key to encrypt and pre-generate
                // challenge including YK to implement Diffie-Hellman Key Exchange/Agreement Algorithm in the following
                // steps. actually official server can tell if client has same public key based on the returned value
                // of server_handshake->ProcessChallenge(), because it uses its private key to decrypt challenge
                // Write my public key. so it is commented to remember myself.
                // writer.write_aligned_bytes((const unsigned char *)server_public_key_, sizeof(server_public_key_));
                // std::cout << "AUDIT: server write_aligned_bytes(server_public_key_) to client";
            }
#else // ENABLE_SECURE_HAND_SHAKE
            writer.WriteMini(false);  // HasCookie off
            std::cout << "AUDIT: server WriteMini(HasCookie Off false) to client";
#endif
            // MTU. Lower MTU if it exceeds our own limit.
            // because the size clients send us is hardcoded as
            // bitStream.pad_zeros_up_to(mtuSizes[MTUSizeIndex] -
            // UDP_HEADER_SIZE);  see connect() for details
            ushort newClientMTU =
                (recvParams->bytesRead + UDP_HEADER_SIZE >= MAXIMUM_MTU_SIZE) ?
            MAXIMUM_MTU_SIZE :
                             recvParams->bytesRead + UDP_HEADER_SIZE;
            writer.WriteMini(newClientMTU);
            std::cout << "AUDIT: server WriteMini(newClientMTU)" << newClientMTU
                << " to client";
            // Pad response with zeros to MTU size
            // so the connection's MTU will be tested in both directions
            // writer.pad_zeros_up_to(newClientMTU - writer.get_written_bytes()); //this is wrong
            writer.pad_zeros_up_to(newClientMTU - UDP_HEADER_SIZE);
            std::cout << "AUDIT: server pad_zeros_up_to "
                << newClientMTU - UDP_HEADER_SIZE;

            send_params_t bsp;
            bsp.data = writer.char_data();
            bsp.length = writer.get_written_bytes();
            bsp.receiverINetAddress = recvParams->senderINetAddress;

            // this send will never return 10040 error because bsp.length must be <= MAXIMUM_MTU_SIZE
            if (recvParams->localBoundSocket->Send(&bsp, TRACKE_MALLOC) > 0)
            {
                for (index = 0; index < pluginListNTS.Size(); index++)
                    pluginListNTS[index]->OnDirectSocketSend(&bsp);
            }

#if !defined(__native_client__) && !defined(WINDOWS_STORE_RT)
            if (recvParams->localBoundSocket->IsBerkleySocket())
                ((berkley_socket_t*)recvParams->localBoundSocket)->SetDoNotFragment(
                0);
#endif
        }
        //return true;
    }
}
void network_application_t::OnConnectionReply1(recv_params_t* recvParams,
    bool* isOfflinerecvParams)
{
    if (recvParams->bytesRead
        >= sizeof(msg_id_t) + sizeof(OFFLINE_MESSAGE_DATA_ID)
        + guid_t::size())
    {
        // isOfflinerecvParams ?
        *isOfflinerecvParams = memcmp(recvParams->data + sizeof(msg_id_t),
            OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
    }
    if (*isOfflinerecvParams) // only process offline msg
    {
        std::cout << "Client starts to handle ID_OPEN_CONNECTION_REPLY_1";
        for (index = 0; index < pluginListNTS.Size(); index++)
            pluginListNTS[index]->OnDirectSocketReceive(recvParams);

        geco_bit_stream_t fromServerReader((uchar*)recvParams->data,
            recvParams->bytesRead);
        fromServerReader.skip_read_bytes(sizeof(msg_id_t));
        fromServerReader.skip_read_bytes(sizeof(OFFLINE_MESSAGE_DATA_ID));

        guid_t serverGuid;
        fromServerReader.Read(serverGuid);

        bool serverRequiresSecureConn;
        fromServerReader.ReadMini(serverRequiresSecureConn);

        uint cookie;
        if (serverRequiresSecureConn)
        {
            fromServerReader.Read(cookie);
        }

        connection_request_t *connectionRequest;
        connReqQLock.Lock();
        for (unsigned int index = 0; index < connReqQ.Size(); index++)
        {
            connectionRequest = connReqQ[index];
            if (connectionRequest->actionToTake == connection_request_t::CONNECT
                && connectionRequest->receiverAddr
                == recvParams->senderINetAddress)
            {
                /// we can  unlock now
                connReqQLock.Unlock();

                /// prepare out data to server
                geco_bit_stream_t toServerWriter;
                toServerWriter.Write((msg_id_t)ID_OPEN_CONNECTION_REQUEST_2);
                toServerWriter.Write(OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));

                // server require secure connection
                if (serverRequiresSecureConn)
                {
                    toServerWriter.Write(cookie);

#if ENABLE_SECURE_HAND_SHAKE==1

                    //unsigned char publicKeyReceivedFromServer[cat::EasyHandshake::PUBLIC_KEY_BYTES];
                    cat::TunnelPublicKey publicKeyReceivedFromServer;
                    // @Important !!! see id_request_1 for details why we do not read public key
                    //fromServerReader.ReadAlignedBytes(publicKeyReceivedFromServer, sizeof(publicKeyReceivedFromServer));

                    //@Important !!!
                    // to be easiliy man-in-middle attacked, do not use in production environment
                    //  locally stored the received public key from server if ACCEPT_ANY_PUBLIC_KEY is enabled
                    if (connectionRequest->publicKeyMode == ACCEPT_ANY_PUBLIC_KEY)
                    {
                        memcpy(connectionRequest->remote_public_key, publicKeyReceivedFromServer.GetPublicKey(), cat::EasyHandshake::PUBLIC_KEY_BYTES);
                        if (!connectionRequest->client_handshake->Initialize(publicKeyReceivedFromServer)
                            || !connectionRequest->client_handshake->GenerateChallenge(
                            connectionRequest->handshakeChallenge))
                        {
                            std::cout
                                << "AUDIT: client_handshake :: server passed a bad public key with ACCEPT_ANY_PUBLIC_KEY and generate challenge failed, Caution !!! to be easiliy man-in-middle attacked, do not use in production environment";
                            return;
                        }
                    }

                    // compare received and locally stored server public keys
                    //if (!cat::SecureEqual(publicKeyReceivedFromServer, connectionRequest->remote_public_key, cat::EasyHandshake::PUBLIC_KEY_BYTES))
                    //{
                    //	std::cout << "AUDIT: Expected public key does not match what was sent by server -- Reporting back ID_PUBLIC_KEY_MISMATCH to user";
                    //	msgid = ID_PUBLIC_KEY_MISMATCH;  // Attempted a connection and couldn't
                    //	packet = AllocPacket(sizeof(MessageID), &msgid);
                    //	packet->systemAddress = connectionRequest->receiverAddr;
                    //	packet->guid = serverGuid;
                    //	GECO_ASSERT(allocPacketQ.PushTail(packet) == true);
                    //	return;
                    //}

                    // client contains no challenge  We might still pass if we are in the security exception list
                    if (connectionRequest->client_handshake == 0)
                    {
                        toServerWriter.WriteMini(false); //  has chanllenge off
                    }
                    else 	// client contains  a challenge
                    {
                        toServerWriter.WriteMini(true); // has chanllenge on
                        toServerWriter.write_aligned_bytes(
                            (const unsigned char*)connectionRequest->handshakeChallenge,
                            cat::EasyHandshake::CHALLENGE_BYTES);
                        std::cout
                            << "AUDIT: client contains a challenge and write_aligned_bytes(connectionRequest->handshakeChallenge) to server";
                    }
#else // ENABLE_SECURE_HAND_SHAKE
                    // client contain a challenge
                    toServerWriter.WriteMini(false);
#endif
                }
                else 	// Server does not need security
                {
#if ENABLE_SECURE_HAND_SHAKE==1
                    // Security disabled by server but client expects security (indicated by client_handshake not null) so failing
                    if (connectionRequest->client_handshake != 0)
                    {
                        std::cout
                            << "AUDIT: Security disabled by server but client expects security (indicated by client_handshake not null) so failing!";
                        msgid = ID_WECLI_SECURE_BUT_SRV_NO; // Attempted a connection and couldn't
                        packet = AllocPacket(sizeof(msg_id_t), &msgid);
                        packet->systemAddress = connectionRequest->receiverAddr;
                        packet->guid = serverGuid;
                        bool ret = allocPacketQ.PushTail(packet);
                        assert(ret == true);
                        return;
                    }
#endif
                }

                // echo server's bound address
                toServerWriter.WriteMini(connectionRequest->receiverAddr);
                std::cout
                    << "client WriteMini(connectionRequest->receiverAddr) to server";

                // echo MTU
                ushort mtu;
                fromServerReader.Read(mtu);
                toServerWriter.WriteMini(mtu);
                std::cout << "client WriteMini(mtu)" << mtu << " to server";

                // echo Our guid
                toServerWriter.WriteMini(myGuid);
                std::cout << "client WriteMini(myGuid) " << myGuid.g
                    << " to server ";

                send_params_t outcome_data;
                outcome_data.data = toServerWriter.char_data();
                outcome_data.length = toServerWriter.get_written_bytes();
                outcome_data.receiverINetAddress =
                    recvParams->senderINetAddress;
                recvParams->localBoundSocket->Send(&outcome_data, TRACKE_MALLOC
                    );

                for (index = 0; index < pluginListNTS.Size(); index++)
                    pluginListNTS[index]->OnDirectSocketSend(&outcome_data);

                //return true;
            }
        }
        connReqQLock.Unlock();
    }
}
void network_application_t::OnConnectionReply2(recv_params_t* recvParams,
    bool* isOfflinerecvParams)
{
    if (recvParams->bytesRead
        >= sizeof(msg_id_t) + sizeof(OFFLINE_MESSAGE_DATA_ID)
        + guid_t::size())
    {
        *isOfflinerecvParams = memcmp(recvParams->data + sizeof(msg_id_t),
            OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID)) == 0;
    }
    if (*isOfflinerecvParams)
    {
        std::cout << "Client handle  ID_OPEN_CONNECTION_REPLY_2 STARTS";

        for (index = 0; index < pluginListNTS.Size(); index++)
            pluginListNTS[index]->OnDirectSocketReceive(recvParams);

        guid_t guid;
        ushort mtu;
        bool clientSecureRequiredbyServer = false;
        network_address_t ourOwnBoundAddEchoFromServer;

        geco_bit_stream_t bs((unsigned char*)recvParams->data,
            recvParams->bytesRead);
        bs.skip_read_bytes(sizeof(msg_id_t));
        bs.skip_read_bytes(sizeof(OFFLINE_MESSAGE_DATA_ID));
        bs.Read(guid);
        bs.Read(ourOwnBoundAddEchoFromServer);
        bs.Read(mtu);
        bs.ReadMini(clientSecureRequiredbyServer);

#if ENABLE_SECURE_HAND_SHAKE==1
        char answer[cat::EasyHandshake::ANSWER_BYTES];
        std::cout << "AUDIT: clientSecureRequiredbyServer="
            << (int)clientSecureRequiredbyServer;
        if (clientSecureRequiredbyServer)
        {
            std::cout << "AUDIT: read server's answer";
            bs.ReadAlignedBytes((unsigned char*)answer, sizeof(answer));
        }
        //cat::ClientEasyHandshake *client_handshake = 0;
#endif // ENABLE_SECURE_HAND_SHAKE == 1

        // start to remove conn req from client
        connection_request_t *connReq;
        bool unlock = true;

        connReqQLock.Lock();
        for (unsigned i = 0; i < connReqQ.Size(); i++)
        {
            connReq = connReqQ[i];
            if (connReq->receiverAddr == recvParams->senderINetAddress)
            {
                assert(connReq->actionToTake == connection_request_t::CONNECT);

                // check if this client has secure required by server
#if ENABLE_SECURE_HAND_SHAKE == 1
                std::cout
                    << "AUDIT: System address matches an entry in the requestedConnectionQueue and doSecurity="
                    << (int)clientSecureRequiredbyServer;
                if (clientSecureRequiredbyServer)
                {
                    // server says you need secure actually you do not,  notify client
                    if (connReq->client_handshake == 0)
                    {
                        connReqQLock.Unlock();

                        std::cout
                            << "AUDIT: Server wants us to pass its pubkey to connect() but we didn't";
                        msgid = ID_WECLINOTPASS_SRVPUBKEY_WHENCONNECT;
                        packet = AllocPacket(sizeof(msg_id_t), &msgid);
                        //packet = AllocPacket(sizeof(MessageID) * 2);
                        // Attempted a connection and couldn't
                        //packet->data[0] = ID_WECLINOTUSE_SRVPUBKEY_2CONNECT;
                        //packet->data[1] = 0; // Indicate server public key is missing
                        //packet->bitSize = (sizeof(char) * 8);
                        bool ret = allocPacketQ.PushTail(packet);
                        assert(ret == true);
                        return;
                    }
                }
#endif
                connReqQLock.Unlock();
                unlock = false;

                // You might get this when already connected because of cross-connections
                bool thisIPFloodsConnRequest = false;
                remote_system_t* free_rs = GetRemoteSystem(
                    recvParams->senderINetAddress, true, true);
                if (free_rs == 0)
                {
                    if (connReq->socket != 0)
                    {
                        if (connReq->socket != recvParams->localBoundSocket)
                            recvParams->localBoundSocket = connReq->socket;
                    }
                    Add2RemoteSystemList(recvParams, free_rs,
                        thisIPFloodsConnRequest, mtu,
                        ourOwnBoundAddEchoFromServer, guid,
                        clientSecureRequiredbyServer);
                }

                // 4/13/09 Attackers can flood ID_OPEN_CONNECTION_REQUEST and use up all available connection slots
                // Ignore connection attempts if this IP address connected within the last 100 milliseconds
                if (thisIPFloodsConnRequest == false)
                {
                    // Don't check GetRemoteSystemFromGUID, server will verify
                    if (free_rs != 0)
                    {
                        // process challenge answer from server
#if ENABLE_SECURE_HAND_SHAKE==1
                        cat::u8 ident[cat::EasyHandshake::IDENTITY_BYTES];
                        bool doIdentity = false;
                        if (connReq->client_handshake != 0)
                        {
                            std::cout
                                << "AUDIT: Client Processing Server's answer";
                            if (connReq->publicKeyMode
                                == USE_TWO_WAY_AUTHENTICATION)
                            {
                                if (!connReq->client_handshake->ProcessAnswerWithIdentity(
                                    answer, ident,
                                    free_rs->reliabilityLayer.GetAuthenticatedEncryption()))
                                {
                                    std::cout
                                        << "AUDIT: Processing answer -- Invalid Answer";
                                    connReqQLock.Unlock();
                                    return;
                                }
                                doIdentity = true;
                            }
                            else
                            {
                                if (!connReq->client_handshake->ProcessAnswer(
                                    answer,
                                    free_rs->reliabilityLayer.GetAuthenticatedEncryption()))
                                {
                                    connReqQLock.Unlock();
                                    // notify user
                                    std::cout
                                        << "AUDIT: AUDIT: Client id Processing answer -- Invalid Answer\nExpected public key does not match what was sent by server -- Reporting back ID_PUBLIC_KEY_MISMATCH to user";
                                    msgid = ID_PUBLIC_KEY_MISMATCH; // Attempted a connection and couldn't
                                    packet = AllocPacket(sizeof(msg_id_t),
                                        &msgid);
                                    packet->systemAddress =
                                        recvParams->senderINetAddress;
                                    packet->guid = myGuid;
                                    bool ret = allocPacketQ.PushTail(packet);
                                    assert(ret == true);
                                    return;
                                }
                            }
                        }
#endif // ENABLE_SECURE_HAND_SHAKE

                        free_rs->weInitiateConnection = true;
                        free_rs->connectMode =
                            remote_system_t::REQUESTED_CONNECTION;
                        if (connReq->timeout != 0)
                            free_rs->reliabilityLayer.SetTimeoutTime(
                            connReq->timeout);

                        geco_bit_stream_t temp;
                        temp.Write(ID_CONNECTION_REQUEST);
                        temp.WriteMini(guid);
                        temp.WriteMini(GetTimeMS());

#if ENABLE_SECURE_HAND_SHAKE==1
                        if (clientSecureRequiredbyServer)
                        {
                            temp.WriteMini(true);
                            unsigned char proof[32];
                            free_rs->reliabilityLayer.GetAuthenticatedEncryption()->GenerateProof(
                                proof, sizeof(proof));
                            temp.write_aligned_bytes(proof, sizeof(proof));
                            if (doIdentity)
                            {
                                temp.WriteMini(true);
                                temp.write_aligned_bytes(ident, sizeof(ident));
                            }
                            else
                            {
                                temp.WriteMini(false);
                            }
                        }
                        else
                        {
                            temp.WriteMini(false);
                        }
#else
                        temp.WriteMini(false);
#endif // ENABLE_SECURE_HAND_SHAKE

                        // send passwd to server
                        if (connReq->pwd != 0 && connReq->pwdLen > 0)
                        {
                            temp.write_aligned_bytes((uchar*)connReq->pwd,
                                connReq->pwdLen);
                        }

                        reliable_send_params_t sendParams;
                        sendParams.data = temp.char_data();
                        sendParams.bitsSize = temp.get_written_bits();
                        sendParams.broadcast = false;
                        sendParams.sendPriority =
                            packet_send_priority_t::UNBUFFERED_IMMEDIATELY_SEND;
                        sendParams.orderingChannel = 0;
                        sendParams.currentTime = recvParams->timeRead;
                        sendParams.useCallerDataAllocation = false;
                        sendParams.packetReliability =
                            packet_reliability_t::RELIABLE_NOT_ACK_RECEIPT_OF_PACKET;
                        sendParams.receipt = 0;
                        SendImmediate(sendParams);
                    }
                    // Failed, no connections available anymore notify user
                    else
                    {
                        msgid = ID_CONNECTION_ATTEMPT_FAILED;
                        packet = AllocPacket(sizeof(msgid), &msgid);
                        // Attempted a connection and couldn't
                        packet->systemAddress = connReq->receiverAddr;
                        packet->guid = guid;
                        bool ret = allocPacketQ.PushTail(packet);
                        assert(ret == true);
                    }
                }

                /// remove all same connections cached in the queue
                connReqQLock.Lock();
                for (unsigned int k = 0; k < connReqQ.Size(); k++)
                {
                    if (connReqQ[k]->receiverAddr
                        == recvParams->senderINetAddress)
                    {
                        connReqQ.RemoveAtIndex(k);
                        //break;
                    }
                }
                connReqQLock.Unlock();

#if ENABLE_SECURE_HAND_SHAKE == 1
                std::cout
                    << "AUDIT: clients is deleting client_handshake object"
                    << connReq->client_handshake;
                OP_DELETE(connReq->client_handshake, TRACKE_MALLOC);
#endif // ENABLE_SECURE_HAND_SHAKE
                OP_DELETE(connReq, TRACKE_MALLOC);
                std::cout << "Client Connects Successfully !";
                break;
            }
        }

        if (unlock)
        {
            connReqQLock.Unlock();
        }
    }
}

void network_application_t::ProcessConnectionRequestCancelQ(void)
{
    //std::cout << "Network Thread Process ConnectionRequest CancelQ";

    if (connReqCancelQ.IsEmpty())
    {
        //std::cout << "ConnectionRequestCancelQ is EMPTY";
        return;
    }

    //std::cout << "Connection Request CancelQ is NOT EMPTY";

    network_address_t connReqCancelAddr;
    connection_request_t* connReq = 0;

    connReqCancelQLock.Lock();
    for (uint index = 0; index < connReqCancelQ.Size(); index++)
    {
        /// Cancel pending connection attempt, if there is one
        connReqQLock.Lock();
        for (uint i = 0; i < connReqQ.Size(); i++)
        {
            if (connReqQ[i]->receiverAddr == connReqCancelAddr)
            {
#if ENABLE_SECURE_HAND_SHAKE == 1
                std::cout << "AUDIT: Deleting requested Connection Queue" << i
                    << " client_handshake "
                    << connReqQ[i]->client_handshake;
                OP_DELETE(connReqQ[i]->client_handshake, TRACKE_MALLOC);
#endif
                OP_DELETE(connReqQ[i], TRACKE_MALLOC);
                connReqQ.RemoveAtIndex(i);
                break;
            }
        }
        connReqQLock.Unlock();
    }
    connReqCancelQLock.Unlock();
}

void network_application_t::ProcessConnectionRequestQ(TimeUS& timeUS,
    TimeMS& timeMS)
{
    //DEBUG << "Network Thread Process ConnectionRequestQ";

    if (connReqQ.IsEmpty())
    {
        //std::cout << "ConnectionRequestQ is EMPTY";
        return;
    }

    //std::cout << "Connection Request Q not EMPTY";

    if (timeUS == 0)
    {
        timeUS = Get64BitsTimeUS();
        timeMS = (TimeMS)(timeUS / 1000);
    }

    connection_request_t *connReq = 0;

    connReqQLock.Lock();
    for (uint index = 0; index < connReqQ.Size(); index++)
    {
        connReq = connReqQ[index];
        if (connReq->nextRequestTime < timeMS)
        {
            std::cout << "nextRequestTime = " << connReq->nextRequestTime
                << "timeMS = " << timeMS;
            /// reach the max try times
            if (connReq->requestsMade == connReq->connAttemptTimes + 1)
            {
                /// free data inside conn req
                if (connReq->data != 0)
                {
                    gFreeEx(connReq->data, TRACKE_MALLOC);
                    connReq->data = 0;
                }

                /// Tell USER connection attempt failed
                msg_id_t msgid = ID_CONNECTION_ATTEMPT_FAILED;
                network_packet_t* packet = AllocPacket(sizeof(msg_id_t), &msgid);
                packet->systemAddress = connReq->receiverAddr;
                bool ret = allocPacketQ.PushTail(packet);
                assert(ret == true);

#if ENABLE_SECURE_HAND_SHAKE==1
                std::cout
                    << "AUDIT: Connection attempt FAILED so deleting connectionRequest->client_handshake object "
                    << connReq->client_handshake;
                OP_DELETE(connReq->client_handshake,
                    TRACKE_MALLOC);
#endif
                OP_DELETE(connReq, TRACKE_MALLOC);

                /// remove this conn request fron  queue
                connReqQ.RemoveAtIndex(index);
            }
            else /// try to connect again
            {
                std::cout << "Try to connect again";

                /// more times try to request connection, less mtu used
                int MTUSizeIndex = connReq->requestsMade
                    / (connReq->connAttemptTimes / mtuSizesCount);
                if (MTUSizeIndex >= mtuSizesCount)
                    MTUSizeIndex = mtuSizesCount - 1;

                connReq->requestsMade++;
                connReq->nextRequestTime = timeMS
                    + connReq->connAttemptIntervalMS;

                geco_bit_stream_t bitStream;
                bitStream.Write(ID_OPEN_CONNECTION_REQUEST_1);
                bitStream.Write(OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                bitStream.Write((msg_id_t)JACKIE_INET_PROTOCOL_VERSION);
                bitStream.pad_zeros_up_to(mtuSizes[MTUSizeIndex] - UDP_HEADER_SIZE);
                //bitStream.pad_zeros_up_to(3000 -
                //	UDP_HEADER_SIZE); //will trigger 10040 recvfrom

                connReq->receiverAddr.FixForIPVersion(
                    connReq->socket->GetBoundAddress());

                send_params_t jsp;
                jsp.data = bitStream.char_data();
                jsp.length = bitStream.get_written_bytes();
                jsp.receiverINetAddress = connReq->receiverAddr;

#if !defined(__native_client__) && !defined(WINDOWS_STORE_RT)
                if (connReq->socket->IsBerkleySocket())
                    ((berkley_socket_t*)connReq->socket)->SetDoNotFragment(1);
#endif
                Time sendToStart = GetTimeMS();
                if (connReq->socket->Send(&jsp, TRACKE_MALLOC) < 0
                    && jsp.bytesWritten == 10040)
                {
                    // MessageId: WSAEMSGSIZE
                    // MessageText:
                    // A message sent on a datagram socket was larger than the internal message
                    // buffer or some other network limit, or the buffer used to receive a datagram
                    // into was smaller than the datagram itself.
                    std::cout << "Send return 10040 error!!!";
                    connReq->requestsMade = (unsigned char)((MTUSizeIndex + 1)
                        * (connReq->connAttemptTimes / mtuSizesCount));
                    connReq->nextRequestTime = timeMS;
                }
                else
                {
                    Time sendToEnd = GetTimeMS();
                    if (sendToEnd - sendToStart > 100)
                    {
                        std::cout << "> 100";
                        /// Drop to lowest MTU
                        int lowestMtuIndex = connReq->connAttemptTimes
                            / mtuSizesCount * (mtuSizesCount - 1);
                        if (lowestMtuIndex > connReq->requestsMade)
                        {
                            connReq->requestsMade =
                                (unsigned char)lowestMtuIndex;
                            connReq->nextRequestTime = timeMS;
                        }
                        else
                        {
                            connReq->requestsMade =
                                (unsigned char)(connReq->connAttemptTimes
                                + 1);
                        }
                    }
                    //}

                    /// set back  the SetDoNotFragment to allowed fragment
#if !defined(__native_client__) && !defined(WINDOWS_STORE_RT)
                    if (connReq->socket->IsBerkleySocket())
                        ((berkley_socket_t*)connReq->socket)->SetDoNotFragment(0);
#endif
                }
            }
        }
        connReqQLock.Unlock();
    }
}

void network_application_t::ProcessAllocCommandQ(TimeUS& timeUS, TimeMS& timeMS)
{
    //std::cout << "Network Thread Process Alloc CommandQ";

    cmd_t* cmd = 0;
    remote_system_t* remoteEndPoint = 0;

    /// process command queue
    for (uint index = 0; index < allocCommandQ.Size(); index++)
    {

        /// no need to check if bufferedCommand == 0, because we never push 0 pointer

        // this is very stupid way to ad your code in the assert, when in release mode, this code will never run !!!!
        //assert(allocCommandQ.PopHead(cmd) == true);
        allocCommandQ.PopHead(cmd);
        assert(cmd != NULL);

        switch (cmd->commandID)
        {
            case cmd_t::BCS_SEND:
                std::cout << "BCS_SEND";
                /// GetTime is a very slow call so do it once and as late as possible
                if (timeUS == 0)
                {
                    timeUS = Get64BitsTimeUS();
                    timeMS = (TimeMS)(timeUS / (TimeUS)1000);
                }
                /// send data stored in this bc right now
                if (SendRightNow(timeUS, true, cmd) == false)
                    gFreeEx(cmd->data, TRACKE_MALLOC);
                /// Set the new connection state AFTER we call sendImmediate in case we are
                /// setting it to a disconnection state, which does not allow further sends
                if (cmd->repStatus != remote_system_t::NO_ACTION)
                {
                    remoteEndPoint = GetRemoteSystem(cmd->systemIdentifier, true,
                        true);
                    if (remoteEndPoint != 0)
                        remoteEndPoint->connectMode = cmd->repStatus;
                }
                break;
            case cmd_t::BCS_CLOSE_CONNECTION:
                std::cout << "BCS_CLOSE_CONNECTION";
                CloseConnectionInternally(false, true, cmd);
                break;
            case cmd_t::BCS_CHANGE_SYSTEM_ADDRESS: //re-rout
                remoteEndPoint = GetRemoteSystem(cmd->systemIdentifier, true, true);
                if (remoteEndPoint != 0)
                {
                    int existingSystemIndex = GetRemoteSystemIndex(
                        remoteEndPoint->systemAddress);
                    RefRemoteEndPoint(cmd->systemIdentifier.systemAddress,
                        existingSystemIndex);
                }
                break;
            case cmd_t::BCS_GET_SOCKET:
                std::cout << "BCS_GET_SOCKET";
                break;
            case cmd_t::BCS_ADD_2_BANNED_LIST:
                AddToBanList(cmd->arrayparams,
                    *((TimeMS*)(cmd->arrayparams + strlen(cmd->arrayparams) + 1)));
                std::cout << "BCS_ADD_2_BANNED_LIST";
                break;
            case cmd_t::BCS_CONEECT:
            {
                char* passwd = cmd->data;
                cmd->data += strlen(passwd) + 1;
                uchar passwdlength = *(uint*)cmd->data;
                cmd->data += sizeof(passwdlength);
                key_pair_t* jackiePublicKey = *(key_pair_t**)cmd->data;
                cmd->data += sizeof(jackiePublicKey);
                uchar ConnectionSocketIndex = *(uint*)cmd->data;
                cmd->data += sizeof(ConnectionSocketIndex);
                uchar attemptTimes = *(uint*)cmd->data;
                cmd->data += sizeof(attemptTimes);
                ushort AttemptIntervalMS = *(uint*)cmd->data;
                cmd->data += sizeof(AttemptIntervalMS);
                TimeMS timeout = *(uint*)cmd->data;
                cmd->data += sizeof(timeout);
                uint extraData = *(uint*)cmd->data;
                Connect_(cmd->systemIdentifier.systemAddress.ToString(),
                    cmd->systemIdentifier.systemAddress.GetPortHostOrder(),
                    passwd, passwdlength, jackiePublicKey,
                    ConnectionSocketIndex, attemptTimes, AttemptIntervalMS,
                    timeout, extraData);
            }
            break;
            default:
                std::cout << "Not Found Matched BufferedCommand";
                break;
        }

        std::cout << "Network Thread Reclaims One cmd_t";
        bool ret = deAllocCommandQ.PushTail(cmd);
        assert(ret == true);
    }

}
void network_application_t::ProcessAllocJISRecvParamsQ(void)
{
    //static bool oneshot = false;
    //if (!oneshot)
    //{
    //	GecoMemoryStream bitStream;
    //	bitStream.Write(ID_OPEN_CONNECTION_REQUEST_1);
    //	bitStream.Write(OFFLINE_MESSAGE_DATA_ID,
    //		sizeof(OFFLINE_MESSAGE_DATA_ID));
    //	//bitStream.Write((MessageID)JACKIE_INET_PROTOCOL_VERSION);
    //	bitStream.Write((MessageID)12);
    //	// definitely will not be fragmented because
    //	// UDP_HEADER_SIZE = 28 > 1+16+1=18
    //	bitStream.PadZeroAfterAlignedWRPos(mtuSizes[0] -
    //		UDP_HEADER_SIZE);
    //	JISSendParams jsp;
    //	jsp.data = bitStream.DataInt8();
    //	jsp.length = bitStream.get_written_bytes();
    //	jsp.receiverINetAddress = bindedSockets[0]->GetBoundAddress();
    //	JISSendResult len = bindedSockets[0]->Send(&jsp, TRACE_FILE_AND_LINE_);
    //	std::cout << "actually has sent " << len << " bytes ";
    //	Sleep(1000);
    //	oneshot = true;
    //}

    recv_params_t* recvParams = 0;
    for (uint outter = 0; outter < bindedSockets.Size(); outter++)
    {
        for (uint inner = 0; inner < allocRecvParamQ[outter].Size(); inner++)
        {
            /// no need to check if recvParams == 0, because we never push 0 pointer
            bool ret = allocRecvParamQ[outter].PopHead(recvParams);
            assert(ret == true);
            ProcessOneRecvParam(recvParams);
            ReclaimOneJISRecvParams(recvParams, inner);
        }
    }
}

/// @TO-DO
void network_application_t::AdjustTimestamp(network_packet_t*& incomePacket) const
{
    //std::cout << "@TO-DO AdjustTimestamp()";

    if ((unsigned char)incomePacket->data[0] == ID_TIMESTAMP)
    {
        if (incomePacket->length >= sizeof(char) + sizeof(Time))
        {
            char* data = (char*)&incomePacket->data[sizeof(char)];
            //#ifdef _DEBUG
            //				RakAssert(IsActive());
            //				RakAssert(data);
            //#endif
            //
            //				RakNet::BitStream timeBS(data, sizeof(RakNet::Time), false);
            //				timeBS.EndianSwapBytes(0, sizeof(RakNet::Time));
            //				RakNet::Time encodedTimestamp;
            //				timeBS.Read(encodedTimestamp);
            //
            //				encodedTimestamp = encodedTimestamp - GetBestClockDifferential(systemAddress);
            //				timeBS.SetWriteOffset(0);
            //				timeBS.Write(encodedTimestamp);
        }
    }
}

const guid_t& network_application_t::GetGuidFromSystemAddress(
    const network_address_t input) const
{
    if (input == JACKIE_NULL_ADDRESS)
        return myGuid;

    if (input.systemIndex != (system_index_t)-1
        && input.systemIndex < maxConnections
        && remoteSystemList[input.systemIndex].systemAddress == input)
        return remoteSystemList[input.systemIndex].guid;

    for (unsigned int i = 0; i < maxConnections; i++)
    {
        if (remoteSystemList[i].systemAddress == input)
        {
            // Set the systemIndex so future lookups will be fast
            remoteSystemList[i].guid.systemIndex = (system_index_t)i;
            return remoteSystemList[i].guid;
        }
    }
    return JACKIE_NULL_GUID;
}

remote_system_t* network_application_t::GetRemoteSystem(const network_address_t& sa,
    bool neededBySendThread, bool onlyWantActiveEndPoint) const
{
    if (sa == JACKIE_NULL_ADDRESS)
        return 0;

    if (neededBySendThread)
    {
        int index = GetRemoteSystemIndex(sa);
        if (index != -1)
        {
            if (!onlyWantActiveEndPoint || remoteSystemList[index].isActive)
            {
                assert(remoteSystemList[index].systemAddress == sa);
                return &remoteSystemList[index];
            }
        }
    }
    else
    {
        /// Active EndPoints take priority.  But if matched end point is inactice,
        /// return the first EndPoint match found
        int inActiveEndPointIndex = -1;
        for (uint index = 0; index < maxConnections; index++)
        {
            if (remoteSystemList[index].systemAddress == sa)
            {
                if (remoteSystemList[index].isActive)
                    return &remoteSystemList[index];
                else if (inActiveEndPointIndex == -1)
                    inActiveEndPointIndex = index;
            }
        }

        /// matched end pint was found but it is inactive
        if (inActiveEndPointIndex != -1 && !onlyWantActiveEndPoint)
            return &remoteSystemList[inActiveEndPointIndex];
    }

    // no matched end point found
    return 0;
}
remote_system_t* network_application_t::GetRemoteSystem(
    const guid_address_wrapper_t& senderWrapper, bool neededBySendThread,
    bool onlyWantActiveEndPoint) const
{
    if (senderWrapper.guid != JACKIE_NULL_GUID)
        return GetRemoteSystem(senderWrapper.guid, onlyWantActiveEndPoint);
    else
        return GetRemoteSystem(senderWrapper.systemAddress, neededBySendThread,
        onlyWantActiveEndPoint);
}
remote_system_t* network_application_t::GetRemoteSystem(
    const guid_t& senderGUID, bool onlyWantActiveEndPoint) const
{
    if (senderGUID == JACKIE_NULL_GUID)
        return 0;
    for (uint i = 0; i < maxConnections; i++)
    {
        if (remoteSystemList[i].guid == senderGUID
            && (onlyWantActiveEndPoint == false
            || remoteSystemList[i].isActive))
        {
            return remoteSystemList + i;
        }
    }
    return 0;
}
remote_system_t* network_application_t::GetRemoteSystem(
    const network_address_t& sa) const
{
    int index = GetRemoteSystemIndex(sa);
    if (index == -1)
        return 0;
    return remoteSystemList + index;
}
int network_application_t::GetRemoteSystemIndex(const network_address_t &sa) const
{
    uint hashindex = network_address_t::ToHashCode(sa);
    hashindex = hashindex
        % (maxConnections * RemoteEndPointLookupHashMutiple - 1);
    JackieRemoteIndex* curr = remoteSystemLookup[hashindex];
    while (curr != 0)
    {
        if (remoteSystemList[curr->index].systemAddress == sa)
            return curr->index;
        curr = curr->next;
    }
    return -1;
}

void network_application_t::RefRemoteEndPoint(const network_address_t &sa, uint index)
{
#ifdef _DEBUG
    for (int remoteSystemIndex = 0;
        remoteSystemIndex < maxConnections; ++remoteSystemIndex)
    {
        if (remoteSystemList[remoteSystemIndex].isActive)
        {
            unsigned int hashIndex = GetRemoteSystemIndex(remoteSystemList[remoteSystemIndex].systemAddress);
            assert(hashIndex == remoteSystemIndex);
        }
    }
#endif // _DEBUG

    remote_system_t* remote = remoteSystemList + index;
    network_address_t old = remote->systemAddress;
    if (old != JACKIE_NULL_ADDRESS)
    {
        // The system might be active if rerouting
        assert(remoteSystemList[index].isActive == false);
        // Remove the reference if the reference is pointing to this inactive system
        if (GetRemoteSystem(old) == remote)
        {
            DeRefRemoteSystem(old);
        }
    }

    DeRefRemoteSystem(sa);
    remoteSystemList[index].systemAddress = sa;

    uint hashindex = network_address_t::ToHashCode(sa);
    hashindex = hashindex
        % (maxConnections * RemoteEndPointLookupHashMutiple - 1);

    JackieRemoteIndex *rsi = 0;
    do
    {
        rsi = remoteSystemIndexPool.Allocate();
    } while (rsi == 0);

    if (remoteSystemLookup[hashindex] == 0)
    {
        rsi->next = 0;
        rsi->index = index;
        remoteSystemLookup[hashindex] = rsi;
    }
    else
    {
        JackieRemoteIndex *cur = remoteSystemLookup[hashindex];
        while (cur->next != 0)
        {
            cur = cur->next;
        } /// move to last one
        cur->next = rsi;
        rsi->next = 0;
        rsi->index = index;
    }

}
void network_application_t::DeRefRemoteSystem(const network_address_t &sa)
{
    uint hashindex = network_address_t::ToHashCode(sa);
    hashindex = hashindex
        % (maxConnections * RemoteEndPointLookupHashMutiple - 1);

    JackieRemoteIndex *cur = remoteSystemLookup[hashindex];
    JackieRemoteIndex *last = 0;

    while (cur != 0)
    {
        if (remoteSystemList[cur->index].systemAddress == sa)
        {
            if (last == 0)
            {
                remoteSystemLookup[hashindex] = cur->next;
            }
            else
            {
                last->next = cur->next;
            }
            remoteSystemIndexPool.Reclaim(cur);
            break;
        }
        last = cur;
        cur = cur->next;
    }
}

//@TO-DO
bool network_application_t::SendRightNow(TimeUS currentTime, bool useCallerAlloc,
    cmd_t* bufferedCommand)
{
    std::cout << "@TO-DO::SendRightNow()";
    return true;
}
//@TO-DO
void network_application_t::CloseConnectionInternally(
    bool sendDisconnectionNotification, bool performImmediate,
    cmd_t* bufferedCommand)
{
    std::cout << "@TO-DO::CloseConnectionInternally()";
}

// Attatches a Plugin interface to run code automatically
// on message receipt in the Receive call
void network_application_t::set_plugin(network_plugin_t *plugin)
{
    bool isNotThreadsafe = plugin->UsesReliabilityLayer();
    if (isNotThreadsafe)
    {
        if (pluginListNTS.GetIndexOf(plugin) == MAX_UNSIGNED_LONG)
        {
            plugin->serverApplication = this;
            plugin->OnAttach();
            pluginListNTS.InsertAtLast(plugin);
        }
    }
    else
    {
        if (pluginListTS.GetIndexOf(plugin) == MAX_UNSIGNED_LONG)
        {
            plugin->serverApplication = this;
            plugin->OnAttach();
            pluginListTS.InsertAtLast(plugin);
        }
    }
}

void network_application_t::PacketGoThroughPluginCBs(network_packet_t*& incomePacket)
{

    uint i;
    for (i = 0; i < pluginListTS.Size(); i++)
    {
        switch ((msg_id_t)incomePacket->data[0])
        {
            case ID_DISCONNECTION_NOTIFICATION:
                pluginListTS[i]->OnClosedConnection(incomePacket->systemAddress,
                    incomePacket->guid, LCR_DISCONNECTION_NOTIFICATION);
                break;
            case ID_CONNECTION_LOST:
                pluginListTS[i]->OnClosedConnection(incomePacket->systemAddress,
                    incomePacket->guid, LCR_CONNECTION_LOST);
                break;
            case ID_NEW_INCOMING_CONNECTION:
                pluginListTS[i]->OnNewConnection(incomePacket->systemAddress,
                    incomePacket->guid, true);
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                pluginListTS[i]->OnNewConnection(incomePacket->systemAddress,
                    incomePacket->guid, false);
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_CONNECTION_ATTEMPT_FAILED);
                break;
            case ID_WECLINOTPASS_SRVPUBKEY_WHENCONNECT:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY);
                break;
            case ID_WECLI_SECURE_BUT_SRV_NO:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_OUR_SYSTEM_REQUIRES_SECURITY);
                break;
            case ID_PUBLIC_KEY_MISMATCH:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_PUBLIC_KEY_MISMATCH);
                break;
            case ID_ALREADY_CONNECTED:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_ALREADY_CONNECTED);
                break;
            case ID_CANNOT_ACCEPT_INCOMING_CONNECTIONS:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_NO_FREE_INCOMING_CONNECTIONS);
                break;
            case ID_CONNECTION_BANNED:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_CONNECTION_BANNED);
                break;
            case ID_INVALID_PASSWORD:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_INVALID_PASSWORD);
                break;
            case ID_INCOMPATIBLE_PROTOCOL_VERSION:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_INCOMPATIBLE_PROTOCOL);
                break;
            case ID_YOU_CONNECT_TOO_OFTEN:
                pluginListTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_IP_RECENTLY_CONNECTED);
                break;
        }
    }

    for (i = 0; i < pluginListNTS.Size(); i++)
    {
        switch (incomePacket->data[0])
        {
            case ID_DISCONNECTION_NOTIFICATION:
                pluginListNTS[i]->OnClosedConnection(incomePacket->systemAddress,
                    incomePacket->guid, LCR_DISCONNECTION_NOTIFICATION);
                break;
            case ID_CONNECTION_LOST:
                pluginListNTS[i]->OnClosedConnection(incomePacket->systemAddress,
                    incomePacket->guid, LCR_CONNECTION_LOST);
                break;
            case ID_NEW_INCOMING_CONNECTION:
                pluginListNTS[i]->OnNewConnection(incomePacket->systemAddress,
                    incomePacket->guid, true);
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                pluginListNTS[i]->OnNewConnection(incomePacket->systemAddress,
                    incomePacket->guid, false);
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_CONNECTION_ATTEMPT_FAILED);
                break;
            case ID_WECLINOTPASS_SRVPUBKEY_WHENCONNECT:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY);
                break;
            case ID_WECLI_SECURE_BUT_SRV_NO:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_OUR_SYSTEM_REQUIRES_SECURITY);
                break;
            case ID_PUBLIC_KEY_MISMATCH:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_PUBLIC_KEY_MISMATCH);
                break;
            case ID_ALREADY_CONNECTED:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_ALREADY_CONNECTED);
                break;
            case ID_CANNOT_ACCEPT_INCOMING_CONNECTIONS:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_NO_FREE_INCOMING_CONNECTIONS);
                break;
            case ID_CONNECTION_BANNED:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_CONNECTION_BANNED);
                break;
            case ID_INVALID_PASSWORD:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_INVALID_PASSWORD);
                break;
            case ID_INCOMPATIBLE_PROTOCOL_VERSION:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_INCOMPATIBLE_PROTOCOL);
                break;
            case ID_YOU_CONNECT_TOO_OFTEN:
                pluginListNTS[i]->OnFailedConnectionAttempt(incomePacket,
                    CAFR_IP_RECENTLY_CONNECTED);
                break;
        }
    }

}
void network_application_t::PacketGoThroughPlugins(network_packet_t*& incomePacket)
{

    std::cout << "User Thread Packet Go Through Plugin";

    uint i;
    plugin_action_to_take_t pluginResult;

    for (i = 0; i < pluginListTS.Size(); i++)
    {
        pluginResult = pluginListTS[i]->OnRecvPacket(incomePacket);
        if (pluginResult == PROCESSED_BY_ME_THEN_DEALLOC)
        {
            reclaim_packet(incomePacket);
            // Will do the loop again and get another incomePacket
            incomePacket = 0;
            break; // break out of the enclosing forloop
        }
        else if (pluginResult == HOLD_ON_BY_ME_NOT_DEALLOC)
        {
            incomePacket = 0;
            break;
        }
    }

    for (i = 0; i < pluginListNTS.Size(); i++)
    {
        pluginResult = pluginListNTS[i]->OnRecvPacket(incomePacket);
        if (pluginResult == PROCESSED_BY_ME_THEN_DEALLOC)
        {
            reclaim_packet(incomePacket);
            // Will do the loop again and get another incomePacket
            incomePacket = 0;
            break; // break out of the enclosing forloop
        }
        else if (pluginResult == HOLD_ON_BY_ME_NOT_DEALLOC)
        {
            incomePacket = 0;
            break;
        }
    }
}
void network_application_t::UpdatePlugins(void)
{
    //std::cout << "User Thread Update Plugins";
    uint i;
    for (i = 0; i < pluginListTS.Size(); i++)
    {
        pluginListTS[i]->Update();
    }
    for (i = 0; i < pluginListNTS.Size(); i++)
    {
        pluginListNTS[i]->Update();
    }
}

network_packet_t* network_application_t::fetch_packet(void)
{
    //	TIMED_FUNC();
    // sleep a while to make user thread more responsible
    // and low cpu usaage rate
    GecoSleep(userThreadSleepTime);

#if USE_SINGLE_THREAD == 0
    if (!(active())) return 0;
#endif

#if USE_SINGLE_THREAD != 0
    RunRecvCycleOnce(0);
    RunNetworkUpdateCycleOnce();
#endif

    return RunGetPacketCycleOnce();
}
network_packet_t* network_application_t::RunGetPacketCycleOnce(void)
{
    ReclaimAllCommands();

    /// UPDATE all plugins
    UpdatePlugins();

    /// Pop out one Packet from queue
    if (allocPacketQ.Size() > 0)
    {
        network_packet_t *incomePacket = 0;

        /// Get one income packet from bufferedPacketsQueue
        bool ret = allocPacketQ.PopHead(incomePacket);
        assert(ret == true);
        assert(incomePacket->data != NULL);

        AdjustTimestamp(incomePacket);

        /// Some locally generated packets need to be processed by plugins,
        /// for example ID_FCM2_NEW_HOST. The plugin itself should intercept
        /// these messages generated remotely
        PacketGoThroughPluginCBs(incomePacket);
        PacketGoThroughPlugins(incomePacket);

        return incomePacket;
    }

    return 0;
}

void network_application_t::RunNetworkUpdateCycleOnce(void)
{
    //TIMED_FUNC();
    //// @NOTICE I moved this code to JISbEKELY::RecvFrom()
    //// uint32_t index;
    //static JISRecvParams recv;
    //#if !defined(WINDOWS_STORE_RT) && !defined(__native_client__)
    //
    //		for( index = 0; index < JISList.Size(); index++ )
    //		{
    //			if( JISList[index]->IsBerkleySocket() )
    //			{
    //				JISBerkley* berkelySock = ( (JISBerkley*) JISList[index] );
    //				if( berkelySock->GetSocketTransceiver() != 0 )
    //				{
    //					int len;
    //					char dataOut[MAXIMUM_MTU_SIZE];
    //					do
    //					{
    //						len = berkelySock->GetSocketTransceiver()->
    //							JackieINetRecvFrom(dataOut, &recv.senderINetAddress, true);
    //
    //						if( len > 0 )
    //						{
    //							ProcessOneRecvParam(&recv, updateBitStream);
    //						}
    //					} while( len > 0 );
    //
    //				}
    //			}
    //		}
    //#endif

    /// who alloc who dealloc
    ReclaimAllPackets();

    TimeUS timeUS = 0;
    TimeMS timeMS = 0;
    /// process buffered commands
    ProcessAllocCommandQ(timeUS, timeMS);

    /// process buffered recv params
    ProcessAllocJISRecvParamsQ();

    /// process buffered connection request and cance
    /// Cancel certain conn req before process Connection Request Q
    ProcessConnectionRequestCancelQ();
    ProcessConnectionRequestQ(timeUS, timeMS);
}

void network_application_t::RunRecvCycleOnce(uint index)
{
    //TIMED_FUNC();
    ReclaimAllJISRecvParams(index);
    recv_params_t* recvParams = AllocJISRecvParams(index);
    int result = ((berkley_socket_t*)bindedSockets[index])->RecvFrom(recvParams);
    if (result > 0)
    {
        bool ret = allocRecvParamQ[index].PushTail(recvParams);
        assert(ret == true);
        if (incomeDatagramEventHandler != 0)
        {
            if (!incomeDatagramEventHandler(recvParams))
                std::cout << "incomeDatagramEventHandler(recvStruct) Failed.";
        }
#if USE_SINGLE_THREAD == 0
        if (allocRecvParamQ[index].Size() >=
            allocRecvParamQ->Size() / 2) quitAndDataEvents.TriggerEvent();
#endif
    }
    else
    {
        //@ Remember myself
        // this hsould not happen because we have hard coded the max MTU is 1500
        // so client who uses this library will send not datagram bigger than that,
        // so this error will not trigger. Hoever, for the clients
        /// who uses other library sneds us a datagram that is bigger than our max MTU,
        /// 10040 error  will happend n such case.
        /// in this case, we have nothing to do with such case only way is to tell this guy in realistic world "man, please send smaller datagram to me"
        /// i put it here is just reminding myself and
        if (recvParams->bytesRead == 10040)
        {
            std::cout
                << "recvfrom() return 10040 error, the remote send a bigger datagram than our  max mtu";
        }
        JISRecvParamsPool[index].Reclaim(recvParams);
    }
}

JACKIE_THREAD_DECLARATION(geco::net::RunRecvCycleLoop)
{
    network_application_t *serv = *(network_application_t**)arguments;
    uint index = *((uint*)((char*)arguments + sizeof(network_application_t*)));

    serv->isRecvPollingThreadActive.Increment();

    std::cout << "Recv thread " << "is running in backend....";
    while (!serv->endThreads)
    {
        serv->RunRecvCycleOnce(index);
    }
    std::cout << "Recv polling thread Stops....";

    serv->isRecvPollingThreadActive.Decrement();
    gFreeEx(arguments, TRACKE_MALLOC);
    return 0;
}

JACKIE_THREAD_DECLARATION(geco::net::RunNetworkUpdateCycleLoop)
{
    network_application_t *serv = (network_application_t*)arguments;
    serv->isNetworkUpdateThreadActive = true;

    std::cout << "Network thread is running in backend....";
    while (!serv->endThreads)
    {
        /// Normally, buffered sending packets go out every other 10 ms.
        /// or TriggerEvent() is called by recv thread
        serv->RunNetworkUpdateCycleOnce();
#if USE_SINGLE_THREAD == 0
        serv->quitAndDataEvents.WaitEvent(10);
#endif
    }
    std::cout << "Send polling thread Stops....";
    serv->isNetworkUpdateThreadActive = false;
    return 0;
}

JACKIE_THREAD_DECLARATION(geco::net::UDTConnect)
{
    return 0;
}

GECO_STATIC_FACTORY_DEFIS(network_application_t, network_application_t);

void network_application_t::ResetSendReceipt(void)
{
    sendReceiptSerialMutex.Lock();
    sendReceiptSerial = 1;
    sendReceiptSerialMutex.Unlock();
}

ulonglong network_application_t::CreateUniqueRandness(void)
{
    ulonglong g = Get64BitsTimeUS();
    TimeUS lastTime, thisTime, diff;
    unsigned char diffByte = 0;
    // Sleep a small random time, then use the last 4 bits as a source of randomness
    for (int j = 0; j < 4; j++)
    {
        diffByte = 0;
        for (int index = 0; index < 4; index++)
        {
            lastTime = Get64BitsTimeUS();
            GecoSleep(1);
            thisTime = Get64BitsTimeUS();
            diff = thisTime - lastTime;
            diffByte ^= (unsigned char)((diff & 15) << (index * 2)); ///0xF = 1111 = 15
            if (index == 3)
                diffByte ^= (unsigned char)((diff & 15) >> 2);
        }
        ((unsigned char*)&g)[4 + j] ^= diffByte;
    }
    return g;
}

void network_application_t::CancelConnectionRequest(const network_address_t& target)
{
    std::cout << "User Thread Cancel Connection Request To "
        << target.ToString();
    connReqCancelQLock.Lock();
    bool ret = connReqCancelQ.PushTail(target);
    assert(ret == true);
    connReqCancelQLock.Unlock();
}

void network_application_t::Connect_(const char* host, ushort port,
    const char *passwd /*= 0*/, uchar passwdLength /*= 0*/,
    key_pair_t *jackiePublicKey /*= 0*/, uchar localSocketIndex /*= 0*/,
    uchar attemptTimes /*= 6*/, ushort attemptIntervalMS /*= 100000*/,
    TimeMS timeout /*= 0*/, uint extraData /*= 0*/)
{
    cmd_t* conn_cmd = alloc_cmd();
    conn_cmd->commandID = cmd_t::BCS_CONEECT;
    conn_cmd->systemIdentifier.systemAddress.FromString(host, port);
    conn_cmd->data = (char*)gMallocEx(
        strlen(passwd) + 1 + sizeof(passwdLength) + sizeof(key_pair_t *)
        + sizeof(localSocketIndex) + sizeof(attemptTimes)
        + sizeof(attemptIntervalMS) + sizeof(timeout)
        + sizeof(extraData), TRACKE_MALLOC);

    memcpy(conn_cmd->data, passwd, strlen(passwd) + 1);
    conn_cmd->data += strlen(passwd) + 1;

    memcpy(conn_cmd->data, &passwdLength, sizeof(passwdLength));
    conn_cmd->data += sizeof(passwdLength);

    memcpy(conn_cmd->data, &jackiePublicKey, sizeof(jackiePublicKey));
    conn_cmd->data += sizeof(jackiePublicKey);

    memcpy(conn_cmd->data, &localSocketIndex, sizeof(localSocketIndex));
    conn_cmd->data += sizeof(localSocketIndex);

    memcpy(conn_cmd->data, &attemptTimes, sizeof(attemptTimes));
    conn_cmd->data += sizeof(attemptTimes);

    memcpy(conn_cmd->data, &attemptIntervalMS, sizeof(attemptIntervalMS));
    conn_cmd->data += sizeof(attemptIntervalMS);

    memcpy(conn_cmd->data, &timeout, sizeof(timeout));
    conn_cmd->data += sizeof(timeout);

    memcpy(conn_cmd->data, &extraData, sizeof(extraData));
    run_cmd(conn_cmd);
}

connection_attempt_result_t network_application_t::Connect(const char* host,
    ushort port, const char *passwd /*= 0*/, uchar passwdLength /*= 0*/,
    key_pair_t *jackiePublicKey /*= 0*/,
    uchar ConnectionSocketIndex /*= 0*/, uchar attemptTimes /*= 6*/,
    ushort AttemptIntervalMS /*= 100000*/, TimeMS timeout /*= 0*/,
    uint extraData /*= 0*/)
{
    std::cout << "User Thread start to Connect() to " << host << ":" << port;

    if (host == 0)
    {
        std::cout << "invalid host adress !";
        return INVALID_PARAM;
    }
    if (port == 0)
    {
        std::cout << "invalid port ! !";
        return INVALID_PARAM;
    }
    if (endThreads)
    {
        std::cout << "not call startup() !\n";
        return INVALID_PARAM;
    }

    if (passwdLength > 8)
        passwdLength = 8;
    if (passwd == 0)
        passwdLength = 0;

    bool found = false;
    for (uint i = 0; i < bindedSockets.Size(); i++)
    {
        if (bindedSockets[i]->GetUserConnectionSocketIndex()
            == ConnectionSocketIndex)
            found = true;
    }

    if (!found)
    {
        std::cout << "invalid ConnectionSocketIndex";
        return INVALID_PARAM;
    }

    network_address_t addr;
    bool ret =
        addr.FromString(host, port,
        bindedSockets[ConnectionSocketIndex]->GetBoundAddress().GetIPVersion());
    if (!ret || addr == JACKIE_NULL_ADDRESS)
        return CANNOT_RESOLVE_DOMAIN_NAME;

    if (GetRemoteSystem(addr, false, true) != 0)
        return ALREADY_CONNECTED_TO_ENDPOINT;

    connection_request_t* connReq = OP_NEW<connection_request_t>(TRACKE_MALLOC);
    connReq->receiverAddr = addr;
    connReq->nextRequestTime = GetTimeMS();
    connReq->requestsMade = 0;
    connReq->data = 0;
    connReq->extraData = extraData;
    connReq->socketIndex = ConnectionSocketIndex;
    connReq->socket = bindedSockets[ConnectionSocketIndex];
    connReq->actionToTake = connection_request_t::CONNECT;
    connReq->connAttemptTimes = attemptTimes;
    connReq->connAttemptIntervalMS = AttemptIntervalMS;
    connReq->timeout = timeout;
    connReq->pwdLen = passwdLength;
    if (passwdLength > 0 && passwd != 0)
    {
        memcpy(connReq->pwd, passwd, passwdLength);
    }

#if ENABLE_SECURE_HAND_SHAKE ==1
    std::cout << "Connect()::Generate Connection Request Challenge";
    connReq->client_handshake = 0;
    connReq->publicKeyMode = secure_connection_mode_t::INSECURE_CONNECTION;
    if (!GenerateConnectionRequestChallenge(connReq, jackiePublicKey))
        return SECURITY_INITIALIZATION_FAILED;
#else
    (void)jackiePublicKey;
#endif

    connReqQLock.Lock();
    for (uint Index = 0; Index < connReqQ.Size(); Index++)
    {
        if (connReqQ[Index]->receiverAddr == addr)
        {
            //connReqQLock.Unlock();
            OP_DELETE(connReq, TRACKE_MALLOC);
            return CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS;
        }
    }
    ret = connReqQ.PushTail(connReq);
    assert(ret == true);
    connReqQLock.Unlock();

    return CONNECTION_ATTEMPT_POSTED;
}

#if ENABLE_SECURE_HAND_SHAKE==1
bool network_application_t::GenerateConnectionRequestChallenge(
    connection_request_t *connectionRequest, key_pair_t *jackiePublicKey)
{
    if (jackiePublicKey == 0)
        return true;

    switch (jackiePublicKey->publicKeyMode)
    {
        default:
        case secure_connection_mode_t::INSECURE_CONNECTION:
            break;

        case secure_connection_mode_t::ACCEPT_ANY_PUBLIC_KEY:
            CAT_OBJCLR(connectionRequest->remote_public_key);
            connectionRequest->client_handshake = OP_NEW<cat::ClientEasyHandshake>(
                TRACKE_MALLOC);
            connectionRequest->publicKeyMode =
                secure_connection_mode_t::ACCEPT_ANY_PUBLIC_KEY;
            break;

        case secure_connection_mode_t::USE_TWO_WAY_AUTHENTICATION:
            if (jackiePublicKey->localKeyPair.Valid() && jackiePublicKey->remoteServerPublicKey.Valid())
            {

            }
            else
            {
                return false;
            }

            // init client_handshake
            connectionRequest->client_handshake = OP_NEW<cat::ClientEasyHandshake>(
                TRACKE_MALLOC);
            // copy server pk
            memcpy(connectionRequest->remote_public_key,
                jackiePublicKey->remoteServerPublicKey.GetPublicKey(),
                jackiePublicKey->remoteServerPublicKey.GetPublicKeyBytes());
            if (!connectionRequest->client_handshake->Initialize(
                jackiePublicKey->remoteServerPublicKey)
                || !connectionRequest->client_handshake->SetIdentity(jackiePublicKey->localKeyPair)
                || !connectionRequest->client_handshake->GenerateChallenge(
                connectionRequest->handshakeChallenge))
            {
                std::cout
                    << "AUDIT: Failure initializing new client_handshake object with identity for this connection Request";
                OP_DELETE(connectionRequest->client_handshake, TRACKE_MALLOC);
                connectionRequest->client_handshake = 0;
                return false;
            }

            std::cout
                << "AUDIT: Success initializing new client handshake object with identity for this connection Request -- pre-generated challenge\n";

            connectionRequest->publicKeyMode =
                secure_connection_mode_t::USE_TWO_WAY_AUTHENTICATION;
            break;

        case secure_connection_mode_t::USE_KNOWN_PUBLIC_KEY:
            if (!jackiePublicKey->remoteServerPublicKey.Valid())
                return false;

            connectionRequest->client_handshake = OP_NEW<cat::ClientEasyHandshake>(
                TRACKE_MALLOC);

            //copy server pk to conn req
            memcpy(connectionRequest->remote_public_key,
                jackiePublicKey->remoteServerPublicKey.GetPublicKey(),
                jackiePublicKey->remoteServerPublicKey.GetPublicKeyBytes());

            if (!connectionRequest->client_handshake->Initialize(
                jackiePublicKey->remoteServerPublicKey)
                || !connectionRequest->client_handshake->GenerateChallenge(
                connectionRequest->handshakeChallenge))
            {
                std::cout
                    << "AUDIT: Failure initializing new client_handshake object for this RequestedConnectionStruct\n";
                OP_DELETE(connectionRequest->client_handshake, TRACKE_MALLOC);
                connectionRequest->client_handshake = 0;
                return false;
            }

            std::cout
                << "AUDIT: Success initializing new client handshake object for this Requested Connection -- pre-generated challenge\n";

            connectionRequest->publicKeyMode = secure_connection_mode_t::USE_KNOWN_PUBLIC_KEY;
            break;
    }

    return true;
}
#endif

uint network_application_t::GetIncomingConnectionsCount(void) const
{
    if (remoteSystemList == 0 || endThreads == true)
        return 0;

    unsigned int income_cnnections = 0;
    for (unsigned int i = 0; i < activeSystemListSize; i++)
    {
        if ((activeSystemList[i])->isActive
            && (activeSystemList[i])->connectMode
            == remote_system_t::CONNECTED
            && (activeSystemList[i])->weInitiateConnection == false)
        {
            income_cnnections++;
        }
    }
    return income_cnnections;
}

void network_application_t::AddToActiveSystemList(uint index2use)
{
    activeSystemList[activeSystemListSize++] = remoteSystemList + index2use;
}

bool network_application_t::IsLoopbackAddress(
    const guid_address_wrapper_t &systemIdentifier, bool matchPort) const
{
    if (systemIdentifier.guid != JACKIE_NULL_GUID)
        return systemIdentifier.guid == myGuid;

    for (int i = 0;
        i < MAX_COUNT_LOCAL_IP_ADDR
        && localIPAddrs[i] != JACKIE_NULL_ADDRESS; i++)
    {
        if (matchPort)
        {
            if (localIPAddrs[i] == systemIdentifier.systemAddress)
                return true;
        }
        else
        {
            if (localIPAddrs[i].EqualsExcludingPort(
                systemIdentifier.systemAddress))
                return true;
        }
    }

    return (matchPort == true
        && systemIdentifier.systemAddress == firstExternalID)
        || (matchPort == false
        && systemIdentifier.systemAddress.EqualsExcludingPort(
        firstExternalID));
}

bool network_application_t::enable_secure_inbound_connections(cat::TunnelKeyPair& keys, bool requireClientPublicKey)
{
#if ENABLE_SECURE_HAND_SHAKE==1
    if (endThreads == false)
        return false;

    //// you must call this first to generte jeys otherwise will fail to gernerate keys
    if (!cat::EasyHandshake::ref()->GetTLS()->Valid())
        cat::EasyHandshake::ref()->GetTLS()->OnInitialize();

    // Copy client public key requirement flag
    _require_client_public_key = requireClientPublicKey;

    if (serverHandShaker.Initialize(keys))
    {
        std::cout
            << "AUDIT: Successfully initialized, filling cookie jar with goodies, storing public key and setting using security flag to true";

        serverHandShaker.FillCookieJar(&serverCookie);
        server_public_key_.LoadMemory(keys.GetPublicKey(), keys.GetPublicKeyBytes());
        secureIncomingConnectionEnabled = true;
        return true;
    }

    std::cout
        << "AUDIT: Failure to initialize so deleting server handshake and cookie jar; also setting secureIncomingConnectionEnabled flag = false";
    secureIncomingConnectionEnabled = false;
    return false;
#else
    (void)public_key;
    (void)private_key;
    (void)requireClientPublicKey;
    return false;
#endif
}

bool network_application_t::IsInSecurityExceptionList(network_address_t& jackieAddr)
{
    /// memcmp recvParams->senderINetAddress.address.addr4.sin_addr
    /// more efficient
    std::cout
        << " JackieApplication::IsInSecurityExceptionList::need implement";
    return false;
}

void network_application_t::Add2RemoteSystemList(recv_params_t* recvParams,
    remote_system_t*& free_rs, bool& thisIPFloodsConnRequest, uint mtu,
    network_address_t& recvivedBoundAddrFromClient, guid_t& guid,
    bool clientSecureRequiredbyServer)
{
    TimeMS time = ::GetTimeMS();
    if (limitConnFrequencyOfSameClient)
    {
        if (!IsLoopbackAddress(recvParams->senderINetAddress, false))
        {
            for (unsigned i = 0; i < maxConnections; i++)
            {
                if (remoteSystemList[i].isActive == true
                    && remoteSystemList[i].systemAddress.EqualsExcludingPort(
                    recvParams->senderINetAddress)
                    && time >= remoteSystemList[i].connectionTime
                    && time - remoteSystemList[i].connectionTime < 100)
                {
                    thisIPFloodsConnRequest = true;
                    //ValidateRemoteSystemLookup();
                    free_rs = 0;
                }
            }
        }
    } ///end of  limitConnFrequencyOfSameClient

    // make sure we are using the same port on what we received
    recvivedBoundAddrFromClient.SetPortNetworkOrder(
        recvParams->localBoundSocket->GetBoundAddress().GetPortNetworkOrder());

    // AssignSystemAddressToRemoteSystemList
    for (unsigned index2use = 0; index2use < maxConnections; index2use++)
    {
        if (remoteSystemList[index2use].isActive == false)
        {
            RefRemoteEndPoint(recvParams->senderINetAddress, index2use);

            free_rs = remoteSystemList + index2use;
            free_rs->guid = guid;
            free_rs->MTUSize = defaultMTUSize;
            if (mtu > defaultMTUSize)
            {
                free_rs->MTUSize = mtu;
                assert(free_rs->MTUSize <= MAXIMUM_MTU_SIZE);
            }

            // This one line causes future incoming packets to go through the reliability layer
            free_rs->isActive = true;

            // Reserve this reliability layer for ourselves.
            free_rs->reliabilityLayer.Reset(true, free_rs->MTUSize,
                clientSecureRequiredbyServer);
            free_rs->reliabilityLayer.SetSplitMessageProgressInterval(
                splitMessageProgressInterval);
            free_rs->reliabilityLayer.SetUnreliableTimeout(unreliableTimeout);
            free_rs->reliabilityLayer.SetTimeoutTime(defaultTimeoutTime);
            AddToActiveSystemList(index2use);
            if (recvParams->localBoundSocket->GetBoundAddress()
                == recvivedBoundAddrFromClient)
            {
                free_rs->socket2use = recvParams->localBoundSocket;
            }
            else
            {
                // Client -----> Server : recv from  ip1
                // Client <----- Server : send from ip2
                // Client  -----> Server:recv from ip2 with server_bound_addr = ip1
                // we use ip2 because we believe that  the passive-connection
                // endpoint is always dominont and more official obviously

                char str[256];
                recvivedBoundAddrFromClient.ToString(true, str);
                // See if this is an internal IP address.
                // If so, force binding on it so we reply on the same IP address as they sent to.
                int foundIndex = -1;
                for (unsigned int ipListIndex = 0;
                    ipListIndex < MAX_COUNT_LOCAL_IP_ADDR; ipListIndex++)
                {
                    if (localIPAddrs[ipListIndex] == JACKIE_NULL_ADDRESS)
                        break;

                    if (recvivedBoundAddrFromClient.EqualsExcludingPort(
                        localIPAddrs[ipListIndex]))
                    {
                        foundIndex = ipListIndex;
                        break;
                    }
                }

                // @Reminder
                // Originally this code was to force a machine with multiple IP
                // addresses to reply back on the IP that the datagram came in on
                if (foundIndex == -1)
                {
                    // Client [LAN192.168.0.0.8] -----> GateWay[WAN227.0.54.23] ----->Server [LAN192.168.0.0.8]
                    // Serve is located in LAN and client connect to the Gateway that forwards the data to Server
                    // in this case server_bound_addr = 227.0.54.23 but recvParams->localBoundSocket->GetBoundAddress() = 192.168.0.0.8
                    // we use recvParams->localBoundSocket because it does not matter
                    // Hi man, nobody will deploy the real game server in LAN if it is client/server artechtecture (LAN game exclusive)
                    free_rs->socket2use = recvParams->localBoundSocket;
                }
                else
                {
                    // Force binding
                    unsigned int socketListIndex;
                    for (socketListIndex = 0;
                        socketListIndex < bindedSockets.Size();
                        socketListIndex++)
                    {
                        if (bindedSockets[socketListIndex]->GetBoundAddress()
                            == recvivedBoundAddrFromClient)
                        {
                            // Force binding with existing socket
                            free_rs->socket2use =
                                bindedSockets[socketListIndex];
                            break;
                        }
                    }

                    if (socketListIndex == bindedSockets.Size())
                    {
                        // this hsould not happen !!
                        std::cout
                            << "remote client have no right socket to assign!!!";
                    }

                }
            } // server_bound_addr != recv incomeing socket

            // setup remoteClient varaibles
            for (unsigned j = 0; j < (unsigned)PING_TIMES_ARRAY_SIZE; j++)
            {
                free_rs->pingAndClockDifferential[j].pingTime = 65535;
                free_rs->pingAndClockDifferential[j].clockDifferential = 0;
            }
            free_rs->connectMode = remote_system_t::UNVERIFIED_SENDER;
            free_rs->pingAndClockDifferentialWriteIndex = 0;
            free_rs->lowestPing = 65535;
            free_rs->nextPingTime = 0; // Ping immediately
            free_rs->weInitiateConnection = false;
            free_rs->connectionTime = time;
            free_rs->myExternalSystemAddress = JACKIE_NULL_ADDRESS;
            free_rs->lastReliableSend = time;

#ifdef _DEBUG
            int indexLoopupCheck = GetRemoteSystemIndexGeneral(recvParams->senderINetAddress, true);
            if (indexLoopupCheck != (int)index2use)
            {
                assert((int)indexLoopupCheck == (int)index2use);
            }
            else
            {
                std::cout << "Server enable client of " << recvParams->senderINetAddress.ToString() << " to active end point with index of " << index2use;
            }
#endif
            break;
        } //remoteSystemList[index2use].isActive == false
    } // forloop AssignSystemAddressToRemoteSystemList done
}

bool network_application_t::SendImmediate(reliable_send_params_t& sendParams)
{
    std::cout << "JackieApplication::SendImmediate needs implementations !";
    uint* sendList;
    uint sendListSize = 0;
    bool callerDataAllocationUsed = false;
    uint sendListIndex; 	// Iterates into the list of remote systems

    uint remoteSystemIndex;
    if (sendParams.receiverAdress.systemAddress != JACKIE_NULL_ADDRESS)
        remoteSystemIndex = GetRemoteSystemIndexGeneral(
        sendParams.receiverAdress.systemAddress, true);
    else if (sendParams.receiverAdress.guid != JACKIE_NULL_GUID)
        remoteSystemIndex = GetRemoteSystemIndexGeneral(
        sendParams.receiverAdress.guid);
    else
        remoteSystemIndex = (unsigned int)-1;

    // 03/06/06 - If broadcast is false, use the optimized version of GetIndexFromSystemAddress
    if (sendParams.broadcast == false)
    {
        // not found
        if (remoteSystemIndex == (unsigned int)-1)
        {
            return false;
        }

#if USE_STACK_ALLOCA ==1
        sendList = (unsigned *)alloca(sizeof(unsigned));
#else
        sendList = (unsigned *)gMallocEx(sizeof(unsigned), TRACKE_MALLOC);
#endif

        if (remoteSystemList[remoteSystemIndex].isActive
            && remoteSystemList[remoteSystemIndex].connectMode
            != remote_system_t::DISCONNECT_ASAP
            && remoteSystemList[remoteSystemIndex].connectMode
            != remote_system_t::DISCONNECT_ASAP_SILENTLY
            && remoteSystemList[remoteSystemIndex].connectMode
            != remote_system_t::DISCONNECT_ON_NO_ACK)
        {
            sendList[0] = remoteSystemIndex;
            sendListSize = 1;
        }
    }
    // remoteSystemList in network thread
    else
    {
#if USE_STACK_ALLOCA==1
        sendList = (unsigned *)alloca(sizeof(unsigned));
#else
        sendList = (unsigned *)gMallocEx(sizeof(unsigned), TRACKE_MALLOC);
#endif
        unsigned int idx;
        for (idx = 0; idx < maxConnections; idx++)
        {
            if (remoteSystemIndex != (unsigned int)-1
                && idx == remoteSystemIndex)
                continue;

            if (remoteSystemList[idx].isActive
                && remoteSystemList[idx].systemAddress
                != JACKIE_NULL_ADDRESS)
                sendList[sendListSize++] = idx;
        }
    }

    if (sendListSize == 0)
    {
#if USE_STACK_ALLOCA !=1
        gFreeEx(sendList, TRACKE_MALLOC);
#endif
        return false;
    }

    // start to send one by one
    bool useData;
    for (sendListIndex = 0; sendListIndex < sendListSize; sendListIndex++)
    {
        // Send may split the packet and thus deallocate data.
        // Don't assume data is valid if we use the callerAllocationData
        useData = sendParams.useCallerDataAllocation
            && callerDataAllocationUsed == false
            && sendListIndex + 1 == sendListSize;
        sendParams.useCallerDataAllocation = (useData == false);
        sendParams.mtu = remoteSystemList[sendList[sendListIndex]].MTUSize;
        remoteSystemList[sendList[sendListIndex]].reliabilityLayer.Send(
            sendParams);
        if (useData)
            callerDataAllocationUsed = true;

        // update lastReliableSend
        if (sendParams.packetReliability == RELIABLE_NOT_ACK_RECEIPT_OF_PACKET
            || sendParams.packetReliability
            == RELIABLE_ORDERED_NOT_ACK_RECEIPT_OF_PACKET
            || sendParams.packetReliability
            == RELIABLE_SEQUENCED_NOT_ACK_RECEIPT_OF_PACKET
            || sendParams.packetReliability
            == RELIABLE_ACK_RECEIPT_OF_PACKET
            || sendParams.packetReliability
            == RELIABLE_ORDERED_ACK_RECEIPT_OF_PACKET
            /*||reliability==RELIABLE_SEQUENCED_WITH_ACK_RECEIPT*/)
        {
            remoteSystemList[sendList[sendListIndex]].lastReliableSend =
                (TimeMS)(sendParams.currentTime / 1000);
        }
    }

#if USE_STACK_ALLOCA !=1
    gFreeEx(sendList, TRACKE_MALLOC);
#endif

    // Return value only meaningful if true was passed for useCallerDataAllocation.  Means the reliability layer used that data copy, so the caller should not deallocate it
    return callerDataAllocationUsed;
}

int network_application_t::GetRemoteSystemIndexGeneral(
    const network_address_t& systemAddress,
    bool calledFromNetworkThread /*= false*/) const
{
    if (systemAddress == JACKIE_NULL_ADDRESS)
        return -1;

    if (systemAddress.systemIndex != (system_index_t)-1
        && systemAddress.systemIndex > -1
        && systemAddress.systemIndex < maxConnections
        && remoteSystemList[systemAddress.systemIndex].systemAddress
        == systemAddress
        && remoteSystemList[systemAddress.systemIndex].isActive)
        return systemAddress.systemIndex;

    if (calledFromNetworkThread)
    {
        return GetRemoteSystemIndex(systemAddress);
    }
    // remoteSystemList in user and network thread
    else
    {
        system_index_t i;
        for (i = 0; i < maxConnections; i++)
        {
            if (remoteSystemList[i].systemAddress == systemAddress
                && remoteSystemList[i].isActive)
            {
                remoteSystemList[i].systemAddress.systemIndex = i;
                return i;
            }
        }

        // If no active results found, try previously active results.
        for (i = 0; i < maxConnections; i++)
            if (remoteSystemList[i].systemAddress == systemAddress)
            {
                // speed up find when furtyre
                remoteSystemList[i].systemAddress.systemIndex = i;
                return i;
            }
    }
}

int network_application_t::GetRemoteSystemIndexGeneral(const guid_t& input,
    bool calledFromNetworkThread /*NOT USED*/) const
{
    if (input == JACKIE_NULL_GUID)
        return -1;

    if (input == myGuid)
        return -1;

    if (input.systemIndex != (system_index_t)-1 && input.systemIndex >= 0
        && input.systemIndex < maxConnections
        && remoteSystemList[input.systemIndex].guid == input)
        return input.systemIndex;

    for (system_index_t i = 0; i < maxConnections; i++)
    {
        if (remoteSystemList[i].guid == input)
        {
            // Set the systemIndex so future lookups will be fast
            remoteSystemList[i].guid.systemIndex = i;
            return i;
        }
    }

    return (unsigned int)-1;
}

//bool JackieApplication::IsBanned(NetworkAddress& senderINetAddress)
bool network_application_t::IsBanned(network_address_t& senderINetAddress)
{
    if (banList.Size() == 0)
        return false;

    char IP[65];
    senderINetAddress.ToString(false, IP);
    if (IP == 0 || IP[0] == 0 || IP[0] == ' ' || strlen(IP) > 65)
        return false;

    for (index = 0; index < banList.Size(); index++)
    {
        // timeout and flag it as unused
        if (banList[index]->timeout > 0
            && banList[index]->timeout <= GetTimeMS())
        {
            // remove this expired ban
            Banned* tmp = banList[index];
            banList.RemoveAtIndexFast(index);
            OP_DELETE(tmp, TRACKE_MALLOC);
        }
        else
        {
            uint characterIndex = 0;
            while (1) 	// compare ip char by char
            {
                if (banList[index]->IP[characterIndex] == IP[characterIndex])
                {
                    if (IP[characterIndex] == 0)
                    {
                        return true;
                    }
                    characterIndex++;
                }
                else
                {
                    if (banList[index]->IP[characterIndex] == 0
                        || IP[characterIndex] == 0)
                    {
                        // End of one of the strings
                        break;
                    }

                    // Characters do not match
                    if (banList[index]->IP[characterIndex] == '*')
                    {
                        // Domain is banned.
                        return true;
                    }

                    // Characters do not match and it is not a *
                    break;

                }
            }
            index++;
        }
    }
    // not match found
    return false;
}

void network_application_t::AddToBanList(const char IP[32],
    TimeMS milliseconds /*= 0*/)
{
    std::cout << "AddToBanList " << IP << ", " << milliseconds;
    if (IP == 0 || IP[0] == 0 || strlen(IP) > 32)
        return;

    unsigned index;
    TimeMS time = GetTimeMS();

    // If this guy is already in the ban list, do nothing
    for (index = 0; index < banList.Size(); index++)
    {
        if (strcmp(IP, banList[index]->IP) == 0)
        {
            std::cout << IP
                << " Already in the ban list.  Just update the time and times";
            // Already in the ban list.  Just update the time and banned times
            banList[index]->bannedTImes++;
            if (milliseconds == 0)
                banList[index]->timeout = 0; // Infinite
            else
                banList[index]->timeout = time + milliseconds;
            return;
        }
    }

    // not in the ban list so create a new ban for him
    Banned *banStruct = OP_NEW<Banned>(TRACKE_MALLOC);
    banStruct->firstTimeBanned = time;
    banStruct->bannedTImes = 1;
    if (milliseconds == 0)
        banStruct->timeout = 0; // Infinite
    else
        banStruct->timeout = time + milliseconds;
    strcpy(banStruct->IP, IP);
    banList.InsertAtLast(banStruct);
}

void network_application_t::ban_remote_system(const char IP[32],
    TimeMS milliseconds /*= 0*/)
{
    cmd_t* c = alloc_cmd();
    c->commandID = cmd_t::BCS_ADD_2_BANNED_LIST;
    c->data = 0;
    memcpy(c->arrayparams, IP, strlen(IP) + 1);
    memcpy(c->arrayparams + strlen(IP) + 1, &milliseconds, sizeof(TimeMS));
    //c->data = (char*)jackieMalloc_Ex(strlen(IP) + sizeof(TimeMS) + 1, TRACE_FILE_AND_LINE_);
    //memcpy(c->data, IP, strlen(IP) + 1);
    //memcpy(c->data + strlen(IP) + 1, &milliseconds, sizeof(TimeMS));
    run_cmd(c);
}
