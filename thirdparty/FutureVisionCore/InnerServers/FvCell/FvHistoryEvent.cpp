#include "FvHistoryEvent.h"
#include <../FvBase/FvBaseAppIntInterface.h>



FvHistoryEvent::FvHistoryEvent(FvNetMessageID msgID, FvEventNumber number,
							   const void* msg, int msgLen, Level level,
							   FvEntityID iEntityID,
							   const FvString* pName)
:m_kLevel(level)
,m_kMsgIE(BaseAppIntInterface::EntityMessage)
,m_uiNumber(number)
,m_iMsgLen(msgLen)
,m_iEntityID(iEntityID)
,m_pkName(pName)
{
	m_kMsgIE.SetID(msgID);
	FV_ASSERT(msgLen>=0 && msg);
	if(msgLen)
	{
		m_pcMsg = new char[msgLen];
		memcpy(m_pcMsg, msg, msgLen);
	}
	else
	{
		m_pcMsg = NULL;
	}
}

FvHistoryEvent::FvHistoryEvent(const FvHistoryEvent& kEvent)
:m_kLevel(kEvent.m_kLevel)
,m_kMsgIE(kEvent.m_kMsgIE)
,m_uiNumber(kEvent.m_uiNumber)
,m_iMsgLen(kEvent.m_iMsgLen)
,m_iEntityID(kEvent.m_iEntityID)
,m_pkName(kEvent.m_pkName)
{
	FV_ASSERT(m_iMsgLen>=0 && kEvent.m_pcMsg);
	if(m_iMsgLen)
	{
		m_pcMsg = new char[m_iMsgLen];
		memcpy(m_pcMsg, kEvent.m_pcMsg, m_iMsgLen);
	}
	else
	{
		m_pcMsg = NULL;
	}
}

FvHistoryEvent & FvHistoryEvent::operator=(const FvHistoryEvent& kEvent)
{
	if(m_pcMsg)
	{
		delete [] m_pcMsg;
	}
	else
	{
		FV_ASSERT(m_iMsgLen == 0);
	}

	m_kLevel = kEvent.m_kLevel;
	m_kMsgIE = kEvent.m_kMsgIE;
	m_uiNumber = kEvent.m_uiNumber;
	m_iMsgLen = kEvent.m_iMsgLen;
	m_iEntityID = kEvent.m_iEntityID;
	m_pkName = kEvent.m_pkName;

	FV_ASSERT(m_iMsgLen>=0 && kEvent.m_pcMsg);
	if(m_iMsgLen)
	{
		m_pcMsg = new char[m_iMsgLen];
		memcpy(m_pcMsg, kEvent.m_pcMsg, m_iMsgLen);
	}
	else
	{
		m_pcMsg = NULL;
	}

	return *this;
}


FvEventHistory::FvEventHistory()
:m_iTrimSize(0)
{

}

FvEventHistory::~FvEventHistory()
{
	Clear();
}

void FvEventHistory::Trim()
{
	while(m_kContainer.size() > 100)
	{
		delete *m_kContainer.begin();
		m_kContainer.pop_front();
	}
}

void FvEventHistory::Clear()
{
	const_iterator itrB = begin();
	const_iterator itrE = end();
	while(itrB != itrE)
	{
		delete *itrB;
		++itrB;
	}
	m_kContainer.clear();
}
