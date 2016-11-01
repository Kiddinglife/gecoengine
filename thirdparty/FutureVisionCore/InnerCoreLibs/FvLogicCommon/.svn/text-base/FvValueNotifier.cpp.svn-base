#include "FvValueNotifier.h"

#include "FvLogicDebug.h"

#include <FvMask.h>
#include <FvMath.h>

//+-----------------------------------------------------------------------------------------------------------------------------

void FvValueNotifierInterface::Notify()
{
	FV_ASSERT_WARNING(GetState() != NONE);
	FV_CHECK_RETURN(GetState() != NONE);
	if(GetState() == MATCH) 
	{
		_OnMatch();
	}
	else 
	{
		_OnNoMatch();
	}
}

//+-----------------------------------------------------------------------------------------------------------------------------
void FvValueRangeNotifier::OnValueUpdated(const FvInt32& oldValue, const FvInt32& newValue, List& kList)
{
	kList.EndIterator();
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		List::iterator iter = kList.GetIterator();
		FvValueRangeNotifier* pkCallback = (*iter).Content();
		FV_ASSERT_ERROR(pkCallback);
		kList.Next();
		pkCallback->OnModified(oldValue, newValue);
	}
}
FvValueRangeNotifier::FvValueRangeNotifier(void)
:m_iMax(0)
,m_iMin(0)
,m_iValue(0)
{

}
void FvValueRangeNotifier::OnModified(const FvInt32 iOldValue, const FvInt32 iNewValue)
{
	m_iValue = iNewValue;
	if(GetState() == NO_MATCH)
	{
		if(IsMatch(iNewValue))
		{
			_SetState(MATCH);
			_OnMatch();
		}
	}
	else
	{
		if(!IsMatch(iNewValue))
		{
			_SetState(NO_MATCH);
			_OnNoMatch();
		}
	}
}
void FvValueRangeNotifier::Init(const FvInt32 iCurrentValue, const FvInt32 iMin, const FvInt32 iMax)
{
	FV_ASSERT_WARNING(iMin <= iMax);
	m_iMin = iMin;
	m_iMax = FvMathTool::Max(iMax, iMin);
	m_iValue = iCurrentValue;
	if(IsMatch(iCurrentValue))
	{
		_SetState(MATCH);
	}
	else
	{
		_SetState(NO_MATCH);
	}
}
void FvValueRangeNotifier::Detach()
{
	FvRefNode1::Detach();
	_SetState(NONE);
}
//+-----------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------
void FvValueMaskNotifier::OnValueUpdated(const FvInt32& oldValue, const FvInt32& newValue, List& kList)
{
	kList.EndIterator();
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		List::iterator iter = kList.GetIterator();
		FvValueMaskNotifier* pkCallback = (*iter).Content();
		FV_ASSERT_ERROR(pkCallback);
		kList.Next();
		pkCallback->OnModified(oldValue, newValue);
	}
}
void FvValueMaskNotifier::Init(const FvUInt32 uiCurState,  const FvUInt32 uiMatchMask)
{
	m_uiMatchMask = uiMatchMask;
	if(IsMatch(uiCurState))
	{
		_SetState(MATCH);
	}
	else
	{
		_SetState(NO_MATCH);
	}
}
void FvValueMaskNotifier::OnModified(const FvUInt32 oldValue, const FvUInt32 newValue)
{
	if(GetState() == NO_MATCH)
	{
		if(IsMatch(newValue))
		{
			_SetState(MATCH);
			_OnMatch();
		}
	}
	else
	{
		if(!IsMatch(newValue))
		{
			_SetState(NO_MATCH);
			_OnNoMatch();
		}
	}
}
void FvValueMaskNotifier::Detach()
{
	FvRefNode1::Detach();
	_SetState(NONE);
}

//+-----------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------

bool FvValueMaskAnyNotifier::IsMatch(const FvUInt32 uiMask)const
{
	return FvMask::HasAny(uiMask, m_uiMatchMask);
}
bool FvValueMaskAllNotifier::IsMatch(const FvUInt32 uiMask)const
{
	return FvMask::HasAll(uiMask, m_uiMatchMask);
}