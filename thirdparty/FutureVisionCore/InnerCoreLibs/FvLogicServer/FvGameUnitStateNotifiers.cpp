#include "FvGameUnitStateNotifiers.h"
#include "FvGameUnitAppearance.h"

void FvGameUnitStateNotifyInterface::Init(const FvStateCondition& kCondition, FvGameUnitAppearance& kObj)
{
	kObj.AttachStateNotifier(*this);
	m_kCondition = kCondition;
	if(kObj.IsMatch(m_kCondition))
	{
		_SetState(MATCH);
	}
	else
	{
		_SetState(NO_MATCH);
	}
}
bool FvGameUnitStateNotifyInterface::IsMatch(const FvGameUnitAppearance& kObj)const
{
	return kObj.IsMatch(m_kCondition);
}

void FvGameUnitStateNotifyInterface::OnValueUpdated(const FvGameUnitAppearance& kObj, List& kList)
{
	kList.EndIterator();
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		FvGameUnitStateNotifyInterface* pkCallback = kList.GetIterator()->Content();
		FV_ASSERT(pkCallback);
		kList.Next();
		pkCallback->OnStateModified(kObj);
	}
}

void FvGameUnitStateNotifyInterface::OnStateModified(const FvGameUnitAppearance& kObj)
{
	if(GetState() == NO_MATCH)
	{
		if(kObj.IsMatch(m_kCondition))
		{
			_SetState(MATCH);
			_OnMatch();
		}
	}
	else if(GetState() == MATCH)
	{
		if(!kObj.IsMatch(m_kCondition))
		{
			_SetState(NO_MATCH);
			_OnNoMatch();
		}
	}
}
void FvGameUnitStateNotifyInterface::Detach()
{
	_SetState(NONE);
	FvRefNode1::Detach();
}