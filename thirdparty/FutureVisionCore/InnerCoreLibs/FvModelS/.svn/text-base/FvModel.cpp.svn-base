#include "FvModel.h"
#include "FvModelSerializer.h"

#include <FvMatrix.h>
#include <FvXMLSection.h>

#include <Ogre.h>
#include <OgrePrerequisites.h>
#include <OgreIteratorWrappers.h>

#ifndef FV_SERVER
FvInt32 FvModel::ms_uiModelIdentify = 0;
#endif // !FV_SERVER

FvModel::FvModel(const FvString& kZoneName)
#ifndef FV_SERVER

: m_bCastShadows(false),
m_u32CurrentLodLevel(0),
m_fVolume(0),
m_kZoneName(kZoneName)
{
	ms_uiModelIdentify++;
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	for(int i = 0; i < FV_MAX_LOD_LEVEL; i++)
	{
		char acBuffer[32];
		sprintf_s(acBuffer, "FvModelNode_%I32x_LOD%d",FvInt32(ms_uiModelIdentify),i);
		m_akNodes[i].m_pkNode = pkSceneManager->createSceneNode(acBuffer);
	}
}
#else // !FV_SERVER
{}


#endif // FV_SERVER

FvModel::FvModel()
#ifndef FV_SERVER

: m_bCastShadows(false),
m_u32CurrentLodLevel(0),
m_fVolume(0),
m_kZoneName("Outside")
{
	ms_uiModelIdentify++;
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	for(int i = 0; i < FV_MAX_LOD_LEVEL; i++)
	{
		char acBuffer[32];
		sprintf_s(acBuffer, "FvModelNode_%I32x_LOD%d",FvInt32(ms_uiModelIdentify),i);
		m_akNodes[i].m_pkNode = pkSceneManager->createSceneNode(acBuffer);
	}
}
#else // !FV_SERVER
{}


#endif // FV_SERVER

#ifndef FV_SERVER
void DestroyModelNode(Ogre::SceneNode *pkNode)
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

FvModel::~FvModel()
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

bool FvModel::Load(const FvString &kFileName, const FvString &kGroupName,
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

	FvModelSerializer kSerializer;

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
void FvModel::Transform(const FvMatrix &kMatrix)
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

const FvModel::Node& FvModel::GetModleNode()
{
	return m_akNodes[m_u32CurrentLodLevel];
}

FvUInt32 FvModel::GetCurrentLodLevel()
{
	return m_u32CurrentLodLevel;
}

void FvModel::SetCurrentLodLevel(FvUInt32 u32CurrentLodLevel)
{
	FV_ASSERT(u32CurrentLodLevel < FV_MAX_LOD_LEVEL);
	FV_ASSERT(m_akNodes[0].m_pkNode);

	while(!m_akNodes[u32CurrentLodLevel].m_kEntityList.Size())
	{
		--u32CurrentLodLevel;
	}

	m_u32CurrentLodLevel = u32CurrentLodLevel;
}

float FvModel::GetVolume()
{
	return m_fVolume;
}
#endif // !FV_SERVER