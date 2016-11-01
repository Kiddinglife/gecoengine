#include "FvRenderManager.h"

#include "FvCamera.h"
#include "FvZoneManager.h"
#include "FvZoneLight.h"
#include "FvSceneNode.h"
#include <RenderSystems/Direct3D9/include/OgreD3D9RenderSystem.h>
#include <RenderSystems/Direct3D9/include/OgreD3D9Texture.h>
#include <FvInstanceEntity.h>
#include <FvTerrainPage.h>
#include <FvBillboard.h>
#include "FvOcean.h"
#include <FvOcclusionQueryRenderable.h>
#include <../../../FvProject/FvGpuProgramParams.h>
#include "FvWater.h"

using namespace Ogre;

FvVector2 FvRenderManager::ms_kLodDistance[FV_MAX_LOD_LEVEL] =
{
	FvVector2(1500, 2500),
	FvVector2(1600, 10000),
	FvVector2(6400, 40000),
	FvVector2(28900, 1500)
};

bool FvRenderManager::ms_bIsOCStart = false;

FvRenderManager::FvRenderManager() :
Ogre::SceneManager("FvRenderManager"),
m_uiCurrentGroupID(FIRST_GROUP_ID),
m_uiVisitFlag(0),
m_pkBoundsInfo(NULL),
m_bDeferredShadingEnable(false),
m_bDirLightInShellsEnable(false),
m_bHDREnable(false),
m_bShadowEnable(false),
m_bInstanceEnable(false),
m_bDetailBendingEnable(false),
m_pkOutsideRenderingListener(NULL),
m_kOutdoorExtraParams(1,1,1,1),
m_u32ShadowReceiverPass(4),
m_pkDepthTarget(NULL)
{
	m_pkDeferredLightingQuad = NULL;
	m_pkSunMoonLight = NULL;
	m_fSunMoonLightPower = 1.0f;
	m_fAmbientPower = 1.0f;
	m_pkAdaptedQuad = NULL;
	m_u32AdaptedQuadQueue = 8;
	m_bManualCastDirShadow = true;
	m_pkPointLight = NULL;
	m_u32FreeListPoint = 0;
	m_akOcclusionAABBCache[0].m_u32Number = 0;
	m_akOcclusionAABBCache[1].m_u32Number = 0;

	for(FvUInt32 i(0); i < 4; ++i)
	{
		m_apkShadowMaps[i] = NULL;
	}
	
}

FvRenderManager::~FvRenderManager()
{
	for(std::list<Ogre::Entity*>::iterator it = m_kExtraEntityList.begin();
		it != m_kExtraEntityList.end(); ++it)
	{
		destroyEntity(*it);
	}
}

const Ogre::String &FvRenderManager::getTypeName() const
{
	return FvRenderManagerFactory::FACTORY_TYPE_NAME;
}


void FvRenderManager::_FindVisibleTerrains(FvCamera* pkCamera)
{
	m_kOutsideZones.clear();
	m_kInsideZones.clear();
	m_uiCurrentGroupID = FIRST_GROUP_ID;
	m_bHasOutsideRendering = false;
	m_bHasDirLighting = false;

	FvZoneManager::Instance().VisitTerrains(pkCamera,this);
}


