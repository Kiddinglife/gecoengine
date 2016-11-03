//{future source message}
#include "FvMatrix.h"
#include "FvQuaternion.h"
#include "FvPowerDefines.h"

#include <FvDebug.h>

const FvMatrix FvMatrix::ZERO(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
const FvMatrix FvMatrix::IDENTITY(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

//----------------------------------------------------------------------------
FvMatrix::FvMatrix()
{
	FvZero16f(FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
FvMatrix::FvMatrix(const float* fpMatrix)
{
	FvSet16f(FV_FLOAT_POINT_THIS, fpMatrix);
}
//----------------------------------------------------------------------------
FvMatrix::FvMatrix(const FvMatrix& kMatrix)
{
	FvSet16f(FV_FLOAT_POINT_THIS, kMatrix);
}
//----------------------------------------------------------------------------
FvMatrix::FvMatrix(
	float _11, float _12, float _13, float _14,
	float _21, float _22, float _23, float _24,
	float _31, float _32, float _33, float _34,
	float _41, float _42, float _43, float _44)
{
	this->_11 = _11;
	this->_12 = _12;
	this->_13 = _13;
	this->_14 = _14;

	this->_21 = _21;
	this->_22 = _22;
	this->_23 = _23;
	this->_24 = _24;

	this->_31 = _31;
	this->_32 = _32;
	this->_33 = _33;
	this->_34 = _34;

	this->_41 = _41;
	this->_42 = _42;
	this->_43 = _43;
	this->_44 = _44;
}
//----------------------------------------------------------------------------
FvMatrix::FvMatrix(FvVector4& kRow0, FvVector4& kRow1, FvVector4& kRow2,
	FvVector4& kRow3)
{
	Row(0) = kRow0;
	Row(1) = kRow1;
	Row(2) = kRow2;
	Row(3) = kRow3;
}
//----------------------------------------------------------------------------
FvMatrix::operator float* ()
{
	return FV_FLOAT_POINT_THIS;
}
//----------------------------------------------------------------------------
FvMatrix::operator const float* () const
{
	return FV_FLOAT_POINT_THIS;
}
//----------------------------------------------------------------------------
float& FvMatrix::operator () (unsigned int uiRow, unsigned int uiCol)
{
	return m[uiRow][uiCol];
}
//----------------------------------------------------------------------------
const float FvMatrix::operator () (unsigned int uiRow, unsigned int uiCol) const
{
	return m[uiRow][uiCol];
}
//----------------------------------------------------------------------------
FvMatrix& FvMatrix::operator *= (const FvMatrix& kMatrix)
{
	FvMatrixMultiply4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kMatrix);
	return *this;
}
//----------------------------------------------------------------------------
FvMatrix& FvMatrix::operator += (const FvMatrix& kMatrix)
{
	FvAdd16f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kMatrix);
	return *this;
}
//----------------------------------------------------------------------------
FvMatrix& FvMatrix::operator -= (const FvMatrix& kMatrix)
{
	FvSub16f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kMatrix);
	return *this;
}
//----------------------------------------------------------------------------
FvMatrix& FvMatrix::operator *= (float fScale)
{
	FvScale16f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, fScale);
	return *this;
}
//----------------------------------------------------------------------------
FvMatrix& FvMatrix::operator /= (float fScale)
{
	FvScale16f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return *this;
}
//----------------------------------------------------------------------------
FvMatrix FvMatrix::operator + () const
{
	return *this;
}
//----------------------------------------------------------------------------
FvMatrix FvMatrix::operator - () const
{
	return (*this) * -1.0f;
}
//----------------------------------------------------------------------------
FvMatrix FvMatrix::operator * (const FvMatrix& kMatrix) const
{
	FvMatrix kRes;
	FvMatrixMultiply4f(kRes, FV_FLOAT_POINT_THIS, kMatrix);
	return kRes;
}
//----------------------------------------------------------------------------
FvMatrix FvMatrix::operator + (const FvMatrix& kMatrix) const
{
	FvMatrix kRes;
	FvAdd16f(kRes, FV_FLOAT_POINT_THIS, kMatrix);
	return kRes;
}
//----------------------------------------------------------------------------
FvMatrix FvMatrix::operator - (const FvMatrix& kMatrix) const
{
	FvMatrix kRes;
	FvSub16f(kRes, FV_FLOAT_POINT_THIS, kMatrix);
	return kRes;
}
//----------------------------------------------------------------------------
FvMatrix FvMatrix::operator * (float fScale) const
{
	FvMatrix kRes;
	FvScale16f(kRes, FV_FLOAT_POINT_THIS, fScale);
	return kRes;
}
//----------------------------------------------------------------------------
FvMatrix FvMatrix::operator / (float fScale) const
{
	FvMatrix kRes;
	FvScale16f(kRes, FV_FLOAT_POINT_THIS, 1.0f / fScale);
	return kRes;
}
//----------------------------------------------------------------------------
FvMatrix operator * (float fScale, const FvMatrix& kMatrix)
{
	FvMatrix kRes;
	FvScale16f(kRes, kMatrix, fScale);
	return kRes;
}
//----------------------------------------------------------------------------
bool FvMatrix::operator == (const FvMatrix& kMatrix) const
{
	return (_11 == kMatrix._11) && (_12 == kMatrix._12) &&
		(_13 == kMatrix._13) && (_14 == kMatrix._14) &&
		(_21 == kMatrix._21) && (_22 == kMatrix._22) &&
		(_23 == kMatrix._23) && (_24 == kMatrix._24) &&
		(_31 == kMatrix._31) && (_32 == kMatrix._32) &&
		(_33 == kMatrix._33) && (_34 == kMatrix._34) &&
		(_41 == kMatrix._41) && (_42 == kMatrix._42) &&
		(_43 == kMatrix._43) && (_44 == kMatrix._44);
}
//----------------------------------------------------------------------------
bool FvMatrix::operator != (const FvMatrix& kMatrix) const
{
	return (_11 != kMatrix._11) || (_12 != kMatrix._12) ||
		(_13 != kMatrix._13) || (_14 != kMatrix._14) ||
		(_21 != kMatrix._21) || (_22 != kMatrix._22) ||
		(_23 != kMatrix._23) || (_24 != kMatrix._24) ||
		(_31 != kMatrix._31) || (_32 != kMatrix._32) ||
		(_33 != kMatrix._33) || (_34 != kMatrix._34) ||
		(_41 != kMatrix._41) || (_42 != kMatrix._42) ||
		(_43 != kMatrix._43) || (_44 != kMatrix._44);
}
//----------------------------------------------------------------------------
FvVector3& FvMatrix::operator [] (int i)
{
	return *(FvVector3*)(&m[i]);
}
//----------------------------------------------------------------------------
const FvVector3& FvMatrix::operator [] (int i) const
{
	return *(const FvVector3*)(&m[i]);
}
//----------------------------------------------------------------------------
FvVector4& FvMatrix::Row(int i)
{
	FV_ASSERT(0 <= i && i < 4);
	return *(FvVector4*)(&m[i]);
}
//----------------------------------------------------------------------------
const FvVector4& FvMatrix::Row(int i) const
{
	FV_ASSERT(0 <= i && i < 4);
	return *(const FvVector4*)(&m[i]);
}
//----------------------------------------------------------------------------
void FvMatrix::Row(int i, const FvVector4& kVec)
{
	Row(i) = kVec;
}
//----------------------------------------------------------------------------
FvVector4 FvMatrix::Col(int i) const
{
	FV_ASSERT(0 <= i && i < 4);
	return FvVector4(m[0][i],  m[1][i], m[2][i], m[3][i]);
}
//----------------------------------------------------------------------------
void FvMatrix::Col(int i, const FvVector4& kVec)
{
	FV_ASSERT(0 <= i && i < 4);
	m[0][i] = kVec.x;
	m[1][i] = kVec.y;
	m[2][i] = kVec.z;
	m[3][i] = kVec.w;
}
//----------------------------------------------------------------------------
void FvMatrix::SetZero()
{
	*this = ZERO;
}
//----------------------------------------------------------------------------
void FvMatrix::SetIdentity()
{
	*this = IDENTITY;
}
//----------------------------------------------------------------------------
void FvMatrix::SetScale(const float x, const float y, const float z)
{
	FvMatrixScaling4f(FV_FLOAT_POINT_THIS, x, y, z);
}
//----------------------------------------------------------------------------
void FvMatrix::SetScale(const FvVector3& kScale)
{
	SetScale(kScale.x, kScale.y, kScale.z);
}
//----------------------------------------------------------------------------
void FvMatrix::SetTranslate(const float x, const float y, const float z)
{
	FvMatrixTranslation4f(FV_FLOAT_POINT_THIS, x, y, z);
}
//----------------------------------------------------------------------------
void FvMatrix::SetTranslate(const FvVector3& kPos)
{
	SetTranslate(kPos.x, kPos.y, kPos.z);
}
//----------------------------------------------------------------------------
void FvMatrix::SetRotateX(const float fAngle)
{
	FvMatrixRotationX4f(FV_FLOAT_POINT_THIS, fAngle);
}
//----------------------------------------------------------------------------
void FvMatrix::SetRotateY(const float fAngle)
{
	FvMatrixRotationY4f(FV_FLOAT_POINT_THIS, fAngle);
}
//----------------------------------------------------------------------------
void FvMatrix::SetRotateZ(const float fAngle)
{
	FvMatrixRotationZ4f(FV_FLOAT_POINT_THIS, fAngle);
}
//----------------------------------------------------------------------------
void FvMatrix::SetRotate(const FvQuaternion& kQuat)
{
	FvMatrixRotationQuaternion4f(FV_FLOAT_POINT_THIS, kQuat);
}
//----------------------------------------------------------------------------
void FvMatrix::SetRotate(float fYaw, float fPitch, float fRoll)
{
	FvMatrixRotationYawPitchRoll4f(FV_FLOAT_POINT_THIS, fYaw, fPitch, fRoll);
}
//----------------------------------------------------------------------------
static bool IsValidAngle(float fAngle)
{
	return (-100.0f < fAngle) && (fAngle < 100.0f);
}
//----------------------------------------------------------------------------
void FvMatrix::SetRotateInverse(float fYaw, float fPitch, float fRoll)
{
	FV_ASSERT(IsValidAngle(fYaw));
	FV_ASSERT(IsValidAngle(fPitch));
	FV_ASSERT(IsValidAngle(fRoll));


	float sya, cya;
	float sxa, cxa;
	float sza, cza;

	FvSinCosf(&sya, &cya, fYaw);
	FvSinCosf(&sxa, &cxa, fPitch);
	FvSinCosf(&sza, &cza, fRoll);

	m[0][0] = cya * cza;
	m[0][1] = -cya * sza;
	m[0][2] = sya * cxa;
	m[0][3] =  0.0f;

	m[1][0] = cxa * sza;
	m[1][1] = cxa * cza;
	m[1][2] = -sxa;
	m[1][3] =  0.0f;

	m[2][0] = -sya * cza + cya * sza * sxa;
	m[2][1] = sya * sza + cya * cza * sxa;
	m[2][2] = cxa * cya;
	m[2][3] =  0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}
