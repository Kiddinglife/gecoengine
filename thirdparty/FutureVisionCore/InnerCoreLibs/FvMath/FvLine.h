//{future header message}
#ifndef __FvLine_H__
#define __FvLine_H__

#include "FvVector2.h"

class FV_MATH_API FvLine
{
public:
	FvLine(const FvVector2& normal, const float d);

	FvLine(const FvVector2& kVector0, const FvVector2& kVector1, bool bNeedToNormalise = true);

	float Intersect(const FvLine& kOther) const;

	FvVector2 Param(float t) const;

	float Project(const FvVector2& kPoint) const;

	bool IsMinCutter(const FvLine& kCutter) const;

	bool IsParallel(const FvLine& kOther) const;

	bool IsInFrontOf(const FvVector2& kPoint) const;

	const FvVector2 &Normal() const;

	float Distance() const;

protected:
	FvVector2 m_kNormal;
	float m_fDistance;
};

#endif /* __FvLine_H__ */
