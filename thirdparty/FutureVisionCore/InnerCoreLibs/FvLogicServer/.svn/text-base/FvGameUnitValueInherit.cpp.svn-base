#include "FvGameUnitValueInherit.h"
#include "FvGameUnitLogic.h"


FvGameUnitValueInherit::FvGameUnitValueInherit(void)
:m_rpTarget(NULL)
,m_uiValueIdx(0)
{
	FV_MEM_TRACK;
}

FvGameUnitValueInherit::~FvGameUnitValueInherit(void)
{
}
void FvGameUnitValueInherit::SetTarget(FvGameUnitLogic& kTarget, const FvUInt32 uiValueIdx)
{
	m_rpTarget = &kTarget;
	m_uiValueIdx = uiValueIdx;
}
void FvGameUnitValueInherit::_OnUpdated(const FvInt32& oldInheritValue, const FvInt32& newInheritValue)
{
	FV_ASSERT_ERROR(m_rpTarget && "绝对不能是NULL");
	const FvInt32 iDeltaValue = newInheritValue - oldInheritValue;
	m_rpTarget->ModifyValue32(m_uiValueIdx, iDeltaValue);
}
void FvGameUnitValueInherit::_OnAttach(const bool bUpdate)
{
	if(bUpdate)
	{
		FV_ASSERT_ERROR(m_rpTarget && "绝对不能是NULL");
		const FvInt32 iDeltaValue = GetInheritValue();
		m_rpTarget->ModifyValue32(m_uiValueIdx, iDeltaValue);
	}
}
void FvGameUnitValueInherit::_OnDetach(const bool bUpdate)
{
	if(bUpdate)
	{
		FV_ASSERT_ERROR(m_rpTarget && "绝对不能是NULL");
		const FvInt32 iDeltaValue = GetInheritValue();
		m_rpTarget->ModifyValue32(m_uiValueIdx, -iDeltaValue);
	}
}

FvGameUnitValueModifier::FvGameUnitValueModifier(void)
:m_iDeltaValue(0)
,m_uiValueIdx(0)
,m_bAttached(false)
{}
FvGameUnitValueModifier::~FvGameUnitValueModifier(void)
{
	FV_ASSERT_ERROR(m_bAttached == false && "FvGameUnitValueModifier必须清理");
}
void FvGameUnitValueModifier::Detach(FvGameUnitLogic& kTarget, const bool bUpdate)
{
	if(m_bAttached && bUpdate)
	{
		kTarget.ModifyValue32(m_uiValueIdx, -m_iDeltaValue);
	}		
	m_bAttached = false;
}
void FvGameUnitValueModifier::SetModValue(FvGameUnitLogic& kTarget, const FvInt32 iDeltaValue)
{
	if(m_bAttached)
	{
		kTarget.ModifyValue32(m_uiValueIdx, -m_iDeltaValue);
		m_iDeltaValue = iDeltaValue;
		kTarget.ModifyValue32(m_uiValueIdx, m_iDeltaValue);
		return;
	}
	m_iDeltaValue = iDeltaValue;
}
void FvGameUnitValueModifier::Attach(FvGameUnitLogic& kTarget, const FvUInt32 uiValueIdx, const bool bUpdate)
{
	if(m_bAttached && bUpdate)
	{
		kTarget.ModifyValue32(m_uiValueIdx, -m_iDeltaValue);
	}
	m_uiValueIdx = FvUInt16(uiValueIdx);
	m_bAttached = true;
	if(bUpdate)
	{
		kTarget.ModifyValue32(m_uiValueIdx, m_iDeltaValue);
	}
}
