#include "network_socket_t.h"
#include "geco-wsa-singleton.h"
#include "geco_application.h"

GECO_NET_BEGIN_NSPACE

const char* socket_binding_result_to_str(socket_binding_result_t reason)
{
    const char* JISBindResultStrings[4] =
    {
        "JISBindResult_SUCCESS",
        "JISBindResult_REQUIRES_NET_SUPPORT_IPV6_DEFINED",
        "JISBindResult_FAILED_BIND_SOCKET",
        "JISBindResult_FAILED_SEND_TEST"
    };

    unsigned int index = reason;

    if (index < (sizeof(JISBindResultStrings) / sizeof(char*)))
    {
        return JISBindResultStrings[index];
    }

    return "JISBindResult_UNKNOWN";
}
const char* network_socket_type_to_str(network_socket_type_t reason)
{
    const char* const JISTypeStrings[9] =
    {
        "JISType_WINDOWS_STORE_8",
        "JISType_PS3",
        "JISType_PS4",
        "JISType_CHROME",
        "JISType_VITA",
        "JISType_XBOX_360",
        "JISType_XBOX_720",
        "JISType_WINDOWS",
        "JISType_LINUX"
    };
    unsigned int index = reason;

    if (index < (sizeof(JISTypeStrings) / sizeof(char*)))
    {
        return JISTypeStrings[index];
    }

    return "JISType_UNKNOWN";
}

/////////////////////////////// JISAllocator starts /////////////////////////////////
inline network_socket_t* network_socket_alloc_t::AllocJIS(void)
{
    network_socket_t* s2 = 0;
#if defined(WINDOWS_STORE_RT)
    s2 = geco::ultils::OP_NEW<JISWINSTROE8>(TRACKE_MALLOC);
    if(s2 != 0) s2->SetSocketType(JISType_WINDOWS_STORE_8);
#elif defined(__native_client__)
    s2 = geco::ultils::OP_NEW<JISNativeClient>(TRACKE_MALLOC);
    s2->SetSocketType(RNS2T_CHROME);
#elif defined(_WIN32)
    s2 = geco::ultils::OP_NEW<berkley_socket_t>(TRACKE_MALLOC);
    if (s2 != 0) s2->SetSocketType(JISType_WINDOWS);
#else
    s2 = geco::ultils::OP_NEW<berkley_socket_t>(TRACKE_MALLOC);
    if(s2 != 0) s2->SetSocketType(JISType_LINUX);
#endif
    return s2;
}
inline void network_socket_alloc_t::DeallocJIS(network_socket_t *s)
{
    geco::ultils::OP_DELETE(s, TRACKE_MALLOC);
}
//////////////////////////////// JISAllocator ends  ///////////////////////

/////////////////////////////// GECO_INet_Socket Implementations /////////////////////////////////
void network_socket_t::GetMyIP(network_address_t addresses[MAX_COUNT_LOCAL_IP_ADDR])
{
#if defined(WINDOWS_STORE_RT)
    JISWINSTROE8::GetMyIP(addresses);
#elif defined(__native_client__)
    RNS2_NativeClient::GetMyIP(addresses);
#elif defined(_WIN32)
    berkley_socket_t::GetMyIPBerkley(addresses);
#else /// linux-like system
    berkley_socket_t::GetMyIPBerkley(addresses);
#endif
}
void network_socket_t::Print(void)
{
    const char* addrStr = boundAddress.ToString();
    const char* socketTypeStr = network_socket_type_to_str(socketType);
    printf("GECO_INet_Socket::virtual print():: socketType(%s), userConnectionSocketIndex(%d),\n boundAddress(%s)", addrStr, socketTypeStr, userConnectionSocketIndex);
}
/////////////////////////////// GECO_INet_Socket Implementations /////////////////////////////////

#if defined (WINDOWS_STORE_RT) 	/// We are using WINDOWS_STORE_RT plateform
//@TODO
#elif defined (__native_client__)  	/// We are using NaCI plateform
//@TODO
# else 	/// We are using Wins or LINUX or Unix plateform

