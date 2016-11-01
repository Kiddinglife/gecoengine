#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNavigator", 0 )

#include "FvNavigator.h"
#include "FvZoneWaypointSet.h"
//#include "common/chunk_portal.hpp"
#include "FvAStar.h"
#include <FvZoneSpace.h>
#include <FvZone.h>
#include <sstream>

class FvZoneWPSetState
{
public:
	FvZoneWPSetState();
	FvZoneWPSetState( FvZoneWaypointSetPtr spSet );
	FvZoneWPSetState( const FvNavLoc &kLoc );

	typedef FvZoneWaypointConns::const_iterator adjacency_iterator;

	int Compare( const FvZoneWPSetState &kOther ) const
		{ return FvIntPtr(&*m_spSet) - FvIntPtr(&*kOther.m_spSet); }

	FvString Desc() const
	{
		std::stringstream ss;
		ss << '(' << m_kPosition.x << ", " <<
			m_kPosition.y << ", " <<
			m_kPosition.z << ") at " <<
			m_spSet->Zone()->Identifier();
		return ss.str();
	}

	unsigned int Hash() const
	{
		return (FvUIntPtr)(&*m_spSet);
	}

	bool IsGoal( const FvZoneWPSetState &kGoal ) const
		{ return m_spSet == kGoal.m_spSet; }

	adjacency_iterator AdjacenciesBegin() const
		{ return m_spSet->ConnectionsBegin(); }
	adjacency_iterator AdjacenciesEnd() const
		{ return m_spSet->ConnectionsEnd(); }

	bool GetAdjacency( adjacency_iterator kIter, FvZoneWPSetState &kNeigh,
		const FvZoneWPSetState &kGoal ) const;

	float DistanceFromParent() const
		{ return m_fDistanceFromParent; }

	float DistanceToGoal( const FvZoneWPSetState &kGoal ) const
		{ return ( m_kPosition - kGoal.m_kPosition ).Length(); }

	FvZoneWaypointSetPtr Set() const
		{ return m_spSet; }

	void PassedActivatedPortal( const bool bA )
		{ m_bPassedActivatedPortal = bA; }

	bool PassedActivatedPortal() const
		{ return m_bPassedActivatedPortal; }

	void PassedShellBoundary( bool a )
		{ m_bPassedShellBoundary = a; }

	bool PassedShellBoundary() const
		{ return m_bPassedShellBoundary; }

	const FvVector3 &Position() const	{ return m_kPosition; }
	static bool ms_bBlockNonPermissive;

private:
	FvZoneWaypointSetPtr m_spSet;
	float m_fDistanceFromParent;
	bool m_bPassedActivatedPortal;
	bool m_bPassedShellBoundary;
	FvVector3 m_kPosition;
};

bool FvZoneWPSetState::ms_bBlockNonPermissive = true;

FvZoneWPSetState::FvZoneWPSetState() :
	m_fDistanceFromParent( 0.f )
{
}

FvZoneWPSetState::FvZoneWPSetState( FvZoneWaypointSetPtr spSet ) :
	m_spSet( spSet ),
	m_fDistanceFromParent( 0.f ),
	m_kPosition( spSet->Zone()->Centre() )
{
}

FvZoneWPSetState::FvZoneWPSetState( const FvNavLoc &kLoc ) :
	m_spSet( kLoc.Set() ),
	m_fDistanceFromParent( 0.f ),
	m_kPosition( kLoc.Point() )
{
}

namespace
{
	bool IsActivated( FvZoneBoundary::Portal *pkPortal, FvZone *pkFromZone )
	{
		//ChunkPortal * pCP = NULL;
		//ChunkPyCache::NamedPyObjects os = ChunkPyCache::instance( *pkFromZone ).objects();
		//ChunkPyCache::NamedPyObjects::iterator i = os.begin();
		//for ( ; i != os.end(); i++ )
		//{
		//	if ( ChunkPortal::Check( &(*i->second) ) )
		//	{
		//		ChunkPortal * p = (ChunkPortal*)&*i->second;
		//		if ( p->pPortal() == pkPortal )
		//		{
		//			pCP = p;
		//			break;
		//		}
		//	}
		//}

		//if ( pCP && pCP->Activated() )
		//	return true;

		return false;
	}
}

