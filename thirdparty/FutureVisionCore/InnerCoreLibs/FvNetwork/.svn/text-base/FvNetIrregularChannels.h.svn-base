//{future header message}
#ifndef __FvNetIrregularChannels_H__
#define __FvNetIrregularChannels_H__

#include "FvNetwork.h"
#include "FvNetMonitoredChannels.h"

class FV_NETWORK_API FvNetIrregularChannels : public FvNetMonitoredChannels
{
public:
	void AddIfNecessary( FvNetChannel & channel );

protected:
	iterator & ChannelIter( FvNetChannel & channel ) const;
	float DefaultPeriod() const;

private:
	virtual int HandleTimeout( FvNetTimerID, void * );
};

#endif // __FvNetIrregularChannels_H__