#if defined(__APPLE__)
/// This C routine is called by CFSocket when there's data waiting on our 
/// UDP socket.  It just redirects the call to Objective-C code.
static void SocketReadCallback(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *data, void *info)
{}
#endif

////////////////////////////// JISBerkley implementations ////////////////////////////
berkley_socket_t::berkley_socket_t()
{
    WSAStartupSingleton::AddRef();
    rns2Socket = (socket_fd_t)INVALID_SOCKET;
    jst = 0;
}
berkley_socket_t::~berkley_socket_t()
{
    WSAStartupSingleton::Deref();
    if (rns2Socket != INVALID_SOCKET)
    {
        closesocket__(rns2Socket);
        rns2Socket = (socket_fd_t)INVALID_SOCKET;
    }
}

bool berkley_socket_t::IsPortInUse(unsigned short port, const char *hostAddress,
        unsigned short addressFamily, int type)
{
    berkley_socket_binding_params_t bbp =
    {
        port, //unsigned short port;
        (char*)hostAddress,//char *hostAddress;
        addressFamily,//unsigned short addressFamily; // AF_INET or AF_INET6
        type,//int type; // SOCK_DGRAM
        0,//int protocol; // 0
        false,//bool nonBlockingSocket;
        false,//int setBroadcast;
        false,//int setIPHdrIncl;
        false,//int doNotFragment;
        0,//int pollingThreadPriority;
        0,//JISEventHandler *eventHandler;
        0//unsigned short remotePortJackieNetWasStartedOn_PS3_PS4_PSP2;
    };

    network_address_t boundAddress;
    berkley_socket_t *rns2 = (berkley_socket_t*)network_socket_alloc_t::AllocJIS();
    socket_binding_result_t bindResult = rns2->BindSharedIPV4And6(&bbp, TRACKE_MALLOC);
    network_socket_alloc_t::DeallocJIS(rns2);
    return bindResult == JISBindResult_FAILED_BIND_SOCKET;
}

