#include "FvCullFrustum.h"

FvCullFrustum::CullPlane::CullPlane() :
FvPlane()
{
	m_pkPortal = NULL;
}

FvCullFrustum::CullPlane::CullPlane(const FvPlane &kPlane) :
FvPlane(kPlane)
{
	m_pkPortal = NULL;
}

FvCullFrustum::CullPlane::CullPlane(const FvVector3& kNormal, 
									const FvVector3 &kPoint) :
FvPlane(kPoint,kNormal)
{
	m_pkPortal = NULL;
}

FvCullFrustum::CullPlane::CullPlane(const FvVector3 &kPoint0, 
									const FvVector3 &kPoint1, const FvVector3 &kPoint2):
FvPlane(kPoint0,kPoint1,kPoint2)
{
	m_pkPortal = NULL;
}

void FvCullFrustum::CullPlane::SetFromOgrePlane(FvPlane &kOgrePlane)
{
	m_fDistance = kOgrePlane.Distance();
	m_kNormal = kOgrePlane.Normal();
	m_pkPortal = 0;
}

FvCullFrustum::CullPlane::~CullPlane()
{

}

FvCullFrustum::FvCullFrustum() :
m_eProjectionType(PT_PERSPECTIVE), 
m_bUseOriginPlane(false)
{

}

FvCullFrustum::~FvCullFrustum()
{
	RemoveAllCullingPlanes();

	CullPlaneList::iterator kIt = m_kCullingPlaneReservoir.begin();
	while ( kIt != m_kCullingPlaneReservoir.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		kIt++;
		delete pkCullPlane;
	}
	m_kCullingPlaneReservoir.clear();
}

bool FvCullFrustum::IsVisible(const FvBoundingBox &kBound) const
{
	if (kBound.InsideOut()) 
		return false;

	FvVector3 kCentre = kBound.Centre();
	FvVector3 kHalfSize = kBound.HalfSize();

	if (m_bUseOriginPlane)
	{
		FvPlane::Side eSide = m_kOriginPlane.GetSide(kCentre, kHalfSize);
		if (eSide == FvPlane::SIDE_NEGATIVE)
		{
			return false;
		}
	}

	CullPlaneList::const_iterator kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		FvPlane::Side eSide = pkCullPlane->GetSide(kCentre, kHalfSize);
		if (eSide == FvPlane::SIDE_NEGATIVE)
		{
			return false;
		}
		kIt++;
	}
	return true;
}

bool FvCullFrustum::IsVisible(const FvVector3 &kCentre, float fRadius) const
{
	if (m_bUseOriginPlane)
	{
		FvPlane::Side eSide = m_kOriginPlane.GetSide(kCentre);
		if (eSide == FvPlane::SIDE_NEGATIVE)
		{
			float fDist = m_kOriginPlane.DistanceTo(kCentre);
			if (fDist > fRadius)
			{
				return false;
			}
		}
	}

	CullPlaneList::const_iterator kPIt = m_kActiveCullingPlanes.begin();
	while ( kPIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kPIt;
		FvPlane::Side eSide = pkCullPlane->GetSide(kCentre);
		if (eSide == FvPlane::SIDE_NEGATIVE)
		{
			float fDist = pkCullPlane->DistanceTo(kCentre);
			if (fDist > fRadius)
			{
				return false;
			}
		}
		kPIt++;
	}
	return true;
}

bool FvCullFrustum::IsVisible(FvZoneBoundary::Portal *pkPortal)
{
	if (pkPortal == NULL)
		return false;

	if (m_kActiveCullingPlanes.size() == 0)
		return true;

	CullPlaneList::const_iterator kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		if (pkCullPlane->GetPortal() == pkPortal)
		{
			return false;
		}
		kIt++;
	}

	bool bVisibleFlag;

	if (m_bUseOriginPlane)
	{
		bVisibleFlag = false;
		for (unsigned int iCorner = 0; iCorner < pkPortal->m_kWPoints.size(); iCorner++)
		{
			FvPlane::Side eSide = m_kOriginPlane.GetSide(pkPortal->m_kWPoints[iCorner]);
			if (eSide != FvPlane::SIDE_NEGATIVE)
			{
				bVisibleFlag = true;
			}
		}
		if (bVisibleFlag == false)
		{
			return false;
		}
	}

	kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		bVisibleFlag = false;

		for (unsigned int iCorner = 0; iCorner < pkPortal->m_kWPoints.size(); iCorner++)
		{
			FvPlane::Side eSide = pkCullPlane->GetSide(pkPortal->m_kWPoints[iCorner]);
			if (eSide != FvPlane::SIDE_NEGATIVE)
			{
				bVisibleFlag = true;
			}
		}
		if (bVisibleFlag == false)
		{
			return false;
		}
		kIt++;
	}

	return true;
}

