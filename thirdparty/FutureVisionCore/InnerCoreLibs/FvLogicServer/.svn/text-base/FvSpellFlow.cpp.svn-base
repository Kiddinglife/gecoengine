#include "FvSpellFlow.h"

#include "FvGameUnitLogic.h"
#include "FvTargetSelector.h"
#include "FvUnitAura.h"
#include "FvAreaAura.h"
#include "FvUnitHitEffect.h"
#include "FvAreaHitEffect.h"
#include "FvLogicGameData.h"

#include <FvLogBus.h>
#include <FvGameRandom.h>
#include <FvGameTimer.h>
#include <FvDestroyManager.h>




static const FvUInt32 ERR_EFFECT_IDX = FvUInt32(-1);

FvRefListNameSpace<FvSpellFlow>::List1 FvSpellFlow::ms_FreeNodeList;//!

Private_FvSpellFlow::SpellTargetList FvSpellFlow::ms_kTargetList;

FvOwnPtr<FvAreaTargetSelector> FvSpellFlow::ms_opAreaTargetSelector = NULL;//! 区域目标选择器

namespace Private_FvSpellFlow
{
	void SpellTargetList::Select(const FvGameUnitLogic& kCaster, const FvTarget& kFocus, const FvTargetSelectInfo& kInfo, FvAreaTargetSelector& kTargetSelector)
	{
		m_kTargetList.Clear();
		m_kPosList.clear();
		m_uiRandomStartIdx = 0;
		if(kInfo.m_iStateIdx == 0)
		{
			m_rpStateCondition = NULL;
		}
		else
		{
			m_rpStateCondition = FvLogicGameData::Instance().GetStateCondition(kInfo.m_iStateIdx);
		}
		///<Caster>
		Select(kCaster, kCaster.Appearance(), kInfo.m_kCasterEffectRange, kInfo.m_iCasterMask, kTargetSelector);
		///<Focus>
		FvGameUnitAppearance* pkObj = kFocus.Obj();
		if(pkObj)
		{
			Select(kCaster, *pkObj, kInfo.m_kTargetEffectRange, kInfo.m_iTargetMask, kTargetSelector);
		}
		else
		{
			const FvVector3* pkPos = kFocus.Pos();
			if(pkPos)
			{
				Select(kCaster, *pkPos, kInfo.m_kTargetEffectRange, kInfo.m_iTargetMask, kTargetSelector);
			}
		}
		if(kInfo.m_iMaxAffectTargets)
		{
			RandomSelect(kInfo.m_iMaxAffectTargets);
		}
		else
		{
			m_uiCnt = m_kTargetList.GetCnt();
		}
	}

	void SpellTargetList::Select(const FvGameUnitLogic& kCaster, FvGameUnitAppearance& kObj, const FvAreaRange& kRange, const FvUInt32 uiMask, FvAreaTargetSelector& kTargetSelector)
	{
		const FvUInt32 uiSelfMask = FvSpellSelectRalation::GetSelfRelation(uiMask);
		if(uiSelfMask)
		{
			if(kCaster.Appearance().IsMatchRelationMask(uiSelfMask, kObj))
			{
				AddObj(kObj);
			}
		}
		const FvUInt32 uiRoundMask = FvSpellSelectRalation::GetRoundRelation(uiMask);
		if(uiRoundMask)
		{
			kTargetSelector.Select(kCaster, kCaster.GetDirection(), kObj.Position(), uiRoundMask, kRange);
			const FvAreaTargetSelector::TargetList& kTargets = kTargetSelector.GetTargets();
			for (FvUInt32 uiIdx = 0; uiIdx < kTargets.GetCnt(); ++uiIdx)
			{
				FvGameUnitAppearance& kTarget = kTargets._GetTarget(uiIdx);
				if(&kTarget != &kObj)
				{
					AddObj(kTarget);
				}
			}
		}
	}
	void SpellTargetList::Select(const FvGameUnitLogic& kCaster, const FvVector3& kPos, const FvAreaRange& kRange, const FvUInt32 uiMask, FvAreaTargetSelector& kTargetSelector)
	{
		if(uiMask&_MASK_(FvSpellSelectRalation::POS))
		{
			m_kPosList.push_back(kPos);
		}
		const FvUInt32 uiRoundMask = FvSpellSelectRalation::GetRoundRelation(uiMask);
		if(uiRoundMask)
		{
			kTargetSelector.Select(kCaster, kCaster.GetDirection(), kPos, uiRoundMask, kRange);
			const FvAreaTargetSelector::TargetList& kTargets = kTargetSelector.GetTargets();
			for (FvUInt32 uiIdx = 0; uiIdx < kTargets.GetCnt(); ++uiIdx)
			{
				FvGameUnitAppearance& kTarget = kTargets._GetTarget(uiIdx);
				AddObj(kTarget);
			}
		}
	}

