#include "FvZoneObstacle.h"
#include "FvZoneManager.h"

#include <FvBsp.h>
#include <FvWatcher.h>
#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvUInt32 FvZoneObstacle::ms_uiNextMark = 0;

FvZoneObstacle::FvZoneObstacle( const FvMatrix &kTransform,
							 const FvBoundingBox *kBB, FvZoneItemPtr spItem ) :
m_uiMark( ms_uiNextMark - 16 ),
m_kBB( *kBB ),
m_spItem( spItem ),
m_kTransform( kTransform )
{
	m_kTransformInverse.Invert( kTransform );
}

FvZoneObstacle::~FvZoneObstacle()
{

}

bool FvZoneObstacle::Mark() const
{
	if (m_uiMark == ms_uiNextMark) return true;
	m_uiMark = ms_uiNextMark;
	return false;
}

bool FvZoneObstacle::ClipAgainstBB( FvVector3 &kStart, FvVector3 &kExtent, 
								  float fBloat ) const
{
	return m_kBB.Clip( kStart, kExtent, fBloat );
}

class FvCSCV : public FvCollisionVisitor
{
public:
	FvCSCV( const FvZoneBSPObstacle &kObstacle, FvCollisionState & kState ) :
	  m_kBSPObstacle( kObstacle ), m_kState( kState ), m_bStop( false ) { }

private:
	virtual bool Visit( const FvWorldTriangle &kHitTriangle, float fRD );

	const FvZoneBSPObstacle &m_kBSPObstacle;
	FvCollisionState &m_kState;
	bool m_bStop;

	friend class FvZoneBSPObstacle;
};

bool FvCSCV::Visit( const FvWorldTriangle &kHitTriangle, float fRD )
{	
	//FV_GUARD;
	float fNDist = m_kState.m_fSTravel + (m_kState.m_fETravel - m_kState.m_fSTravel) * fRD;

	if (m_kState.m_bOnlyLess && fNDist > m_kState.m_fDist) return true;	 
	if (m_kState.m_bOnlyMore && fNDist < m_kState.m_fDist) return false;	 
	m_kState.m_fDist = fNDist;

	int iSay = m_kState.m_kCallback( m_kBSPObstacle, kHitTriangle, m_kState.m_fDist );

	if( (iSay & 3) != 3) m_kState.m_fDist = fNDist;

	if (!iSay)
	{
		m_bStop = true;
		return true;
	}

	m_kState.m_bOnlyLess = !(iSay & 2);
	m_kState.m_bOnlyMore = !(iSay & 1);

	return m_kState.m_bOnlyLess;
}

FvZoneBSPObstacle::FvZoneBSPObstacle( const FvBSPTree &kBSPTree,
								   const FvMatrix &kTransform, const FvBoundingBox *kBB, FvZoneItemPtr spItem ) :
FvZoneObstacle( kTransform, kBB, spItem ),
m_kBSPTree( kBSPTree )
{
}

bool FvZoneBSPObstacle::Collide( const FvVector3 &kSource,
							   const FvVector3 &kExtent, FvCollisionState &kState ) const
{
	//FV_GUARD;
	FvCSCV kCSCV( *this, kState );

	float fRD = 1.0f;
#ifdef FV_COLLISION_DEBUG
	FV_DEBUG_MSG( "FvZoneBSPObstacle::Collide(pt): %s\n",
		m_kBSPTree.name().c_str() );
#endif // FV_COLLISION_DEBUG
	m_kBSPTree.GetRoot()->Intersects( kSource, kExtent, fRD, NULL, &kCSCV );

	return kCSCV.m_bStop;
}

bool FvZoneBSPObstacle::Collide( const FvWorldTriangle &kSource,
							   const FvVector3 &kExtent, FvCollisionState &kState ) const
{
	FvCSCV kCSCV( *this, kState );

#ifdef FV_COLLISION_DEBUG
	FV_DEBUG_MSG( "FvZoneBSPObstacle::Collide(kSource): %s\n",
		m_kBSPTree.name().c_str() );
#endif // FV_COLLISION_DEBUG

	m_kBSPTree.GetRoot()->Intersects( kSource, kExtent - kSource.Point0(), &kCSCV );

	return kCSCV.m_bStop;
}

#ifndef FV_SERVER