void FvRenderManager::_findVisibleObjects( Ogre::Camera *pkCam, 
	Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, bool bOnlyShadowCasters)
{
	m_bOnlyShadowCasters = bOnlyShadowCasters;
	FvTerrainPage::SetTerrainVisible(false);

	if(FvInstance::GetInstanceEnable())
	{
		FvInstance::Reset();
	}

	FvCamera *pkCamera = static_cast<FvCamera*>(pkCam);
	pkCamera->RemoveAllExtraCullingPlanes();
	pkCamera->Update();

	if(m_pkBoundsInfo != pkVisibleBounds)
		m_pkBoundsInfo = pkVisibleBounds;

	if(pkCamera->GetVisibleFinder())
	{
		pkCamera->GetVisibleFinder()->FindVisibleObject(getRenderQueue(),
			pkCamera,pkVisibleBounds,m_bOnlyShadowCasters);

		if(FvInstance::GetInstanceEnable())
			FvInstance::Dump(getRenderQueue());

		FvZone::NextRenderMark();

		return;
	}

	bool bHasDirLightingSave;

	if(!FV_MASK_HAS_ANY(m_uiVisitFlag,FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
		m_uiVisitFlag = FvZoneVisitor::VISIT_FLAG_NONE;

	if(m_bOnlyShadowCasters)
	{
		bHasDirLightingSave = m_bHasDirLighting;

		if(((FvCamera*)pkCam)->_IsSunShadow())
		{
			if((!bHasDirLightingSave) || (!m_bManualCastDirShadow)) return;
		}

		if(!FV_MASK_HAS_ANY(m_uiVisitFlag,FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
			m_uiVisitFlag |= FvZoneVisitor::VISIT_FLAG_SHADOW_CASTER;
		ms_bIsOCStart = false;
	}
	else
	{
		m_u32PointLightNumber = 0;
		m_u32GlobalPointLightNumber = 0;
		m_u32PointLightSpecNumber = 0;
		m_u32GlobalPointLightSpecNumber = 0;
		ms_bIsOCStart = true;
		FV_ASSERT(!m_akOcclusionAABBCache[m_u32FreeListPoint].m_u32Number);
	}

	m_kOutsideZones.clear();
	m_kInsideZones.clear();
	m_uiCurrentGroupID = FIRST_GROUP_ID;
	m_bHasOutsideRendering = false;
	m_bHasDirLighting = false;

	FvZoneManager::Instance().VisitZones(pkCamera,this,m_bOnlyShadowCasters);

	if(!m_bOnlyShadowCasters)
	{
		m_u32FreeListPoint = !m_u32FreeListPoint;

		if(m_pkOutsideRenderingListener)
		{
			m_pkOutsideRenderingListener->Callback(m_bHasOutsideRendering);
		}

		for(std::list<Ogre::Entity*>::iterator it = m_kExtraEntityList.begin();
			it != m_kExtraEntityList.end(); ++it)
		{
			(*it)->_updateRenderQueue(getRenderQueue());
		}

		FvRefList<FvBillboard*, FvRefNode1>::iterator iter = m_kBillboardList.GetHead();
		while (!m_kBillboardList.IsEnd(iter))
		{
			FvBillboard* obj = static_cast<FvBillboard*>(iter);
			FvRefList<FvBillboard*, FvRefNode1>::Next(iter);
			FV_ASSERT(obj);
			//
			getRenderQueue()->addRenderable(obj, obj->GetGroupID());

		}

		if(m_pkDeferredLightingQuad)
		{
			getRenderQueue()->addRenderable(m_pkDeferredLightingQuad->getSubEntity(0), 1);

			FvUInt8* pu8Ambient = (FvUInt8*)&m_au32AmbientCache[31];
			pu8Ambient[0] = (FvUInt8)(mAmbientLight.b * 255);
			pu8Ambient[1] = (FvUInt8)(mAmbientLight.g * 255);
			pu8Ambient[2] = (FvUInt8)(mAmbientLight.r * 255);
			pu8Ambient[3] = (FvUInt8)(m_fAmbientPower * 51);

			FvUInt8* pu8Direction = (FvUInt8*)&m_au32DirectionCache[31];

			pu8Direction[0] = (FvUInt8)(m_kOutdoorExtraParams.z * 51.f);
			pu8Direction[1] = (FvUInt8)(m_kOutdoorExtraParams.y * 51.f);
			pu8Direction[2] = (FvUInt8)(m_kOutdoorExtraParams.x * 51.f);
			pu8Direction[3] = 255;

			// = 0xFFFFFFFF;
			
			/*if(m_bHasDirLighting)
				m_au32DirectionCache[31] = 0xFFFFFFFF;
			else
				m_au32DirectionCache[31] = 0;*/

			m_spManualGlobalLighting->getBuffer()->lock(HardwareBuffer::HBL_DISCARD);

			const PixelBox& kPixelBox = m_spManualGlobalLighting->getBuffer()->getCurrentLock();

			FvCrazyCopy(kPixelBox.data, m_au32AmbientCache, sizeof(m_au32AmbientCache));
			FvCrazyCopy((void*)(((FvUInt32*)kPixelBox.data) + kPixelBox.rowPitch), m_au32DirectionCache, sizeof(m_au32DirectionCache));

			m_spManualGlobalLighting->getBuffer()->unlock();

			if(m_bShadowEnable)
			{
				getRenderQueue()->addRenderable(m_pkDeferredLightingQuad->getSubEntity(0), 2);
			}
		}

		if(m_pkAdaptedQuad && m_bHDREnable)
		{
			getRenderQueue()->addRenderable(m_pkAdaptedQuad->getSubEntity(0), m_u32AdaptedQuadQueue);
			getRenderQueue()->addRenderable(m_pkAdaptedQuad->getSubEntity(0), 14);
			getRenderQueue()->addRenderable(m_pkAdaptedQuad->getSubEntity(0), 15);
			getRenderQueue()->addRenderable(m_pkAdaptedQuad->getSubEntity(0), 13);
			getRenderQueue()->addRenderable(m_pkAdaptedQuad->getSubEntity(0), 11);
			getRenderQueue()->addRenderable(m_pkAdaptedQuad->getSubEntity(0), 12);
			getRenderQueue()->addRenderable(m_pkAdaptedQuad->getSubEntity(0), 21);
		}

		/*if(m_kOcclusionQueryAABBList.Size())
		{
			getRenderQueue()->addRenderable(m_kOcclusionQueryAABBList.GetHead()->m_Content, 79);
		}*/
	}
	else
	{
		m_bHasDirLighting = bHasDirLightingSave;
	}

	for(std::list<FvCamera::VisibleObjectFinder*>::iterator kFIt = m_kVisibleObjectFinders.begin();
		kFIt != m_kVisibleObjectFinders.end(); ++kFIt)
		(*kFIt)->FindVisibleObject(getRenderQueue(),pkCamera,pkVisibleBounds,m_bOnlyShadowCasters);

	if(FvInstance::GetInstanceEnable())
	{
		FvInstance::Dump(getRenderQueue());
	}

	return;
}

void FvRenderManager::AddVisibleFinder(FvCamera::VisibleObjectFinder *pkFinder)
{
	FV_ASSERT(pkFinder);
	for(std::list<FvCamera::VisibleObjectFinder*>::iterator kFIt = m_kVisibleObjectFinders.begin();
		kFIt != m_kVisibleObjectFinders.end(); ++kFIt)
	{
		if((*kFIt) == pkFinder)
			return;
	}
	m_kVisibleObjectFinders.push_back(pkFinder);
}

void FvRenderManager::RemoveVisibleFinder(FvCamera::VisibleObjectFinder *pkFinder)
{
	FV_ASSERT(pkFinder);
	for(std::list<FvCamera::VisibleObjectFinder*>::iterator kFIt = m_kVisibleObjectFinders.begin();
		kFIt != m_kVisibleObjectFinders.end(); ++kFIt)
	{
		if((*kFIt) == pkFinder)
		{
			m_kVisibleObjectFinders.erase(kFIt);
			break;
		}
	}
}

Ogre::Camera* FvRenderManager::createCamera(const Ogre::String &kName)
{
	if (mCameras.find(kName) != mCameras.end())
	{
		OGRE_EXCEPT(
			Ogre::Exception::ERR_DUPLICATE_ITEM,
			"A camera with the name " + kName + " already exists",
			"SceneManager::createCamera" );
	}

	FvCamera *c = OGRE_NEW FvCamera(kName, this);
	mCameras.insert(CameraList::value_type(kName, c));

	mCamVisibleObjectsMap[c] = Ogre::VisibleObjectsBoundsInfo();

	return c;
}

Ogre::SceneNode *FvRenderManager::createSceneNodeImpl(void)
{
	return OGRE_NEW FvSceneNode(this);
}

Ogre::SceneNode *FvRenderManager::createSceneNodeImpl(const Ogre::String &kName)
{
	return OGRE_NEW FvSceneNode(this,kName);
}

bool FvRenderManager::renderableQueued(Ogre::Renderable *pkRend, FvUInt8 uiGroupID, 
	FvUInt16 uiPriority, Ogre::Technique **ppTech, Ogre::RenderQueue *pkQueue)
{
	if(!pkRend->getUserAny().isEmpty())
	{
		if((Ogre::any_cast<FvUInt32>(pkRend->getUserAny())) == FvZone::ms_uiRenderMark)
			return false;
		else
			(*(Ogre::Any*)(void*)(&pkRend->getUserAny())) = FvZone::ms_uiRenderMark;
	}
	else
	{
		pkRend->setUserAny(Any(FvZone::ms_uiRenderMark));
	}

	const FvString& kName = (*ppTech)->getName();
	Ogre::Technique* pkOverwriteTech = NULL;
	switch(uiGroupID)
	{
	case OCEAN_GROUP_ID:
		{
			FvUInt8 u8WaterGroup;

			if(m_bDeferredShadingEnable)
			{
				u8WaterGroup = 3;
			}
			else
			{
				u8WaterGroup = 80;
			}

			RenderQueueGroup* pGroup = mRenderQueue->getQueueGroup(u8WaterGroup);
			pGroup->addRenderable(pkRend, FvOcean::GetTechnique(), uiPriority);	
			return false;
		}
		break;
	case EDGE_HIGH_LIGHT_COLOR_EFFECT:
		{
			if(!m_bOnlyShadowCasters)
			{
				pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
					kName + "_edge_highlight");

				if(pkOverwriteTech)
				{
					RenderQueueGroup* pGroup = mRenderQueue->getQueueGroup(83);
					pGroup->addRenderable(pkRend,pkOverwriteTech,uiPriority);
				}
			}

			pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
				kName + "_color_effect");
			if(pkOverwriteTech) (*ppTech) = pkOverwriteTech;
			uiGroupID = RENDER_QUEUE_MAX;
		}
		break;
	case COLOR_EFFECT_GROUP_ID:
		pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
			kName + "_color_effect");
		if(pkOverwriteTech) (*ppTech) = pkOverwriteTech;
		uiGroupID = RENDER_QUEUE_MAX;
		break;
	case TREE_GROUP_ID:
		if(m_bDetailBendingEnable)
		{
			pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
				kName + "_DetailBending");
			if(pkOverwriteTech) (*ppTech) = pkOverwriteTech;
		}
		uiGroupID = RENDER_QUEUE_MAX;
		break;
	case FADE_OUT_GROUP_ID:
		pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
			kName + "_fade_out");
		if(pkOverwriteTech) (*ppTech) = pkOverwriteTech;
		uiGroupID = RENDER_QUEUE_MAX;
		break;
	case EDGE_HIGH_LIGHT:
		{
			if(!m_bOnlyShadowCasters)
			{
				pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
					kName + "_edge_highlight");

				if(pkOverwriteTech)
				{
					RenderQueueGroup* pGroup = mRenderQueue->getQueueGroup(83);
					pGroup->addRenderable(pkRend,pkOverwriteTech,uiPriority);
				}
			}

			uiGroupID = RENDER_QUEUE_MAX;
		}
		break;
	case EDGE_HIGH_LIGHT_BLEND:
		{
			if(m_bOnlyShadowCasters)
				return false;

			pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
				kName + "_edge_highlight");

			if(pkOverwriteTech)
			{
				RenderQueueGroup* pGroup = mRenderQueue->getQueueGroup(83);
				pGroup->addRenderable(pkRend,pkOverwriteTech,uiPriority);
			}

			pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
				kName + "_blend");
			if(pkOverwriteTech)
			{
				if(m_bDeferredShadingEnable)
				{
					pkOverwriteTech->getPass(0)->getTextureUnitState(0)->setTextureName(m_spRefractionRT->getName());
					pkOverwriteTech->getPass(0)->getTextureUnitState(1)->setTextureName(m_kDepthMap);
				}

				(*ppTech) = pkOverwriteTech;

				if(m_uiCurrentGroupID == FIRST_GROUP_ID)
				{		
					pkRend->setCustomParameter(5, Ogre::Vector4(
						mAmbientLight.r, mAmbientLight.g, mAmbientLight.b, mAmbientLight.a));
				}
				else if(m_uiCurrentGroupID > FIRST_GROUP_ID &&
					m_uiCurrentGroupID <= FIRST_GROUP_ID + m_kInsideZones.size())
				{
					FvUInt32 uiIndex = MAX_OUTSIDE_ZONES + (m_uiCurrentGroupID - FIRST_GROUP_ID);
					FvZone *pkZone = this->VisitIndexToZone(m_uiCurrentGroupID);

					if(pkZone)
					{
						Ogre::ColourValue &kAmbientLight = FvZoneLightCache::ms_kInstance(
							*pkZone).AmbientColor();
						pkRend->setCustomParameter(5, Ogre::Vector4(
							kAmbientLight.r, kAmbientLight.g, kAmbientLight.b, kAmbientLight.a));
					}
					else
					{
						pkRend->setCustomParameter(5, Ogre::Vector4(
							mAmbientLight.r, mAmbientLight.g, mAmbientLight.b, mAmbientLight.a));
					}
				}

				RenderQueueGroup* pGroup = mRenderQueue->getQueueGroup(BLEND_GROUP_ID);
				pGroup->addRenderable(pkRend,pkOverwriteTech,uiPriority);
				return false;
			}
			else
			{
				uiGroupID = RENDER_QUEUE_MAX;
			}
		}
		break;
	case BLEND_GROUP_ID:
		if(m_bOnlyShadowCasters)
			return false;

		pkOverwriteTech = (*ppTech)->getParent()->getTechnique(
			kName + "_blend");
		if(pkOverwriteTech)
		{
			if(m_bDeferredShadingEnable)
			{
				pkOverwriteTech->getPass(0)->getTextureUnitState(0)->setTextureName(m_spRefractionRT->getName());
				pkOverwriteTech->getPass(0)->getTextureUnitState(1)->setTextureName(m_kDepthMap);
			}

			(*ppTech) = pkOverwriteTech;

			if(m_uiCurrentGroupID == FIRST_GROUP_ID)
			{		
				pkRend->setCustomParameter(5, Ogre::Vector4(
					mAmbientLight.r, mAmbientLight.g, mAmbientLight.b, mAmbientLight.a));
			}
			else if(m_uiCurrentGroupID > FIRST_GROUP_ID &&
				m_uiCurrentGroupID <= FIRST_GROUP_ID + m_kInsideZones.size())
			{
				FvUInt32 uiIndex = MAX_OUTSIDE_ZONES + (m_uiCurrentGroupID - FIRST_GROUP_ID);
				FvZone *pkZone = this->VisitIndexToZone(m_uiCurrentGroupID);

				if(pkZone)
				{
					Ogre::ColourValue &kAmbientLight = FvZoneLightCache::ms_kInstance(
						*pkZone).AmbientColor();
					pkRend->setCustomParameter(5, Ogre::Vector4(
						kAmbientLight.r, kAmbientLight.g, kAmbientLight.b, kAmbientLight.a));
				}
				else
				{
					pkRend->setCustomParameter(5, Ogre::Vector4(
						mAmbientLight.r, mAmbientLight.g, mAmbientLight.b, mAmbientLight.a));
				}
			}
		}
		else
		{
			uiGroupID = RENDER_QUEUE_MAX;
		}

		break;

	case BLEND_GROUP_ID_ANIM:
		//if(m_uiCurrentGroupID > FIRST_GROUP_ID &&
		//	m_uiCurrentGroupID <= FIRST_GROUP_ID + m_kInsideZones.size())
		//{
		//	FvUInt32 uiIndex = MAX_OUTSIDE_ZONES + (m_uiCurrentGroupID - FIRST_GROUP_ID);
		//	FvZone *pkZone = this->VisitIndexToZone(m_uiCurrentGroupID);

		//	if(pkZone)
		//	{
		//		Ogre::ColourValue &kAmbientLight = FvZoneLightCache::ms_kInstance(
		//			*pkZone).AmbientColor();
		//		pkRend->setCustomParameter(5, Ogre::Vector4(
		//			kAmbientLight.r, kAmbientLight.g, kAmbientLight.b, kAmbientLight.a));
		//	}
		//	else
		//	{
				pkRend->setCustomParameter(5, Ogre::Vector4(
					mAmbientLight.r, mAmbientLight.g, mAmbientLight.b, mAmbientLight.a));
		//	}
		//}

		break;
	}

	if(uiGroupID == RENDER_QUEUE_MAX)
	{
		uiGroupID = m_uiCurrentGroupID;
		
		RenderQueueGroup* pGroup = mRenderQueue->getQueueGroup(uiGroupID);
		pGroup->addRenderable(pkRend,*ppTech,uiPriority);
		return false;
	}
	return true;
}

RenderQueue *FvRenderManager::getRenderQueue(void)
{
	if (!mRenderQueue)
	{
		initRenderQueue();
		mRenderQueue->setRenderableListener(this);
	}
	return mRenderQueue;
}

