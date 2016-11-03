#include "FvZoneTerrain.h"
#include "FvZoneSpace.h"
#include "FvZoneManager.h"
#include "FvZoneTerrainObstacle.h"
#include "FvCamera.h"
#include "FvRenderManager.h"

#include <FvTerrainRenderable.h>
#include <FvTerrainLodController.h>

int iZoneTerrainToken;

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvZoneTerrain::FvZoneTerrain() :
FvZoneItem( WANTS_VISIT )
{

}

FvZoneTerrain::~FvZoneTerrain()
{

}


void FvZoneTerrain::Toss( FvZone *pkZone )
{
	//FV_GUARD;
	if (m_pkZone != NULL)
	{
		FvZoneTerrainCache::ms_kInstance( *m_pkZone ).ZoneTerrain( NULL );
	}

	this->FvZoneItem::Toss( pkZone );

	if (m_pkZone != NULL)
	{
		FvZoneTerrainCache::ms_kInstance( *pkZone ).ZoneTerrain( this );
	}
}

void FvZoneTerrain::VisitTerrain(FvCamera *pkCamera, FvZoneVisitor *pkVisitor)
{
	if(!m_spTerrainPage->IsInLodControl())
		FvTerrainLodController::Instance().AddPage(m_spTerrainPage.GetObject());

	if(pkCamera->getLodCamera() == NULL)
		return;

	Ogre::RenderQueue *pkQueue = pkVisitor->GetRenderQueue();
	Ogre::VisibleObjectsBoundsInfo *pkBoundsInfo = pkVisitor->GetVisibleObjectsBoundsInfo();
	if(pkQueue == NULL)
		return;

	int iNumTitles = m_spTerrainPage->NumTitles();

	for(int i = 0; i < iNumTitles * iNumTitles ; i++)
	{					
		FvTerrainRenderable *pkRenderable = m_spTerrainPage->GetRenderable(i);		
		if(pkRenderable)
		{			
			if(pkCamera->IsVisible(pkRenderable->BoundingBox()))
			{
				pkRenderable->SetLightMask(m_pkZone->VisitIndex());
				pkRenderable->NotifyCameraPosition(*(FvVector3*)&pkCamera->getLodCamera()->getRealPosition());

				pkQueue->addRenderable(pkRenderable,RENDER_QUEUE_MAX);

				FvTerrainPage::SetTerrainVisible(true);
			}
		}
	}
}

void FvZoneTerrain::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	if(!m_spTerrainPage->IsInLodControl())
		FvTerrainLodController::Instance().AddPage(m_spTerrainPage.GetObject());

	if(!pkVisitor->Visit(this) ||
		pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_SHADOW_CASTER) ||
		pkCamera->getLodCamera() == NULL)
		return;

	Ogre::RenderQueue *pkQueue = pkVisitor->GetRenderQueue();
	Ogre::VisibleObjectsBoundsInfo *pkBoundsInfo = pkVisitor->GetVisibleObjectsBoundsInfo();
	if(pkQueue == NULL)
		return;

	int iNumTitles = m_spTerrainPage->NumTitles();
	m_spTerrainPage->NotifyCameraPosition(*(FvVector3*)&pkCamera->getLodCamera()->getRealPosition());
	for(int i = 0; i < iNumTitles * iNumTitles ; i++)
	{					
		FvTerrainRenderable *pkRenderable = m_spTerrainPage->GetRenderable(i);		
		if(pkRenderable)
		{			
			if(pkCamera->IsVisible(pkRenderable->BoundingBox()))
			{
				pkRenderable->SetLightMask(m_pkZone->VisitIndex());
				pkRenderable->NotifyCameraPosition(*(FvVector3*)&pkCamera->getLodCamera()->getRealPosition());

				Ogre::Any kState = pkRenderable->getUserAny();
				if(m_spTerrainPage->GetOceanType() != FvTerrainPage::OCEAN_TYPE_ONLY)
					pkQueue->addRenderable(pkRenderable,RENDER_QUEUE_MAX);

				if(m_spTerrainPage->GetOceanType() != FvTerrainPage::OCEAN_TYPE_NONE)
				{
					pkRenderable->setUserAny(kState);
					pkQueue->addRenderable(pkRenderable,FvRenderManager::OCEAN_GROUP_ID);
				}

				//if(pkBoundsInfo)
				//{
				//	pkBoundsInfo->merge(
				//		Ogre::AxisAlignedBox(
				//		*(Ogre::Vector3*)&pkRenderable->BoundingBox().MinBounds(),
				//		*(Ogre::Vector3*)&pkRenderable->BoundingBox().MaxBounds()),
				//		Ogre::Sphere(*(Ogre::Vector3*)&pkRenderable->Centre(),
				//		pkRenderable->BoundingRadius()),
				//		pkCamera,true);
				//}
				FvTerrainPage::SetTerrainVisible(true);
			}
		}
	}
}

FvUInt32 FvZoneTerrain::TypeFlags() const
{
	return 0;
}