//////////////////////////////////////////////////////////////////////////
socket_binding_result_t berkley_socket_t::Bind(berkley_socket_binding_params_t *bindParameters,
        const char *file, unsigned int line)
{
    socket_binding_result_t bindResult = BindShared(bindParameters, file, line);
    /// we do not test bindResult == JISBindResult_FAILED_SEND_TEST here 
    while (bindResult == JISBindResult_FAILED_BIND_SOCKET)
    {
        // Sometimes windows will fail if the socket is recreated too quickly
        GecoSleep(100);
        bindResult = BindShared(bindParameters, file, line);
    }
    return bindResult;
}
socket_binding_result_t berkley_socket_t::BindShared(berkley_socket_binding_params_t *bindParameters,
        const char *file, unsigned int line)
{
    socket_binding_result_t br;

#if NET_SUPPORT_IPV6==1
    br = BindSharedIPV4And6(bindParameters, file, line);
#else
    br = BindSharedIPV4(bindParameters, file, line);
#endif

    if (br != JISBindResult_SUCCESS) return br;

    char zero[128] =
    {   0};
    send_params_t sendParams =
    {   (char*)&zero, sizeof(zero), 0, boundAddress, 0};

    Send(&sendParams, TRACKE_MALLOC);
    GecoSleep(10); // make sure data has been delivered into us
    recv_params_t recvParams;
    recvParams.localBoundSocket = this;
    recv_result_t rr = RecvFrom(&recvParams);

    if (rr <= 0 || sendParams.bytesWritten != rr) return JISBindResult_FAILED_SEND_RECV_TEST;

    /// deep-copy @param bindParameters into @mem this->binding
    memcpy(&this->binding, bindParameters, sizeof(berkley_socket_binding_params_t));

    return br;
}
socket_binding_result_t berkley_socket_t::BindSharedIPV4(berkley_socket_binding_params_t *bindParameters,
        const char *file, unsigned int line)
{
    memset(&boundAddress.address.addr4, 0, sizeof(sockaddr_in));
    boundAddress.address.addr4.sin_port = htons(bindParameters->port);
    boundAddress.address.addr4.sin_family = AF_INET;

    if ((rns2Socket = (int)socket__(bindParameters->addressFamily, bindParameters->type, bindParameters->protocol)) == SOCKET_ERROR)
    {
#if defined(_WIN32)
        HLOCAL messageBuffer = 0;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
                (PTSTR)& messageBuffer, 0, 0);
        // I see this hit on XP with IPV6 for some reason
        fwprintf(stderr, L"JISBerkley::BindSharedIPV4()::socket__()::failed with errno code (%d, %ls)\n", GetLastError(), (PCTSTR)LocalLock(messageBuffer));
        LocalFree(messageBuffer);
#elif (defined(__GNUC__) || defined(__GCCXML__) )
        fprintf(stderr, "JISBerkley::BindSharedIPV4()::socket__()::failed with errno code (%d-%s)\n", errno, strerror(errno));
#endif
        return JISBindResult_FAILED_BIND_SOCKET;
    }

    SetSocketOptions();
    SetNonBlockingSocket(bindParameters->isNonBlocking);
    SetBroadcastSocket(bindParameters->isBroadcast);
    SetIPHdrIncl(bindParameters->setIPHdrIncl);

    /// test hostAddress exists and != empty string
    if (bindParameters->hostAddress != 0 && bindParameters->hostAddress[0] != 0)
    {
        boundAddress.address.addr4.sin_addr.s_addr =
        inet_addr__(bindParameters->hostAddress);
    }
    else
    {
        boundAddress.address.addr4.sin_addr.s_addr = INADDR_ANY;
    }

    // bind our address to the socket
    if (bind__(rns2Socket, (struct sockaddr *) &boundAddress.address.addr4, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        SAFE_CLOSE_SOCK(rns2Socket);
        return JISBindResult_FAILED_BIND_SOCKET;
    }

    /// reinit bounfAddress to double check it is correct and fill out some other params
    /// INADDR_ANY will be changed to localhost ip adress
    GetSystemAddressViaJISSocketIPV4(this->rns2Socket, &boundAddress);

    return JISBindResult_SUCCESS;
}
socket_binding_result_t berkley_socket_t::BindSharedIPV4And6(berkley_socket_binding_params_t *bindParameters,
        const char *file, unsigned int line)
{
#if NET_SUPPORT_IPV6 ==1

    int ret = 0;
    struct addrinfo *servinfo = 0, *aip;  // will point to the results

    struct addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));// make sure the struct is empty
    hints.ai_socktype = SOCK_DGRAM;// UDP sockets
    hints.ai_flags = AI_PASSIVE;// fill in my IP for me
    hints.ai_family = bindParameters->addressFamily;

    char portStr[32];
    ::Itoa(bindParameters->port, portStr, 10);

    if( bindParameters->hostAddress != 0 &&
            ( _stricmp(bindParameters->hostAddress, "UNASSIGNED_SYSTEM_ADDRESS") == 0
                    || bindParameters->hostAddress[0] == 0 ) )
    {
        // empty ip adress with port can work on Ubuntu
        getaddrinfo(0, portStr, &hints, &servinfo);
    }
    else if( bindParameters->hostAddress != 0 && bindParameters->hostAddress[0] != 0
            && ( _stricmp(bindParameters->hostAddress, "UNASSIGNED_SYSTEM_ADDRESS") != 0 ) )
    {
        getaddrinfo(bindParameters->hostAddress, portStr, &hints, &servinfo);
    }

    // Try all returned addresses until one works
    for( aip = servinfo; aip != 0; aip = aip->ai_next )
    {
        // Open socket. The address type depends on what
        // getaddrinfo() gave us.
        rns2Socket = socket__(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
        if( rns2Socket == -1 ) return JISBindResult_FAILED_BIND_SOCKET;

        ret = bind__(rns2Socket, aip->ai_addr, (int) aip->ai_addrlen);
        if( ret >= 0 )
        {
            // Is this valid?
            memcpy(&boundAddress.address.addr6, aip->ai_addr, sizeof(boundAddress.address.addr6));

            freeaddrinfo(servinfo);// free the linked-list

            SetSocketOptions();
            SetNonBlockingSocket(bindParameters->isNonBlocking);
            SetBroadcastSocket(bindParameters->isBroadcast);
            SetIPHdrIncl(bindParameters->setIPHdrIncl);

            GetSystemAddressViaJISSocketIPV4And6(rns2Socket, &boundAddress);

            return JISBindResult_SUCCESS;
        }
        else
        {
#if defined(_WIN32)
            if( ( binding.isNonBlocking && GetLastError() != WSAEWOULDBLOCK ) || !binding.isNonBlocking )
            {
                HLOCAL messageBuffer = 0;
                FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL, GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (PTSTR) & messageBuffer, 0, 0);
                // I see this hit on XP with IPV6 for some reason
                fwprintf(stderr, L"JISBerkley::BindSharedIPV4And6()::bind__()::failed with errno code (%d, %ls)\n", GetLastError(), (PCTSTR) LocalLock(messageBuffer));
                LocalFree(messageBuffer);
            }
#elif (defined(__GNUC__) || defined(__GCCXML__) )
            if( ( binding.isNonBlocking && errno != EAGAIN && errno != EWOULDBLOCK ) || !binding.isNonBlocking )
            {
                fprintf_s(stderr, "JISBerkley::BindSharedIPV4And6()::bind__()::failed with errno code (%d-%s)\n", errno, strerror(errno));
            }
#endif
            SAFE_CLOSE_SOCK(this->rns2Socket);
        }
    }

    return JISBindResult_FAILED_BIND_SOCKET;
