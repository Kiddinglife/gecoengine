#include "FvZoneWaypointSet.h"
#include "FvZoneSpace.h"
#include "FvWaypoint.h"

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNavigator", 0 )

bool FvZoneWaypoint::Contains( const FvVector3 &kPoint ) const
{
	if (kPoint.z < m_fMinHeight - 0.1f) return false;
	if (kPoint.z > m_fMaxHeight + 0.1f) return false;

	return this->ContainsProjection( kPoint );
}

bool FvZoneWaypoint::ContainsProjection( const FvVector3 &kPoint ) const
{
	float u, v, xd, yd, c;
	bool bInside = true;
	Edges::const_iterator eit = m_kEdges.begin();
	Edges::const_iterator end = m_kEdges.end();

	const FvVector2 * pLastPoint = &m_kEdges.back().m_kStart;

	int i = 0;
	while (eit != end)
	{
		i = eit - m_kEdges.begin();
		const FvVector2 &thisPoint = eit++->m_kStart;

		u = thisPoint.x - pLastPoint->x;
		v = thisPoint.y - pLastPoint->y;

		xd = kPoint.x - pLastPoint->x;
		yd = kPoint.y - pLastPoint->y;

		c = xd * v - yd * u;

		bInside &= (c > -0.01f);
		if (!bInside)
		{
			return false;
		}

		pLastPoint = &thisPoint;
	}

	return bInside;
}

float FvZoneWaypoint::DistanceSquared( const FvZone *pkZone, const FvVector3 &kLPoint ) const
{
	FvVector3 kClipPoint( kLPoint );
	this->Clip( pkZone, kClipPoint );

	return (kLPoint - kClipPoint).LengthSquared();
}

namespace
{

bool ProjectPointToLine( const FvVector2 &kStart, const FvVector2 &kEnd, FvVector2 &kPoint )
{
	FvVector2 dir = kEnd - kStart;
	FvVector2 project = kPoint - kStart;
	float length = dir.Length();

	dir.Normalise();

	float dot = project.DotProduct( dir );

	if (0.f <= dot && dot <= length)
	{
		kPoint = kStart + dir * dot;
		return true;
	}

	return false;
}

}

void FvZoneWaypoint::Clip( const FvZone *pkZone, FvVector3 &kLPoint ) const
{
	Edges::const_iterator eit = m_kEdges.begin();
	Edges::const_iterator end = m_kEdges.end();

	const FvVector2 *pPrevPoint = &m_kEdges.back().m_kStart;

	while (eit != end)
	{
		const FvVector2 &thisPoint = eit->m_kStart;

		FvVector2 edgeVec( thisPoint - *pPrevPoint );
		FvVector2 pointVec( kLPoint.x - pPrevPoint->x, kLPoint.y - pPrevPoint->y );

		bool isOutsidePoly = edgeVec.CrossProduct( pointVec ) > 0.f;

		if (isOutsidePoly)
			break;

		pPrevPoint = &thisPoint;
		++eit;
	}

	if (eit != end)
	{
		float bestDistSquared = FLT_MAX;
		FvVector2 bestPoint( m_kEdges.front().m_kStart );
		FvVector2 p2d( kLPoint.x, kLPoint.y );

		for (eit = m_kEdges.begin(); eit != end; ++eit)
		{
			FvVector2 p = p2d;
			FvVector2 next = ( eit + 1 == end ) ? m_kEdges.front().m_kStart : ( eit + 1 )->m_kStart;

			if (ProjectPointToLine( eit->m_kStart, next, p ))
			{
				float distSquared = ( p - p2d ).LengthSquared();

				if (distSquared < bestDistSquared)
				{
					bestDistSquared = distSquared;
					bestPoint = p;
				}
			}
		}

		kLPoint.x = bestPoint.x;
		kLPoint.y = bestPoint.y;
	}

	const FvBoundingBox& bb = pkZone->BoundingBox();

	kLPoint.z = bb.Centre().z;

	if (!bb.Intersects( kLPoint ))
	{
		for (eit = m_kEdges.begin(); eit != end; ++eit)
		{
			const Edge* edge = &*eit;

			if (!edge->AdjacentToZone())
			{
				const Edge* next = ( eit + 1 == end ) ? &m_kEdges.front() : &*( eit + 1 );

				FvVector3 start( edge->m_kStart.x, edge->m_kStart.y, bb.Centre().z );
				FvVector3 end( next->m_kStart.x, next->m_kStart.y, bb.Centre().z );

				bb.Clip( start, end );

				FvVector3 middle = ( start + end ) / 2;

				bb.Clip( middle, kLPoint );
				kLPoint = middle + ( kLPoint - middle ) * 0.99; // move in a bit

				break;
			}
		}
	}

	kLPoint.z = FvClampEx( m_fMinHeight, kLPoint.z, m_fMaxHeight );
}