bool FvZoneWPSetState::GetAdjacency( FvZoneWaypointConns::const_iterator kIter,
		FvZoneWPSetState &kNeigh,
		const FvZoneWPSetState &kGoal ) const
{
	FvZoneWaypointSetPtr pDestWaypointSet = kIter->first;
	FvZoneBoundary::Portal * pPortal = kIter->second;

	FvZone *pkFromZone = m_spSet->Zone();
	FvZone *pkToZone = pPortal->m_pkZone;

	if ( pPortal != NULL && !pPortal->m_bPermissive )
		if ( FvZoneWPSetState::ms_bBlockNonPermissive )
			return false;

	FvZoneBoundary::Portal * pBackPortal = NULL;
	FvZone::piterator p = pkToZone->PBegin();
	for ( ; p != pkToZone->PEnd(); p++ )
	{
		if ( p->m_pkZone == pkFromZone )
		{
			pBackPortal = &(*p);
			break;
		}
	}
	if ( pBackPortal == NULL )
	{
		FV_WARNING_MSG( "FvZoneWPSetState::GetAdjacency: "
			"Encountered one way portal connection, assuming non passable.\n" );
		return false;
	}

	kNeigh.PassedShellBoundary(
		pkFromZone != pkToZone &&
		( !pkFromZone->IsOutsideZone() ||
		!pkToZone->IsOutsideZone() ) );

	if( kNeigh.PassedShellBoundary() )
	{
		kNeigh.PassedActivatedPortal( IsActivated( pPortal, pkFromZone ) |
			IsActivated( pBackPortal, pkToZone ) );
	}
	else
	{
		kNeigh.PassedActivatedPortal( false );
	}

	kNeigh.m_spSet = pDestWaypointSet;

	if ( !kNeigh.m_spSet->Zone() )
	{
		FV_WARNING_MSG( "FvZoneWPSetState::GetAdjacency: "
			"FvZone associated with neighbouring Waypoint Set no longer exists.\n" );
		return false;
	}

	FvZonePtr nec = kNeigh.m_spSet->Zone();
	FvBoundingBox bb = nec->LocalBB();
	FvVector3 start = nec->TransformInverse().ApplyPoint( m_kPosition );
	FvVector3 end = nec->TransformInverse().ApplyPoint( kGoal.m_kPosition );

	if (bb.Clip( start, end ))
	{
		kNeigh.m_kPosition = nec->Transform().ApplyPoint( start );
	}
	else
	{
		FvVector2 dir( end.x - start.x, end.y - start.y );

		FvVector2 min( bb.MaxBounds().x, bb.MaxBounds().y );

		float minDistSquared = FLT_MAX;

		for (int i = 0; i < 4; ++i)
		{
			FvVector2 p;
			p.x = ( i & 1 ) ? bb.MinBounds().x : bb.MaxBounds().x;
			p.y = ( i & 2 ) ? bb.MinBounds().y : bb.MaxBounds().y;

			FvVector2 minVec( p.x - start.x, p.y - start.y );
			float distSquared = minVec.CrossProduct( dir );
			distSquared = distSquared * distSquared;

			if (distSquared < minDistSquared)
			{
				min = p;
				minDistSquared = distSquared;
			}
		}
		kNeigh.m_kPosition = nec->Transform().ApplyPoint( FvVector3( min.x, min.y, start.z ) );
	}

	kNeigh.m_fDistanceFromParent = this->DistanceToGoal( kNeigh );
	return true;
}

class FvZoneWaypointState
{
public:
	typedef int adjacency_iterator;

	FvZoneWaypointState();
	FvZoneWaypointState( FvZoneWaypointSetPtr kDstSet, const FvVector3 &kDstPoint,
		   FvZoneWaypointSetPtr spSrcSet );
	FvZoneWaypointState( const FvNavLoc &kNavLoc );

	int Compare( const FvZoneWaypointState &kOther ) const
		{ return (m_kNavLoc.Set() == kOther.m_kNavLoc.Set()) ?
			m_kNavLoc.Waypoint() - kOther.m_kNavLoc.Waypoint() :
			FvIntPtr(&*m_kNavLoc.Set()) - FvIntPtr(&*kOther.m_kNavLoc.Set()); }

