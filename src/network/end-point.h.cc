// must include <winsock2.h> before <windows.h> that has been included in "end-point.h"
#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#endif

#include "end-point.h"
GecoNetEndpoint::FrontEndInterfaces GecoNetEndpoint::ms_kFrontEndInterfaces;

#ifdef __linux__
extern "C"
{
/// 32 bit unsigned integer.
#define __u32 uint
/// 8 bit unsigned integer.
#define __u8 uchar
#include <linux/errqueue.h>
}
#include <sys/uio.h>
#include <netinet/ip.h>
#elif defined(_WIN32)//! win32
static bool s_bGetAdaptersAddresses = false;
struct AdapterInfo
{
    eastl::string kName;
    GecoNetAddress uiAddr;
    AdapterInfo(eastl::string& kName_, GecoNetAddress& uiAddr_) :kName(kName_), uiAddr(uiAddr_)
    {}
};
static eastl::vector<AdapterInfo> s_kAdapterInfoList;
void GetAdaptersAddresses();
#else
struct if_nameindex
{
    unsigned int if_index;
    char *if_name;
};

struct if_nameindex *if_nameindex(void)
{
    static struct if_nameindex staticIfList[3] =
    {
        {   1, "eth0"},
        {   2, GECO_NET_LOCALHOSTNAME},
        {   0, 0}};

    return staticIfList;
}

INLINE void if_freenameindex(struct if_nameindex *)
{}
#endif	// !__linux__

GecoNetEndpoint::GecoNetEndpoint(bool useSyncHijack) :
        m_kSocket(NO_SOCKET), m_bShouldSendClose(false)
{
#ifdef _WIN32
    GetAdaptersAddresses();
#endif
}

bool GecoNetEndpoint::GetClosedPort(GecoNetAddress & closedPort)
{
    bool isResultSet = false;

#ifdef __linux__
    //assert( errno == ECONNREFUSED );

    struct sockaddr_in offender;
    offender.sin_family = 0;
    offender.sin_port = 0;
    offender.sin_addr.s_addr = 0;

    struct msghdr errHeader;
    struct iovec errPacket;

    char data[256];
    char control[256];

    errHeader.msg_name = &offender;
    errHeader.msg_namelen = sizeof(offender);
    errHeader.msg_iov = &errPacket;
    errHeader.msg_iovlen = 1;
    errHeader.msg_control = control;
    errHeader.msg_controllen = sizeof(control);
    errHeader.msg_flags = 0; // result only

    errPacket.iov_base = data;
    errPacket.iov_len = sizeof(data);

    int errMsgErr = recvmsg(*this, &errHeader, MSG_ERRQUEUE);
    if (errMsgErr < 0)
    {
        return false;
    }

    struct cmsghdr * ctlHeader;

    for (ctlHeader = CMSG_FIRSTHDR(&errHeader); ctlHeader != NULL; ctlHeader = CMSG_NXTHDR(&errHeader, ctlHeader))
    {
        if (ctlHeader->cmsg_level == SOL_IP && ctlHeader->cmsg_type == IP_RECVERR)
            break;
    }

    if (ctlHeader != NULL)
    {
        struct sock_extended_err * extError = (struct sock_extended_err*) CMSG_DATA(ctlHeader);
        if (errHeader.msg_namelen == 0)
        {
            offender = *(sockaddr_in*) SO_EE_OFFENDER(extError);
            offender.sin_port = 0;
            network_logger()->error<const char*>(
                    "ProcessPendingEvents:Kernel has a bug:recv_msg did not set msg_name.\n");
        }

        closedPort.su.sin.sin_addr.s_addr = offender.sin_addr.s_addr;
        closedPort.su.sin.sin_port = offender.sin_port;

        isResultSet = true;
    }
#endif // __linux__

    return isResultSet;
}

const GecoNetAddStringInterfaces* GecoNetEndpoint::GetInterfaces()
{
#ifdef _WIN32
    if (!s_kAdapterInfoList.empty())
    {
        for (int i = 0; i < (int)s_kAdapterInfoList.size(); ++i)
        {
            ms_kGecoNetAddStringInterfaces[s_kAdapterInfoList[i].uiAddr] = s_kAdapterInfoList[i].kName;
        }
    }
#else

    struct if_nameindex* pIfInfo = if_nameindex();
    if (!pIfInfo)
    {
        printf("Unable to discover network interfaces.\n");
        return NULL;
    }
    int flags = 0;
    struct if_nameindex* pIfInfoCur = pIfInfo;
    GecoNetAddress addr;
    while (pIfInfoCur->if_name)
    {
        flags = 0;
        this->GetInterfaceFlags(pIfInfoCur->if_name, flags);

        if ((flags & IFF_UP) && (flags & IFF_RUNNING))
        {
            if (this->GetInterfaceAddress(pIfInfoCur->if_name, addr) == 0)
            {
                ms_kGecoNetAddStringInterfaces[addr] = pIfInfoCur->if_name;
            }
        }
        ++pIfInfoCur;
    }
    if_freenameindex(pIfInfo);
#endif
    return &ms_kGecoNetAddStringInterfaces;
}

