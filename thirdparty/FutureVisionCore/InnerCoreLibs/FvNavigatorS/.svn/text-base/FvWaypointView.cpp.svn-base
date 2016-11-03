#include "FvWaypointView.h"

#include "FvWaypoint.h"

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNavigator", 0 )

void FvIWaypointView::WriteToSection( FvXMLSectionPtr spSection, 
	FvZone *pkZone, float fDefaultGirth, bool bRemoveOld ) const
{
	int maxSetNum = 0;
	int maxPolyNum = 0;
	if (bRemoveOld)
	{
		while (1)
		{
			FvXMLSectionPtr pWSect = spSection->OpenSection( "navPolySet" );
			if (!pWSect)
			{
				pWSect = spSection->OpenSection( "waypointSet" );
			}
			if (!pWSect) 
			{
				break;
			}
			spSection->DelChild( pWSect );
		}
	}
	else
	{
		std::string wsName("waypointSet");
		std::string wpName("waypoint");
		std::string nsName("navPolySet");
		std::string npName("navPoly");
		for (size_t i = 0; i < spSection->CountChildren(); i++)
		{
			FvXMLSectionPtr wsds = spSection->OpenChild( i );
			if ( !((wsds->SectionName() == nsName) ||
				   (wsds->SectionName() == wsName)) ) continue;
			maxSetNum = std::max( wsds->AsInt(), maxSetNum );
			for (size_t j = 0; j < wsds->CountChildren(); j++)
			{
				FvXMLSectionPtr wpds = wsds->OpenChild( j );
				if ( !((wpds->SectionName() == npName) ||
					   (wpds->SectionName() == wpName)) ) continue;
				maxPolyNum = std::max( wpds->AsInt(), maxPolyNum );
			}
		}
	}

	while (1)
	{
		FvXMLSectionPtr pWSect = spSection->OpenSection( "waypoint" );
		if (!pWSect) 
		{
			pWSect = spSection->OpenSection( "navPoly" );
		}
		if (!pWSect) 
		{
			break;
		}
		spSection->DelChild( pWSect );
	}

	std::map<int,FvXMLSectionPtr> navPolySetSections;

	for (int p = 0; p<this->GetPolygonCount(); p++)
	{
		PolygonRef polygon( *const_cast<FvIWaypointView*>(this), p );
		unsigned int vertexCount = polygon.Size();

		FvXMLSectionPtr & ourNavPolySet = navPolySetSections[ polygon.NavPolySetIndex() ];

		if (!ourNavPolySet)
		{
			ourNavPolySet = spSection->NewSection( "navPolySet" );
			ourNavPolySet->SetInt( polygon.NavPolySetIndex() + maxSetNum );
			float girth = this->GetGirth( polygon.NavPolySetIndex() );
			if (girth <= 0.f) girth = fDefaultGirth;
			ourNavPolySet->WriteFloat( "girth", girth );
		}

		FvXMLSectionPtr pNavPoly = ourNavPolySet->NewSection( "navPoly" );
		pNavPoly->SetInt( maxPolyNum+p+1 );

		FvVector3 mhv = pkZone->TransformInverse().ApplyPoint( 
							FvVector3( pkZone->Centre().x, pkZone->Centre().y,
								polygon.MinHeight() ) );
		FvVector3 hv = pkZone->TransformInverse().ApplyPoint( 
							FvVector3( pkZone->Centre().x, pkZone->Centre().y,
								polygon.MaxHeight() ) );
		pNavPoly->WriteFloat( "minHeight", mhv.z );
		pNavPoly->WriteFloat( "height", hv.z );

		for (unsigned int v = 0; v < vertexCount; v++)
		{
			FvXMLSectionPtr pVertex = pNavPoly->NewSection( "vertex" );
			VertexRef vr = polygon[v];
			int aNavPoly = vr.AdjNavPoly();

			if (aNavPoly> 0 && aNavPoly <= this->GetPolygonCount())
			{
				aNavPoly += maxPolyNum;
			}
			else if (vr.AdjToAnotherZone()) 
			{
				aNavPoly = FV_ZONE_ADJACENT_CONSTANT;
			}

			FvVector3 nc = pkZone->TransformInverse().ApplyPoint( 
							FvVector3( vr.Pos().x, vr.Pos().y,
								polygon.MaxHeight() ) );

			pVertex->SetVector3( FvVector3(
				nc.x, nc.y, float( aNavPoly ) ) );
		}
	}

	FV_INFO_MSG( "Wrote %d polygons to data section %s\n",
		this->GetPolygonCount(), spSection->SectionName().c_str() );
}


