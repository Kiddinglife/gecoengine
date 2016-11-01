//{future header message}
#ifndef __FvZoneTerrainObstacle_H__
#define __FvZoneTerrainObstacle_H__

#include "FvZoneObstacle.h"

class FvBoundingBox;
class FvTerrainPage;

class FvZoneTerrainObstacle : public FvZoneObstacle
{
public:
	FvZoneTerrainObstacle( const FvTerrainPage &kTerrainPage,
		const FvMatrix &kTransform, const FvBoundingBox *pkBoundingBox,
		FvZoneItemPtr spItem );

	virtual bool Collide( const FvVector3 &kStart, const FvVector3 &kEnd,
		FvCollisionState &kState ) const;
	virtual bool Collide( const FvWorldTriangle &kStart, const FvVector3 &kEnd,
		FvCollisionState &kState ) const;

	virtual bool ClipAgainstBB( FvVector3 &kStart, FvVector3 &kExtent, 
		float fBloat = 0.f ) const;

	const FvTerrainPage &TerrainPage() const { return m_kTerrainPage; }

private:

	const FvTerrainPage &m_kTerrainPage;
};


#endif // __FvZoneTerrainObstacle_H__