FvUInt16 FvZoneWaypoint::ms_uiNextMark = 256;

extern void NavmeshPopulationRemove( const FvString &kSource );

FvZoneWaypointSetData::FvZoneWaypointSetData() :
	m_fGirth( 0.f ),
	m_kSource(),
	m_pkEdges( NULL )
{
}

FvZoneWaypointSetData::~FvZoneWaypointSetData()
{
	if (!m_kSource.empty())
	{
		NavmeshPopulationRemove( m_kSource );
	}

	if (m_pkEdges) delete [] m_pkEdges;
}

bool FvZoneWaypointSetData::LoadFromXML( FvXMLSectionPtr spSection,
	const FvString &kSectionName )
{
	m_fGirth = spSection->ReadFloat( "Girth", 0.5f );

	typedef FvZoneWaypoint::Edge Edge;
	std::vector<Edge> edgesBuf;
	std::map<int,int> wpids;

	FvXMLSectionIterator dit = spSection->Begin();
	FvXMLSectionIterator dnd = spSection->End();
	while (dit != dnd)
	{
		FvXMLSectionPtr pWaypoint = *dit++;
		if ( pWaypoint->SectionName() != kSectionName ) continue;

		m_kWaypoints.push_back( FvZoneWaypoint() );
		FvZoneWaypoint & cw = m_kWaypoints.back();

		wpids[ pWaypoint->AsInt() ] = m_kWaypoints.size()-1;

		float defHeight = pWaypoint->ReadFloat( "height", 0.f );
		cw.m_fMinHeight = pWaypoint->ReadFloat( "minHeight", defHeight );
		cw.m_fMaxHeight = pWaypoint->ReadFloat( "maxHeight", defHeight );
		(FvUIntPtr&)cw.m_kEdges = sizeof(Edge)*edgesBuf.size();
		cw.m_uiEdgeCount = 0;

		FvXMLSectionIterator vit = pWaypoint->Begin();
		FvXMLSectionIterator vnd = pWaypoint->End();
		while (vit != vnd)
		{
			FvXMLSectionPtr pVertex = *vit++;
			if (pVertex->SectionName() != "vertex") continue;

			edgesBuf.push_back( FvZoneWaypoint::Edge() );
			FvZoneWaypoint::Edge & ce = edgesBuf.back();
			cw.m_uiEdgeCount++;

			FvVector3 v = pVertex->AsVector3();
			ce.m_kStart.x = v.x;
			ce.m_kStart.y = v.y;
			FvInt32 vzi = int(v.z);

			if (pVertex->OpenSection( "adjacentZone" ))
			{
				ce.m_uiNeighbour = 65535;
			}
			else if (vzi == FV_ZONE_ADJACENT_CONSTANT)
			{
				ce.m_uiNeighbour = 65535;
			}
			else
			{
				ce.m_uiNeighbour = vzi > 0 ? vzi : ~-vzi;
			}

		}

		FV_ASSERT( cw.m_kEdges.size() >= 3 )

		cw.m_uiMark = cw.ms_uiNextMark - 16;
	}

	m_pkEdges = new Edge[edgesBuf.size()];
	memcpy( m_pkEdges, &edgesBuf.front(), sizeof(Edge)*edgesBuf.size() );

	FvZoneWaypoints::iterator wit;
	FvZoneWaypoint::Edges::iterator eit;
	std::map<int,int>::iterator nit;
	for (wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); wit++)
	{
		(FvUIntPtr&)wit->m_kEdges += (FvUIntPtr)m_pkEdges;

		for (eit = wit->m_kEdges.begin(); eit != wit->m_kEdges.end(); eit++)
		{
			if (eit->NeighbouringWaypoint() >= 0)
			{
				nit = wpids.find( eit->m_uiNeighbour );
				if (nit == wpids.end())
				{
					FV_ERROR_MSG( "FvZoneWaypointSet::Load: "
						"Cannot Find neighbouring Waypoint %u "
						"on kEdge %td of Waypoint index %td\n",
						eit->m_uiNeighbour,
							   eit - wit->m_kEdges.begin(),
							   wit - m_kWaypoints.begin() );
					return false;
				}
				FV_ASSERT( nit != wpids.end() );
				eit->m_uiNeighbour = nit->second;
			}
		}
	}

	return true;
}

