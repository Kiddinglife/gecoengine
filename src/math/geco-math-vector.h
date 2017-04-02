//{future header message}
#ifndef __FvVector2_H__
#define __FvVector2_H__

#include "geco-math-power.h"

class GECOAPI GecoVector2
{
public:
	float x, y;
	bool Normalised;

	enum Coord
	{
		X = 0,
		Y = 1
	};

	GecoVector2();
	GecoVector2(float fX, float fY);
	GecoVector2(const float* fpVector);
	GecoVector2(const GecoVector2& kVector);

	operator float* ();
	operator const float* () const;

	float& operator[] (Coord eCoord);
	float operator[] (Coord eCoord) const;

	GecoVector2& operator = (const GecoVector2& kVector);

	GecoVector2& operator += (const GecoVector2& kVector);
	GecoVector2& operator -= (const GecoVector2& kVector);
	GecoVector2& operator *= (float fScale);
	GecoVector2& operator /= (float fScale);

	GecoVector2 operator + () const;
	GecoVector2 operator - () const;

	GecoVector2 operator + (const GecoVector2& kVector) const;
	GecoVector2 operator - (const GecoVector2& kVector) const;
	GecoVector2 operator * (float fScale) const;
	GecoVector2 operator / (float fScale) const;

	GECOAPI friend GecoVector2 operator * (float fScale, const GecoVector2& kVector);
	GECOAPI friend GecoVector2 operator / (float fScale, const GecoVector2& kVector);

	bool operator== (const GecoVector2& kVector) const;
	bool operator!= (const GecoVector2& kVector) const;

	void SetZero();
	void Set(float fX, float fY);
	void Scale(const GecoVector2& kVector, float fScale);
	float Length() const;
	float LengthSquared() const;
	void Normalise();
	void ParallelMultiply(const GecoVector2& kVector);
	void ParallelMultiply(const GecoVector2& kVector0, const GecoVector2& kVector1);
	void Lerp(const GecoVector2& kVector0, const GecoVector2& kVector1, float fAlpha);
	void Clamp(const GecoVector2& kLower, const GecoVector2& kUpper);
	GecoVector2 UnitVector() const;

	float DotProduct(const GecoVector2& kVector) const;
	float CrossProduct(const GecoVector2& kVector) const;

	void ProjectOnto(const GecoVector2& kVector0, const GecoVector2& kVector1);
	GecoVector2 ProjectOnto(const GecoVector2& kVector) const;

	void to(geco_bit_stream_t& kOS, bool normalized = false);
	void from(geco_bit_stream_t& kIS, bool normalized = false);

	static const GecoVector2 ZERO;
	static const GecoVector2 UNIT_X;
	static const GecoVector2 UNIT_Y;
};

class GECOAPI GecoVector3
{
public:
	float x, y, z;
	bool Normalised;

	enum Coord
	{
		X = 0,
		Y = 1,
		Z = 2
	};

	GecoVector3();
	GecoVector3(float fX, float fY, float fZ);
	GecoVector3(const float* fpVector);
	GecoVector3(const GecoVector3& kVector);
	GecoVector3(const GecoVector2& kVector, float fZ);
	GecoVector3(float fX, const GecoVector2& kVector);

	operator float* ();
	operator const float* () const;

	float& operator[] (Coord eCoord);
	float operator[] (Coord eCoord) const;

	GecoVector3& operator = (const GecoVector3& kVector);

	GecoVector3& operator += (const GecoVector3& kVector);
	GecoVector3& operator -= (const GecoVector3& kVector);
	GecoVector3& operator *= (float fScale);
	GecoVector3& operator /= (float fScale);

	GecoVector3 operator + () const;
	GecoVector3 operator - () const;

	GecoVector3 operator + (const GecoVector3& kVector) const;
	GecoVector3 operator - (const GecoVector3& kVector) const;
	GecoVector3 operator * (float fScale) const;
	GecoVector3 operator / (float fScale) const;