static const int s_iNavPolySetEltSize = 16;
static const int s_iNavPolyEltSize = 12;
static const int s_iNavPolyEdgeEltSize = 12;

void FvIWaypointView::SaveOut( FvZone *pkZone, float fDefaultGirth,
	bool bRemoveAllOld )
{
	//DataSectionPtr pChunkBin = BWResource::openSection( pkZone->binFileName() );

	//BinaryPtr pNewNavmesh = this->AsBinary( FvMatrix::identity, fDefaultGirth );

	//if (!bRemoveAllOld && pChunkBin)
	//{
	//	BinaryPtr pOldNavmesh = pChunkBin->readBinary( "worldNavmesh" );

	//	std::vector<int>	oldParts;
	//	int keepSize = 0;
	//	const char * dataPtr = pOldNavmesh->cdata();
	//	const char * dataEnd = dataPtr + pOldNavmesh->len();

	//	while (dataPtr < dataEnd)
	//	{
	//		const char * dataElt = dataPtr;

	//		int aVersion = *((int*&)dataPtr)++;
	//		float aGirth = *((float*&)dataPtr)++;
	//		int navPolyElts = *((int*&)dataPtr)++;
	//		int navPolyEdges = *((int*&)dataPtr)++;

	//		dataPtr += navPolyElts * s_iNavPolyEltSize;
	//		dataPtr += navPolyEdges * s_iNavPolyEdgeEltSize;

	//		int offset = dataPtr - pOldNavmesh->cdata();
	//		if (aGirth == fDefaultGirth) offset |= (1<<31);
	//		else keepSize += dataPtr - dataElt;
	//		oldParts.push_back( offset );
	//	}

	//	if (keepSize > 0)
	//	{
	//		BinaryPtr combo =
	//			new BinaryBlock( NULL, pNewNavmesh->len() + keepSize, "BinaryBlock/IWaypointView" );
	//		char * comboPtr = combo->cdata();
	//		memcpy( comboPtr, pNewNavmesh->cdata(), pNewNavmesh->len() );
	//		comboPtr += pNewNavmesh->len();

	//		int curOffset = 0;
	//		for (unsigned int i = 0; i < oldParts.size(); ++i)
	//		{
	//			if (!(oldParts[i] >> 31))
	//			{
	//				memcpy( comboPtr, pOldNavmesh->cdata() + curOffset,
	//					oldParts[i]-curOffset );
	//				comboPtr += oldParts[i]-curOffset;
	//			}
	//			curOffset = oldParts[i] & ~(1<<31);
	//		}

	//		FV_ASSERT( comboPtr == combo->cdata()+combo->len() );
	//		pNewNavmesh = combo;
	//	}
	//}

	//if (!pChunkBin) 
	//{
	//	DataSectionPtr pTemp = 
	//		new BinSection( "", new BinaryBlock( NULL, 0, "BinaryBlock/IWaypointView" ) );
	//	pTemp->newSection( "worldNavmesh" );
	//	pTemp->save( pkZone->binFileName() );

	//	pChunkBin = BWResource::openSection( pkZone->binFileName() );
	//}

	//pChunkBin->writeBinary( "worldNavmesh", pNewNavmesh );
	//pChunkBin->save();

	//DataSectionPtr pChunkSect =
	//	BWResource::openSection( pkZone->resourceID() );

	//const char* oldSects[] =
	//	{ "waypoint", "waypointSet", "navPoly", "navPolySet", "navmesh" };

	//for (int i = 0; i < sizeof( oldSects ) / sizeof( *oldSects ); i++)
	//{

	//	DataSectionPtr pOldData;
	//	while ((pOldData = pChunkSect->openSection( oldSects[i] )))
	//		pChunkSect->delChild( pOldData );
	//}

	//pChunkSect->writeString( "worldNavmesh/resource",
	//	pkZone->identifier() + ".cdata/worldNavmesh" );

	//pChunkSect->deleteSections( "boundary" );

	//pChunkSect->save();
}


#include <FvMemoryStream.h>

struct PairOfStreams
{
	PairOfStreams() :
		m_pkCtrl( new FvMemoryOStream() ),
		m_pkData( new FvMemoryOStream() ),
		m_fGirth( 0.f ),
		m_iCount( 0 )
	{ }

	FvMemoryOStream *m_pkCtrl;
	FvMemoryOStream *m_pkData;
	float m_fGirth;
	int m_iCount;
	std::map<int,int> m_kIndexMap;
};
typedef std::map<int,PairOfStreams> NavPolySetsAsBinaryMap;

