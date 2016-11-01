#include "FvAngleCursor.h"

#include <FvMath.h>
#include <FvAngle.h>

FvAngleCursor::FvAngleCursor(void)
:m_fCurDiretion(0.0f)
,m_fRotSpd(20.0f)
,m_fRotDirection(0.0f)
{
}

FvAngleCursor::~FvAngleCursor(void)
{
}
bool FvAngleCursor::Update(const float fDeltaTime, const float fNewDir)
{
	const float fNewDirMod = m_fRotDirection + fNewDir;
	if(fNewDirMod == m_fCurDiretion)
	{
		return false;
	}
	const float fMaxRot = fDeltaTime*m_fRotSpd;
	float fDeltaAngle = fNewDirMod - m_fCurDiretion;
	FvAngle::Normalise(fDeltaAngle);
	if(fabs(fDeltaAngle) < fMaxRot)
	{
		m_fCurDiretion = fNewDirMod;
	}
	else
	{
		m_fCurDiretion += fMaxRot*FvMathTool::Sign(fDeltaAngle);
	}
	return true;
}

void FvAngleCursor::SetDirection(const float fNewDir)
{
	m_fCurDiretion = fNewDir;
	FvAngle::Normalise(m_fCurDiretion);
}
void FvAngleCursor::SetRotDirection(const float fDeltaAngle)
{
	m_fRotDirection = fDeltaAngle;
	FvAngle::Normalise(m_fRotDirection);
}
float FvAngleCursor::Direction()const
{
	return m_fCurDiretion;
}
void FvAngleCursor::SetRotSpd(const float fSpd)
{
	m_fRotSpd = FvMathTool::Max(1.0f, fabs(fSpd));
}