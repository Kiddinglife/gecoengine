#include "FvMemberDescription.h"


FvMemberDescription::FvMemberDescription()
#if FV_ENABLE_WATCHERS
:m_uiSentToOwnClient( 0 )
,m_uiSentToOtherClients( 0 )
,m_uiAddedToHistoryQueue( 0 )
,m_uiSentToGhosts( 0 )
,m_uiSentToBase( 0 )
,m_uiReceived( 0 )
,m_uiBytesSentToOwnClient( 0 )
,m_uiBytesSentToOtherClients( 0 )
,m_uiBytesAddedToHistoryQueue( 0 )
,m_uiBytesSentToGhosts( 0 )
,m_uiBytesSentToBase( 0 )
,m_uiBytesReceived( 0 )
#endif
{
}

#if FV_ENABLE_WATCHERS
FvWatcherPtr FvMemberDescription::pWatcher()
{
	static FvWatcherPtr watchMe = NULL;

	if (!watchMe)
	{
		watchMe = new FvDirectoryWatcher();
		FvMemberDescription * pNull = NULL;

		watchMe->AddChild( "messagesSentToOwnClient", 
						   MakeWatcher( pNull->m_uiSentToOwnClient ));
		watchMe->AddChild( "messagesSentToOtherClients", 
						   MakeWatcher( pNull->m_uiSentToOtherClients ));
		watchMe->AddChild( "messagesAddedToHistoryQueue", 
						   MakeWatcher( pNull->m_uiAddedToHistoryQueue ));
		watchMe->AddChild( "messagesSentToGhosts", 
						   MakeWatcher( pNull->m_uiSentToGhosts ));
		watchMe->AddChild( "messagesSentToBase", 
						   MakeWatcher( pNull->m_uiSentToBase ));
		watchMe->AddChild( "bytesSentToOwnClient", 
						   MakeWatcher( pNull->m_uiBytesSentToOwnClient ));
		watchMe->AddChild( "messagesReceived", 
						   MakeWatcher( pNull->m_uiReceived ));
		watchMe->AddChild( "bytesSentToOtherClients", 
						   MakeWatcher( pNull->m_uiBytesSentToOtherClients ));
		watchMe->AddChild( "bytesAddedToHistoryQueue", 
						   MakeWatcher( pNull->m_uiBytesAddedToHistoryQueue ));
		watchMe->AddChild( "bytesSentToGhosts", 
						   MakeWatcher( pNull->m_uiBytesSentToGhosts ));
		watchMe->AddChild( "bytesSentToBase", 
						   MakeWatcher( pNull->m_uiBytesSentToBase ));
		watchMe->AddChild( "bytesReceived", 
						   MakeWatcher( pNull->m_uiBytesReceived ));
	}
	return watchMe;
}

#endif