void FvZoneWaypointSetData::Transform( const FvMatrix &kMat )
{
	FvZoneWaypoints::iterator wit;
	FvZoneWaypoint::Edges::iterator eit;
	const FvVector3 & kZTrans = kMat[2];
	const float fZOff = kMat.ApplyToOrigin()[2];

	for (wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); wit++)
	{
		wit->m_fMinHeight = kZTrans.z * wit->m_fMinHeight + fZOff;
		wit->m_fMaxHeight = kZTrans.z * wit->m_fMaxHeight + fZOff;

		for (eit = wit->m_kEdges.begin(); eit != wit->m_kEdges.end(); eit++)
		{
			FvVector3 v = kMat.ApplyPoint(
				FvVector3( eit->m_kStart.x, eit->m_kStart.y, 0.f ) );
			eit->m_kStart.x = v.x; eit->m_kStart.y = v.y;
		}
	}
}

int FvZoneWaypointSetData::Find( const FvVector3 &kLPoint, bool bIgnoreHeight )
{
	int bestWaypoint = -1;
	float bestHeightDiff = FLT_MAX;

	FvZoneWaypoints::iterator wit;
	for (wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); ++wit)
	{
		if (bIgnoreHeight)
		{
			if (wit->ContainsProjection( kLPoint ))
			{
				if (kLPoint.z > wit->m_fMinHeight - 0.1f &&
						kLPoint.z < wit->m_fMaxHeight + 0.1f)
				{
					return wit - m_kWaypoints.begin();
				}
				else 
				{
					float wpAvgDiff = fabs( kLPoint.z -
						(wit->m_fMaxHeight + wit->m_fMinHeight) / 2 );
					if (bestHeightDiff > wpAvgDiff)
					{
						bestHeightDiff = wpAvgDiff;
						bestWaypoint = wit - m_kWaypoints.begin();
					}
				}
			}
		}
		else
		{
			if (wit->Contains( kLPoint ))
				return wit - m_kWaypoints.begin();
		}
	}
	return bestWaypoint;
}

int FvZoneWaypointSetData::Find( const FvZone *pkZone, const FvVector3 &kLPoint,
	float &fBestDistanceSquared )
{
	int bestWaypoint = -1;
	FvZoneWaypoints::iterator wit;
	for (wit = m_kWaypoints.begin(); wit != m_kWaypoints.end(); ++wit)
	{
		float distanceSquared = wit->DistanceSquared( pkZone, kLPoint );
		if (fBestDistanceSquared > distanceSquared)
		{
			fBestDistanceSquared = distanceSquared;
			bestWaypoint = wit - m_kWaypoints.begin();
		}
	}
	return bestWaypoint;
}

int FvZoneWaypointSetData::GetAbsoluteEdgeIndex(
		const FvZoneWaypoint::Edge &kEdge ) const
{
	return &kEdge - m_pkEdges;
}

#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS ( pkZone, spSection, "Waypoint", true )
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneWaypointSet, waypointSet, 0 )

FvZoneWaypointSet::FvZoneWaypointSet():
	m_spData( NULL ),
	m_kConnections(),
	m_kEdgeLabels(),
	m_kBacklinks()
{
}

