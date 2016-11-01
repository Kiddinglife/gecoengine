
FV_INLINE
float FvCamera::GetNearPlane(void) const
{
	return this->getNearClipDistance();
}	

FV_INLINE
void FvCamera::SetNearPlane(float fNear)
{
	this->setNearClipDistance(fNear);
}

FV_INLINE
float FvCamera::GetFarPlane(void) const
{
	return this->getFarClipDistance();
}

FV_INLINE
void FvCamera::SetFarPlane(float fFar)
{
	this->setFarClipDistance(fFar);
}

FV_INLINE
float FvCamera::GetFOV(void) const
{
	return 90.f;
}

FV_INLINE
void FvCamera::SetFOV(float fFOV)
{

}

FV_INLINE
float FvCamera::GetAspectRatio() const
{
	return this->getAspectRatio();
}

FV_INLINE
void FvCamera::SetAspectRatio( float fRatio )
{
	this->setAspectRatio(fRatio);
}

FV_INLINE
float FvCamera::GetViewHeight() const
{
	return 0.f;
}

FV_INLINE
void FvCamera::SetViewHeight(float fHeight)
{

}

FV_INLINE
float FvCamera::GetViewWidth() const
{
	return 0.f;
}

FV_INLINE
bool FvCamera::GetOrtho() const
{
	return true;
}

FV_INLINE
void FvCamera::SetOrtho(bool bOrtho)
{

}

FV_INLINE
bool FvCamera::isVisible(const Ogre::AxisAlignedBox &kBound, 
						 Ogre::FrustumPlane *pkCulledBy) const
{
	return this->IsVisible(*(FvBoundingBox*)&kBound);
}

FV_INLINE
bool FvCamera::isVisible( const Ogre::Sphere& bound,
	Ogre::FrustumPlane* culledBy) const
{
	return this->IsVisible(*(FvVector3*)&(bound.getCenter()), bound.getRadius());
}

FV_INLINE
int FvCamera::AddPortalCullingPlanes(FvZoneBoundary::Portal *pkPortal)
{
	return m_kCullFrustum.AddPortalCullingPlanes(pkPortal);
}

FV_INLINE
void FvCamera::RemovePortalCullingPlanes(FvZoneBoundary::Portal *pkPortal)
{
	m_kCullFrustum.RemovePortalCullingPlanes(pkPortal);
}

FV_INLINE
void FvCamera::RemoveAllExtraCullingPlanes(void)
{
	m_kCullFrustum.RemoveAllCullingPlanes();
}

FV_INLINE
void FvCamera::setProjectionType(Ogre::ProjectionType eType)
{
	m_kCullFrustum.SetProjectionType(FvCullFrustum::ProjectionType(eType));
	Camera::setProjectionType(eType);
}

FV_INLINE
void FvCamera::_DirtySetPosition( const FvVector3& kPos )
{
	mPosition.x = kPos.x;
	mPosition.y = kPos.y;
	mPosition.z = kPos.z;
}

FV_INLINE
bool FvCamera::_IsSunShadow()
{
	return m_bIsSunShadow;
}

FV_INLINE
void FvCamera::_SetSunShadow(bool bEnable)
{
	m_bIsSunShadow = bEnable;
}

FV_INLINE
void FvCamera::SetVisibleFinder(VisibleObjectFinder *pkFinder)
{
	m_pkVisibleObjectFinders = pkFinder;
}

FV_INLINE
FvCamera::VisibleObjectFinder *FvCamera::GetVisibleFinder()
{
	return m_pkVisibleObjectFinders;
}