void FvRenderManager::_populateLightList(const Ogre::Vector3 &kPosition, float fRadius, 
								Ogre::LightList &kDestList, FvUInt32 uiLightMask)
{
	if(FV_MASK_HAS_ANY(m_uiVisitFlag,FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
	{
		const LightList &kCandidateLights = this->_getLightsAffectingFrustum();
		kDestList.clear();
		kDestList.reserve(kCandidateLights.size());
		LightList::const_iterator kIt;
		for (kIt = kCandidateLights.begin(); kIt != kCandidateLights.end(); ++kIt)
		{
			Light *pkLight = *kIt;
			if (pkLight->getType() == Light::LT_DIRECTIONAL)
				kDestList.push_back(pkLight);
		}

		return;
	}

	if(!m_bDeferredShadingEnable)
	{
		FvZone *pkZone = this->VisitIndexToZone(uiLightMask);
		if(pkZone)
			this->FindZoneItemLight(pkZone,*(FvVector3*)&kPosition,fRadius,kDestList);
	}
}

void FvRenderManager::_populateLightList(const Ogre::SceneNode *pkNode, float fRadius, 
								Ogre::LightList &kDestList, FvUInt32 uiLightMask)
{
	if(FV_MASK_HAS_ANY(m_uiVisitFlag,FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
	{
		const LightList &kCandidateLights = this->_getLightsAffectingFrustum();
		kDestList.clear();
		kDestList.reserve(kCandidateLights.size());
		LightList::const_iterator kIt;
		for (kIt = kCandidateLights.begin(); kIt != kCandidateLights.end(); ++kIt)
		{
			Light *pkLight = *kIt;
			if (pkLight->getType() == Light::LT_DIRECTIONAL)
				kDestList.push_back(pkLight);
		}

		return;
	}

	if(!m_bDeferredShadingEnable)
	{
		FvSceneNode *pkSceneNode = (FvSceneNode*)pkNode;
		while(!pkSceneNode->GetZone() && pkSceneNode->getParent())
			pkSceneNode = static_cast<FvSceneNode*>(pkSceneNode->getParent());

		if(pkSceneNode->GetZone())
			this->FindZoneItemLight(pkSceneNode->GetZone(),
				*(FvVector3*)&pkNode->_getDerivedPosition(),fRadius,kDestList);
	}
}

bool FvRenderManager::validatePassForRendering(const Ogre::Pass* pass)
{
	if(FV_MASK_HAS_ANY(m_uiVisitFlag,FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
		return true;

	return SceneManager::validatePassForRendering(pass);
}

bool FvRenderManager::Visit(FvZone *pkZone)
{
	if(pkZone->m_uiCurrentGroupID == 0)
	{
		if(pkZone->IsOutsideLight())
		{
			m_bHasOutsideRendering = true;
			m_bHasDirLighting = true;
			if(m_kOutsideZones.size() >= MAX_OUTSIDE_ZONES)
				return false;

			m_kOutsideZones.push_back(pkZone);
			FvUInt32 uiIndex = m_kOutsideZones.size();
			pkZone->VisitIndex(uiIndex);
			m_uiCurrentGroupID = FIRST_GROUP_ID;
			pkZone->m_uiCurrentGroupID = m_uiCurrentGroupID;
			return true;
		}

		if(m_kInsideZones.size() >= MAX_INSIDE_ZONES)
			return false;

		m_kInsideZones.push_back(pkZone);
		FvUInt32 uiIndex = MAX_OUTSIDE_ZONES + m_kInsideZones.size();
		pkZone->VisitIndex(uiIndex);
		m_uiCurrentGroupID = FIRST_GROUP_ID + m_kInsideZones.size();
		pkZone->m_uiCurrentGroupID = m_uiCurrentGroupID;

		Ogre::ColourValue &kAmbientLight = FvZoneLightCache::ms_kInstance(
			*pkZone).AmbientColor();

		FvUInt32 u32Index = 31 - m_kInsideZones.size();

		FvUInt8* pu8Ambient = (FvUInt8*)&m_au32AmbientCache[u32Index];

		pu8Ambient[0] = (FvUInt8)(kAmbientLight.b * 255);
		pu8Ambient[1] = (FvUInt8)(kAmbientLight.g * 255);
		pu8Ambient[2] = (FvUInt8)(kAmbientLight.r * 255);
		if(m_bHDREnable)
		{
			pu8Ambient[3] = (FvUInt8)(51.f * kAmbientLight.a);
		}
		else
		{
			pu8Ambient[3] = 51.f;
		}

		if(m_bShadowEnable)
		{
			FvUInt8* pu8Direction = (FvUInt8*)&m_au32DirectionCache[u32Index];

			pu8Direction[0] = 51;
			pu8Direction[1] = 51;
			pu8Direction[2] = 51;
			pu8Direction[3] = 255;
			//m_au32DirectionCache[u32Index] = 0xFFFFFFFF;
		}
		else
		{
			FvUInt8* pu8Direction = (FvUInt8*)&m_au32DirectionCache[u32Index];

			pu8Direction[0] = 51;
			pu8Direction[1] = 51;
			pu8Direction[2] = 51;
			pu8Direction[3] = 0;
			//m_au32DirectionCache[u32Index] = 0;
		}

		m_bHasDirLighting = true;

		return true;
	}
	else
	{
		m_uiCurrentGroupID = pkZone->m_uiCurrentGroupID;
		return true;
	}
}

bool FvRenderManager::fireRenderQueueStarted(FvUInt8 uiID, const Ogre::String &kInvocation)
{
	if((*(FvUInt32*)kInvocation.c_str()) == 
		(*(FvUInt32*)RenderQueueInvocation::RENDER_QUEUE_INVOCATION_SHADOWS.c_str()))
		return Ogre::SceneManager::fireRenderQueueStarted(uiID,kInvocation);

	if(m_bDeferredShadingEnable)
	{
		/*if(uiID == FIRST_GROUP_ID)
		{
			FvUInt32 u32Index = 31 - uiID + FIRST_GROUP_ID;
			float fZone = ((float)u32Index) / 32.f + 1.f / 64.f;
			mAutoParamDataSource->setAmbientLightColour(Ogre::ColourValue(fZone, fZone, fZone, fZone));
		}*/
		if(uiID >= FIRST_GROUP_ID &&
			uiID <= FIRST_GROUP_ID + m_kInsideZones.size())
		{
			FvUInt32 u32Index = 31 - uiID + FIRST_GROUP_ID;
			float fZone = ((float)u32Index) / 32.f + 1.f / 64.f;
			mAutoParamDataSource->setAmbientLightColour(Ogre::ColourValue(fZone, fZone, fZone, fZone));
		}
	}
	else
	{
		if(uiID == FIRST_GROUP_ID)
		{		
			mAutoParamDataSource->setAmbientLightColour(mAmbientLight);
			mDestRenderSystem->setAmbientLight(mAmbientLight.r, mAmbientLight.g, mAmbientLight.b);
		}
		else if(uiID > FIRST_GROUP_ID &&
			uiID <= FIRST_GROUP_ID + m_kInsideZones.size())
		{
			FvUInt32 uiIndex = MAX_OUTSIDE_ZONES + (uiID - FIRST_GROUP_ID);
			FvZone *pkZone = this->VisitIndexToZone(uiIndex);
			Ogre::ColourValue &kAmbientLight = FvZoneLightCache::ms_kInstance(
				*pkZone).AmbientColor();
			mAutoParamDataSource->setAmbientLightColour(kAmbientLight);
			mDestRenderSystem->setAmbientLight(kAmbientLight.r, kAmbientLight.g, kAmbientLight.b);
		}
	}

	return Ogre::SceneManager::fireRenderQueueStarted(uiID,kInvocation);
}

bool FvRenderManager::fireRenderQueueEnded(FvUInt8 uiID, const Ogre::String &kInvocation)
{
	return Ogre::SceneManager::fireRenderQueueEnded(uiID,kInvocation);
}

void FvRenderManager::FindZoneItemLight(FvZone *pkZone, const FvVector3 &kPosition, 
										float fRadius, Ogre::LightList &kDestList)
{
	FvZoneLightCache &kCache = FvZoneLightCache::ms_kInstance(*pkZone);

	const LightList &kCandidateLights = this->_getLightsAffectingFrustum();

	kDestList.clear();
	kDestList.reserve(kCandidateLights.size());

	LightList::const_iterator kIt;
	for (kIt = kCandidateLights.begin(); kIt != kCandidateLights.end(); ++kIt)
	{
		Light *pkLight = *kIt;
		pkLight->_calcTempSquareDist(*(Ogre::Vector3*)&kPosition);

		if (pkLight->getType() == Light::LT_DIRECTIONAL)
		{
			kDestList.push_back(pkLight);
		}
		else
		{
			float fRange = pkLight->getAttenuationRange();
			float fMaxDist = fRange + fRadius;
			if (pkLight->tempSquareDist <= Math::Sqr(fMaxDist))
			{
				if(kCache.IsInAllLights(pkLight))
					kDestList.push_back(pkLight);
			}
		}
	}

	if (this->isShadowTechniqueTextureBased())
	{
		if (kDestList.size() > this->getShadowTextureCount())
		{
			LightList::iterator kStart = kDestList.begin();
			std::advance(kStart, getShadowTextureCount());
			std::stable_sort(kStart, kDestList.end(), lightLess());
		}
	}
	else
	{
		std::stable_sort(kDestList.begin(), kDestList.end(), lightLess());
	}

	size_t stLightIndex = 0;
	for (LightList::iterator kLIt = kDestList.begin(); kLIt != kDestList.end(); ++kLIt, ++stLightIndex)
	{
		(*kLIt)->_notifyIndexInFrame(stLightIndex);
	}
}

FvZone *FvRenderManager::VisitIndexToZone(FvUInt32 uiIndex)
{
	FvUInt32 uiVectorIndex = uiIndex - 1;
	if(uiVectorIndex < m_kOutsideZones.size() &&
		uiVectorIndex >= 0)
		return m_kOutsideZones[uiIndex - 1];

	uiVectorIndex -= MAX_OUTSIDE_ZONES;
	if(uiVectorIndex >= 0 &&
		uiVectorIndex < m_kInsideZones.size())
		return m_kInsideZones[uiVectorIndex];

	return NULL;
}
//----------------------------------------------------------------------------
void FvRenderManager::AttachExtraEntity(Ogre::Entity* pkEntity)
{
	m_kExtraEntityList.push_back(pkEntity);
}
//----------------------------------------------------------------------------
void FvRenderManager::SetDeferredShadingEnable(bool bEnable)
{
	m_bDeferredShadingEnable = bEnable;
}
//----------------------------------------------------------------------------
bool FvRenderManager::GetDeferredShadingEnable()
{
	return m_bDeferredShadingEnable;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetDirLightInShellsEnable(bool bEnable)
{
	m_bDirLightInShellsEnable = bEnable;
}
//----------------------------------------------------------------------------
bool FvRenderManager::GetDirLightInShellsEnable()
{
	return m_bDirLightInShellsEnable;
}
//----------------------------------------------------------------------------
void FvRenderManager::RemoveExtraEntity(Ogre::Entity* pkEntity)
{
	for(std::list<Ogre::Entity*>::iterator it = m_kExtraEntityList.begin();
		it != m_kExtraEntityList.end(); ++it)
	{
		if((*it) == pkEntity)
		{
			m_kExtraEntityList.erase(it);
			break;
		}
	}
}

void FvRenderManager::renderVisibleObjectsDefaultSequence( void )
{
	static RenderOperation ro;
	static float s_afPixelBuffer[64];
	// Render each separate queue
	RenderQueue::QueueGroupIterator queueIt = getRenderQueue()->_getQueueGroupIterator();

	// NB only queues which have been created are rendered, no time is wasted
	//   parsing through non-existent queues (even though there are 10 available)

	while (queueIt.hasMoreElements())
	{
		// Get queue group id
		FvUInt8 qId = queueIt.peekNextKey();
		RenderQueueGroup* pGroup = queueIt.getNext();
		// Skip this one if not to be processed
		if (!isRenderQueueToBeProcessed(qId))
			continue;


		bool repeatQueue = false;
		do // for repeating queues
		{
			// Fire queue started event
			if (fireRenderQueueStarted(qId, 
				mIlluminationStage == IRS_RENDER_TO_TEXTURE ? 
				RenderQueueInvocation::RENDER_QUEUE_INVOCATION_SHADOWS : 
			StringUtil::BLANK))
			{
				// Someone requested we skip this queue
				break;
			}

			if(mIlluminationStage != IRS_RENDER_TO_TEXTURE)
			{
				if(qId == 1 && m_bDeferredShadingEnable)
				{
					(m_pkDeferredLightingQuad->getSubEntity(0))->getRenderOperation(ro);
					ro.srcRenderable = m_pkDeferredLightingQuad->getSubEntity(0);

					LPDIRECT3DDEVICE9 lpDevice = ((D3D9RenderSystem*)mDestRenderSystem)->getDevice();

					FvUInt32 u32PassNumber = 0;

					FvCamera* pkSceneCamera = (FvCamera*)mCameraInProgress;
					const Ogre::Matrix4& kView = pkSceneCamera->getViewMatrix();

					const FvVector3 kUpVector(0, 0, 1);

					s_afPixelBuffer[0] = FvDot3f((float*)&kView, kUpVector);
					s_afPixelBuffer[1] = FvDot3f(((float*)&kView) + 4, kUpVector);
					s_afPixelBuffer[2] = FvDot3f(((float*)&kView) + 8, kUpVector);
					s_afPixelBuffer[4] = m_bHDREnable ? m_pkSunMoonLight->getDiffuseColour().a : 1.0f;
					s_afPixelBuffer[5] = m_kOutdoorExtraParams.y;
					s_afPixelBuffer[6] = m_kOutdoorExtraParams.z;
					s_afPixelBuffer[7] = m_kOutdoorExtraParams.w;

					if(m_pkSunMoonLight && m_bHasDirLighting)
					{
						const Vector3& kDirection = m_pkSunMoonLight->getDirection();
						s_afPixelBuffer[8] = -FvDot3f((float*)&kView, (const float*)&kDirection);
						s_afPixelBuffer[9] = -FvDot3f(((float*)&kView) + 4, (const float*)&kDirection);
						s_afPixelBuffer[10] = -FvDot3f(((float*)&kView) + 8, (const float*)&kDirection);

						if(m_bHDREnable)
						{
							s_afPixelBuffer[12] = m_pkSunMoonLight->getDiffuseColour().r * m_fSunMoonLightPower;
							s_afPixelBuffer[13] = m_pkSunMoonLight->getDiffuseColour().g * m_fSunMoonLightPower;
							s_afPixelBuffer[14] = m_pkSunMoonLight->getDiffuseColour().b * m_fSunMoonLightPower;

							s_afPixelBuffer[16] = m_pkSunMoonLight->getSpecularColour().r * m_fSunMoonLightPower;
							s_afPixelBuffer[17] = m_pkSunMoonLight->getSpecularColour().g * m_fSunMoonLightPower;
							s_afPixelBuffer[18] = m_pkSunMoonLight->getSpecularColour().b * m_fSunMoonLightPower;
						}
						else
						{
							s_afPixelBuffer[12] = m_pkSunMoonLight->getDiffuseColour().r;
							s_afPixelBuffer[13] = m_pkSunMoonLight->getDiffuseColour().g;
							s_afPixelBuffer[14] = m_pkSunMoonLight->getDiffuseColour().b;

							s_afPixelBuffer[16] = m_pkSunMoonLight->getSpecularColour().r;
							s_afPixelBuffer[17] = m_pkSunMoonLight->getSpecularColour().g;
							s_afPixelBuffer[18] = m_pkSunMoonLight->getSpecularColour().b;
						}

						//Ogre::Vector4 kLightDirection(-m_pkSunMoonLight->getDirection().x,
						//	-m_pkSunMoonLight->getDirection().y, -m_pkSunMoonLight->getDirection().z, 1);
						//Ogre::Vector4 kLightDiffuseColor(m_pkSunMoonLight->getDiffuseColour().r,
						//	m_pkSunMoonLight->getDiffuseColour().g, m_pkSunMoonLight->getDiffuseColour().b,
						//	m_pkSunMoonLight->getDiffuseColour().a);
						//Ogre::Vector4 kLightSpecularColor(m_pkSunMoonLight->getSpecularColour().r,
						//	m_pkSunMoonLight->getSpecularColour().g, m_pkSunMoonLight->getSpecularColour().b,
						//	m_pkSunMoonLight->getSpecularColour().a);
						//if(m_bHDREnable)
						//{
						//	kLightDiffuseColor *= m_fSunMoonLightPower;
						//	kLightSpecularColor *= m_fSunMoonLightPower;
						//}

						if(m_bShadowEnable)
						{
							if(m_bManualCastDirShadow)
							{
								u32PassNumber = 2;
							}
						}
						else
						{
							u32PassNumber = 1;
						}
					}

					Ogre::Pass* pkUsePass = m_spDeferredLightingMaterial->getTechnique(0)->getPass(u32PassNumber);

					const Ogre::Pass* pkCurrentPass = _setPass(
						pkUsePass, false, false);

					_RenderSingleObject(m_pkDeferredLightingQuad->getSubEntity(0), pkCurrentPass, false, false);

					if(u32PassNumber > 0)
					{
						lpDevice->SetPixelShaderConstantF(0, s_afPixelBuffer, 5);
					}
					else
					{
						lpDevice->SetPixelShaderConstantF(0, s_afPixelBuffer, 2);
					}

					mDestRenderSystem->_render(ro);

					if(m_pkPointLight)
					{
						FV_ASSERT(!m_spPointLightMaterial.isNull());

						RenderOperation kPointLight;
						(m_pkPointLight->getSubEntity(0))->getRenderOperation(kPointLight);
						kPointLight.srcRenderable = m_pkPointLight->getSubEntity(0);

						const Ogre::Matrix4& kView = mCameraInProgress->getViewMatrix();

						//s_afPixelBuffer[0] = FvDot3f((float*)&kView, kUpVector);
						//s_afPixelBuffer[1] = FvDot3f(((float*)&kView) + 4, kUpVector);
						//s_afPixelBuffer[2] = FvDot3f(((float*)&kView) + 8, kUpVector);

						const Ogre::Pass* pkCurrentPass = _setPass(
							m_spPointLightMaterial->getTechnique(0)->getPass(0), false, false);
						_RenderSingleObject(m_pkPointLight->getSubEntity(0), pkCurrentPass, false, false);

						for(FvUInt32 i(0); i < m_u32PointLightNumber; ++i)
						{
							m_apkPointLight[i]->m_kPositionWS.w = 1;
							
							m_apkPointLight[i]->m_kPositionVS.x = FvDot4f((float*)&kView, m_apkPointLight[i]->m_kPositionWS);
							m_apkPointLight[i]->m_kPositionVS.y = FvDot4f(((float*)&kView) + 4, m_apkPointLight[i]->m_kPositionWS);
							m_apkPointLight[i]->m_kPositionVS.z = FvDot4f(((float*)&kView) + 8, m_apkPointLight[i]->m_kPositionWS);

							m_apkPointLight[i]->m_kPositionWS.w = m_apkPointLight[i]->m_kAttenuation.x;

							lpDevice->SetVertexShaderConstantF(0, (float*)(m_apkPointLight[i]), 1);
							lpDevice->SetPixelShaderConstantF(1, (float*)(m_apkPointLight[i]) + 4, 3);
							
							mDestRenderSystem->_render(kPointLight);
						}

						pkCurrentPass = _setPass(
							m_spPointLightMaterial->getTechnique(0)->getPass(2), false, false);
						_RenderSingleObject(m_pkPointLight->getSubEntity(0), pkCurrentPass, false, false);

						for(FvUInt32 i(0); i < m_u32PointLightSpecNumber; ++i)
						{
							m_apkPointLightSpec[i]->m_kPositionWS.w = 1;

							m_apkPointLightSpec[i]->m_kPositionVS.x = FvDot4f((float*)&kView, m_apkPointLightSpec[i]->m_kPositionWS);
							m_apkPointLightSpec[i]->m_kPositionVS.y = FvDot4f(((float*)&kView) + 4, m_apkPointLightSpec[i]->m_kPositionWS);
							m_apkPointLightSpec[i]->m_kPositionVS.z = FvDot4f(((float*)&kView) + 8, m_apkPointLightSpec[i]->m_kPositionWS);

							m_apkPointLightSpec[i]->m_kPositionWS.w = m_apkPointLightSpec[i]->m_kAttenuation.x;

							lpDevice->SetVertexShaderConstantF(0, (float*)(m_apkPointLightSpec[i]), 1);
							lpDevice->SetPixelShaderConstantF(1, (float*)(m_apkPointLightSpec[i]) + 4, 4);

							mDestRenderSystem->_render(kPointLight);
						}

						pkCurrentPass = _setPass(
							m_spPointLightMaterial->getTechnique(0)->getPass(1), false, false);
						_RenderSingleObject(m_pkDeferredLightingQuad->getSubEntity(0), pkCurrentPass, false, false);

						for(FvUInt32 i(0); i < m_u32GlobalPointLightNumber; ++i)
						{
							m_apkGlobalPointLight[i]->m_kPositionWS.w = 1;

							m_apkGlobalPointLight[i]->m_kPositionVS.x = FvDot4f((float*)&kView, m_apkGlobalPointLight[i]->m_kPositionWS);
							m_apkGlobalPointLight[i]->m_kPositionVS.y = FvDot4f(((float*)&kView) + 4, m_apkGlobalPointLight[i]->m_kPositionWS);
							m_apkGlobalPointLight[i]->m_kPositionVS.z = FvDot4f(((float*)&kView) + 8, m_apkGlobalPointLight[i]->m_kPositionWS);

							lpDevice->SetPixelShaderConstantF(1, (float*)(m_apkGlobalPointLight[i]) + 4, 3);

							mDestRenderSystem->_render(ro);
						}

						pkCurrentPass = _setPass(
							m_spPointLightMaterial->getTechnique(0)->getPass(3), false, false);
						_RenderSingleObject(m_pkDeferredLightingQuad->getSubEntity(0), pkCurrentPass, false, false);

						for(FvUInt32 i(0); i < m_u32GlobalPointLightSpecNumber; ++i)
						{
							

							m_apkGlobalPointLightSpec[i]->m_kPositionWS.w = 1;

							m_apkGlobalPointLightSpec[i]->m_kPositionVS.x = FvDot4f((float*)&kView, m_apkGlobalPointLightSpec[i]->m_kPositionWS);
							m_apkGlobalPointLightSpec[i]->m_kPositionVS.y = FvDot4f(((float*)&kView) + 4, m_apkGlobalPointLightSpec[i]->m_kPositionWS);
							m_apkGlobalPointLightSpec[i]->m_kPositionVS.z = FvDot4f(((float*)&kView) + 8, m_apkGlobalPointLightSpec[i]->m_kPositionWS);

							lpDevice->SetPixelShaderConstantF(1, (float*)(m_apkGlobalPointLightSpec[i]) + 4, 4);

							mDestRenderSystem->_render(ro);
						}
					}
					
				}
				else if(qId == 2 && m_bShadowEnable && m_bManualCastDirShadow)
				{
					mDestRenderSystem->setStencilCheckEnabled(true);

					mDestRenderSystem->setStencilBufferParams(CMPF_ALWAYS_PASS, 0, 0xffffffff, SOP_KEEP, SOP_INCREMENT);

					const Ogre::Pass* pkCurrentPass = _setPass(
						m_spDeferredLightingMaterial->getTechnique(0)->getPass(3), false, false);

					LPDIRECT3DDEVICE9 lpDevice = ((D3D9RenderSystem*)mDestRenderSystem)->getDevice();

					renderSingleObject(m_pkDeferredLightingQuad->getSubEntity(0), pkCurrentPass, false, false);

					(m_pkDeferredLightingQuad->getSubEntity(0))->getRenderOperation(ro);
					ro.srcRenderable = m_pkDeferredLightingQuad->getSubEntity(0);

					mDestRenderSystem->setCurrentPassIterationCount(1);

					lpDevice->SetVertexShaderConstantF(0, FvGpuProgramParams::GetPSSMParamsCache() + 2, 1);

					mDestRenderSystem->_render(ro);

					lpDevice->SetVertexShaderConstantF(0, FvGpuProgramParams::GetPSSMParamsCache() + 3, 1);

					mDestRenderSystem->_render(ro);
					
					mDestRenderSystem->setStencilBufferParams(CMPF_EQUAL, 4);

					pkCurrentPass = _setPass(
						m_spDeferredLightingMaterial->getTechnique(0)->getPass(m_u32ShadowReceiverPass), false, false);
					_RenderSingleObject(m_pkDeferredLightingQuad->getSubEntity(0), pkCurrentPass, false, false);

					if(m_u32ShadowReceiverPass > 4)
					{
						lpDevice->SetVertexShaderConstantF(9, m_afShadowPCFParams, 4);
						lpDevice->SetPixelShaderConstantF(4, m_afShadowMapParams, 1);
					}

					mDestRenderSystem->_render(ro);

					mDestRenderSystem->setStencilBufferParams(CMPF_EQUAL, 3);
					lpDevice->SetTexture(1, m_apkShadowMaps[1]);
					lpDevice->SetPixelShaderConstantF(0, FvGpuProgramParams::GetPSSMParamsCache() + 20, 4);
					mDestRenderSystem->_render(ro);

					mDestRenderSystem->setStencilBufferParams(CMPF_EQUAL, 2);
					lpDevice->SetTexture(1, m_apkShadowMaps[2]);
					lpDevice->SetPixelShaderConstantF(0, FvGpuProgramParams::GetPSSMParamsCache() + 36, 4);
					mDestRenderSystem->_render(ro);

					mDestRenderSystem->setStencilBufferParams(CMPF_EQUAL, 1);
					lpDevice->SetTexture(1, m_apkShadowMaps[3]);
					lpDevice->SetPixelShaderConstantF(0, FvGpuProgramParams::GetPSSMParamsCache() + 52, 4);
					mDestRenderSystem->_render(ro);

					mDestRenderSystem->setStencilBufferParams(CMPF_LESS, 0);

					pkCurrentPass = _setPass(
						m_spDeferredLightingMaterial->getTechnique(0)->getPass(6), false, false);
					_RenderSingleObject(m_pkDeferredLightingQuad->getSubEntity(0), pkCurrentPass, false, false);
					lpDevice->SetPixelShaderConstantF(0, m_afTerrainShadowMatrix, 4);
					mDestRenderSystem->_render(ro);

					mDestRenderSystem->setStencilCheckEnabled(false);					
				}
				else if(qId == 8 && qId == m_u32AdaptedQuadQueue)
				{
					if(m_pkAdaptedQuad)
						_injectRenderWithPass(m_spHDRAdaptedMaterial->getTechnique(0)->getPass(0), m_pkAdaptedQuad->getSubEntity(0), false);
				}
				else if(qId == 9 && qId == m_u32AdaptedQuadQueue)
				{
					if(m_pkAdaptedQuad)
						_injectRenderWithPass(m_spHDRAdaptedMaterial->getTechnique(0)->getPass(1), m_pkAdaptedQuad->getSubEntity(0), false);
				}
				else if(qId == 14)
				{
					if(m_pkAdaptedQuad)
					{
						if(m_u32AdaptedQuadQueue == 8)
						{
							_injectRenderWithPass(m_spHDRBrightPassMaterial->getTechnique(0)->getPass(1), m_pkAdaptedQuad->getSubEntity(0), false);
						}
						else if(m_u32AdaptedQuadQueue == 9)
						{
							_injectRenderWithPass(m_spHDRBrightPassMaterial->getTechnique(0)->getPass(0), m_pkAdaptedQuad->getSubEntity(0), false);
						}
					}
				}
				else if(qId == 15)
				{
					if(m_pkAdaptedQuad)
					{
						if(m_u32AdaptedQuadQueue == 8)
						{
							_injectRenderWithPass(m_spHDRFinalMaterial->getTechnique(0)->getPass(1), m_pkAdaptedQuad->getSubEntity(0), false);
						}
						else if(m_u32AdaptedQuadQueue == 9)
						{
							_injectRenderWithPass(m_spHDRFinalMaterial->getTechnique(0)->getPass(0), m_pkAdaptedQuad->getSubEntity(0), false);
						}
					}
				}
				else if(qId == 21)
				{
					_injectRenderWithPass(m_spHDRLumInitial->getTechnique(0)->getPass(0), m_pkAdaptedQuad->getSubEntity(0), false);
				}
				else if(qId == 28 || qId == 3)
				{
					LPDIRECT3DDEVICE9 lpDevice = ((D3D9RenderSystem*)mDestRenderSystem)->getDevice();
					lpDevice->SetRenderTarget(1, m_pkDepthTarget);

					_renderQueueGroupObjects(pGroup, QueuedRenderableCollection::OM_PASS_GROUP);
				}
				else if(qId == 29)
				{
					LPDIRECT3DDEVICE9 lpDevice = ((D3D9RenderSystem*)mDestRenderSystem)->getDevice();
					lpDevice->SetRenderTarget(1, NULL);

					_renderQueueGroupObjects(pGroup, QueuedRenderableCollection::OM_PASS_GROUP);

					if(m_bHDREnable)
					{
						if((!m_spSceneRT.isNull()) && (!m_spRefractionRT.isNull()))
						{
							IDirect3DTexture9* pkScene = ((Ogre::D3D9Texture*)(m_spSceneRT.getPointer()))->getNormTexture();
							IDirect3DTexture9* pkRefraction = ((Ogre::D3D9Texture*)(m_spRefractionRT.getPointer()))->getNormTexture();

							IDirect3DSurface9* pkSceneRT(NULL);
							IDirect3DSurface9* pkRefractionRT(NULL);

							pkScene->GetSurfaceLevel(0, &pkSceneRT);
							pkRefraction->GetSurfaceLevel(0, &pkRefractionRT);

							LPDIRECT3DDEVICE9 lpDevice = ((D3D9RenderSystem*)mDestRenderSystem)->getDevice();

							HRESULT hr = lpDevice->StretchRect(pkSceneRT, NULL, pkRefractionRT, NULL, D3DTEXF_NONE);

							pkSceneRT->Release();
							pkRefractionRT->Release();
						}
					}
				}
				else
				{
					_renderQueueGroupObjects(pGroup, QueuedRenderableCollection::OM_PASS_GROUP);
				}
			}
			else
			{
				if(qId > 1 && qId < 80)
				{
					_renderQueueGroupObjects(pGroup, QueuedRenderableCollection::OM_PASS_GROUP);
				}
			}			

			// Fire queue ended event
			if (fireRenderQueueEnded(qId, 
				mIlluminationStage == IRS_RENDER_TO_TEXTURE ? 
				RenderQueueInvocation::RENDER_QUEUE_INVOCATION_SHADOWS : 
			StringUtil::BLANK))
			{
				// Someone requested we repeat this queue
				repeatQueue = true;
			}
			else
			{
				repeatQueue = false;
			}
		} while (repeatQueue);

	} // for each queue group
}
//----------------------------------------------------------------------------
void FvRenderManager::SetDeferredLightingQuad( Ogre::Entity* pkEntity )
{
	m_pkDeferredLightingQuad = pkEntity;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetDeferredLightingMaterial( const Ogre::MaterialPtr& kMat )
{
	m_spDeferredLightingMaterial = kMat;

	if(m_spManualGlobalLighting.isNull())
	{
		m_spManualGlobalLighting = Ogre::TextureManager::getSingleton().createManual(
			"ManualGlobalLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 32, 2, 0, 0,
			PF_A8R8G8B8, TU_DYNAMIC);
	}

	FV_ASSERT(!m_spManualGlobalLighting.isNull());

	Ogre::TextureUnitState *pkTexUnit =
		m_spDeferredLightingMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(4);

	FV_ASSERT(pkTexUnit);

	pkTexUnit->setTextureName(m_spManualGlobalLighting->getName());

	pkTexUnit = m_spDeferredLightingMaterial->getTechnique(0)->getPass(1)->getTextureUnitState(4);

	FV_ASSERT(pkTexUnit);

	pkTexUnit->setTextureName(m_spManualGlobalLighting->getName());

	pkTexUnit = m_spDeferredLightingMaterial->getTechnique(0)->getPass(2)->getTextureUnitState(4);

	FV_ASSERT(pkTexUnit);

	pkTexUnit->setTextureName(m_spManualGlobalLighting->getName());
}
//----------------------------------------------------------------------------
void FvRenderManager::SetOutsideRenderingListener(
	OutsideRenderingListener* pkListener)
{
	m_pkOutsideRenderingListener = pkListener;
}

void FvRenderManager::SetSunMoonLight( Ogre::Light* pkSunLight )
{
	m_pkSunMoonLight = pkSunLight;
}

void FvRenderManager::SetHDREnable( bool bEnable )
{
	m_bHDREnable = bEnable;
}

bool FvRenderManager::GetHDREnable()
{
	return m_bHDREnable;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetGlobalLightPower( float fSunMoonLightPower, float fAmbientPower )
{
	m_fSunMoonLightPower = fSunMoonLightPower;
	m_fAmbientPower = fAmbientPower;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetAdaptedQuad(Ogre::Entity* pkEntity)
{
	m_pkAdaptedQuad = pkEntity;

	if(m_spHDRAdaptedMaterial.isNull())
	{
		m_spHDRAdaptedMaterial = Ogre::MaterialManager::getSingleton().load(
			"FvHDRAdapted", "General");
	}
	
	if(m_spHDRFinalMaterial.isNull())
	{
		m_spHDRFinalMaterial = Ogre::MaterialManager::getSingleton().load(
			"FvFinalScenePassPS", "General");
	}

	if(m_spHDRBrightPassMaterial.isNull())
	{
		m_spHDRBrightPassMaterial = Ogre::MaterialManager::getSingleton().load(
			"FvBrightPassFilterPS", "General");
	}

	if(m_spHDRLumInitial.isNull())
	{
		m_spHDRLumInitial = Ogre::MaterialManager::getSingleton().load(
			"FvSampleLumInitial", "General");
	}
}
//----------------------------------------------------------------------------
void FvRenderManager::SwapAdaptedQueue()
{
	if(m_u32AdaptedQuadQueue == 8)
		m_u32AdaptedQuadQueue = 9;
	else if(m_u32AdaptedQuadQueue == 9)
		m_u32AdaptedQuadQueue = 8;
#ifdef FV_DEBUG
	else
		FV_ASSERT(!"Unknown error!");
#endif
}

void FvRenderManager::SetIlluminationStage( Ogre::SceneManager::IlluminationRenderStage eStage )
{
	mIlluminationStage = eStage;
}

Ogre::SceneManager::IlluminationRenderStage FvRenderManager::GetIlluminationStage()
{
	return mIlluminationStage;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetShadowEnable(bool bEnable)
{
	m_bShadowEnable = bEnable;
}
//----------------------------------------------------------------------------
bool FvRenderManager::GetShadowEnable()
{
	return m_bShadowEnable;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetInstanceEnable(bool bEnable)
{
	m_bInstanceEnable = bEnable;
}
//----------------------------------------------------------------------------
bool FvRenderManager::GetInstanceEnable()
{
	return m_bInstanceEnable;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetManualCastDirShadow(bool bEnable)
{
	m_bManualCastDirShadow = bEnable;
}
//----------------------------------------------------------------------------
bool FvRenderManager::GetManualCastDirShadow()
{
	return m_bManualCastDirShadow;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetPointLightEntity(Ogre::Entity* pkPointLight)
{
	m_pkPointLight = pkPointLight;
}
//----------------------------------------------------------------------------
bool FvRenderManager::VisitPointLight(FvZoneOmniLight::Params* pkPointLight,
	bool bGlobal, bool bSpecular)
{
	if(bGlobal)
	{
		if(bSpecular)
		{
			if(m_u32GlobalPointLightSpecNumber < 10)
			{

				m_apkGlobalPointLightSpec[m_u32GlobalPointLightSpecNumber++] = pkPointLight;
			}
		}
		else
		{
			if(m_u32GlobalPointLightNumber < 10)
			{

				m_apkGlobalPointLight[m_u32GlobalPointLightNumber++] = pkPointLight;
			}
		}
	}
	else
	{
		if(bSpecular)
		{
			if(m_u32PointLightSpecNumber < 200)
			{
				m_apkPointLightSpec[m_u32PointLightSpecNumber++] = pkPointLight;
			}
		}
		else
		{
			if(m_u32PointLightNumber < 200)
			{
				m_apkPointLight[m_u32PointLightNumber++] = pkPointLight;
			}
		}
	}
	
	return true;
}
//----------------------------------------------------------------------------
bool FvRenderManager::IsOnlyShadowCasters()
{
	return m_bOnlyShadowCasters;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetOutdoorExtraParams(const FvVector4& kParams)
{
	m_kOutdoorExtraParams = kParams;
}
//----------------------------------------------------------------------------
const FvVector4& FvRenderManager::GetOutdoorExtraParams()
{
	return m_kOutdoorExtraParams;
}
//----------------------------------------------------------------------------
void FvRenderManager::AddBillboard(FvBillboard* pkBillboard)
{
	m_kBillboardList.AttachBack(*pkBillboard);
}
//----------------------------------------------------------------------------
void FvRenderManager::RemoveBillboard(FvBillboard* pkBillboard)
{
	pkBillboard->Detach();
}
//----------------------------------------------------------------------------
void FvRenderManager::SetDetailBendingEnable(bool bEnable)
{
	m_bDetailBendingEnable = bEnable;
	FvInstance::SetDetailBendingEnable(bEnable);
}
//----------------------------------------------------------------------------
bool FvRenderManager::GetDetailBendingEnable()
{
	return m_bDetailBendingEnable;
}
//----------------------------------------------------------------------------
FvUInt8 FvRenderManager::GetCurrentGroupID()
{
	return m_uiCurrentGroupID;
}
//----------------------------------------------------------------------------
FvVector2& FvRenderManager::GetLodDistance(FvUInt32 u32Level)
{
	FV_ASSERT(u32Level < FV_MAX_LOD_LEVEL);
	return ms_kLodDistance[u32Level];
}
//----------------------------------------------------------------------------
void FvRenderManager::SetOcclusionMeshMaterial(const Ogre::MeshPtr& spMesh,
	const Ogre::MaterialPtr& spMaterial)
{
	m_spOcclusionAABBMesh = spMesh;
	m_spOcclusionAABBMaterial = spMaterial;
}
//----------------------------------------------------------------------------
const Ogre::MeshPtr& FvRenderManager::GetOcclusionMesh()
{
	return m_spOcclusionAABBMesh;
}
//----------------------------------------------------------------------------
const Ogre::MaterialPtr& FvRenderManager::GetOcclusionMaterial()
{
	return m_spOcclusionAABBMaterial;
}
//----------------------------------------------------------------------------
void FvRenderManager::AddOcclusionQueryAABB(FvOcclusionQueryAABB* pkQuery)
{
	OcclusionQueryList* pkList = &m_akOcclusionAABBCache[m_u32FreeListPoint];

	pkList->m_apkOcclusions[pkList->m_u32Number++] = pkQuery;
}
//----------------------------------------------------------------------------
bool FvRenderManager::IsOCStart()
{
	return ms_bIsOCStart;
}
//----------------------------------------------------------------------------
void FvRenderManager::_renderScene(Ogre::Camera* camera, Ogre::Viewport* vp,
	bool includeOverlays)
{
	Root::getSingleton()._setCurrentSceneManager(this);
	mActiveQueuedRenderableVisitor->targetSceneMgr = this;
	mAutoParamDataSource->setCurrentSceneManager(this);

	// Also set the internal viewport pointer at this point, for calls that need it
	// However don't call setViewport just yet (see below)
	mCurrentViewport = vp;

	if (isShadowTechniqueInUse())
	{
		// Prepare shadow materials
		initShadowVolumeMaterials();
	}

	// Perform a quick pre-check to see whether we should override far distance
	// When using stencil volumes we have to use infinite far distance
	// to prevent dark caps getting clipped
	if (isShadowTechniqueStencilBased() && 
		camera->getProjectionType() == PT_PERSPECTIVE &&
		camera->getFarClipDistance() != 0 && 
		mDestRenderSystem->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE) && 
		mShadowUseInfiniteFarPlane)
	{
		// infinite far distance
		camera->setFarClipDistance(0);
	}

	mCameraInProgress = camera;


	// Update controllers 
	ControllerManager::getSingleton().updateAllControllers();

	// Update the scene, only do this once per frame
	unsigned long thisFrameNumber = Root::getSingleton().getNextFrameNumber();

	static Camera* s_pkCameraCache = NULL;

	if (thisFrameNumber != mLastFrameNumber)
	{
		// Update animations
		_applySceneAnimations();
		mLastFrameNumber = thisFrameNumber;

		s_pkCameraCache = camera;
	}
	else
	{
		assert(camera);

		if(s_pkCameraCache == camera)
		{
			// Tell params about viewport
			mAutoParamDataSource->setCurrentViewport(vp);
			// Set the viewport - this is deliberately after the shadow texture update
			setViewport(vp);

			// Tell params about camera
			mAutoParamDataSource->setCurrentCamera(camera, mCameraRelativeRendering);
			// Set autoparams for finite dir light extrusion
			mAutoParamDataSource->setShadowDirLightExtrusionDistance(mShadowDirLightExtrudeDist);

			// Tell params about current ambient light
			mAutoParamDataSource->setAmbientLightColour(mAmbientLight);
			// Tell rendersystem
			mDestRenderSystem->setAmbientLight(mAmbientLight.r, mAmbientLight.g, mAmbientLight.b);

			// Tell params about render target
			mAutoParamDataSource->setCurrentRenderTarget(vp->getTarget());

			mDestRenderSystem->_beginGeometryCount();
			// Clear the viewport if required
			if (mCurrentViewport->getClearEveryFrame())
			{
				mDestRenderSystem->clearFrameBuffer(
					mCurrentViewport->getClearBuffers(), 
					mCurrentViewport->getBackgroundColour());
			}        
			// Begin the frame
			mDestRenderSystem->_beginFrame();

			// Set rasterisation mode
			mDestRenderSystem->_setPolygonMode(camera->getPolygonMode());

			// Set initial camera state
			mDestRenderSystem->_setProjectionMatrix(mCameraInProgress->getProjectionMatrixRS());

			mCachedViewMatrix = mCameraInProgress->getViewMatrix(true);

			if (mCameraRelativeRendering)
			{
				mCachedViewMatrix.setTrans(Vector3::ZERO);
				mCameraRelativePosition = mCameraInProgress->getDerivedPosition();
			}
			mDestRenderSystem->_setTextureProjectionRelativeTo(mCameraRelativeRendering, camera->getDerivedPosition());

			mDestRenderSystem->_setViewMatrix(mCachedViewMatrix);

			// Render scene content
			_renderVisibleObjects();

			// End frame
			mDestRenderSystem->_endFrame();

			// Notify camera of vis faces
			camera->_notifyRenderedFaces(mDestRenderSystem->_getFaceCount());

			// Notify camera of vis batches
			camera->_notifyRenderedBatches(mDestRenderSystem->_getBatchCount());

			return;
		}
		else
		{
			s_pkCameraCache = camera;
		}
	}

	{
		// Lock scene graph mutex, no more changes until we're ready to render
		OGRE_LOCK_MUTEX(sceneGraphMutex)

			// Update scene graph for this camera (can happen multiple times per frame)
			_updateSceneGraph(camera);

		// Auto-track nodes
		AutoTrackingSceneNodes::iterator atsni, atsniend;
		atsniend = mAutoTrackingSceneNodes.end();
		for (atsni = mAutoTrackingSceneNodes.begin(); atsni != atsniend; ++atsni)
		{
			(*atsni)->_autoTrack();
		}
		// Auto-track camera if required
		camera->_autoTrack();


		if (mIlluminationStage != IRS_RENDER_TO_TEXTURE && mFindVisibleObjects)
		{
			// Locate any lights which could be affecting the frustum
			findLightsAffectingFrustum(camera);

			// Are we using any shadows at all?
			if (isShadowTechniqueInUse() && vp->getShadowsEnabled())
			{
				// Prepare shadow textures if texture shadow based shadowing
				// technique in use
				if (isShadowTechniqueTextureBased())
				{
					// *******
					// WARNING
					// *******
					// This call will result in re-entrant calls to this method
					// therefore anything which comes before this is NOT 
					// guaranteed persistent. Make sure that anything which 
					// MUST be specific to this camera / target is done 
					// AFTER THIS POINT
					prepareShadowTextures(camera, vp);
					// reset the cameras & viewport because of the re-entrant call
					mCameraInProgress = camera;
					mCurrentViewport = vp;
				}
			}
		}

		// Invert vertex winding?
		if (camera->isReflected() || 
			FV_MASK_HAS_ANY(m_uiVisitFlag,FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
		{
			mDestRenderSystem->setInvertVertexWinding(true);
		}
		else
		{
			mDestRenderSystem->setInvertVertexWinding(false);
		}

		// Tell params about viewport
		mAutoParamDataSource->setCurrentViewport(vp);
		// Set the viewport - this is deliberately after the shadow texture update
		setViewport(vp);

		// Tell params about camera
		mAutoParamDataSource->setCurrentCamera(camera, mCameraRelativeRendering);
		// Set autoparams for finite dir light extrusion
		mAutoParamDataSource->setShadowDirLightExtrusionDistance(mShadowDirLightExtrudeDist);

		// Tell params about current ambient light
		mAutoParamDataSource->setAmbientLightColour(mAmbientLight);
		// Tell rendersystem
		mDestRenderSystem->setAmbientLight(mAmbientLight.r, mAmbientLight.g, mAmbientLight.b);

		// Tell params about render target
		mAutoParamDataSource->setCurrentRenderTarget(vp->getTarget());


		// Set camera window clipping planes (if any)
		if (mDestRenderSystem->getCapabilities()->hasCapability(RSC_USER_CLIP_PLANES))
		{
			mDestRenderSystem->resetClipPlanes();
			if (camera->isWindowSet())  
			{
				mDestRenderSystem->setClipPlanes(camera->getWindowPlanes());
			}
			else if (camera->isReflected() || 
				FV_MASK_HAS_ANY(m_uiVisitFlag,FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
			{
				static PlaneList clipPlanes(1);
				clipPlanes[0] = camera->getReflectionPlane();
				clipPlanes[0].d += 0.5;
				mDestRenderSystem->setClipPlanes(clipPlanes);
			}
		}

		// Prepare render queue for receiving new objects
		prepareRenderQueue();

		if (mFindVisibleObjects)
		{
			// Assemble an AAB on the fly which contains the scene elements visible
			// by the camera.
			CamVisibleObjectsMap::iterator camVisObjIt = mCamVisibleObjectsMap.find( camera );

			assert (camVisObjIt != mCamVisibleObjectsMap.end() &&
				"Should never fail to find a visible object bound for a camera, "
				"did you override SceneManager::createCamera or something?");

			// reset the bounds
			camVisObjIt->second.reset();

			// Parse the scene and tag visibles
			firePreFindVisibleObjects(vp);
			_findVisibleObjects(camera, &(camVisObjIt->second),
				mIlluminationStage == IRS_RENDER_TO_TEXTURE? true : false);
			firePostFindVisibleObjects(vp);

			mAutoParamDataSource->setMainCamBoundsInfo(&(camVisObjIt->second));
		}
		// Add overlays, if viewport deems it
		if (vp->getOverlaysEnabled() && mIlluminationStage != IRS_RENDER_TO_TEXTURE)
		{
			OverlayManager::getSingleton()._queueOverlaysForRendering(camera, getRenderQueue(), vp);
		}
		// Queue skies, if viewport seems it
		if (vp->getSkiesEnabled() && mFindVisibleObjects && mIlluminationStage != IRS_RENDER_TO_TEXTURE)
		{
			_queueSkiesForRendering(camera);
		}
	} // end lock on scene graph mutex

	mDestRenderSystem->_beginGeometryCount();
	// Clear the viewport if required
	if (mCurrentViewport->getClearEveryFrame())
	{
		mDestRenderSystem->clearFrameBuffer(
			mCurrentViewport->getClearBuffers(), 
			mCurrentViewport->getBackgroundColour());
	}        
	// Begin the frame
	mDestRenderSystem->_beginFrame();

	if(ms_bIsOCStart)
	{
		OcclusionQueryList* pkList = &m_akOcclusionAABBCache[m_u32FreeListPoint];

		for(FvUInt32 i(0); i < pkList->m_u32Number; ++i)
		{
			pkList->m_apkOcclusions[i]->GetResult();
		}
		pkList->m_u32Number = 0;
	}

	// Set rasterisation mode
	mDestRenderSystem->_setPolygonMode(camera->getPolygonMode());

	// Set initial camera state
	mDestRenderSystem->_setProjectionMatrix(mCameraInProgress->getProjectionMatrixRS());

	mCachedViewMatrix = mCameraInProgress->getViewMatrix(true);

	if (mCameraRelativeRendering)
	{
		mCachedViewMatrix.setTrans(Vector3::ZERO);
		mCameraRelativePosition = mCameraInProgress->getDerivedPosition();
	}
	mDestRenderSystem->_setTextureProjectionRelativeTo(mCameraRelativeRendering, camera->getDerivedPosition());

	mDestRenderSystem->_setViewMatrix(mCachedViewMatrix);

	// Render scene content
	_renderVisibleObjects();

	// End frame
	mDestRenderSystem->_endFrame();

	// Notify camera of vis faces
	camera->_notifyRenderedFaces(mDestRenderSystem->_getFaceCount());

	// Notify camera of vis batches
	camera->_notifyRenderedBatches(mDestRenderSystem->_getBatchCount());


}
//----------------------------------------------------------------------------
void FvRenderManager::SetShadowMap(FvUInt32 u32Level,
	IDirect3DTexture9* pkTexture)
{
	FV_ASSERT(u32Level < 4);
	m_apkShadowMaps[u32Level] = pkTexture;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetShadowReceiverPass(FvUInt32 u32Pass)
{
	m_u32ShadowReceiverPass = u32Pass;
}
//----------------------------------------------------------------------------
void FvRenderManager::_RenderSingleObject(Renderable* rend, const Pass* pass,
	bool lightScissoringClipping, bool doLightIteration,
	const LightList* manualLightList)
{
    unsigned short numMatrices;
    static RenderOperation ro;

    // Set up rendering operation
    // I know, I know, const_cast is nasty but otherwise it requires all internal
    // state of the Renderable assigned to the rop to be mutable
    const_cast<Renderable*>(rend)->getRenderOperation(ro);
    ro.srcRenderable = rend;

    // Set world transformation
    numMatrices = rend->getNumWorldTransforms();
	
	if (numMatrices > 0)
	{
	    rend->getWorldTransforms(mTempXform);

		if (mCameraRelativeRendering && !rend->getUseIdentityView())
		{
			for (unsigned short i = 0; i < numMatrices; ++i)
			{
				mTempXform[i].setTrans(mTempXform[i].getTrans() - mCameraRelativePosition);
			}
		}
		
		if (numMatrices > 1)
		{
			mDestRenderSystem->_setWorldMatrices(mTempXform, numMatrices);
		}
		else
		{
			mDestRenderSystem->_setWorldMatrix(*mTempXform);
		}
	}
    // Issue view / projection changes if any
    useRenderableViewProjMode(rend);

    if (!mSuppressRenderStateChanges)
    {
        bool passSurfaceAndLightParams = true;

        if (pass->isProgrammable())
        {
            // Tell auto params object about the renderable change
            mAutoParamDataSource->setCurrentRenderable(rend);
            // Tell auto params object about the world matrices, eliminated query from renderable again
            mAutoParamDataSource->setWorldMatrices(mTempXform, numMatrices);
            pass->_updateAutoParamsNoLights(mAutoParamDataSource);
            if (pass->hasVertexProgram())
            {
                passSurfaceAndLightParams = pass->getVertexProgram()->getPassSurfaceAndLightStates();
            }
        }

        // Reissue any texture gen settings which are dependent on view matrix
        Pass::ConstTextureUnitStateIterator texIter =  pass->getTextureUnitStateIterator();
        size_t unit = 0;
        while(texIter.hasMoreElements())
        {
            TextureUnitState* pTex = texIter.getNext();
            if (pTex->hasViewRelativeTextureCoordinateGeneration())
            {
                mDestRenderSystem->_setTextureUnitSettings(unit, *pTex);
            }
            ++unit;
        }

        // Sort out normalisation
		// Assume first world matrix representative - shaders that use multiple
		// matrices should control renormalisation themselves
		if ((pass->getNormaliseNormals() || mNormaliseNormalsOnScale)
			&& mTempXform[0].hasScale())
			mDestRenderSystem->setNormaliseNormals(true);
		else
			mDestRenderSystem->setNormaliseNormals(false);

		// Sort out negative scaling
		// Assume first world matrix representative 
		if (mFlipCullingOnNegativeScale)
		{
			CullingMode cullMode = mPassCullingMode;

			if (mTempXform[0].hasNegativeScale())
			{
				switch(mPassCullingMode)
				{
				case CULL_CLOCKWISE:
					cullMode = CULL_ANTICLOCKWISE;
					break;
				case CULL_ANTICLOCKWISE:
					cullMode = CULL_CLOCKWISE;
					break;
				};
			}

			// this also copes with returning from negative scale in previous render op
			// for same pass
			if (cullMode != mDestRenderSystem->_getCullingMode())
				mDestRenderSystem->_setCullingMode(cullMode);
		}

		// Set up the solid / wireframe override
		// Precedence is Camera, Object, Material
		// Camera might not override object if not overrideable
		PolygonMode reqMode = pass->getPolygonMode();
		if (pass->getPolygonModeOverrideable() && rend->getPolygonModeOverrideable())
		{
            PolygonMode camPolyMode = mCameraInProgress->getPolygonMode();
			// check camera detial only when render detail is overridable
			if (reqMode > camPolyMode)
			{
				// only downgrade detail; if cam says wireframe we don't go up to solid
				reqMode = camPolyMode;
			}
		}
		mDestRenderSystem->_setPolygonMode(reqMode);

		if (doLightIteration)
		{
            // Create local light list for faster light iteration setup
            static LightList localLightList;


			// Here's where we issue the rendering operation to the render system
			// Note that we may do this once per light, therefore it's in a loop
			// and the light parameters are updated once per traversal through the
			// loop
			const LightList& rendLightList = rend->getLights();

			bool iteratePerLight = pass->getIteratePerLight();

			// deliberately unsigned in case start light exceeds number of lights
			// in which case this pass would be skipped
			int lightsLeft = 1;
			if (iteratePerLight)
			{
				lightsLeft = static_cast<int>(rendLightList.size()) - pass->getStartLight();
				// Don't allow total light count for all iterations to exceed max per pass
				if (lightsLeft > static_cast<int>(pass->getMaxSimultaneousLights()))
				{
					lightsLeft = static_cast<int>(pass->getMaxSimultaneousLights());
				}
			}


			const LightList* pLightListToUse;
			// Start counting from the start light
			size_t lightIndex = pass->getStartLight();
			size_t depthInc = 0;

			while (lightsLeft > 0)
			{
				// Determine light list to use
				if (iteratePerLight)
				{
					// Starting shadow texture index.
					size_t shadowTexIndex = mShadowTextures.size();
					if (mShadowTextureIndexLightList.size() > lightIndex)
						shadowTexIndex = mShadowTextureIndexLightList[lightIndex];

					localLightList.resize(pass->getLightCountPerIteration());

					LightList::iterator destit = localLightList.begin();
					unsigned short numShadowTextureLights = 0;
					for (; destit != localLightList.end() 
							&& lightIndex < rendLightList.size(); 
						++lightIndex, --lightsLeft)
					{
						Light* currLight = rendLightList[lightIndex];

						// Check whether we need to filter this one out
						if (pass->getRunOnlyForOneLightType() && 
							pass->getOnlyLightType() != currLight->getType())
						{
							// Skip
							// Also skip shadow texture(s)
							if (isShadowTechniqueTextureBased())
							{
								shadowTexIndex += mShadowTextureCountPerType[currLight->getType()];
							}
							continue;
						}

						*destit++ = currLight;

						// potentially need to update content_type shadow texunit
						// corresponding to this light
						if (isShadowTechniqueTextureBased())
						{
							size_t textureCountPerLight = mShadowTextureCountPerType[currLight->getType()];
							for (size_t j = 0; j < textureCountPerLight && shadowTexIndex < mShadowTextures.size(); ++j)
							{
								// link the numShadowTextureLights'th shadow texture unit
								unsigned short tuindex = 
									pass->_getTextureUnitWithContentTypeIndex(
									TextureUnitState::CONTENT_SHADOW, numShadowTextureLights);
								if (tuindex > pass->getNumTextureUnitStates()) break;

								// I know, nasty const_cast
								TextureUnitState* tu = 
									const_cast<TextureUnitState*>(
										pass->getTextureUnitState(tuindex));
								const TexturePtr& shadowTex = mShadowTextures[shadowTexIndex];
								tu->_setTexturePtr(shadowTex);
								Camera *cam = shadowTex->getBuffer()->getRenderTarget()->getViewport(0)->getCamera();
								tu->setProjectiveTexturing(!pass->hasVertexProgram(), cam);
								mAutoParamDataSource->setTextureProjector(cam, numShadowTextureLights);
								++numShadowTextureLights;
								++shadowTexIndex;
								// Have to set TU on rendersystem right now, although
								// autoparams will be set later
								mDestRenderSystem->_setTextureUnitSettings(tuindex, *tu);
							}
						}



					}
					// Did we run out of lights before slots? e.g. 5 lights, 2 per iteration
					if (destit != localLightList.end())
					{
						localLightList.erase(destit, localLightList.end());
						lightsLeft = 0;
					}
					pLightListToUse = &localLightList;

					// deal with the case where we found no lights
					// since this is light iteration, we shouldn't render at all
					if (pLightListToUse->empty())
						return;

				}
				else // !iterate per light
				{
					// Use complete light list potentially adjusted by start light
					if (pass->getStartLight() || pass->getMaxSimultaneousLights() != OGRE_MAX_SIMULTANEOUS_LIGHTS)
					{
						// out of lights?
						// skip manual 2nd lighting passes onwards if we run out of lights, but never the first one
						if (pass->getStartLight() > 0 &&
							pass->getStartLight() >= rendLightList.size())
						{
							lightsLeft = 0;
							break;
						}
						else
						{
							localLightList.clear();
							LightList::const_iterator copyStart = rendLightList.begin();
							std::advance(copyStart, pass->getStartLight());
							LightList::const_iterator copyEnd = copyStart;
							// Clamp lights to copy to avoid overrunning the end of the list
							size_t lightsToCopy = std::min(
								static_cast<size_t>(pass->getMaxSimultaneousLights()), 
								rendLightList.size() - pass->getStartLight());
							std::advance(copyEnd, lightsToCopy);
							localLightList.insert(localLightList.begin(), 
								copyStart, copyEnd);
							pLightListToUse = &localLightList;
						}
					}
					else
					{
						pLightListToUse = &rendLightList;
					}
					lightsLeft = 0;
				}


				// Do we need to update GPU program parameters?
				if (pass->isProgrammable())
				{
					// Update any automatic gpu params for lights
					// Other bits of information will have to be looked up
					mAutoParamDataSource->setCurrentLightList(pLightListToUse);
					pass->_updateAutoParamsLightsOnly(mAutoParamDataSource);
					// NOTE: We MUST bind parameters AFTER updating the autos

					if (pass->hasVertexProgram())
					{
						mDestRenderSystem->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, 
							pass->getVertexProgramParameters());
					}
					if (pass->hasGeometryProgram())
					{
						mDestRenderSystem->bindGpuProgramParameters(GPT_GEOMETRY_PROGRAM,
							pass->getGeometryProgramParameters());
					}
					if (pass->hasFragmentProgram())
					{
						mDestRenderSystem->bindGpuProgramParameters(GPT_FRAGMENT_PROGRAM, 
							pass->getFragmentProgramParameters());
					}
				}
				// Do we need to update light states? 
				// Only do this if fixed-function vertex lighting applies
				if (pass->getLightingEnabled() && passSurfaceAndLightParams)
				{
					mDestRenderSystem->_useLights(*pLightListToUse, pass->getMaxSimultaneousLights());
				}
				// optional light scissoring & clipping
				ClipResult scissored = CLIPPED_NONE;
				ClipResult clipped = CLIPPED_NONE;
				if (lightScissoringClipping && 
					(pass->getLightScissoringEnabled() || pass->getLightClipPlanesEnabled()))
				{
					// if there's no lights hitting the scene, then we might as 
					// well stop since clipping cannot include anything
					if (pLightListToUse->empty())
						continue;

					if (pass->getLightScissoringEnabled())
						scissored = buildAndSetScissor(*pLightListToUse, mCameraInProgress);
				
					if (pass->getLightClipPlanesEnabled())
						clipped = buildAndSetLightClip(*pLightListToUse);

					if (scissored == CLIPPED_ALL || clipped == CLIPPED_ALL)
						continue;
				}
				// issue the render op		
				// nfz: check for gpu_multipass
				mDestRenderSystem->setCurrentPassIterationCount(pass->getPassIterationCount());
				// We might need to update the depth bias each iteration
				if (pass->getIterationDepthBias() != 0.0f)
				{
					float depthBiasBase = pass->getDepthBiasConstant() + 
						pass->getIterationDepthBias() * depthInc;
					// depthInc deals with light iteration 
					
					// Note that we have to set the depth bias here even if the depthInc
					// is zero (in which case you would think there is no change from
					// what was set in _setPass(). The reason is that if there are
					// multiple Renderables with this Pass, we won't go through _setPass
					// again at the start of the iteration for the next Renderable
					// because of Pass state grouping. So set it always

					// Set modified depth bias right away
					mDestRenderSystem->_setDepthBias(depthBiasBase, pass->getDepthBiasSlopeScale());

					// Set to increment internally too if rendersystem iterates
					mDestRenderSystem->setDeriveDepthBias(true, 
						depthBiasBase, pass->getIterationDepthBias(), 
						pass->getDepthBiasSlopeScale());
				}
				else
				{
					mDestRenderSystem->setDeriveDepthBias(false);
				}
				depthInc += pass->getPassIterationCount();

				
			} // possibly iterate per light
		}
		else // no automatic light processing
		{
			// Even if manually driving lights, check light type passes
			bool skipBecauseOfLightType = false;
			if (pass->getRunOnlyForOneLightType())
			{
				if (!manualLightList ||
					(manualLightList->size() == 1 && 
					manualLightList->at(0)->getType() != pass->getOnlyLightType())) 
				{
					skipBecauseOfLightType = true;
				}
			}

			if (!skipBecauseOfLightType)
			{
				// Do we need to update GPU program parameters?
				if (pass->isProgrammable())
				{
					// Do we have a manual light list?
					if (manualLightList)
					{
						// Update any automatic gpu params for lights
						mAutoParamDataSource->setCurrentLightList(manualLightList);
						pass->_updateAutoParamsLightsOnly(mAutoParamDataSource);
					}

					if (pass->hasVertexProgram())
					{
						mDestRenderSystem->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, 
							pass->getVertexProgramParameters());
					}
					if (pass->hasGeometryProgram())
					{
						mDestRenderSystem->bindGpuProgramParameters(GPT_GEOMETRY_PROGRAM,
							pass->getGeometryProgramParameters());
					}
					if (pass->hasFragmentProgram())
					{
						mDestRenderSystem->bindGpuProgramParameters(GPT_FRAGMENT_PROGRAM, 
							pass->getFragmentProgramParameters());
					}
				}

				// Use manual lights if present, and not using vertex programs that don't use fixed pipeline
				if (manualLightList && 
					pass->getLightingEnabled() && passSurfaceAndLightParams)
				{
					mDestRenderSystem->_useLights(*manualLightList, pass->getMaxSimultaneousLights());
				}

				// optional light scissoring
				ClipResult scissored = CLIPPED_NONE;
				ClipResult clipped = CLIPPED_NONE;
				if (lightScissoringClipping && manualLightList && pass->getLightScissoringEnabled())
				{
					scissored = buildAndSetScissor(*manualLightList, mCameraInProgress);
				}
				if (lightScissoringClipping && manualLightList && pass->getLightClipPlanesEnabled())
				{
					clipped = buildAndSetLightClip(*manualLightList);
				}
	
				// don't bother rendering if clipped / scissored entirely
				if (scissored != CLIPPED_ALL && clipped != CLIPPED_ALL)
				{
					// issue the render op		
					// nfz: set up multipass rendering
					
				}
				
			} // !skipBecauseOfLightType
		}

	}
}
//----------------------------------------------------------------------------
float* FvRenderManager::GetRotMatrixBuffer()
{
	return m_afShadowPCFParams;
}
//----------------------------------------------------------------------------
float* FvRenderManager::GetShadowMapParams()
{
	return m_afShadowMapParams;
}
//----------------------------------------------------------------------------
float* FvRenderManager::GetTerrainShadowMatrix()
{
	return m_afTerrainShadowMatrix;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetDepthTarget(IDirect3DSurface9* pkTarget)
{
	if(!pkTarget)
	{
		if(m_pkDepthTarget) m_pkDepthTarget->Release();
	}
	m_pkDepthTarget = pkTarget;
}
//----------------------------------------------------------------------------
IDirect3DSurface9* FvRenderManager::GetDepthTarget()
{
	return m_pkDepthTarget;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetPointLightMaterial(const Ogre::MaterialPtr& kMat)
{
	m_spPointLightMaterial = kMat;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetSceneRT(const Ogre::TexturePtr& spSceneRT)
{
	//FV_ASSERT(!spSceneRT.isNull());
	m_spSceneRT = spSceneRT;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetRefractionRT(const Ogre::TexturePtr& spRefractionRT)
{
	//FV_ASSERT(!spRefractionRT.isNull());
	m_spRefractionRT = spRefractionRT;
}
//----------------------------------------------------------------------------
void FvRenderManager::SetDepthMap(const FvString& kDepthMap)
{
	m_kDepthMap = kDepthMap;
}
//----------------------------------------------------------------------------
void FvRenderManager::ResetOC()
{
	m_akOcclusionAABBCache[0].m_u32Number = 0;
	m_akOcclusionAABBCache[1].m_u32Number = 0;
}
//----------------------------------------------------------------------------
const Ogre::String FvRenderManagerFactory::FACTORY_TYPE_NAME = "FvRenderManager";
Ogre::SceneManager *FvRenderManagerFactory::createInstance(const Ogre::String &kInstanceName)
{
	return OGRE_NEW FvRenderManager;
}

void FvRenderManagerFactory::destroyInstance(Ogre::SceneManager *pkInstance)
{
	OGRE_DELETE pkInstance;
}

void FvRenderManagerFactory::initMetaData() const
{
	mMetaData.typeName = FACTORY_TYPE_NAME;
	mMetaData.description = "FutureVision RenderManager";
	mMetaData.sceneTypeMask = 0xFFFF;
	mMetaData.worldGeometrySupported = false;
}