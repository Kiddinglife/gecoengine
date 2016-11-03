//{future header message}
#ifndef __FvAngle_H__
#define __FvAngle_H__

#include "FvMath.h"

struct FvDegree
{
	FvDegree(float fValue)
	{
		m_fDegree = fValue;
	}

	float m_fDegree;
};

struct FvRadians
{
	FvRadians(float fValue)
	{
		m_fRadians = fValue;
	}

	float m_fRadians;
};

class FV_MATH_API FvAngle
{
public:
	FvAngle();
	FvAngle(const FvAngle& rAngle);
	FvAngle(float fValueInRadians);
	FvAngle(FvRadians fValue);
	FvAngle(FvDegree fValue);
	~FvAngle();

	const FvAngle& operator = (FvAngle kAngle);
	const FvAngle& operator = (float fValueInRadians);
	const FvAngle& operator = (FvRadians fValue);
	const FvAngle& operator = (FvDegree fValue);
	
	const FvAngle& operator += (FvAngle kAngle);
	const FvAngle& operator += (float fValueInRadians);
	const FvAngle& operator += (FvRadians fValue);
	const FvAngle& operator += (FvDegree fValue);

	const FvAngle& operator -= (FvAngle kAngle);
	const FvAngle& operator -= (float fValueInRadians);
	const FvAngle& operator -= (FvRadians fValue);
	const FvAngle& operator -= (FvDegree fValue);

	//operator float& ();

	operator float () const;
	float GetAngle()const;
	
	static void Normalise(float& fAngle);
	static FvAngle Decay(FvAngle kSrc, FvAngle kDst, float fHalfLife, float fDeltaTime);
	static float SameSignAngle(FvAngle kAngle, FvAngle kClosest);
	static FvAngle TurnRange(FvAngle kFrom, FvAngle kTo);
	
	bool ClampBetween(FvAngle kMin, FvAngle kMax);
	bool IsBetween(FvAngle kMin, FvAngle kMax) const;
	void Lerp(FvAngle a, FvAngle b, float t);
	void Normalise();

private:
	float m_fAngle;

};

#endif /* __FvAngle_H__ */