#else
    return BindSharedIPV4(bindParameters, file, line);
#endif
}
//////////////////////////////////////////////////////////////////////////

inline recv_result_t berkley_socket_t::RecvFrom(recv_params_t *recvFromStruct)
{
    assert(recvFromStruct != 0);

    if (jst != 0)
    {
        return jst->JackieINetRecvFrom(recvFromStruct->data,
                &recvFromStruct->senderINetAddress, false);
    }

#if NET_SUPPORT_IPV6 ==1
    return RecvFromIPV4And6(recvFromStruct);
#else
    return RecvFromIPV4(recvFromStruct);
#endif
}

recv_result_t berkley_socket_t::RecvFromIPV4(recv_params_t *recvFromStruct)
{
    static sockaddr_in sa =
    {   AF_INET, 0, 0};
    static socklen_t sockLen = sizeof(sa);
    static socklen_t* socketlenPtr = (socklen_t*)&sockLen;
    static sockaddr* sockAddrPtr = (sockaddr*)&sa;
    static const int flag = 0;

    TRY_ONE_MORE_TIME:
    recvFromStruct->bytesRead = recvfrom__(this->rns2Socket, recvFromStruct->data, MAXIMUM_MTU_SIZE, flag, sockAddrPtr, socketlenPtr);

    /// there are only two resons for UDP recvfrom() return 0 :
    /// 1. Socket has been soft closed by shutdown() or setting up linear attribute
    /// 2. Receives an empty (0 size) message from remote endpoint 
    /// However, we cannot expect temote endpoint  will never sends 0 length mesage
    /// to us. So, the following lines are commented 
    assert(recvFromStruct->bytesRead != 0);
    if (recvFromStruct->bytesRead == 0)
    {
        char buf[128];
        recvFromStruct->senderINetAddress.ToString(true, buf);
        //JERROR << "RecvFromIPV4():: recv 0 length data from " << buf;
        //fprintf_s(stderr, "ERROR::JISBerkley::RecvFromBlockingIPV4()::recvfrom__()::Got %i bytes from %s\n", recvFromStruct->bytesRead, recvFromStruct->senderINetAddress.ToString().c_str());
    }

    if (recvFromStruct->bytesRead < 0)
    {
        int val = recvFromStruct->bytesRead;
#if defined(_WIN32)
        if ((binding.isNonBlocking && GetLastError() != WSAEWOULDBLOCK) ||
                !binding.isNonBlocking)
        {
            HLOCAL messageBuffer = 0;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (PTSTR)& messageBuffer, 0, 0);
            // I see this hit on XP with IPV6 for some reason
            fwprintf(stderr, L"JISBerkley::RecvFromNonBlockingIPV4()::recvfrom__()::failed with errno code (%d, %ls)\n", GetLastError(), (PCTSTR)LocalLock(messageBuffer));
            LocalFree(messageBuffer);

            /// here you can choose some specific error and handle these errors as you like
            /// here i only handle WSAEMSGSIZE error in
            /// line 1784 void ServerApplication::RunRecvCycleOnce(UInt32 index)
            // other errors will be ignored and retry recvfrom(0 until it succeeds
            // when you recv a datagram that is bigger than recv buffer size you passed to
            /// recvfrom() third paramater, thsi will trigger wsamsgszie error
            if (GetLastError() == WSAEMSGSIZE)
            {
                recvFromStruct->bytesRead = (int)WSAEMSGSIZE;
            }
            else
            {
                goto TRY_ONE_MORE_TIME;
            }
        }
#elif (defined(__GNUC__) || defined(__GCCXML__) )
        if((binding.isNonBlocking && errno != EAGAIN && errno != EWOULDBLOCK) || !binding.isNonBlocking)
        {
            fprintf(stderr, "JISBerkley::RecvFromNonBlockingIPV4()::recvfrom__()::failed with errno code (%d-%s)\n", errno, strerror(errno));
            /// here you can choose some specific error and handle these errors as you like
            /// here i only handle WSAEMSGSIZE error in
            /// line 1784 void ServerApplication::RunRecvCycleOnce(UInt32 index)
            // other errors will be ignored and retry recvfrom(0 until it succeeds
            if (errno == EMSGSIZE)
            {
                recvFromStruct->bytesRead = (int)EMSGSIZE;
            }
            else
            {
                goto TRY_ONE_MORE_TIME;
            }
        }
#endif
        return val; 	/// read failed
    }

    /// fill out the remote endpoint address
    recvFromStruct->timeRead = Get64BitsTimeUS();

    recvFromStruct->senderINetAddress.address.addr4 = sa;