float FvZoneEnvironmentCollider::Ground( const FvVector3 &kPos, float fDropDistance, bool bOneSided )
{
	// FV_GUARD;
	FvVector3 kLowestPoint( kPos.x, kPos.y, kPos.z - fDropDistance );

	float fDist = -1.f;
	FvZoneSpace *pkCS = &*FvZoneManager::Instance().CameraSpace();
	if (pkCS != NULL)
    {
        fDist = 
            pkCS->Collide
            ( 
                kPos, 
                kLowestPoint, 
                FvClosestObstacleEx
                (
                    bOneSided, 
                    FvVector3(0.0f, 0.0f, -1.0f)
                )
            );
    }
	if (fDist < 0.0f) return FvEnvironmentCollider::NO_GROUND_COLLISION;

	return kPos.z - fDist;
}

float FvZoneEnvironmentCollider::Ground( const FvVector3 &kPos )
{
	// FV_GUARD;
	FvVector3 kDropSrc( kPos.x, kPos.y, kPos.z + 15.f );
	FvVector3 kDropMax( kPos.x, kPos.y, kPos.z - 15.f );

	float fDist = -1.f;
	FvZoneSpace *pkCS = &*FvZoneManager::Instance().CameraSpace();
	if (pkCS != NULL)
		fDist = pkCS->Collide( kDropSrc, kDropMax, FvClosestObstacle::ms_kDefault );
	if (fDist < 0.f) return FvEnvironmentCollider::NO_GROUND_COLLISION;

	return kPos.z + 15.f - fDist;
}

#define GROUND_TEST_INTERVAL 500.f
float FvZoneEnvironmentTerrainCollider::Ground( const FvVector3 &kPos )
{
	// FV_GUARD;
	FvVector3 kDropSrc( kPos.x, kPos.y, kPos.z + GROUND_TEST_INTERVAL );
	FvVector3 kDropMax( kPos.x, kPos.y, kPos.z - GROUND_TEST_INTERVAL );

	float fDist = -1.f;
	FvZoneSpace *pkCS = &*FvZoneManager::Instance().CameraSpace();
	FvClosestTerrainObstacle kTerrainCallback;
	if (pkCS != NULL)
		fDist = pkCS->Collide( kDropSrc, kDropMax, kTerrainCallback );
	if (fDist < 0.f) return FvEnvironmentCollider::NO_GROUND_COLLISION;

	return kPos.z + GROUND_TEST_INTERVAL - fDist;
}

#include <FvWorldTriangle.h>
#ifdef FV_EDITOR_ENABLED
extern char * g_pcZoneParticlesLabel;
#endif // FV_EDITOR_ENABLED

class FvCRCClosestTriangle : public FvCollisionCallback
{
public:
	virtual int operator()( const FvZoneObstacle &kObstacle,
		const FvWorldTriangle &kTriangle, float fDist )
	{
		// FV_GUARD;	
#ifdef FV_EDITOR_ENABLED
		if (kObstacle.Item()->Label() == g_pcZoneParticlesLabel)
			return COLLIDE_ALL;
#endif // FV_EDITOR_ENABLED

		m_kCollider = FvWorldTriangle(
			kObstacle.m_kTransform.ApplyPoint( kTriangle.Point0() ),
			kObstacle.m_kTransform.ApplyPoint( kTriangle.Point1() ),
			kObstacle.m_kTransform.ApplyPoint( kTriangle.Point2() ) );

		return COLLIDE_BEFORE;
	}

	static FvCRCClosestTriangle ms_kDefault;
	FvWorldTriangle m_kCollider;
};

FvCRCClosestTriangle FvCRCClosestTriangle::ms_kDefault;

float FvZoneEnvironmentCollider::Collide( const FvVector3 &kStart, const FvVector3 &kEnd, FvWorldTriangle &kResult )
{
	// FV_GUARD;
	float fDist = -1.f;
	FvZoneSpace *pkCS = &*FvZoneManager::Instance().CameraSpace();
	if (pkCS != NULL)
		fDist = pkCS->Collide( kStart, kEnd, FvCRCClosestTriangle::ms_kDefault );
	if (fDist < 0.f) return FvEnvironmentCollider::NO_GROUND_COLLISION;

	kResult = FvCRCClosestTriangle::ms_kDefault.m_kCollider;

	return fDist;
}

FvClosestTerrainObstacle::FvClosestTerrainObstacle() :
m_fDist( -1 ),
m_bCollided( false )
{
}