bool FvCullFrustum::IsFullyVisible(const FvBoundingBox &kBound)
{
	if (kBound.InsideOut()) 
		return false;

	FvVector3 kCentre = kBound.Centre();
	FvVector3 kHalfSize = kBound.HalfSize();

	if (m_bUseOriginPlane)
	{
		FvPlane::Side eSide = m_kOriginPlane.GetSide(kCentre, kHalfSize);
		if (eSide != FvPlane::SIDE_POSITIVE)
			return false;
	}

	CullPlaneList::const_iterator kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		FvPlane::Side eSide = pkCullPlane->GetSide(kCentre, kHalfSize);
		if (eSide != FvPlane::SIDE_POSITIVE)
		{
			return false;
		}
		kIt++;
	}
	return true;
}

bool FvCullFrustum::IsFullyVisible(const FvVector3 kCentre, float fRadius)
{
	if (m_bUseOriginPlane)
	{
		if (m_kOriginPlane.DistanceTo(kCentre) <= fRadius ||
			m_kOriginPlane.GetSide(kCentre) != FvPlane::SIDE_POSITIVE)
		{
				return false;
		}
	}

	CullPlaneList::const_iterator kPIt = m_kActiveCullingPlanes.begin();
	while ( kPIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kPIt;
		if (pkCullPlane->DistanceTo(kCentre) <= fRadius ||
			pkCullPlane->GetSide(kCentre) != FvPlane::SIDE_POSITIVE)
		{
			return false;
		}
		kPIt++;
	}
	return true;
}

bool FvCullFrustum::IsFullyVisible(FvZoneBoundary::Portal *pkPortal)
{
	if (pkPortal == NULL)
		return false;

	if (m_kActiveCullingPlanes.size() == 0)
		return true;

	CullPlaneList::const_iterator kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		if (pkCullPlane->GetPortal() == pkPortal)
		{
			return false;
		}
		kIt++;
	}

	if (m_bUseOriginPlane)
	{
		for (unsigned int iCorner = 0; iCorner < pkPortal->m_kWPoints.size(); iCorner++)
		{
			FvPlane::Side eSide = m_kOriginPlane.GetSide(pkPortal->m_kWPoints[iCorner]);
			if (eSide == FvPlane::SIDE_NEGATIVE)
			{
				return false;
			}
		}
	}

	kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;

		for (unsigned int iCorner = 0; iCorner < pkPortal->m_kWPoints.size(); iCorner++)
		{
			FvPlane::Side eSide = pkCullPlane->GetSide(pkPortal->m_kWPoints[iCorner]);
			if (eSide == FvPlane::SIDE_NEGATIVE)
			{
				return false;
			}
		}
		kIt++;
	}

	return true;
}

FvCullFrustum::Visibility FvCullFrustum::GetVisibility(const FvBoundingBox kBound)
{
	if(kBound.InsideOut())
		return NONE;

	FvVector3 kCentre = kBound.Centre();
	FvVector3 kHalfSize = kBound.HalfSize();

	bool bAllInside = true;

	if (m_bUseOriginPlane)
	{
		FvPlane::Side eSide = m_kOriginPlane.GetSide(kCentre, kHalfSize);
		if (eSide == FvPlane::SIDE_NEGATIVE)
		{
			return NONE;
		}
		if(eSide == FvPlane::SIDE_BOTH) 
		{
			bAllInside = false;
		}
	}

	CullPlaneList::iterator kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		FvPlane::Side eSide = pkCullPlane->GetSide(kCentre, kHalfSize);
		if(eSide == FvPlane::SIDE_NEGATIVE) 
		{
			return NONE;
		}
		if(eSide == FvPlane::SIDE_BOTH) 
		{
			bAllInside = false;
		}
		kIt++;
	}

	if ( bAllInside )
		return FULL;
	else
		return PARTIAL;
}

