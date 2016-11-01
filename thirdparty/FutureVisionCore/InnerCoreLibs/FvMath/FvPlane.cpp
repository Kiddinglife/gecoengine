//{future source message}
#include "FvPlane.h"

//----------------------------------------------------------------------------
FvPlane::FvPlane() : m_kNormal(FvVector3::UNIT_X), m_fDistance(0)
{

}
//----------------------------------------------------------------------------
FvPlane::FvPlane(const FvVector3& kNormal, const float d) : m_kNormal(kNormal),
	m_fDistance(d)
{

}
//----------------------------------------------------------------------------
FvPlane::FvPlane(const FvVector3& kPoint, const FvVector3& kNormal)
{
	Init(kPoint, kNormal);
}
//----------------------------------------------------------------------------
FvPlane::FvPlane(const FvVector3& kPoint0, const FvVector3& kPoint1,
	const FvVector3& kPoint2, bool bShouldNormalise)
{
	Init(kPoint0, kPoint1, kPoint2, bShouldNormalise);
}
//----------------------------------------------------------------------------
void FvPlane::Init(const FvVector3& kPoint0, const FvVector3& kPoint1,
	const FvVector3& kPoint2, bool bShouldNormalise)
{
	m_kNormal.CrossProduct(kPoint1 - kPoint0, kPoint2 - kPoint0);

	if(bShouldNormalise)
		m_kNormal.Normalise();

	m_fDistance = m_kNormal.DotProduct(kPoint0);
}
//----------------------------------------------------------------------------
void FvPlane::Init(const FvVector3& kPoint, const FvVector3& kNormal)
{
	m_kNormal = kNormal;
	m_fDistance = kNormal.DotProduct(kPoint);
}
//----------------------------------------------------------------------------
float FvPlane::DistanceTo(const FvVector3& kPoint) const
{
	return m_kNormal.DotProduct(kPoint) - m_fDistance;
}
//----------------------------------------------------------------------------
bool FvPlane::IsInFrontOf(const FvVector3& kPoint) const
{
	float fNormalDotPoint = m_kNormal.DotProduct(kPoint);
	if (FvAlmostEqual(fNormalDotPoint, m_fDistance))
		return true;
	return fNormalDotPoint > m_fDistance;
}
//----------------------------------------------------------------------------
bool FvPlane::IsInFrontOfExact(const FvVector3& kPoint) const
{
	return m_kNormal.DotProduct(kPoint) > m_fDistance;
}
//----------------------------------------------------------------------------
float FvPlane::Z(float x, float y) const
{
	if (m_kNormal[FV_Z_COORD] == 0)
		return 0.0f;
	return (m_fDistance - m_kNormal[FV_X_COORD] * x -
		m_kNormal[FV_Y_COORD] * y) / m_kNormal[FV_Z_COORD];
}
//----------------------------------------------------------------------------
FvVector3 FvPlane::IntersectRay(const FvVector3& kSource,
	const FvVector3& kDir) const
{
	return kSource +
		IntersectRayHalf(kSource, m_kNormal.DotProduct(kDir)) * kDir;
}
//----------------------------------------------------------------------------
float FvPlane::IntersectRayHalf(const FvVector3& kSource,
	float fNormalDotDir) const
{
	if (fNormalDotDir == 0)
		return 0.0f;
	return (m_fDistance - m_kNormal.DotProduct(kSource)) / fNormalDotDir;
}
//----------------------------------------------------------------------------
float FvPlane::IntersectRayHalfNoCheck(const FvVector3& kSource,
	float fOneOverNormalDotDir) const
{
	return (m_fDistance - m_kNormal.DotProduct(kSource)) *
		fOneOverNormalDotDir;
}
//----------------------------------------------------------------------------
FvLine FvPlane::Intersect(const FvPlane& kSlice) const
{
	const FvPlane& kBasis = *this;

	FvVector3 kDir3D;
	kDir3D.CrossProduct(kBasis.Normal(), kSlice.Normal());

	if (FvAlmostZero(kDir3D.LengthSquared(), 0.0001f))
		return FvLine(FvVector2(0.0f, 0.0f), 0.0f);

	FvVector3 kNormal3D;
	kNormal3D.CrossProduct(m_kNormal, kDir3D);

	FvVector3 kOrigin3D = Param(FvVector2(0, 0));

	FvVector3 kIntersectPoint3D = kSlice.IntersectRay(kOrigin3D, kNormal3D);

	FvVector2 kIntersectPoint2D = Project(kIntersectPoint3D);

	FvVector2 kLineNormal = Project(kOrigin3D + kNormal3D);
	kLineNormal.Normalise();

	float fLineD = kLineNormal.DotProduct(kIntersectPoint2D);

	FvVector3 ir = Param(kIntersectPoint2D);

	return FvLine(kLineNormal, fLineD);
}
//----------------------------------------------------------------------------
const FvVector3& FvPlane::Normal() const
{
	return m_kNormal;
}
//----------------------------------------------------------------------------
float FvPlane::Distance() const
{
	return m_fDistance;
}
//----------------------------------------------------------------------------
FvVector3 FvPlane::Param(const FvVector2 & kParam) const
{
	FvVector3 xdir, ydir;
	Basis(xdir, ydir);

	float dOverNormLen = m_fDistance / m_kNormal.LengthSquared();

	return dOverNormLen * m_kNormal + kParam.x * xdir + kParam.y * ydir;
}
//----------------------------------------------------------------------------
void FvPlane::Basis(FvVector3& xdir, FvVector3& ydir) const
{
	float fabses[3] =
	{
		float(FvFabsf(m_kNormal.x)),
		float(FvFabsf(m_kNormal.y)),
		float(FvFabsf(m_kNormal.z))
	};

	int bi = fabses[0] > fabses[1] ?
		(fabses[0] > fabses[2] ? 0 : 2) : (fabses[1] > fabses[2] ? 1 : 2);

	const int biMoreL[3] = { 1, 2, 0 };
	const int biLessL[3] = { 2, 0, 1 };
	
	int biMore = biMoreL[bi];
	int biLess = biLessL[bi];

	xdir[biLess] = 0;
	xdir[biMore] = 1;
	xdir[bi] = -m_kNormal[biMore] / m_kNormal[ bi ];

	ydir.CrossProduct(m_kNormal, xdir);

	xdir.Normalise();
	ydir.Normalise();
}
//----------------------------------------------------------------------------
FvVector2 FvPlane::Project(const FvVector3 & kPoint) const
{
	FvVector3 xdir, ydir;
	Basis(xdir, ydir);

	FvVector3 kOffset = kPoint - (m_fDistance / m_kNormal.LengthSquared()) *
		m_kNormal;

	return FvVector2(xdir.DotProduct(kOffset), ydir.DotProduct(kOffset));
}
//----------------------------------------------------------------------------
void FvPlane::SetHidden()
{
	m_kNormal.Set(0, 0, 0);
	m_fDistance = 1;
}
//----------------------------------------------------------------------------
void FvPlane::SetAlwaysVisible()
{
	m_kNormal.Set(0, 0, 0);
	m_fDistance = -1;
}
//----------------------------------------------------------------------------
FvPlane::Side FvPlane::GetSide(const FvVector3& kPoint) const
{
	float fNormalDotPoint = m_kNormal.DotProduct(kPoint);
	if (FvAlmostEqual(fNormalDotPoint, m_fDistance))
		return SIDE_BOTH;
	if(fNormalDotPoint > m_fDistance)
	{
		return SIDE_POSITIVE;
	}
	else
	{
		return SIDE_NEGATIVE;
	}
}
//----------------------------------------------------------------------------
FvPlane::Side FvPlane::GetSide(const FvVector3& kCenter,
	const FvVector3& kHalfSize) const
{
	float fDistanceToCenter = DistanceTo(kCenter);
	float fMaxAbsDistance =
		FvFabsf(m_kNormal.x * kHalfSize.x) +
		FvFabsf(m_kNormal.y * kHalfSize.y) +
		FvFabsf(m_kNormal.z * kHalfSize.z);
	
	if(fDistanceToCenter < -fMaxAbsDistance)
		return SIDE_NEGATIVE;

	if (fDistanceToCenter > +fMaxAbsDistance)
		return SIDE_POSITIVE;

	return SIDE_BOTH;
}
//----------------------------------------------------------------------------
FvPlane::Side FvPlane::GetSideExact(const FvVector3& kPoint) const
{
	float fNormalDotPoint = m_kNormal.DotProduct(kPoint);
	
	if(fNormalDotPoint > m_fDistance)
	{
		return SIDE_POSITIVE;
	}
	
	if(fNormalDotPoint < m_fDistance)
	{
		return SIDE_NEGATIVE;
	}

	return SIDE_BOTH;
}
//----------------------------------------------------------------------------
