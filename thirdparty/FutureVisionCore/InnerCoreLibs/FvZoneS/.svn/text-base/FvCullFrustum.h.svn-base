//{future header message}
#ifndef __FvCullFrustum_H__
#define __FvCullFrustum_H__

#include <FvPlane.h>
#include <FvVector3.h>

#include "FvZoneDefine.h"
#include "FvZoneBoundary.h"

#include <list>

class FV_ZONE_API FvCullFrustum
{
public:
	enum ProjectionType
	{
		PT_ORTHOGRAPHIC,
		PT_PERSPECTIVE
	};

	enum Visibility
	{
		NONE,
		PARTIAL,
		FULL
	};

	class CullPlane : public FvPlane
	{
	public:
		CullPlane();
		CullPlane(const FvPlane &kPlane);
		CullPlane(const FvVector3& kNormal, 
			const FvVector3 &kPoint);
		CullPlane(const FvVector3& kPoint0, 
			const FvVector3 &kPoint1, const FvVector3 &kPoint2);
		void SetFromOgrePlane(FvPlane &kOgrePlane);

		~CullPlane();

		FvZoneBoundary::Portal *GetPortal();
		void SetPortal(FvZoneBoundary::Portal *pkPortal);

	protected:
		FvZoneBoundary::Portal *m_pkPortal;
	};

	FvCullFrustum();
	~FvCullFrustum();

	bool IsVisible(const FvBoundingBox &kBound) const;
	bool IsVisible(const FvVector3 &kCentre, float fRadius) const;
	bool IsVisible(FvZoneBoundary::Portal *pkPortal);
	bool IsFullyVisible(const FvBoundingBox &kBound);
	bool IsFullyVisible(const FvVector3 kCentre, float fRadius);
	bool IsFullyVisible(FvZoneBoundary::Portal *pkPortal);
	FvCullFrustum::Visibility GetVisibility(const FvBoundingBox kBound);

	int AddPortalCullingPlanes(FvZoneBoundary::Portal *pkPortal);
	void RemovePortalCullingPlanes(FvZoneBoundary::Portal *pkPortal);
	void RemoveAllCullingPlanes(void);

	void SetOrigin(const FvVector3 &kNewOrigin);
	void SetOriginPlane(const FvVector3 &kNormal, const FvVector3 &kPoint);
	void SetUseOriginPlane(bool bUseOriginPlane);

	FvCullFrustum::CullPlane *GetUnusedCullingPlane(void);

	void SetProjectionType(ProjectionType eType);
	ProjectionType GetProjectionType() const;

protected:

	FvVector3 m_kOrigin;
	FvPlane  m_kOriginPlane;
	bool m_bUseOriginPlane;

    typedef std::list<CullPlane *> CullPlaneList;
	CullPlaneList m_kActiveCullingPlanes;
	CullPlaneList m_kCullingPlaneReservoir;

	ProjectionType m_eProjectionType;
};

#include "FvCullFrustum.inl"

#endif // __FvCullFrustum_H__