#ifdef _DEBUG
    recvFromStruct->senderINetAddress.debugPort = ntohs(recvFromStruct->senderINetAddress.address.addr4.sin_port);
#endif // _DEBUG

    /// read succeeds
    return recvFromStruct->bytesRead;
}
inline recv_result_t berkley_socket_t::RecvFromIPV4And6(recv_params_t *recvFromStruct)
{
#if  NET_SUPPORT_IPV6==1
    static sockaddr_storage sa =
    {   0};
    static socklen_t sockLen = sizeof(sa);
    static socklen_t* socketlenPtr = (socklen_t*) &sockLen;
    static sockaddr* sockAddrPtr = (sockaddr*) &sa;
    static const int flag = 0;

    recvFromStruct->bytesRead = recvfrom__(rns2Socket, recvFromStruct->data, MAXIMUM_MTU_SIZE, flag, sockAddrPtr, socketlenPtr);

    /// there are only two resons for UDP recvfrom() return 0 :
    /// 1. Socket has been soft closed by shutdown() or setting up linear attribute
    /// 2. Receives an empty (0 size) message from remote endpoint 
    /// However, we cannot expect temote endpoint  will never sends 0 length mesage
    // so the folowing lines of codes are commented
    //if( recvFromStruct->bytesRead == 0 )
    //{
    //	fprintf_s(stderr, "JISBerkley::RecvFromBlockingIPV4And6()::recvfrom__()::Got %i bytes from %s\n", recvFromStruct->bytesRead, recvFromStruct->systemAddress.ToString());
    //}

    if( recvFromStruct->bytesRead < 0 )
    {
#if defined(_WIN32)
        if( ( binding.isNonBlocking && GetLastError() != WSAEWOULDBLOCK ) || !binding.isNonBlocking )
        {
            HLOCAL messageBuffer = 0;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (PTSTR) & messageBuffer, 0, 0);
            // I see this hit on XP with IPV6 for some reason
            fwprintf(stderr, L"JISBerkley::RecvFromNonBlockingIPV4()::recvfrom__()::failed with errno code (%d, %ls)\n", GetLastError(), (PCTSTR) LocalLock(messageBuffer));
            LocalFree(messageBuffer);
        }
#elif (defined(__GNUC__) || defined(__GCCXML__) )
        if( (binding.isNonBlocking && errno != EAGAIN && errno != EWOULDBLOCK) || !binding.isNonBlocking)
        {
            fprintf_s(stderr, "JISBerkley::RecvFromNonBlockingIPV4()::recvfrom__()::failed with errno code (%d-%s)\n", errno, strerror(errno));
        }
#endif
    }

    recvFromStruct->timeRead = Get64BitsTimeUS();

    if (sa.ss_family == AF_INET)
    {
        memcpy(&recvFromStruct->senderINetAddress.address.addr4, (sockaddr_in *)&sa, sizeof(sockaddr_in));
        recvFromStruct->senderINetAddress.debugPort = ntohs(recvFromStruct->senderINetAddress.address.addr4.sin_port);
    }
    else
    {
        memcpy(&recvFromStruct->senderINetAddress.address.addr6, (sockaddr_in6 *)&sa, sizeof(sockaddr_in6));
        recvFromStruct->senderINetAddress.debugPort = ntohs(recvFromStruct->senderINetAddress.address.addr6.sin6_port);
    }

    return recvFromStruct->bytesRead;