	FvString Desc() const
	{
		if (!m_kNavLoc.Set())
		{
			return "no Set";
		}

		if (!m_kNavLoc.Set()->Zone())
		{
			return "no zone";
		}

		FvVector3 v = m_kNavLoc.Point();
		std::stringstream ss;
		ss << m_kNavLoc.Waypoint() << " (" << v.x << ' ' << v.y << ' ' << v.z << ')' <<
			" in " << m_kNavLoc.Set()->Zone()->Identifier();
		return ss.str();
	}

	unsigned int Hash() const
	{
		return (FvUIntPtr)(&*m_kNavLoc.Set()) + m_kNavLoc.Waypoint();
	}

	bool IsGoal( const FvZoneWaypointState &kGoal ) const
		{ return m_kNavLoc.Set() == kGoal.m_kNavLoc.Set() &&
			m_kNavLoc.Waypoint() == kGoal.m_kNavLoc.Waypoint(); }

	adjacency_iterator AdjacenciesBegin() const
		{ return 0; }
	adjacency_iterator AdjacenciesEnd() const
		{ return m_kNavLoc.Waypoint() >= 0 ?
			m_kNavLoc.Set()->Waypoint( m_kNavLoc.Waypoint() ).m_kEdges.size() : 0; }

	bool GetAdjacency( adjacency_iterator kIter, FvZoneWaypointState &kNeigh,
		const FvZoneWaypointState &kGoal ) const;

	float DistanceFromParent() const
		{ return m_fDistanceFromParent; }

	float DistanceToGoal( const FvZoneWaypointState &kGoal ) const
		{ return (m_kNavLoc.Point() - kGoal.m_kNavLoc.Point()).Length(); }

	const FvNavLoc &NavLoc() const
		{ return m_kNavLoc; }

private:
	FvNavLoc m_kNavLoc;
	float m_fDistanceFromParent;
};

FvZoneWaypointState::FvZoneWaypointState() :
	m_fDistanceFromParent( 0.f )
{
}

FvZoneWaypointState::FvZoneWaypointState( FvZoneWaypointSetPtr spDstSet,
		const FvVector3 &kDstPoint,
		FvZoneWaypointSetPtr spSrcSet ) :
	m_fDistanceFromParent( 0.f )
{
	m_kNavLoc.m_spSet = spDstSet;
	m_kNavLoc.m_iWaypoint = -1;

	m_kNavLoc.m_kPoint = kDstPoint;

	m_kNavLoc.Clip();
}

FvZoneWaypointState::FvZoneWaypointState( const FvNavLoc &kNavLoc ) :
	m_kNavLoc( kNavLoc ),
	m_fDistanceFromParent( 0.f )
{
}

bool FvZoneWaypointState::GetAdjacency( int iIndex, FvZoneWaypointState &kNeigh,
	const FvZoneWaypointState &kGoal ) const
{
	const FvZoneWaypoint & cw = m_kNavLoc.Set()->Waypoint( m_kNavLoc.Waypoint() );
	const FvZoneWaypoint::Edge & cwe = cw.m_kEdges[ iIndex ];

	int waypoint = cwe.NeighbouringWaypoint();
	bool bWaypointAdjToZone = cwe.AdjacentToZone();
	if (waypoint >= 0)
	{
		kNeigh.m_kNavLoc.m_spSet = m_kNavLoc.Set();
		kNeigh.m_kNavLoc.m_iWaypoint = waypoint;
	}
	else if (bWaypointAdjToZone)
	{
		kNeigh.m_kNavLoc.m_spSet =
			m_kNavLoc.Set()->ConnectionWaypoint( cwe );
		kNeigh.m_kNavLoc.m_iWaypoint = -1;
	}
	else
	{
		return false;
	}

	FvVector2 src( m_kNavLoc.Point().x, m_kNavLoc.Point().y );
	FvVector2 dst( kGoal.m_kNavLoc.Point().x, kGoal.m_kNavLoc.Point().y );
	FvVector2 way;
	FvVector2 del = dst - src;
	FvVector2 p1 = cwe.m_kStart;
	FvVector2 p2 = cw.m_kEdges[ (iIndex+1) % cw.m_kEdges.size() ].m_kStart;

	float cp1 = del.CrossProduct( p1 - src );
	float cp2 = del.CrossProduct( p2 - src );
	if (cp1 > 0.f && cp2 < 0.f)
	{
		way = p1 + (cp1/(cp1-cp2)) * (p2-p1);
	}
	else
	{
		way = (fabs(cp1) < fabs(cp2)) ? p1 : p2;
	}

	if (kNeigh.m_kNavLoc.m_iWaypoint == -1)
	{
		del.Normalise();
		way += del * 0.2f;
	}

	kNeigh.m_kNavLoc.m_kPoint.Set( way.x, way.y, cw.m_fMaxHeight );

	if (bWaypointAdjToZone)
	{
		if (kNeigh.m_kNavLoc.Set() && kNeigh.m_kNavLoc.Set()->Zone())
		{
			FvBoundingBox bb = kNeigh.m_kNavLoc.Set()->Zone()->BoundingBox();
			static const float inABit = 0.01f;
			kNeigh.m_kNavLoc.m_kPoint.x = FvClampEx( bb.MinBounds().x + inABit,
				kNeigh.m_kNavLoc.m_kPoint.x, bb.MaxBounds().x - inABit );
			kNeigh.m_kNavLoc.m_kPoint.y = FvClampEx( bb.MinBounds().y + inABit,
				kNeigh.m_kNavLoc.m_kPoint.y, bb.MaxBounds().y - inABit );
		}
		else
		{
			return false;
		}
	}

	kNeigh.m_kNavLoc.Clip();
	kNeigh.m_fDistanceFromParent =
		(kNeigh.m_kNavLoc.Point() - m_kNavLoc.Point()).Length();

	return true;
}

