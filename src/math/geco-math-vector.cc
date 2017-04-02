//{future source message}
#include "Geco-math-vector.h"

const GecoVector2 GecoVector2::ZERO = GecoVector2(0, 0);
const GecoVector2 GecoVector2::UNIT_X = GecoVector2(1, 0);
const GecoVector2 GecoVector2::UNIT_Y = GecoVector2(0, 1);

//---------------------------------------------------------------------------
GecoVector2::GecoVector2()
{
	GecoZero2f(GECO_FLOAT_POINT_THIS);
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector2::GecoVector2(float fX, float fY) : x(fX), y(fY)
{
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector2::GecoVector2(const float* fpVector)
{
	GecoSet2f(GECO_FLOAT_POINT_THIS, fpVector);
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector2::GecoVector2(const GecoVector2& kVector)
{
	GecoSet2f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector));
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector2::operator float* ()
{
	return GECO_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
GecoVector2::operator const float* () const
{
	return GECO_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
float& GecoVector2::operator[] (GecoVector2::Coord eCoord)
{
	return GECO_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
float GecoVector2::operator[] (GecoVector2::Coord eCoord) const
{
	return GECO_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
GecoVector2& GecoVector2::operator = (const GecoVector2& kVector)
{
	GecoSet2f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
GecoVector2& GecoVector2::operator += (const GecoVector2& kVector)
{
	GecoAdd2f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
GecoVector2& GecoVector2::operator -= (const GecoVector2& kVector)
{
	GecoSub2f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
GecoVector2& GecoVector2::operator *= (float fScale)
{
	GecoScale2f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS, fScale);
	return *this;
}
//---------------------------------------------------------------------------
GecoVector2& GecoVector2::operator /= (float fScale)
{
	GecoScale2f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS, 1.0f / fScale);
	return *this;
}
//---------------------------------------------------------------------------
GecoVector2 GecoVector2::operator + () const
{
	return GecoVector2(*this);
}
//---------------------------------------------------------------------------
GecoVector2 GecoVector2::operator - () const
{
	return GecoVector2(-x, -y);
}
//---------------------------------------------------------------------------
GecoVector2 GecoVector2::operator + (const GecoVector2& kVector) const
{
	GecoVector2 kRes;
	GecoAdd2f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector2 GecoVector2::operator - (const GecoVector2& kVector) const
{
	GecoVector2 kRes;
	GecoSub2f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector2 GecoVector2::operator * (float fScale) const
{
	GecoVector2 kRes;
	GecoScale2f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS, fScale);
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector2 GecoVector2::operator / (float fScale) const
{
	GecoVector2 kRes;
	GecoScale2f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS, 1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector2 operator * (float fScale, const GecoVector2& kVector)
{
	GecoVector2 kRes;
	GecoScale2f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_CONST(kVector),
		fScale);
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector2 operator / (float fScale, const GecoVector2& kVector)
{
	GecoVector2 kRes;
	GecoScale2f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_CONST(kVector),
		1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
bool GecoVector2::operator== (const GecoVector2& kVector) const
{
	return (x == kVector.x) && (y == kVector.y);
}
//---------------------------------------------------------------------------
bool GecoVector2::operator!= (const GecoVector2& kVector) const
{
	return (x != kVector.x) || (y != kVector.y);
}
//---------------------------------------------------------------------------
void GecoVector2::SetZero()
{
	GecoZero2f(GECO_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void GecoVector2::Set(float fX, float fY)
{
	x = fX;
	y = fY;
}
//---------------------------------------------------------------------------
void GecoVector2::Scale(const GecoVector2& kVector, float fScale)
{
	GecoScale2f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector),
		fScale);
}
//---------------------------------------------------------------------------
float GecoVector2::Length() const
{
	return GecoSqrtf(LengthSquared());
}
//---------------------------------------------------------------------------
float GecoVector2::LengthSquared() const
{
	return GecoLengthSqr2f(GECO_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void GecoVector2::Normalise()
{
	float fLengthSquared = LengthSquared();
	if (fLengthSquared)
		*this *= GecoInvSqrtf(fLengthSquared);
	Normalised = true;
}
//---------------------------------------------------------------------------
float GecoVector2::DotProduct(const GecoVector2& kVector) const
{
	return GecoDot2f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
float GecoVector2::CrossProduct(const GecoVector2& kVector) const
{
	return GecoVector2Crossf(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
void GecoVector2::ProjectOnto(const GecoVector2& kVector0,
	const GecoVector2& kVector1)
{
	*this = kVector1 * (kVector0.DotProduct(kVector1)
		/ kVector1.LengthSquared());
}
//---------------------------------------------------------------------------
GecoVector2 GecoVector2::ProjectOnto(const GecoVector2& kVector) const
{
	GecoVector2 kRes;
	kRes.ProjectOnto(*this, kVector);
	return kRes;
}
//----------------------------------------------------------------------------
GecoVector2 GecoVector2::UnitVector() const
{
	GecoVector2 kResVector(*this);
	kResVector.Normalise();
	return kResVector;
}
//----------------------------------------------------------------------------
void GecoVector2::Lerp(const GecoVector2& kVector0, const GecoVector2& kVector1,
	float fAlpha)
{
	*this = GECO_LERP(fAlpha, kVector0, kVector1);
}
//----------------------------------------------------------------------------
void GecoVector2::Clamp(const GecoVector2& kLower, const GecoVector2& kUpper)
{
	*this = GecoClampEx(kLower, *this, kUpper);
}
//----------------------------------------------------------------------------
void GecoVector2::ParallelMultiply(const GecoVector2& kVector)
{
	GecoMul3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS, kVector);
}
//----------------------------------------------------------------------------
void GecoVector2::ParallelMultiply(const GecoVector2& kVector0,
	const GecoVector2& kVector1)
{
	GecoMul3f(GECO_FLOAT_POINT_THIS, kVector0, kVector1);
}
//----------------------------------------------------------------------------
bool GecoAlmostEqual(const GecoVector2& kVector0, const GecoVector2& kVector1,
	const float fEpsilon)
{
	return almost_equal(kVector0.x, kVector1.x, fEpsilon) &&
		almost_equal(kVector0.y, kVector1.y, fEpsilon);
}

#include "../common/ds/geco-bit-stream.h"
void GecoVector2::to(geco_bit_stream_t& kOS, bool normalized)
{
	normalized ? kOS.WriteNormVector(x, y) : kOS.WriteVector(x, y);
}
void GecoVector2::from(geco_bit_stream_t& kIS, bool normalized)
{
	normalized ? kIS.ReadNormVector(x, y) : kIS.ReadVector(x, y);
}
//---------------------------------------------------------------------------

const GecoVector3 GecoVector3::ZERO = GecoVector3(0, 0, 0);
const GecoVector3 GecoVector3::UNIT_X = GecoVector3(1, 0, 0);
const GecoVector3 GecoVector3::UNIT_Y = GecoVector3(0, 1, 0);
const GecoVector3 GecoVector3::UNIT_Z = GecoVector3(0, 0, 1);

//---------------------------------------------------------------------------
GecoVector3::GecoVector3()
{
	GecoZero3f(GECO_FLOAT_POINT_THIS);
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector3::GecoVector3(float fX, float fY, float fZ) : x(fX), y(fY), z(fZ)
{
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector3::GecoVector3(const float* fpVector)
{
	GecoSet3f(GECO_FLOAT_POINT_THIS, fpVector);
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector3::GecoVector3(const GecoVector3& kVector)
{
	GecoSet3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector));
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector3::GecoVector3(const GecoVector2& kVector, float fZ) : x(kVector.x),
y(kVector.y), z(fZ)
{
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector3::GecoVector3(float fX, const GecoVector2& kVector) : x(fX),
y(kVector.x), z(kVector.y)
{
	Normalised = false;
}
//---------------------------------------------------------------------------
GecoVector3::operator float* ()
{
	return GECO_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
GecoVector3::operator const float* () const
{
	return GECO_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
float& GecoVector3::operator[] (GecoVector3::Coord eCoord)
{
	return GECO_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
float GecoVector3::operator[] (GecoVector3::Coord eCoord) const
{
	return GECO_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
GecoVector3& GecoVector3::operator = (const GecoVector3& kVector)
{
	GecoSet3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
GecoVector3& GecoVector3::operator += (const GecoVector3& kVector)
{
	GecoAdd3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
GecoVector3& GecoVector3::operator -= (const GecoVector3& kVector)
{
	GecoSub3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
GecoVector3& GecoVector3::operator *= (float fScale)
{
	GecoScale3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS, fScale);
	return *this;
}
//---------------------------------------------------------------------------
GecoVector3& GecoVector3::operator /= (float fScale)
{
	GecoScale3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS, 1.0f / fScale);
	return *this;
}
//---------------------------------------------------------------------------
GecoVector3 GecoVector3::operator + () const
{
	return GecoVector3(*this);
}
//---------------------------------------------------------------------------
GecoVector3 GecoVector3::operator - () const
{
	return GecoVector3(-x, -y, -z);
}
//---------------------------------------------------------------------------
GecoVector3 GecoVector3::operator + (const GecoVector3& kVector) const
{
	GecoVector3 kRes;
	GecoAdd3f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector3 GecoVector3::operator - (const GecoVector3& kVector) const
{
	GecoVector3 kRes;
	GecoSub3f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector3 GecoVector3::operator * (float fScale) const
{
	GecoVector3 kRes;
	GecoScale3f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS, fScale);
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector3 GecoVector3::operator / (float fScale) const
{
	GecoVector3 kRes;
	GecoScale3f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS, 1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector3 operator * (float fScale, const GecoVector3& kVector)
{
	GecoVector3 kRes;
	GecoScale3f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_CONST(kVector),
		fScale);
	return kRes;
}
//---------------------------------------------------------------------------
GecoVector3 operator / (float fScale, const GecoVector3& kVector)
{
	GecoVector3 kRes;
	GecoScale3f(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_CONST(kVector),
		1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
bool GecoVector3::operator== (const GecoVector3& kVector) const
{
	return (x == kVector.x) && (y == kVector.y) && (z == kVector.z);
}
//---------------------------------------------------------------------------
bool GecoVector3::operator!= (const GecoVector3& kVector) const
{
	return (x != kVector.x) || (y != kVector.y) || (z != kVector.z);
}
//---------------------------------------------------------------------------
void GecoVector3::SetZero()
{
	GecoZero3f(GECO_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void GecoVector3::Set(float fX, float fY, float fZ)
{
	x = fX;
	y = fY;
	z = fZ;
}
//---------------------------------------------------------------------------
void GecoVector3::Scale(const GecoVector3& kVector, float fScale)
{
	GecoScale3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector),
		fScale);
}
//---------------------------------------------------------------------------
float GecoVector3::Length() const
{
	return GecoSqrtf(LengthSquared());
}
//---------------------------------------------------------------------------
float GecoVector3::LengthSquared() const
{
	return GecoLengthSqr3f(GECO_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void GecoVector3::Normalise()
{
	float fLengthSquared = LengthSquared();
	if (fLengthSquared)
		*this *= GecoInvSqrtf(fLengthSquared);
	Normalised = true;
}
//---------------------------------------------------------------------------
void GecoVector3::SetPitchYaw(float fPitchInRadians, float fYawInRadians)
{
	GecoVector3SetPitchYawZUpf(GECO_FLOAT_POINT_THIS, fPitchInRadians, fYawInRadians);
}
//---------------------------------------------------------------------------
float GecoVector3::GetYaw() const
{
	return GecoVector3GetYawZUpf(GECO_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
float GecoVector3::GetPitch() const
{
	return GecoVector3GetPitchZUpf(GECO_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
float GecoVector3::DotProduct(const GecoVector3& kVector) const
{
	return GecoDot3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
void GecoVector3::CrossProduct(const GecoVector3& kVector0,
	const GecoVector3& kVector1)
{
	GecoVector3Crossf(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_CONST(kVector0),
		GECO_FLOAT_POINT_CONST(kVector1));
}
//---------------------------------------------------------------------------
GecoVector3 GecoVector3::CrossProduct(const GecoVector3& kVector) const
{
	GecoVector3 kRes;
	GecoVector3Crossf(GECO_FLOAT_POINT(kRes), GECO_FLOAT_POINT_THIS,
		GECO_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
void GecoVector3::ProjectOnto(const GecoVector3& kVector0,
	const GecoVector3& kVector1)
{
	*this = kVector1 * (kVector0.DotProduct(kVector1)
		/ kVector1.LengthSquared());
}
//---------------------------------------------------------------------------
GecoVector3 GecoVector3::ProjectOnto(const GecoVector3& kVector) const
{
	GecoVector3 kRes;
	kRes.ProjectOnto(*this, kVector);
	return kRes;
}
//----------------------------------------------------------------------------
void GecoVector3::Lerp(const GecoVector3& kVector0, const GecoVector3& kVector1,
	float fAlpha)
{
	*this = GECO_LERP(fAlpha, kVector0, kVector1);
}
//----------------------------------------------------------------------------
void GecoVector3::Clamp(const GecoVector3& kLower, const GecoVector3& kUpper)
{
	*this = GecoClampEx(kLower, *this, kUpper);
}
//----------------------------------------------------------------------------
GecoVector3 GecoVector3::UnitVector() const
{
	GecoVector3 kResVector(*this);
	kResVector.Normalise();
	return kResVector;
}
//----------------------------------------------------------------------------
void GecoVector3::ParallelMultiply(const GecoVector3& kVector)
{
	GecoMul3f(GECO_FLOAT_POINT_THIS, GECO_FLOAT_POINT_THIS, kVector);
}
//----------------------------------------------------------------------------
void GecoVector3::ParallelMultiply(const GecoVector3& kVector0,
	const GecoVector3& kVector1)
{
	GecoMul3f(GECO_FLOAT_POINT_THIS, kVector0, kVector1);
}
//---------------------------------------------------------------------------
bool GecoAlmostEqual(const GecoVector3 &kVector0, const GecoVector3& kVector1,
	const float fEpsilon)
{
	return almost_equal(kVector0.x, kVector1.x, fEpsilon) &&
		almost_equal(kVector0.y, kVector1.y, fEpsilon) &&
		almost_equal(kVector0.z, kVector1.z, fEpsilon);
}
float GecoDistance(const GecoVector3 &kVector0, const GecoVector3 &kVector1)
{
	const float dx = kVector0.x - kVector1.x;
	const float dy = kVector0.y - kVector1.y;
	const float dz = kVector0.z - kVector1.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}
float GecoDistance2(const GecoVector3 &kVector0, const GecoVector3 &kVector1)
{
	const float dx = kVector0.x - kVector1.x;
	const float dy = kVector0.y - kVector1.y;
	const float dz = kVector0.z - kVector1.z;
	return (dx*dx + dy*dy + dz*dz);
}
//----------------------------------------------------------------------------