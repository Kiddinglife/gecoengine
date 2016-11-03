#include "FvTree.h"

#include <FvMatrix.h>
#include <FvXMLSection.h>
#include "FvTreeSerializer.h"

#include <Ogre.h>

FvInt32 FvTree::ms_uiTreeIdentify = 0;

//----------------------------------------------------------------------------
FvTree::FvTree(const FvString& kZoneName) : m_bCastShadows(false),
	m_fVolume(0), m_u32CurrentLodLevel(0), m_kZoneName(kZoneName)
{
	++ms_uiTreeIdentify;
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	for(int i = 0; i < FV_MAX_LOD_LEVEL; i++)
	{
		char acBuffer[32];
		sprintf_s(acBuffer, "FvTreeNode_%I32x_LOD%d",FvInt32(ms_uiTreeIdentify),i);
		m_akNodes[i].m_pkNode = pkSceneManager->createSceneNode(acBuffer);
	}
}
//----------------------------------------------------------------------------
void DestroyTreeNode(Ogre::SceneNode *pkNode)
{
	FV_ASSERT(Ogre::Root::getSingleton()._getCurrentSceneManager());
	Ogre::Node::ChildNodeIterator kObjIt = pkNode->getChildIterator();
	while (kObjIt.hasMoreElements())
		DestroyTreeNode(((Ogre::SceneNode*)kObjIt.getNext()));

	Ogre::SceneNode::ObjectIterator kMovObjIt = pkNode->getAttachedObjectIterator();
	while (kMovObjIt.hasMoreElements())
	{
		Ogre::MovableObject *pkObj = kMovObjIt.getNext();
		Ogre::Root::getSingleton()._getCurrentSceneManager()->destroyMovableObject(pkObj);
	}
	pkNode->detachAllObjects();

	Ogre::Root::getSingleton()._getCurrentSceneManager()->destroySceneNode(pkNode);
}
//----------------------------------------------------------------------------
FvTree::~FvTree()
{
	if(!m_spBSPTree.isNull())
	{
		FvBSPProxyManager::getSingleton().remove(m_spBSPTree->getHandle());
		m_spBSPTree.setNull();
	}

	for(FvUInt32 i(0); i < FV_MAX_LOD_LEVEL; ++i)
	{
		if(m_akNodes[i].m_pkNode)
			DestroyTreeNode(m_akNodes[i].m_pkNode);
	}

	/*if(m_pkNode)
		DestroyTreeNode(m_pkNode);*/
}
//----------------------------------------------------------------------------
bool FvTree::Load(const FvString &kFileName, const FvString &kGroupName,
	const FvString& kExtraName, const FvMatrix& kMatrix, bool bCastShadows)
{
	bool bExist = Ogre::ResourceGroupManager::getSingleton().
		resourceExists(kGroupName,kFileName);

	if(!bExist)
		return false;

	m_kFileName = kFileName;
	m_kGroupName = kGroupName;
	m_kExtaName = kExtraName;
	m_bCastShadows = bCastShadows;

	Ogre::DataStreamPtr spStream = Ogre::ResourceGroupManager::getSingleton().
		openResource(kFileName,kGroupName);

	FvTreeSerializer kSerializer;

	bool bRes = kSerializer.ImportTree(spStream,this);

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

	/*Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);

	m_pkNode = pkSceneManager->createSceneNode(m_kNodeName);
	if(!m_pkNode)
		return false;

	this->Transform(kMatrix);*/

	
	return bRes;
}
//----------------------------------------------------------------------------
FvBoundingBox const & FvTree::BoundingBox() const
{
	return m_kBoudingBox;
}
//----------------------------------------------------------------------------
const FvBSPTree * FvTree::GetBSPTree() const
{
	if(m_spBSPTree.getPointer())
	{
		return (FvBSPTree*)(*m_spBSPTree);
	}
	return NULL;
}
//----------------------------------------------------------------------------
const FvString FvTree::GetFileName() const
{
	return m_kFileName;
}
//----------------------------------------------------------------------------
void FvTree::Transform(const FvMatrix &kMatrix)
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
//----------------------------------------------------------------------------
const FvTree::Node& FvTree::GetTreeNode()
{
	return m_akNodes[m_u32CurrentLodLevel];
}
//----------------------------------------------------------------------------
SceneNode *FvTree::GetNode()
{
	return m_akNodes[m_u32CurrentLodLevel].m_pkNode;
}
//----------------------------------------------------------------------------
FvUInt32 FvTree::GetCurrentLodLevel()
{
	return m_u32CurrentLodLevel;
}
//----------------------------------------------------------------------------
void FvTree::SetCurrentLodLevel(FvUInt32 u32CurrentLodLevel)
{
	FV_ASSERT(u32CurrentLodLevel < FV_MAX_LOD_LEVEL);
	FV_ASSERT(m_akNodes[0].m_pkNode);

	while(!m_akNodes[u32CurrentLodLevel].m_kEntityList.Size())
	{
		--u32CurrentLodLevel;
	}

	m_u32CurrentLodLevel = u32CurrentLodLevel;
}
//----------------------------------------------------------------------------
float FvTree::GetVolume()
{
	return m_fVolume;
}
//----------------------------------------------------------------------------