FvZoneWaypointSet::~FvZoneWaypointSet()
{
}

bool FvZoneWaypointSet::Load( FvZone * pkZone, FvXMLSectionPtr spSection,
	const char *pcSectionName, bool bInWorldCoords )
{
	FvString kSectionNameStr = pcSectionName;

	FvZoneWaypointSetDataPtr spZWSD = new FvZoneWaypointSetData();
	bool bOK = spZWSD->LoadFromXML( spSection, kSectionNameStr );
	if (!bOK) return false;

	if (bInWorldCoords)
		spZWSD->Transform( pkZone->TransformInverse() );

	m_spData = spZWSD;
	return true;
}

void FvZoneWaypointSet::RemoveOurConnections()
{
	m_kEdgeLabels.clear();

	FvZoneWaypointConns::iterator i = m_kConnections.begin();
	for (; i != m_kConnections.end(); ++i)
	{
		i->first->RemoveBacklink( this );
	}

	m_kConnections.clear();
}

void FvZoneWaypointSet::DeleteConnection( FvZoneWaypointSetPtr pSet )
{
	FvZoneWaypointConns::iterator found;
	if ((found = m_kConnections.find( pSet )) == m_kConnections.end())
	{
		FV_ERROR_MSG( "FvZoneWaypointSet::DeleteConnection: connection from %u/%s "
				"to %u/%s does not exist\n",
			this->Zone()->Space()->ID(),
			this->Zone()->Identifier().c_str(),
			pSet->Zone()->Space()->ID(),
			pSet->Zone()->Identifier().c_str() );
		return;
	}

	std::vector<FvWaypointEdgeIndex> removingEdges;
	FvZoneWaypointEdgeLabels::iterator edgeLabel = m_kEdgeLabels.begin();
	for (;edgeLabel != m_kEdgeLabels.end(); ++edgeLabel)
	{
		if (edgeLabel->second == pSet)
		{
			removingEdges.push_back( edgeLabel->first );
		}
	}
	while (removingEdges.size())
	{
		m_kConnections.erase( m_kEdgeLabels[removingEdges.back()] );
		m_kEdgeLabels.erase( removingEdges.back() );
		removingEdges.pop_back();
	}

	pSet->RemoveBacklink( this );

	m_kConnections.erase( pSet );

}

void FvZoneWaypointSet::RemoveOthersConnections()
{
	while (m_kBacklinks.size())
	{
		FvZoneWaypointSets::iterator kBCIter = m_kBacklinks.begin();
		FvZoneWaypointSetPtr pSet = m_kBacklinks.front();

		bool bFound = false;
		FvZoneWaypointConns::iterator kOtherConn = pSet->ConnectionsBegin();
		for (;kOtherConn != pSet->ConnectionsEnd(); ++kOtherConn)
		{
			if (kOtherConn->first == this)
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			FV_ERROR_MSG( "FvZoneWaypointSet::RemoveOthersConnections: "
				"Back connection not found.\n" );
			m_kBacklinks.erase( kBCIter );
			continue;
		}

		(*kBCIter)->DeleteConnection( FvZoneWaypointSetPtr( this ) );
	}

}

void FvZoneWaypointSet::AddBacklink( FvZoneWaypointSetPtr pWaypointSet )
{
	m_kBacklinks.push_back( pWaypointSet );
}

void FvZoneWaypointSet::RemoveBacklink( FvZoneWaypointSetPtr pWaypointSet )
{
	FvZoneWaypointSets::iterator blIter = m_kBacklinks.begin();
	for( ; blIter != m_kBacklinks.end(); ++blIter )
	{
		if ( *blIter == pWaypointSet )
		{
			m_kBacklinks.erase( blIter );
			return;
		}
	}

	FV_ERROR_MSG( "FvZoneWaypointSet::RemoveBacklink: "
		"trying to remove backlink that doesn't exist\n" );

}

