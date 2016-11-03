#include "FvWaypointSet.h"
#include "FvWaypointZone.h"

#include <float.h>
#include <set>

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNavigator", 0 )

FvWaypointSet::FvWaypointSet( FvWaypointZone &kZone, int iSetNum ) :
	m_kZone( kZone ),
	m_iSetNum( iSetNum ),
	m_bAdjacentSetsCurrent( false ),
	m_bVisited( false )
{
}

FvWaypointSet::~FvWaypointSet()
{
	this->Clear();
}

void FvWaypointSet::Clear()
{
	Waypoints::iterator wit;
	for (wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); wit++)
	{
		delete *wit;
	}
	m_kWaypoints.clear();

	m_kAdjacentSets.clear();
	m_bAdjacentSetsCurrent = true;
}

void FvWaypointSet::AddWaypoint( FvWaypoint *pkWaypoint )
{
	if (m_kWaypoints.size() < pkWaypoint->GetID() + 1)
		m_kWaypoints.resize( pkWaypoint->GetID() + 1 );
	m_kWaypoints[ pkWaypoint->GetID() ] = pkWaypoint;
}

void FvWaypointSet::LinkWaypoints()
{
	Waypoints::iterator wit;
	FvWaypoint* pWaypoint;
	FvWaypoint* pAdjacent;

	for(wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); wit++)
	{
		pWaypoint = *wit;
		if (pWaypoint == NULL) continue;

		for (int i = 0; i < pWaypoint->VertexCount(); i++)
		{
			if (pWaypoint->AdjacentZoneID(i) == m_kZone.GetZoneID())
			{
				pAdjacent = this->FindWaypoint(pWaypoint->AdjacentID(i));

				if(!pAdjacent)
				{
					FV_WARNING_MSG( "Waypoint %lu in zone %s set %d "
						"linked to non-existent waypoint %lu on edge %d\n",
						pWaypoint->GetID(),
						m_kZone.GetZoneID().c_str(),
						m_iSetNum,
						pWaypoint->AdjacentID(i),
						i );
				}
				else
				{
					pWaypoint->AdjacentWaypoint(i, pAdjacent);
				}
			}
		}
	}

	m_bAdjacentSetsCurrent = false;
}

void FvWaypointSet::Bind( FvWaypointZone &kAZone )
{
	Waypoints::iterator wit;
	FvWaypoint* pWaypoint;

	for(wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); wit++)
	{
		pWaypoint = *wit;
		if (pWaypoint == NULL) continue;

		int vc = pWaypoint->VertexCount();
		for (int i = 0; i < vc; i++)
		{
			if (pWaypoint->AdjacentZoneID(i) == kAZone.GetZoneID())
			{
				FvVector2 midPos = (pWaypoint->VertexPosition(i) +
					pWaypoint->VertexPosition((i+1)%vc)) * 0.5f;
				FvVector3 lookPos( midPos[0], pWaypoint->Height(), midPos[1] );
				FvWPSpec wpspec;
				if (!kAZone.FindEnclosingWaypoint( lookPos, wpspec ) &&
					!kAZone.FindClosestWaypoint( lookPos, wpspec ))
				{
					FV_WARNING_MSG( "Waypoint %lu in zone %s set %d "
						"can find no set in zone %s for edge %d\n",
						pWaypoint->GetID(),
						m_kZone.GetZoneID().c_str(),
						m_iSetNum,
						kAZone.GetZoneID().c_str(),
						i );
				}
				else
				{
					pWaypoint->AdjacentWaypointSet( i, wpspec.pWPSet );
				}
			}
		}
	}

	m_bAdjacentSetsCurrent = false;
}

