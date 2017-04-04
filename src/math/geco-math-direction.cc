#include "geco-math-direction.h"

struct GecoDegree
{
	GecoDegree(float fValue)
	{
		m_fDegree = fValue;
	}

	float m_fDegree;
};

struct GecoRadians
{
	GecoRadians(float fValue)
	{
		m_fRadians = fValue;
	}

	float m_fRadians;
};

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
//geco_bit_stream_t & operator >> (geco_bit_stream_t& kIS, GecoDirection3& kDes)
//{
//	if (kIS.is_compression_mode())
//	{
//		kIS.read_ranged_float(kDes.m_fYaw, -GECO_MATH_PI_F, GECO_MATH_PI_F);
//		kIS.read_ranged_float(kDes.m_fPitch, -GECO_MATH_PI_F, GECO_MATH_PI_F);
//		kIS.read_ranged_float(kDes.m_fRoll, -GECO_MATH_PI_F, GECO_MATH_PI_F);
//	}
//	else
//	{
//		kIS.Read(kDes.m_fYaw);
//		kIS.Read(kDes.m_fPitch);
//		kIS.Read(kDes.m_fRoll);
//	}
//	return kIS;
//}
//geco_bit_stream_t& operator << (geco_bit_stream_t& kOS, const GecoDirection3& kDes)
//{
//	if (kOS.is_compression_mode())
//	{
//		kOS.write_ranged_float(kDes.m_fYaw, -GECO_MATH_PI_F, GECO_MATH_PI_F);
//		kOS.write_ranged_float(kDes.m_fPitch, -GECO_MATH_PI_F, GECO_MATH_PI_F);
//		kOS.write_ranged_float(kDes.m_fRoll, -GECO_MATH_PI_F, GECO_MATH_PI_F);
//	}
//	else
//	{
//		kOS.Write(kDes.m_fYaw);
//		kOS.Write(kDes.m_fPitch);
//		kOS.Write(kDes.m_fRoll);
//	}
//	return kOS;
//}

uchar AngleToInt8(float angle)
{
	return (uchar)floorf((angle * 128.f) / GECO_MATH_PI_F + 0.5f);
}
float Int8ToAngle(uchar angle)
{
	return float(angle) * (GECO_MATH_PI_F / 128.f);
}
uchar HalfAngleToInt8(float angle)
{
	return (uchar)floorf((angle * 256.f) / GECO_MATH_PI_F + 0.5f);
}
float Int8ToHalfAngle(uchar angle)
{
	return float(angle) * (GECO_MATH_PI_F / 256.f);
}

GecoYawPitch::GecoYawPitch(float yaw, float pitch)
{
	this->Set(yaw, pitch);
}
GecoYawPitch::GecoYawPitch() {};
void GecoYawPitch::Set(float yaw, float pitch)
{
	m_uiYaw = AngleToInt8(yaw);
	m_uiPitch = HalfAngleToInt8(pitch);
}
void GecoYawPitch::Get(float & yaw, float & pitch) const
{
	yaw = Int8ToAngle(m_uiYaw);
	pitch = Int8ToHalfAngle(m_uiPitch);
}
//geco_bit_stream_t & operator >> (geco_bit_stream_t& kIS, GecoYawPitch& kDes)
//{
//	if (kIS.is_compression_mode())
//	{
//		kIS.ReadMini(kDes.m_uiYaw);
//		kIS.ReadMini(kDes.m_uiPitch);
//	}
//	else
//	{
//		kIS.Read(kDes.m_uiYaw);
//		kIS.Read(kDes.m_uiPitch);
//	}
//	return kIS;
//}
//geco_bit_stream_t& operator << (geco_bit_stream_t& kOS, const GecoYawPitch& kDes)
//{
//	if (kOS.is_compression_mode())
//	{
//		kOS.WriteMini(kDes.m_uiYaw);
//		kOS.WriteMini(kDes.m_uiPitch);
//	}
//	else
//	{
//		kOS.Write(kDes.m_uiYaw);
//		kOS.Write(kDes.m_uiPitch);
//	}
//	return kOS;
//}