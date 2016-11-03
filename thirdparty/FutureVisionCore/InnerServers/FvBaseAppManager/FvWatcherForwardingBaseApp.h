//{future header message}
#ifndef __FvWatcherForwardingBaseApp_H__
#define __FvWatcherForwardingBaseApp_H__
/**
#include <FvNetTypes.h>
#include <FvWatcher.h>

class WatcherPathRequestV2;
class ForwardingCollector;


class FvBaseAppForwardingWatcher : public ForwardingWatcher
{
public:
	ForwardingCollector *newCollector(
		WatcherPathRequestV2 & pathRequest,
		const std::string & destWatcher, const std::string & targetInfo );


	AddressList * getComponentAddressList( const std::string & targetInfo );
};
**/
#endif//__FvWatcherForwardingBaseApp_H__