void FvWaypointSet::Loose( FvWaypointZone &kAZone )
{
	Waypoints::iterator wit;
	FvWaypoint* pWaypoint;

	for(wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); wit++)
	{
		pWaypoint = *wit;
		if (pWaypoint == NULL) continue;

		int vc = pWaypoint->VertexCount();
		for (int i = 0; i < vc; i++)
		{
			if (pWaypoint->AdjacentZoneID(i) == kAZone.GetZoneID())
			{
				FvWaypointSet * pWPSet = pWaypoint->AdjacentWaypointSet( i );
				if (pWPSet != NULL && &pWPSet->Zone() == &kAZone)
				{
					pWaypoint->AdjacentWaypointSet( i, NULL );
				}
			}
		}
	}

	m_bAdjacentSetsCurrent = false;
}

FvWaypoint* FvWaypointSet::FindWaypoint( FvWaypointID uiWaypointID )
{
	if (uiWaypointID >= m_kWaypoints.size()) return NULL;
	return m_kWaypoints[ uiWaypointID ];
}

FvWaypoint* FvWaypointSet::FindEnclosingWaypoint(const FvVector3 &kPosition)
{
	Waypoints::iterator waypointIter;
	FvWaypoint* pWaypoint;

	for(waypointIter = m_kWaypoints.begin();
		waypointIter != m_kWaypoints.end();
		waypointIter++)
	{
		pWaypoint = *waypointIter;
		if(!pWaypoint)
			continue;

		if(pWaypoint->ContainsPoint(kPosition.x, kPosition.y, kPosition.z))
			return pWaypoint;
	}

	return NULL;
}

FvWaypoint* FvWaypointSet::FindClosestWaypoint( const FvVector3 &kPosition,
	float &fFoundDistSquared )
{
	Waypoints::iterator waypointIter;
	FvWaypoint* pWaypoint;
	FvWaypoint* pBestWaypoint = NULL;
	float bestDistanceSquared = FLT_MAX;
	float distanceSquared;

	for(waypointIter = m_kWaypoints.begin();
		waypointIter != m_kWaypoints.end();
		waypointIter++)
	{
		pWaypoint = *waypointIter;
		if (!pWaypoint) continue;

		distanceSquared = (pWaypoint->Centre() - kPosition).LengthSquared();

		if(distanceSquared < bestDistanceSquared)
		{
			bestDistanceSquared = distanceSquared;
			pBestWaypoint = pWaypoint;
		}
	}

	fFoundDistSquared = bestDistanceSquared;
	return pBestWaypoint;
}

FvWaypointID FvWaypointSet::EndWaypointID() const
{
	return m_kWaypoints.size();
}

unsigned int FvWaypointSet::GetAdjacentSetCount() const
{
	if(!m_bAdjacentSetsCurrent)
		this->CacheAdjacentSets();

	return m_kAdjacentSets.size();
}

FvWaypointSet* FvWaypointSet::GetAdjacentSet(unsigned int iIndex) const
{
	if(!m_bAdjacentSetsCurrent)
		this->CacheAdjacentSets();

	if (iIndex >= m_kAdjacentSets.size()) return NULL;
	return m_kAdjacentSets[iIndex];
}

void FvWaypointSet::CacheAdjacentSets() const
{
	std::set<FvWaypointSet*> adjSetsSet;

	Waypoints::const_iterator wit;
	FvWaypoint* pWaypoint;

	for(wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); wit++)
	{
		pWaypoint = *wit;
		if (pWaypoint == NULL) continue;

		int vc = pWaypoint->VertexCount();
		for (int i = 0; i < vc; i++)
		{
			FvWaypointSet * pWPSet = pWaypoint->AdjacentWaypointSet( i );
			if (pWPSet != NULL) adjSetsSet.insert( pWPSet );
		}
	}

	m_kAdjacentSets.clear();
	std::set<FvWaypointSet*>::iterator sit;
	for (sit = adjSetsSet.begin(); sit != adjSetsSet.end(); sit++)
	{
		m_kAdjacentSets.push_back( *sit );
	}

	m_bAdjacentSetsCurrent = true;
}