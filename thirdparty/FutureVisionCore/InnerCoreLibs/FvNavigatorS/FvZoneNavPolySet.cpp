#include "FvZoneNavPolySet.h"
#include <FvZoneSpace.h>
#include <FvConcurrency.h>

#include <OgreResourceGroupManager.h>


FV_DECLARE_DEBUG_COMPONENT2( "FvNavigator", 0 )

int iZoneNavPolySetToken = 0;

FvZoneNavPolySet::FvZoneNavPolySet()
{
}

FvZoneNavPolySet::~FvZoneNavPolySet()
{
}


#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS ( pkZone, spSection, "navPoly", false )
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneNavPolySet, navPolySet, 0 )	// in local coords

typedef std::vector< FvZoneWaypointSetData * > NavmeshPopulationRecord;
typedef std::map< FvString, NavmeshPopulationRecord > NavmeshPopulation;
static NavmeshPopulation s_kNavmeshPopulation;
static FvSimpleMutex s_kNavmeshPopulationLock;

void NavmeshPopulationRemove( const FvString& source )
{
	FvSimpleMutexHolder kSMH( s_kNavmeshPopulationLock );

	s_kNavmeshPopulation.erase( source );
}

template <class C> inline C consume( const char * & ptr )
{
	C * oldPtr = (C*)ptr;
	ptr += sizeof(C);
	return *oldPtr;
}


static const int s_iNavPolySetEltSize = 16;
static const int s_iNavPolyEltSize = 12;
static const int s_iNavPolyEdgeEltSize = 12;

FvZoneItemXMLFactory::Result FvZoneNavPolySet::NavmeshFactory( FvZone *pkZone, 
	FvXMLSectionPtr spSection )
{
	FvString kResName = spSection->ReadString( "resource" );
	FvString kFullName = pkZone->Mapping()->Path() + kResName;

	s_kNavmeshPopulationLock.Grab();
	NavmeshPopulation::iterator found = s_kNavmeshPopulation.find( kFullName );
	if (found != s_kNavmeshPopulation.end() && found->second.back()->IncRefTry())
	{
		const NavmeshPopulationRecord &poprec = found->second;

		std::vector<FvZoneNavPolySet*> sets;
		sets.reserve( poprec.size() );

		for (unsigned int i = 0; i < poprec.size(); ++i)
		{
			FvZoneNavPolySet * pSet = new FvZoneNavPolySet();
			pSet->m_spData = poprec[i];
			sets.push_back( pSet );
		}

		found->second.back()->DecRef();
		s_kNavmeshPopulationLock.Give();

		for (std::vector<FvZoneNavPolySet*>::iterator iter = sets.begin();
			iter != sets.end(); ++iter)
		{
			pkZone->AddStaticItem( *iter );
		}

		return FvZoneItemXMLFactory::SucceededWithoutItem();
	}
	s_kNavmeshPopulationLock.Give();

	Ogre::DataStreamPtr spNavmesh;
	try
	{
		spNavmesh = Ogre::ResourceGroupManager::getSingleton().openResource(
			pkZone->Mapping()->Path() + kResName,
			pkZone->Mapping()->Group());
	}catch(...){}

	if (spNavmesh.isNull())
	{
		FV_ERROR_MSG( "Could not read navmesh '%s'\n", kResName.c_str() );
		return FvZoneItemXMLFactory::Result( NULL );
	}

	size_t stNavmeshSize = spNavmesh->size();
	if (!stNavmeshSize)
	{
		return FvZoneItemXMLFactory::SucceededWithoutItem();
	}

	char *pcNavmeshData = new char[stNavmeshSize];
	spNavmesh->read(pcNavmeshData,stNavmeshSize);

	s_kNavmeshPopulationLock.Grab();
	found = s_kNavmeshPopulation.insert( std::make_pair(
		kFullName, NavmeshPopulationRecord() ) ).first;
	s_kNavmeshPopulationLock.Give();

	const char *dataBeg = pcNavmeshData;
	const char *dataEnd = dataBeg + stNavmeshSize;
	const char *dataPtr = dataBeg;

	while (dataPtr < dataEnd)
	{
		int aVersion = consume<int>( dataPtr );
		float aGirth = consume<float>( dataPtr );
		int navPolyElts = consume<int>( dataPtr );
		int navPolyEdges = consume<int>( dataPtr );

		FV_ASSERT( aVersion == 0 );

		FvZoneWaypointSetDataPtr cwsd = new FvZoneWaypointSetData();
		cwsd->m_fGirth = aGirth;

		const char *edgePtr = dataPtr + navPolyElts * s_iNavPolyEltSize;
		cwsd->m_kWaypoints.resize( navPolyElts );
		cwsd->m_pkEdges = new FvZoneWaypoint::Edge[ navPolyEdges ];
		FvZoneWaypoint::Edge * nedge = cwsd->m_pkEdges;
		for (int p = 0; p < navPolyElts; p++)
		{
			FvZoneWaypoint & wp = cwsd->m_kWaypoints[p];

			wp.m_fMinHeight = consume<float>( dataPtr );
			wp.m_fMaxHeight = consume<float>( dataPtr );
			int vertexCount = consume<int>( dataPtr );

			wp.m_kEdges = FvZoneWaypoint::Edges( nedge, nedge+vertexCount );
			nedge += vertexCount;
			for (int e = 0; e < vertexCount; e++)
			{
				FvZoneWaypoint::Edge & edge = wp.m_kEdges[e];
				edge.m_kStart.x =	consume<float>( edgePtr );
				edge.m_kStart.y = consume<float>( edgePtr );
				edge.m_uiNeighbour = consume<int>( edgePtr );
				--navPolyEdges;
			}

			wp.m_uiMark = wp.ms_uiNextMark - 16;
		}

		FV_ASSERT( navPolyEdges == 0 );
		dataPtr = edgePtr;

		cwsd->m_kSource = found->first;
		found->second.push_back( &*cwsd );

		FvZoneNavPolySet * pSet = new FvZoneNavPolySet();
		pSet->m_spData = cwsd;
		pkZone->AddStaticItem( pSet );
	}
	
	delete[] pcNavmeshData;

	return FvZoneItemXMLFactory::SucceededWithoutItem();
}

FvZoneItemXMLFactory g_kNavmeshFactoryLink( "worldNavmesh", 0,
	&FvZoneNavPolySet::NavmeshFactory );