	void SpellTargetList::AddObj(FvGameUnitAppearance& kObj)
	{
		if(m_rpStateCondition && !kObj.IsMatch(*m_rpStateCondition))
		{
			return;
		}
		for (FvUInt32 uiIdx = 0; uiIdx < m_kTargetList.GetCnt(); ++uiIdx)
		{
			FvGameUnitAppearance& kTarget = m_kTargetList._GetTarget(uiIdx);
			if(&kTarget == &kObj)
			{
				return;
			}
		}
		m_kTargetList.Add(kObj);
	}
	void SpellTargetList::RandomSelect(const FvUInt32 uiCnt)
	{
		if(m_kTargetList.GetCnt() <= uiCnt)
		{
			m_uiCnt = m_kTargetList.GetCnt();
			m_uiRandomStartIdx = 0;
			return;
		}
		m_uiCnt = uiCnt;
		const FvInt32 uiRandomRange = m_kTargetList.GetCnt() - uiCnt;
		FV_ASSERT_ERROR(uiRandomRange > 0);
		m_uiRandomStartIdx = FvGameRandom::GetValue(0, uiRandomRange);
	}
	FvUInt32 SpellTargetList::GetObjCnt()const
	{
		return m_uiCnt;
	}
	FvGameUnitAppearance& SpellTargetList::_GetTarget(const FvUInt32 uiIdx)const
	{
		FV_ASSERT_ERROR(uiIdx < GetObjCnt());
		return m_kTargetList._GetTarget(m_uiRandomStartIdx + uiIdx);		
	}
	void SpellTargetList::GetTarget(std::vector<FvGameUnitAppearance*>& kObjList)const
	{
		const FvUInt32 uiSize = GetObjCnt();
		kObjList.reserve(uiSize);
		for (FvUInt32 uiIdx = 0; uiIdx < uiSize; ++uiIdx)
		{
			FvGameUnitAppearance& kTarget = _GetTarget(uiIdx);
			kObjList.push_back(&kTarget);
		}
	}
}

//+---------------------------------------------------------------------------------------------------------------------------------------------

void FvSpellFlow::ClearFreeFlow()
{
	ms_FreeNodeList.BeginIterator();
	while (!ms_FreeNodeList.IsEnd())
	{
		FvRefListNameSpace<FvSpellFlow>::List1::iterator iter = ms_FreeNodeList.GetIterator();
		FvSpellFlow* pkSpellFlow = (*iter).Content();
		ms_FreeNodeList.Next();
		FV_ASSERT_ERROR(pkSpellFlow);
		FvDestroyManager::Destroy(*pkSpellFlow);
	}
	ms_FreeNodeList.Clear();
	FV_SAFE_DELETE(ms_opAreaTargetSelector);
}
void FvSpellFlow::SetTargetSelector(FvAreaTargetSelector* pkSelector)
{
	ms_opAreaTargetSelector = pkSelector;
}