void FvZoneWaypointSet::Connect(
						FvZoneWaypointSetPtr pWaypointSet,
						FvZoneBoundary::Portal * pPortal,
						FvZoneWaypoint::Edge & edge )
{
	FvWaypointEdgeIndex edgeIndex = m_spData->GetAbsoluteEdgeIndex( edge );

	if (edge.m_uiNeighbour != 65535)
	{
		FV_WARNING_MSG( "FvZoneWaypointSet::Connect called on "
			"non zone-adjacent kEdge\n" );
		return;
	}

	FvZoneWaypointConns::iterator found = m_kConnections.find( pWaypointSet );

	if (found == m_kConnections.end())
	{
		m_kConnections[pWaypointSet] = pPortal;

		pWaypointSet->AddBacklink( this );

	}
	m_kEdgeLabels[edgeIndex] = pWaypointSet;
}

void FvZoneWaypointSet::Toss( FvZone * pkZone )
{
	if (pkZone == m_pkZone) return;

	if (m_pkZone != NULL)
	{
		this->RemoveOthersConnections();
		this->RemoveOurConnections();

		FvZoneNavigator::ms_kInstance( *m_pkZone ).Del( this );
	}

	this->FvZoneItem::Toss( pkZone );

	if (m_pkZone != NULL)
	{
		if (m_pkZone->Online())
		{
			FV_CRITICAL_MSG( "FvZoneWaypointSet::Toss: "
				"Tossing after loading is not supported\n" );
		}

		FvZoneNavigator::ms_kInstance( *m_pkZone ).Add( this );
	}
}

void FvZoneWaypointSet::Bind()
{
	FvZoneWaypoints::iterator wit;
	FvZoneWaypoint::Edges::iterator eit;

	for (wit = m_spData->m_kWaypoints.begin(); wit != m_spData->m_kWaypoints.end(); ++wit)
	{
		float wzmin = wit->m_fMinHeight;
		float wzmax = wit->m_fMaxHeight;
		float wzavg = (wzmin + wzmax) * 0.5f + 0.1f;

		for (eit = wit->m_kEdges.begin(); eit != wit->m_kEdges.end(); ++eit)
		{
			if (eit->m_uiNeighbour != 65535) continue;

			FvZoneWaypoint::Edges::iterator nextEdgeIter = eit + 1;
			if (nextEdgeIter == wit->m_kEdges.end())
				nextEdgeIter = wit->m_kEdges.begin();

			FvVector3 v = FvVector3( (eit->m_kStart.x + nextEdgeIter->m_kStart.x)/2.f,
					(eit->m_kStart.y + nextEdgeIter->m_kStart.y)/2.f, 0.f );

			FvVector3 wv = v;
			wv.z = wzavg;
			FvVector3 lwv = m_pkZone->TransformInverse().ApplyPoint( wv );

			FvZone::piterator pit = m_pkZone->PBegin();
			FvZone::piterator pnd = m_pkZone->PEnd();
			FvZoneBoundary::Portal * cp = NULL;
			while (pit != pnd)
			{
				if (pit->HasZone())
				{
					const FvVector3& kMinBB = pit->m_pkZone->BoundingBox().MinBounds();
					const FvVector3& kMaxBB = pit->m_pkZone->BoundingBox().MaxBounds();
					if((wv[0] >= kMinBB[0]) && (wv[1] >= kMinBB[1]) && 
						(wv[0] < kMaxBB[0]) && (wv[1] < kMaxBB[1]))
					{
						float minDist = pit->m_pkZone->IsOutsideZone() ?
							0.f : 1.f;
						if (FvZone::FindBetterPortal( cp, minDist, &*pit, lwv ))
						{
							cp = &*pit;
						}
					}
				}
				pit++;
			}
			if (cp == NULL)
			{
				wv.z = wzmax + 0.1f;
				lwv = m_pkZone->TransformInverse().ApplyPoint( wv );

				FvZone::piterator pit = m_pkZone->PBegin();
				FvZone::piterator pnd = m_pkZone->PEnd();
				while (pit != pnd)
				{
					if (pit->HasZone())
					{
						const FvVector3& kMinBB = pit->m_pkZone->BoundingBox().MinBounds();
						const FvVector3& kMaxBB = pit->m_pkZone->BoundingBox().MaxBounds();
						if((wv[0] >= kMinBB[0]) && (wv[1] >= kMinBB[1]) && 
							(wv[0] < kMaxBB[0]) && (wv[1] < kMaxBB[1]))
						{
							float minDist = pit->m_pkZone->IsOutsideZone() ?
								0.f : 1.f;
							if (FvZone::FindBetterPortal( cp, minDist, &*pit, lwv ))
							{
								cp = &*pit;
							}
						}
					}
					pit++;
				}
				if (cp == NULL)
				{
					continue;
				}
			}
			FvZone * pConn = cp->m_pkZone;

			FvZoneNavigator::Result cnr;
			FvVector3 ltpv = cp->m_kOrigin + cp->m_kUAxis*cp->m_kUAxis.DotProduct( lwv ) +
								cp->m_kVAxis*cp->m_kVAxis.DotProduct( lwv );
			FvVector3 tpwv = m_pkZone->Transform().ApplyPoint( ltpv );

			if (!FvZoneNavigator::ms_kInstance( *pConn ).Find(
				tpwv, Girth(), cnr, false ))
			{
				if (FvZoneNavigator::ms_kInstance( *pConn ).HasNavPolySet( Girth() ))
				{
					FV_ERROR_MSG( "FvZoneWaypointSet::Bind: No adjacent navPoly set"
						" through bound portal from %s to %s with Girth %f\n",
						m_pkZone->Identifier().c_str(),
						pConn->Identifier().c_str(), Girth() );
				}
				continue;
			}
			
			this->Connect( &*cnr.m_spSet, cp, *eit );
		}
	}

}

