#include "FvWaypointZone.h"

#include "FvWaypointSet.h"

#include <float.h>

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNavigator", 0 )

FvWaypointZone::FvWaypointZone(FvZoneID kZoneID) :
	m_kZoneID( kZoneID ),
	m_kCentre( 0.f, 0.f, 0.f ),
	m_fVolume( 0.f )
{
}

FvWaypointZone::~FvWaypointZone()
{
	this->Clear();

	for (WaypointSets::iterator waypointIter = m_kWaypointSets.begin();
		waypointIter != m_kWaypointSets.end();
		waypointIter++)
	{
		if (*waypointIter != NULL)
			delete *waypointIter;
	}
	m_kWaypointSets.clear();
}

void FvWaypointZone::AddWaypoint(int iSet, FvWaypoint *pkWaypoint)
{
	if (m_kWaypointSets.size() < unsigned int(iSet+1))
		m_kWaypointSets.resize(iSet+1);
	if (m_kWaypointSets[iSet] == NULL)
		m_kWaypointSets[iSet] = new FvWaypointSet( *this, iSet );
	m_kWaypointSets[iSet]->AddWaypoint( pkWaypoint );
}

void FvWaypointZone::AddAdjacency(FvWaypointZone *pkAdjacentZone)
{
	if (std::find( m_kAdjacentZones.begin(), m_kAdjacentZones.end(),
		pkAdjacentZone ) == m_kAdjacentZones.end())
	{
		m_kAdjacentZones.push_back( pkAdjacentZone );

		for (WaypointSets::iterator wit = m_kWaypointSets.begin();
			wit != m_kWaypointSets.end();
			wit++)
		{
			if (*wit != NULL)
				(*wit)->Bind( *pkAdjacentZone );
		}
	}
}

void FvWaypointZone::DelAdjacency(FvWaypointZone *pkAdjacentZone)
{
	WaypointZones::iterator found = std::find(
		m_kAdjacentZones.begin(), m_kAdjacentZones.end(), pkAdjacentZone );
	if (found != m_kAdjacentZones.end())
	{
		m_kAdjacentZones.erase( found );

		for (WaypointSets::iterator wit = m_kWaypointSets.begin();
			wit != m_kWaypointSets.end();
			wit++)
		{
			if (*wit != NULL)
				(*wit)->Loose( *pkAdjacentZone );
		}
	}
}

void FvWaypointZone::LinkWaypoints()
{
	for (WaypointSets::iterator wit = m_kWaypointSets.begin();
		wit != m_kWaypointSets.end();
		wit++)
	{
		if (*wit != NULL)
			(*wit)->LinkWaypoints();
	}
}

void FvWaypointZone::Clear()
{
	WaypointZones::iterator adjacencyIter;
	WaypointSets::iterator waypointIter;

	for(adjacencyIter = m_kAdjacentZones.begin();
		adjacencyIter != m_kAdjacentZones.end();
		adjacencyIter++)
	{
		(*adjacencyIter)->DelAdjacency( this );
	}
	m_kAdjacentZones.clear();

	for(waypointIter = m_kWaypointSets.begin();
		waypointIter != m_kWaypointSets.end();
		waypointIter++)
	{
		if (*waypointIter != NULL)
			(*waypointIter)->Clear();
	}
}

FvWaypointSet * FvWaypointZone::FindWaypointSet( unsigned int uiSet )
{
	if (uiSet >= m_kWaypointSets.size()) return NULL;
	return m_kWaypointSets[uiSet];
}

bool FvWaypointZone::FindWaypoint(FvWaypointID uiWaypointID, FvWPSpec &kWPSpec)
{
	WaypointSets::iterator wit;
	FvWaypointSet * pWPSet;
	FvWaypoint * pWaypoint;

	for (wit = m_kWaypointSets.begin(); wit != m_kWaypointSets.end(); wit++)
	{
		pWPSet = *wit;
		if (pWPSet == NULL) continue;

		pWaypoint = pWPSet->FindWaypoint( uiWaypointID );
		if (pWaypoint != NULL)
		{
			kWPSpec.pWPSet = pWPSet;
			kWPSpec.pWaypoint = pWaypoint;
			return true;
		}
	}

	return false;
}

bool FvWaypointZone::FindEnclosingWaypoint( const FvVector3 &kPosition,
	FvWPSpec &kWPSpec )
{
	WaypointSets::iterator wit;
	FvWaypointSet * pWPSet;
	FvWaypoint * pWaypoint;

	for (wit = m_kWaypointSets.begin(); wit != m_kWaypointSets.end(); wit++)
	{
		pWPSet = *wit;
		if (pWPSet == NULL) continue;

		pWaypoint = pWPSet->FindEnclosingWaypoint( kPosition );
		if (pWaypoint != NULL)
		{
			kWPSpec.pWPSet = pWPSet;
			kWPSpec.pWaypoint = pWaypoint;
			return true;
		}
	}

	return false;
}

bool FvWaypointZone::FindClosestWaypoint( const FvVector3 &kPosition,
	FvWPSpec &kWPSpec )
{
	WaypointSets::iterator wit;
	FvWaypointSet * pWPSet;
	FvWaypoint * pWaypoint;
	float bestDistanceSquared = FLT_MAX;
	float distanceSquared = 0.f;

	kWPSpec.pWPSet = NULL;
	kWPSpec.pWaypoint = NULL;

	for (wit = m_kWaypointSets.begin(); wit != m_kWaypointSets.end(); wit++)
	{
		pWPSet = *wit;
		if (pWPSet == NULL) continue;

		pWaypoint = pWPSet->FindClosestWaypoint( kPosition, distanceSquared );
		if (distanceSquared < bestDistanceSquared && pWaypoint != NULL)
		{
			bestDistanceSquared = distanceSquared;
			kWPSpec.pWPSet = pWPSet;
			kWPSpec.pWaypoint = pWaypoint;
		}
	}

	return kWPSpec.pWaypoint != NULL;
}

const FvHullBorder& FvWaypointZone::HullBorder() const
{
	return m_kHullBorder;
}

void FvWaypointZone::AddPlane(const FvPlane& plane)
{
	m_kHullBorder.push_back(plane);
}

void FvWaypointZone::SetBoundingBox( const FvBoundingBox &kBB )
{
	m_kBB = kBB;
	FvVector3 delta = kBB.MaxBounds() - kBB.MinBounds();
	m_fVolume = delta.x * delta.y * delta.z;
	m_kCentre = (kBB.MaxBounds() + kBB.MinBounds()) * 0.5f;
}

bool FvWaypointZone::IsOutsideZone() const
{
	return m_kZoneID.length() >= 9 && m_kZoneID[8] == 'o';
}

bool FvWaypointZone::ContainsPoint( const FvVector3 &kPoint )
{
	if (!m_kBB.Intersects( kPoint )) return false;

	for(unsigned int i = 0; i < m_kHullBorder.size(); i++)
	{
		if(!m_kHullBorder[i].IsInFrontOf(kPoint))
			return false;
	}

	return true;
}