int FvCullFrustum::AddPortalCullingPlanes(FvZoneBoundary::Portal *pkPortal)
{		
	int iAddedCullingPlanes = 0;

	unsigned int i,j;
	FvPlane::Side ePT0Side, ePT1Side;
	bool bVisible;
	CullPlaneList::iterator kPIt;
	for (i = 0; i < pkPortal->m_kWPoints.size(); i++)
	{
		j = i+1;
		if (j > (pkPortal->m_kWPoints.size() - 1))
		{
			j = 0;
		}
		bVisible = true;
		kPIt = m_kActiveCullingPlanes.begin();
		while ( kPIt != m_kActiveCullingPlanes.end() )
		{
			CullPlane *pkCullPlane = *kPIt;
			ePT0Side = pkCullPlane->GetSide(pkPortal->m_kWPoints[i]);
			ePT1Side = pkCullPlane->GetSide(pkPortal->m_kWPoints[j]);
			if (ePT0Side == FvPlane::SIDE_NEGATIVE &&
				ePT1Side == FvPlane::SIDE_NEGATIVE)
			{
				bVisible = false;
			}
			kPIt++;
		}
		if (bVisible)
		{
			CullPlane *pkNewCullPlane = this->GetUnusedCullingPlane();
			if (m_eProjectionType == PT_ORTHOGRAPHIC) 
			{
				pkNewCullPlane->Init(pkPortal->m_kWPoints[j] + m_kOriginPlane.Normal(),
					pkPortal->m_kWPoints[j], pkPortal->m_kWPoints[i]);
			}
			else
			{
				pkNewCullPlane->Init(m_kOrigin, pkPortal->m_kWPoints[j], pkPortal->m_kWPoints[i]);	
			}
			pkNewCullPlane->SetPortal(pkPortal);
			m_kActiveCullingPlanes.push_front(pkNewCullPlane);
			iAddedCullingPlanes++;
		}
	}
	if (iAddedCullingPlanes > 0)
	{
		CullPlane *pkNewCullPlane = this->GetUnusedCullingPlane();
		pkNewCullPlane->Init(pkPortal->m_kWPoints[2], pkPortal->m_kWPoints[1], pkPortal->m_kWPoints[0]);
		pkNewCullPlane->SetPortal(pkPortal);
		m_kActiveCullingPlanes.push_back(pkNewCullPlane);
		iAddedCullingPlanes++;
	}
	return iAddedCullingPlanes;
}

void FvCullFrustum::RemovePortalCullingPlanes(FvZoneBoundary::Portal *pkPortal)
{
	CullPlaneList::iterator kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		if (pkCullPlane->GetPortal() == pkPortal)
		{
			m_kCullingPlaneReservoir.push_front(pkCullPlane);
			kIt = m_kActiveCullingPlanes.erase(kIt);
		}
		else
		{
			kIt++;
		}
	}
}

void FvCullFrustum::RemoveAllCullingPlanes(void)
{
	CullPlaneList::iterator kIt = m_kActiveCullingPlanes.begin();
	while ( kIt != m_kActiveCullingPlanes.end() )
	{
		CullPlane *pkCullPlane = *kIt;
		m_kCullingPlaneReservoir.push_front(pkCullPlane);
		kIt++;
	}
	m_kActiveCullingPlanes.clear();
}

FvCullFrustum::CullPlane *FvCullFrustum::GetUnusedCullingPlane(void)
{
	CullPlane *pkCullPlane = 0;
	if (m_kCullingPlaneReservoir.size() > 0)
	{
		CullPlaneList::iterator kIt = m_kCullingPlaneReservoir.begin();
		pkCullPlane = *kIt;
		m_kCullingPlaneReservoir.erase(kIt);
		return pkCullPlane;
	}
	pkCullPlane = new CullPlane;
	return pkCullPlane;
}