int iZoneWaypointSetToken = 0;

FvZoneNavigator::FvZoneNavigator( FvZone &kZone ) :
	m_kZone( kZone )
{
  if (ms_bUseGirthGrids)
  {
	const FvBoundingBox & bb = m_kZone.BoundingBox();
	float maxDim = std::max( bb.MaxBounds().x - bb.MinBounds().x,
		bb.MaxBounds().y - bb.MinBounds().y );
	float oneSqProp = 1.f / (GG_SIZE-2);
	m_kGGOrigin = FvVector2( bb.MinBounds().x - maxDim*oneSqProp,
		bb.MinBounds().y - maxDim*oneSqProp );
	m_fGGResolution = 1.f / (maxDim*oneSqProp);
  }
}

FvZoneNavigator::~FvZoneNavigator()
{
  if (ms_bUseGirthGrids)
  {
	for (GirthGrids::iterator git = m_kGirthGrids.begin();
		git != m_kGirthGrids.end();
		++git)
	{
		delete [] (*git).m_pkGrid;
	}
	m_kGirthGrids.clear();
  }
}

void FvZoneNavigator::Bind( bool /*bLooseNotBind*/ )
{
	FvZoneWaypointSets::iterator it;
	for (it = m_kWPSets.begin(); it != m_kWPSets.end(); ++it)
	{
		(*it)->Bind();
	}
}