class FvNavigatorCache : public FvReferenceCount
{
public:
	const FvZoneWaypointState *SaveWayPath( AStar<FvZoneWaypointState> &kAStar );

	const FvZoneWaypointState *FindWayPath(
		const FvZoneWaypointState &kSrc, const FvZoneWaypointState &kDst );

	const FvZoneWPSetState *SaveWaySetPath( AStar<FvZoneWPSetState> &kAStar );

	const FvZoneWPSetState *FindWaySetPath(
		const FvZoneWPSetState &kSrc, const FvZoneWPSetState &kDst );

	int GetWaySetPathSize() const;

	void ClearWPSetCache()
		{ m_kWaySetPath.clear(); }

	void ClearWPCache()
		{ m_kWayPath.clear(); }

	const std::vector<FvZoneWaypointState> &WayPath() const
		{ return m_kWayPath; }

	void PassedShellBoundary( bool bA )
		{ m_bPassedShellBoundary = bA; }

	bool PassedShellBoundary() const
		{ return m_bPassedShellBoundary; }
private:
	std::vector<FvZoneWaypointState> m_kWayPath;
	std::vector<FvZoneWPSetState> m_kWaySetPath;
	bool m_bPassedShellBoundary;
};

const FvZoneWaypointState * FvNavigatorCache::SaveWayPath(
	AStar<FvZoneWaypointState> &kAStar )
{
	std::vector<const FvZoneWaypointState *> fwdPath;

	const FvZoneWaypointState * as = kAStar.first();
	bool first = true;
	const FvZoneWaypointState* last = as;

	while (as != NULL)
	{
		if (!FvAlmostZero( as->DistanceFromParent() ) || first)
		{
			fwdPath.push_back( as );
			first = false;
		}

		as = kAStar.next();

		if (as)
		{
			last = as;
		}
	}

	if (fwdPath.size() < 2)
	{
		fwdPath.push_back( last );
	}

	m_kWayPath.clear();
	for (int i = fwdPath.size()-1; i >= 0; i--)
	{
		m_kWayPath.push_back( *fwdPath[i] );
	}

	FV_ASSERT_DEBUG( m_kWayPath.size() >= 2 );
	return &m_kWayPath[m_kWayPath.size()-2];
}

const FvZoneWaypointState * FvNavigatorCache::FindWayPath(
	const FvZoneWaypointState &kSrc, const FvZoneWaypointState &kDst )
{
	if (m_kWayPath.size() < 2) return NULL;

	if (kDst.NavLoc().Set() != m_kWayPath.front().NavLoc().Set() ||
		kDst.NavLoc().Waypoint() != m_kWayPath.front().NavLoc().Waypoint())
			return NULL;

	if (kSrc.NavLoc().Set() == m_kWayPath.back().NavLoc().Set() &&
		kSrc.NavLoc().Waypoint() == m_kWayPath.back().NavLoc().Waypoint())
	{
		return &m_kWayPath[m_kWayPath.size()-2];
	}

	m_kWayPath.pop_back();

	if (m_kWayPath.size() < 2) return NULL;

	if (kSrc.NavLoc().Set() == m_kWayPath.back().NavLoc().Set() &&
		kSrc.NavLoc().Waypoint() == m_kWayPath.back().NavLoc().Waypoint())
	{
		return &m_kWayPath[m_kWayPath.size()-2];
	}

	m_kWayPath.clear();
	return NULL;
}

