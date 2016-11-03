//{future header message}
#ifndef __FvMatrix_H__
#define __FvMatrix_H__

#include "FvVector4.h"

class FvQuaternion;

class FV_MATH_API FvMatrix
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};

	FvMatrix();
	FvMatrix(const float* fpMatrix);
	FvMatrix(const FvMatrix& kMatrix);
	FvMatrix(
		float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44);

	FvMatrix(FvVector4& kRow0, FvVector4& kRow1, FvVector4& kRow2, FvVector4& kRow3);

	operator float* ();
	operator const float* () const;

	float& operator () (unsigned int uiRow, unsigned int uiCol);
	const float operator () (unsigned int uiRow, unsigned int uiCol) const;	

	FvMatrix& operator *= (const FvMatrix& kMatrix);
	FvMatrix& operator += (const FvMatrix& kMatrix);
	FvMatrix& operator -= (const FvMatrix& kMatrix);
	FvMatrix& operator *= (float fScale);
	FvMatrix& operator /= (float fScale);

	FvMatrix operator + () const;
	FvMatrix operator - () const;

	FvMatrix operator * (const FvMatrix& kMatrix) const;
	FvMatrix operator + (const FvMatrix& kMatrix) const;
	FvMatrix operator - (const FvMatrix& kMatrix) const;
	FvMatrix operator * (float fScale) const;
	FvMatrix operator / (float fScale) const;

	friend FvMatrix operator * (float fScale, const FvMatrix& kMatrix);

	bool operator == (const FvMatrix& kMatrix) const;
	bool operator != (const FvMatrix& kMatrix) const;

	FvVector3& operator [] (int i);
	const FvVector3& operator [] (int i) const; 

	FvVector4 & Row(int i);
	const FvVector4 & Row(int i) const;
	void Row(int i, const FvVector4& kVec);

	FvVector4 Col(int i) const;
	void Col(int i, const FvVector4& kVec);

	void SetZero();
	void SetIdentity();

	void SetScale(const float x, const float y, const float z);
	void SetScale(const FvVector3& kScale);

	void SetTranslate(const float x, const float y, const float z);
	void SetTranslate(const FvVector3& kPos);

	void SetRotateX(const float fAngle);
	void SetRotateY(const float fAngle);
	void SetRotateZ(const float fAngle);
	void SetRotate(const FvQuaternion& kQuat);
	void SetRotate(float fYaw, float fPitch, float fRoll);
	void SetRotateInverse(float fYaw, float fPitch, float fRoll);

	void Multiply(const FvMatrix& kMat0, const FvMatrix& kMat1);
	void PreMultiply(const FvMatrix& kMat);
	void PostMultiply(const FvMatrix& kMat);

	void InvertOrthonormal(const FvMatrix& kMat);
	void InvertOrthonormal();
	bool Invert(const FvMatrix& kMat);
	bool Invert();
	float GetDeterminant() const;

	void Transpose(const FvMatrix& kMat);
	void Transpose();

	void LookAt(const FvVector3& kPosition, const FvVector3& kDirection, const FvVector3& kUp);

	FvVector3 ApplyPoint(const FvVector3& kVec) const;
	void ApplyPoint(FvVector3& kVec0, const FvVector3& kVec1) const;
	void ApplyPoint(FvVector4& kVec0, const FvVector3& kVec1) const;
	void ApplyPoint(FvVector4& kVec0, const FvVector4& kVec1) const;

	FvVector3 ApplyVector(const FvVector3& kVec) const;
	void ApplyVector(FvVector3& kVec0, const FvVector3& kVec1) const;

	const FvVector3& ApplyToUnitAxisVector(int iAxis) const;
	const FvVector3& ApplyToOrigin() const;

	void PreRotateX(const float fAngle);
	void PreRotateY(const float fAngle);
	void PreRotateZ(const float fAngle);
	void PreTranslateBy(const FvVector3& kVec);

	void PostRotateX(const float fAngle);
	void PostRotateY(const float fAngle);
	void PostRotateZ(const float fAngle);
	void PostTranslateBy(const FvVector3& kVec);

	bool IsMirrored() const;

	void OrthogonalProjection(float w, float h, float zn, float zf);
	void PerspectiveProjection(float fov, float aspectRatio, float nearPlane, float farPlane);

	float Yaw() const;
	float Pitch() const;
	float Roll() const;

	static const FvMatrix ZERO;
	static const FvMatrix IDENTITY;
};

#include <FvBinaryStream.h>
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvMatrix)

#endif /* __FvQuaternion_H__ */
