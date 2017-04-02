#include "geco-math-direction.h"

//----------------------------------------------------------------------------
GecoAngle::GecoAngle()
{
	m_fAngle = 0.0f;
}
//----------------------------------------------------------------------------
GecoAngle::GecoAngle(const GecoAngle& rAngle)
{
	m_fAngle = rAngle.m_fAngle;
}
//----------------------------------------------------------------------------
GecoAngle::GecoAngle(float fValueInRadians)
{
	m_fAngle = fValueInRadians;
	Normalise();
}
//----------------------------------------------------------------------------
GecoAngle::GecoAngle(GecoRadians fValue)
{
	m_fAngle = fValue.m_fRadians;
	Normalise();
}
//----------------------------------------------------------------------------
GecoAngle::GecoAngle(GecoDegree fValue)
{
	m_fAngle = GecoDegreeToRadiantf(fValue.m_fDegree);
	Normalise();
}
//----------------------------------------------------------------------------
GecoAngle::~GecoAngle()
{

}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator = (GecoAngle kAngle)
{
	m_fAngle = kAngle.m_fAngle;
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator = (float fValueInRadians)
{
	m_fAngle = fValueInRadians;
	Normalise();
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator = (GecoRadians fValue)
{
	m_fAngle = fValue.m_fRadians;
	Normalise();
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator = (GecoDegree fValue)
{
	m_fAngle = GecoDegreeToRadiantf(fValue.m_fDegree);
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator += (GecoAngle kAngle)
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle + kAngle.m_fAngle);
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator += (float fValueInRadians)
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle + fValueInRadians);
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator += (GecoRadians fValue)
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle + fValue.m_fRadians);
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator += (GecoDegree fValue)
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle +
		GecoDegreeToRadiantf(fValue.m_fDegree));
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator -= (GecoAngle kAngle)
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle - kAngle.m_fAngle);
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator -= (float fValueInRadians)
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle - fValueInRadians);
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator -= (GecoRadians fValue)
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle - fValue.m_fRadians);
	return *this;
}
//----------------------------------------------------------------------------
const GecoAngle& GecoAngle::operator -= (GecoDegree fValue)
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle -
		GecoDegreeToRadiantf(fValue.m_fDegree));
	return *this;
}
//----------------------------------------------------------------------------
//GecoAngle::operator float& ()
//{
//	return m_fAngle;
//}
//----------------------------------------------------------------------------
GecoAngle::operator float() const
{
	return m_fAngle;
}
float GecoAngle::GetAngle()const
{
	return m_fAngle;
}
//----------------------------------------------------------------------------
void GecoAngle::Normalise()
{
	m_fAngle = GecoNormaliseAnglef(m_fAngle);
}
void GecoAngle::Normalise(float& fAngle)
{
	fAngle = GecoNormaliseAnglef(fAngle);
}
//----------------------------------------------------------------------------
GecoAngle GecoAngle::Decay(GecoAngle kSrc, GecoAngle kDst, float fHalfLife,
	float fDeltaTime)
{
	float fDst = SameSignAngle(kSrc, kDst);
	return GecoDecayf(kSrc, fDst, fHalfLife, fDeltaTime);
}
//----------------------------------------------------------------------------
float GecoAngle::SameSignAngle(GecoAngle kAngle, GecoAngle kClosest)
{
	return GecoSameSignAnglef(kAngle, kClosest);
}
//----------------------------------------------------------------------------
GecoAngle GecoAngle::TurnRange(GecoAngle kFrom, GecoAngle kTo)
{
	return GecoTurnRangeAnglef(kFrom, kTo);
}
//----------------------------------------------------------------------------
bool GecoAngle::ClampBetween(GecoAngle kMin, GecoAngle kMax)
{
	float fDiff = TurnRange(kMin, *this);
	float fRange = TurnRange(kMin, kMax);

	if (fDiff > fRange)
	{
		m_fAngle = (fDiff - fRange * 0.5f < GECO_MATH_PI_F) ? kMax : kMin;
		Normalise();
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------
bool GecoAngle::IsBetween(GecoAngle kMin, GecoAngle kMax) const
{
	float fDiff = TurnRange(kMin, *this);
	float fRange = TurnRange(kMin, kMax);

	return (fDiff <= fRange);
}
//----------------------------------------------------------------------------
void GecoAngle::Lerp(GecoAngle a, GecoAngle b, float t)
{
	*this = GECO_LERP(t, a, SameSignAngle(a, b));
	Normalise();
}
//----------------------------------------------------------------------------

GecoDirection3::GecoDirection3()
	:m_fYaw(0.0f), m_fPitch(0.0f), m_fRoll(0.0f)
{}
GecoDirection3::GecoDirection3(const float fYaw, const float fPitch, const float fRoll)
	: m_fYaw(fYaw), m_fPitch(fPitch), m_fRoll(fRoll)
{
	GecoAngle::Normalise(m_fYaw);
	GecoAngle::Normalise(m_fPitch);
	GecoAngle::Normalise(m_fRoll);
}
float GecoDirection3::Yaw()const { return m_fYaw; }
float GecoDirection3::Pitch()const { return m_fPitch; }
float GecoDirection3::Roll()const { return m_fRoll; }
void GecoDirection3::SetYaw(const float fYaw)
{
	m_fYaw = fYaw;
	GecoAngle::Normalise(m_fYaw);
}
void GecoDirection3::SetPitch(const float fPitch)
{
	m_fPitch = fPitch;
	GecoAngle::Normalise(m_fPitch);
}
void GecoDirection3::SetRoll(const float fRoll)
{
	m_fRoll = fRoll;
	GecoAngle::Normalise(m_fRoll);
}
bool GecoDirection3::IsEqual(const GecoDirection3& kOther)const
{
	return (m_fYaw == kOther.m_fYaw) && (m_fPitch == kOther.m_fPitch) && (m_fRoll == kOther.m_fRoll);
}
bool GecoDirection3::IsEqual(const GecoDirection3& kOne, const GecoDirection3& kOther)
{
	return (kOne.m_fYaw == kOther.m_fYaw) && (kOne.m_fPitch == kOther.m_fPitch) && (kOne.m_fRoll == kOther.m_fRoll);
}