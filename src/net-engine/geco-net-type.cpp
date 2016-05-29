#include "geco-net-type.h"
#include "geco-wsa-singleton.h"
#include "geco-globals.h"
#include <stdio.h>

using namespace geco::ultils;

GECO_NET_BEGIN_NSPACE

const char *IPV6_LOOPBACK = "::1";
const char *IPV4_LOOPBACK = "127.0.0.1";

#ifndef SWIG
const network_address_t JACKIE_NULL_ADDRESS;
const guid_t JACKIE_NULL_GUID;
#endif

socket_binding_params_t::socket_binding_params_t()
{
#ifdef __native_client__
    blockingSocket=USE_NON_BLOBKING_SOCKET;
#else
    blockingSocket = USE_BLOBKING_SOCKET;
#endif
    port = 0;
    hostAddress[0] = 0;
    remotePortWasStartedOn_PS3_PSP2 = 0;
    extraSocketOptions = 0;
    socketFamily = AF_INET;
    blockingSocket = USE_BLOBKING_SOCKET;
}
socket_binding_params_t::socket_binding_params_t(const char *_hostAddress, ushort _port)
{
#ifdef __native_client__
    blockingSocket = USE_NON_BLOBKING_SOCKET;
#else
    blockingSocket = USE_BLOBKING_SOCKET;
#endif
    remotePortWasStartedOn_PS3_PSP2 = 0;
    port = _port;
    if (_hostAddress != 0) strcpy(hostAddress, _hostAddress);
    hostAddress[0] = 0;
    extraSocketOptions = 0;
    socketFamily = AF_INET;
    blockingSocket = USE_BLOBKING_SOCKET;
}

int network_address_t::size(void)
{
#if NET_SUPPORT_IPV6==1
    return sizeof(sockaddr_in6) + sizeof(char);
#else
    return sizeof(unsigned int) + sizeof(ushort) + sizeof(char);
#endif
}
unsigned int network_address_t::ToHashCode(const network_address_t &sa)
{
    unsigned int lastHash = SuperFastHashIncremental((const char*)& sa.address.addr4.sin_port,
        sizeof(sa.address.addr4.sin_port), sizeof(sa.address.addr4.sin_port));

#if NET_SUPPORT_IPV6==1
    if( sa.address.addr4.sin_family == AF_INET )
        return SuperFastHashIncremental((const char*) & sa.address.addr4.sin_addr.s_addr, 
        sizeof(sa.address.addr4.sin_addr.s_addr), lastHash);
    else
        return SuperFastHashIncremental((const char*) & sa.address.addr6.sin6_addr.s6_addr, 
        sizeof(sa.address.addr6.sin6_addr.s6_addr), lastHash);
#else
    return SuperFastHashIncremental((const char*)& sa.address.addr4.sin_addr.s_addr,
        sizeof(sa.address.addr4.sin_addr.s_addr), lastHash);
#endif
}

network_address_t::network_address_t()
{
    address.addr4.sin_family = AF_INET;
    systemIndex = (system_index_t)-1;
    debugPort = 0;
}
network_address_t::network_address_t(const char *str)
{
    address.addr4.sin_family = AF_INET;
    SetPortHostOrder(0);
    FromString(str);
    systemIndex = (system_index_t)-1;
}
network_address_t::network_address_t(const char *str, ushort port)
{
    address.addr4.sin_family = AF_INET;
    FromString(str, port);
    systemIndex = (system_index_t)-1;
}
network_address_t& network_address_t::operator = (const
    network_address_t& input)
{
    memcpy(&address, &input.address, sizeof(address));
    systemIndex = input.systemIndex;
    debugPort = input.debugPort;
    return *this;
}

bool network_address_t::operator==(const network_address_t& right) const
{
    return (address.addr4.sin_port == right.address.addr4.sin_port) && EqualsExcludingPort(right);
}