	GECOAPI friend GecoVector3 operator * (float fScale, const GecoVector3& kVector);
	GECOAPI friend GecoVector3 operator / (float fScale, const GecoVector3& kVector);

	bool operator== (const GecoVector3& kVector) const;
	bool operator!= (const GecoVector3& kVector) const;

	void SetZero();
	void Set(float fX, float fY, float fZ);
	void Scale(const GecoVector3& kVector, float fScale);
	float Length() const;
	float LengthSquared() const;
	void Normalise();
	void ParallelMultiply(const GecoVector3& kVector);
	void ParallelMultiply(const GecoVector3& kVector0, const GecoVector3& kVector1);
	void Lerp(const GecoVector3& kVector0, const GecoVector3& kVector1, float fAlpha);
	void Clamp(const GecoVector3& kLower, const GecoVector3& kUpper);
	GecoVector3 UnitVector() const;

	void SetPitchYaw(float fPitchInRadians, float fYawInRadians);
	float GetYaw() const;
	float GetPitch() const;

	float DotProduct(const GecoVector3& kVector) const;
	void CrossProduct(const GecoVector3& kVector0, const GecoVector3& kVector1);
	GecoVector3 CrossProduct(const GecoVector3& kVector) const;

	void ProjectOnto(const GecoVector3& kVector0, const GecoVector3& kVector1);
	GecoVector3 ProjectOnto(const GecoVector3& kVector) const;

	static const GecoVector3 ZERO;
	static const GecoVector3 UNIT_X;
	static const GecoVector3 UNIT_Y;
	static const GecoVector3 UNIT_Z;
};

GECOAPI bool FvAlmostEqual(const GecoVector3 &kVector0, const GecoVector3 &kVector1, const float fEpsilon = 0.0004f);
GECOAPI bool GecoAlmostEqual(const GecoVector2& kVector0, const GecoVector2& kVector1, const float fEpsilon = 0.0004f);

GECOAPI float FvDistance(const GecoVector3 &kVector0, const GecoVector3 &kVector1);
GECOAPI float FvDistance2(const GecoVector3 &kVector0, const GecoVector3 &kVector1);//! length*length


INLINE geco_bit_stream_t & operator >> (geco_bit_stream_t& kIS, GecoVector2& kDes)
{
	if (kIS.is_compression_mode())
	{
		kDes.Normalised ? kIS.ReadNormVector(kDes.x, kDes.y) : kIS.ReadVector(kDes.x, kDes.y);
	}
	else
	{
		kIS.Read(kDes.x);
		kIS.Read(kDes.y);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator << (geco_bit_stream_t& kOS, const GecoVector2& kDes)
{
	if (kOS.is_compression_mode())
	{
		kDes.Normalised ? kOS.WriteNormVector(kDes.x, kDes.y) : kOS.WriteVector(kDes.x, kDes.y);
	}
	{
		kOS.Write(kDes.x);
		kOS.Write(kDes.y);
	}
	return kOS;
}

INLINE geco_bit_stream_t & operator >> (geco_bit_stream_t& kIS, GecoVector3& kDes)
{
	if (kIS.is_compression_mode())
	{
		kDes.Normalised ? kIS.ReadNormVector(kDes.x, kDes.y, kDes.z) : kIS.ReadVector(kDes.x, kDes.y, kDes.z);
	}
	else
	{
		kIS.Read(kDes.x);
		kIS.Read(kDes.y);
		kIS.Read(kDes.z);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator << (geco_bit_stream_t& kOS, const GecoVector3& kDes)
{
	if (kOS.is_compression_mode())
	{
		kDes.Normalised ? kOS.WriteNormVector(kDes.x, kDes.y, kDes.z) : kOS.WriteVector(kDes.x, kDes.y, kDes.z);
	}
	{
		kOS.Write(kDes.x);
		kOS.Write(kDes.y);
		kOS.Write(kDes.z);
	}
	return kOS;
}

#endif /* __FvVector2_H__ */
