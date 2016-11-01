#include "FvZoneModel.h"
#include "FvZoneModelObstacle.h"
#include "FvZone.h"
#include "FvZoneSpace.h"
#include "FvZoneManager.h"
#include "FvCamera.h"
#include "FvSceneNode.h"
#include "FvRenderManager.h"

#include <FvWatcher.h>
#include <FvDebug.h>

int iZoneModelToken;

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS (spSection, pkZone)
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneModel, model, 0 )
FV_IMPLEMENT_ZONE_ITEM_XML_ALIAS( FvZoneModel, shell, 0 )

FvZoneModel::FvZoneModel() :
FvZoneItem( (WantFlags)(WANTS_VISIT | WANTS_TICK) ),
m_fAnimRateMultiplier( 1.f ),
m_kTransform( FvMatrix::IDENTITY ),
m_bCastShadows(false),
m_bReflectionVisible( false ),
m_bIsRender(false)
#if FV_UMBRA_ENABLE	
,m_bUumbraOccluder( false )
#endif
{
}


FvZoneModel::~FvZoneModel()
{
	//FV_GUARD;
}

bool FvZoneModel::Load( FvXMLSectionPtr spSection, FvZone *pkZone )
{
	m_spModel = new FvModel(pkZone->IsOutsideZone() ? "Outside" : pkZone->Identifier());
	FvString kResource = spSection->ReadString("resource");
	m_kTransform = spSection->ReadMatrix34("transform");
	m_bCastShadows = true/* spSection->ReadBool("castShadows")*/;
	m_bReflectionVisible = spSection->ReadBool("reflectionVisible");

	if (pkZone)
	{
		FvMatrix kMatrix = pkZone->Transform();
		kMatrix.PreMultiply(m_kTransform);

		if(!m_spModel->Load(kResource,pkZone->Mapping()->Group(),
			pkZone->IsOutsideZone() ? "" : pkZone->Identifier(),
			kMatrix,m_bCastShadows))
		{
			m_spModel = NULL;
			return false;
		}
	}

	return true;
}

void FvZoneModel::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	if(!m_spModel)
		return;

	if((VisitMark() == FvZone::ms_uiNextMark) && m_bIsRender)
	{
		return;
	}
	else
	{
		m_bIsRender = false;
	}

	if(!pkVisitor->Visit(this))
		return;

	if(pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION) 
		&& !m_bReflectionVisible)
		return;

	bool bOnlyShadowCaster = pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_SHADOW_CASTER);
	if(bOnlyShadowCaster && !m_bCastShadows)
		return;

	float fDistSq = pkCamera->getLodCamera()->getRealPosition().squaredDistance(m_spModel->GetNode()->_getDerivedPosition());

	float fInvVolume = m_spModel->GetVolume();

	if((fDistSq * fInvVolume) > FvRenderManager::GetLodDistance(3).y)
		return;

	FvInt32 u32LodLevel = m_spModel->GetCurrentLodLevel();

	const FvVector2 kParams = FvRenderManager::GetLodDistance(u32LodLevel);

	if(u32LodLevel && fDistSq < kParams.x)
	{
		--u32LodLevel;
	}
	else if(u32LodLevel < 3 && fDistSq > kParams.y)
	{
		++u32LodLevel;
	}

	m_spModel->SetCurrentLodLevel(u32LodLevel);

	//if(fDistSq > 22500) return;

	const FvModel::Node& kNode = m_spModel->GetModleNode();

	if(pkCamera->isVisible(kNode.m_pkNode->_getWorldAABB()))
	{
		FvUInt8 u8GroupID = ((FvRenderManager*)(Ogre::Root::getSingleton()._getCurrentSceneManager()))->GetCurrentGroupID();

		const FvRefList<FvInstanceEntity*, FvRefNode1>& kEntityList = kNode.m_kEntityList;

		FvRefList<FvInstanceEntity*, FvRefNode1>::iterator iter = kEntityList.GetHead();
		while (!kEntityList.IsEnd(iter))
		{
			FvInstanceEntity* obj = static_cast<FvInstanceEntity*>(iter);
			FvRefList<FvInstanceEntity*, FvRefNode1>::Next(iter);
			FV_ASSERT(obj);

			obj->_updateRenderQueue(pkVisitor->GetRenderQueue());

			obj->GetInstancePool()->SetGroupID(u8GroupID);

		}

		m_bIsRender = true;
	}

	//m_spModel->GetNode()->_findVisibleObjects(
	//	pkCamera,pkVisitor->GetRenderQueue(),
	//	pkVisitor->GetVisibleObjectsBoundsInfo(),
	//	true,false,bOnlyShadowCaster);
}

void FvZoneModel::Tick( float fTime )
{

}

void FvZoneModel::Lend( FvZone *pkLender )
{
	//FV_GUARD;
	if (m_spModel != NULL && m_pkZone != NULL)
	{
		FvMatrix kWorld( m_pkZone->Transform() );
		kWorld.PreMultiply( this->m_kTransform );

		FvBoundingBox kBB = m_spModel->BoundingBox();
		kBB.TransformBy( kWorld );

		//if(!IsShellModel(NULL))
		//{
		//	this->LendByBoundingBoxShell( pkLender, kBB );
		//}
		//else
		//{
		//	this->LendByBoundingBox( pkLender, kBB );
		//}

		this->LendByBoundingBox( pkLender, kBB );
	}
}

const char *FvZoneModel::Label() const
{
	return m_kLabel.c_str();
}

void FvZoneModel::Toss( FvZone *pkZone )
{
	//FV_GUARD;
	if (m_pkZone != NULL)
	{
		FvZoneModelObstacle::ms_kInstance( *m_pkZone ).DelObstacles( this );
	}

	this->FvZoneItem::Toss( pkZone );

	if (m_pkZone != NULL)
	{
		FvMatrix kWorld( m_pkZone->Transform() );
		kWorld.PreMultiply( this->m_kTransform );

		if(m_spModel)
		{
			m_spModel->Transform(kWorld);
			static_cast<FvSceneNode*>(m_spModel->GetNode())->SetZone(m_pkZone,true);
			FvZoneModelObstacle::ms_kInstance( *m_pkZone ).AddModel(
				m_spModel, kWorld, this );
		}
	}
}

bool FvZoneModel::AddZBounds( FvBoundingBox &kBB ) const
{
	//FV_GUARD;
	if (m_spModel)
	{
		FvBoundingBox kME = m_spModel->BoundingBox();
		kME.TransformBy( m_kTransform );
		kBB.AddZBounds( kME.MinBounds().z );
		kBB.AddZBounds( kME.MaxBounds().z );
	}
	return true;
}

bool FvZoneModel::IsShellModel( const FvXMLSectionPtr spSection ) const
{
	//FV_GUARD;
	if (Zone() && Zone()->IsOutsideZone())
		return false;

	if( !m_spModel )
		return false;

	FvString kItemRes = m_spModel->GetFileName();

	return kItemRes.substr( 0, 7 ) == "shells/" ||
		kItemRes.find( "/shells/" ) != std::string::npos;
}
