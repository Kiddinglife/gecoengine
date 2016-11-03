#include "FvTreeSerializer.h"
#include "FvTree.h"
#include <Ogre.h>

//----------------------------------------------------------------------------
FvXMLTreeSerializerImpl::FvXMLTreeSerializerImpl()
{

}
//----------------------------------------------------------------------------
FvXMLTreeSerializerImpl::~FvXMLTreeSerializerImpl()
{

}
//----------------------------------------------------------------------------
bool FvXMLTreeSerializerImpl::ExportTree(const FvTree *pkTree,
	const FvString& kFileName)
{
	return true;
}
//----------------------------------------------------------------------------
bool FvXMLTreeSerializerImpl::ImportTree(Ogre::DataStreamPtr &kStream,
	FvTree *pkDest)
{
	FvXMLSectionPtr spScene = FvXMLSection::OpenSection(kStream);
	if(spScene == NULL)
		return false;

	pkDest->m_kBoudingBox.SetBounds(spScene->ReadVector3("boundingBox/min"),
		spScene->ReadVector3("boundingBox/max"));

	std::vector<FvXMLSectionPtr> kNodesSections;
	spScene->OpenSections("nodes", kNodesSections);

	for(FvUInt32 i(0); i < kNodesSections.size(); ++i)
	{
		FvUInt32 u32Lod = kNodesSections[i]->AsAttributeInt("Lod");
		FV_ASSERT(u32Lod < FV_MAX_LOD_LEVEL);
		pkDest->m_u32CurrentLodLevel = u32Lod;

		FV_ASSERT(pkDest->m_akNodes[u32Lod].m_pkNode);
		FvXMLSectionIterator kIt = kNodesSections[i]->Begin();
		for(; kIt != kNodesSections[i]->End(); ++kIt)
			this->ReadNode(*kIt,pkDest->m_akNodes[u32Lod].m_pkNode,pkDest);
	}

	pkDest->m_u32CurrentLodLevel = 0;

	/*FvXMLSectionPtr spNodes = spScene->OpenSection("nodes");
	FvXMLSectionIterator kIt = spNodes->Begin();
	for(; kIt != spNodes->End(); ++kIt)
		this->ReadNode(*kIt,pkDest->m_pkNode,pkDest);

	pkDest->m_pkNode->_update(true,true);*/

	FvXMLSectionPtr spCollision = spScene->OpenSection("bspCollision");
	if(spCollision)
		this->ReadCollision(spCollision,pkDest);

	return true;
}
//----------------------------------------------------------------------------
void FvXMLTreeSerializerImpl::ReadNode(FvXMLSectionPtr spSection,
	Ogre::SceneNode *pkParent,FvTree *pkDest )
{
	FvXMLSectionPtr spEntity = spSection->OpenSection("entity");
	std::vector<FvXMLSectionPtr> kNodes;
	spSection->OpenSections("node",kNodes);
	if(!spEntity && kNodes.empty())
		return;

	FvString kIdentifier = spSection->ReadString("identifier");
	FvVector3 kPosition = spSection->ReadVector3("position");
	FvVector3 kScale = spSection->ReadVector3("scale");
	FvQuaternion kRotation = spSection->ReadQuaternion("rotation");

	SceneNode *pkCurrent = pkParent->createChildSceneNode(
		pkParent->getName() + "_" + kIdentifier);
	pkCurrent->setPosition(Vector3(kPosition.x,kPosition.y,kPosition.z));
	pkCurrent->setScale(Vector3(kScale.x,kScale.y,kScale.z));
	pkCurrent->setOrientation(Quaternion(kRotation.w,kRotation.x,kRotation.y,kRotation.z));

	this->ReadEntity(spEntity,pkCurrent,pkDest);

	std::vector<FvXMLSectionPtr>::iterator kIt = kNodes.begin();
	for(;kIt != kNodes.end(); ++kIt)
		this->ReadNode(*kIt,pkCurrent,pkDest);
}
//----------------------------------------------------------------------------
void FvXMLTreeSerializerImpl::ReadEntity(FvXMLSectionPtr spSection,
	Ogre::SceneNode *pkNode,FvTree *pkDest)
{
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);

	FvString kMeshIdentifier = spSection->ReadString("identifier");
	FvString kMeshFile = spSection->ReadString("mesh");

	FvString kMaterialName;

	std::vector<FvXMLSectionPtr> kSubentities;
	spSection->OpenSections("subentities/subentity",kSubentities);
	//FV_ASSERT(kSubentities.size() == 1);

	char acBuffer[16] = {0};

	NameValuePairList kParams;
	kParams["mesh"] = kMeshFile;
	sprintf_s(acBuffer, "%d", 80);
	kParams["instance_number"] = acBuffer;
	sprintf_s(acBuffer, "%d", pkDest->m_u32CurrentLodLevel > 0 ? RENDER_QUEUE_MAX : 53);
	kParams["render_queue_group"] = acBuffer;
	kParams["zone_name"] = pkDest->m_kZoneName;

	std::vector<FvXMLSectionPtr>::iterator kSubIt = kSubentities.begin();
	for(; kSubIt != kSubentities.end(); ++kSubIt)
	{
		int iIndex = (*kSubIt)->ReadInt("index",-1);
		FV_ASSERT(iIndex >= 0);
		kMaterialName = (*kSubIt)->ReadString("materialName");

		kParams["material"] = kMaterialName;
		sprintf_s(acBuffer, "%d", iIndex);
		kParams["sub_mesh"] = acBuffer;

		FvInstanceEntity* pkEntity = (FvInstanceEntity*)(pkSceneManager->createMovableObject(
			pkNode->getName() + "_" + kMeshIdentifier + "_submesh" + acBuffer + pkDest->m_kExtaName, "FvInstanceEntity", &kParams));

		pkEntity->setCastShadows(pkDest->m_bCastShadows);
		pkNode->attachObject(pkEntity);
		pkDest->m_akNodes[pkDest->m_u32CurrentLodLevel].m_kEntityList.AttachBack(*pkEntity);
		pkEntity->setRenderQueueGroup(53);
	}	

	/*NameValuePairList kParams;
	kParams["mesh"] = kMeshFile;
	kParams["material"] = kMaterialName;
	kParams["instance_number"] = acBuffer;*/
	//kParams["render_queue"]

	//Ogre::Entity *pkEntity = pkSceneManager->createEntity(
	//	pkNode->getName() + "_" + kMeshIdentifier,kMeshFile);
}
//----------------------------------------------------------------------------
void FvXMLTreeSerializerImpl::ReadCollision(FvXMLSectionPtr spSection,
	FvTree *pkDest)
{
	FvString kCollisionFile = spSection->ReadString("bsp");
	pkDest->m_spBSPTree = FvBSPProxyManager::getSingleton().load(kCollisionFile,
		pkDest->m_kGroupName);
}
//----------------------------------------------------------------------------
FvTreeSerializer::FvTreeSerializer()
{
	m_kImplementations.insert(FvTreeSerializerImplMap::value_type("node",
		new FvXMLTreeSerializerImpl));
}
//----------------------------------------------------------------------------
FvTreeSerializer::~FvTreeSerializer()
{
	for(FvTreeSerializerImplMap::iterator kIt = m_kImplementations.begin();
		kIt != m_kImplementations.end(); ++kIt)
	{
		delete kIt->second;
	}
	m_kImplementations.clear();
}
//----------------------------------------------------------------------------
bool FvTreeSerializer::ExportTree(const FvTree *pkTree,
	const FvString& kFileName, const FvString &kImpl)
{
	FvTreeSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ExportTree(pkTree,kFileName);
}
//----------------------------------------------------------------------------
bool FvTreeSerializer::ImportTree(Ogre::DataStreamPtr &kStream,
	FvTree *pkDest, const FvString &kImpl)
{
	FvTreeSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ImportTree(kStream,pkDest);
}
//----------------------------------------------------------------------------
