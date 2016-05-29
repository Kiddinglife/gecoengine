#include "geco-net-plugin.h"

GECO_NET_BEGIN_NSPACE
network_plugin_t::network_plugin_t()
{
    serverApplication = 0;
#if JackieNet_SUPPORT_PacketizedTCP==1 && JackieNet_SUPPORT_TCPInterface==1
    tcpInterface = 0;
#endif
}
GECO_NET_END_NSPACE