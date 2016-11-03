//{future header message}
#include "FvVector3.h"

const FvVector3 FvVector3::ZERO = FvVector3(0, 0, 0);
const FvVector3 FvVector3::UNIT_X = FvVector3(1, 0, 0);
const FvVector3 FvVector3::UNIT_Y = FvVector3(0, 1, 0);
const FvVector3 FvVector3::UNIT_Z = FvVector3(0, 0, 1);

//---------------------------------------------------------------------------
FvVector3::FvVector3()
{
	FvZero3f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
FvVector3::FvVector3(float fX, float fY, float fZ) : x(fX), y(fY), z(fZ)
{

}
//---------------------------------------------------------------------------
FvVector3::FvVector3(const float* fpVector)
{
	FvSet3f(FV_FLOAT_POINT_THIS, fpVector);
}
//---------------------------------------------------------------------------
FvVector3::FvVector3(const FvVector3& kVector)
{
	FvSet3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvVector3::FvVector3(const FvVector2& kVector, float fZ) : x(kVector.x),
y(kVector.y), z(fZ)
{

}
//---------------------------------------------------------------------------
FvVector3::FvVector3(float fX, const FvVector2& kVector) : x(fX),
y(kVector.x), z(kVector.y)
{

}
//---------------------------------------------------------------------------
FvVector3::operator float* ()
{
	return FV_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
FvVector3::operator const float* () const
{
	return FV_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
float& FvVector3::operator[] (FvVector3::Coord eCoord)
{
	return FV_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
float FvVector3::operator[] (FvVector3::Coord eCoord) const
{
	return FV_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
FvVector3& FvVector3::operator = (const FvVector3& kVector)
{
	FvSet3f(FV_FLOAT_POINT_THIS,  FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector3& FvVector3::operator += (const FvVector3& kVector)
{
	FvAdd3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector3& FvVector3::operator -= (const FvVector3& kVector)
{
	FvSub3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector3& FvVector3::operator *= (float fScale)
{
	FvScale3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, fScale);
	return *this;
}
//---------------------------------------------------------------------------
FvVector3& FvVector3::operator /= (float fScale)
{
	FvScale3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return *this;
}
//---------------------------------------------------------------------------
FvVector3 FvVector3::operator + () const
{
	return FvVector3(*this);
}
//---------------------------------------------------------------------------
FvVector3 FvVector3::operator - () const
{
	return FvVector3(-x, -y, -z);
}
//---------------------------------------------------------------------------
FvVector3 FvVector3::operator + (const FvVector3& kVector) const
{
	FvVector3 kRes;
	FvAdd3f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
FvVector3 FvVector3::operator - (const FvVector3& kVector) const
{
	FvVector3 kRes;
	FvSub3f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
FvVector3 FvVector3::operator * (float fScale) const
{
	FvVector3 kRes;
	FvScale3f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS, fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector3 FvVector3::operator / (float fScale) const
{
	FvVector3 kRes;
	FvScale3f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector3 operator * (float fScale, const FvVector3& kVector)
{
	FvVector3 kRes;
	FvScale3f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_CONST(kVector),
		fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector3 operator / (float fScale, const FvVector3& kVector)
{
	FvVector3 kRes;
	FvScale3f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_CONST(kVector),
		1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
bool FvVector3::operator== (const FvVector3& kVector) const
{
	return (x == kVector.x) && (y == kVector.y) && (z == kVector.z);
}
//---------------------------------------------------------------------------
bool FvVector3::operator!= (const FvVector3& kVector) const
{
	return (x != kVector.x) || (y != kVector.y) || (z != kVector.z);
}
//---------------------------------------------------------------------------
void FvVector3::SetZero()
{
	FvZero3f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void FvVector3::Set(float fX, float fY, float fZ)
{
	x = fX;
	y = fY;
	z = fZ;
}
//---------------------------------------------------------------------------
void FvVector3::Scale(const FvVector3& kVector, float fScale)
{
	FvScale3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector),
		fScale);
}
//---------------------------------------------------------------------------
float FvVector3::Length() const
{
	return FvSqrtf(LengthSquared());
}
//---------------------------------------------------------------------------
float FvVector3::LengthSquared() const
{
	return FvLengthSqr3f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void FvVector3::Normalise()
{
	float fLengthSquared = LengthSquared();
	if(fLengthSquared)
		*this *= FvInvSqrtf(fLengthSquared);
}
//---------------------------------------------------------------------------
void FvVector3::SetPitchYaw(float fPitchInRadians, float fYawInRadians)
{
	FvVector3SetPitchYawZUpf(FV_FLOAT_POINT_THIS, fPitchInRadians, fYawInRadians);
}
//---------------------------------------------------------------------------
float FvVector3::GetYaw() const
{
	return FvVector3GetYawZUpf(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
float FvVector3::GetPitch() const
{
	return FvVector3GetPitchZUpf(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
float FvVector3::DotProduct(const FvVector3& kVector) const
{
	return FvDot3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
void FvVector3::CrossProduct(const FvVector3& kVector0,
	const FvVector3& kVector1)
{
	FvVector3Crossf(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector0),
		FV_FLOAT_POINT_CONST(kVector1));
}
//---------------------------------------------------------------------------
FvVector3 FvVector3::CrossProduct(const FvVector3& kVector) const
{
	FvVector3 kRes;
	FvVector3Crossf(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
void FvVector3::ProjectOnto(const FvVector3& kVector0,
	const FvVector3& kVector1)
{
	*this = kVector1 * (kVector0.DotProduct(kVector1)
		/ kVector1.LengthSquared());
}
//---------------------------------------------------------------------------
FvVector3 FvVector3::ProjectOnto(const FvVector3& kVector) const
{
	FvVector3 kRes;
	kRes.ProjectOnto(*this, kVector);
	return kRes;
}
//----------------------------------------------------------------------------
void FvVector3::Lerp(const FvVector3& kVector0, const FvVector3& kVector1,
	float fAlpha)
{
	*this = FV_LERP(fAlpha, kVector0, kVector1);
}
//----------------------------------------------------------------------------
void FvVector3::Clamp(const FvVector3& kLower, const FvVector3& kUpper)
{
	*this = FvClampEx(kLower, *this, kUpper);
}
//----------------------------------------------------------------------------
FvVector3 FvVector3::UnitVector() const
{
	FvVector3 kResVector(*this);
	kResVector.Normalise();
	return kResVector;
}
//----------------------------------------------------------------------------
void FvVector3::ParallelMultiply(const FvVector3& kVector)
{
	FvMul3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kVector);
}
//----------------------------------------------------------------------------
void FvVector3::ParallelMultiply(const FvVector3& kVector0,
	const FvVector3& kVector1)
{
	FvMul3f(FV_FLOAT_POINT_THIS, kVector0, kVector1);
}
//---------------------------------------------------------------------------
bool FvAlmostEqual(const FvVector3 &kVector0, const FvVector3& kVector1,
	const float fEpsilon)
{
	return FvAlmostEqual(kVector0.x, kVector1.x, fEpsilon) &&
		FvAlmostEqual(kVector0.y, kVector1.y, fEpsilon) &&
		FvAlmostEqual(kVector0.z, kVector1.z, fEpsilon);
}
float FvDistance(const FvVector3 &kVector0, const FvVector3 &kVector1)
{
	const float dx = kVector0.x - kVector1.x;
	const float dy = kVector0.y - kVector1.y;
	const float dz = kVector0.z - kVector1.z;
	return sqrt(dx*dx+dy*dy+dz*dz);
}
float FvDistance2(const FvVector3 &kVector0, const FvVector3 &kVector1)
{
	const float dx = kVector0.x - kVector1.x;
	const float dy = kVector0.y - kVector1.y;
	const float dz = kVector0.z - kVector1.z;
	return (dx*dx+dy*dy+dz*dz);
}
//----------------------------------------------------------------------------
