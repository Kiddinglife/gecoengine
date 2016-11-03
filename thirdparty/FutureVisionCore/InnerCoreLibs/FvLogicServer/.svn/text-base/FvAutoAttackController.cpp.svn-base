#include "FvAutoAttackController.h"


#include <FvGameTimer.h>
#include <FvGameRandom.h>
#include <FvLogBus.h>

//+-----------------------------------------------------------------------------------------------------------------------

FvTickTaskList<FvAutoAttackController, &FvAutoAttackController::_OnCheckTime> FvAutoAttackController::ms_TaskList;

FvAutoAttackController::FvAutoAttackController(void)
:m_uiSpanTime(150)
#pragma warning(push)
#pragma warning(disable: 4355)
,m_NextAttackTime(*this)
,m_kCheckTask(*this)
,m_kActionEndAsyn_CB(*this)
#pragma warning(pop)
,m_eState(DEACTIVE)
{
	m_kCaller.SetEventSize(EVENT_SUP);
}

FvAutoAttackController::~FvAutoAttackController(void)
{
}
//+-----------------------------------------------------------------------------------------------------------------------
void FvAutoAttackController::SetAttackSpan(const FvUInt32 uiSpanTime)
{
	m_uiSpanTime = uiSpanTime;
}
void FvAutoAttackController::_OnNextAttackTime(FvTimeEventNodeInterface&)
{
	Check();
}
//+-----------------------------------------------------------------------------------------------------------------------
void FvAutoAttackController::Start()
{
	FvLogBus::CritOk("FvAutoAttackController::Start TIME[%I64u]", FvGameTimer::Time());
	if(m_eState != ACTIVE)
	{
		m_eState = ACTIVE;
		if(m_uiSpanTime && m_NextAttackTime.IsAttach() == false && m_kActionEndAsyn_CB.IsAttach() == false)
		{
			Check();
		}
	}
}
void FvAutoAttackController::End()
{
	FvLogBus::CritOk("FvAutoAttackController::End TIME[%I64u]", FvGameTimer::Time());
	if(m_eState != DEACTIVE)
	{
		m_eState = DEACTIVE;
		_OnEnd();
		m_NextAttackTime.Detach();
		m_kCheckTask.Detach();
	}
}
void FvAutoAttackController::_OnEnd()
{

}
//+-----------------------------------------------------------------------------------------------------------------------
void FvAutoAttackController::Check()
{
	if(IsEndAttack())
	{
		m_kCaller.Invoke(TARGET_ERROR, *this);
		End();
		return;
	}
	if(OnNextAttackTime())
	{
		const FvUInt32 uiSpan = FvGameRandom::GetValueByRange(m_uiSpanTime, 20);
		FvGameTimer::SetTime(uiSpan, m_NextAttackTime, &FvAutoAttackController::_OnNextAttackTime);
		m_kCheckTask.Detach();
	}
	else
	{
		ms_TaskList.AttachBack(m_kCheckTask);
	}
}
void FvAutoAttackController::_OnCheckTime(const float)
{
	Check();
}
//+-----------------------------------------------------------------------------------------------------------------------
void FvAutoAttackController::Wait(FvUnitAct& kAction)
{
	FvLogBus::CritOk("FvAutoAttackController::Wait TIME[%I64u]", FvGameTimer::Time());
	kAction.CallbackList().Attach(m_kActionEndAsyn_CB);
	m_NextAttackTime.ClearFunc();
	m_kCheckTask.Detach();
	//!
	_OnPause();
}
void FvAutoAttackController::_OnActionEnd(const FvUInt32)
{
	FvLogBus::CritOk("FvAutoAttackController::_OnActionEnd TIME[%I64u]", FvGameTimer::Time());
	m_kActionEndAsyn_CB.Detach();
	m_NextAttackTime.SetFunc(&FvAutoAttackController::_OnNextAttackTime);
	if(m_NextAttackTime.IsAttach() == false)
	{
		if(m_eState== ACTIVE)
		{
			const FvUInt32 uiSpan = FvGameRandom::GetValueByRange(m_uiSpanTime/2, 20);
			FvGameTimer::SetTime(uiSpan, m_NextAttackTime, &FvAutoAttackController::_OnNextAttackTime);
		}
	}
	else
	{
		const FvUInt64 uiReserveTime = m_NextAttackTime.GetReserveDuration(FvGameTimer::Timer());
		const FvUInt64 uiSpanMin = m_uiSpanTime/2;
		if(uiReserveTime < uiSpanMin)
		{
			FvGameTimer::SetTime(uiSpanMin, m_NextAttackTime, &FvAutoAttackController::_OnNextAttackTime);
		}
		//FvLogBus::CritOk("FvAutoAttackController::攻击时间间隔未完");	
	}
}
//+-----------------------------------------------------------------------------------------------------------------------