bool network_address_t::EqualsExcludingPort(const network_address_t& right) const
{
    return (address.addr4.sin_family == AF_INET && address.addr4.sin_addr.s_addr == right.address.addr4.sin_addr.s_addr)
#if NET_SUPPORT_IPV6==1
        || (address.addr4.sin_family == AF_INET6 && memcmp(address.addr6.sin6_addr.s6_addr, right.address.addr6.sin6_addr.s6_addr, sizeof(address.addr6.sin6_addr.s6_addr)) == 0)
#endif
        ;
}

bool network_address_t::operator!=(const network_address_t& right) const
{
    return (*this == right) == false;
}
bool network_address_t::operator>(const network_address_t& right) const
{
    if (address.addr4.sin_port == right.address.addr4.sin_port)
    {
#if NET_SUPPORT_IPV6 ==1
        if (address.addr4.sin_family == AF_INET)
            return address.addr4.sin_addr.s_addr > right.address.addr4.sin_addr.s_addr;
        return memcmp(address.addr6.sin6_addr.s6_addr, right.address.addr6.sin6_addr.s6_addr, sizeof(address.addr6.sin6_addr.s6_addr)) > 0;
#else
        return address.addr4.sin_addr.s_addr > right.address.addr4.sin_addr.s_addr;
#endif
    }
    return address.addr4.sin_port > right.address.addr4.sin_port;
}
bool network_address_t::operator<(const network_address_t& right) const
{
    if (address.addr4.sin_port == right.address.addr4.sin_port)
    {
#if NET_SUPPORT_IPV6==1
        if (address.addr4.sin_family == AF_INET)
            return address.addr4.sin_addr.s_addr < right.address.addr4.sin_addr.s_addr;
        return memcmp(address.addr6.sin6_addr.s6_addr, right.address.addr6.sin6_addr.s6_addr, sizeof(address.addr6.sin6_addr.s6_addr))>0;
#else
        return address.addr4.sin_addr.s_addr < right.address.addr4.sin_addr.s_addr;
#endif
    }
    return address.addr4.sin_port < right.address.addr4.sin_port;
}

unsigned char network_address_t::GetIPVersion(void) const
{
    if (address.addr4.sin_family == AF_INET)
    {
        return 	4;
    }
    else
    {
        return 6;
    }
}
unsigned char network_address_t::GetIPProtocol(void) const
{
#if NET_SUPPORT_IPV6==1
    if (address.addr4.sin_family == AF_INET)
        return IPPROTO_IP;
    return IPPROTO_IPV6;
#else
    return IPPROTO_IP;
#endif
}

