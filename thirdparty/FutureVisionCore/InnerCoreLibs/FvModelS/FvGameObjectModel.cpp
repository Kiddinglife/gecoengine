#include "FvGameObjectModel.h"
#include "FvGameObjectModelSerializer.h"

#include <FvMatrix.h>
#include <FvXMLSection.h>

#include <Ogre.h>
#include <OgrePrerequisites.h>
#include <OgreIteratorWrappers.h>
#include <FvRenderManager.h>

#ifndef FV_SERVER
FvInt32 FvGameObjectModel::ms_uiGameObjectModelIdentify = 0;
#endif // !FV_SERVER

FvGameObjectModel::FvGameObjectModel()
#ifndef FV_SERVER

: m_bCastShadows(false),
m_u32CurrentLodLevel(0),
m_fVolume(0),
m_eState(FvAnimatedCharacterNodePrivateData::NORMAL)
{
	ms_uiGameObjectModelIdentify++;
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	for(int i = 0; i < FV_MAX_LOD_LEVEL; i++)
	{
		char acBuffer[32];
		sprintf_s(acBuffer, "FvGameObjectModelNode_%I32x_LOD%d",FvInt32(ms_uiGameObjectModelIdentify),i);
		m_akNodes[i].m_pkNode = pkSceneManager->createSceneNode(acBuffer);
	}
}
#else // !FV_SERVER
{}


#endif // FV_SERVER

#ifndef FV_SERVER
static void DestroyModelNode(Ogre::SceneNode *pkNode)
{
	FV_ASSERT(Ogre::Root::getSingleton()._getCurrentSceneManager());
	Ogre::Node::ChildNodeIterator kObjIt = pkNode->getChildIterator();
	while (kObjIt.hasMoreElements())
		DestroyModelNode(((Ogre::SceneNode*)kObjIt.getNext()));

	Ogre::SceneNode::ObjectIterator kMovObjIt = pkNode->getAttachedObjectIterator();
	while (kMovObjIt.hasMoreElements())
	{
		Ogre::MovableObject *pkObj = kMovObjIt.getNext();
		Ogre::Root::getSingleton()._getCurrentSceneManager()->destroyMovableObject(pkObj);
	}
	pkNode->detachAllObjects();

	Ogre::Root::getSingleton()._getCurrentSceneManager()->destroySceneNode(pkNode);
}
#endif // !FV_SERVER

FvGameObjectModel::~FvGameObjectModel()
{
	if(!m_spBSPTree.isNull())
	{
		FvBSPProxyManager::getSingleton().remove(m_spBSPTree->getHandle());
		m_spBSPTree.setNull();
	}
#ifndef FV_SERVER
	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		if(m_akNodes[i].m_pkNode)
			DestroyModelNode(m_akNodes[i].m_pkNode);
	}
#endif // !FV_SERVER
}

bool FvGameObjectModel::Load(const FvString &kFileName, const FvString &kGroupName,
	const FvString& kExtraName, const FvMatrix& kMatrix, bool bCastShadows)
{
	bool bExist = Ogre::ResourceGroupManager::getSingleton().
		resourceExists(kGroupName,kFileName);

	if(!bExist)
		return false;

	m_kFileName = kFileName;
	m_kGroupName = kGroupName;
	m_kExtaName = kExtraName;

	Ogre::DataStreamPtr spStream = Ogre::ResourceGroupManager::getSingleton().
		openResource(kFileName,kGroupName);

	FvGameObjectModelSerializer kSerializer;

	bool bRes = kSerializer.ImportModel(spStream,this);

#ifndef FV_SERVER
	m_bCastShadows = bCastShadows;
	/*Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);*/

	/*m_pkNode = pkSceneManager->createSceneNode(m_kNodeName);
	if(!m_pkNode)
		return false;*/

	this->Transform(kMatrix);

	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		if(m_akNodes[i].m_pkNode)
		{
			m_akNodes[i].m_pkNode->_update(true,true);
		}
	}

	const AxisAlignedBox& kWorldAABB = m_akNodes[0].m_pkNode->_getWorldAABB();

	Vector3 kSize = kWorldAABB.getSize();

	m_fVolume = 1.0f / FvSqrtf(kSize.x * kSize.y * kSize.z);

	//this->Transform(kMatrix);
#endif // !FV_SERVER

	return bRes;
}
#ifndef FV_SERVER
void FvGameObjectModel::Transform(const FvMatrix &kMatrix)
{
	Vector3 kPosition,kScale;
	FvQuaternion kRotation;
	FvMatrixDecomposeWorld4f((float*)&kRotation,(float*)&kScale,(float*)&kPosition,
		(float*)&kMatrix);

	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		if(m_akNodes[i].m_pkNode)
		{
			m_akNodes[i].m_pkNode->setPosition(kPosition);
			m_akNodes[i].m_pkNode->setScale(kScale);
			m_akNodes[i].m_pkNode->setOrientation(Quaternion(kRotation.w,kRotation.x,kRotation.y,kRotation.z));
			m_akNodes[i].m_pkNode->_update(false, false);
		}
	}

	/*if(m_pkNode == NULL)
		return;

	Vector3 kPosition,kScale;
	FvQuaternion kRotation;
	FvMatrixDecomposeWorld4f((float*)&kRotation,(float*)&kScale,(float*)&kPosition,
		(float*)&kMatrix);
	m_pkNode->setPosition(kPosition);
	m_pkNode->setScale(kScale);
	m_pkNode->setOrientation(Quaternion(kRotation.w,kRotation.x,kRotation.y,kRotation.z));*/
}

