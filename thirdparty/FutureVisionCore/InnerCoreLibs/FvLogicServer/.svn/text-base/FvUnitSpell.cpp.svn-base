#include "FvUnitSpell.h"


#include "FvGameUnitLogic.h"
#include "FvLogicGameData.h"

#include <FvGameTimer.h>
#include <FvLogBus.h>
#include <FvDestroyManager.h>


//+----------------------------------------------------------------------------------------------------
FvCreator<FvSendWork, FvUInt32> FvUnitSpell::ms_SendWorkCreator;
FvCreator<FvSpellTargetEffect, FvUInt32> FvUnitSpell::ms_TargetEffectCreator;



//+----------------------------------------------------------------------------------------------------

FvSendWork::FvSendWork()
{}
FvSendWork::~FvSendWork()
{}
void FvSendWork::_OnStart()
{

}
void FvSendWork::_OnEnd()
{
	FvDestroyManager::Destroy(*this);
}

//+----------------------------------------------------------------------------------------------------

//+----------------------------------------------------------------------------------------------------
FvUnitSpell::FvUnitSpell()
:m_rpSpellInfo(NULL)
,m_eState(IDLE)
,m_uiCastReserveTime(0)
,m_bDurationSpell(false)
#pragma warning(push)
#pragma warning(disable: 4355)
,m_kSendWorkEndAsynCB(*this)
,m_kCasterStateNotifiers(*this)
,m_kFocusStateNotifiers(*this)
#pragma warning(pop)
{
	m_kTimeFlow.Init(FvGameTimer::Timer(), EVENT_SUP, *this);
}

