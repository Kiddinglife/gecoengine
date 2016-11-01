//{future header message}
#ifndef __FvNetCondemnedChannels_H__
#define __FvNetCondemnedChannels_H__

#include "FvNetwork.h"
#include "FvNetMisc.h"
#include "FvNetInterfaces.h"

#include <list>
#include <map>

class FvNetChannel;

class FV_NETWORK_API FvNetCondemnedChannels : public FvNetTimerExpiryHandler
{
public:
	FvNetCondemnedChannels() : m_kTimerID( FV_NET_TIMER_ID_NONE ) {}

	void Add( FvNetChannel * pChannel );

	FvNetChannel *Find( FvNetChannelID channelID ) const;

	bool DeleteFinishedChannels();

	int NumCriticalChannels() const;

private:
	static const int AGE_LIMIT = 5;

	virtual int HandleTimeout( FvNetTimerID, void * );
	bool ShouldDelete( FvNetChannel * pChannel, FvUInt64 now );

	typedef std::list< FvNetChannel * > NonIndexedChannels;
	typedef std::map< FvNetChannelID, FvNetChannel * > IndexedChannels;

	NonIndexedChannels m_kNonIndexedChannels;
	IndexedChannels	m_kIndexedChannels;

	FvNetTimerID m_kTimerID;
};

#endif // __FvNetCondemnedChannels_H__