int FvClosestTerrainObstacle::operator() ( const FvZoneObstacle &kObstacle,
										const FvWorldTriangle &kTriangle, float fDist )
{
	if (kTriangle.GetFlags() & FV_TRIANGLE_TERRAIN)
	{
		if (!m_bCollided || fDist < m_fDist)
		{
			m_fDist = fDist;
			m_bCollided = true;
		}

		return COLLIDE_BEFORE;
	}
	else
	{
		return COLLIDE_ALL;
	}
}


void FvClosestTerrainObstacle::Reset()
{
	m_fDist = -1;
	m_bCollided = false;
}


float FvClosestTerrainObstacle::Dist() const
{
	return m_fDist;
}


bool FvClosestTerrainObstacle::Collided() const
{
	return m_bCollided;
}
#endif // FV_SERVER


FvCollisionAdvance::FvCollisionAdvance( const FvVector3 &kOrigin,
									   const FvVector3 &kAxis1, const FvVector3 &kAxis2,
									   const FvVector3 &kDirection,
									   float fMaxAdvance ) :
m_kDir( kDirection ),
m_fAdvance( fMaxAdvance ),
m_uiIgnoreFlags( 0 )
{
	//FV_GUARD;
	bool bIsRightHanded = (kDirection.DotProduct( kAxis1.CrossProduct( kAxis2 ) ) ) > 0.f;

	FvVector3 kV1;
	FvVector3 kV2;

	if (bIsRightHanded)
	{
		kV1 = kAxis2;
		kV2 = kAxis1;
	}
	else
	{
		kV1 = kAxis1;
		kV2 = kAxis2;
	}

	m_kPlaneEq0.Init( kOrigin,		kDirection.CrossProduct( kV2 ) );
	m_kPlaneEq1.Init( kOrigin + kV2,kDirection.CrossProduct( kV1 ) );
	m_kPlaneEq2.Init( kOrigin + kV1,kV2.CrossProduct( kDirection ) );
	m_kPlaneEq3.Init( kOrigin,		kV1.CrossProduct( kDirection ) );

	FvVector3 kNormal( kV2.CrossProduct( kV1 ) );
	kNormal.Normalise();
	m_fAdjust = 1.f/m_kDir.DotProduct( kNormal );

	m_kPlaneEqBase.Init( kOrigin,	kNormal );
}

FvCollisionAdvance::~FvCollisionAdvance()
{
}

int FvCollisionAdvance::operator()( const FvZoneObstacle &kObstacle,
								   const FvWorldTriangle &kTriangle, float fDist )
{
	//FV_GUARD;

	if (kTriangle.GetFlags() & m_uiIgnoreFlags)
	{
		return COLLIDE_ALL;
	}

	FvWorldPolygon kPoly(3);

	kPoly.m_kData[0] = kObstacle.m_kTransform.ApplyPoint( kTriangle.Point0() );
	kPoly.m_kData[1] = kObstacle.m_kTransform.ApplyPoint( kTriangle.Point1() );
	kPoly.m_kData[2] = kObstacle.m_kTransform.ApplyPoint( kTriangle.Point2() );

	FvWorldTriangle kWT( kPoly.m_kData[0], kPoly.m_kData[1], kPoly.m_kData[2],
		kTriangle.GetFlags() );

	kPoly.Chop( m_kPlaneEq0 );
	kPoly.Chop( m_kPlaneEq1 );
	kPoly.Chop( m_kPlaneEq2 );
	kPoly.Chop( m_kPlaneEq3 );
	kPoly.Chop( m_kPlaneEqBase );

	std::vector<FvVector3>::const_iterator kIter = kPoly.m_kData.begin();

	while (kIter != kPoly.m_kData.end())
	{
		const float fDist = m_kPlaneEqBase.DistanceTo( *kIter ) * m_fAdjust;

		if (fDist < m_fAdvance)
		{
			m_fAdvance = fDist;

			m_kHitTriangle = kWT;
			m_spHitItem = kObstacle.Item();
		}

		kIter++;
	}

	return COLLIDE_ALL;
}

FvCollisionCallback FvCollisionCallback::ms_kDefault;
FvClosestObstacle FvClosestObstacle::ms_kDefault;

FV_ZONE_API FvCollisionCallback &s_kCollisionCallbackDefault = FvCollisionCallback::ms_kDefault;