void FvZoneTerrain::CalculateBB()
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( m_spTerrainPage )
	{
		return;
	}

	m_kBoundingBox = m_spTerrainPage->BoundingBox();

	if (m_pkZone != NULL)
	{
		FvBoundingBox kLocalBB = m_pkZone->LocalBB();
		if( AddZBounds( kLocalBB ) )
		{
			m_pkZone->LocalBB( kLocalBB );
			kLocalBB.TransformBy( m_pkZone->Transform() );
			m_pkZone->BoundingBox( kLocalBB );
		}

#ifndef FV_SERVER
		m_pkZone->AddZBoundsToVisibilityBox(
			m_kBoundingBox.MinBounds().z, 
			m_kBoundingBox.MaxBounds().z);
#endif // FV_SERVER
	}
}

bool FvZoneTerrain::Load( FvXMLSectionPtr spSection, FvZone *pkZone, FvString *kErrorString )
{
	FvString kFullName,kResName;
	kFullName = spSection->ReadString("resource");
	size_t stPos = kFullName.find_first_of('.',0);
	kResName = kFullName.substr(0,stPos);
	kResName += ".ftd";
	m_spTerrainPage = new FvTerrainPage(pkZone->Space()->TerrainSettings());

	if(!m_spTerrainPage->Load(pkZone->Mapping()->Path() + kResName,
		pkZone->Mapping()->Group(),pkZone->Transform().ApplyToOrigin(),
		FvZoneManager::Instance().CameraTrans().ApplyToOrigin()))
	{
		if ( kErrorString )
		{
			*kErrorString = "Could not load terrain block " + kResName + 
				" Reason: " + *kErrorString + "\n";
			return false;
		}
	}

	this->CalculateBB();

	return true;
}

bool FvZoneTerrain::AddZBounds( FvBoundingBox &kBB ) const
{
	kBB.AddZBounds( m_kBoundingBox.MinBounds().z );
	kBB.AddZBounds( m_kBoundingBox.MaxBounds().z );
	return true;
}

#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS (spSection, pkZone, &kErrorString)
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneTerrain, terrain, 0 )

FvZoneTerrainCache::FvZoneTerrainCache( FvZone &kZone ) :
m_pkZone( &kZone ),
m_pkZoneTerrain( NULL ),
m_spObstacle( NULL )
{
}

FvZoneTerrainCache::~FvZoneTerrainCache()
{
}

int FvZoneTerrainCache::Focus()
{
	//FV_GUARD;
	if (!m_pkZoneTerrain || !m_spObstacle) return 0;

	const FvVector3 &kMinB = m_spObstacle->m_kBB.MinBounds();
	const FvVector3 &kMaxB = m_spObstacle->m_kBB.MaxBounds();

	FvVector3 kMidPt = m_spObstacle->m_kTransform.ApplyPoint( (kMinB + kMaxB) / 2.f );

	FvZoneSpace::Column *pkCol = m_pkZone->Space()->pColumn( kMidPt );
	//FV_ASSERT_DEV( pkCol );

	if( pkCol )
		pkCol->AddObstacle( *m_spObstacle );

	return 1;
}

void FvZoneTerrainCache::ZoneTerrain( FvZoneTerrain *pkTerrain )
{
	//FV_GUARD;
	if (pkTerrain != m_pkZoneTerrain)
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

		m_pkZoneTerrain = pkTerrain;

		if (m_pkZoneTerrain != NULL)
		{
			if (m_pkZoneTerrain->m_kBoundingBox.MinBounds().z == m_pkZoneTerrain->m_kBoundingBox.MaxBounds().z)
			{
				m_pkZoneTerrain->m_kBoundingBox.AddZBounds(m_pkZoneTerrain->m_kBoundingBox.MinBounds().z + 1.0f);
			}
			m_spObstacle = new FvZoneTerrainObstacle( *m_pkZoneTerrain->m_spTerrainPage,
				m_pkZone->Transform(), &m_pkZoneTerrain->m_kBoundingBox, pkTerrain );

			if (m_pkZone->Focussed()) this->Focus();
		}
	}
}

FvZoneCache::Instance<FvZoneTerrainCache> FvZoneTerrainCache::ms_kInstance;

class FvTerrainFinderInstance : public FvTerrainFinder
{
public:
	FvTerrainFinderInstance()
	{
		FvTerrainPage::SetTerrainFinder( *this );
	}

	virtual FvTerrainFinder::Details FindTerrainPage( const FvVector3 &kPos )
	{
		//FV_GUARD;
		FvTerrainFinder::Details kDetails;

		FvZoneSpacePtr spSpace = FvZoneManager::Instance().CameraSpace();

		if (spSpace)
		{
			FvZoneSpace::Column *pkColumn = spSpace->pColumn( kPos, false );

			if ( pkColumn != NULL )
			{
				FvZone *pkZone = pkColumn->OutsideZone();

				if (pkZone != NULL)
				{
					FvZoneTerrain *pkZoneTerrain =
						FvZoneTerrainCache::ms_kInstance( *pkZone ).ZoneTerrain();

					if (pkZoneTerrain != NULL)
					{
						kDetails.m_pkPage = pkZoneTerrain->TerrainPage().Get();
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