void network_address_t::SetPortHostOrder(ushort s)
{
    address.addr4.sin_port = htons(s);
    debugPort = s;
}
void network_address_t::SetPortNetworkOrder(ushort s)
{
    address.addr4.sin_port = s;
    debugPort = ntohs(s);
}
void network_address_t::SetPortNetworkOrder(const network_address_t& right)
{
    address.addr4.sin_port = right.address.addr4.sin_port;
    debugPort = right.debugPort;
}
ushort network_address_t::GetPortHostOrder(void) const
{
    return ntohs(address.addr4.sin_port);
}
ushort network_address_t::GetPortNetworkOrder(void) const
{
    return address.addr4.sin_port;
}
void network_address_t::SetToLoopBack(void)
{
    SetToLoopBack(4);
}
void network_address_t::SetToLoopBack(unsigned char ipVersion)
{
    if (ipVersion == 4)
    {
        FromString(IPV4_LOOPBACK, '\0', ipVersion);
    }
    else
    {
        FromString(IPV6_LOOPBACK, '\0', ipVersion);
    }
}
bool network_address_t::IsLoopback(void) const
{
    if (GetIPVersion() == 4)
    {
        if (htonl(address.addr4.sin_addr.s_addr) == 2130706433)
            return true;
        if (address.addr4.sin_addr.s_addr == 0)
            return true;
    }
#if NET_SUPPORT_IPV6==1
    else
    {
        const static char localhost[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
        if( memcmp(&address.addr6.sin6_addr, localhost, 16) == 0 )
            return true;
    }
#endif
    return false;
}
bool network_address_t::IsLANAddress(void)
{
#if defined(WIN32)
    return address.addr4.sin_addr.S_un.S_un_b.s_b1 == 10 || address.addr4.sin_addr.S_un.S_un_b.s_b1 == 192;
#else
    return (address.addr4.sin_addr.s_addr >> 24) == 10 ||
        (address.addr4.sin_addr.s_addr >> 24) == 192;
#endif
}

bool network_address_t::SetIP4Address(const char *str, char portDelineator)
{
    if (isDomainIPAddr(str))
    {

#if defined(_WIN32)
        if (_strnicmp(str, "localhost", 9) == 0)
#else
        if (strncasecmp(str, "localhost", 9) == 0)
#endif
        {
            address.addr4.sin_addr.s_addr = inet_addr__("127.0.0.1");

            if (str[9])
            {
                SetPortHostOrder((unsigned short)atoi(str + 9));
            }
            return true;
        }

        char ip[65];
        ip[0] = 0;
        DomainNameToIP(str, ip);
        if (ip[0])
        {
            address.addr4.sin_addr.s_addr = inet_addr__(ip);
        }
        else
        {
            *this = JACKIE_NULL_ADDRESS;
            return false;
        }
    }
    else
    {
        // Split the string into the first part, and the : part
        int index, portIndex;
        char IPPart[22];
        char portPart[10];
        // Only write the valid parts, don't change existing if invalid
        for (index = 0; str[index] && str[index] != portDelineator && index < 22; index++)
        {
            if (str[index] != '.' && (str[index]<'0' || str[index]>'9'))
                break;
            IPPart[index] = str[index];
        }
        IPPart[index] = 0;
        portPart[0] = 0;
        if (str[index] && str[index + 1])
        {
            index++;
            for (portIndex = 0; portIndex < 10 && str[index] && index < 22 + 10; index++, portIndex++)
            {
                if (str[index]<'0' || str[index]>'9')
                    break;

                portPart[portIndex] = str[index];
            }
            portPart[portIndex] = 0;
        }

        if (IPPart[0])
        {
            address.addr4.sin_addr.s_addr = inet_addr__(IPPart);
        }

        if (portPart[0])
        {
            address.addr4.sin_port = htons((unsigned short)atoi(portPart));
            debugPort = ntohs(address.addr4.sin_port);
        }
    }
    return true;
}
bool network_address_t::FromString(const char *str, char portDelineator,
    unsigned char ipVersion)
{
#if NET_SUPPORT_IPV6 != 1
    return SetIP4Address(str, portDelineator);
#else
    if (str == 0)
    {
        memset(&address, 0, sizeof(address));
        address.addr4.sin_family = AF_INET;
        return true;
    }

#if NET_SUPPORT_IPV6 ==1
    char ipPart[INET6_ADDRSTRLEN];
#else
    char ipPart[INET_ADDRSTRLEN];
#endif

    char portPart[32];
    int i = 0, j;

    // TODO - what about 255.255.255.255?
    if( ipVersion == 4 && strcmp(str, IPV6_LOOPBACK) == 0 )
    {
        strcpy(ipPart, IPV4_LOOPBACK);
    } else if( ipVersion == 6 && strcmp(str, IPV4_LOOPBACK) == 0 )
    {
        address.addr4.sin_family = AF_INET6;
        strcpy(ipPart, IPV6_LOOPBACK);
    } else if( isDomainIPAddr(str) == false )
    {
        for( ; i < sizeof(ipPart) && str[i] != 0 && str[i] != portDelineator; i++ )
        {
            if( ( str[i]<'0' || str[i]>'9' ) && ( str[i]<'a' || str[i]>'f' ) && ( str[i]<'A' || str[i]>'F' ) && str[i] != '.' && str[i] != ':' && str[i] != '%' && str[i] != '-' && str[i] != '/' )
                break;

            ipPart[i] = str[i];
        }
        ipPart[i] = 0;
    } else
    {
        strncpy(ipPart, str, sizeof(ipPart));
        ipPart[sizeof(ipPart) - 1] = 0;
    }

    j = 0;
    if( str[i] == portDelineator && portDelineator != 0 )
    {
        i++;
        for( ; j < sizeof(portPart) && str[i] != 0; i++, j++ )
        {
            portPart[j] = str[i];
        }
    }
    portPart[j] = 0;

    // needed for getaddrinfo
    WSAStartupSingleton::AddRef();

    // This could be a domain, or a printable address such as "192.0.2.1" or "2001:db8:63b3:1::3490"
    // I want to convert it to its binary representation
    addrinfo hints, *servinfo = 0;
    memset(&hints, 0, sizeof hints);
    hints.ai_socktype = SOCK_DGRAM;
    if( ipVersion == 6 )
        hints.ai_family = AF_INET6;
    else if( ipVersion == 4 )
        hints.ai_family = AF_INET;
    else
        hints.ai_family = AF_UNSPEC;
    getaddrinfo(ipPart, "", &hints, &servinfo);
    if( servinfo == 0 )
    {
        if( ipVersion == 6 )
        {
            ipVersion = 4;
            hints.ai_family = AF_UNSPEC;
            getaddrinfo(ipPart, "", &hints, &servinfo);
            if( servinfo == 0 )
                return false;
        } else
            return false;
    }

    JACKIE_ASSERT(servinfo);

    unsigned short oldPort = address.addr4.sin_port;
#if NET_SUPPORT_IPV6 ==1
    if( servinfo->ai_family == AF_INET )
    {
        address.addr4.sin_family = AF_INET;
        memcpy(&address.addr4, ( struct sockaddr_in * )servinfo->ai_addr, sizeof(struct sockaddr_in));
    } else
    {
        address.addr4.sin_family = AF_INET6;
        memcpy(&address.addr6, ( struct sockaddr_in6 * )servinfo->ai_addr, sizeof(struct sockaddr_in6));
    }
#else
    address.addr4.sin_family = AF_INET;
    memcpy(&address.addr4, ( struct sockaddr_in * )servinfo->ai_addr, sizeof(struct sockaddr_in));
#endif

    freeaddrinfo(servinfo); // free the linked list

    // needed for getaddrinfo
    WSAStartupSingleton::Deref();

    // PORT
    if( portPart[0] )
    {
        address.addr4.sin_port = htons((unsigned short) atoi(portPart));
        debugPort = ntohs(address.addr4.sin_port);
    } else
    {
        address.addr4.sin_port = oldPort;
    }
#endif // #if NET_SUPPORT_IPV6!=1
    return true;
}
bool network_address_t::FromString(const char *str, ushort port, unsigned char ipVersion)
{
    bool b = FromString(str, '\0', ipVersion);
    if (b == false)
    {
        *this = JACKIE_NULL_ADDRESS;
        return false;
    }
    address.addr4.sin_port = htons(port);
    debugPort = ntohs(address.addr4.sin_port);
    return true;
}

const char* network_address_t::ToString(bool writePort, char portDelineator) const
{
    unsigned char strIndex = 0;

#if NET_SUPPORT_IPV6 == 1
    static char str[8][INET6_ADDRSTRLEN + 5 + 1];
#else
    static char str[8][22 + 5 + 1];
#endif

    unsigned char lastStrIndex = strIndex;
    strIndex++;
    ToString(writePort, str[lastStrIndex & 7], portDelineator);
    return (char*)str[lastStrIndex & 7];
}
void network_address_t::ToString(bool writePort, char *dest, char portDelineator) const
{
#if NET_SUPPORT_IPV6 !=1
    ToString_IPV4(writePort, dest, portDelineator);
#else
    ToString_IPV6(writePort, dest, portDelineator);
#endif // #if RAKNET_SUPPORT_IPV6!=1
}
void network_address_t::ToString_IPV4(bool writePort, char *dest, char portDelineator)
const
{
    if (*this == JACKIE_NULL_ADDRESS)
    {
        strcpy(dest, "UNASSIGNED_SYSTEM_ADDRESS");
        return;
    }

    char portStr[2];
    portStr[0] = portDelineator;
    portStr[1] = 0;

    in_addr in;
    in.s_addr = address.addr4.sin_addr.s_addr;
    const char *ntoaStr = inet_ntoa(in);
    strcpy(dest, ntoaStr);
    if (writePort)
    {
        strcat(dest, portStr);
        Itoa(GetPortHostOrder(), dest + strlen(dest), 10);
    }
}
void network_address_t::ToString_IPV6(bool writePort, char *dest, char portDelineator)
const
{
    int ret;
    (void)ret;

    if (*this == JACKIE_NULL_ADDRESS)
    {
        strcpy(dest, "UNASSIGNED_SYSTEM_ADDRESS");
        return;
    }

    if (address.addr4.sin_family == AF_INET)
    {
        ret = getnameinfo((struct sockaddr *) &address.addr4, sizeof(struct sockaddr_in), dest, 22, NULL, 0, NI_NUMERICHOST);
    }
    else
    {
#if NET_SUPPORT_IPV6 ==1
        ret = getnameinfo((struct sockaddr *) &address.addr6, sizeof(struct sockaddr_in6), dest, INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);
#endif
    }
    if (ret != 0)
    {
        dest[0] = 0;
    }

    if (writePort)
    {
        unsigned char ch[2];
        ch[0] = portDelineator;
        ch[1] = 0;
        strcat(dest, (const char*)ch);
        Itoa(ntohs(address.addr4.sin_port), dest + strlen(dest), 10);
    }
}

guid_t::guid_t(ulonglong _g)
{
    g = _g;
    systemIndex = (system_index_t)-1;
}
guid_t::guid_t()
{
    systemIndex = (system_index_t)-1;
    g = (ulonglong)-1;
}
guid_t& guid_t::operator = (const guid_t& input)
{
    g = input.g;
    systemIndex = input.systemIndex;
    return *this;
}

inline bool guid_t::operator==(const guid_t& right) const
{
    return g == right.g;
}
inline bool guid_t::operator!=(const guid_t& right) const
{
    return g != right.g;
}
inline bool guid_t::operator > (const guid_t& right) const
{
    return g > right.g;
}
inline bool guid_t::operator < (const guid_t& right) const
{
    return g < right.g;
}

const char *guid_t::ToString(void) const
{
    static unsigned char strIndex = 0;
    static char str[8][64];

    unsigned char lastStrIndex = strIndex;
    strIndex++;
    ToString(str[lastStrIndex & 7]);
    return (char*)str[lastStrIndex & 7];
}
void guid_t::ToString(char *dest) const
{
    if (*this == JACKIE_NULL_GUID)
        strcpy(dest, "JACKIE_INet_GUID_Null");
    else
        //sprintf(dest, "%u.%u.%u.%u.%u.%u", g[0], g[1], g[2], g[3], g[4], g[5]);
        sprintf(dest, "%" PRINTF_64BITS_MODIFIER "u", (long long unsigned int) g);
    // sprintf(dest, "%u.%u.%u.%u.%u.%u", g[0], g[1], g[2], g[3], g[4], g[5]);
}
unsigned long guid_t::ToUInt32(const guid_t &g)
{
    return ((unsigned long)(g.g >> 32)) ^ ((unsigned long)(g.g & 0xFFFFFFFF));
}
int guid_t::size() { return (int) sizeof(ulonglong); }
bool guid_t::FromString(const char *source)
{
    if (source == 0)
        return false;

#if   defined(WIN32)
    g = _strtoui64(source, NULL, 10);
#else
    // Changed from g=strtoull(source,0,10); for android
    g = strtoull(source, (char **)NULL, 10);
#endif

    return true;
}

guid_address_wrapper_t::guid_address_wrapper_t(const network_packet_t& packet)
{
    guid = packet.guid;
    systemAddress = packet.systemAddress;
}
unsigned long guid_address_wrapper_t::ToHashCode(const guid_address_wrapper_t &aog)
{
    if (aog.guid != JACKIE_NULL_GUID)
        return guid_t::ToUInt32(aog.guid);
    return network_address_t::ToHashCode(aog.systemAddress);
}
const char *guid_address_wrapper_t::ToString(bool writePort) const
{
    if (guid != JACKIE_NULL_GUID) return guid.ToString();
    return systemAddress.ToString(writePort);
}
void guid_address_wrapper_t::ToString(bool writePort, char *dest) const
{
    if (guid != JACKIE_NULL_GUID) return guid.ToString(dest);
    return systemAddress.ToString(writePort, dest);
}

/// Every platform except windows store 8 and native client supports Berkley sockets
#if !defined(WINDOWS_STORE_RT)
void DomainNameToIP_Berkley_IPV4And6(const char *domainName,
    char ip[65])
{
#if NET_SUPPORT_IPV6 == 1
    struct addrinfo hints, *res, *p;
    int status;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_DGRAM;

    WSAStartupSingleton::AddRef();
    if( ( status = getaddrinfo(domainName, NULL, &hints, &res) ) != 0 )
    {
        memset(ip, 0, 65 * sizeof(char));
        WSAStartupSingleton::Deref();
        return;
    }
    WSAStartupSingleton::Deref();

    p = res;
    // 	for(p = res;p != NULL; p = p->ai_next) {
    void *addr;
    //		char *ipver;

    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if( p->ai_family == AF_INET )
    {
        struct sockaddr_in *ipv4 = ( struct sockaddr_in * )p->ai_addr;
        addr = &( ipv4->sin_addr );
        strcpy(ip, inet_ntoa(ipv4->sin_addr));
    } else
    {
        // TODO - test
        struct sockaddr_in6 *ipv6 = ( struct sockaddr_in6 * )p->ai_addr;
        addr = &( ipv6->sin6_addr );
        // inet_ntop function does not exist on windows
        // http://www.mail-archive.com/users@ipv6.org/msg02107.html
        getnameinfo(( struct sockaddr * )ipv6, sizeof(struct sockaddr_in6), ip, 1, NULL, 0, NI_NUMERICHOST);
    }
    freeaddrinfo(res); // free the linked list
    //	}
#else
    DomainNameToIP_Berkley_IPV4(domainName, ip);
#endif // #if NET_SUPPORT_IPV6==1
}
void DomainNameToIP_Berkley_IPV4(const char *domainName,
    char ip[65])
{
    static struct in_addr addr;
    memset(&addr, 0, sizeof(in_addr));

    WSAStartupSingleton::AddRef();
    // Use inet_addr instead? What is the difference?
    struct hostent * phe = gethostbyname(domainName);
    WSAStartupSingleton::Deref();

    if (phe == 0 || phe->h_addr_list[0] == 0)
    {
        printf("Yow! Bad host lookup.\n");
        memset(ip, 0, 65 * sizeof(char));
        return;
    }

    if (phe->h_addr_list[0] == 0)
    {
        memset(ip, 0, 65 * sizeof(char));
        return;
    }

    memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
    strcpy(ip, inet_ntoa(addr));
}
#else
void DomainNameToIP_Non_Berkley(const char *domainName, char ip[65])
{
    //to-do
}
#endif ///  !defined(WINDOWS_STORE_RT)

bool isDomainIPAddr(const char *host)
{
    unsigned int i = 0;
    while (host[i])
    {
        // IPV4: natpunch.jenkinssoftware.com
        // IPV6: fe80::7c:31f7:fec4:27de%14
        if ((host[i] >= 'g' && host[i] <= 'z') ||
            (host[i] >= 'A' && host[i] <= 'Z'))
            return true;
        ++i;
    }
    return false;
}

GECO_NET_END_NSPACE