//----------------------------------------------------------------------------
void FvMatrix::Multiply(const FvMatrix& kMat0, const FvMatrix& kMat1)
{
	FvMatrixMultiply4f(FV_FLOAT_POINT_THIS, kMat0, kMat1);
}
//----------------------------------------------------------------------------
void FvMatrix::PreMultiply(const FvMatrix& kMat)
{
	FvMatrixMultiply4f(FV_FLOAT_POINT_THIS, kMat, FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
void FvMatrix::PostMultiply(const FvMatrix& kMat)
{
	FvMatrixMultiply4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS, kMat);
}
//----------------------------------------------------------------------------
void FvMatrix::InvertOrthonormal(const FvMatrix& kMat)
{
	m[0][0] = kMat.m[0][0];
	m[0][1] = kMat.m[1][0];
	m[0][2] = kMat.m[2][0];
	m[0][3] = 0.0f;

	m[1][0] = kMat.m[0][1];
	m[1][1] = kMat.m[1][1];
	m[1][2] = kMat.m[2][1];
	m[1][3] = 0.0f;

	m[2][0] = kMat.m[0][2];
	m[2][1] = kMat.m[1][2];
	m[2][2] = kMat.m[2][2];
	m[2][3] = 0.0f;

	m[3][0] = -(kMat.m[3][0] * m[0][0] + kMat.m[3][1] * m[1][0] + kMat.m[3][2] * m[2][0]);
	m[3][1] = -(kMat.m[3][0] * m[0][1] + kMat.m[3][1] * m[1][1] + kMat.m[3][2] * m[2][1]);
	m[3][2] = -(kMat.m[3][0] * m[0][2] + kMat.m[3][1] * m[1][2] + kMat.m[3][2] * m[2][2]);
	m[3][3] = 1.0f;
}
//----------------------------------------------------------------------------
void FvMatrix::InvertOrthonormal()
{
	const FvMatrix kMat(*this);
	InvertOrthonormal(kMat);
}
//----------------------------------------------------------------------------
bool FvMatrix::Invert(const FvMatrix& kMat)
{
	return FvMatrixInverse4f(FV_FLOAT_POINT_THIS, NULL, kMat) ? true : false;
}
//----------------------------------------------------------------------------
bool FvMatrix::Invert()
{
	return Invert(*this);
}
//----------------------------------------------------------------------------
float FvMatrix::GetDeterminant() const
{
	return FvMatrixDeterminant4f(FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
void FvMatrix::Transpose(const FvMatrix& kMat)
{
	FvMatrixTranspose4f(FV_FLOAT_POINT_THIS, kMat);
}
//----------------------------------------------------------------------------
void FvMatrix::Transpose()
{
	FvMatrixTranspose4f(FV_FLOAT_POINT_THIS, FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
void FvMatrix::LookAt(const FvVector3& kPosition, const FvVector3& kDirection,
	const FvVector3& kUp)
{
	FV_ASSERT(!"Can't use!");
}
//----------------------------------------------------------------------------
FvVector3 FvMatrix::ApplyPoint(const FvVector3& kVec) const
{
	FvVector3 kRes;
	FvVector3TransformCoordM4f(kRes, kVec, FV_FLOAT_POINT_THIS);
	return kRes;
}
//----------------------------------------------------------------------------
void FvMatrix::ApplyPoint(FvVector3& kVec0, const FvVector3& kVec1) const
{
	FvVector3TransformCoordM4f(kVec0, kVec1, FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
void FvMatrix::ApplyPoint(FvVector4& kVec0, const FvVector3& kVec1) const
{
	FvVector3TransformM4f(kVec0, kVec1, FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
void FvMatrix::ApplyPoint(FvVector4& kVec0, const FvVector4& kVec1) const
{
	FvVector4Transformf(kVec0, kVec1, FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
FvVector3 FvMatrix::ApplyVector(const FvVector3& kVec) const
{
	FvVector3 kRes;
	FvVector3TransformNormalM4f(kRes, kVec, FV_FLOAT_POINT_THIS);
	return kRes;
}
//----------------------------------------------------------------------------
void FvMatrix::ApplyVector(FvVector3& kVec0, const FvVector3& kVec1) const
{
	FvVector3TransformNormalM4f(kVec0, kVec1, FV_FLOAT_POINT_THIS);
}
//----------------------------------------------------------------------------
const FvVector3& FvMatrix::ApplyToUnitAxisVector(int iAxis) const
{
	return *(const FvVector3*)m[iAxis];
}
//----------------------------------------------------------------------------
const FvVector3& FvMatrix::ApplyToOrigin() const
{
	return (*this)[3];
}
//----------------------------------------------------------------------------
void FvMatrix::PreRotateX(const float fAngle)
{
	FvMatrix kMat;
	FvMatrixRotationX4f(kMat, fAngle);
	PreMultiply(kMat);
}
//----------------------------------------------------------------------------
void FvMatrix::PreRotateY(const float fAngle)
{
	FvMatrix kMat;
	FvMatrixRotationY4f(kMat, fAngle);
	PreMultiply(kMat);
}
//----------------------------------------------------------------------------
void FvMatrix::PreRotateZ(const float fAngle)
{
	FvMatrix kMat;
	FvMatrixRotationZ4f(kMat, fAngle);
	PreMultiply(kMat);
}
//----------------------------------------------------------------------------
void FvMatrix::PreTranslateBy(const FvVector3& kVec)
{
	(*this)[3] += ApplyVector(kVec);
}
//----------------------------------------------------------------------------
void FvMatrix::PostRotateX(const float fAngle)
{
	FvMatrix kMat;
	FvMatrixRotationX4f(kMat, fAngle);
	PostMultiply(kMat);
}
//----------------------------------------------------------------------------
void FvMatrix::PostRotateY(const float fAngle)
{
	FvMatrix kMat;
	FvMatrixRotationY4f(kMat, fAngle);
	PostMultiply(kMat);
}
//----------------------------------------------------------------------------
void FvMatrix::PostRotateZ(const float fAngle)
{
	FvMatrix kMat;
	FvMatrixRotationZ4f(kMat, fAngle);
	PostMultiply(kMat);
}
//----------------------------------------------------------------------------
void FvMatrix::PostTranslateBy(const FvVector3& kVec)
{
	(*this)[3] += kVec;
}
//----------------------------------------------------------------------------
bool FvMatrix::IsMirrored() const
{
	FvVector3 kVec;
	kVec.CrossProduct((*this)[0], (*this)[2]);
	return kVec.DotProduct((*this)[1]) > 0.0f;
}
//----------------------------------------------------------------------------
void FvMatrix::OrthogonalProjection(float w, float h, float zn, float zf)
{
	FV_ASSERT(!"Can't use!");
}
//----------------------------------------------------------------------------
void FvMatrix::PerspectiveProjection(float fov, float aspectRatio,
	float nearPlane, float farPlane)
{
	FV_ASSERT(!"Can't use!");
}
//----------------------------------------------------------------------------
float FvMatrix::Yaw() const
{
	FvVector3 zdir = ApplyToUnitAxisVector(2);
	zdir.Normalise();
	return FvAtan2f(zdir.x, zdir.z);
}
//----------------------------------------------------------------------------
float FvMatrix::Pitch() const
{
	FvVector3 zdir = this->ApplyToUnitAxisVector(2);
	zdir.Normalise();
	return -FvAsinf(FvClampEx(-1.0f, zdir.y, +1.0f));
}
//----------------------------------------------------------------------------
float FvMatrix::Roll() const
{
	float roll;

	FvVector3 xdir = ApplyToUnitAxisVector(0);
	FvVector3 zdir = ApplyToUnitAxisVector(2);
	xdir.Normalise();
	zdir.Normalise();

	const float zdirxzlen = FvSqrtf(zdir.z*zdir.z + zdir.x*zdir.x);
	const float acarg = (zdir.z * xdir.x - zdir.x * xdir.z) / zdirxzlen;
	if (acarg <= -1.0f) return FV_MATH_PI_F;
	if (zdirxzlen == 0.0f || acarg >= 1.0f) return 0.0f;
	roll = FvAcosf(FvClampEx(-1.0f, acarg, +1.0f));

	return xdir.y < 0.f ? -roll : roll;
}
//----------------------------------------------------------------------------
