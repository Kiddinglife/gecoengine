//{future header message}
#ifndef __FvMemberDescription_H__
#define __FvMemberDescription_H__

#include "FvEntityDefDefines.h"

#include <FvWatcher.h>

class FV_ENTITYDEF_API FvMemberDescription
{
public:
	FvMemberDescription();

#if FV_ENABLE_WATCHERS
	static FvWatcherPtr pWatcher();

	void CountSentToOwnClient( FvInt32 bytes ) const
	{
		m_uiSentToOwnClient++;
		m_uiBytesSentToOwnClient += bytes;
	}

	void CountSentToOtherClients( FvInt32 bytes ) const
	{
		m_uiSentToOtherClients++;
		m_uiBytesSentToOtherClients += bytes;
	}

	void CountAddedToHistoryQueue( FvInt32 bytes ) const
	{
		m_uiAddedToHistoryQueue++;
		m_uiBytesAddedToHistoryQueue += bytes;
	}

	void CountSentToGhosts( FvInt32 bytes ) const
	{
		m_uiSentToGhosts++;
		m_uiBytesSentToGhosts += bytes;
	}

	void CountSentToBase( FvInt32 bytes ) const
	{
		m_uiSentToBase++;
		m_uiBytesSentToBase += bytes;
	}

	void CountReceived( FvInt32 bytes ) const
	{
		m_uiReceived++;
		m_uiBytesReceived += bytes;
	}

private:
	mutable FvUInt32 m_uiSentToOwnClient;
	mutable FvUInt32 m_uiSentToOtherClients;
	mutable FvUInt32 m_uiAddedToHistoryQueue;
	mutable FvUInt32 m_uiSentToGhosts;
	mutable FvUInt32 m_uiSentToBase;
	mutable FvUInt32 m_uiReceived;
	mutable FvUInt32 m_uiBytesSentToOwnClient;
	mutable FvUInt32 m_uiBytesSentToOtherClients;
	mutable FvUInt32 m_uiBytesAddedToHistoryQueue;
	mutable FvUInt32 m_uiBytesSentToGhosts;
	mutable FvUInt32 m_uiBytesSentToBase;
	mutable FvUInt32 m_uiBytesReceived;
#endif // FV_ENABLE_WATCHERS
};

#endif // __FvMemberDescription_H__
