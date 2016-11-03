#include "FvNetBundle.h"


FV_INLINE
FvHistoryEvent::~FvHistoryEvent()
{
	if(m_pcMsg)
		delete [] m_pcMsg;
}


FV_INLINE
FvEventNumber FvHistoryEvent::Number() const
{
	return m_uiNumber;
}


FV_INLINE
void FvHistoryEvent::AddToBundle(FvNetBundle& bundle)
{
	bundle.StartMessage(m_kMsgIE);
	bundle << m_iEntityID;
	if(m_pcMsg)
	{
		FV_ASSERT(m_iMsgLen > 0);
		bundle.AddBlob(m_pcMsg, m_iMsgLen);
	}
}


FV_INLINE
bool FvHistoryEvent::IsStateChange() const
{
	return (m_kMsgIE.GetID() & 0x80) == 0x80;
}


FV_INLINE
bool FvHistoryEvent::ShouldSend(float threshold, int detailLevel) const
{
	return this->IsStateChange() ?
		(detailLevel <= m_kLevel.m_uiDetail) :
		(threshold < m_kLevel.m_fPriority);
}


FV_INLINE
void FvEventHistory::Add(FvHistoryEvent* pEvent)
{
	m_kContainer.push_back(pEvent);
}




