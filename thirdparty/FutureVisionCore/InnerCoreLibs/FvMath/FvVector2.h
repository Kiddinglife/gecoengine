//{future header message}
#ifndef __FvVector2_H__
#define __FvVector2_H__

#include "FvMath.h"

class FV_MATH_API FvVector2
{
public:
	float x, y;

	enum Coord
	{
		X = 0,
		Y = 1
	};

	FvVector2();
	FvVector2(float fX, float fY);
	FvVector2(const float* fpVector);
	FvVector2(const FvVector2& kVector);

	operator float* ();
	operator const float* () const;

	float& operator[] (Coord eCoord);
	float operator[] (Coord eCoord) const;

	FvVector2& operator = (const FvVector2& kVector);

	FvVector2& operator += (const FvVector2& kVector);
	FvVector2& operator -= (const FvVector2& kVector);
	FvVector2& operator *= (float fScale);
	FvVector2& operator /= (float fScale);

	FvVector2 operator + () const;
	FvVector2 operator - () const;

	FvVector2 operator + (const FvVector2& kVector) const;
	FvVector2 operator - (const FvVector2& kVector) const;
	FvVector2 operator * (float fScale) const;
	FvVector2 operator / (float fScale) const;

	FV_MATH_API friend FvVector2 operator * (float fScale, const FvVector2& kVector);
	FV_MATH_API friend FvVector2 operator / (float fScale, const FvVector2& kVector);

	bool operator== (const FvVector2& kVector) const;
	bool operator!= (const FvVector2& kVector) const;

	void SetZero();
	void Set(float fX, float fY);
	void Scale(const FvVector2& kVector, float fScale);
	float Length() const;
	float LengthSquared() const;
	void Normalise();
	void ParallelMultiply(const FvVector2& kVector);
	void ParallelMultiply(const FvVector2& kVector0, const FvVector2& kVector1);
	void Lerp(const FvVector2& kVector0, const FvVector2& kVector1, float fAlpha);
	void Clamp(const FvVector2& kLower, const FvVector2& kUpper);
	FvVector2 UnitVector() const;

	float DotProduct(const FvVector2& kVector) const;
	float CrossProduct(const FvVector2& kVector) const;

	void ProjectOnto(const FvVector2& kVector0, const FvVector2& kVector1);
	FvVector2 ProjectOnto(const FvVector2& kVector) const;

	static const FvVector2 ZERO;
	static const FvVector2 UNIT_X;
	static const FvVector2 UNIT_Y;
};

FV_MATH_API bool FvAlmostEqual(const FvVector2& kVector0, const FvVector2& kVector1, const float fEpsilon = 0.0004f);

#include <FvBinaryStream.h>
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvVector2)

#endif /* __FvVector2_H__ */
