#include "FvWorldPolygon.h"

#include <FvDebug.h>

//----------------------------------------------------------------------------
static bool TooClose(const FvVector3& kPoint0, const FvVector3& kPoint1)
{
	const float MIN_DIST = 0.001f;
	const float MIN_DIST2 = MIN_DIST * MIN_DIST;

	return (kPoint0 - kPoint1).LengthSquared() <= MIN_DIST2;
}


//----------------------------------------------------------------------------
static int AddToPoly(FvWorldPolygon& kPoly, const FvVector3& kPoint)
{
	if(kPoly.m_kData.empty() || !TooClose(kPoly.m_kData.back(), kPoint))
	{
		kPoly.m_kData.push_back(kPoint);
		return 0;
	}

	return 1;
}
//----------------------------------------------------------------------------
static void CompressPoly(FvWorldPolygon& kPoly)
{
	if(kPoly.m_kData.size() < 3)
	{
		kPoly.m_kData.clear();
		return;
	}

	if(TooClose( kPoly.m_kData.front(), kPoly.m_kData.back()))
	{
		kPoly.m_kData.pop_back();
	}

	if(kPoly.m_kData.size() < 3)
	{
		kPoly.m_kData.clear();
	}
}
//----------------------------------------------------------------------------
FvWorldPolygon::FvWorldPolygon()
{

}
//----------------------------------------------------------------------------
FvWorldPolygon::FvWorldPolygon(size_t stSize) : m_kData(stSize)
{

}
//----------------------------------------------------------------------------
void FvWorldPolygon::Split(const FvPlane& kPlane,
	FvWorldPolygon& kFrontPoly, FvWorldPolygon& kBackPoly) const
{
	kFrontPoly.m_kData.clear();
	kBackPoly.m_kData.clear();

	if(m_kData.empty())
		return;

	if(m_kData.size() < 3)
	{
		return;
	}

	const FvVector3* pkPrevPoint = &m_kData.back();
	float fPrevDist = kPlane.DistanceTo(*pkPrevPoint);
	bool bWasInFront = (fPrevDist > 0.f);
	int iCompressions = 0;

	std::vector<FvVector3>::const_iterator iter = m_kData.begin();

	while(iter != m_kData.end())
	{
		float currDist = kPlane.DistanceTo(*iter);
		bool bIsInFront = (currDist > 0.f);

		if(bIsInFront != bWasInFront)
		{
			float fRatio = FvFabsf(fPrevDist / (currDist - fPrevDist));
			FvVector3 kCutPoint =
				(*pkPrevPoint) + fRatio * (*iter - *pkPrevPoint);

			iCompressions += AddToPoly(kFrontPoly, kCutPoint);
			iCompressions += AddToPoly(kBackPoly, kCutPoint);

			FV_ASSERT(FvFabsf(kPlane.DistanceTo(kCutPoint)) < 0.01f);
		}

		if (bIsInFront)
		{
			iCompressions += AddToPoly(kFrontPoly, *iter);
		}
		else
		{
			iCompressions += AddToPoly(kBackPoly, *iter);
		}

		bWasInFront = bIsInFront;
		pkPrevPoint = &(*iter);
		fPrevDist = currDist;

		iter++;
	}

	CompressPoly(kFrontPoly);
	CompressPoly(kBackPoly);
}
//----------------------------------------------------------------------------
void FvWorldPolygon::Chop(const FvPlane& kPlane)
{
	FvWorldPolygon kNewPoly;
	FvWorldPolygon kDummyPoly;

	Split(kPlane, kNewPoly, kDummyPoly );

	*this = kNewPoly;
}
//----------------------------------------------------------------------------
