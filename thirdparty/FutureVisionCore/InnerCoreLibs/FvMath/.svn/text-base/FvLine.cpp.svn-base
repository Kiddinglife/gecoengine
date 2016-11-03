//{future source message}
#include "FvLine.h"

//----------------------------------------------------------------------------
FvLine::FvLine(const FvVector2& normal, const float d)
	: m_kNormal(normal), m_fDistance(d)
{

}
//----------------------------------------------------------------------------
FvLine::FvLine(const FvVector2& kVector0, const FvVector2& kVector1,
	bool bNeedToNormalise)
{
	m_kNormal.x = kVector0.y - kVector1.y;
	m_kNormal.y = kVector1.x - kVector0.x;
	if(bNeedToNormalise)
		m_kNormal.Normalise();

	m_fDistance = m_kNormal.DotProduct(kVector0);
}
//----------------------------------------------------------------------------
float FvLine::Intersect(const FvLine& kOther) const
{
	const FvVector2& kOn = kOther.m_kNormal;
	return (kOther.m_fDistance - m_fDistance * kOn.DotProduct(m_kNormal)) /
		kOn.CrossProduct(m_kNormal);
}
//----------------------------------------------------------------------------
FvVector2 FvLine::Param(float t) const
{
	return FvVector2(m_kNormal.x * m_fDistance + m_kNormal.y * t,
		m_kNormal.y * m_fDistance - m_kNormal.x * t);
}
//----------------------------------------------------------------------------
float FvLine::Project( const FvVector2 & kPoint ) const
{
	FvVector2 kDir(m_kNormal.y, -m_kNormal.x);
	return kDir.DotProduct(kPoint - m_kNormal * m_fDistance);
}
//----------------------------------------------------------------------------
bool FvLine::IsMinCutter(const FvLine & kCutter) const
{
	return m_kNormal.CrossProduct(kCutter.m_kNormal) > 0.f;
}
//----------------------------------------------------------------------------
bool FvLine::IsParallel(const FvLine & kOther) const
{
	return FvFabsf(m_kNormal.CrossProduct(kOther.m_kNormal)) < 0.001f;
}
//----------------------------------------------------------------------------
bool FvLine::IsInFrontOf(const FvVector2& kPoint) const
{
	return m_kNormal.DotProduct(kPoint) > m_fDistance;
}
//----------------------------------------------------------------------------
const FvVector2& FvLine::Normal() const
{
	return m_kNormal;
}
//----------------------------------------------------------------------------
float FvLine::Distance() const
{
	return m_fDistance;
}
//----------------------------------------------------------------------------
