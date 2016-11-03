
#include "FvGameUnitLogic.h"


#include "FvGameUnitValueInherit.h"
#include "FvValueMapping.h"

#include <FvLogBus.h>



FvInt32 FvGameUnitLogic::ModifyValue32(const FvUInt32 uiIdx, const FvInt32& iDeltaValue)
{
	FvInt32 iOldValue = 0;
	if(Appearance()._GetValue32(uiIdx, iOldValue))
	{
		FvInt32 iNewValue = _Normal32(uiIdx, iOldValue + iDeltaValue);
		_SetValue32(uiIdx, iNewValue);
		Appearance()._GetValue32(uiIdx, iNewValue);
		const FvInt32 iDeltaValueMod = iNewValue - iOldValue;
		//FvLogBus::CritOk("FvCellUnit::ModifyValue32 ValueType[%d] OldValue[%d] -> NewValue[%d]", uiIdx, iOldValue, iNewValue);
		return iDeltaValueMod;
	}
	else
		return 0;
}
FvInt64 FvGameUnitLogic::ModifyValue64(const FvUInt32 uiIdx, const FvInt64& iDeltaValue)
{
	FvInt64 iOldValue = 0;
	if(Appearance()._GetValue64(uiIdx, iOldValue))
	{
		FvInt64 iNewValue = iOldValue + iDeltaValue;
		_SetValue64(uiIdx, iNewValue);
		Appearance()._GetValue64(uiIdx, iNewValue);
		const FvInt64 iDeltaValueMod = iNewValue - iOldValue;
		//FvLogBus::CritOk("FvCellUnit::ModifyValue64 ValueType[%d] OldValue[%d] -> NewValue[%d]", uiIdx, iOldValue, iNewValue);
		return iDeltaValueMod;
	}
	else
		return 0;
}


//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvGameUnitLogic::AttachValue32Mapping(const FvUInt32 uiIdx, FvValueMapping& kMapping)
{
	_ValueMappingList::const_iterator iter = m_kValue32Mappings.find(uiIdx);
	if(iter == m_kValue32Mappings.end())
	{
		FvValueMappingList* pkList = new FvValueMappingList();
		pkList->AttachBack(kMapping);
		m_kValue32Mappings[uiIdx] = pkList;
	}
	else
	{
		FvValueMappingList* pkList = (*iter).second;
		FV_ASSERT_ERROR(pkList);
		pkList->AttachBack(kMapping);
	}
}

FvInt32 FvGameUnitLogic::UpdateValueMapping(const FvUInt32 uiIdx, const FvInt32& iDeltaValue)
{
	_ValueMappingList::const_iterator iter = m_kValue32Mappings.find(uiIdx);
	if(iter == m_kValue32Mappings.end())
	{
		return iDeltaValue;
	}
	FvInt32 iDeltaValueReserve = iDeltaValue;
	FvValueMappingList* pkList = (*iter).second;
	FV_ASSERT_ERROR(pkList);
	FvValueMappingList& kList = *pkList;
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		FvValueMapping* pkMapping = kList.GetIterator()->Content();
		kList.Next();
		FV_ASSERT_ERROR(pkMapping);
		const FvUInt32 uiMappingIdx = pkMapping->Idx();
		const FvInt32 iMappingDeltaValue = pkMapping->Value(iDeltaValueReserve); 
		const FvInt32 iMappingDeltaValueMod = ModifyValue32(uiMappingIdx, iMappingDeltaValue);
		const FvInt32 iSrcDeltaValue = pkMapping->RevertValue(iMappingDeltaValueMod);
		iDeltaValueReserve -= iSrcDeltaValue;
		FvLogBus::CritOk("Mapping Idx[%d] Value[%d] 抵消原属性[%d]", uiMappingIdx, iMappingDeltaValueMod, iSrcDeltaValue);
		if(pkMapping->Value(iDeltaValueReserve) == 0)
		{
			FvLogBus::CritOk("完全抵消");
			kList.EndIterator();
			return 0;
		}
	}
	const FvInt32 iMappingDeltaValueMod = iDeltaValue - iDeltaValueReserve;
	FvLogBus::CritOk("没有完全抵消 抵消[%d]剩余[%d]", iMappingDeltaValueMod, iDeltaValueReserve);
	return iDeltaValueReserve;
}

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvGameUnitLogic::AddAuraStateMask(const FvUInt32 eMask)
{
	for (FvInt32 idx = 0; idx < 31; ++idx)
	{
		if(FvMask::HasAny(eMask, _MASK_(idx)))
		{
			AddAuraState(idx);
		}
	}
}
void FvGameUnitLogic::DelAuraStateMask(const FvUInt32 eMask)
{
	for (FvInt32 idx = 0; idx < 31; ++idx)
	{
		if(FvMask::HasAny(eMask, _MASK_(idx)))
		{
			DelAuraState(idx);
		}
	}
}
void FvGameUnitLogic::AddActionStateMask(const FvUInt32 eMask)
{
	for (FvInt32 idx = 0; idx < 31; ++idx)
	{
		if(FvMask::HasAny(eMask, _MASK_(idx)))
		{
			AddActionState(idx);
		}
	}
}
void FvGameUnitLogic::DelActionStateMask(const FvUInt32 eMask)
{
	for (FvInt32 idx = 0; idx < 31; ++idx)
	{
		if(FvMask::HasAny(eMask, _MASK_(idx)))
		{
			DelActionState(idx);
		}
	}
}
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------