void FvSpellFlow::_Init(FvSpellFlow* pkSpellFlow, const float fHitTime, FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo, const FvUInt32 uiSpellId, const FvTarget& kTarget)
{
	if(pkSpellFlow)
	{
		pkSpellFlow->Init(kCaster, kInfo, uiSpellId, kTarget);
		ms_FreeNodeList.AttachBack(*pkSpellFlow);
		pkSpellFlow->SetHitTime(FvUInt32(fHitTime*100));
	}
}
bool FvSpellFlow::MakeSpellFlow(FvSpellFlow& kDefaultSpellFlow, FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo, const FvSpellID uiSpellId, const FvTarget& kTarget)
{
	const FvSpellProcInfo& kProc = kInfo.m_kProcInfo;
	if(kProc.m_KTraveller.m_iType == 0)
	{
		kDefaultSpellFlow.Init(kCaster, kInfo, uiSpellId, kTarget);
		kDefaultSpellFlow.Fresh();
		return true;
	}
	else
	{
		//! 远程技能
		float fSpd = kProc.m_KTraveller.m_iSpeed*0.01f;
		fSpd = max(fSpd, 1.0f);
		if(kTarget.Pos())
		{
			const FvVector3* pos = kTarget.Pos();
			const float fDistance = FvDistance(*pos, kCaster.Appearance().Position());
			const float fDeltaTime = fDistance/fSpd;
			FvSpellFlow* pkSpellFlow = kCaster.MakeSpellFlow(kInfo, uiSpellId, kTarget);
			ms_FreeNodeList.AttachBack(*pkSpellFlow);
			_Init(pkSpellFlow, fDeltaTime, kCaster, kInfo, uiSpellId, kTarget);
			return true;
		}
		else if(kTarget.Obj())
		{
			FvGameUnitAppearance* pkObj = kTarget.Obj();
			const float fDistance = FvDistance(pkObj->Position(), kCaster.Appearance().Position());
			const float fDeltaTime = fDistance/fSpd;
			FvSpellFlow* pkSpellFlow = kCaster.MakeSpellFlow(kInfo, uiSpellId, kTarget);
			ms_FreeNodeList.AttachBack(*pkSpellFlow);
			_Init(pkSpellFlow, fDeltaTime, kCaster, kInfo, uiSpellId, kTarget);
			return true;
		}
		return false;
	}
}

FvSpellFlow::FvSpellFlow(void)
:m_uiSpellId(0)
,m_rpSpellInfo(NULL)
#pragma warning(push)
#pragma warning(disable: 4355)
,m_kHitTimeNode(*this)
#pragma warning(pop)
{
	
}

FvSpellFlow::~FvSpellFlow(void)
{
	CloseOwnAuras();
}

void FvSpellFlow::Init(FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo, const FvSpellID uiSpellId, const FvTarget& kTarget)
{
	m_rpCaster = &kCaster;
	m_kFocus = kTarget;
	m_rpSpellInfo = &kInfo;
	_RecordCasterInfo(kInfo);
	m_uiSpellId = uiSpellId;
}
const FvSpellInfo& FvSpellFlow::GetInfo()const
{
	FV_ASSERT_ERROR(m_rpSpellInfo);
	return *m_rpSpellInfo;
}

void FvSpellFlow::_RecordCasterInfo(const FvSpellInfo& kInfo)
{
	if(m_rpCaster == NULL)
	{
		return;
	}
	FvGameUnitLogic& kCaster = *m_rpCaster;
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellInfo::EFFECT_MAX; ++uiIdx)
	{
		//const FvAuraInfo& kAuraInfo = kInfo.m_AuraInfos[uiIdx];
		const FvAuraInfo& kAuraInfo = *(*g_pfnGetAura)(kInfo.m_AuraIds[uiIdx]);
		if(!kAuraInfo.IsEmpty())
		{
			kCaster.Appearance().GetAuraValue(kAuraInfo, m_CasterAuraRefValues[uiIdx]);
		}
	}
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellInfo::EFFECT_MAX; ++uiIdx)
	{
		const FvHitEffectInfo& kHitEffect = kInfo.m_HitEffectInfos[uiIdx];
		if(!kHitEffect.IsEmpty())
		{
			kCaster.Appearance().GetHitEffectValue(kHitEffect, m_CasterHitEffectRefValues[uiIdx]);
		}
	}
}
void FvSpellFlow::SetHitTime(const FvUInt32 uiDeltaTime)
{
	FvGameTimer::SetTime(uiDeltaTime, m_kHitTimeNode);
}
void FvSpellFlow::_OnHitTime(FvTimeEventNodeInterface&)
{
	Fresh();
	FvDestroyManager::Destroy(*this);
}

