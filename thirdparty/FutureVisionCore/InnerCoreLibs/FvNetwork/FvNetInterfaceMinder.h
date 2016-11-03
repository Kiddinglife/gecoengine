//{future header message}
#ifndef __FvNetInterfaceMinder_H__
#define __FvNetInterfaceMinder_H__

#include "FvNetwork.h"
#include "FvNetTypes.h"
#include "FvNetEndpoint.h"
#include "FvNetMachineGuard.h"
#include "FvNetInterfaceElement.h"

#include <FvBinaryStream.h>
#include <FvTimestamp.h>

#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

class FvNetBundle;
class FvNetInputMessageHandler;
class FvNetNub;
class FvNetPacket;

class FvNetChannel;

class FV_NETWORK_API FvNetInterfaceMinder
{
public:
	FvNetInterfaceMinder( const char * name );

	FvNetInterfaceElement &Add( const char * name, FvInt8 lengthStyle,
			int lengthParam, FvNetInputMessageHandler * pHandler = NULL );

	FvNetInputMessageHandler *handler( int index );
	void handler( int index, FvNetInputMessageHandler * pHandler );
	const FvNetInterfaceElement & InterfaceElement( FvUInt8 id ) const;

	void RegisterWithNub( FvNetNub & nub );
	FvNetReason RegisterWithMachined( FvNetNub & nub, int id ) const;

private:
	FvNetInterfaceElements m_kElements;
	const char *m_pcName;
};

#include "FvNetInterfaceMinder.inl"

#endif // __FvNetInterfaceMinder_H__