int GecoNetEndpoint::FindDefaultInterface(char * name)
{
#ifndef __linux__

#ifdef _WIN32

    if (!s_kAdapterInfoList.empty())
    {
        strcpy(name, s_kAdapterInfoList[0].kName.c_str());
        return 0;
    }
    else
    {
        return -1;
    }

#else

    strcpy(name, "eth0");
    return 0;

#endif

#else // __linux__
    int ret = -1;

    struct if_nameindex* pIfInfo = if_nameindex();
    if (pIfInfo)
    {
        int flags = 0;
        struct if_nameindex* pIfInfoCur = pIfInfo;
        GecoNetAddress addr;
        while (pIfInfoCur->if_name)
        {
            flags = 0;
            this->GetInterfaceFlags(pIfInfoCur->if_name, flags);

            if ((flags & IFF_UP) && (flags & IFF_RUNNING))
            {
                if (this->GetInterfaceAddress(pIfInfoCur->if_name, addr) == 0)
                {
                    strcpy(name, pIfInfoCur->if_name);
                    ret = 0;
                    // we only stop if it's not a loopback address,
                    // otherwise we continue, hoping to find a better one
                    if (!(flags & IFF_LOOPBACK))
                        break;
                }
            }
            ++pIfInfoCur;
        }
        if_freenameindex(pIfInfo);
    }
    else
    {
        network_logger()->error("GecoNetEndpoint::FindDefaultInterface: "
                "if_nameindex returned NULL ({})", strerror(errno));
    }

    return ret;
#endif // !__linux__
}

int GecoNetEndpoint::FindIndicatedInterface(const char * spec, char * name)
{
    name[0] = 0;

    if (spec == NULL || spec[0] == 0)
        return -1;

    if (strlen(spec) > IFNAMSIZ)
    {
        network_logger()->error("{}, InterfaceName:{} is Too long\n", __FUNCTION__, spec);
        return -1;
    }

    char* slash;
    int netmaskbits = 32;
    GecoNetAddress addr;

    if (slash = const_cast<char *>(strchr(spec, '/')))
    {
        char iftemp[IFNAMSIZ] = { 0 };
        strncpy(iftemp, spec, slash - spec);
        iftemp[slash - spec] = 0;
        bool ok = GecoNetEndpoint::ConvertAddress(addr, iftemp, 0) == 0;

        netmaskbits = atoi(slash + 1);
        ok &= netmaskbits > 0 && netmaskbits <= 32;

        if (!ok)
        {
            network_logger()->error("GecoNetEndpoint::FindIndicatedInterface: "
                    "netmask match {} length {} is not valid.\n", iftemp, slash + 1);
            return -1;
        }
    }
    else if (this->GetInterfaceAddress(spec, addr) == 0)
    {
        strcpy(name, spec);
    }
    else if (GecoNetEndpoint::ConvertAddress(addr, spec) == 0)
    {
        netmaskbits = 32;
    }
    else
    {
        network_logger()->error("GecoNetEndpoint::FindIndicatedInterface: "
                "No interface matching interface spec '{}' found\n", spec);
        return -1;
    }

    if (name[0] == 0)
    {
        int netmaskshift = 32 - netmaskbits;
        u_int32_t netmaskmatch = ntohl(s4addr(&addr.su));
        eastl::vector<eastl::string> interfaceNames;
#ifdef _WIN32
        if (!s_kAdapterInfoList.empty())
        {
            for (int i = 0; i < (int)s_kAdapterInfoList.size(); ++i)
            {
                interfaceNames.push_back(s_kAdapterInfoList[i].kName);
            }
        }
#endif
        eastl::vector<eastl::string>::iterator iter = interfaceNames.begin();
        GecoNetAddress tip;
        while (iter != interfaceNames.end())
        {
            char * currName = (char *) iter->c_str();
            if (this->GetInterfaceAddress(currName, tip) == 0)
            {
                u_int32_t htip = ntohl(s4addr(&tip.su));
                if ((htip >> netmaskshift) == (netmaskmatch >> netmaskshift))
                {
                    strncpy(name, currName, IFNAMSIZ);
                    network_logger()->debug("Endpoint::bind(): found a match address {}\n", name);
                    break;
                }
            }

            ++iter;
        }

        if (name[0] == 0)
        {
            uchar * qik = (uchar*) &addr;
            network_logger()->error("GecoNetEndpoint::FindIndicatedInterface: "
                    "No interface matching netmask spec '%s' found "
                    "(evals to {}.{}.{}.{}:{})\n", spec, qik[0], qik[1], qik[2], qik[3], netmaskbits);

            return -2;
        }
    }

    return 0;
}

