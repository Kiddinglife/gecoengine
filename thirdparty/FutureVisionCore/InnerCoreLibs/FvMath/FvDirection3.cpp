#include "FvDirection3.h"

#include "FvAngle.h"

FvDirection3::FvDirection3()
:m_fYaw(0.0f), m_fPitch(0.0f), m_fRoll(0.0f)
{}	
FvDirection3::FvDirection3(const float fYaw, const float fPitch, const float fRoll)
:m_fYaw(fYaw), m_fPitch(fPitch), m_fRoll(fRoll)
{
	FvAngle::Normalise(m_fYaw);
	FvAngle::Normalise(m_fPitch);
	FvAngle::Normalise(m_fRoll);
}	
float FvDirection3::Yaw()const{return m_fYaw;}
float FvDirection3::Pitch()const{return m_fPitch;}
float FvDirection3::Roll()const{return m_fRoll;}
void FvDirection3::SetYaw(const float fYaw)
{
	m_fYaw = fYaw;
	FvAngle::Normalise(m_fYaw);
}
void FvDirection3::SetPitch(const float fPitch)
{
	m_fPitch = fPitch;
	FvAngle::Normalise(m_fPitch);
}
void FvDirection3::SetRoll(const float fRoll)
{
	m_fRoll = fRoll;
	FvAngle::Normalise(m_fRoll);
}
bool FvDirection3::IsEqual(const FvDirection3& kOther)const
{
	return (m_fYaw == kOther.m_fYaw) && (m_fPitch == kOther.m_fPitch) && (m_fRoll == kOther.m_fRoll);
}
bool FvDirection3::IsEqual(const FvDirection3& kOne, const FvDirection3& kOther)
{
	return (kOne.m_fYaw == kOther.m_fYaw) && (kOne.m_fPitch == kOther.m_fPitch) && (kOne.m_fRoll == kOther.m_fRoll);
}
