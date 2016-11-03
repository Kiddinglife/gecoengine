
#include "FvGameUnitLogic.h"

#include "FvUnitAura.h"
#include "FvUnitHitEffect.h"


#include <FvLogBus.h>



bool FvGameUnitLogic::Prepare(const FvSpellID uiSpellID, const FvSpellInfo& kInfo, FvGameUnitAppearance* pkFocus)
{
	return m_kSpell.Prepare(pkFocus, uiSpellID, kInfo);
}
bool FvGameUnitLogic::Prepare(const FvSpellID uiSpellID, const FvSpellInfo& kInfo, const FvVector3& kPos)
{
	return m_kSpell.Prepare(kPos, uiSpellID, kInfo);
}
bool FvGameUnitLogic::Prepare(const FvSpellID uiSpellID, const FvSpellInfo& kInfo, const FvTarget& kTarget)
{
	return m_kSpell.Prepare(kTarget, uiSpellID, kInfo);
}
void FvGameUnitLogic::CancelSpell()
{
	m_kSpell.Cancel();
}
void FvGameUnitLogic::_OnAddAura(FvRefNode2<FvUnitAura*>& kAuraNode)
{
	FV_ASSERT_ERROR(kAuraNode.m_Content);
	m_AuraList.AttachBack(kAuraNode);
}
void FvGameUnitLogic::_OnDelAura(FvUnitAura& kAura)
{

}
void FvGameUnitLogic::RemoveAuraByType(const FvUInt32 uiAuraType)
{
	
}
void FvGameUnitLogic::RemoveAuraBySpellId(const FvSpellID uiSpellId)
{
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		pkAura->End();
	}
	m_AuraList.Clear();
}
bool FvGameUnitLogic::HasAuraByType(const FvUInt32 uiAuraType)const
{
	return false;
}
FvUInt32 FvGameUnitLogic::GetAuraCntByType(const FvUInt32 uiAuraType)const
{
	return 0;
}
bool FvGameUnitLogic::HasAuraBySpellId(const FvSpellID uiSpellId)const
{
	FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetHead();
	while (!m_AuraList.IsEnd(iter))
	{
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		FvRefList<FvUnitAura*>::Next(iter);
		if(pkAura->SpellId() == uiSpellId)
		{
			return true;
		}
	}
	return false;
}
bool FvGameUnitLogic::HasAuraByChannel(const FvUInt32 uiChannel)const
{
	FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetHead();
	while (!m_AuraList.IsEnd(iter))
	{
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		FvRefList<FvUnitAura*>::Next(iter);
		if(pkAura->GetChannelID() == uiChannel)
		{
			return true;
		}
	}
	return false;
}

FvUInt32 FvGameUnitLogic::GetAuraCnt(const FvSpellID uiSpellId, const FvSpellEffectIdx uiEffectIdx)const
{
	FvUInt32 uiCnt = 0;
	FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetHead();
	while (!m_AuraList.IsEnd(iter))
	{
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		FvRefList<FvUnitAura*>::Next(iter);
		if(pkAura->SpellId() == uiSpellId && pkAura->EffectIdx() == uiEffectIdx)
		{
			++uiCnt;
		}
	}
	return uiCnt;
}
FvUInt32 FvGameUnitLogic::DispelAuraBySign(const FvEffectSign::Idx eSign)
{
	if(eSign == 0)
		return 0;
	FvUInt32 uiCnt = 0;
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		if(FvAura::CanDispel(*pkAura) && pkAura->GetInfo().m_iEffectSign == eSign)
		{
			pkAura->End();
			++uiCnt;
		}
	}
	return uiCnt;
}
FvUInt32 FvGameUnitLogic::DispelAuraByType(const FvUInt32 uiAuraType)
{
	FvUInt32 uiCnt = 0;
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		if(FvAura::CanDispel(*pkAura))
		{
			pkAura->End();
			++uiCnt;
		}
	}
	return uiCnt;
}
FvUInt32 FvGameUnitLogic::DispelAuraBySpell(const FvSpellID uiSpell)
{
	FvUInt32 uiCnt = 0;
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		if(FvAura::CanDispel(*pkAura) && pkAura->SpellId() == uiSpell)
		{
			pkAura->End();
			++uiCnt;
		}
	}
	return uiCnt;
}
FvUInt32 FvGameUnitLogic::TradeAuraBySign(const FvEffectSign::Idx eSign, FvGameUnitAppearance& kReciver)
{
	FvUInt32 uiCnt = 0;
	return uiCnt;
}
FvUInt32 FvGameUnitLogic::TradeAuraByType(const FvUInt32 uiAuraType, FvGameUnitAppearance& kReciver)
{
	FvUInt32 uiCnt = 0;
	return uiCnt;
}