//+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvSpellFlow::Fresh()
{
	FV_CHECK_RETURN(m_rpCaster);
	FV_ASSERT_ERROR(m_rpSpellInfo);
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellInfo::SELECT_MAX; ++uiIdx)
	{
		if(GetInfo().HasSelect(uiIdx))
		{
			FreshEffect(uiIdx);
		}
	}
}

const Private_FvSpellFlow::SpellTargetList& FvSpellFlow::UpdateTargetSelector(const FvGameUnitLogic& kCaster, const FvUInt32 uiSelectIdx)const
{
	FV_ASSERT_ERROR(m_rpSpellInfo);
	FV_ASSERT_ERROR(ms_opAreaTargetSelector);
	if(uiSelectIdx >= FvSpellInfo::SELECT_MAX)
	{
		return ms_kTargetList;
	}
	ms_kTargetList.Select(*m_rpCaster, m_kFocus, GetInfo().m_kTargetSelect[uiSelectIdx], *ms_opAreaTargetSelector);
	return ms_kTargetList;
}
const Private_FvSpellFlow::SpellTargetList& FvSpellFlow::FreshEffect(const FvGameUnitLogic& kCaster, const FvUInt32 uiSelectIdx)
{
	FV_ASSERT_ERROR(m_rpSpellInfo);
	FV_ASSERT_ERROR(ms_opAreaTargetSelector);
	if(uiSelectIdx >= FvSpellInfo::SELECT_MAX)
	{
		return ms_kTargetList;
	}
	ms_kTargetList.Select(*m_rpCaster, m_kFocus, GetInfo().m_kTargetSelect[uiSelectIdx], *ms_opAreaTargetSelector);
	///<Aura>
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellInfo::EFFECT_MAX; ++uiIdx)
	{
		const FvAuraInfo* pkInfo = GetInfo().GetSelectAura(uiSelectIdx, uiIdx);
		if(pkInfo)
		{
			_Fresh(*m_rpCaster, uiIdx, ms_kTargetList, *pkInfo);
		}
	}
	///<HitEffect>
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellInfo::EFFECT_MAX; ++uiIdx)
	{
		const FvHitEffectInfo* pkInfo = GetInfo().GetSelectHitEffect(uiSelectIdx, uiIdx);
		if(pkInfo)
		{
			_Fresh(*m_rpCaster, uiIdx, ms_kTargetList, *pkInfo);
		}
	}
	return ms_kTargetList;
}
void FvSpellFlow::FreshEffect(const FvUInt32 uiSelectIdx)
{
	FV_ASSERT_ERROR(m_rpSpellInfo);
	if(m_rpCaster == NULL)
	{
		return;
	}
	FreshEffect(*m_rpCaster, uiSelectIdx);
}
void FvSpellFlow::_Fresh(FvGameUnitLogic& kCaster, const FvUInt32 uiEffectIdx, const TargetList& ms_kTargetList, const FvAuraInfo& kInfo)
{
	if(kInfo.m_iAreaFlag == 1)
	{
		for (std::vector<FvVector3>::const_iterator iter = ms_kTargetList.PosList().begin(); iter != ms_kTargetList.PosList().end(); ++iter)
		{
			const FvVector3& kPos = (*iter);
			_CreateAreaAura(uiEffectIdx, kInfo, kCaster, m_CasterAuraRefValues[uiEffectIdx], kPos);
		}
	}
	else
	{
		for (FvUInt32 uiIdx = 0; uiIdx < ms_kTargetList.GetObjCnt(); ++uiIdx)
		{
			FvGameUnitAppearance& kObj = ms_kTargetList._GetTarget(uiIdx);
			_CreateUnitAura(uiEffectIdx, kInfo, kCaster, m_CasterAuraRefValues[uiEffectIdx], kObj);
		}
	}
}
void FvSpellFlow::_Fresh(FvGameUnitLogic& kCaster, const FvUInt32 uiEffectIdx, const TargetList& ms_kTargetList, const FvHitEffectInfo& kInfo)
{
	if(kInfo.m_iAreaFlag == 1)
	{
		for (std::vector<FvVector3>::const_iterator iter = ms_kTargetList.PosList().begin(); iter != ms_kTargetList.PosList().end(); ++iter)
		{
			const FvVector3& kPos = (*iter);
			_CreateAreaHitEffect(uiEffectIdx, kInfo, kCaster, m_CasterHitEffectRefValues[uiEffectIdx], kPos);
		}
	}
	else
	{
		for (FvUInt32 uiIdx = 0; uiIdx < ms_kTargetList.GetObjCnt(); ++uiIdx)
		{
			FvGameUnitAppearance& kObj = ms_kTargetList._GetTarget(uiIdx);
			_CreateUnitHitEffect(uiEffectIdx, kInfo, kCaster, m_CasterHitEffectRefValues[uiEffectIdx], kObj);
		}
	}
}

