//{future header message}
#ifndef __FvCamera_H__
#define __FvCamera_H__

#include <FvPowerDefines.h>
#include <FvBaseTypes.h>
#include <FvBoundingBox.h>

#include <OgreCamera.h>
#include <OgreSceneManager.h>

#include "FvZoneDefine.h"
#include "FvCullFrustum.h"
#include "FvZoneBoundary.h"

class FV_ZONE_API FvCamera : public Ogre::Camera
{
public:
	enum Visibility
	{
		NONE,
		PARTIAL,
		FULL
	};

	class VisibleObjectFinder
	{
	public:
		virtual void FindVisibleObject( 
			Ogre::RenderQueue *pkQueue,
			FvCamera *pkCamera, 
			Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
			bool bOnlyShadowCasters ){}
	};

	FvCamera(const FvString& kName,
		Ogre::SceneManager *pkSceneManager);
	~FvCamera();

	float GetNearPlane(void) const;
	void SetNearPlane(float fNear);

	float GetFarPlane(void) const;
	void SetFarPlane(float fFar);
	
	float GetFOV(void) const;
	void SetFOV(float fFOV);

	float GetAspectRatio() const;
	void SetAspectRatio( float fRatio );

	float GetViewHeight() const;
	void SetViewHeight(float fHeight);

	float GetViewWidth() const;

	bool GetOrtho() const;
	void SetOrtho(bool bOrtho);

	virtual bool isVisible(const Ogre::AxisAlignedBox &kBound, 
		Ogre::FrustumPlane *pkCulledBy = 0) const;

	virtual bool isVisible(const Ogre::Sphere& bound,
		Ogre::FrustumPlane* culledBy = 0) const;

	bool IsVisible(const FvBoundingBox &kBound) const;
	bool IsVisible(const FvVector3& kCenter, float fRadius) const;
	bool IsVisible (FvZoneBoundary::Portal *pkPortal);
	FvCamera::Visibility GetVisibility(const FvBoundingBox &kBound);

	void Update();

	int AddPortalCullingPlanes(FvZoneBoundary::Portal *pkPortal);
	void RemovePortalCullingPlanes(FvZoneBoundary::Portal *pkPortal);
	void RemoveAllExtraCullingPlanes(void);

	void setProjectionType(Ogre::ProjectionType eType);

	void _DirtySetPosition(const FvVector3& kPos);

	bool _IsSunShadow();

	void _SetSunShadow(bool bEnable);

	void _SetMainPosition(const FvVector3& kPos);

	const FvVector3& _GetMainPosition();

	void SetVisibleFinder(VisibleObjectFinder *pkFinder);
	VisibleObjectFinder *GetVisibleFinder();

	static VisibleObjectFinder ms_kDefaultVisibleObjectFinder;

	void updateFrustumImpl(void) const;
	void updateViewImpl(void) const;
	const Ogre::Matrix4& GetViewProjectionMatrix(void) const;

protected:

	FvCullFrustum m_kCullFrustum;
	bool m_bIsSunShadow;
	FvVector3 m_kMainPosition;

	VisibleObjectFinder *m_pkVisibleObjectFinders;

	mutable Ogre::Matrix4 m_kViewProjectionMatrix;
	mutable bool m_bViewProjectionMatrixDirty;
};

#include "FvCamera.inl"

#endif // __FvCamera_H__