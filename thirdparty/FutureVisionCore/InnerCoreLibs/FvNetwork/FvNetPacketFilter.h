//{future header message}
#ifndef __FvNetPacketFilter_H__
#define __FvNetPacketFilter_H__

#include "FvNetwork.h"
#include "FvNetMisc.h"

#include <FvSmartPointer.h>

#include <string>

class FvNetChannel;
class FvNetNub;
class FvNetPacket;
class FvNetPacketFilter;

class FV_NETWORK_API FvNetPacketFilter : public FvSafeReferenceCount
{
public:
	virtual ~FvNetPacketFilter() {}

	virtual FvNetReason Send( FvNetNub & nub, const FvNetAddress & addr, FvNetPacket * pPacket );

	virtual FvNetReason Recv( FvNetNub & nub, const FvNetAddress & addr, FvNetPacket * pPacket );

	virtual int MaxSpareSize() { return 0; }
};

typedef FvSmartPointer< FvNetPacketFilter > FvNetPacketFilterPtr;

#endif // __FvNetPacketFilter_H__
