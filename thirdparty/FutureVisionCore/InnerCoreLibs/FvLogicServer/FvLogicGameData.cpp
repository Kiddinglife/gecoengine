#include "FvLogicGameData.h"


FvLogicGameData::FvLogicGameData()
{
	
}
const FvStateCondition* FvLogicGameData::GetStateCondition(const FvUInt32 uiKey)const
{
	std::map<FvUInt32, const FvStateCondition*>::const_iterator iter= m_kStateConditionList.find(uiKey);
	if(iter == m_kStateConditionList.end())
	{
		return NULL;
	}
	return (*iter).second;
}
void FvLogicGameData::AddCondition(const FvUInt32& uiKey, FvStateCondition& kCondition)
{
	std::map<FvUInt32, const FvStateCondition*>::const_iterator iter= m_kStateConditionList.find(uiKey);
	if(iter == m_kStateConditionList.end())
	{
		m_kStateConditionList[uiKey] = &kCondition;
	}
	else
	{
		FvLogBus::Warning("FvLogicGameData::AddCondition ÷ÿ∏¥≤Â»Î");
	}
}