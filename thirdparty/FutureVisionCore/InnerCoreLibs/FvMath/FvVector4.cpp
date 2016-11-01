//{future header message}
#include "FvVector4.h"

const FvVector4 FvVector4::ZERO = FvVector4(0, 0, 0, 0);
const FvVector4 FvVector4::UNIT_X = FvVector4(1, 0, 0, 0);
const FvVector4 FvVector4::UNIT_Y = FvVector4(0, 1, 0, 0);
const FvVector4 FvVector4::UNIT_Z = FvVector4(0, 0, 1, 0);

const FvVector4 FvVector4::ZERO_POINT = FvVector4(0, 0, 0, 1);
const FvVector4 FvVector4::UNIT_X_POINT = FvVector4(1, 0, 0, 1);
const FvVector4 FvVector4::UNIT_Y_POINT = FvVector4(0, 1, 0, 1);
const FvVector4 FvVector4::UNIT_Z_POINT = FvVector4(0, 0, 1, 1);

//---------------------------------------------------------------------------
FvVector4::FvVector4()
{
	FvZero4f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
FvVector4::FvVector4(float fX, float fY, float fZ, float fW) : x(fX), y(fY),
	z(fZ), w(fW)
{

}
//---------------------------------------------------------------------------
FvVector4::FvVector4(const float* fpVector)
{
	FvSet4f(FV_FLOAT_POINT_THIS, fpVector);
}
//---------------------------------------------------------------------------
FvVector4::FvVector4(const FvVector4& kVector)
{
	FvSet4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvVector4::FvVector4(const FvVector2& kVector, float fZ, float fW) : z(fZ),
	w(fW)
{
	FvSet2f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvVector4::FvVector4(float fX, const FvVector2& kVector, float fW) : x(fX),
	w(fW)
{
	FvSet2f(FV_FLOAT_POINT_THIS + 1, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvVector4::FvVector4(float fX, float fY, const FvVector2& kVector) : x(fX),
	w(fY)
{
	FvSet2f(FV_FLOAT_POINT_THIS + 2, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvVector4::FvVector4(const FvVector2& kVector0, const FvVector2& kVector1)
{
	FvSet2f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector0));
	FvSet2f(FV_FLOAT_POINT_THIS + 2, FV_FLOAT_POINT_CONST(kVector1));
}
//---------------------------------------------------------------------------
FvVector4::FvVector4(const FvVector3& kVector, float fW) : w(fW)
{
	FvSet3f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvVector4::FvVector4(float fX, const FvVector3& kVector) : x(fX)
{
	FvSet3f(FV_FLOAT_POINT_THIS + 1, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvVector4::operator float* ()
{
	return FV_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
FvVector4::operator const float* () const
{
	return FV_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
float& FvVector4::operator[] (FvVector4::Coord eCoord)
{
	return FV_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
float FvVector4::operator[] (FvVector4::Coord eCoord) const
{
	return FV_FLOAT_POINT_THIS[eCoord];
}
//---------------------------------------------------------------------------
FvVector4& FvVector4::operator = (const FvVector4& kVector)
{
	FvSet4f(FV_FLOAT_POINT_THIS,  FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector4& FvVector4::operator += (const FvVector4& kVector)
{
	FvAdd4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector4& FvVector4::operator -= (const FvVector4& kVector)
{
	FvSub4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return *this;
}
//---------------------------------------------------------------------------
FvVector4& FvVector4::operator *= (float fScale)
{
	FvScale4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, fScale);
	return *this;
}
//---------------------------------------------------------------------------
FvVector4& FvVector4::operator /= (float fScale)
{
	FvScale4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return *this;
}
//---------------------------------------------------------------------------
FvVector4 FvVector4::operator + () const
{
	return FvVector4(*this);
}
//---------------------------------------------------------------------------
FvVector4 FvVector4::operator - () const
{
	return FvVector4(-x, -y, -z, -w);
}
//---------------------------------------------------------------------------
FvVector4 FvVector4::operator + (const FvVector4& kVector) const
{
	FvVector4 kRes;
	FvAdd4f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
FvVector4 FvVector4::operator - (const FvVector4& kVector) const
{
	FvVector4 kRes;
	FvSub4f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_CONST(kVector));
	return kRes;
}
//---------------------------------------------------------------------------
FvVector4 FvVector4::operator * (float fScale) const
{
	FvVector4 kRes;
	FvScale4f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS, fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector4 FvVector4::operator / (float fScale) const
{
	FvVector4 kRes;
	FvScale4f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector4 operator * (float fScale, const FvVector4& kVector)
{
	FvVector4 kRes;
	FvScale4f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_CONST(kVector),
		fScale);
	return kRes;
}
//---------------------------------------------------------------------------
FvVector4 operator / (float fScale, const FvVector4& kVector)
{
	FvVector4 kRes;
	FvScale4f(FV_FLOAT_POINT(kRes), FV_FLOAT_POINT_CONST(kVector),
		1.0f / fScale);
	return kRes;
}
//----------------------------------------------------------------------------
bool FvVector4::operator== (const FvVector4& kVector) const
{
	return (x == kVector.x) && (y == kVector.y) && (z == kVector.z)
		&& (w == kVector.w);
}
//---------------------------------------------------------------------------
bool FvVector4::operator!= (const FvVector4& kVector) const
{
	return (x != kVector.x) || (y != kVector.y) || (z != kVector.z)
		|| (w != kVector.w);
}
//---------------------------------------------------------------------------
void FvVector4::SetZero()
{
	FvZero4f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void FvVector4::Set(float fX, float fY, float fZ, float fW)
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}
//---------------------------------------------------------------------------
void FvVector4::Scale(const FvVector4& kVector, float fScale)
{
	FvScale4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector), fScale);
}
//----------------------------------------------------------------------------
void FvVector4::Scale(float fScale)
{
	FvScale4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, fScale);
}
//---------------------------------------------------------------------------
float FvVector4::Length() const
{
	return FvSqrtf(LengthSquared());
}
//---------------------------------------------------------------------------
float FvVector4::LengthSquared() const
{
	return FvLengthSqr4f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
void FvVector4::Normalise()
{
	float fLengthSquared = LengthSquared();
	if(fLengthSquared)
		*this *= FvInvSqrtf(fLengthSquared);
}
//---------------------------------------------------------------------------
void FvVector4::ParallelMultiply(const FvVector4& kVector)
{
	FvMul4f(FV_FLOAT_POINT_THIS,
		FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//----------------------------------------------------------------------------
void FvVector4::ParallelMultiply(const FvVector4& kVector0,
	const FvVector4& kVector1)
{
	FvMul4f(FV_FLOAT_POINT_THIS, kVector0, kVector1);
}
//---------------------------------------------------------------------------
float FvVector4::DotProduct(const FvVector4& kVector) const
{
	return FvDot4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_CONST(kVector));
}
//---------------------------------------------------------------------------
FvOutcode FvVector4::CalculateOutcode() const
{
	return FvVector4Outcode(FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
FvVector4 FvVector4::UnitVector() const
{
	FvVector4 kResVector(*this);
	kResVector.Normalise();
	return kResVector;
}
//----------------------------------------------------------------------------
void FvVector4::Lerp(const FvVector4& kVector0, const FvVector4& kVector1,
	float fAlpha)
{
	*this = FV_LERP(fAlpha, kVector0, kVector1);
}
//----------------------------------------------------------------------------
void FvVector4::Clamp(const FvVector4& kLower, const FvVector4& kUpper)
{
	*this = FvClampEx(kLower, *this, kUpper);
}
//---------------------------------------------------------------------------
bool FvAlmostEqual( const FvVector4& kVector0, const FvVector4& kVector1,
	const float fEpsilon)
{
	return FvAlmostEqual(kVector0.x, kVector1.x, fEpsilon) &&
		FvAlmostEqual(kVector0.y, kVector1.y, fEpsilon) &&
		FvAlmostEqual(kVector0.z, kVector1.z, fEpsilon) &&
		FvAlmostEqual(kVector0.w, kVector1.w, fEpsilon);
}
//---------------------------------------------------------------------------