#ifdef _WIN32

int GecoNetEndpoint::GetInterfaceFlags(char * name, int & flags)
{
    if (_stricmp(name, GECO_NET_LOCALHOSTNAME))
    {
        flags = IFF_UP | IFF_LOOPBACK | IFF_RUNNING;
    }
    else
    {
        flags = IFF_UP | IFF_BROADCAST | IFF_NOTRAILERS |
        IFF_RUNNING | IFF_MULTICAST;
    }

    return 0;
}

int GecoNetEndpoint::GetInterfaceAddress(const char * name, GecoNetAddress & address)
{
    if (!s_kAdapterInfoList.empty())
    {
        for (int i = 0; i < (int)s_kAdapterInfoList.size(); ++i)
        {
            if (!_stricmp(s_kAdapterInfoList[i].kName.c_str(), name))
            {
                address = s_kAdapterInfoList[i].uiAddr;
                return 0;
            }
        }
    }

    return -1;
}

#endif

int GecoNetEndpoint::ConvertAddress(GecoNetAddress& address, const char * string, ushort port)
{
    GecoNetAddress trial(port, string);
    if (!trial.IsNone())
    {
        address = trial;
        return 0;
    }
    // ok, try looking it up then
    struct hostent * hosts = gethostbyname(string);
    if (hosts != NULL)
    {
        if (hosts->h_addrtype == AF_INET)
        {
            s4addr(&address.su) = *(u_int32_t*) (hosts->h_addr_list[0]);
            saddr_family(&address.su) = AF_INET;
        }
        else if (hosts->h_addrtype == AF_INET6)
        {
            memcpy_fast(s6addr(&address.su), hosts->h_addr_list[0], sizeof(in6_addr));
            saddr_family(&address.su) = AF_INET6;
        }
        else
            network_logger()->critical("GecoNetEndpoint::ConvertAddress(): no such af {}", hosts->h_addrtype);
        return 0;
    }
    return -1;
}

#ifdef __linux__
int GecoNetEndpoint::GetQueueSizes(int & tx, int & rx) const
{
    int ret = -1;

    u_int16_t nport = 0;
    GecoNetAddress addr;
    this->GetLocalAddress(&addr);

    char match[16];
    //geco_snprintf(match, sizeof(match), "%04X", (int) ntohs(addr.su.sa.sa_family));

    FILE * f = fopen("/proc/net/udp", "r");

    if (!f)
    {
        network_logger()->error("GecoNetEndpoint::GetQueueSizes: "
                "could not open /proc/net/udp: {}\n", strerror(errno));
        return -1;
    }

    char aline[256];
    fgets(aline, 256, f);

    while (fgets(aline, 256, f) != NULL)
    {
        if (!strncmp(aline + 4 + 1 + 1 + 8 + 1, match, 4))
        {
            char * start = aline + 4 + 1 + 1 + 8 + 1 + 4 + 1 + 8 + 1 + 4 + 1 + 2 + 1;
            start[8] = 0;
            tx = strtol(start, NULL, 16);

            start += 8 + 1;
            start[8] = 0;
            rx = strtol(start, NULL, 16);

            ret = 0;

            break;
        }
    }

    fclose(f);

    return ret;
}
#else // __linux__
int GecoNetEndpoint::GetQueueSizes(int &, int &) const
{
    return -1;
}
#endif // !__linux__

int GecoNetEndpoint::GetBufferSize(int optname) const
{
#ifdef __linux__
    assert(optname == SO_SNDBUF || optname == SO_RCVBUF);

    int recvbuf = -1;
    socklen_t rbargsize = sizeof(int);
    int rberr = getsockopt(m_kSocket, SOL_SOCKET, optname, (char*) &recvbuf, &rbargsize);

    if (rberr == 0 && rbargsize == sizeof(int))
    {
        return recvbuf;
    }
    else
    {
        network_logger()->error("GecoNetEndpoint::GetBufferSize: "
                "Failed to read option {}: {}\n", optname == SO_SNDBUF ? "SO_SNDBUF" : "SO_RCVBUF", strerror(errno));

        return -1;
    }

#else // __linux__
    return -1;
#endif // !__linux__
}

