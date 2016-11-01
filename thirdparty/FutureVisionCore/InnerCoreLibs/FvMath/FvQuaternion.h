//{future header message}
#ifndef __FvQuaternion_H__
#define __FvQuaternion_H__

#include "FvMath.h"
#include "FvVector3.h"

class FvMatrix;

class FV_MATH_API FvQuaternion
{
public:
	float x, y, z, w;

	enum Coord
	{
		X = 0,
		Y = 1,
		Z = 2,
		W = 3
	};

	FvQuaternion();
	FvQuaternion(float fX, float fY, float fZ, float fW);
	FvQuaternion(const float* fpQuat);
	FvQuaternion(const FvQuaternion& kQuat);
	FvQuaternion(const FvVector3& kVec, float fW);

	operator float* ();
	operator const float* () const;

	FvQuaternion& operator += (const FvQuaternion& kQuat);
	FvQuaternion& operator -= (const FvQuaternion& kQuat);
	FvQuaternion& operator *= (const FvQuaternion& kQuat);
	FvQuaternion& operator *= (float fScale);
	FvQuaternion& operator /= (float fScale);

	FvQuaternion operator + () const;
	FvQuaternion operator - () const;

	FvQuaternion operator + (const FvQuaternion& kQuat) const;
	FvQuaternion operator - (const FvQuaternion& kQuat) const;
	FvQuaternion operator * (const FvQuaternion& kQuat) const;
	FvQuaternion operator * (float fScale) const;
	FvQuaternion operator / (float fScale) const;

	friend FvQuaternion operator * (float fScale,const FvQuaternion& kQuat);

	FvVector3 operator * (const FvVector3& kVec);

	bool operator == (const FvQuaternion& kQuat) const;
	bool operator != (const FvQuaternion& kQuat) const;

	void SetZero();
	void Set(float fX, float fY, float fZ, float fW);
	void Set(const FvVector3& kVec, float fW);

	void FromAngleAxis(float fAngle, const FvVector3& fAxis);

	void FromMatrix(const FvMatrix& kMat);

	void Normalise();
	void Invert();
	void Minimise();

	void Slerp(const FvQuaternion& kStart, const FvQuaternion &kEnd, float t);

	void Multiply(const FvQuaternion& kQuat0, const FvQuaternion& kQuat1);
	void PreMultiply(const FvQuaternion& kQuat);
	void PostMultiply(const FvQuaternion& kQuat);

	float DotProduct(const FvQuaternion& kQuat) const;
	float Length() const;
	float LengthSquared() const;
};

#endif /* __FvQuaternion_H__ */
