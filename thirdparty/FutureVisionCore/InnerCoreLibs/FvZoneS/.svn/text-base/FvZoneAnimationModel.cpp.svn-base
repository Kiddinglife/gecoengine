#include "FvZoneAnimationModel.h"
#include "FvZoneAnimationModelObstacle.h"
#include "FvZone.h"
#include "FvZoneSpace.h"
#include "FvZoneManager.h"
#include "FvCamera.h"
#include "FvSceneNode.h"
#include "FvRenderManager.h"

#include <FvWatcher.h>
#include <FvDebug.h>

int iZoneModeToken;

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS (spSection, pkZone)
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneAnimationModel, anim, 0 )
//-----------------------------------------------------------------------
FvZoneAnimationModel::FvZoneAnimationModel()
:FvZoneItem((WantFlags)(WANTS_VISIT | WANTS_TICK))
,m_fAnimRateMultiplier(1.f)
,m_kTransform(FvMatrix::IDENTITY)
,m_bCastShadows(false)
,m_bReflectionVisible(false)
,m_bIsRender(false)
#if FV_UMBRA_ENABLE	
,m_bUumbraOccluder( false )
#endif
{
}

FvZoneAnimationModel::~FvZoneAnimationModel()
{
}

bool FvZoneAnimationModel::Load(FvXMLSectionPtr spSection, FvZone* pkZone)
{
	m_spAnimModel = new FvAnimationModel(pkZone->IsOutsideZone() ? "Outside" : pkZone->Identifier());
	FvString kResource = spSection->ReadString("resource");
	m_kTransform = spSection->ReadMatrix34("transform");
	m_bCastShadows = false/* spSection->ReadBool("castShadows")*/;
	m_bReflectionVisible = false/*spSection->ReadBool("reflectionVisible")*/;
	FvString kAnimation = spSection->ReadString("animation");
	if (pkZone)
	{
		FvMatrix kMatrix = pkZone->Transform();
		kMatrix.PreMultiply(m_kTransform);

		if(!m_spAnimModel->Load(kResource,pkZone->Mapping()->Group(),
			pkZone->IsOutsideZone() ? "" : pkZone->Identifier(),
			kMatrix,m_bCastShadows))
		{
			m_spAnimModel = NULL;
			return false;
		}
	}
	AnimationState* anim = m_spAnimModel->GetDefaultAnim();
	if (anim)
	{
		anim->setLoop(true);
		anim->setEnabled(true);
	}
	return true;
}

void FvZoneAnimationModel::Visit(FvCamera* pkCamera, FvZoneVisitor* pkVisitor)
{
	if (!m_spAnimModel)
		return;
	if ((VisitMark() == FvZone::ms_uiNextMark) && m_bIsRender)
	{
		return;
	}
	else
	{
		m_bIsRender = false;
	}
	if (!pkVisitor->Visit(this))
		return;
	if(pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION) 
	&& !m_bReflectionVisible)
		return;
	bool bOnlyShadowCaster = pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_SHADOW_CASTER);
	if(bOnlyShadowCaster && !m_bCastShadows)
		return;

	float fDistSq = pkCamera->getLodCamera()->getRealPosition().squaredDistance(m_spAnimModel->GetNode()->_getDerivedPosition());

	float fInvVolume = m_spAnimModel->GetVolume();

	if((fDistSq * fInvVolume) > FvRenderManager::GetLodDistance(0).x)
		return;

	FvInt32 u32LodLevel = m_spAnimModel->GetCurrentLodLevel();

	const FvVector2 kParams = FvRenderManager::GetLodDistance(u32LodLevel);

	if(u32LodLevel && fDistSq < kParams.x)
	{
		--u32LodLevel;
	}
	else if(u32LodLevel < 3 && fDistSq > kParams.y)
	{
		++u32LodLevel;
	}

	m_spAnimModel->SetCurrentLodLevel(u32LodLevel);

	const FvAnimationModel::Node& kNode = m_spAnimModel->GetAnimationModelNode();
	
	if(pkCamera->isVisible(kNode.m_pkNode->_getWorldAABB()))
	{
		kNode.m_pkNode->_findVisibleObjects(pkCamera,pkVisitor->GetRenderQueue(),
			pkVisitor->GetVisibleObjectsBoundsInfo());
		m_bIsRender = true;
	}
}
//----------------------------------------------------------------------------
void FvZoneAnimationModel::Lend(FvZone *pkLender)
{
	//FV_GUARD;
	if (m_spAnimModel != NULL && m_pkZone != NULL)
	{
		FvMatrix kWorld( m_pkZone->Transform() );
		kWorld.PreMultiply( this->m_kTransform );

		FvBoundingBox kBB = m_spAnimModel->BoundingBox();
		kBB.TransformBy( kWorld );

		this->LendByBoundingBox( pkLender, kBB );
	}
}
//----------------------------------------------------------------------------
const char * FvZoneAnimationModel::Label() const
{
	return m_kLabel.c_str();
}
//----------------------------------------------------------------------------
FvBoundingBox FvZoneAnimationModel::LocalBB() const
{
	if(m_spAnimModel == NULL)
		return FvBoundingBox();

	return m_spAnimModel->BoundingBox();
}
//----------------------------------------------------------------------------
bool FvZoneAnimationModel::AddZBounds(FvBoundingBox &kBB) const
{
	//FV_GUARD;
	if (m_spAnimModel)
	{
		FvBoundingBox kME = m_spAnimModel->BoundingBox();
		kME.TransformBy( m_kTransform );
		kBB.AddZBounds( kME.MinBounds().z );
		kBB.AddZBounds( kME.MaxBounds().z );
	}
	return true;
}
//----------------------------------------------------------------------------
void FvZoneAnimationModel::Toss( FvZone *pkZone )
{
	//FV_GUARD;
	if (m_pkZone != NULL)
	{
		FvZoneAnimationModelObstacle::ms_kInstance( *m_pkZone ).DelObstacles( this );
	}

	this->FvZoneItem::Toss( pkZone );

	if (m_pkZone != NULL)
	{
		FvMatrix kWorld( m_pkZone->Transform() );
		kWorld.PreMultiply( this->m_kTransform );

		if(m_spAnimModel)
		{
			m_spAnimModel->Transform(kWorld);
			static_cast<FvSceneNode*>(m_spAnimModel->GetNode())->SetZone(m_pkZone,true);
			FvZoneAnimationModelObstacle::ms_kInstance( *m_pkZone ).AddAnimationModel(
				m_spAnimModel, kWorld, this );
		}
	}
}
//----------------------------------------------------------------------------
void FvZoneAnimationModel::Tick( float fTime )
{
	AnimationState* anim = m_spAnimModel->GetDefaultAnim();
	if (anim)
	{
		anim->addTime(fTime);
	}
}