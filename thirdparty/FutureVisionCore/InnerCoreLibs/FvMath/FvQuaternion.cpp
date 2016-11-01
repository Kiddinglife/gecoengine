//{future source message}
#include "FvQuaternion.h"
#include "FvMatrix.h"

//---------------------------------------------------------------------------
FvQuaternion::FvQuaternion()
{
	FvZero4f(FV_FLOAT_POINT_THIS);
}
//---------------------------------------------------------------------------
FvQuaternion::FvQuaternion(float fX, float fY, float fZ, float fW) : x(fX),
y(fY), z(fZ), w(fW)
{

}
//---------------------------------------------------------------------------
FvQuaternion::FvQuaternion(const float* fpQuat)
{
	FvSet4f(FV_FLOAT_POINT_THIS, fpQuat);
}
//---------------------------------------------------------------------------
FvQuaternion::FvQuaternion(const FvQuaternion& kQuat)
{
	FvSet4f(FV_FLOAT_POINT_THIS, kQuat);
}
//----------------------------------------------------------------------------
FvQuaternion::FvQuaternion(const FvVector3& kVec, float fW)
{
	FvSet3f(FV_FLOAT_POINT_THIS, kVec);
	w = fW;
}
//---------------------------------------------------------------------------
FvQuaternion::operator float* ()
{
	return FV_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
FvQuaternion::operator const float* () const
{
	return FV_FLOAT_POINT_THIS;
}
//---------------------------------------------------------------------------
FvQuaternion& FvQuaternion::operator += (const FvQuaternion& kQuat)
{
	FvAdd4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kQuat);
	return *this;
}
//---------------------------------------------------------------------------
FvQuaternion& FvQuaternion::operator -= (const FvQuaternion& kQuat)
{
	FvSub4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kQuat);
	return *this;
}
//---------------------------------------------------------------------------
FvQuaternion& FvQuaternion::operator *= (const FvQuaternion& kQuat)
{
	FvQuaternionMultiplyf(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kQuat);
	return *this;
}
//----------------------------------------------------------------------------
FvQuaternion& FvQuaternion::operator *= (float fScale)
{
	FvScale4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, fScale);
	return *this;
}
//---------------------------------------------------------------------------
FvQuaternion& FvQuaternion::operator /= (float fScale)
{
	FvScale4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return *this;
}
//---------------------------------------------------------------------------
FvQuaternion FvQuaternion::operator + () const
{
	return *this;
}
//---------------------------------------------------------------------------
FvQuaternion FvQuaternion::operator - () const
{
	return *this * -1.0f;
}
//---------------------------------------------------------------------------
FvQuaternion FvQuaternion::operator + (const FvQuaternion& kQuat) const
{
	FvQuaternion kRes;
	FvAdd4f(kRes, FV_FLOAT_POINT_THIS, kQuat);
	return kRes;
}
//----------------------------------------------------------------------------
FvQuaternion FvQuaternion::operator - (const FvQuaternion& kQuat) const
{
	FvQuaternion kRes;
	FvSub4f(kRes, FV_FLOAT_POINT_THIS, kQuat);
	return kRes;
}
//----------------------------------------------------------------------------
FvQuaternion FvQuaternion::operator * (const FvQuaternion& kQuat) const
{
	FvQuaternion kRes;
	FvQuaternionMultiplyf(kRes, FV_FLOAT_POINT_THIS, kQuat);
	return kRes;
}
//----------------------------------------------------------------------------
FvQuaternion FvQuaternion::operator * (float fScale) const
{
	FvQuaternion kRes;
	FvScale4f(kRes, FV_FLOAT_POINT_THIS, fScale);
	return kRes;
}
//----------------------------------------------------------------------------
FvVector3 FvQuaternion::operator*(const FvVector3& v)
{
	FvVector3 uv, uuv;
	FvVector3 qvec(x, y, z);
	uv = qvec.CrossProduct(v);
	uuv = qvec.CrossProduct(uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;
}
//----------------------------------------------------------------------------
FvQuaternion FvQuaternion::operator / (float fScale) const
{
	FvQuaternion kRes;
	FvScale4f(kRes, FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return kRes;
}
//----------------------------------------------------------------------------
FvQuaternion operator * (float fScale,const FvQuaternion& kQuat)
{
	FvQuaternion kRes;
	FvScale4f(kRes, kQuat, fScale);
	return kRes;
}
//----------------------------------------------------------------------------
bool FvQuaternion::operator == (const FvQuaternion& kQuat) const
{
	return (x == kQuat.x) && (y == kQuat.y) &&
		(z == kQuat.z) && (w == kQuat.w);
}
//----------------------------------------------------------------------------
bool FvQuaternion::operator != (const FvQuaternion& kQuat) const
{
	return (x != kQuat.x) || (y != kQuat.y) ||
		(z != kQuat.z) || (w != kQuat.w);
}
//----------------------------------------------------------------------------
void FvQuaternion::SetZero()
{
	x = y = z = w = 0.0f;
}
//----------------------------------------------------------------------------
void FvQuaternion::Set(float fX, float fY, float fZ, float fW)
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}
//----------------------------------------------------------------------------
void FvQuaternion::Set(const FvVector3& kVec, float fW)
{
	FvSet3f(FV_FLOAT_POINT_THIS, kVec);
	w = fW;
}
//----------------------------------------------------------------------------
void FvQuaternion::FromAngleAxis(float fAngle, const FvVector3& fAxis)
{
	FvQuaternionRotationAxisf(FV_FLOAT_POINT_THIS, fAxis, fAngle);
}
//----------------------------------------------------------------------------
void FvQuaternion::FromMatrix(const FvMatrix& kMat)
{
	FvQuaternionRotationMatrixf(FV_FLOAT_POINT_THIS, kMat);
}
//----------------------------------------------------------------------------
void FvQuaternion::Normalise()
{
	FvNormalize4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
void FvQuaternion::Invert()
{
	FvQuaternionInversef(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
void FvQuaternion::Minimise()
{
	if (w < 0)
	{
		*this = -(*this);
	}
}
//----------------------------------------------------------------------------
void FvQuaternion::Slerp(const FvQuaternion& kStart,
	const FvQuaternion &kEnd, float t)
{
	FvLerp4f(FV_FLOAT_POINT_THIS, kStart, kEnd, t);
}
//----------------------------------------------------------------------------
void FvQuaternion::Multiply(const FvQuaternion& kQuat0,
	const FvQuaternion& kQuat1 )
{
	FvQuaternionMultiplyf(FV_FLOAT_POINT_THIS, kQuat0, kQuat1);
}
//----------------------------------------------------------------------------
void FvQuaternion::PreMultiply(const FvQuaternion& kQuat)
{
	Multiply(kQuat, *this);
}
//----------------------------------------------------------------------------
void FvQuaternion::PostMultiply(const FvQuaternion& kQuat)
{
	Multiply(*this, kQuat);
}
//----------------------------------------------------------------------------
float FvQuaternion::DotProduct(const FvQuaternion& kQuat) const
{
	return FvDot4f(FV_FLOAT_POINT_THIS, kQuat);
}
//----------------------------------------------------------------------------
float FvQuaternion::Length() const
{
	return FvLength4f(FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
float FvQuaternion::LengthSquared() const
{
	return FvLengthSqr4f(FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
