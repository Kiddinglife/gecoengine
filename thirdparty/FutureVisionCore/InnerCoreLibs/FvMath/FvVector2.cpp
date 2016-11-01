//{future source message}
#include "FvVector2.h"

const FvVector2 FvVector2::ZERO = FvVector2(0, 0);
const FvVector2 FvVector2::UNIT_X = FvVector2(1, 0);
const FvVector2 FvVector2::UNIT_Y = FvVector2(0, 1);

//---------------------------------------------------------------------------
FvVector2::FvVector2()
{
	FvZero2f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
FvVector2::FvVector2(float fX, float fY) : x(fX), y(fY)
{

}
//---------------------------------------------------------------------------
FvVector2::FvVector2(const float* fpVector)
{
	FvSet2f(FV_FLOAT_POINT_THIS, fpVector);
}
//---------------------------------------------------------------------------
FvVector2::FvVector2(const FvVector2& kVector)
{
	FvSet2f(FV_FLOAT_POINT_THIS,  FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvVector2::operator float* ()
{
	return FV_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
FvVector2::operator const float* () const
{
	return FV_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
float& FvVector2::operator[] (FvVector2::Coord eCoord)
{
	return FV_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
float FvVector2::operator[] (FvVector2::Coord eCoord) const
{
	return FV_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
FvVector2& FvVector2::operator = (const FvVector2& kVector)
{
	FvSet2f(FV_FLOAT_POINT_THIS,  FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector2& FvVector2::operator += (const FvVector2& kVector)
{
	FvAdd2f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector2& FvVector2::operator -= (const FvVector2& kVector)
{
	FvSub2f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector2& FvVector2::operator *= (float fScale)
{
	FvScale2f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, fScale);
	return *this;
}
//---------------------------------------------------------------------------
FvVector2& FvVector2::operator /= (float fScale)
{
	FvScale2f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return *this;
}
//---------------------------------------------------------------------------
FvVector2 FvVector2::operator + () const
{
	return FvVector2(*this);
}
//---------------------------------------------------------------------------
FvVector2 FvVector2::operator - () const
{
	return FvVector2(-x, -y);
}
//---------------------------------------------------------------------------
FvVector2 FvVector2::operator + (const FvVector2& kVector) const
{
	FvVector2 kRes;
	FvAdd2f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
FvVector2 FvVector2::operator - (const FvVector2& kVector) const
{
	FvVector2 kRes;
	FvSub2f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
FvVector2 FvVector2::operator * (float fScale) const
{
	FvVector2 kRes;
	FvScale2f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS, fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector2 FvVector2::operator / (float fScale) const
{
	FvVector2 kRes;
	FvScale2f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector2 operator * (float fScale, const FvVector2& kVector)
{
	FvVector2 kRes;
	FvScale2f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_CONST(kVector),
		fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector2 operator / (float fScale, const FvVector2& kVector)
{
	FvVector2 kRes;
	FvScale2f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_CONST(kVector),
		1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
bool FvVector2::operator== (const FvVector2& kVector) const
{
	return (x == kVector.x) && (y == kVector.y);
}
//---------------------------------------------------------------------------
bool FvVector2::operator!= (const FvVector2& kVector) const
{
	return (x != kVector.x) || (y != kVector.y);
}
//---------------------------------------------------------------------------
void FvVector2::SetZero()
{
	FvZero2f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void FvVector2::Set(float fX, float fY)
{
	x = fX;
	y = fY;
}
//---------------------------------------------------------------------------
void FvVector2::Scale(const FvVector2& kVector, float fScale)
{
	FvScale2f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector),
		fScale);
}
//---------------------------------------------------------------------------
float FvVector2::Length() const
{
	return FvSqrtf(LengthSquared());
}
//---------------------------------------------------------------------------
float FvVector2::LengthSquared() const
{
	return FvLengthSqr2f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void FvVector2::Normalise()
{
	float fLengthSquared = LengthSquared();
	if(fLengthSquared)
		*this *= FvInvSqrtf(fLengthSquared);
}
//---------------------------------------------------------------------------
float FvVector2::DotProduct(const FvVector2& kVector) const
{
	return FvDot2f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
float FvVector2::CrossProduct(const FvVector2& kVector) const
{
	return FvVector2Crossf(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
void FvVector2::ProjectOnto(const FvVector2& kVector0,
	const FvVector2& kVector1)
{
	*this = kVector1 * ( kVector0.DotProduct( kVector1 )
		/ kVector1.LengthSquared() );
}
//---------------------------------------------------------------------------
FvVector2 FvVector2::ProjectOnto(const FvVector2& kVector) const
{
	FvVector2 kRes;
	kRes.ProjectOnto(*this, kVector);
	return kRes;
}
//----------------------------------------------------------------------------
FvVector2 FvVector2::UnitVector() const
{
	FvVector2 kResVector(*this);
	kResVector.Normalise();
	return kResVector;
}
//----------------------------------------------------------------------------
void FvVector2::Lerp(const FvVector2& kVector0, const FvVector2& kVector1,
	float fAlpha)
{
	*this = FV_LERP(fAlpha, kVector0, kVector1);
}
//----------------------------------------------------------------------------
void FvVector2::Clamp(const FvVector2& kLower, const FvVector2& kUpper)
{
	*this = FvClampEx(kLower, *this, kUpper);
}
//----------------------------------------------------------------------------
void FvVector2::ParallelMultiply(const FvVector2& kVector)
{
	FvMul3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kVector);
}
//----------------------------------------------------------------------------
void FvVector2::ParallelMultiply(const FvVector2& kVector0,
	const FvVector2& kVector1)
{
	FvMul3f(FV_FLOAT_POINT_THIS, kVector0, kVector1);
}
//----------------------------------------------------------------------------
bool FvAlmostEqual(const FvVector2& kVector0, const FvVector2& kVector1,
	const float fEpsilon)
{
	return FvAlmostEqual(kVector0.x, kVector1.x, fEpsilon) && 
		FvAlmostEqual(kVector0.y, kVector1.y, fEpsilon);
}
//---------------------------------------------------------------------------