#else
    return RecvFromIPV4(recvFromStruct);
#endif
}
//////////////////////////////////////////////////////////////////////////
send_result_t berkley_socket_t::Send(send_params_t *sendParameters,
        const char *file, unsigned int line)
{
    /// we will nevwer send o len data
    assert(sendParameters->data != 0);
    assert(sendParameters->length > 0);

    if (jst != 0)
    {
        return jst->JackieINetSendTo(sendParameters->data,
                sendParameters->length,
                sendParameters->receiverINetAddress);
    }
    else
    {
        return SendWithoutVDP(rns2Socket, sendParameters, file, line);
    }
}

send_result_t berkley_socket_t::SendWithoutVDP(socket_fd_t rns2Socket,
        send_params_t *sendParameters,
        const char *file, unsigned int line)
{
    int len = 0;
    int oldTTL = -1;
    int newTTL = -1;
    socklen_t opLen = sizeof(oldTTL);

    oldTTL = -1;
    if (sendParameters->ttl > 0)
    {
        // Get the current TTL
        if (getsockopt__(rns2Socket,
                        sendParameters->receiverINetAddress.GetIPProtocol(), IP_TTL, (char *)& oldTTL, &opLen) != -1)
        {
            newTTL = sendParameters->ttl;
            setsockopt__(rns2Socket, sendParameters->receiverINetAddress.GetIPProtocol(), IP_TTL, (char *)& newTTL, sizeof(newTTL));
        }
    }

    TRY_ONE_MORE_TIME:
    if (sendParameters->receiverINetAddress.address.addr4.sin_family == AF_INET)
    {
        len = sendto__(rns2Socket, sendParameters->data, sendParameters->length, 0, (const sockaddr*)& sendParameters->receiverINetAddress.address.addr4, sizeof(sockaddr_in));
    }
    else
    {
#if NET_SUPPORT_IPV6 ==1
        len = sendto__(rns2Socket, sendParameters->data, sendParameters->length, 0, (const sockaddr*)& sendParameters->receiverINetAddress.address.addr6, sizeof(sockaddr_in6));
#endif
    }

    /// only when sendParameters->length == 0, sendto() will return 0;
    /// otherwise it will return > 0, or error code of -1
    /// we forbid to send o length data
    ///assert(len != 0);

    if (len < 0)
    {
#if defined(_WIN32)
        DWORD err = GetLastError();
        HLOCAL messageBuffer = 0;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
                (PTSTR)& messageBuffer, 0, 0);
        // I see this hit on XP with IPV6 for some reason
        fwprintf(stderr, L"JISBerkley::RecvFromNonBlockingIPV4()::sendto__()::failed with errno code (%d, %ls)\n",
                GetLastError(), (PCTSTR)LocalLock(messageBuffer));
        LocalFree(messageBuffer);

        /// try again only in two cases"
        /// 1.it is nonblocking socket with WSAEWOULDBLOCK, it is not like a error
        /// 2. other error occurs but it is not WSAEMSGSIZE, we can avoid to send
        /// bigger dagram than max MSGSIZE by when client connects to server
        ///  by Calling ServerApplication::Connect()
        if ((binding.isNonBlocking && errno == WSAEWOULDBLOCK) ||
                err != WSAEMSGSIZE)
        {
            goto TRY_ONE_MORE_TIME;
        }
        sendParameters->bytesWritten = WSAEMSGSIZE;
#elif (defined(__GNUC__) || defined(__GCCXML__) )
        fprintf(stderr,
                "JISBerkley::SendWithoutVDP()::sendto__() failed with errno %i(%s) for char %i and length %i.\n",
                errno, strerror(errno),sendParameters->data[0], sendParameters->length);

        if ((binding.isNonBlocking && errno == EWOULDBLOCK) || errno != EMSGSIZE)
        {
            goto TRY_ONE_MORE_TIME;
        }
        sendParameters->bytesWritten = errno; /// 10040
#endif
        /// send failed sendParameters->bytesWritten records the error number
        /// of EMSGSIZE 10040
        return len;
    }

    if (oldTTL != -1)
    {
        setsockopt__(rns2Socket, sendParameters->receiverINetAddress.GetIPProtocol(), IP_TTL, (char *)& oldTTL, sizeof(oldTTL));
    }

    /// send succeeds
    sendParameters->bytesWritten = len;
    return len;
}

