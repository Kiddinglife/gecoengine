//{future header message}
#ifndef __FvNetKeepAliveChannels_H__
#define __FvNetKeepAliveChannels_H__

#include "FvNetwork.h"
#include "FvNetMonitoredChannels.h"

class FV_NETWORK_API FvNetKeepAliveChannels : public FvNetMonitoredChannels
{
public:
	void AddIfNecessary( FvNetChannel & channel );

protected:
	iterator & ChannelIter( FvNetChannel & channel ) const;
	float DefaultPeriod() const;

private:
	virtual int HandleTimeout( FvNetTimerID, void * );
};

#endif // __FvNetKeepAliveChannels_H__