FvUnitAura* FvGameUnitLogic::SetAuraDuration(const FvSpellID uiSpell, const FvSpellEffectIdx uiEffectIdx, const FvUInt32 uiDuration)
{
	//FvLogBus::CritOk("FvGameUnitLogic::SetFirstAuraDuration(%d, %d, %d)", uiSpell, FvUInt32(uiEffectIdx), uiDuration);
	FvUnitAura* pkShortAura = NULL;///最短时间的光环
	FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetHead();
	while (!m_AuraList.IsEnd(iter))
	{
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		FvRefList<FvUnitAura*>::Next(iter);
		if(pkAura->SpellId() == uiSpell && pkAura->EffectIdx() == uiEffectIdx)
		{
			if(pkShortAura == NULL)
			{
				pkShortAura = pkAura;
			}
			else
			{
				if(pkAura->GetDuration() < pkShortAura->GetDuration())
				{
					pkShortAura = pkAura;
				}
			}
		}
	}
	if(pkShortAura)
	{
		pkShortAura->SetDurTime(uiDuration);
	}
	return pkShortAura;
}
void FvGameUnitLogic::CloseAllAura()
{
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		pkAura->End();
	}
}
void FvGameUnitLogic::CloseAura(const FvSpellID uiSpell)
{
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		if(pkAura->SpellId() == uiSpell)
		{
			pkAura->End();
		}
	}
}
void FvGameUnitLogic::CloseBorrowAura()
{
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		if(!pkAura->IsOwn())
		{
			pkAura->End();
		}
	}
}
void FvGameUnitLogic::CloseAuraByChannel(const FvUInt32 uiChannel)
{
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		if(pkAura->GetChannelID() == uiChannel)
		{
			pkAura->End();
		}
	}
}
//+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvGameUnitLogic::_OnAddHitEffect(FvRefNode2<FvUnitHitEffect*>& kHitEffectNode)
{
	m_HitEffectList.AttachBack(kHitEffectNode);
}
void FvGameUnitLogic::_ClearHitEffects()
{
	m_HitEffectList.BeginIterator();
	while (!m_HitEffectList.IsEnd())
	{
		FvRefList<FvUnitHitEffect*>::iterator iter = m_HitEffectList.GetIterator();
		FvUnitHitEffect* pkHitEffect = (*iter).m_Content;
		FV_ASSERT_ERROR(pkHitEffect);
		m_HitEffectList.Next();
		pkHitEffect->End();
	}
	m_HitEffectList.Clear();
}
void FvGameUnitLogic::_SaveClearAuras(const bool bUpdate)
{
	m_AuraList.BeginIterator();
	while (!m_AuraList.IsEnd())
	{
		FvRefList<FvUnitAura*>::iterator iter = m_AuraList.GetIterator();
		FvUnitAura* pkAura = (*iter).m_Content;
		FV_ASSERT_ERROR(pkAura);
		m_AuraList.Next();
		pkAura->Save(bUpdate);
	}
	m_AuraList.Clear();
}