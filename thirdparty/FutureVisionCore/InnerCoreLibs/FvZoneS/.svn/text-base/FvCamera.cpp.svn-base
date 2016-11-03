#include "FvCamera.h"

#include <FvDebug.h>

#include <OgrePlane.h>
#include <OgreFrustum.h>

FvCamera::VisibleObjectFinder FvCamera::ms_kDefaultVisibleObjectFinder;

FvCamera::FvCamera(const FvString &kName,
				   Ogre::SceneManager *pkSceneManager) :
Ogre::Camera(kName,pkSceneManager),
m_bIsSunShadow(false),
m_pkVisibleObjectFinders(NULL),
m_kMainPosition(0,0,0),
m_bViewProjectionMatrixDirty(false)
{
}

FvCamera::~FvCamera()
{

}

bool FvCamera::IsVisible(const FvBoundingBox &kBound) const
{
	if(kBound.InsideOut())
		return false;

	Frustum::updateFrustumPlanes();

	/*const Ogre::AxisAlignedBox& kBounding = getBoundingBox();

	FvBoundingBox kCameraBounding((FvVector3&)kBounding.getMinimum(), (FvVector3&)kBounding.getMaximum());

	if(kCameraBounding.Intersects(kBound))
	{*/
		bool bCullResults;
		bCullResults = m_kCullFrustum.IsVisible(kBound);
		if (!bCullResults)
			return false;

		FvVector3 kCentre = kBound.Centre();
		FvVector3 kHalfSize = kBound.HalfSize();

		for (int iPlane = 0; iPlane < 6; ++iPlane)
		{
			if (iPlane == Ogre::FRUSTUM_PLANE_FAR && mFarDist == 0)
				continue;

			Ogre::Plane::Side eSide = mFrustumPlanes[iPlane].getSide(
				*(Ogre::Vector3*)&kCentre, *(Ogre::Vector3*)&kHalfSize);
			if (eSide == Ogre::Plane::NEGATIVE_SIDE)
			{
				return false;
			}

		}

		return true;
	/*}
	else
	{
		return false;
	}*/
}


bool FvCamera::IsVisible( const FvVector3& kCenter, float fRadius ) const
{
	Frustum::updateFrustumPlanes();

	bool bCullResults;
	bCullResults = m_kCullFrustum.IsVisible(kCenter, fRadius);
	if (!bCullResults)
		return false;

	for (int iPlane = 0; iPlane < 6; ++iPlane)
	{
		if (iPlane == Ogre::FRUSTUM_PLANE_FAR && mFarDist == 0)
			continue;

		float fDist = mFrustumPlanes[iPlane].getDistance(*(Ogre::Vector3*)&kCenter);

		if(fDist < (-fRadius))
		{
			return false;
		}
	}

	return true;
}

bool FvCamera::IsVisible(FvZoneBoundary::Portal *pkPortal)
{
	if (pkPortal == NULL)
		return false;

	FvVector3 kCameraToPortal = pkPortal->m_kCentre - *(FvVector3*)&getDerivedPosition();
	float fDotProduct = kCameraToPortal.DotProduct(pkPortal->m_kWNormal);
	if ( fDotProduct > 0 )
		return false;

	if (!m_kCullFrustum.IsVisible(pkPortal))
		return false;

	Frustum::updateFrustumPlanes();

	bool bVisibleFlag;

	for (int iPlane = 1; iPlane < 6; ++iPlane)
	{
		bVisibleFlag = false;
		if (iPlane == Ogre::FRUSTUM_PLANE_FAR && mFarDist == 0)
			continue;

		for (unsigned int iCorner = 0; iCorner < pkPortal->m_kWPoints.size(); iCorner++)
		{
			Ogre::Plane::Side eSide = mFrustumPlanes[iPlane].getSide(
				*(Ogre::Vector3*)&pkPortal->m_kWPoints[iCorner]);
			if (eSide != Ogre::Plane::NEGATIVE_SIDE)
			{
				bVisibleFlag = true;
			}
		}
		if (bVisibleFlag == false)
		{
			return false;
		}
	}

	return true;
}

FvCamera::Visibility FvCamera::GetVisibility(const FvBoundingBox &kBound)
{
	if ( kBound.InsideOut() )
		return NONE;

	FvVector3 kCentre = kBound.Centre();
	FvVector3 kHalfSize = kBound.HalfSize();

	bool bAllInside = true;

	for ( int iPlane = 0; iPlane < 6; ++iPlane )
	{
		if (iPlane == Ogre::FRUSTUM_PLANE_FAR && mFarDist == 0)
			continue;

		Ogre::Plane::Side eSide = getFrustumPlane(iPlane).getSide(*(Ogre::Vector3*)&kCentre, 
			*(Ogre::Vector3*)&kHalfSize);
		if(eSide == Ogre::Plane::NEGATIVE_SIDE) return NONE;
		if(eSide == Ogre::Plane::BOTH_SIDE) 
			bAllInside = false;
	}

	switch(m_kCullFrustum.GetVisibility(kBound))
	{
	case FvCullFrustum::NONE:
		return NONE;
	case FvCullFrustum::PARTIAL:
		return PARTIAL;
	case FvCullFrustum::FULL:
		break;
	}

	if ( bAllInside )
		return FULL;
	else
		return PARTIAL;
}

void FvCamera::Update()
{
	if (m_kCullFrustum.GetProjectionType() == 
		FvCullFrustum::PT_PERSPECTIVE && !this->mCustomViewMatrix)
	{
		m_kCullFrustum.SetUseOriginPlane(true);
		m_kCullFrustum.SetOrigin(*(FvVector3*)&getDerivedPosition());
		m_kCullFrustum.SetOriginPlane(*(FvVector3*)&getDerivedDirection(),
			*(FvVector3*)&getDerivedPosition());
	}
	else
	{
		m_kCullFrustum.SetUseOriginPlane(false);
	}
}

void FvCamera::_SetMainPosition(const FvVector3& kPos)
{
	m_kMainPosition = kPos;
}

const FvVector3& FvCamera::_GetMainPosition()
{
	return m_kMainPosition;
}

void FvCamera::updateFrustumImpl(void) const
{
	Ogre::Camera::updateFrustumImpl();
	m_bViewProjectionMatrixDirty = true;
}

void FvCamera::updateViewImpl(void) const
{
	Ogre::Camera::updateViewImpl();
	m_bViewProjectionMatrixDirty = true;
}

const Ogre::Matrix4& FvCamera::GetViewProjectionMatrix(void) const
{
	if(m_bViewProjectionMatrixDirty)
	{
		m_kViewProjectionMatrix = const_cast<FvCamera*>(this)->getProjectionMatrixWithRSDepth() * 
			const_cast<FvCamera*>(this)->getViewMatrix(true);
		m_bViewProjectionMatrixDirty = false;
	}
	return m_kViewProjectionMatrix;
}
//----------------------------------------------------------------------------