//BinaryPtr FvIWaypointView::AsBinary( const FvMatrix &kTransformToLocal,
//	float fDefaultGirth )
//{
//	NavPolySetsAsBinaryMap	navPolySets;
//
//	for (int p = 0; p < this->GetPolygonCount(); p++)
//	{
//		PolygonRef polygon( *const_cast<FvIWaypointView*>(this), p );
//
//		PairOfStreams & pos = navPolySets[ polygon.NavPolySetIndex() ];
//
//		if (pos.m_fGirth <= 0.f)
//		{
//			pos.m_fGirth = this->GetGirth( polygon.NavPolySetIndex() );
//			if (pos.m_fGirth <= 0.f)
//				pos.m_fGirth = fDefaultGirth;
//		}
//
//		pos.m_kIndexMap[ p+1 ] = pos.m_iCount++;
//	}
//
//	for (int p = 0; p < this->GetPolygonCount(); p++)
//	{
//		PolygonRef polygon( *const_cast<FvIWaypointView*>(this), p );
//		unsigned int vertexCount = polygon.Size();
//
//		PairOfStreams & pos = navPolySets[ polygon.NavPolySetIndex() ];
//		FvMemoryOStream & cmos = *pos.m_pkCtrl;
//		FvMemoryOStream & dmos = *pos.m_pkData;
//
//		FvVector3 mhv = kTransformToLocal.ApplyPoint( FvVector3(
//			polygon[0].Pos().x, polygon[0].Pos().y, polygon.MinHeight() ) );
//		FvVector3 Mhv = kTransformToLocal.ApplyPoint( FvVector3(
//			polygon[0].Pos().x, polygon[0].Pos().y, polygon.MaxHeight() ) );
//
//		cmos << mhv.y << Mhv.y << vertexCount;
//
//		for (unsigned int v = 0; v < vertexCount; v++)
//		{
//			VertexRef vr = polygon[v];
//
//			FvVector3 nc = kTransformToLocal.ApplyPoint( FvVector3(
//				vr.Pos().x, vr.Pos().y, polygon.MaxHeight() ) );
//			dmos << nc.x << nc.z;
//
//			int aNavPoly = vr.AdjNavPoly();
//			if (aNavPoly > 0 && aNavPoly <= this->GetPolygonCount())
//				aNavPoly = pos.m_kIndexMap[ aNavPoly ];
//			else if (vr.AdjToAnotherZone())
//				aNavPoly = 65535;
//			else if (aNavPoly <= 0)
//				aNavPoly = ~-aNavPoly;
//			dmos << aNavPoly;
//		}
//	}
//
//	FvUInt32	dataSize = 0;
//	for (NavPolySetsAsBinaryMap::iterator it = navPolySets.begin();
//		it != navPolySets.end();
//		++it)
//	{
//		dataSize += s_iNavPolySetEltSize +
//			it->second.m_pkCtrl->Size() + it->second.m_pkData->Size();
//	}
//
//	BinaryPtr dataBlock = 
//		new BinaryBlock( NULL, dataSize, "BinaryBlock/IWaypointView" );
//	char * dataPtr = dataBlock->cdata();
//	for (NavPolySetsAsBinaryMap::iterator it = navPolySets.begin();
//		it != navPolySets.end();
//		++it)
//	{
//		PairOfStreams & pos = it->second;
//		*((int*&)dataPtr)++ = 0;
//		*((float*&)dataPtr)++ = pos.m_fGirth;
//		*((int*&)dataPtr)++ = pos.m_pkCtrl->Size()/s_iNavPolyEltSize;
//		*((int*&)dataPtr)++ = pos.m_pkData->Size()/s_iNavPolyEdgeEltSize;
//
//		memcpy( dataPtr, pos.m_pkCtrl->Data(), pos.m_pkCtrl->Size() );
//		dataPtr += pos.m_pkCtrl->Size();
//
//		memcpy( dataPtr, pos.m_pkData->Data(), pos.m_pkData->Size() );
//		dataPtr += pos.m_pkData->Size();
//
//		delete pos.m_pkCtrl;
//		delete pos.m_pkData;
//	}
//
//	return dataBlock;
//}

bool FvIWaypointView::EquivGirth( int iPolygon, float fGirth ) const
{
	int set = this->GetSet( iPolygon );
	if (set <= 0) return true;
	float g = this->GetGirth( set );
	if (g == 0 || g == fGirth) return true;
	return false;
}
