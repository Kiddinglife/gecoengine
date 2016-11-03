#include "FvAnimationModel.h"
#include "FvAnimationModelSerializer.h"

#include <Ogre.h>
#include <OgrePrerequisites.h>
#include <FvRenderManager.h>
FvInt32 FvAnimationModel::ms_uiAnimationModelIdentify = 0;

FvAnimationModel::FvAnimationModel( const FvString& kZoneName )
:m_bCastShadows(false)
,m_u32CurrentLodLevel(0)
,m_fVolume(0)
,m_kZoneName(kZoneName)
,m_sDefaultAnimName("")
,m_eState(FvAnimatedCharacterNodePrivateData::NORMAL)
{
	ms_uiAnimationModelIdentify++;
	Ogre::SceneManager* pkSceneManager = Ogre::Root::getSingleton()._getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	for (int i = 0; i < FV_MAX_LOD_LEVEL; i++)
	{
		char acBuffer[32];
		sprintf_s(acBuffer,"FvAnimationModelNode_%I32x_LOD%d", FvInt32(ms_uiAnimationModelIdentify),i);
		m_akNodes[i].m_pkNode = pkSceneManager->createSceneNode(acBuffer);
	}
}

FvAnimationModel::~FvAnimationModel()
{
	if (!m_spBSPTree.isNull())
	{
		FvBSPProxyManager::getSingleton().remove(m_spBSPTree->getHandle());
		m_spBSPTree.setNull();

	}

	for (FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		if (m_akNodes[i].m_pkNode)
		{
			DestroyModelNode(m_akNodes[i].m_pkNode);
		}
	}
}

void FvAnimationModel::DestroyModelNode(Ogre::SceneNode *pkNode)
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

bool FvAnimationModel::Load( const FvString& kFileName, const FvString& kGroupName, const FvString& kExtraName, const FvMatrix& kMatrix, bool bCastShadows /*= false*/ )
{
	bool bExist = Ogre::ResourceGroupManager::getSingleton().resourceExists(kGroupName,kFileName);
	if (!bExist)
		return false;

	m_kFileName = kFileName;
	m_kGroupName = kGroupName;
	m_kExtraName = kExtraName;

	Ogre::DataStreamPtr spStream = Ogre::ResourceGroupManager::getSingleton().openResource(kFileName,kGroupName);
	FvAnimationModelSerializer kSerializer;
	bool bRes = kSerializer.ImportModel(spStream, this);

	m_bCastShadows = bCastShadows;
	Transform(kMatrix);
	// Ôö¼Ó¶¯»­
	bool hasANnim = !(m_kModelAnimations.empty());
	if (hasANnim)
	{
		FvStringHashMap<AnimationState*>::iterator iter = m_kModelAnimations.begin();
		m_sDefaultAnimName = iter->first;
	}else
	{
		for (FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
		{
			if (m_akNodes[i].m_pkNode)
			{
				m_akNodes[i].m_pkNode->_update(true,true);
				for(FvUInt32 j(0); j < m_akNodes[i].m_kEntityList.size(); ++j)
				{
					m_akNodes[i].m_kEntityList[j]->setRenderQueueGroup(FvRenderManager::BLEND_GROUP_ID_ANIM);
				}
			}
		}
	}

	const AxisAlignedBox& kWorldAABB = m_akNodes[0].m_pkNode->_getWorldAABB();
	Vector3 kSize = kWorldAABB.getSize();
	m_fVolume = 1.0f / FvSqrtf(kSize.x * kSize.y * kSize.z);


	return bRes;

}

void FvAnimationModel::Transform( const FvMatrix &kMatrix )
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
}


const FvAnimationModel::Node& FvAnimationModel::GetModelNode()
{
	return m_akNodes[m_u32CurrentLodLevel];
}

FvString FvAnimationModel::GetDefaultAnimName()
{
	return m_sDefaultAnimName;
}

AnimationState* FvAnimationModel::GetDefaultAnim()
{
	if (m_sDefaultAnimName != "")
	{
		return this->GetAnimation(m_sDefaultAnimName);
	}
	return NULL;
}

FvUInt32 FvAnimationModel::GetCurrentLodLevel()
{
	return m_u32CurrentLodLevel;
}

void FvAnimationModel::SetCurrentLodLevel(FvUInt32 u32CurrentLodLevel)
{
	FV_ASSERT(u32CurrentLodLevel < FV_MAX_LOD_LEVEL);
	FV_ASSERT(m_akNodes[0].m_pkNode);

	while(!m_akNodes[u32CurrentLodLevel].m_kEntityList.size())
	{
		--u32CurrentLodLevel;
	}

	m_u32CurrentLodLevel = u32CurrentLodLevel;
}

float FvAnimationModel::GetVolume()
{
	return m_fVolume;
}

void FvAnimationModel::SetRenderState(FvAnimatedCharacterNodePrivateData::RenderState eState)
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

FvAnimatedCharacterNodePrivateData::RenderState FvAnimationModel::GetRenderState( void )
{
	return m_eState;
}