bool GecoNetEndpoint::SetBufferSize(int optname, int size)
{
#ifdef __linux__
    setsockopt(m_kSocket, SOL_SOCKET, optname, (const char*) &size, sizeof(size));
#else
    //! add by Uman,2009/10/22
    return setsockopt(m_kSocket, SOL_SOCKET, optname, (const char*)&size,
            sizeof(size)) == 0;
#endif // __linux__

    return this->GetBufferSize(optname) >= size;
}

bool GecoNetEndpoint::RecvAll(void * gramData, int gramSize)
{
    while (gramSize > 0)
    {
        int len = this->Recv(gramData, gramSize);

        if (len <= 0)
        {
            if (len == 0)
            {
                network_logger()->warn<const char*>("GecoNetEndpoint::RecvAll: Connection lost\n");
            }
            else
            {
                network_logger()->error("GecoNetEndpoint::RecvAll: Got error '{}'\n", strerror(errno));
            }

            return false;
        }
        gramSize -= len;
        gramData = ((char *) gramData) + len;
    }

    return true;
}

void GecoNetEndpoint::AddFrontEndInterface(const eastl::string & name, GecoNetAddress* addr)
{
    ms_kFrontEndInterfaces[name] = *addr;
}

#ifdef _WIN32

void WCharToSTLStr(WCHAR* pSrc, eastl::string& des)
{
    if (!pSrc)
    return;

    int wLen = (int)wcslen(pSrc);
    int desLen(0);
    desLen = WideCharToMultiByte(CP_ACP, 0, pSrc, wLen, NULL, 0, NULL, NULL);
    des.assign(desLen, 0);
    WideCharToMultiByte(CP_ACP, 0, pSrc, wLen, const_cast<char*>(des.data()), desLen, NULL, NULL);
}

void GetAdaptersAddresses()
{
    if (s_bGetAdaptersAddresses)
    return;
    s_bGetAdaptersAddresses = true;
    s_kAdapterInfoList.clear();

    DWORD dwRetVal = 0;
    PIP_ADAPTER_ADDRESSES pkAddresses = NULL;
    ULONG uiOutBufLen = 15000;
    ULONG uiIterations = 0;

    do
    {
        pkAddresses = (IP_ADAPTER_ADDRESSES *)new char[uiOutBufLen];
        if (!pkAddresses)
        return;

        dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pkAddresses, &uiOutBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW)
        {
            delete[] pkAddresses;
            pkAddresses = NULL;
        }
        else
        {
            break;
        }

        uiIterations++;

    }while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (uiIterations < 3));

    if (dwRetVal != NO_ERROR)
    {
        printf("Call to GetAdaptersAddresses failed with error: %d\n", dwRetVal);

        if (dwRetVal == ERROR_NO_DATA)
        {
            network_logger()->error("No addresses were found in GetAdaptersAddresses\n");
        }
        else
        {
            LPVOID lpMsgBuf = NULL;
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPTSTR)& lpMsgBuf, 0, NULL))
            {
                printf("GetAdaptersAddresses Error: %s\n", lpMsgBuf);
                LocalFree(lpMsgBuf);
            }
        }

        if (pkAddresses)
        delete[](char*)pkAddresses;

        return;
    }

    PIP_ADAPTER_ADDRESSES pkCurrAddresses = pkAddresses;
    while (pkCurrAddresses)
    {
        if (pkCurrAddresses->FirstUnicastAddress)
        {
            eastl::string kFriendlyName;
            WCharToSTLStr(pkCurrAddresses->FriendlyName, kFriendlyName);
            sockaddr* pAddr = pkCurrAddresses->FirstUnicastAddress->Address.lpSockaddr;

            //! 127.0.0.1
            if (IN4ADDR_ISLOOPBACK((sockaddr_in*)pAddr) || IN6ADDR_ISLOOPBACK((sockaddr_in6*)pAddr))
            kFriendlyName = GECO_NET_LOCALHOSTNAME;
            s_kAdapterInfoList.push_back(AdapterInfo(kFriendlyName, GecoNetAddress(pAddr)));
        }

        pkCurrAddresses = pkCurrAddresses->Next;
    }

    //! ����λ��,��һ���ŷ�localhost��ַ
    if (s_kAdapterInfoList.size() > 1)
    {
        if (s_kAdapterInfoList[0].kName == GECO_NET_LOCALHOSTNAME)
        {
            AdapterInfo kTmp = s_kAdapterInfoList.back();
            s_kAdapterInfoList.back() = s_kAdapterInfoList.front();
            s_kAdapterInfoList.front() = kTmp;
        }
    }

    if (pkAddresses)
    delete[](char*)pkAddresses;
}

#endif