const FvZoneWPSetState * FvNavigatorCache::SaveWaySetPath(
	AStar<FvZoneWPSetState> &kAStar )
{
	std::vector<const FvZoneWPSetState *> fwdPath;

	m_bPassedShellBoundary = false;
	const FvZoneWPSetState * as = kAStar.first();
	while (as != NULL)
	{
		m_bPassedShellBoundary = m_bPassedShellBoundary || as->PassedShellBoundary();
		fwdPath.push_back( as );
		as = kAStar.next();
	}

	m_kWaySetPath.clear();
	for (int i = fwdPath.size()-1; i >= 0; i--)
	{
		m_kWaySetPath.push_back( *fwdPath[i] );
	}

	FV_ASSERT_DEBUG( m_kWaySetPath.size() >= 2 );
	return &m_kWaySetPath[m_kWaySetPath.size()-2];
}

const FvZoneWPSetState * FvNavigatorCache::FindWaySetPath(
	const FvZoneWPSetState &kSrc, const FvZoneWPSetState &kDst )
{
	if( PassedShellBoundary() )
	{
		m_kWaySetPath.clear();
		return NULL;
	}

	if (m_kWaySetPath.size() < 2) return NULL;

	if (kDst.Set() != m_kWaySetPath.front().Set())
		return NULL;

	if (kSrc.Set() == m_kWaySetPath.back().Set())
	{
		return &m_kWaySetPath[m_kWaySetPath.size()-2];
	}

	m_kWaySetPath.pop_back();

	if (m_kWaySetPath.size() < 2) return NULL;

	if (kSrc.Set() == m_kWaySetPath.back().Set())
	{
		return &m_kWaySetPath[m_kWaySetPath.size()-2];
	}

	m_kWaySetPath.clear();
	return NULL;
}

int FvNavigatorCache::GetWaySetPathSize() const
{
	return m_kWaySetPath.size();
}

FvNavLoc::FvNavLoc()
{
}

FvNavLoc::FvNavLoc( FvZoneSpace *pkSpace, const FvVector3 &kPoint, float fGirth )
{
	FvVector3 p( kPoint );
	p.z += 0.01f;
	FvZone * pkZone = pkSpace->FindZoneFromPoint( p );
	if (pkZone != NULL)
	{
		m_kPoint = kPoint;
		FvZoneNavigator::Result res;
		if (FvZoneNavigator::ms_kInstance( *pkZone ).Find( m_kPoint, fGirth, res ))
		{
			m_spSet = res.m_spSet;
			m_iWaypoint = res.m_iWaypoint;
		}
	}
}

FvNavLoc::FvNavLoc( FvZone *pkZone, const FvVector3 &kPoint, float fGirth ) :
	m_kPoint( kPoint )
{
	FV_ASSERT_DEBUG( pkZone != NULL );

	FvZoneNavigator::Result res;
	if (FvZoneNavigator::ms_kInstance( *pkZone ).Find( m_kPoint, fGirth, res ))
	{
		m_spSet = res.m_spSet;
		m_iWaypoint = res.m_iWaypoint;
	}
}

FvNavLoc::FvNavLoc( const FvNavLoc &kGuess, const FvVector3 &kPoint )
	: m_kPoint( kPoint )
{
	FV_ASSERT_DEBUG( kGuess.Valid() );

	FvZoneNavigator::Result res;

	if (kGuess.Waypoint() == -1)
		m_iWaypoint = kGuess.Set()->Find( m_kPoint );
	else if ((m_kPoint - kGuess.Point()).LengthSquared() < 0.00001f)
		m_iWaypoint = kGuess.Waypoint();
	else if (kGuess.Set()->Waypoint( kGuess.Waypoint() ).Contains( m_kPoint ))
		m_iWaypoint = kGuess.Waypoint();
	else
		m_iWaypoint = kGuess.Set()->Find( m_kPoint );
	m_spSet = kGuess.Set();

	if (m_iWaypoint < 0)
		*this = FvNavLoc( m_spSet->Zone()->Space(), kPoint, m_spSet->Girth() );
}