void berkley_socket_t::GetSystemAddressViaJISSocket(socket_fd_t rns2Socket, network_address_t *systemAddressOut)
{
    WSAStartupSingleton::AddRef();
    GetSystemAddressViaJISSocketIPV4And6(rns2Socket, systemAddressOut);
    WSAStartupSingleton::Deref();
}
void berkley_socket_t::GetSystemAddressViaJISSocketIPV4(socket_fd_t rns2Socket,
        network_address_t *systemAddressOut)
{
    static sockaddr_in sa;
    static socklen_t len = sizeof(sa);
    //memset(&sa, 0, sizeof(sockaddr_in));

    if (getsockname__(rns2Socket, (struct sockaddr *)&sa, &len) == SOCKET_ERROR)
    {
#if defined(_WIN32)
        fprintf_s(stderr, "JISBerkley::GetSystemAddressViaJISSocketIPV4()::getsockname__()::failed with errno code (%s)\n", strerror(WSAGetLastError()));
#elif (defined(__GNUC__) || defined(__GCCXML__) )
        fprintf(stderr, "JISBerkley::GetSystemAddressViaJISSocketIPV4()::getsockname__()::failed with errno code (%s)\n", strerror(errno));
#endif
        *systemAddressOut = JACKIE_NULL_ADDRESS;
        return;
    }

    systemAddressOut->SetPortNetworkOrder(sa.sin_port);
    systemAddressOut->address.addr4.sin_addr.s_addr = sa.sin_addr.s_addr;

    if (systemAddressOut->address.addr4.sin_addr.s_addr == INADDR_ANY)
    {
        systemAddressOut->address.addr4.sin_addr.s_addr = inet_addr__("127.0.0.1");
    }
}
void berkley_socket_t::GetSystemAddressViaJISSocketIPV4And6(socket_fd_t rns2Socket,
        network_address_t *systemAddressOut)
{
#if NET_SUPPORT_IPV6 ==1

    socklen_t slen;
    sockaddr_storage ss;
    slen = sizeof(ss);

    if (getsockname__(rns2Socket, (struct sockaddr *)&ss, &slen) == SOCKET_ERROR)
    {
#if defined(_WIN32)
        fprintf_s(stderr, "JISBerkley::GetSystemAddressViaJISSocketIPV4And6()::getsockname__()::failed with errno code (%s)\n", strerror(WSAGetLastError()));
#elif (defined(__GNUC__) || defined(__GCCXML__) )
        fprintf_s(stderr, "JISBerkley::GetSystemAddressViaJISSocketIPV4And6()::getsockname__()::failed with errno code (%s)\n", strerror(errno));
#endif
        *systemAddressOut = GECO_NULL_ADDRESS
        return;
    }

    if( ss.ss_family == AF_INET )
    {
        memcpy(&systemAddressOut->address.addr4, (sockaddr_in *) &ss, sizeof(sockaddr_in));
        systemAddressOut->debugPort = ntohs(systemAddressOut->address.addr4.sin_port);

        unsigned int zero = 0;
        if( memcmp(&systemAddressOut->address.addr4.sin_addr.s_addr, &zero,
                        sizeof(zero)) == 0 )
        systemAddressOut->SetToLoopBack(4);
    }
    else
    {
        memcpy(&systemAddressOut->address.addr6, (sockaddr_in6 *)&ss, sizeof(sockaddr_in6));
        systemAddressOut->debugPort = ntohs(systemAddressOut->address.addr6.sin6_port);

        char zero[16] =
        {   0};
        if (memcmp(&systemAddressOut->address.addr4.sin_addr.s_addr, &zero,
                        sizeof(zero)) == 0)
        systemAddressOut->SetToLoopBack(6);
    }

#else
    GetSystemAddressViaJISSocketIPV4(rns2Socket, systemAddressOut);
#endif
}

