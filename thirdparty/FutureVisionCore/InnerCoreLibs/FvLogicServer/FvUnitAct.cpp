#include "FvUnitAct.h"

#include <FvGameTimer.h>

FvUnitAct::FvUnitAct(void)
:m_eState(DEACTIVE)
#pragma warning(push)
#pragma warning(disable: 4355)
,m_kLockTime(*this)
#pragma warning(pop)
{
}

FvUnitAct::~FvUnitAct(void)
{
}

//+-----------------------------------------------------------------------------------------------------------------------

void FvUnitAct::End()
{
	if(m_eState == ACTIVE)
	{
		m_eState = DEACTIVE;
		_OnEnd();
		m_kCallbackList.Invoke(0, *this);
	}
}
void FvUnitAct::Active()
{
	m_eState = ACTIVE;
}
bool FvUnitAct::IsActive()const
{
	return (m_eState == ACTIVE);
}
void FvUnitAct::Lock(const float fDuration)
{
	FvGameTimer::SetTime(fDuration, m_kLockTime);
}