FvUnitSpell::~FvUnitSpell(void)
{
	m_kSendWorkFlow.Clear();
}
//+----------------------------------------------------------------------------------------------------
FvUnitSpell::_State FvUnitSpell::GetState()const
{
	return m_eState;
}
void FvUnitSpell::_SetState(const _State eState)
{
	m_eState = eState;
}
//+----------------------------------------------------------------------------------------------------
const FvSpellInfo* FvUnitSpell::GetSpellInfo()const{return m_rpSpellInfo;}
FvGameUnitLogic& FvUnitSpell::Self(){return FvGameUnitLogic::Self(*this);}
const FvGameUnitLogic& FvUnitSpell::Self()const{return FvGameUnitLogic::Self(*this);}
//+----------------------------------------------------------------------------------------------------
bool FvUnitSpell::Prepare(FvGameUnitAppearance* pkFocus, const FvSpellID uiSpellId, const FvSpellInfo& kInfo)
{
	if(pkFocus == NULL)
	{
		return false;
	}
	if(GetState() != IDLE)
	{
		Self().Appearance().SendMessage(Self().Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_NO_INFO));
		return false;
	}
	if(_CheckPower(Self().Appearance(), kInfo) == false)
	{
		Self().Appearance().SendMessage(Self().Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_CASTER_POWER));
		return false;
	}	
	if(Check(kInfo, pkFocus) == false)
	{
		return false;
	}
	const FvStateCondition* pkCondition = FvLogicGameData::Instance().GetStateCondition(kInfo.m_kProcInfo.m_iFocusStateIdx);
	if(pkCondition)
	{
		m_kFocusStateNotifiers.Init(*pkCondition, *pkFocus);
	}
	m_kTarget.Set(*pkFocus);
	return _Prepare(uiSpellId, kInfo);
}
bool FvUnitSpell::Prepare(const FvVector3& kPos, const FvSpellID uiSpellId, const FvSpellInfo& kInfo)
{
	if(GetState() != IDLE)
	{
		Self().Appearance().SendMessage(Self().Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_NO_INFO));
		return false;
	}
	if(_CheckPower(Self().Appearance(), kInfo) == false)
	{
		Self().Appearance().SendMessage(Self().Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_CASTER_POWER));
		return false;
	}
	if(Check(kInfo, kPos) == false)
	{
		return false;
	}
	m_kTarget.Set(kPos);
	return _Prepare(uiSpellId, kInfo);
}
bool FvUnitSpell::Prepare(const FvTarget& kTarget, const FvSpellID uiSpellId, const FvSpellInfo& kInfo)
{
	if(GetState() != IDLE)
	{
		Self().Appearance().SendMessage(Self().Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_NO_INFO));
		return false;
	}
	if(_CheckPower(Self().Appearance(), kInfo) == false)
	{
		Self().Appearance().SendMessage(Self().Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_CASTER_POWER));
		return false;
	}
	if(Check(kInfo, kTarget) == false)
	{
		return false;
	}
	m_kTarget = kTarget;
	return _Prepare(uiSpellId, kInfo);
}
void FvUnitSpell::Delay(const float fDeltaTime)
{
	FV_ASSERT_WARNING(fDeltaTime > 0.0f);
	FV_CHECK_RETURN(m_rpSpellInfo);
	if(m_rpSpellInfo->m_kProcInfo.m_iCutDurationMask == 0)
	{
		return;
	}
	if(GetState() == READY)
	{
		m_rpSpellInfo->m_kProcInfo.m_iPrepareTime;
		const FvUInt32 uiDelayTime = m_kTimeFlow.GetReserveTime(EVNET_PRE_CAST);
		const FvSpellProcInfo& kProc = m_rpSpellInfo->m_kProcInfo;
		const FvUInt32 uiNewDelayTime = FvMathTool::Min<FvUInt32>(uiDelayTime + FvUInt32(fDeltaTime*100), kProc.m_iPrepareTime);
		FV_ASSERT_WARNING(uiNewDelayTime >= uiDelayTime);
		if(uiNewDelayTime > uiDelayTime)
		{
			m_kTimeFlow.DelayAll(uiNewDelayTime - uiDelayTime);
		}
		Self().OnPrepareTimeUpdate(m_rpSpellInfo->m_iSpellId, uiNewDelayTime*0.01f);
	}
	else if(GetState() == CASTING)
	{
		if(m_uiCastReserveTime > 0)
		{
			if(m_uiCastReserveTime > FvInt32(fDeltaTime*100))
			{
				m_uiCastReserveTime = m_uiCastReserveTime - FvInt32(fDeltaTime*100);
				Self().OnCastTimeUpdate(m_rpSpellInfo->m_iSpellId, m_uiCastReserveTime*0.01f);
			}
			else
			{
				m_uiCastReserveTime = 0;
			}
		}
	}
}
bool FvUnitSpell::_Prepare(const FvSpellID uiSpellId, const FvSpellInfo& kInfo)
{
	m_rpSpellInfo = &kInfo;
	m_kCancelFrameEnd.Detach();
	Active();
	const FvSpellProcInfo& kProc = m_rpSpellInfo->m_kProcInfo;
	const FvStateCondition* pkCondition = FvLogicGameData::Instance().GetStateCondition(kInfo.m_kProcInfo.m_iCasterStateIdx);
	if(pkCondition)
	{
		m_kCasterStateNotifiers.Init(*pkCondition, Self().Appearance());
	}
	_SetState(READY);
	//!
	if(kProc.m_iCastCnt)
	{
		m_uiCastReserveTime = kProc.m_iCastCnt*(kProc.m_iCastTime+kProc.m_iCastEndTime);
		m_bDurationSpell = true;
	}
	else
	{
		m_uiCastReserveTime = -1;
		m_bDurationSpell = false;
	}
	//!
	FvUInt32 uiTime = kProc.m_iPrepareTime;
	m_kTimeFlow.SetTimeLapse(EVNET_PRE_CAST, uiTime, &FvUnitSpell::_OnPreCast);
	//!
	Self().OnPrepare(m_rpSpellInfo->m_iSpellId);
	//!
	return true;
}
bool FvUnitSpell::Cancel()
{
	if(GetState() == READY || GetState() == CASTING)
	{
		FvLogBus::CritOk("FvUnitSpel::Cancel Successfully");
		m_kCancelFrameEnd.Exec(*this);
		return true;
	}
	else
	{
		return false;
	}
}
void FvUnitSpell::_OnCancel()
{
	End();
}

//+----------------------------------------------------------------------------------------------------------

