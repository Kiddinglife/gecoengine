#include "FvGameUnitAppearance.h"

#include "FvGameUnitValueInherit.h"
#include "FvValueMapping.h"

#include <FvLogBus.h>

FvGameUnitAppearance::FvGameUnitAppearance(void)
{
}

FvGameUnitAppearance::~FvGameUnitAppearance(void)
{
	
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvGameUnitAppearance::Clear()
{
	/// <清理属性继承>
	for (FvUInt32 uiIdx = 0; uiIdx < m_Value32SrcInheritList.Size(); ++uiIdx)
	{
		m_Value32SrcInheritList[uiIdx].DetachAll(true);
	}
	m_Value32SrcInheritList.Clear();
	/// </清理属性继承>

	m_Value32UpdateListeners.Clear();
	m_Value32RangeListeners.Clear();
	m_Value64UpdateListeners.Clear();
	m_Value32RangeListeners.Clear();

	_ClearReference();
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
FvInt32 FvGameUnitAppearance::GetValue32(const FvRefIdxValue& kValue)const
{
	if(kValue.IsEmpty())
		return 0;
	else
		return FvInt32(GetValue32(kValue.m_iType)*kValue.m_iPercValue*0.01f);
}
FvInt32 FvGameUnitAppearance::GetValue32(const FvUInt32 uiIdx, const FvInt32 iDefaultVlaue /* = 0 */)const
{
	FvInt32 iValue = 0;
	if(_GetValue32(uiIdx, iValue))
	{
		return iValue;
	}
	else
		return iDefaultVlaue;
}
FvInt64 FvGameUnitAppearance::GetValue64(const FvUInt32 uiIdx, const FvInt64 iDefaultVlaue /* = 0 */)const
{
	FvInt64 iValue = 0;
	if(_GetValue64(uiIdx, iValue))
	{
		return iValue;
	}
	else
		return iDefaultVlaue;
}
bool FvGameUnitAppearance::Value32RangeIn(const FvUInt32 uiIdx, const FvInt32 iValueInf, const FvInt32 iValueSup)const
{
	FvInt32 iValue = 0;
	if(_GetValue32(uiIdx, iValue))
	{
		return (iValueInf <= iValue) && (iValue <= iValueSup);
	}
	else
		return true;
}
bool FvGameUnitAppearance::Value64RangeIn(const FvUInt32 uiIdx, const FvInt32 iValueInf, const FvInt32 iValueSup)const
{
	FV_ASSERT_ERROR(0);
	return false;
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvGameUnitAppearance::_OnValue32Updated(const FvUInt32 uiIdx, const FvInt32& iNewValue, const FvInt32& iOldValue)
{
	FV_ASSERT_WARNING(uiIdx < m_Value32UpdateListeners.Size());
	if(uiIdx < m_Value32UpdateListeners.Size())
	{
		FvInt32 oldValue = iOldValue;
		FvValueCallbackInterface<FvInt32>::OnValueUpdated(oldValue, iNewValue, m_Value32UpdateListeners[uiIdx]);
	}
	FV_ASSERT_WARNING(uiIdx < m_Value32RangeListeners.Size());
	if(uiIdx < m_Value32RangeListeners.Size())
	{
		FvInt32 oldValue = iOldValue;
		FvValueRangeNotifier::OnValueUpdated(oldValue, iNewValue, m_Value32RangeListeners[uiIdx]);
	}

	if(uiIdx < m_Value32SrcInheritList.Size())
	{
		m_Value32SrcInheritList[uiIdx].OnUpdate(iOldValue, iNewValue);
	}
}
void FvGameUnitAppearance::_OnValue64Updated(const FvUInt32 uiIdx, const FvInt64& iNewValue, const FvInt64& iOldValue)
{
	FV_ASSERT_WARNING(uiIdx < m_Value64UpdateListeners.Size());
	if(uiIdx < m_Value64UpdateListeners.Size())
	{
		FvInt64 oldValue = iOldValue;
		FvValueCallbackInterface<FvInt64>::OnValueUpdated(oldValue, iNewValue, m_Value64UpdateListeners[uiIdx]);	
	}
}
void FvGameUnitAppearance::_OnMoveStateUpdated(const FvInt32& iNewValue, const FvInt32& iOldValue)
{
	FvValueMaskNotifier::OnValueUpdated(iOldValue, iOldValue, m_MoveStateNotifierList);
	FvGameUnitStateNotifyInterface::OnValueUpdated(*this, m_kStateNotifierList);
}
void FvGameUnitAppearance::_OnActionStateUpdated(const FvInt32& iNewValue, const FvInt32& iOldValue)
{
	FvValueMaskNotifier::OnValueUpdated(iOldValue, iOldValue, m_ActionStateNotifierList);
	FvGameUnitStateNotifyInterface::OnValueUpdated(*this, m_kStateNotifierList);
}
void FvGameUnitAppearance::_OnAuraStateUpdated(const FvInt32& iNewValue, const FvInt32& iOldValue)
{
	FvValueMaskNotifier::OnValueUpdated(iOldValue, iOldValue, m_AuraStateNotifierList);
	FvGameUnitStateNotifyInterface::OnValueUpdated(*this, m_kStateNotifierList);
}
void FvGameUnitAppearance::_OnUnitStateUpdated(const FvInt32& iNewValue, const FvInt32& iOldValue)
{
	FvValueMaskNotifier::OnValueUpdated(iOldValue, iOldValue, m_UnitStateNotifierList);
	FvGameUnitStateNotifyInterface::OnValueUpdated(*this, m_kStateNotifierList);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvGameUnitAppearance::AttachValue32Listen(const FvUInt32 uiIdx, FvValueCallbackInterface<FvInt32>& kCallback)
{
	FV_ASSERT_WARNING(uiIdx < m_Value32UpdateListeners.Size());
	if(uiIdx < m_Value32UpdateListeners.Size())
	{
		kCallback.AttachTo(m_Value32UpdateListeners[uiIdx]);
	}
}
void FvGameUnitAppearance::AttachValue32Listen(const FvUInt32 uiIdx, FvValueRangeNotifier& kCallback)
{
	FV_ASSERT_WARNING(uiIdx < m_Value32RangeListeners.Size());
	if(uiIdx < m_Value32RangeListeners.Size())
	{
		kCallback.AttachTo(m_Value32RangeListeners[uiIdx]);
	}
}
void FvGameUnitAppearance::AttachValue64Listen(const FvUInt32 uiIdx, FvValueCallbackInterface<FvInt64>& kCallback)
{
	FV_ASSERT_WARNING(uiIdx < m_Value64UpdateListeners.Size());
	if(uiIdx < m_Value64UpdateListeners.Size())
	{
		//kCallback.AttachTo(m_Value64UpdateListeners[uiIdx]);
	}
}
void FvGameUnitAppearance::AttachValue32Inherit(const FvUInt32 uiIdx, FvGameUnitValueInherit& kInherit)
{
	FvInt32 iValue = 0;
	if(_GetValue32(uiIdx, iValue))
	{
		FV_ASSERT_ERROR(uiIdx < m_Value32SrcInheritList.Size());
		m_Value32SrcInheritList[uiIdx].AttachBack(kInherit);
	}
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvGameUnitAppearance::AttachAuraStateNotifier(FvValueMaskNotifier& kNotifier)
{
	kNotifier.AttachTo(m_AuraStateNotifierList);
}
void FvGameUnitAppearance::AttachActionStateNotifier(FvValueMaskNotifier& kNotifier)
{
	kNotifier.AttachTo(m_ActionStateNotifierList);
}
void FvGameUnitAppearance::AttachMoveStateNotifier(FvValueMaskNotifier& kNotifier)
{
	kNotifier.AttachTo(m_MoveStateNotifierList);
}
void FvGameUnitAppearance::AttachUnitStateNotifier(FvValueMaskNotifier& kNotifier)
{
	kNotifier.AttachTo(m_UnitStateNotifierList);
}
void FvGameUnitAppearance::AttachStateNotifier(FvRefNode1<FvGameUnitStateNotifyInterface*>& kNotifier)
{
	m_kStateNotifierList.AttachBack(kNotifier);
}
bool FvGameUnitAppearance::IsMatch(const FvStateCondition& kCondition)const
{
	if(kCondition.m_iReqAuraState && !HasAuraState(kCondition.m_iReqAuraState))
	{
		return false;
	}
	if(kCondition.m_iNotAuraState && HasAuraState(kCondition.m_iNotAuraState))
	{
		return false;
	}
	if(kCondition.m_iReqActionState && !HasActionState(kCondition.m_iReqActionState))
	{
		return false;
	}
	if(kCondition.m_iNotActionState && HasActionState(kCondition.m_iNotActionState))
	{
		return false;
	}
	if(kCondition.m_iReqMoveState && !HasMoveState(kCondition.m_iReqMoveState))
	{
		return false;
	}
	if(kCondition.m_iNotMoveState && HasMoveState(kCondition.m_iNotMoveState))
	{
		return false;
	}
	return true;
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvGameUnitAppearance::GetAuraValue(const FvAuraInfo& kInfo, FvAuraValue& kValue)const
{
	for (FvUInt32 uiIdx = 0; uiIdx < FvAuraInfo::VALUE_MAX; ++uiIdx)
	{
		const FvSpellValueSet& kValueSet = kInfo.m_kValueSet[uiIdx];
		kValue.m_kValues[uiIdx].m_iValueBase = GetValue32(kValueSet.m_kCasterRefBase);
		kValue.m_kValues[uiIdx].m_iValueMod = GetValue32(kValueSet.m_kCasterRefMod);
	}
}
void FvGameUnitAppearance::GetHitEffectValue(const FvHitEffectInfo& kInfo, FvEffectValue& kValue)const
{
	const FvSpellValueSet& kValueSet = kInfo.m_kValueSet;
	kValue.m_iValueBase = GetValue32(kValueSet.m_kCasterRefBase);
	kValue.m_iValueMod = GetValue32(kValueSet.m_kCasterRefMod);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------