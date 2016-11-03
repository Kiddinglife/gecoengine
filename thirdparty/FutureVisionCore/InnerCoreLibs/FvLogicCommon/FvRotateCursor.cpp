#include "FvRotateCursor.h"

#include "FvLogBus.h"
#include <FvAngle.h>

FvRotateCursor::FvRotateCursor(void)
:m_fAllowAngle(0.7f)
,m_fCursorYaw(0.0f)
,m_iAccordSign(0)
,m_bAccordSpd(3.0f)
,m_bActive(false)
{
	m_spBodyDirection = new FvProvider1<float>(0.0f);
	m_spFeetDirection = new FvProvider1<float>(0.0f);
}

FvRotateCursor::~FvRotateCursor(void)
{
}
void FvRotateCursor::SetAllowAngle(const float fAngle)
{
	m_fAllowAngle = fAngle;
}
void FvRotateCursor::SetAccordSpd(const float fAngleSpd)
{
	m_bAccordSpd = fAngleSpd;
}
void FvRotateCursor::ForceAccord()
{
	if(m_fCursorYaw == 0)
	{
		return;
	}
	if(m_iAccordSign == 0)
	{
		if(m_fCursorYaw > 0.0f)
		{
			m_iAccordSign = 1;
		}
		else
		{
			m_iAccordSign = -1;
		}
		OnRoteStart(m_iAccordSign);
	}
}	
bool FvRotateCursor::Tick(const float fDeltaTime)
{
	if(m_spControlDirection == NULL)
	{
		return false;
	}
	bool bResult = false;
	const float fYaw = m_spControlDirection->GetData0().Yaw();
	float fFeetDirection = m_spFeetDirection->GetData0();
	const float fOldCursorYaw = m_fCursorYaw;
	m_fCursorYaw = fYaw - fFeetDirection;
	FvAngle::Normalise(m_fCursorYaw);
	const FvInt32 iFomated = _FomatByMaxAngle(fFeetDirection, fYaw);
	if(fOldCursorYaw != m_fCursorYaw)
	{
		OnRoteUpdated();
	}
	if(iFomated)
	{
		if(m_iAccordSign == 0)
		{
			m_iAccordSign = iFomated;
			OnRoteStart(m_iAccordSign);
		}
		bResult = true;
	}

	if(m_iAccordSign != 0)
	{
		const float fFrameDeltaYaw = m_bAccordSpd*fDeltaTime;
		if(m_fCursorYaw > fFrameDeltaYaw)
		{
			fFeetDirection += fFrameDeltaYaw;
			FvAngle::Normalise(fFeetDirection);
			bResult = true;
		}
		else if(m_fCursorYaw < -fFrameDeltaYaw)
		{
			fFeetDirection -= fFrameDeltaYaw;
			FvAngle::Normalise(fFeetDirection);
			bResult = true;
		}
		else
		{
			fFeetDirection = fYaw;
			OnRoteEnd(m_iAccordSign);
			m_iAccordSign = 0;
		}
	}

	m_spFeetDirection->SetValue0(fFeetDirection);
	m_spBodyDirection->SetValue0((fFeetDirection+fYaw)*0.5f);

	return bResult;
}
FvInt32 FvRotateCursor::_FomatByMaxAngle(float& fFeetDirection, const float fBodyYaw)
{
	if(m_fCursorYaw > m_fAllowAngle)
	{
		fFeetDirection = fBodyYaw - m_fAllowAngle;
		m_fCursorYaw = m_fAllowAngle;
		FvAngle::Normalise(fFeetDirection);
		return 1;
	}
	else if(m_fCursorYaw < -m_fAllowAngle)
	{
		fFeetDirection = fBodyYaw + m_fAllowAngle;
		m_fCursorYaw = -m_fAllowAngle;
		FvAngle::Normalise(fFeetDirection);
		return -1;
	}
	return 0;
}
float FvRotateCursor::GetCursorYaw()const
{
	return m_fCursorYaw;
}
float FvRotateCursor::FeetDirection()const
{
	return m_spFeetDirection->GetData0();
}
float FvRotateCursor::BodyDirection()const
{
	return m_spBodyDirection->GetData0();
}
void FvRotateCursor::FastAccord()
{
	if(&*m_spControlDirection)
	{
		const float fFeetDirection = m_spControlDirection->GetData0().Yaw();
		m_spFeetDirection->SetValue0(fFeetDirection);
		m_spBodyDirection->SetValue0(fFeetDirection);
	}
}
void FvRotateCursor::SetControlDirecion(const FvSmartPointer<FvProvider1<FvDirection3>>& spProvider)
{
	m_spControlDirection = spProvider;
}
const FvSmartPointer<FvProvider1<float>>& FvRotateCursor::FeetDirectionProvider()const
{
	return m_spFeetDirection;
}
const FvSmartPointer<FvProvider1<float>>& FvRotateCursor::BodyDirectionProvider()const
{
	return m_spBodyDirection;
}


bool FvRotateCursor::IsActive()const{return m_bActive;}
void FvRotateCursor::SetActive(const bool bActive){m_bActive = bActive;}