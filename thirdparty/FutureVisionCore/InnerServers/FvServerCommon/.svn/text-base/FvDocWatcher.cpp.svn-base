#include "FvDocWatcher.h"

#include "FvServerConfig.h"
#include "FvServerResource.h"

////////////#define FV_ENABLE_WATCHERS 1
#if FV_ENABLE_WATCHERS

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT( 0 )

namespace FvWatcherDoc
{
	FvXMLSectionPtr s_spDocSection = NULL;

	void InitWatcherDoc( const FvString & appName )
	{
		FvString watchdoc = "watcherdoc/" + appName + ".xml";
		s_spDocSection = FvServerResource::Instance().OpenSection( watchdoc );
	}

	void SetWatcherDoc( FvWatcherPtr pWatcher, const FvString & path )
	{
		if (s_spDocSection && pWatcher)
		{
			FvString comment = s_spDocSection->ReadString( path );
			if (!comment.empty())
				pWatcher->SetComment( comment );
		}
	}
}

#endif // FV_ENABLE_WATCHERS
