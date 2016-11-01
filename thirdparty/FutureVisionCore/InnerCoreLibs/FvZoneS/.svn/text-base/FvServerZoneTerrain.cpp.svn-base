#include "FvServerZoneTerrain.h"
#include "FvZoneSpace.h"

#include <FvDebug.h>

int iZoneTerrainToken;

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvZoneTerrain::FvZoneTerrain() :
	FvZoneItem( WANTS_NOTHING ),
	m_kBB( FvVector3::ZERO, FvVector3::ZERO )
{
}

FvZoneTerrain::~FvZoneTerrain()
{
}

bool FvZoneTerrain::Load( FvXMLSectionPtr spSection, FvZone *pkZone )
{
	FvString kResource = spSection->ReadString( "resource" );

	m_spCacheEntry = FvTerrainPageCache::Instance().FindOrLoad(
			pkZone->Mapping()->Path() + kResource, 
			pkZone->Mapping()->Group(),
			pkZone->Space()->TerrainSettings() );
	if (!m_spCacheEntry)
	{
		FV_ERROR_MSG( "Could not load terrain block '%s'\n", kResource.c_str() );
		return false;
	}

	this->CalculateBB();

	return true;
}

void FvZoneTerrain::CalculateBB()
{
	FV_ASSERT( m_spCacheEntry && m_spCacheEntry->GetPage() );

	m_kBB = m_spCacheEntry->GetPage()->BoundingBox();

	if (m_pkZone != NULL)
	{
		FvBoundingBox kLocalBB = m_pkZone->LocalBB();
		if( AddZBounds( kLocalBB ) )
		{
			m_pkZone->LocalBB( kLocalBB );
			kLocalBB.TransformBy( m_pkZone->Transform() );
			m_pkZone->BoundingBox( kLocalBB );
		}
	}
}

void FvZoneTerrain::Toss( FvZone *pkZone )
{
	if (m_pkZone != NULL)
	{
		FvZoneTerrainCache::ms_kInstance( *m_pkZone ).pTerrain( NULL );
	}

	this->FvZoneItem::Toss( pkZone );

	if (m_pkZone != NULL)
	{
		FvZoneTerrainCache::ms_kInstance( *pkZone ).pTerrain( this );
	}
}

#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS (spSection, pkZone)
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneTerrain, terrain, 0 )

#include "FvZoneTerrainObstacle.h"

FvZoneTerrainCache::FvZoneTerrainCache( FvZone &kZone ) :
	m_pkZone( &kZone ),
	m_pkTerrain( NULL ),
	m_spObstacle( NULL )
{
}

FvZoneTerrainCache::~FvZoneTerrainCache()
{
}

int FvZoneTerrainCache::Focus()
{
	if (!m_pkTerrain || !m_spObstacle) return 0;

	const FvVector3 &kMinB = m_spObstacle->m_kBB.MinBounds();
	const FvVector3 &kMaxB = m_spObstacle->m_kBB.MaxBounds();

	FvVector3 kMidPt = m_spObstacle->m_kTransform.ApplyPoint( (kMinB + kMaxB) / 2.f );

	FvZoneSpace::Column *pkCol = m_pkZone->Space()->pColumn( kMidPt );
	FV_ASSERT( pkCol );

	pkCol->AddObstacle( *m_spObstacle );

	return 1;
}

void FvZoneTerrainCache::pTerrain( FvZoneTerrain *pkTerrain )
{
	if (pkTerrain != m_pkTerrain)
	{
		if (m_spObstacle)
		{
			const FvVector3 &kMinB = m_spObstacle->m_kBB.MinBounds();
			const FvVector3 &kMaxB = m_spObstacle->m_kBB.MaxBounds();
			FvVector3 kMidPt = m_spObstacle->m_kTransform.ApplyPoint( (kMinB + kMaxB) / 2.f );
			FvZoneSpace::Column *pkCol = m_pkZone->Space()->pColumn( kMidPt, false );
			if (pkCol != NULL) pkCol->Stale();

			m_spObstacle = NULL;
		}

		m_pkTerrain = pkTerrain;

		if (m_pkTerrain != NULL)
		{
			m_spObstacle = new FvZoneTerrainObstacle( *m_pkTerrain->GetPage(),
				m_pkZone->Transform(), &m_pkTerrain->m_kBB, pkTerrain );

			if (m_pkZone->Focussed()) this->Focus();
		}
	}
}

FvZoneCache::Instance<FvZoneTerrainCache> FvZoneTerrainCache::ms_kInstance;

FvZoneSpace *g_pkMainSpace = NULL;

class FvTerrainFinderInstance : public FvTerrainFinder
{
public:
	FvTerrainFinderInstance()
	{
		FvTerrainPage::SetTerrainFinder( *this );
	}

	virtual FvTerrainFinder::Details FindTerrainPage( const FvVector3 &kPos )
	{
		FvTerrainFinder::Details kDetails;

		FvZoneSpace *pkSpace = g_pkMainSpace;

		if (pkSpace != NULL)
		{
			FvZoneSpace::Column *pkColumn = pkSpace->pColumn( kPos, false );

			if ( pkColumn != NULL )
			{
				FvZone *pkZone = pkColumn->OutsideZone();

				if (pkZone != NULL)
				{
					FvZoneTerrain *pkZoneTerrain =
						FvZoneTerrainCache::ms_kInstance( *pkZone ).pTerrain();

					if (pkZoneTerrain != NULL)
					{
						kDetails.m_pkPage = pkZoneTerrain->GetPage().GetObject();
						kDetails.m_pkInvMatrix = &pkZone->TransformInverse();
						kDetails.m_pkMatrix = &pkZone->Transform();
					}
				}
			}
		}

		return kDetails;
	}
};

static FvTerrainFinderInstance s_kTerrainFinder;