bool FvZoneNavigator::Find( const FvVector3 &kLPoint, float fGirth,
								Result &kRes, bool bIgnoreHeight )
{
	GirthGrids::iterator git;
	for (git = m_kGirthGrids.begin(); git != m_kGirthGrids.end(); ++git)
	{
		if ((*git).m_fGirth == fGirth) break;
	}
	if (git != m_kGirthGrids.end())
	{
		int xg = int((kLPoint.x - m_kGGOrigin.x) * m_fGGResolution);
		int yg = int((kLPoint.y - m_kGGOrigin.y) * m_fGGResolution);
		if (unsigned int(xg) >= GG_SIZE || unsigned int(yg) >= GG_SIZE)
		{
			return false;
		}
		GGList * gg = (*git).m_pkGrid;

		if (gg[xg+yg*GG_SIZE].Find( kLPoint, kRes, bIgnoreHeight ))
		{
			return true;
		}
		if (!bIgnoreHeight)
		{
			if (gg[xg+yg*GG_SIZE].Find( kLPoint, kRes, true ))
			{
				FvZoneWaypoint & wp = kRes.m_spSet->Waypoint( kRes.m_iWaypoint );

				if (wp.m_fMinHeight < kLPoint.z)
				{
					return true;
				}
			}
		}

		float fBestDistanceSquared = FLT_MAX;
		#define TRY_GRID( xi, yi )													\
		{																			\
			int x = (xi);															\
			int y = (yi);															\
			if (unsigned int(x) < GG_SIZE && unsigned int(y) < GG_SIZE)				\
				gg[x+y*GG_SIZE].Find( &m_kZone, kLPoint, fBestDistanceSquared, kRes );\
		}																			\

		TRY_GRID( xg, yg )

		for (int r = 1; r < 12; r++)
		{
			bool hadCandidate = !!kRes.m_spSet;

			int xgCorner = xg - r;
			int ygCorner = yg - r;
			for (int n = 0; n < r+r; n++)
			{
				TRY_GRID( xgCorner+n  , yg-r )
				TRY_GRID( xgCorner+n+1, yg+r )
				TRY_GRID( xg-r, ygCorner+n+1 )
				TRY_GRID( xg+r, ygCorner+n )
			}

			if (hadCandidate)
			{
				return true;
			}
		}

		return false;
#undef TRY_GRID
	}

	FvZoneWaypointSets::iterator it;
	for (it = m_kWPSets.begin(); it != m_kWPSets.end(); ++it)
	{
		if ((*it)->Girth() != fGirth) continue;

		int found = (*it)->Find( kLPoint, bIgnoreHeight );
		if (found >= 0)
		{
			kRes.m_spSet = *it;
			kRes.m_iWaypoint = found;
			kRes.m_bExactMatch = true;
			return true;
		}
	}

	float bestDistanceSquared = FLT_MAX;
	for (it = m_kWPSets.begin(); it != m_kWPSets.end(); ++it)
	{
		if ((*it)->Girth() != fGirth) continue;

		int found = (*it)->Find( kLPoint, bestDistanceSquared );
		if (found >= 0)
		{
			kRes.m_spSet = *it;
			kRes.m_iWaypoint = found;
			kRes.m_bExactMatch = false;
		}
	}

	return !!kRes.m_spSet;
}

bool FvZoneNavigator::GGList::Find( const FvVector3 &kLPoint,
							Result &kRes, bool bIgnoreHeight )
{
	if (bIgnoreHeight)
	{
		const float INVALID_HEIGHT = 1000000.f;
		float bestHeight = INVALID_HEIGHT;
		Result r;

		for (iterator it = begin(); it != end(); ++it)
		{
			FvZoneWaypoint & wp = (*it).m_pkSet->Waypoint( (*it).m_iWaypoint );

			if (wp.ContainsProjection( kLPoint ))
			{
				if (fabsf( wp.m_fMaxHeight - kLPoint.z ) < fabsf( bestHeight - kLPoint.z ))
				{
					bestHeight = wp.m_fMaxHeight;
					r.m_spSet = (*it).m_pkSet;
					r.m_iWaypoint = (*it).m_iWaypoint;
				}
			}
		}

		if (bestHeight != INVALID_HEIGHT)
		{
			r.m_bExactMatch = true;
			kRes = r;

			return true;
		}
	}
	else
	{
		for (iterator it = begin(); it != end(); ++it)
		{
			FvZoneWaypoint & wp = (*it).m_pkSet->Waypoint( (*it).m_iWaypoint );

			if (wp.Contains( kLPoint ))
			{
				kRes.m_spSet = (*it).m_pkSet;
				kRes.m_iWaypoint = (*it).m_iWaypoint;
				kRes.m_bExactMatch = true;
				return true;
			}
		}
	}

	return false;
}