void FvUnitSpell::_OnPreCast(const FvUInt32&, FvTimeEventNodeInterface&)
{
	FV_ASSERT_ERROR(m_rpSpellInfo);
	if(Check(*m_rpSpellInfo, m_kTarget) == false)
	{
		FvLogBus::CritOk("FvUnitSpell::_OnCast¼ì²âÊ§°Ü");
		End();
		return;
	}
	if(_CheckPower(Self().Appearance(), *m_rpSpellInfo) == false)
	{
		Self().Appearance().SendMessage(Self().Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_CASTER_POWER));
		End();
		return;
	}
	_CostPower(*m_rpSpellInfo);
	m_kSendWorkFlow.Clear();
	Self().OnPreCastWork(m_rpSpellInfo->m_iSpellId);
	FvSendWork* pNewWork = ms_SendWorkCreator.Create(m_rpSpellInfo->m_kProcInfo.m_kSendWorkInfo.m_iType);
	if(pNewWork)
	{
		pNewWork->Init(Self(), m_rpSpellInfo->m_kProcInfo.m_kSendWorkInfo, m_kTarget);
		m_kSendWorkFlow.Attach(*pNewWork);
		m_kSendWorkFlow.Start();
		m_kSendWorkFlow.CallbackList().Attach(m_kSendWorkEndAsynCB);
	}
	else
	{
		m_kSendWorkEndAsynCB.Detach();
		if(m_bDurationSpell)
		{
			Self().OnCastStart();
		}
		_Cast();
	}
}
void FvUnitSpell::_OnCastWorkFlowEnd(const FvUInt32)
{
	m_kSendWorkEndAsynCB.Detach();	
	if(m_bDurationSpell)
	{
		Self().OnCastStart();
	}
	_Cast();
}
//+----------------------------------------------------------------------------------------------------

void FvUnitSpell::_OnCast(const FvUInt32&, FvTimeEventNodeInterface&)
{
	if(Check(*m_rpSpellInfo, m_kTarget) == false)
	{
		FvLogBus::CritOk("FvUnitSpell::_OnCast¼ì²âÊ§°Ü");
		End();
		return;
	}
	_Cast();
}

