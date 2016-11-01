//{future source message}
#include "FvAngle.h"

//----------------------------------------------------------------------------
FvAngle::FvAngle()
{
	m_fAngle = 0.0f;
}
//----------------------------------------------------------------------------
FvAngle::FvAngle(const FvAngle& rAngle)
{
	m_fAngle = rAngle.m_fAngle;
}
//----------------------------------------------------------------------------
FvAngle::FvAngle(float fValueInRadians)
{
	m_fAngle = fValueInRadians;
	Normalise();
}
//----------------------------------------------------------------------------
FvAngle::FvAngle(FvRadians fValue)
{
	m_fAngle = fValue.m_fRadians;
	Normalise();
}
//----------------------------------------------------------------------------
FvAngle::FvAngle(FvDegree fValue)
{
	m_fAngle = FvDegreeToRadiantf(fValue.m_fDegree);
	Normalise();
}
//----------------------------------------------------------------------------
FvAngle::~FvAngle()
{

}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator = (FvAngle kAngle)
{
	m_fAngle = kAngle.m_fAngle;
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator = (float fValueInRadians)
{
	m_fAngle = fValueInRadians;
	Normalise();
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator = (FvRadians fValue)
{
	m_fAngle = fValue.m_fRadians;
	Normalise();
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator = (FvDegree fValue)
{
	m_fAngle = FvDegreeToRadiantf(fValue.m_fDegree);
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator += (FvAngle kAngle)
{
	m_fAngle = FvNormaliseAnglef(m_fAngle + kAngle.m_fAngle);
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator += (float fValueInRadians)
{
	m_fAngle = FvNormaliseAnglef(m_fAngle + fValueInRadians);
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator += (FvRadians fValue)
{
	m_fAngle = FvNormaliseAnglef(m_fAngle + fValue.m_fRadians);
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator += (FvDegree fValue)
{
	m_fAngle = FvNormaliseAnglef(m_fAngle +
		FvDegreeToRadiantf(fValue.m_fDegree));
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator -= (FvAngle kAngle)
{
	m_fAngle = FvNormaliseAnglef(m_fAngle - kAngle.m_fAngle);
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator -= (float fValueInRadians)
{
	m_fAngle = FvNormaliseAnglef(m_fAngle - fValueInRadians);
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator -= (FvRadians fValue)
{
	m_fAngle = FvNormaliseAnglef(m_fAngle - fValue.m_fRadians);
	return *this;
}
//----------------------------------------------------------------------------
const FvAngle& FvAngle::operator -= (FvDegree fValue)
{
	m_fAngle = FvNormaliseAnglef(m_fAngle -
		FvDegreeToRadiantf(fValue.m_fDegree));
	return *this;
}
//----------------------------------------------------------------------------
//FvAngle::operator float& ()
//{
//	return m_fAngle;
//}
//----------------------------------------------------------------------------
FvAngle::operator float () const
{
	return m_fAngle;
}
float FvAngle::GetAngle()const
{
	return m_fAngle;
}
//----------------------------------------------------------------------------
void FvAngle::Normalise()
{
	m_fAngle = FvNormaliseAnglef(m_fAngle);
}
void FvAngle::Normalise(float& fAngle)
{
	fAngle = FvNormaliseAnglef(fAngle);
}
//----------------------------------------------------------------------------
FvAngle FvAngle::Decay(FvAngle kSrc, FvAngle kDst, float fHalfLife,
	float fDeltaTime)
{
	float fDst = SameSignAngle(kSrc, kDst);
	return FvDecayf(kSrc, fDst, fHalfLife, fDeltaTime);
}
//----------------------------------------------------------------------------
float FvAngle::SameSignAngle(FvAngle kAngle, FvAngle kClosest)
{
	return FvSameSignAnglef(kAngle, kClosest);
}
//----------------------------------------------------------------------------
FvAngle FvAngle::TurnRange(FvAngle kFrom, FvAngle kTo)
{
	return FvTurnRangeAnglef(kFrom, kTo);
}
//----------------------------------------------------------------------------
bool FvAngle::ClampBetween(FvAngle kMin, FvAngle kMax)
{
	float fDiff  = TurnRange(kMin, *this);
	float fRange = TurnRange(kMin, kMax);

	if (fDiff > fRange)
	{
		m_fAngle = (fDiff - fRange * 0.5f < FV_MATH_PI_F) ? kMax : kMin;
		Normalise();
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------
bool FvAngle::IsBetween(FvAngle kMin, FvAngle kMax) const
{
	float fDiff  = TurnRange(kMin, *this);
	float fRange = TurnRange(kMin, kMax);

	return (fDiff <= fRange);
}
//----------------------------------------------------------------------------
void FvAngle::Lerp(FvAngle a, FvAngle b, float t)
{
	*this = FV_LERP(t, a, SameSignAngle(a, b));
	Normalise();
}
//----------------------------------------------------------------------------
