//{future header message}
#ifndef __FvVector3_H__
#define __FvVector3_H__

#include "FvVector2.h"

class FV_MATH_API FvVector3
{
public:
	float x, y, z;

	enum Coord
	{
		X = 0,
		Y = 1,
		Z = 2
	};

	FvVector3();
	FvVector3(float fX, float fY, float fZ);
	FvVector3(const float* fpVector);
	FvVector3(const FvVector3& kVector);
	FvVector3(const FvVector2& kVector, float fZ);
	FvVector3(float fX, const FvVector2& kVector);

	operator float* ();
	operator const float* () const;

	float& operator[] (Coord eCoord);
	float operator[] (Coord eCoord) const;

	FvVector3& operator = (const FvVector3& kVector);

	FvVector3& operator += (const FvVector3& kVector);
	FvVector3& operator -= (const FvVector3& kVector);
	FvVector3& operator *= (float fScale);
	FvVector3& operator /= (float fScale);

	FvVector3 operator + () const;
	FvVector3 operator - () const;

	FvVector3 operator + (const FvVector3& kVector) const;
	FvVector3 operator - (const FvVector3& kVector) const;
	FvVector3 operator * (float fScale) const;
	FvVector3 operator / (float fScale) const;

	FV_MATH_API friend FvVector3 operator * (float fScale, const FvVector3& kVector);
	FV_MATH_API friend FvVector3 operator / (float fScale, const FvVector3& kVector);

	bool operator== (const FvVector3& kVector) const;
	bool operator!= (const FvVector3& kVector) const;

	void SetZero();
	void Set(float fX, float fY, float fZ);
	void Scale(const FvVector3& kVector, float fScale);
	float Length() const;
	float LengthSquared() const;
	void Normalise();
	void ParallelMultiply(const FvVector3& kVector);
	void ParallelMultiply(const FvVector3& kVector0, const FvVector3& kVector1);
	void Lerp(const FvVector3& kVector0, const FvVector3& kVector1, float fAlpha);
	void Clamp(const FvVector3& kLower, const FvVector3& kUpper);
	FvVector3 UnitVector() const;

	void SetPitchYaw(float fPitchInRadians, float fYawInRadians);
	float GetYaw() const;
	float GetPitch() const;

	float DotProduct(const FvVector3& kVector) const;
	void CrossProduct(const FvVector3& kVector0, const FvVector3& kVector1);
	FvVector3 CrossProduct(const FvVector3& kVector) const;

	void ProjectOnto(const FvVector3& kVector0, const FvVector3& kVector1);
	FvVector3 ProjectOnto(const FvVector3& kVector) const;

	static const FvVector3 ZERO;
	static const FvVector3 UNIT_X;
	static const FvVector3 UNIT_Y;
	static const FvVector3 UNIT_Z;
};

FV_MATH_API bool FvAlmostEqual(const FvVector3 &kVector0, const FvVector3 &kVector1, const float fEpsilon = 0.0004f);
FV_MATH_API float FvDistance(const FvVector3 &kVector0, const FvVector3 &kVector1);
FV_MATH_API float FvDistance2(const FvVector3 &kVector0, const FvVector3 &kVector1);//! length*length

#include <FvBinaryStream.h>
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvVector3)

#endif /* __FvVector3_H__ */