void FvUnitSpell::_Cast()
{
	FV_ASSERT_ERROR(m_rpSpellInfo);
	const FvSpellProcInfo& kProc = m_rpSpellInfo->m_kProcInfo;

	_SetState(CASTING);	
	Self().OnCast(m_rpSpellInfo->m_iSpellId);
	if(m_uiCastReserveTime > 0)
	{
		Self().OnCastTimeUpdate(m_rpSpellInfo->m_iSpellId, m_uiCastReserveTime*0.01f);
	}

	//
	FvUInt32 uiTime = 0;
	uiTime += kProc.m_iCastTime;
	m_kTimeFlow.SetTimeLapse(EVENT_SEND, uiTime, &FvUnitSpell::_OnSend);
	uiTime += kProc.m_iCastEndTime;
	m_uiCastReserveTime -= (kProc.m_iCastTime + kProc.m_iCastEndTime);
	if(m_uiCastReserveTime > 0)
	{
		//! Ñ­»·Ê©·¨
		m_kTimeFlow.SetTimeLapse(EVENT_CAST, uiTime, &FvUnitSpell::_OnCast);
	}
	else
	{
		m_kTimeFlow.SetTimeLapse(EVENT_ENDING, uiTime, &FvUnitSpell::_OnSendEnd);
	}
}
//+----------------------------------------------------------------------------------------------------------
void FvUnitSpell::_OnSend(const FvUInt32&, FvTimeEventNodeInterface&)
{
	_Send();
}
void FvUnitSpell::_Send()
{
	FV_ASSERT_ERROR(m_rpSpellInfo);
	const FvUInt32 uiSpellId = m_rpSpellInfo->m_iSpellId;
	const FvSpellProcInfo& kProc = m_rpSpellInfo->m_kProcInfo;
	FvInt32 iScriptFlowIdx = 0;
	if(m_rpSpellInfo->m_kProcInfo.m_kMiscValue.GetValue("ScriptSpellFlow", iScriptFlowIdx))
	{
		FvLogBus::CritOk("ScriptSpellFlow");
		m_rpChannelSpellFlow = Self().MakeSpellFlow(*m_rpSpellInfo, uiSpellId, m_kTarget);
	}
	else
	{
		if(FvSpellFlow::MakeSpellFlow(m_DefaultSpellFlow, Self(), *m_rpSpellInfo, uiSpellId, m_kTarget))
		{
			m_rpChannelSpellFlow = &m_DefaultSpellFlow;
		}
	}
	FvInt32 iScriptTargetEffect = 0;
	if(m_rpSpellInfo->m_kProcInfo.m_kMiscValue.GetValue("ScriptTargetEffect", iScriptTargetEffect))
	{
		FvLogBus::CritOk("ScriptTargetEffect");
		FvSpellTargetEffect* pkTargetEffect = ms_TargetEffectCreator.Create(iScriptTargetEffect);
		if(pkTargetEffect)
		{
			pkTargetEffect->OnHandle(Self(), m_kTarget, *m_rpSpellInfo);
		}
	}
}
void FvUnitSpell::_OnSendEnd(const FvUInt32&, FvTimeEventNodeInterface&)
{
	_EndSend();
	if(m_bDurationSpell)
	{
		Self().OnCastEnd();
	}
}
void FvUnitSpell::_EndSend()
{
	m_uiCastReserveTime = 0;
	if(m_rpChannelSpellFlow)
	{
		if(m_rpChannelSpellFlow != &m_DefaultSpellFlow)
		{
			m_rpChannelSpellFlow->End();
			m_rpChannelSpellFlow = NULL;
		}
	}
	FV_ASSERT_ERROR(m_rpSpellInfo);
	const FvSpellProcInfo& kProc = m_rpSpellInfo->m_kProcInfo;
	FvUInt32 uiTime = 0;
	uiTime += kProc.m_iActCoolTime;
	m_kTimeFlow.Stop();
	m_kTimeFlow.SetTimeLapse(EVENT_ENDED, uiTime, &FvUnitSpell::_OnEnd);
	_SetState(ENDING);
}
void FvUnitSpell::_OnEnd(const FvUInt32&, FvTimeEventNodeInterface&)
{
	//! ¿ªÆôÀäÈ´Ê±¼ä
	End();
}	
void FvUnitSpell::_OnEnd()
{
	FV_ASSERT_ERROR(m_rpSpellInfo);
	m_kTarget.Clear();
	if(GetState() == READY)
	{
		Self().OnCancelSpell();
	}
	else if(GetState() == CASTING)
	{
		Self().OnBreakSpell();
	}
	_EndSend();
	_SetState(IDLE);
	m_rpSpellInfo = NULL;
	m_kTimeFlow.Stop();
	m_kSendWorkEndAsynCB.Detach();
	m_kSendWorkFlow.Clear();
	m_DefaultSpellFlow.Clear();
	//! 
	m_kCasterStateNotifiers.Detach();
	m_kFocusStateNotifiers.Detach();

	m_kTarget.Clear();
}

//+-----------------------------------------------------------------------------------------------------------------------------------------------------

