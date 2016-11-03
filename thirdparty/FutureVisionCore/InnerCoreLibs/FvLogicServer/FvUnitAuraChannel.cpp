#include "FvUnitAuraChannel.h"

#include "FvUnitAura.h"

FvUnitAura* FvUnitAuraChannel::GetTop()const
{
	if(m_kList.IsEmpty())
	{
		return NULL;
	}
	else
	{
		List::Iterator iter = m_kList.GetHead();
		FvUnitAura* pkAura = iter->m_Content;
		FV_ASSERT(pkAura);
		return pkAura;
	}
}
void FvUnitAuraChannel::Attach(FvDoubleLinkNode2<FvUnitAura*>& kNode, const bool bUpdate)
{
	FV_ASSERT_ERROR(kNode.m_Content);
	if(m_kList.IsEmpty())
	{
		m_kList.PushBack(kNode);
		kNode.m_Content->OnActiveByChannel(bUpdate);
	}
	else
	{
		List::Iterator iter = m_kList.GetHead();
		FvUnitAura* pkOldTop = iter->m_Content;
		while (!m_kList.IsEnd(iter))
		{
			FvUnitAura* pkAura = iter->m_Content;
			FV_ASSERT(pkAura);
			if(kNode.m_Content->ChannelWeight() > pkAura->ChannelWeight())
			{
				break;
			}
			List::Next(iter);
		}
		m_kList.PushBefore(*iter, kNode);
		if(&kNode == m_kList.GetHead())
		{
			pkOldTop->OnDeactiveByChannel(bUpdate);
			kNode.m_Content->OnActiveByChannel(bUpdate);
		}
	}
}

void FvUnitAuraChannel::Detach(FvDoubleLinkNode2<FvUnitAura*>& kNode, const bool bUpdate)
{
	if(kNode.IsAttach() == false)
	{
		return;
	}
	if(m_kList.IsEmpty())
	{
		return;
	}
	if(&kNode == m_kList.GetHead())
	{
		kNode.Detach();
		kNode.m_Content->OnDeactiveByChannel(bUpdate);
		if(!m_kList.IsEmpty())
		{
			FvUnitAura* pNewTop = m_kList.GetHead()->m_Content;
			FV_ASSERT(pNewTop);
			pNewTop->OnActiveByChannel(bUpdate);
		}
	}
	else
	{
		kNode.Detach();
	}
}

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FvUnitAuraChannelList::FvUnitAuraChannelList()
{

}
FvUnitAuraChannelList::~FvUnitAuraChannelList()
{
	for (std::map<FvUInt32, FvUnitAuraChannel*>::iterator iter = m_kUnitAuraChannelList.begin(); iter != m_kUnitAuraChannelList.end(); ++iter)
	{
		FvUnitAuraChannel* pkChannel = (*iter).second;
		FV_ASSERT_ERROR(pkChannel);
		delete pkChannel;
	}
	m_kUnitAuraChannelList.clear();
}

void FvUnitAuraChannelList::Attach(const FvUInt32 uiChannel, FvDoubleLinkNode2<FvUnitAura*>& kNode, const bool bUpdate)
{
	std::map<FvUInt32, FvUnitAuraChannel*>::iterator iter = m_kUnitAuraChannelList.find(uiChannel);
	if(iter == m_kUnitAuraChannelList.end())
	{
		FvUnitAuraChannel* pkChannel = new FvUnitAuraChannel();
		m_kUnitAuraChannelList[uiChannel] = pkChannel;
		pkChannel->Attach(kNode, bUpdate);
	}
	else
	{
		FvUnitAuraChannel* pkChannel = (*iter).second;
		pkChannel->Attach(kNode, bUpdate);
	}
}
void FvUnitAuraChannelList::Detach(const FvUInt32 uiChannel, FvDoubleLinkNode2<FvUnitAura*>& kNode, const bool bUpdate)
{
	FvUnitAuraChannel* pkChannel = GetChannel(uiChannel);
	if(pkChannel)
	{
		pkChannel->Detach(kNode, bUpdate);
	}
}

FvUnitAuraChannel* FvUnitAuraChannelList::GetChannel(const FvUInt32 uiChannel)const
{
	std::map<FvUInt32, FvUnitAuraChannel*>::const_iterator iter = m_kUnitAuraChannelList.find(uiChannel);
	if(iter == m_kUnitAuraChannelList.end())
	{
		return NULL;
	}
	FvUnitAuraChannel* pkChannel = (*iter).second;
	return pkChannel;
}
bool FvUnitAuraChannelList::IsTop(const FvUnitAura& kAura, const FvUInt32 uiChannel)const
{
	FvUnitAuraChannel* pkChannel = GetChannel(uiChannel);
	if(pkChannel == NULL)
	{
		return false;
	}
	return (pkChannel->GetTop() == &kAura);
}