//{future header message}
#ifndef __FvVector4_H__
#define __FvVector4_H__

#include "FvVector3.h"

class FV_MATH_API FvVector4
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

	FvVector4();
	FvVector4(float fX, float fY, float fZ, float fW);
	FvVector4(const float* fpVector);
	FvVector4(const FvVector4& kVector);
	FvVector4(const FvVector2& kVector, float fZ, float fW);
	FvVector4(float fX, const FvVector2& kVector, float fW);
	FvVector4(float fX, float fY, const FvVector2& kVector);
	FvVector4(const FvVector2& kVector0, const FvVector2& kVector1);
	FvVector4(const FvVector3& kVector, float fW);
	FvVector4(float fX, const FvVector3& kVector);

	operator float* ();
	operator const float* () const;

	float& operator[] (Coord eCoord);
	float operator[] (Coord eCoord) const;

	FvVector4& operator = (const FvVector4& kVector);

	FvVector4& operator += (const FvVector4& kVector);
	FvVector4& operator -= (const FvVector4& kVector);
	FvVector4& operator *= (float fScale);
	FvVector4& operator /= (float fScale);

	FvVector4 operator + () const;
	FvVector4 operator - () const;

	FvVector4 operator + (const FvVector4& kVector) const;
	FvVector4 operator - (const FvVector4& kVector) const;
	FvVector4 operator * (float fScale) const;
	FvVector4 operator / (float fScale) const;

	FV_MATH_API friend FvVector4 operator * (float fScale, const FvVector4& kVector);
	FV_MATH_API friend FvVector4 operator / (float fScale, const FvVector4& kVector);

	bool operator== (const FvVector4& kVector) const;
	bool operator!= (const FvVector4& kVector) const;

	void SetZero();
	void Set(float fX, float fY, float fZ, float fW);
	void Scale(const FvVector4& kVector, float fScale);
	void Scale(float fScale);
	float Length() const;
	float LengthSquared() const;
	void Normalise();
	void ParallelMultiply(const FvVector4& kVector);
	void ParallelMultiply(const FvVector4& kVector0, const FvVector4& kVector1);
	float DotProduct(const FvVector4& kVector) const;
	FvOutcode CalculateOutcode() const;
	void Lerp(const FvVector4& kVector0, const FvVector4& kVector1, float fAlpha);
	void Clamp(const FvVector4& kLower, const FvVector4& kUpper);
	FvVector4 UnitVector() const;

	static const FvVector4 ZERO;
	static const FvVector4 UNIT_X;
	static const FvVector4 UNIT_Y;
	static const FvVector4 UNIT_Z;
	static const FvVector4 ZERO_POINT;
	static const FvVector4 UNIT_X_POINT;
	static const FvVector4 UNIT_Y_POINT;
	static const FvVector4 UNIT_Z_POINT;
};

FV_MATH_API bool FvAlmostEqual(const FvVector4& kVector0, const FvVector4& kVector1, const float fEpsilon = 0.0004f);

#include <FvBinaryStream.h>
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvVector4)

#endif /* __FvVector4_H__ */
