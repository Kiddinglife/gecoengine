//{future header message}
#ifndef __FvZoneObstacle_H__
#define __FvZoneObstacle_H__

#include <FvSmartPointer.h>
#include <FvMatrix.h>
#include <FvPlane.h>
#include <FvBoundingBox.h>
#include <FvWorldTriangle.h>

#include "FvZone.h"
#include "FvZoneItem.h"

#ifndef FV_SERVER
#include "FvEnvironmentCollider.h"
#endif // FV_SERVER

class FvZoneObstacle;
class FvCollisionState;
class FvZone;
class FvZoneSpace;
class FvWorldTriangle;
class FvBSPTree;
class FvBoundingBox;

const int COLLIDE_STOP = 0;
const int COLLIDE_BEFORE = 1;
const int COLLIDE_AFTER = 2;
const int COLLIDE_ALL = COLLIDE_BEFORE | COLLIDE_AFTER;

class FvCollisionCallback
{
public:
	
	virtual int operator()( const FvZoneObstacle & /*kObstacle*/,
		const FvWorldTriangle & /*kTriangle*/, float /*fDist*/ )
	{
		return COLLIDE_STOP;
	}

	static FvCollisionCallback ms_kDefault;
};

class FvClosestObstacle : public FvCollisionCallback
{
public:
	virtual int operator()( const FvZoneObstacle & /*kObstacle*/,
		const FvWorldTriangle & /*kTriangle*/, float /*fDist*/ )
	{
		return COLLIDE_BEFORE;
	}

	static FvClosestObstacle ms_kDefault;
};

class FvClosestObstacleEx : public FvCollisionCallback
{
public:

	FvClosestObstacleEx(bool bOneSided, const FvVector3 &kRayDir):
	m_bOneSided(bOneSided),
	m_kRayDir(kRayDir)
	{
		m_kRayDir.Normalise();
	}

	virtual int operator()( const FvZoneObstacle & /*kObstacle*/,
		const FvWorldTriangle &kTriangle, float /*fDist*/ )
	{
		if (!m_bOneSided)
		{
			return COLLIDE_BEFORE; 
		}
		else
		{
			FvVector3 kNormal = kTriangle.Normal();
			kNormal.Normalise();
			if (kNormal.DotProduct(m_kRayDir) > 0)
				return COLLIDE_ALL; 
			else
				return COLLIDE_BEFORE;
		}
	}

private:
	bool m_bOneSided;
	FvVector3 m_kRayDir;
};

class FvZoneObstacle : public FvReferenceCount
{
public:
	FvZoneObstacle( const FvMatrix &kTransform, const FvBoundingBox *kBB,
		FvZoneItemPtr spItem );
	virtual ~FvZoneObstacle();

	mutable FvUInt32 m_uiMark;

	const FvBoundingBox &m_kBB;

private:
	FvZoneItemPtr m_spItem;

public:
	FvMatrix m_kTransform;
	FvMatrix m_kTransformInverse;

	bool Mark() const;
	static void NextMark() { ms_uiNextMark++; }
	static FvUInt32 ms_uiNextMark;

	virtual bool Collide( const FvVector3 &kSource, const FvVector3 &kExtent,
		FvCollisionState &kState ) const = 0;
	virtual bool Collide( const FvWorldTriangle &kSource, const FvVector3 &kExtent,
		FvCollisionState &kState ) const = 0;

	virtual bool ClipAgainstBB( FvVector3 &kStart, FvVector3 &kExtent, 
		float fBloat = 0.f ) const;

	FvZoneItemPtr Item() const;
	FvZone *Zone() const;
};

typedef FvSmartPointer<FvZoneObstacle> FvZoneObstaclePtr;

class FvZoneBSPObstacle : public FvZoneObstacle
{
public:
	FvZoneBSPObstacle( const FvBSPTree &kBSP, const FvMatrix &kTransform,
		const FvBoundingBox *kBB, FvZoneItemPtr spItem );

	virtual bool Collide( const FvVector3 &kSource, const FvVector3 &kExtent,
		FvCollisionState &kState ) const;
	virtual bool Collide( const FvWorldTriangle &kSource, const FvVector3 &kExtent,
		FvCollisionState &kState ) const;
private:
	const FvBSPTree &m_kBSPTree;
};

class FvCollisionState
{
public:
	FvCollisionState( FvCollisionCallback &kCallback ) :
	  m_kCallback( kCallback ),
		  m_bOnlyLess( false ),
		  m_bOnlyMore( false ),
		  m_fSTravel( 0.f ),
		  m_fETravel( 0.f ),
		  m_fDist( -1.0f )
	  { }

public:
	FvCollisionCallback	&m_kCallback;	 
	bool m_bOnlyLess;					 
	bool m_bOnlyMore;					 
	float m_fSTravel;				 
	float m_fETravel;				 
	float m_fDist;				 
};




#ifndef FV_SERVER
//class ChunkObstacleOccluder : public PhotonOccluder
//{
//public:
//
//	ChunkObstacleOccluder() { }
//
//	virtual float collides(
//		const Vector3 & photonSourcePosition,
//		const Vector3 & cameraPosition,
//		const LensEffect& le );
//};

class FvZoneEnvironmentCollider : public FvEnvironmentCollider
{
public:

	FvZoneEnvironmentCollider() { }

	virtual float Ground( const FvVector3 &kPos, float fDropDistance, bool bOnesided );
	virtual float Ground( const FvVector3 &kPos );
	virtual float Collide( const FvVector3 &kStart, const FvVector3 &kEnd, FvWorldTriangle &kResult );
};

class FvZoneEnvironmentTerrainCollider : public FvZoneEnvironmentCollider
{
public:
	
	FvZoneEnvironmentTerrainCollider() { }

	virtual float Ground( const FvVector3 &kPos );
};

class FvClosestTerrainObstacle : public FvCollisionCallback
{
public:
	FvClosestTerrainObstacle();

	virtual int operator() (const FvZoneObstacle &kObstacle,
		const FvWorldTriangle &kTriangle, float fDist );

	void Reset();

	float Dist() const;

	bool Collided() const;

private:
	float m_fDist;
	bool m_bCollided;
};
#endif // FV_SERVER

class FV_ZONE_API FvCollisionAdvance : public FvCollisionCallback
{
public:
	FvCollisionAdvance( const FvVector3 &kOrigin,
		const FvVector3 &kAxis1, const FvVector3 &kAxis2,
		const FvVector3 &kDirection,
		float fMaxAdvance );
	~FvCollisionAdvance();

	float GetAdvance() const { return max( 0.f, m_fAdvance); }

	const FvWorldTriangle &HitTriangle() const	{ return m_kHitTriangle; }
	FvZoneItemPtr HitItem() const				{ return m_spHitItem; }

	void IgnoreFlags( FvUInt8 uiFlags )				{ m_uiIgnoreFlags = uiFlags; }

private:
	virtual int operator()( const FvZoneObstacle &kObstacle,
		const FvWorldTriangle &kTriangle, float fDist );

	FvPlane m_kPlaneEq0;
	FvPlane m_kPlaneEq1;
	FvPlane m_kPlaneEq2;
	FvPlane m_kPlaneEq3;
	FvPlane m_kPlaneEqBase;
	const FvVector3 m_kDir;
	float m_fAdvance;
	float m_fAdjust;

	FvWorldTriangle	m_kHitTriangle;
	FvZoneItemPtr m_spHitItem;

	FvUInt8 m_uiIgnoreFlags;
};

#include "FvZoneObstacle.inl"

#endif // __FvZoneObstacle_H__