void berkley_socket_t::GetMyIPBerkley(network_address_t addresses[MAX_COUNT_LOCAL_IP_ADDR])
{
    WSAStartupSingleton::AddRef();
#if NET_SUPPORT_IPV6 ==1
    GetMyIPBerkleyV4V6(addresses);
#else
    GetMyIPBerkleyV4(addresses);
#endif
    WSAStartupSingleton::Deref();
}
void berkley_socket_t::GetMyIPBerkleyV4(network_address_t addresses[MAX_COUNT_LOCAL_IP_ADDR])
{
    char buf[80]; if (gethostname(buf, 80) == SOCKET_ERROR) return;
    struct hostent *phe = gethostbyname(buf); if (phe == 0) return;

    int idx = 0; for (idx = 0; idx < MAX_COUNT_LOCAL_IP_ADDR; idx++)
    {
        if (phe->h_addr_list[idx] == 0) break;
        memcpy(&addresses[idx].address.addr4.sin_addr, phe->h_addr_list[idx],
                sizeof(in_addr));
    }

    while (idx < MAX_COUNT_LOCAL_IP_ADDR)
    {
        addresses[idx] = JACKIE_NULL_ADDRESS;
        idx++;
    }
}
void berkley_socket_t::GetMyIPBerkleyV4V6(network_address_t addresses[MAX_COUNT_LOCAL_IP_ADDR])
{
    int idx = 0;

    char buf[80];
    assert(gethostname(buf, 80) != -1);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo)); // make sure the struct is empty
    hints.ai_socktype = SOCK_DGRAM;// UDP sockets
    hints.ai_flags = AI_PASSIVE;// fill in my IP for me

    struct addrinfo *servinfo = 0;
    struct addrinfo *aip = 0;// will point to the results
    getaddrinfo(buf, "", &hints, &servinfo);

    for (idx = 0, aip = servinfo; aip != NULL && idx < MAX_COUNT_LOCAL_IP_ADDR;
            aip = aip->ai_next, idx++)
    {
        if (aip->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)aip->ai_addr;
            memcpy(&addresses[idx].address.addr4, ipv4, sizeof(sockaddr_in));
        }
        else
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)aip->ai_addr;
            memcpy(&addresses[idx].address.addr4, ipv6, sizeof(sockaddr_in6));
        }

    }

    freeaddrinfo(servinfo); // free the linked-list

    while (idx < MAX_COUNT_LOCAL_IP_ADDR)
    {
        addresses[idx] = JACKIE_NULL_ADDRESS;
        idx++;
    }
}
/// STATICS

/// @TO-DO
void berkley_socket_t::Print(void)
{
    network_socket_t::Print();
    /// @TO-DO
}
////////////////////////////// JISBerkley implementations ////////////////////////////

#endif

GECO_NET_END_NSPACE