bool FvUnitSpell::Check(const FvSpellInfo& kInfo, FvGameUnitAppearance* pkObj)const
{
	static FvTarget kTarget;
	if(pkObj)
	{
		kTarget.Set(*pkObj);
	}
	return Check(kInfo, kTarget);
}
bool FvUnitSpell::Check(const FvSpellInfo& kInfo, const FvVector3& kPos)const
{
	static FvTarget kTarget;
	kTarget.Set(kPos);
	return Check(kInfo, kTarget);
}
bool FvUnitSpell::Check(const FvSpellInfo& kInfo, const FvTarget&kTarget)const
{
	if (_CheckCaster(Self(), kInfo) == false)
	{
		return false;
	}
	if(_CheckTarget(Self(), kInfo, kTarget) == false)
	{
		return false;
	}
	return true;
}
bool FvUnitSpell::_CheckCaster(const FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo)
{
	const FvStateCondition* pkCondition = FvLogicGameData::Instance().GetStateCondition(kInfo.m_kProcInfo.m_iCasterStateIdx);
	if(pkCondition)
	{
		const bool bStateResult = kCaster.Appearance().IsMatch(*pkCondition);
		if(bStateResult == false)
		{
			kCaster.Appearance().SendMessage(kCaster.Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_CASTER_MOVE));
			return false;
		}
	}
	return true;
}
bool FvUnitSpell::_CheckTarget(const FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo, const FvTarget&kTarget)
{
	const FvSpellProcInfo& kProc = kInfo.m_kProcInfo;
	if(kTarget.Obj())
	{
		FvGameUnitAppearance* rpTarget = kTarget.Obj();
		if(rpTarget != &kCaster.Appearance())
		{
			return _CheckTarget(kCaster, *rpTarget, kInfo);
		}
		else
		{
			if(kProc.m_iFocusTargetMask == 0)
			{
				return true;
			}
			if(FvMask::HasAny(kProc.m_iFocusTargetMask, _MASK_(FvUnitSociety::SELF)))
			{
				return true;
			}
			else
			{
				kCaster.Appearance().SendMessage(kCaster.Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_TARGET_ERROR));
				return false;
			}
		}
	}
	if(FvMask::HasAny(kProc.m_iFocusTargetMask, _MASK_(FvUnitSociety::GROUND)))
	{
		const FvVector3* pos = kTarget.Pos();
		if(pos && CheckRange(kCaster.Appearance(), *pos, kInfo))
		{
			return true;
		}
		else
		{
			kCaster.Appearance().SendMessage(kCaster.Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_TARGET_ERROR));
			return false;
		}
	}
	else
	{
		kCaster.Appearance().SendMessage(kCaster.Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_TARGET_ERROR));
		return false;
	}
}

//+-----------------------------------------------------------------------------------------------------------------------------------------------------
bool FvUnitSpell::_CheckTarget(const FvGameUnitLogic& kCaster, const FvGameUnitAppearance& kObj, const FvSpellInfo& kInfo)
{
	if(kCaster.Appearance().IsMatchRelationMask(kInfo.m_kProcInfo.m_iFocusTargetMask, kObj) == false)
	{
		kCaster.Appearance().SendMessage(kCaster.Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_TARGET_IS_FREE));
		return false;
	}
	const FvStateCondition* pkCondition = FvLogicGameData::Instance().GetStateCondition(kInfo.m_kProcInfo.m_iFocusStateIdx);
	if(pkCondition)
	{
		const bool bStateResult = kObj.IsMatch(*pkCondition);
		if(bStateResult == false)
		{
			kCaster.Appearance().SendMessage(kCaster.Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_TARGET_IS_FREE));
			return false;
		}
	}
	if(kCaster.IsInFront(kObj.Position().x, kObj.Position().y) == false)
	{
		kCaster.Appearance().SendMessage(kCaster.Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_TARGET_NOT_IN_FRONT));
		return false;
	}
	if(CheckRange(kCaster.Appearance(), kObj, kInfo) == false)
	{
		kCaster.Appearance().SendMessage(kCaster.Appearance().GetSpellMessage(FvGameMessageIdx::SPELL_FAILED_TARGET_OUT_RANGE));
		return false;
	}
	return true;
}
bool FvUnitSpell::_CheckTarget(const FvGameUnitLogic& kCaster, const FvVector3& kPos, const FvSpellInfo& kInfo)
{
	if(CheckRange(kCaster.Appearance(), kPos, kInfo)  == false)
	{
		return false;
	}
	return true;
}
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
//! ×´Ì¬¼ì²â
bool FvUnitSpell::_CheckPower(const FvGameUnitAppearance& kObj, const FvSpellInfo& kInfo)
{
	const FvSpellProcInfo& kProc = kInfo.m_kProcInfo;
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellProcInfo::COST_MAX; ++uiIdx)
	{
		const FvSPellCost& kCost = kProc.m_kCost[uiIdx];
		if (!kCost.IsEmpty())
		{
			const FvInt32 iValue = kObj.GetValue32(kCost.m_kValue.m_iType);
			const FvInt32 iCost = kCost.m_kValue.m_iValue + kObj.GetValue32(kCost.m_kRefValue);
			if(iCost > iValue)
			{
				return false;
			}
		}
	}
	return true;
}
void FvUnitSpell::_CostPower(const FvSpellInfo& kInfo)
{
	const FvSpellProcInfo& kProc = kInfo.m_kProcInfo;
	for (FvUInt32 uiIdx = 0; uiIdx < FvSpellProcInfo::COST_MAX; ++uiIdx)
	{
		const FvSPellCost& kCost = kProc.m_kCost[uiIdx];
		if (!kCost.IsEmpty())
		{
			const FvInt32 iValue = Self().Appearance().GetValue32(kCost.m_kValue.m_iType);
			const FvInt32 iCost = kCost.m_kValue.m_iValue + Self().Appearance().GetValue32(kCost.m_kRefValue);
			Self().ModifyValue32(kCost.m_kValue.m_iType, -iCost);
		}
	}
}
bool FvUnitSpell::_CheckItemInBag()const
{
	return true;
}
bool FvUnitSpell::_CheckItemInEquip()const
{
	return true;
}
bool FvUnitSpell::_CheckAuraState()const
{
	return true;
}

