#include "FvHitEffect.h"
#include "FvGameUnitAppearance.h"

#include <FvDestroyManager.h>
#include <FvLogBus.h>
#include <FvGameTimer.h>
#include <FvGameRandom.h>
#include "FvLogicGameData.h"

FvInt32 FvHitEffect::GetCost(const FvGameUnitAppearance& kObj, const FvIdxValueRange& kCost, const FvRefIdxValue& kRef0Cost, const FvRefIdxValue& kRef1Cost)
{
	FvInt32 iValue = FvGameRandom::GetValue(kCost.m_iValueInf, kCost.m_iValueSup);
	iValue += kObj.GetValue32(kRef0Cost);
	iValue += kObj.GetValue32(kRef1Cost);
	return iValue;
}
//+-------------------------------------------------------------------------------------------------------------------------------
FvHitEffect::FvHitEffect()
:m_rpInfo(NULL)
,m_uiSpellId(0)
,m_uiEffectIdx(0)
#pragma warning(push)
#pragma warning(disable: 4355)
,m_kEffectTimeEvent(*this)
#pragma warning(pop)
,m_eState(START)
{
}
FvHitEffect::~FvHitEffect(void)
{
}
FvHitEffect::_State FvHitEffect::_GetState()const
{
	return m_eState;
}
void FvHitEffect::_SetState(const _State eState)
{
	m_eState = eState;
}
const FvHitEffectInfo& FvHitEffect::GetInfo()const
{
	FV_ASSERT_ERROR(m_rpInfo);
	return *m_rpInfo;
}
bool FvHitEffect::GetMiscValue(const char* pcName, FvInt32& iValue)const
{
	return GetInfo().m_kMiscValue.GetValue(pcName, iValue);
}
FvInt32 FvHitEffect::MiscValue(const char* pcName, const FvInt32 iDefaultValue/* = 0*/)const
{
	return GetInfo().m_kMiscValue.Value(pcName, iDefaultValue);
}
void FvHitEffect::OnEffect(FvTimeEventNodeInterface& kNode)
{
	_Handle();
	End();
}

void FvHitEffect::SetHanldeTime(const FvUInt32 iDeltaMS)
{
	FvGameTimer::SetTime(iDeltaMS, m_kEffectTimeEvent);
}
void FvHitEffect::End()
{
	if(_GetState() == START)
	{
		_SetState(END);
		if(m_kEffectTimeEvent.IsAttach())
		{
			m_kEffectTimeEvent.Detach();
			_Handle();
		}
		_OnEnd();
		FvDestroyManager::Destroy(*this);		
	}
}
void FvHitEffect::SetSpellId(const FvSpellID uiSpell, const FvSpellEffectIdx uiEffectIdx)
{
	m_uiSpellId = uiSpell;
	m_uiEffectIdx = uiEffectIdx;
}
FvSpellID FvHitEffect::SpellId()const
{
	return m_uiSpellId;
}
FvSpellEffectIdx FvHitEffect::EffectIdx()const
{
	return m_uiEffectIdx;
}