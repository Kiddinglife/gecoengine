//{future header message}
#ifndef __FvPlane_H__
#define __FvPlane_H__

#include "FvLine.h"
#include "FvVector3.h"

class FV_MATH_API FvPlane
{
public:
	enum Side
	{
		SIDE_POSITIVE = 1,
		SIDE_NEGATIVE = -1,
		SIDE_BOTH = 0
	};

	FvPlane();

	FvPlane(const FvVector3& normal, const float d);

	FvPlane(const FvVector3& kPoint, const FvVector3& kNormal);

	FvPlane(const FvVector3& kPoint0, const FvVector3& kPoint1,
		const FvVector3& kPoint2, bool bShouldNormalise = true);

	void Init(const FvVector3& kPoint0, const FvVector3& kPoint1,
		const FvVector3& kPoint2, bool bShouldNormalise = true);

	void Init(const FvVector3& kPoint, const FvVector3& kNormal);

	float DistanceTo(const FvVector3& kPoint) const;

	bool IsInFrontOf(const FvVector3& kPoint) const;

	bool IsInFrontOfExact(const FvVector3& kPoint) const;
	
	Side GetSide(const FvVector3& kPoint) const;

	Side GetSideExact(const FvVector3& kPoint) const;

	Side GetSide(const FvVector3& kCenter, const FvVector3& kHalfSize) const;

	float Z(float x, float y) const;

	FvVector3 IntersectRay(const FvVector3& kSource, const FvVector3& kDir) const;

	float IntersectRayHalf(const FvVector3& kSource, float fNormalDotDir) const;

	float IntersectRayHalfNoCheck(const FvVector3& kSource, float fOneOverNormalDotDir) const;

	FvLine Intersect(const FvPlane& kSlice) const;

	void Basis(FvVector3& xdir, FvVector3& ydir) const;

	FvVector3 Param(const FvVector2 & kParam) const;

	FvVector2 Project(const FvVector3 & kPoint) const;

	const FvVector3 &Normal() const;
	float Distance() const;

	void SetHidden();
	void SetAlwaysVisible();

protected:
	FvVector3 m_kNormal;
	float m_fDistance;

};

#endif /* __FvPlane_H__ */
