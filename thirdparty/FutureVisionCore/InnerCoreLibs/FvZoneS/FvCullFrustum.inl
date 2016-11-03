
FV_INLINE
FvZoneBoundary::Portal *FvCullFrustum::CullPlane::GetPortal()
{
	return m_pkPortal;
};

FV_INLINE
void FvCullFrustum::CullPlane::SetPortal(FvZoneBoundary::Portal *pkPortal)
{
	m_pkPortal = pkPortal;
};

FV_INLINE
void FvCullFrustum::SetOrigin(const FvVector3 &kNewOrigin) 
{
	m_kOrigin = kNewOrigin;
}

FV_INLINE
void FvCullFrustum::SetOriginPlane(const FvVector3 &kNormal, const FvVector3 &kPoint)
{
	m_kOriginPlane.Init(kPoint,kNormal);
}

FV_INLINE
void FvCullFrustum::SetUseOriginPlane(bool bUseOriginPlane) 
{
	m_bUseOriginPlane = bUseOriginPlane;
}

FV_INLINE
void FvCullFrustum::SetProjectionType(FvCullFrustum::ProjectionType eType)
{ 
	m_eProjectionType = eType; 
}

FV_INLINE
FvCullFrustum::ProjectionType FvCullFrustum::GetProjectionType() const
{ 
	return m_eProjectionType; 
}