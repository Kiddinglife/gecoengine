#include "FvAura.h"

#include "FvGameUnitAppearance.h"
#include "FvLogicGameData.h"

#include <FvDestroyManager.h>
#include <FvGameTimer.h>
#include <FvGameRandom.h>


FvInt32 FvAura::GetCost(const FvGameUnitAppearance& kObj, const FvIdxValueRange& kCost, const FvRefIdxValue& kRef0Cost, const FvRefIdxValue& kRef1Cost)
{
	FvInt32 iValue = FvGameRandom::GetValue(kCost.m_iValueInf, kCost.m_iValueSup);
	iValue += kObj.GetValue32(kRef0Cost);
	iValue += kObj.GetValue32(kRef1Cost);
	return iValue;
}

bool FvAura::CanDispel(FvAura& kAura)
{
	return true;
}
FvUInt32 FvAura::GetDuration(const FvAuraInfo& kInfo)
{
	return ((kInfo.m_iTickCnt+1)*kInfo.m_iAmplitude);
}


FvAura::FvAura()
:m_rpInfo(NULL)
#pragma warning(push)
#pragma warning(disable: 4355)
,m_kTimeTickEvent(*this)
,m_kTimeStartEvent(*this)
#pragma warning(pop)
{

}

FvAura::~FvAura(void)
{
	
}

//+----------------------------------------------------------------------------------------------------
void FvAura::Start()
{
	_DelState(FvAuraState::ACTIVE);
	_OnStart();
	if(GetInfo().m_iAmplitude)//! 无限时间不设置定时器
	{
		m_kTimeTickEvent.Start(FvGameTimer::Timer(), GetInfo().m_iTickCnt+1, GetInfo().m_iAmplitude);
		_SetEndTime(FvGameTimer::Time() + GetDuration(GetInfo()));	
	}
}
void FvAura::Start(const FvUInt32 uiDelayTime)
{
	FvGameTimer::SetTime(uiDelayTime, m_kTimeStartEvent);
}
void FvAura::End()
{
	if(HasState(FvAuraState::END))
	{
		return;
	}
	_AddState(FvAuraState::END);
	_OnEnd();
	_Destroy();
}
void FvAura::Load(const FvUInt64 uiDuration, const bool bUpdate)
{
	if(GetInfo().m_iAmplitude)//! 无限时间不设置定时器
	{
		m_kTimeTickEvent.SetReserveTime(FvGameTimer::Timer(), GetInfo().m_iAmplitude, uiDuration);
		_SetEndTime(FvGameTimer::Time() + uiDuration);
	}
	_DelState(FvAuraState::ACTIVE);
	_OnLoad(bUpdate);
}
void FvAura::Save(const bool bUpdate)
{
	_OnSave(bUpdate);
	_Destroy();
}
void FvAura::End(const FvUInt32 uiDelayTime)
{
	FV_ASSERT_ERROR(0 && "未实现");
}
//+----------------------------------------------------------------------------------------------------
void FvAura::SetDurTime(const FvUInt32 uiDuration)
{
	m_kTimeTickEvent.SetReserveTime(FvGameTimer::Timer(), GetInfo().m_iAmplitude, uiDuration);
	_SetEndTime(uiDuration+FvGameTimer::Time());
}
void FvAura::ModDurTime(const FvInt32 iDeltaTime)
{
	FvUInt64 uiReserveTime = 0;
	m_kTimeTickEvent.GetReserveTime(FvGameTimer::Timer(), uiReserveTime);
	if(uiReserveTime > -iDeltaTime)
	{
		m_kTimeTickEvent.SetReserveTime(FvGameTimer::Timer(), GetInfo().m_iAmplitude, 0);		
		_SetEndTime(FvGameTimer::Time());
	}
	else
	{
		uiReserveTime += iDeltaTime;
		m_kTimeTickEvent.SetReserveTime(FvGameTimer::Timer(), GetInfo().m_iAmplitude, uiReserveTime);
		_SetEndTime(FvGameTimer::Time() + uiReserveTime+FvGameTimer::Time());
	}
}
//+----------------------------------------------------------------------------------------------------
void FvAura::_OnStart()
{
	Active(true);
}
void FvAura::_OnEnd()
{
	Deactive(true);
}
//+----------------------------------------------------------------------------------------------------
void FvAura::_OnLoad(const bool bUpdate)
{
	Active(bUpdate);
}
void FvAura::_OnSave(const bool bUpdate)
{
	Deactive(bUpdate);
}
//+----------------------------------------------------------------------------------------------------
void FvAura::Active(const bool bUpdate)
{
	if(HasState(FvAuraState::ACTIVE))
	{
		return;
	}
	_AddState(FvAuraState::ACTIVE);
	_OnActive(bUpdate);
}
void FvAura::Deactive(const bool bUpdate)
{
	if(!HasState(FvAuraState::ACTIVE))
	{
		return;
	}
	_DelState(FvAuraState::ACTIVE);
	_OnDeactive(bUpdate);
}

//+----------------------------------------------------------------------------------------------------
bool FvAura::HasState(const FvAuraState::Mask& eMask)const
{
	const FvUInt32 uiState = GetState();
	return FvMask::HasAny(uiState, eMask);
}
void FvAura::_AddState(const FvAuraState::Mask& eMask)
{
	FvUInt32 uiState = GetState();
	FvMask::Add(uiState, eMask);
	_SetState(uiState);
}
void FvAura::_DelState(const FvAuraState::Mask& eMask)
{
	FvUInt32 uiState = GetState();
	FvMask::Del(uiState, eMask);
	_SetState(uiState);
}
//+----------------------------------------------------------------------------------------------------
FvUInt64 FvAura::GetDuration()const
{
	FvUInt64 uiDuration = 0;
	m_kTimeTickEvent.GetReserveTime(FvGameTimer::Timer(), uiDuration);
	return uiDuration;
}

//+------------------------------------------------------------------------------------------------------------------------------------------
//! 时间驱动
void FvAura::_OnStart(FvTimeEventNodeInterface&)
{
	Start();
}
void FvAura::_OnTimeTick(FvTimeEventNodeInterface&)
{
	//! 计算定时次数
	if(HasState(FvAuraState::ACTIVE))
	{
		_OnTick();
	}
}

void FvAura::_OnEnd(FvTimeEventNodeInterface&)
{
	End();
}
//+------------------------------------------------------------------------------------------------------------------------------------------
void FvAura::_Destroy()
{
	m_kTimeStartEvent.Detach();
	m_kTimeTickEvent.Detach();
	_ClearReference();
	FvDestroyManager::Destroy(*this);
}
//+------------------------------------------------------------------------------------------------------------------------------------------
const FvAuraInfo& FvAura::GetInfo()const
{
	FV_ASSERT_ERROR(m_rpInfo);
	return *m_rpInfo;
}
bool FvAura::GetMiscValue(const char* pcName, FvInt32& iValue)const
{
	return GetInfo().m_kMiscValue.GetValue(pcName, iValue);
}
FvInt32 FvAura::MiscValue(const char* pcName, const FvInt32 iDefaultValue/* = 0*/)const
{
	return GetInfo().m_kMiscValue.Value(pcName, iDefaultValue);
}
bool FvAura::IsOwn()const
{
	return HasState(FvAuraState::OWN);
}
void FvAura::SetOwn(const bool bOwn)
{
	if(bOwn)
	{
		_AddState(FvAuraState::OWN);
	}
	else
	{
		_DelState(FvAuraState::OWN);
	}
}