const FvGameObjectModel::Node& FvGameObjectModel::GetModleNode()
{
	return m_akNodes[m_u32CurrentLodLevel];
}

FvUInt32 FvGameObjectModel::GetCurrentLodLevel()
{
	return m_u32CurrentLodLevel;
}

void FvGameObjectModel::SetCurrentLodLevel(FvUInt32 u32CurrentLodLevel)
{
	FV_ASSERT(u32CurrentLodLevel < FV_MAX_LOD_LEVEL);
	FV_ASSERT(m_akNodes[0].m_pkNode);

	while(!m_akNodes[u32CurrentLodLevel].m_kEntityList.size())
	{
		--u32CurrentLodLevel;
	}

	m_u32CurrentLodLevel = u32CurrentLodLevel;
}

float FvGameObjectModel::GetVolume()
{
	return m_fVolume;
}

void FvGameObjectModel::SetRenderState(FvAnimatedCharacterNodePrivateData::RenderState eState)
{
	if(m_eState != eState)
	{
		FvUInt32 u32Group;

		switch(eState)
		{
		case FvAnimatedCharacterNodePrivateData::COLOR_EFFECT:
			u32Group = FvRenderManager::COLOR_EFFECT_GROUP_ID;
			break;
		case FvAnimatedCharacterNodePrivateData::FADE_OUT:
			u32Group = FvRenderManager::FADE_OUT_GROUP_ID;
			break;
		case FvAnimatedCharacterNodePrivateData::BLEND:
			u32Group = FvRenderManager::BLEND_GROUP_ID;
			break;
		case FvAnimatedCharacterNodePrivateData::MESH_UI:
			u32Group = 90;
			break;
		case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT:
			u32Group = FvRenderManager::EDGE_HIGH_LIGHT;
			break;
		case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT_COLOR_EFFECT:
			u32Group = FvRenderManager::EDGE_HIGH_LIGHT_COLOR_EFFECT;
			break;
		case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT_BLEND:
			u32Group = FvRenderManager::EDGE_HIGH_LIGHT_BLEND;
			break;
		default:
			u32Group = Ogre::RENDER_QUEUE_MAX;
			break;
		}
		
		for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
		{
			for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
			{
				m_akNodes[i].m_kEntityList[j]->setRenderQueueGroup(u32Group);
			}
		}

		m_eState = eState;
	}
}

FvAnimatedCharacterNodePrivateData::RenderState FvGameObjectModel::GetRenderState()
{
	return m_eState;
}

void FvGameObjectModel::SetAddColor(float r, float g, float b)
{
	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
		{
			Entity* pkEntity = m_akNodes[i].m_kEntityList[j];

			for(FvUInt32 k(0); k < pkEntity->getNumSubEntities(); ++k)
			{
				pkEntity->getSubEntity(k)->setCustomParameter(2, Ogre::Vector4(r, g, b, 1));
			}
		}
	}
}

void FvGameObjectModel::SetMulColor(float r, float g, float b)
{
	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
		{
			Entity* pkEntity = m_akNodes[i].m_kEntityList[j];

			for(FvUInt32 k(0); k < pkEntity->getNumSubEntities(); ++k)
			{
				pkEntity->getSubEntity(k)->setCustomParameter(1, Ogre::Vector4(r - 1, g - 1, b - 1, 0));
			}
		}
	}
}

void FvGameObjectModel::SetBlendAlpha(float a, float x, float y, float fRefraction)
{
	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
		{
			Entity* pkEntity = m_akNodes[i].m_kEntityList[j];

			for(FvUInt32 k(0); k < pkEntity->getNumSubEntities(); ++k)
			{
				pkEntity->getSubEntity(k)->setCustomParameter(7, Ogre::Vector4(fRefraction, x, -y, a));
			}
		}
	}
}

void FvGameObjectModel::SetFadeOutAlpha(float a)
{
	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
		{
			Entity* pkEntity = m_akNodes[i].m_kEntityList[j];

			for(FvUInt32 k(0); k < pkEntity->getNumSubEntities(); ++k)
			{
				pkEntity->getSubEntity(k)->setCustomParameter(3, Ogre::Vector4(a, a, a, a));
			}
		}
	}
}

void FvGameObjectModel::SetEdgeHighLight(float r, float g, float b, float a)
{
	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
		{
			Entity* pkEntity = m_akNodes[i].m_kEntityList[j];

			for(FvUInt32 k(0); k < pkEntity->getNumSubEntities(); ++k)
			{
				pkEntity->getSubEntity(k)->setCustomParameter(6, Ogre::Vector4(r, g, b, a));
			}
		}
	}
}

void FvGameObjectModel::SetEdgeHighLightWidth(float fWidth)
{
	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
		{
			Entity* pkEntity = m_akNodes[i].m_kEntityList[j];

			for(FvUInt32 k(0); k < pkEntity->getNumSubEntities(); ++k)
			{
				pkEntity->getSubEntity(k)->setCustomParameter(8, Ogre::Vector4(fWidth, fWidth, fWidth, fWidth));
			}
		}
	}
}

void FvGameObjectModel::UpdateRenderQueue(Ogre::RenderQueue* pkQueue, const Ogre::Vector4& kID)
{
	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
		{
			Entity* pkEntity = m_akNodes[i].m_kEntityList[j];

			for(FvUInt32 k(0); k < pkEntity->getNumSubEntities(); ++k)
			{
				pkEntity->getSubEntity(k)->setCustomParameter(4, kID);
			}

			pkEntity->_updateRenderQueue(pkQueue);
		}
	}
}

#endif // !FV_SERVER