void FvSpellFlow::CreateUnitAura(FvGameUnitAppearance& kTarget, const FvUInt32 uiEffectIdx)
{
	const FvAuraInfo* pkAuraInfo = GetInfo().GetAura(uiEffectIdx);
	if(pkAuraInfo && GetCaster())
	{
		FV_ASSERT_ERROR(uiEffectIdx < FvSpellInfo::EFFECT_MAX);
		_CreateUnitAura(uiEffectIdx, *pkAuraInfo, *GetCaster(), m_CasterAuraRefValues[uiEffectIdx], kTarget);
	}
}
void FvSpellFlow::CreateUnitHitEffect(FvGameUnitAppearance& kTarget, const FvUInt32 uiEffectIdx)
{
	const FvHitEffectInfo* pkHitEffectInfo = GetInfo().GetHitEffect(uiEffectIdx);
	if(pkHitEffectInfo && GetCaster())
	{
		FV_ASSERT_ERROR(uiEffectIdx < FvSpellInfo::EFFECT_MAX);
		_CreateUnitHitEffect(uiEffectIdx, *pkHitEffectInfo, *GetCaster(), m_CasterHitEffectRefValues[uiEffectIdx], kTarget);
	}
}
void FvSpellFlow::CreateAreaAura(const FvVector3& kPos, const FvUInt32 uiEffectIdx)
{
	const FvAuraInfo* pkAuraInfo = GetInfo().GetAura(uiEffectIdx);
	if(pkAuraInfo && GetCaster())
	{
		FV_ASSERT_ERROR(uiEffectIdx < FvSpellInfo::EFFECT_MAX);
		_CreateAreaAura(uiEffectIdx, *pkAuraInfo, *GetCaster(), m_CasterAuraRefValues[uiEffectIdx], kPos);
	}
}
void FvSpellFlow::CreateAreaHitEffect(const FvVector3& kPos, const FvUInt32 uiEffectIdx)
{
	const FvHitEffectInfo* pkHitEffectInfo = GetInfo().GetHitEffect(uiEffectIdx);
	if(pkHitEffectInfo && GetCaster())
	{
		FV_ASSERT_ERROR(uiEffectIdx < FvSpellInfo::EFFECT_MAX);
		_CreateAreaHitEffect(uiEffectIdx, *pkHitEffectInfo, *GetCaster(), m_CasterHitEffectRefValues[uiEffectIdx], kPos);
	}
}

void FvSpellFlow::CloseOwnAuras()
{
	for(std::vector<FvAuraOwnInterface*>::const_iterator iter = m_kOwnAuraList.begin(); iter != m_kOwnAuraList.end(); ++iter)
	{
		FvAuraOwnInterface* pkOwn = (*iter);
		FV_ASSERT_ERROR(pkOwn);
		pkOwn->End();
		delete pkOwn;
	}
	m_kOwnAuraList.clear();
}


