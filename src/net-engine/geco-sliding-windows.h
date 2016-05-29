#ifndef JackieSlidingWindows_h__
#define JackieSlidingWindows_h__
/*
http://www.ssfnet.org/Exchange/tcp/tcpTutorialNotes.html

cwnd=max bytes allowed on wire at once

Start:
cwnd=mtu
ssthresh=unlimited

Slow start:
On ack cwnd*=2

congestion avoidance:
On ack during new period
cwnd+=mtu*mtu/cwnd

on loss or duplicate ack during period:
sshtresh=cwnd/2
cwnd=MTU
This reenters slow start

If cwnd < ssthresh, then use slow start
else use congestion avoidance
*/
#include "geco-basic-type.h"
#include "geco-time.h"
#include "geco-net-type.h"
#include "JackieArraryQueue.h"

const  ushort UDP_HEADER_SIZE = 28; ///IP HEADER 20 + UDP HEADER 8 = 28 BYTES

GECO_NET_BEGIN_NSPACE
class JackieSlidingWindows
{
    public:
    JackieSlidingWindows();
    ~JackieSlidingWindows();
};
GECO_NET_END_NSPACE

#endif // JackieSlidingWindows_h__