//+-----------------------------------------------------------------------------------------------------------------------------------------------------

bool FvUnitSpell::CheckRange(const FvGameUnitAppearance& kCaster, const FvVector3& kPos, const FvSpellInfo& kInfo)
{
	const FvVector3 kRootPos = kCaster.Position();
	const float len2 = FvGeographicObject::GetDistance2(kPos, kRootPos);
	const FvSpellProcInfo& kProc = kInfo.m_kProcInfo;
	const float minLen = kProc.m_iMinRange*0.01f;
	if(len2 < minLen*minLen)
	{
		FvLogBus::CritOk("ViCltSpell(%f, %f, %f)::CheckRange FAIL Pos(%f, %f, %f) Len(%f)¾àÀëÌ«Ð¡ MinLen[%f]", 
			kRootPos.x, kRootPos.y, kRootPos.z,
			kPos.x, kPos.y, kPos.z, sqrt(len2), minLen);
		return false;
	}
	const float maxLen = kProc.m_iMaxRange*0.01f + kCaster.BodyRadius();
	if (len2 > maxLen*maxLen)
	{
		FvLogBus::CritOk("ViCltSpell(%f, %f, %f)::CheckRange FAIL Pos(%f, %f, %f) Len(%f)¾àÀëÌ«´ó MaxLen[%f]", 
			kRootPos.x, kRootPos.y, kRootPos.z,
			kPos.x, kPos.y, kPos.z, sqrt(len2), maxLen);
		return false;
	}
	return true;
}

bool FvUnitSpell::CheckRange(const FvGameUnitAppearance& kCaster, const FvGameUnitAppearance& kObj, const FvSpellInfo& kInfo)
{
	const FvVector3 kRootPos = kCaster.Position();
	const float len2 = FvGeographicObject::GetDistance2(kObj.Position(), kRootPos);
	const FvSpellProcInfo& kProc = kInfo.m_kProcInfo;
	const float minLen = kProc.m_iMinRange*0.01f;
	if(len2 < minLen*minLen)
	{
		FvLogBus::CritOk("ViCltSpell(%f, %f, %f)::CheckRange FAIL Obj(%f, %f, %f) Len(%f)¾àÀëÌ«Ð¡ MinLen[%f]", 
			kRootPos.x, kRootPos.y, kRootPos.z,
			kObj.Position().x, kObj.Position().y, kObj.Position().z, sqrt(len2), minLen);
		return false;
	}
	const float maxLen = kProc.m_iMaxRange*0.01f + kCaster.BodyRadius() + kObj.BodyRadius();
	if (len2 > maxLen*maxLen)
	{
		FvLogBus::CritOk("ViCltSpell(%f, %f, %f)::CheckRange FAIL Obj(%f, %f, %f) Len(%f)¾àÀëÌ«´ó MaxLen[%f]", 
			kRootPos.x, kRootPos.y, kRootPos.z,
			kObj.Position().x, kObj.Position().y, kObj.Position().z, sqrt(len2), maxLen);
		return false;
	}
	return true;
}
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
void FvUnitSpell::OnStateError()
{
	Cancel();
}
void FvUnitSpell::OnStateOK(){}

const FvTarget& FvUnitSpell::GetTarget()const{return m_kTarget;}

