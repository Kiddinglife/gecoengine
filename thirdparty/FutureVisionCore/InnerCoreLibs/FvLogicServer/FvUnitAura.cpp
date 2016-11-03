#include "FvUnitAura.h"
#include "FvGameUnitLogic.h"
#include "FvLogicGameData.h"

#include <FvLogBus.h>

//+------------------------------------------------------------------------------------------------------------------------------------------
FvUnitAura::FvUnitAura()
:FvAura()
,m_pkTarget(NULL)
{
	m_kTargetAttachNode.m_Content = this;
	m_kSpellFlowAttachNode.m_Content = this;
	m_kChannelNode.m_Content = this;
}
FvUnitAura::~FvUnitAura(void)
{

}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitAura::SetTarget(FvGameUnitLogic& kTarget)
{
	m_pkTarget = &kTarget;
	kTarget._OnAddAura(m_kTargetAttachNode);
	const FvStateCondition* pkEffectStateCondition = EffectStateCondition();
	if(pkEffectStateCondition && !pkEffectStateCondition->IsEmtpy())
	{
		m_opEffectStateConditionNotifiers = new EffectStateNotifiers(*this);
		m_opEffectStateConditionNotifiers->Init(*pkEffectStateCondition, kTarget.Appearance());
	}
	const FvAuraInfo::ValueRangeArray* pkEffectValueCondition = EffectValueCondition();
	if(pkEffectValueCondition)
	{
		m_opEffectValueConditionNotifiers = new EffectValueNotifiers(*this);
		m_opEffectValueConditionNotifiers->Init(*pkEffectValueCondition, kTarget.Appearance());
	}
	const FvStateCondition* pkEndStateCondition = EndStateCondition();
	if(pkEndStateCondition && !pkEndStateCondition->IsEmtpy())
	{
		m_opEndStateConditionNotifiers = new EndStateNotifiers(*this);
		m_opEndStateConditionNotifiers->Init(*pkEndStateCondition, kTarget.Appearance());
	}
	const FvAuraInfo::ValueRangeArray* pkEndValueCondition = EndValueCondition();
	if(pkEndValueCondition)
	{
		m_opEndValueConditionNotifiers = new EndValueNotifiers(*this);
		m_opEndValueConditionNotifiers->Init(*pkEndValueCondition, kTarget.Appearance());
	}
}
FvGameUnitLogic& FvUnitAura::Target()const
{
	FV_ASSERT_ERROR(m_pkTarget);
	return *m_pkTarget;
}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitAura::_OnStart()
{
	if(GetInfo().m_iCloseEffectChannel)
	{
		Target().CloseAuraByChannel(GetInfo().m_iCloseEffectChannel);
	}
	if(GetChannelID())
	{
		Target().AuraChannels().Attach(GetChannelID(), m_kChannelNode, true);/// 通道中会Active
	}
	else
	{
		Active(true);
	}
	Target()._OnAddAura(m_kTargetAttachNode);
}
void FvUnitAura::_OnEnd()
{
	Target().OnAuraEnd(*this);
	if(GetChannelID())
	{
		Target().AuraChannels().Detach(GetChannelID(), m_kChannelNode, true);/// 通道中会Deactive
	}
	else
	{
		Deactive(true);
	}

	m_kTargetAttachNode.Detach();
	m_kSpellFlowAttachNode.Detach();

	m_opEffectStateConditionNotifiers = NULL;
	m_opEffectValueConditionNotifiers = NULL;
	m_opEndStateConditionNotifiers = NULL;
	m_opEndValueConditionNotifiers = NULL;
}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitAura::_OnLoad(const bool bUpdate)
{
	if(GetChannelID())
	{
		Target().AuraChannels().Attach(GetChannelID(), m_kChannelNode, bUpdate);
	}
	else
	{
		Active(bUpdate);
	}
	Target()._OnAddAura(m_kTargetAttachNode);
}
void FvUnitAura::_OnSave(const bool bUpdate)
{
	Target().OnAuraEnd(*this);
	if(GetChannelID())
	{
		Target().AuraChannels().Detach(GetChannelID(), m_kChannelNode, bUpdate);
	}
	else
	{
		Deactive(bUpdate);
	}

	m_kTargetAttachNode.Detach();
	m_kSpellFlowAttachNode.Detach();

	m_opEffectStateConditionNotifiers = NULL;
	m_opEffectValueConditionNotifiers = NULL;
	m_opEndStateConditionNotifiers = NULL;
	m_opEndValueConditionNotifiers = NULL;
}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitAura::Active(const bool bUpdate)
{
	if(IsAttributeMatch() == false)
	{
		return;
	}
	if(IsTopChannel() == false)
	{
		return;
	}
	if(HasState(FvAuraState::ACTIVE))
	{
		return;
	}
	_AddState(FvAuraState::ACTIVE);
	_Active(bUpdate);
}
void FvUnitAura::Deactive(const bool bUpdate)
{
	if(!HasState(FvAuraState::ACTIVE))
	{
		return;
	}
	_DelState(FvAuraState::ACTIVE);
	_Deactive(bUpdate);
}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitAura::_Active(const bool bUpdate)
{
	FvLogBus::CritOk("FvUnitAura::_Active");
	if(GetInfo().m_iActionStateMask)
	{
		FvLogBus::CritOk("AddActionStateMask[%X]", GetInfo().m_iActionStateMask);
		Target().AddActionStateMask(GetInfo().m_iActionStateMask);
	}
	if(GetInfo().m_iAuraStateMask)
	{
		FvLogBus::CritOk("AddAuraStateMask[%X]", GetInfo().m_iAuraStateMask);
		Target().AddAuraStateMask(GetInfo().m_iAuraStateMask);
	}
	_OnActive(bUpdate);
	Target().OnAuraActive(*this);
}
void FvUnitAura::_Deactive(const bool bUpdate)
{
	FvLogBus::CritOk("FvUnitAura::_Deactive");
	if(GetInfo().m_iActionStateMask)
	{
		FvLogBus::CritOk("DelActionStateMask[%X]", GetInfo().m_iActionStateMask);
		Target().DelActionStateMask(GetInfo().m_iActionStateMask);
	}
	if(GetInfo().m_iAuraStateMask)
	{
		FvLogBus::CritOk("DelAuraStateMask[%X]", GetInfo().m_iAuraStateMask);
		Target().DelAuraStateMask(GetInfo().m_iAuraStateMask);
	}
	_OnDeactive(bUpdate);
	Target().OnAuraDeactive(*this);
}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitAura::OnActiveByChannel(const bool bUpdate)
{
	FvLogBus::CritOk("FvUnitAura::OnActiveByChannel");
	Active(bUpdate);
}
void FvUnitAura::OnDeactiveByChannel(const bool bUpdate)
{
	FvLogBus::CritOk("FvUnitAura::OnDeactiveByChannel");
	Deactive(bUpdate);
}
FvUInt32 FvUnitAura::ChannelWeight()const
{
	return GetInfo().m_iEffectChannelWeight;
}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitAura::AttachToSpellFlow(FvRefList<FvUnitAura*>& kList)
{
	kList.AttachBack(m_kSpellFlowAttachNode);
}
void FvUnitAura::AttachToTarget(FvRefList<FvUnitAura*>& kList)
{
	kList.AttachBack(m_kTargetAttachNode);
}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitAura::_OnEffectMatch()
{
	Active(true);
}
void FvUnitAura::_OnEffectUnMatch()
{
	Deactive(true);
}
void FvUnitAura::_OnEndMatch()
{
	End();
}
void FvUnitAura::_OnEndUnMatch()
{

}
//+------------------------------------------------------------------------------------------------------------------------------------------
bool FvUnitAura::IsTopChannel()const
{
	if(GetChannelID() == 0)
	{
		return true;
	}
	else
	{
		return Target().AuraChannels().IsTop(*this, GetChannelID());
	}
}
//+------------------------------------------------------------------------------------------------------------------------------------------
const FvStateCondition* FvUnitAura::EffectStateCondition()const
{
	if(GetInfo().m_iActionStateMask != 0 || GetInfo().m_iAuraStateMask != 0)/// 只有不修改状态的aura才能有EffectStateCondition
	{
		return NULL;
	}
	const FvStateCondition* pkCondition = FvLogicGameData::Instance().GetStateCondition(GetInfo().m_iEffectStateIdx);
	return pkCondition;
}
const FvAuraInfo::ValueRangeArray* FvUnitAura::EffectValueCondition()const
{
	const FvAuraInfo::ValueRangeArray& kEffectValueCondition = GetInfo().m_kEffectCondition;
	if(FvAuraInfo::IsNotEmpty(kEffectValueCondition))
	{
		return &kEffectValueCondition;
	}
	else
	{
		return NULL;
	}
}
const FvStateCondition* FvUnitAura::EndStateCondition()const
{
	const FvStateCondition* pkCondition = FvLogicGameData::Instance().GetStateCondition(GetInfo().m_iEndStateIdx);
	return pkCondition;
}
const FvAuraInfo::ValueRangeArray* FvUnitAura::EndValueCondition()const
{
	const FvAuraInfo::ValueRangeArray& kEffectValueCondition = GetInfo().m_kEndCondition;
	if(FvAuraInfo::IsNotEmpty(kEffectValueCondition))
	{
		return &kEffectValueCondition;
	}
	else
	{
		return NULL;
	}
}
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FvUnitAura::IsAttributeMatch()const
{
	if(!m_opEffectStateConditionNotifiers.IsNULL() && !m_opEffectStateConditionNotifiers->IsMatch())
	{
		return false;
	}
	if(!m_opEffectValueConditionNotifiers.IsNULL() && !m_opEffectValueConditionNotifiers->IsMatch())
	{
		return false;
	}
	return true;
}
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

