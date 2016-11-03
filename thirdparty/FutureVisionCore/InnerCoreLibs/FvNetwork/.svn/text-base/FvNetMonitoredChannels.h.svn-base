//{future header message}
#ifndef __FvNetMonitoredChannels_H__
#define __FvNetMonitoredChannels_H__

#include "FvNetwork.h"
#include "FvNetInterfaces.h"

#include <list>

class FvNetChannel;
class FvNetNub;

class FV_NETWORK_API FvNetMonitoredChannels : public FvNetTimerExpiryHandler
{
public:
	typedef std::list< FvNetChannel * > Channels;
	typedef Channels::iterator iterator;

	FvNetMonitoredChannels() : m_fPeriod( 0.f ), m_kTimerID( FV_NET_TIMER_ID_NONE ) {}

	void AddIfNecessary( FvNetChannel & channel );
	void DelIfNecessary( FvNetChannel & channel );
	void SetPeriod( float seconds, FvNetNub & nub );

	void StopMonitoring( FvNetNub & nub );

	iterator End()	{ return m_kChannels.end(); }

protected:

	virtual iterator &ChannelIter( FvNetChannel & channel ) const = 0;

	virtual float DefaultPeriod() const = 0;

	Channels m_kChannels;
	float m_fPeriod;
	FvNetTimerID m_kTimerID;
};

#endif // __FvNetMonitoredChannels_H__
