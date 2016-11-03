#include "FvNetPacketFilter.h"
#include "FvNetChannel.h"

FvNetReason FvNetPacketFilter::Send(
	FvNetNub & nub, const FvNetAddress & addr, FvNetPacket * pPacket )
{
	return nub.BasicSendWithRetries( addr, pPacket );
}

FvNetReason FvNetPacketFilter::Recv(
	FvNetNub & nub, const FvNetAddress & addr, FvNetPacket * pPacket )
{
	return nub.ProcessFilteredPacket( addr, pPacket );
}

