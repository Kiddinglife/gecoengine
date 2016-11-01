////#define FV_ENABLE_WATCHERS 1
#if FV_ENABLE_WATCHERS

#include "FvNetWatcherGlue.h"

#include <FvWatcher.h>
#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 )

FV_SINGLETON_STORAGE( FvNetWatcherGlue )

#ifdef _WIN32
	#pragma warning (push)
	#pragma warning (disable: 4355)
#endif

FvNetWatcherGlue::FvNetWatcherGlue(FvUInt16 uiMachinePort) :
	FvNetWatcherNub(uiMachinePort), m_kHandler( *this )
{
	this->SetRequestHandler(&m_kHandler);
}
#ifdef _WIN32
	#pragma warning (pop)
#endif

FvNetWatcherGlue::~FvNetWatcherGlue()
{
}

int FvNetWatcherGlue::HandleInputNotification( int fd )
{
	if (fd != this->GetSocketDescriptor())
	{
		FV_ERROR_MSG( "FvNetWatcherGlue::HandleInputNotification: Got unexpected fd %d!\n", fd );
		return 0;
	}

	FvNetWatcherGlue::Instance().ReceiveRequest();

	return 0;
}

#endif // FV_ENABLE_WATCHERS