FvNavLoc::~FvNavLoc()
{
}

bool FvNavLoc::IsWithinWP() const
{
	if ( m_spSet && m_iWaypoint >= 0 )
		return m_spSet->Waypoint( m_iWaypoint ).Contains( m_kPoint );
	else
		return 0;
}

void FvNavLoc::Clip()
{
	Clip( m_kPoint );
}

void FvNavLoc::Clip( FvVector3 &kPoint ) const
{
	if ( m_spSet && m_iWaypoint >= 0 )
		m_spSet->Waypoint( m_iWaypoint ).Clip( m_spSet->Zone(), kPoint );
}

FvString FvNavLoc::Desc() const
{
	std::stringstream ss;

	if (m_spSet && m_spSet->Zone())
	{
		ss << m_spSet->Zone()->Identifier() << ':' << Waypoint() << ' ';

		ss << m_kPoint;

		if (m_iWaypoint != -1)
		{
			ss << " - ";

			FvZoneWaypoint& wp = m_spSet->Waypoint( m_iWaypoint );

			for (int i = 0; i < wp.m_uiEdgeCount; ++i)
			{
				ss << '(' << wp.m_kEdges[ i ].m_kStart.x << ", "
					<< wp.m_kEdges[ i ].m_kStart.y << ')';

				if (i != wp.m_uiEdgeCount - 1)
				{
					ss << ", ";
				}
			}
		}
	}
	else
	{
		ss << m_kPoint;
	}

	return ss.str();
}

FvNavigator::FvNavigator()
	: m_spCache(NULL)
{
}

FvNavigator::~FvNavigator()
{
}

void FvNavigator::ClearWPSetCache()
{
	if ( m_spCache )
	{
		m_spCache->ClearWPSetCache();
	}
}

void FvNavigator::ClearWPCache()
{
	if (m_spCache)
	{
		m_spCache->ClearWPCache();
	}
}

bool FvNavigator::FindPath( const FvNavLoc &kSrc, const FvNavLoc &kDst, float fMaxDistance,
	FvNavLoc &kWay, bool bBlockNonPermissive, bool &bPassedActivatedPortal )
{
	float maxDistanceInSet = fMaxDistance > FV_GRID_RESOLUTION ? -1.f : fMaxDistance;

	bPassedActivatedPortal = false;

	this->m_bInfiniteLoopProblem = false;

	FV_ASSERT_DEBUG( kSrc.Valid() && kDst.Valid() );

	if (!m_spCache) m_spCache = new FvNavigatorCache();

	if (kSrc.Set() == kDst.Set())
	{
		FvZoneWaypointState srcState( kSrc );
		FvZoneWaypointState dstState( kDst );

		const FvZoneWaypointState * pWayState =
			m_spCache->FindWayPath( srcState, dstState );
		if (pWayState == NULL)
		{
			static AStar<FvZoneWaypointState> astar;
			astar.reset();
			if (astar.search( srcState, dstState, maxDistanceInSet ))
			{
				pWayState = m_spCache->SaveWayPath( astar );
			}
			if ( astar.infiniteLoopProblem )
			{
				FV_ERROR_MSG( "FvNavigator::FindPath: Infinite Loop problem "
					"from waypoint %s to %s\n",
					kSrc.Desc().c_str(), kDst.Desc().c_str() );
				this->m_bInfiniteLoopProblem = true;
			}
		}

		if (pWayState != NULL)
		{
			kWay = pWayState->NavLoc();
			return true;
		}
	}
	else
	{
		FvZoneWPSetState srcSetState( kSrc );
		FvZoneWPSetState dstSetState( kDst );

		const FvZoneWPSetState * pWaySetState =
			m_spCache->FindWaySetPath( srcSetState, dstSetState );

		if (pWaySetState == NULL)
		{
			FvZoneWPSetState::ms_bBlockNonPermissive = bBlockNonPermissive;
			static AStar<FvZoneWPSetState> astarSet;
			astarSet.reset();
			if (astarSet.search( srcSetState, dstSetState, fMaxDistance ))
			{
				pWaySetState = m_spCache->SaveWaySetPath( astarSet );
			}
			if ( astarSet.infiniteLoopProblem )
			{
				FV_ERROR_MSG( "FvNavigator::FindPath: Infinite Loop problem "
					"from waypoint %d to %d\n", kSrc.Waypoint(), kDst.Waypoint() );
				this->m_bInfiniteLoopProblem = true;
			}
		}
		if (pWaySetState != NULL)
		{
			FvZoneWaypointState srcState( kSrc );
			FvZoneWaypointState dstState( pWaySetState->Set(), kDst.Point(),
				kSrc.Set() );
			const FvZoneWaypointState * pWayState =
				m_spCache->FindWayPath( srcState, dstState );
			if (pWayState == NULL)
			{
				static AStar<FvZoneWaypointState> astar;
				astar.reset();
				if (astar.search( srcState, dstState, maxDistanceInSet ))
				{
					pWayState = m_spCache->SaveWayPath( astar );
				}
				if ( astar.infiniteLoopProblem )
				{
					FV_ERROR_MSG( "FvNavigator::FindPath: Infinite Loop problem "
						"from waypoint %d to %d\n", kSrc.Waypoint(), kDst.Waypoint() );
					this->m_bInfiniteLoopProblem = true;
				}

			}
			if (pWayState != NULL)
			{
				kWay = pWayState->NavLoc();
				if ( (kWay.Set() != kSrc.Set()) && pWaySetState->PassedActivatedPortal() )
				{
					bPassedActivatedPortal = true;
				}
				return true;
			}
		}
	}

	return false;
}

