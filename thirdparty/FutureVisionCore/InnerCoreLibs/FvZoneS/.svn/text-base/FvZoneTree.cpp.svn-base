#include "FvZoneTree.h"

#include "FvZoneTreeObstacle.h"
#include "FvZone.h"
#include "FvZoneSpace.h"
#include "FvZoneManager.h"
#include "FvCamera.h"
#include "FvSceneNode.h"
#include "FvRenderManager.h"

#include <FvWatcher.h>
#include <FvDebug.h>

int iZoneTreeToken;

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS (spSection, pkZone)
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneTree, tree, 0 )

//----------------------------------------------------------------------------
FvZoneTree::FvZoneTree() :
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
//----------------------------------------------------------------------------
FvZoneTree::~FvZoneTree()
{
	//FV_GUARD;
}
//----------------------------------------------------------------------------
bool FvZoneTree::Load( FvXMLSectionPtr spSection, FvZone *pkZone /*= NULL */ )
{
	m_spTree = new FvTree(pkZone->IsOutsideZone() ? "Outside" : pkZone->Identifier());
	FvString kResource = spSection->ReadString("resource");
	m_kTransform = spSection->ReadMatrix34("transform");
	m_bCastShadows = true/* spSection->ReadBool("castShadows")*/;
	m_bReflectionVisible = spSection->ReadBool("reflectionVisible");

	if (pkZone)
	{
		FvMatrix kMatrix = pkZone->Transform();
		kMatrix.PreMultiply(m_kTransform);

		if(!m_spTree->Load(kResource,pkZone->Mapping()->Group(),
			pkZone->IsOutsideZone() ? "" : pkZone->Identifier(),
			kMatrix,m_bCastShadows))
		{
			m_spTree = NULL;
			return false;
		}
	}

	return true;
}
//----------------------------------------------------------------------------
void FvZoneTree::Toss( FvZone *pkZone )
{
	//FV_GUARD;
	if (m_pkZone != NULL)
	{
		FvZoneTreeObstacle::ms_kInstance( *m_pkZone ).DelObstacles( this );
	}

	this->FvZoneItem::Toss( pkZone );

	if (m_pkZone != NULL)
	{
		FvMatrix kWorld( m_pkZone->Transform() );
		kWorld.PreMultiply( this->m_kTransform );

		if(m_spTree)
		{
			m_spTree->Transform(kWorld);
			static_cast<FvSceneNode*>(m_spTree->GetNode())->SetZone(m_pkZone,true);
			FvZoneTreeObstacle::ms_kInstance( *m_pkZone ).AddTree(
				m_spTree, kWorld, this );
		}
	}
}
//----------------------------------------------------------------------------
void FvZoneTree::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	if(!m_spTree)
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

	float fDistSq = pkCamera->getLodCamera()->getRealPosition().squaredDistance(m_spTree->GetNode()->_getDerivedPosition());

	float fInvVolume = m_spTree->GetVolume();

	if((fDistSq * fInvVolume) > FvRenderManager::GetLodDistance(0).x)
		return;

	FvInt32 u32LodLevel = m_spTree->GetCurrentLodLevel();

	const FvVector2 kParams = FvRenderManager::GetLodDistance(u32LodLevel);

	if(u32LodLevel && fDistSq < kParams.x)
	{
		--u32LodLevel;
	}
	else if(u32LodLevel < 3 && fDistSq > kParams.y)
	{
		++u32LodLevel;
	}

	m_spTree->SetCurrentLodLevel(u32LodLevel);

	const FvTree::Node& kNode = m_spTree->GetTreeNode();

	if(pkCamera->isVisible(kNode.m_pkNode->_getWorldAABB()))
	{
		FvUInt8 u8GroupID = ((FvRenderManager*)(Ogre::Root::getSingleton()._getCurrentSceneManager()))->GetCurrentGroupID();

		const FvRefList<FvInstanceEntity*, FvRefNode1>& kEntityList = kNode.m_kEntityList;

		kEntityList.BeginIterator();
		while (!kEntityList.IsEnd())
		{
			FvRefList<FvInstanceEntity*, FvRefNode1>::iterator iter = kEntityList.GetIterator();
			FvInstanceEntity* obj = static_cast<FvInstanceEntity*>(iter);
			kEntityList.Next();
			FV_ASSERT(obj);
			//
			obj->_updateRenderQueue(pkVisitor->GetRenderQueue());
			//FV_ASSERT(u8GroupID == 30);
			obj->GetInstancePool()->SetGroupID(u8GroupID);
		}

		m_bIsRender = true;
	}
	//m_spTree->GetNode()->_getWorldAABB()

	//m_spTree->GetNode()->_findVisibleObjects(
	//	pkCamera,pkVisitor->GetRenderQueue(),
	//	pkVisitor->GetVisibleObjectsBoundsInfo(),
	//	true,false,bOnlyShadowCaster);
}
//----------------------------------------------------------------------------
void FvZoneTree::Tick(float fTime)
{

}
//----------------------------------------------------------------------------
void FvZoneTree::Lend(FvZone *pkLender)
{
	//FV_GUARD;
	if (m_spTree != NULL && m_pkZone != NULL)
	{
		FvMatrix kWorld( m_pkZone->Transform() );
		kWorld.PreMultiply( this->m_kTransform );

		FvBoundingBox kBB = m_spTree->BoundingBox();
		kBB.TransformBy( kWorld );

		this->LendByBoundingBox( pkLender, kBB );
	}
}
//----------------------------------------------------------------------------
const char * FvZoneTree::Label() const
{
	return m_kLabel.c_str();
}
//----------------------------------------------------------------------------
FvBoundingBox FvZoneTree::LocalBB() const
{
	if(m_spTree == NULL)
		return FvBoundingBox();

	return m_spTree->BoundingBox();
}
//----------------------------------------------------------------------------
bool FvZoneTree::AddZBounds(FvBoundingBox &kBB) const
{
	//FV_GUARD;
	if (m_spTree)
	{
		FvBoundingBox kME = m_spTree->BoundingBox();
		kME.TransformBy( m_kTransform );
		kBB.AddZBounds( kME.MinBounds().z );
		kBB.AddZBounds( kME.MaxBounds().z );
	}
	return true;
}
//----------------------------------------------------------------------------