//+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void FvSpellFlow::Clear()
{
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellInfo::EFFECT_MAX; ++uiIdx)
	{
		FvAuraValue& kValue = m_CasterAuraRefValues[uiIdx];
		kValue.Clear();
	}
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellInfo::EFFECT_MAX; ++uiIdx)
	{
		FvEffectValue& kValue = m_CasterHitEffectRefValues[uiIdx];
		kValue.Clear();
	}
	CloseOwnAuras();
	m_kFocus.Clear();
	m_rpCaster = NULL;
	m_rpSpellInfo = NULL;
}

//+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void FvSpellFlow::_CreateUnitAura(const FvUInt32 uiEffectId, const FvAuraInfo& kInfo, FvGameUnitLogic& kCaster, const FvAuraValue& kCasterValues, FvGameUnitAppearance& kTarget)
{
	FV_ASSERT_WARNING(kInfo.m_iAreaFlag == 0);
	FvSpellAuraInstance kInstance;
	kInstance.m_uiSpell = m_uiSpellId;
	kInstance.m_uiEffect = uiEffectId;
	kInstance.m_kCasterValue = kCasterValues;
	switch (kInfo.m_iAttachType)
	{
	case FvAuraAttachType::FREE:
		{
			kCaster.MakeUnitAura(kTarget, kInstance);
		}
		break;
	case FvAuraAttachType::BORROW:
		{
			FvAuraOwnInterface* pkOwn = kCaster.BorrowUnitAura(kTarget, kInstance);
			if(pkOwn)
			{
				m_kOwnAuraList.push_back(pkOwn);
			}
		}
		break;	
	}
}

void FvSpellFlow::_CreateUnitHitEffect(const FvUInt32 uiEffectId, const FvHitEffectInfo& kInfo, FvGameUnitLogic& kCaster, const FvEffectValue& kRefValue, FvGameUnitAppearance& kTarget)
{
	FV_ASSERT_WARNING(kInfo.m_iAreaFlag == 0);
	FvSpellHitEffectInstance kInstance;
	kInstance.m_uiSpell = m_uiSpellId;
	kInstance.m_uiEffect = uiEffectId;
	kInstance.m_kCasterValue = kRefValue;
	kCaster.MakeUnitHitEffect(kTarget, kInstance);
}

//+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FvSpellFlow::_CreateAreaAura(const FvUInt32 uiEffectId, const FvAuraInfo& kInfo, FvGameUnitLogic& kCaster, const FvAuraValue& kCasterValues, const FvVector3& kPos)
{
	FV_ASSERT_WARNING(kInfo.m_iAreaFlag == 1);
	FvSpellAuraInstance kInstance;
	kInstance.m_uiSpell = m_uiSpellId;
	kInstance.m_uiEffect = uiEffectId;
	kInstance.m_kCasterValue = kCasterValues;
	switch (kInfo.m_iAttachType)
	{
	case FvAuraAttachType::FREE:
		{
			kCaster.MakeAreaAura(kPos, kInstance);
		}
		break;
	case FvAuraAttachType::BORROW:
		{
			FvAuraOwnInterface* pkOwn = kCaster.BorrowAreaAura(kPos, kInstance);
			if(pkOwn)
			{
				m_kOwnAuraList.push_back(pkOwn);
			}
		}
		break;	
	}
}

void FvSpellFlow::_CreateAreaHitEffect(const FvUInt32 uiEffectId, const FvHitEffectInfo& kInfo, FvGameUnitLogic& kCaster, const FvEffectValue& kRefValue, const FvVector3& kPos)
{
	FV_ASSERT_WARNING(kInfo.m_iAreaFlag == 1);
	FvSpellHitEffectInstance kInstance;
	kInstance.m_uiSpell = m_uiSpellId;
	kInstance.m_uiEffect = uiEffectId;
	kInstance.m_kCasterValue = kRefValue;
	kCaster.MakeAreaHitEffect(kPos, kInstance);
}