void FvZoneNavigator::GGList::Find( const FvZone *pkZone, const FvVector3 &lpoint,
	float &fBestDistanceSquared, Result &kRes )
{
	for (iterator it = begin(); it != end(); ++it)
	{
		FvZoneWaypoint & wp = (*it).m_pkSet->Waypoint( (*it).m_iWaypoint );
		float distanceSquared = wp.DistanceSquared( pkZone, lpoint );
		if (fBestDistanceSquared > distanceSquared)
		{
			fBestDistanceSquared = distanceSquared;
			kRes.m_spSet = (*it).m_pkSet;
			kRes.m_iWaypoint = (*it).m_iWaypoint;
		}
	}
	if (!!kRes.m_spSet)
	{
		FvZoneWaypoint & wp = kRes.m_spSet->Waypoint( kRes.m_iWaypoint );
		kRes.m_bExactMatch = wp.Contains( lpoint );
	}
}

void FvZoneNavigator::Add( FvZoneWaypointSet *pkSet )
{
	m_kWPSets.push_back( pkSet );
 
	if (!ms_bUseGirthGrids) return;

	if (!m_kZone.IsOutsideZone()) return;

	GirthGrids::iterator git;
	for (git = m_kGirthGrids.begin(); git != m_kGirthGrids.end(); ++git)
	{
		if ((*git).m_fGirth == pkSet->Girth()) break;
	}
	if (git == m_kGirthGrids.end())
	{
		m_kGirthGrids.push_back( GirthGrid() );
		git = m_kGirthGrids.end()-1;
		(*git).m_fGirth = pkSet->Girth();
		(*git).m_pkGrid = new GGList[GG_SIZE*GG_SIZE];
	}
	GGList * gg = (*git).m_pkGrid;

	for (int i = 0; i < pkSet->WaypointCount(); ++i)
	{
		FvZoneWaypoint & wp = pkSet->Waypoint( i );

		float mx = 1000000.f, my = 1000000.f, Mx = -1000000.f, My = -1000000.f;
		for (FvZoneWaypoint::Edges::iterator eit = wp.m_kEdges.begin();
			eit != wp.m_kEdges.end();
			++eit)
		{
			FvVector2 gf = (eit->m_kStart - m_kGGOrigin) * m_fGGResolution;
			if (mx > gf.x) mx = gf.x;
			if (my > gf.y) my = gf.y;
			if (Mx < gf.x) Mx = gf.x;
			if (My < gf.y) My = gf.y;
		}

		for (int xg = int(mx); xg <= int(Mx); ++xg)
			for (int yg = int(my); yg <= int(My); ++yg)
		{
			if (unsigned int(xg) < GG_SIZE && unsigned int(yg) < GG_SIZE)
				gg[xg+yg*GG_SIZE].push_back( GGElement(pkSet, i) );
		}
	}
}

bool FvZoneNavigator::IsEmpty()
{
	return m_kWPSets.size() == 0;
}

bool FvZoneNavigator::HasNavPolySet( float girth )
{
	FvZoneWaypointSets::iterator it;
	bool hasGirth = false;
	for (it = m_kWPSets.begin(); it != m_kWPSets.end(); ++it)
	{
		if ((*it)->Girth() != girth) continue;
		hasGirth = true;
		break;
	}
	return hasGirth;
}

void FvZoneNavigator::Del( FvZoneWaypointSet * pSet )
{
	FvZoneWaypointSets::iterator found = std::find(
		m_kWPSets.begin(), m_kWPSets.end(), pSet );
	FV_ASSERT( found != m_kWPSets.end() )
	m_kWPSets.erase( found );

	if (!ms_bUseGirthGrids) return;

	if (!m_kZone.IsOutsideZone()) return;

	GirthGrids::iterator git;
	for (git = m_kGirthGrids.begin(); git != m_kGirthGrids.end(); ++git)
	{
		if ((*git).m_fGirth == pSet->Girth()) break;
	}
	if (git != m_kGirthGrids.end())
	{
		GGList * gg = (*git).m_pkGrid;
		for (unsigned int i = 0; i < GG_SIZE*GG_SIZE; i++)
		{
			for (unsigned int j = 0; j < gg[i].size(); ++j)
			{
				if (gg[i][j].m_pkSet == pSet)
				{
					gg[i].erase( gg[i].begin() + j );
					--j;
				}
			}
		}
	}
}

FvZoneNavigator::Instance<FvZoneNavigator> FvZoneNavigator::ms_kInstance;
bool FvZoneNavigator::ms_bUseGirthGrids = true;

