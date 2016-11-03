#include "FvZoneTerrainObstacle.h"
#include "FvTerrainPage.h"

#ifdef FV_EDITOR_ENABLED
bool s_bCollideTerrainHoles = false;
#endif

class FvTerrainObstacleRayCallback : public FvTerrainCollisionCallback
{
public:
	FvTerrainObstacleRayCallback( FvCollisionState &kCollisionState, const FvZoneTerrainObstacle *pkObstacle,
		const FvVector3 &kStart, const FvVector3 &kEnd) : 
	m_kCollisionState( kCollisionState ),
	m_pkObstacles( pkObstacle ),
	m_kStart( kStart ),
	m_kEnd( kEnd ),
	m_bFinishedColliding( false )
	{
		m_kDir = kEnd - kStart;
		m_fDist = m_kDir.Length();

		m_kDir *= 1.f / m_fDist;
	}
	~FvTerrainObstacleRayCallback()
	{
	}

	bool Collide( const FvWorldTriangle &kTriangle, float fDist)
	{
		//FV_GUARD;
		float fNDist = m_kCollisionState.m_fSTravel + (m_kCollisionState.m_fETravel - m_kCollisionState.m_fSTravel) * (fDist / m_fDist);

		if (m_kCollisionState.m_bOnlyLess && fNDist > m_kCollisionState.m_fDist) return false;
		if (m_kCollisionState.m_bOnlyMore && fNDist < m_kCollisionState.m_fDist) return false;

		FvVector3 kImpact = m_kStart + m_kDir * fDist;

#ifdef FV_EDITOR_ENABLED
		if ( !s_bCollideTerrainHoles )
		{
#endif // FV_EDITOR_ENABLED
		if (m_pkObstacles->TerrainPage().HoleAt( kImpact.x, kImpact.y ))
			return false;
#ifdef FV_EDITOR_ENABLED
		}
#endif // FV_EDITOR_ENABLED

		FvUInt32 uiMaterialKind = 0;
		//if ( pObstacle_->block().dominantTextureMap() != NULL )
		//	uiMaterialKind = pObstacle_->block().dominantTextureMap()->materialKind( kImpact.x, kImpact.y );

		FvWorldTriangle kWT = kTriangle;

		//kWT.Flags( FvWorldTriangle::PackFlags( kWT.CollisionFlags(), uiMaterialKind ) );

		m_kCollisionState.m_fDist = fNDist;

		int iSay = m_kCollisionState.m_kCallback( *m_pkObstacles, kWT, m_kCollisionState.m_fDist );

		if (!iSay) 
		{
			m_bFinishedColliding = true;
			return true;
		}

		m_kCollisionState.m_bOnlyLess = !(iSay & 2);
		m_kCollisionState.m_bOnlyMore = !(iSay & 1);

		return m_kCollisionState.m_bOnlyLess;
	}

	bool FinishedColliding() const { return m_bFinishedColliding; }

private:

	FvCollisionState &m_kCollisionState;
	const FvZoneTerrainObstacle *m_pkObstacles;

	FvVector3 m_kStart;
	FvVector3 m_kEnd;
	FvVector3 m_kDir;
	float m_fDist;

	bool m_bFinishedColliding;
};

class FvTerrainObstaclePrismCallback : public FvTerrainCollisionCallback
{
public:
	FvTerrainObstaclePrismCallback( FvCollisionState &kCollisionState, const FvZoneTerrainObstacle *pkObstacle,
		const FvWorldTriangle &kStart, const FvVector3 &kEnd) : 
	m_kCollisionState( kCollisionState ),
	m_pkObstacle( pkObstacle ),
	m_kStart( kStart ),
	m_kEnd( kEnd )
	{
		m_kDir = kEnd - kStart.Point0();
		m_fDist = m_kDir.Length();

		m_kDir *= 1.f / m_fDist;
	}
	~FvTerrainObstaclePrismCallback()
	{
	}

	bool Collide( const FvWorldTriangle &kTriangle, float fDist)
	{
		//FV_GUARD;
		float fNDist = m_kCollisionState.m_fSTravel + (m_kCollisionState.m_fETravel - m_kCollisionState.m_fSTravel) * (fDist / m_fDist);

		if (m_kCollisionState.m_bOnlyLess && fNDist > m_kCollisionState.m_fDist) return false;
		if (m_kCollisionState.m_bOnlyMore && fNDist < m_kCollisionState.m_fDist) return false;

		FvVector3 kImpact = 
			(kTriangle.Point0() + kTriangle.Point1() + kTriangle.Point2()) * (1.f /3.f);

		if (m_pkObstacle->TerrainPage().HoleAt( kImpact.x, kImpact.y ))
			return false;

		//uint32 uiMaterialKind = 0;
		//if ( pObstacle_->block().dominantTextureMap() != NULL )
		//	uiMaterialKind = pObstacle_->block().dominantTextureMap()->MaterialKind( kImpact.x, kImpact.y );

		FvWorldTriangle kWT = kTriangle;

		//wt.flags( FvWorldTriangle::packFlags( wt.collisionFlags(), uiMaterialKind ) );

		m_kCollisionState.m_fDist = fNDist;

		int iSay = m_kCollisionState.m_kCallback( *m_pkObstacle, kWT, m_kCollisionState.m_fDist );

		if (!iSay) return true;	

		m_kCollisionState.m_bOnlyLess = !(iSay & 2);
		m_kCollisionState.m_bOnlyMore = !(iSay & 1);

		return false;
	}

private:

	FvCollisionState &m_kCollisionState;
	const FvZoneTerrainObstacle *m_pkObstacle;

	FvWorldTriangle	m_kStart;
	FvVector3 m_kEnd;
	FvVector3 m_kDir;
	float m_fDist;
};

FvZoneTerrainObstacle::FvZoneTerrainObstacle( const FvTerrainPage &kTerrainPage,
											 const FvMatrix &kTransform, const FvBoundingBox *pkBoundingBox,
											 FvZoneItemPtr spItem ) :
FvZoneObstacle( kTransform, pkBoundingBox, spItem ),
m_kTerrainPage( kTerrainPage )
{
}

bool FvZoneTerrainObstacle::Collide( const FvVector3 &kStart, const FvVector3 &kEnd,
									FvCollisionState &kState ) const
{
	//FV_GUARD;
	FvTerrainObstacleRayCallback kTOC( kState, this, kStart, kEnd );

	m_kTerrainPage.Collide( kStart, kEnd, &kTOC ); 

	return kTOC.FinishedColliding();
}

bool FvZoneTerrainObstacle::Collide( const FvWorldTriangle &kStart, const FvVector3 &kEnd,
									FvCollisionState &kState ) const
{
	//FV_GUARD;
	FvTerrainObstaclePrismCallback kTOC( kState, this, kStart, kEnd );

	return m_kTerrainPage.Collide( kStart, kEnd, &kTOC );
}

bool FvZoneTerrainObstacle::ClipAgainstBB( FvVector3 &kStart, FvVector3 &kExtent, 
										  float fBloat ) const
{
	return m_kTerrainPage.BoundingBox().Clip( kStart, kExtent, fBloat );
}