struct FvTempWayRef
{
	FvTempWayRef() {}
	FvTempWayRef( const FvNavLoc &kNL ) :
		m_pkSet( &*kNL.Set() ),
		m_iWaypoint( kNL.Waypoint() )
	{}
	FvTempWayRef( FvZoneWaypointSet *pkSet, int iWaypoint ) :
		m_pkSet( pkSet ),
		m_iWaypoint( iWaypoint )
	{}

	FvZoneWaypointSet *m_pkSet;
	int m_iWaypoint;
};

bool FvNavigator::FindSituationAhead( FvUInt32 uiSituation, const FvNavLoc &kSrc,
	float fRadius, const FvVector3 &kTGT, FvVector3 &kDst )
{
	FV_ASSERT( 0 && "deprecated and haven't been converted" );
	FV_ASSERT( kSrc.Valid() );

	const float sweepCrossDist = 30.f;
	float radiusSquared = fRadius * fRadius;
	float bestDistSquared = -1;
	FvZoneWaypoint::Edges::const_iterator eit;
	int neigh;

	FvZoneWaypointSet * srcLPSet = &*kSrc.Set();
	FvVector3 srcLPoint;
	FvVector3 srcWPoint = kSrc.Point();
	FvVector3 tgtLPoint = srcLPSet->Zone()->TransformInverse().ApplyPoint( kTGT );

	FvUInt16 mark = FvZoneWaypoint::ms_uiNextMark++;
	kSrc.Set()->Waypoint( kSrc.Waypoint() ).m_uiMark = mark;

	static FvVectorNoDestructor<FvTempWayRef> stack;
	stack.clear();
	stack.push_back( FvTempWayRef( kSrc ) );
	while (!stack.empty())
	{
		FvTempWayRef tway = stack.back();
		stack.pop_back();

		if (tway.m_pkSet != srcLPSet)
		{
			srcLPSet = tway.m_pkSet;
			srcLPoint = srcLPSet->Zone()->TransformInverse().
				ApplyPoint( srcWPoint );
			tgtLPoint = srcLPSet->Zone()->TransformInverse().
				ApplyPoint( kTGT );
		}

		const FvZoneWaypoint & waypoint = tway.m_pkSet->Waypoint( tway.m_iWaypoint );
		for (eit = waypoint.m_kEdges.begin(); eit != waypoint.m_kEdges.end(); eit++)
		{
			if ((neigh = eit->NeighbouringWaypoint()) >= 0)
			{
				const FvZoneWaypoint & nwp = tway.m_pkSet->Waypoint( neigh );
				if (nwp.m_uiMark == mark) continue;
				nwp.m_uiMark = mark;
				if (nwp.DistanceSquared( tway.m_pkSet->Zone(), srcLPoint ) < radiusSquared)
					stack.push_back( FvTempWayRef( tway.m_pkSet, neigh ) );
			}
			else if (eit->AdjacentToZone())
			{
				FvZoneWaypointSetPtr pOtherSet =
					tway.m_pkSet->ConnectionWaypoint( *eit );
				if (!pOtherSet)
				{
					FV_WARNING_MSG( "FvNavigator::FindSituationAhead: "
						"no Waypoint set for edge adjacent to zone\n" );
					continue;
				}
				if (!pOtherSet->Zone())
				{
					FV_WARNING_MSG( "FvNavigator::FindSituationAhead: "
						"adjacent Waypoint set has no zone\n" );
					continue;
				}
				FvVector3 otherLPoint = pOtherSet->Zone()->TransformInverse().
					ApplyPoint( srcWPoint );
				int otherWP = pOtherSet->Find( otherLPoint );
				if (otherWP < 0) continue;

				const FvZoneWaypoint & nwp = pOtherSet->Waypoint( otherWP );
				if (nwp.m_uiMark == mark) continue;
				nwp.m_uiMark = mark;
				if (nwp.DistanceSquared( pOtherSet->Zone(), otherLPoint ) < radiusSquared)
				{
					stack.push_back( FvTempWayRef( pOtherSet.GetObject(),
						otherWP ) );
				}
			}
			else
			{
				FvUInt32 vista = eit->NeighbouringVista();

				const FvVector2 & p1 = eit->m_kStart;
				const FvVector2 & p2 = ((eit+1 != waypoint.m_kEdges.end()) ?
					*(eit+1) : waypoint.m_kEdges.front()).m_kStart;

				FvLine eline( p1, p2 );
				const FvVector2 tgtFlat( tgtLPoint.x, tgtLPoint.y );
				if (!eline.IsInFrontOf( tgtFlat )) continue;

				for (int w = 0; w < 3; w++)
				{
					if ( ((vista >> (w<<2)) & 0xF) != uiSituation) continue;

					float t;

					const FvVector2 & normal = eline.Normal();
					FvVector2 ortho(p2.x-p1.x, p2.y-p1.y);
					ortho.Normalise();
					ortho = ortho * 0.5f;
					FvVector2 dir( normal.y, -normal.x );
					if (w == 0)			// front
					{
						FvLine lline( p2, p2 + normal + ortho );
						if (!lline.IsInFrontOf( tgtFlat )) continue;
						FvLine rline( p1 + normal - ortho, p1 );
						if (!rline.IsInFrontOf( tgtFlat )) continue;
						t = 0.5f;
					}
					else if (w == 1)	// left
					{
						FvLine lline( p2, p2 + normal + dir );
						if (!lline.IsInFrontOf( tgtFlat )) continue;
						FvLine rline( p2 + normal*sweepCrossDist, (p1+p2)*0.5 );
						if (!rline.IsInFrontOf( tgtFlat )) continue;
						t = 0.25f;
					}
					else				// right
					{
						FvLine lline( p1 + normal + dir, p1 );
						if (!lline.IsInFrontOf( tgtFlat )) continue;
						FvLine rline( (p1+p2)*0.5, p1 + normal*sweepCrossDist );
						if (!rline.IsInFrontOf( tgtFlat )) continue;
						t = 0.75f;
					}

					FvVector2 candPoint = p1 * (1.f-t) + p2 * t - normal * 0.25f;

					float candDistSquared = (candPoint - FvVector2(
						srcLPoint.x, srcLPoint.y )).LengthSquared();
					if (candDistSquared < bestDistSquared ||
						bestDistSquared < 0.f)
					{
						bestDistSquared = candDistSquared;
						kDst = srcLPSet->Zone()->Transform().ApplyPoint(
							FvVector3( candPoint.x, candPoint.y, waypoint.m_fMaxHeight ) );
					}
				}
			}
		}
	}


	return bestDistSquared >= 0.f;
}

int FvNavigator::GetWaySetPathSize() const
{
	if (m_spCache)
		return m_spCache->GetWaySetPathSize();
	else
		return 0;
}

void FvNavigator::GetWaypointPath( const FvNavLoc &kSrcLoc,std::vector<FvVector3> &kWPPath )
{
	kWPPath.clear();

	if (m_spCache)
	{
		const std::vector<FvZoneWaypointState> &kPath = m_spCache->WayPath();
		std::vector<FvZoneWaypointState>::const_reverse_iterator kIter =
			kPath.rbegin();

		while (kIter != kPath.rend())
		{
			kWPPath.push_back( kIter->NavLoc().Point() );
			++kIter;
		}
	}
}
