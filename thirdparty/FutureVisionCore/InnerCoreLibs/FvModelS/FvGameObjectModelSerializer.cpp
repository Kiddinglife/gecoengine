#include "FvGameObjectModelSerializer.h"

#include <Ogre.h>

//----------------------------------------------------------------------------
FvGameObjectModelSerializer::FvGameObjectModelSerializer()
{
	m_kImplementations.insert(FvGameObjectModelSerializerImplMap::value_type("node",
		new FvXMLGameObjectModelSerializerImpl));
}
//----------------------------------------------------------------------------
FvGameObjectModelSerializer::~FvGameObjectModelSerializer()
{
	for(FvGameObjectModelSerializerImplMap::iterator kIt = m_kImplementations.begin();
		kIt != m_kImplementations.end(); ++kIt)
	{
		delete kIt->second;
	}
	m_kImplementations.clear();
}
//----------------------------------------------------------------------------
bool FvGameObjectModelSerializer::ExportModel(
	const FvGameObjectModel *pkModel,const FvString& kFileName,
	const FvString &kImpl)
{
	FvGameObjectModelSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ExportModel(pkModel,kFileName);
}
//----------------------------------------------------------------------------
bool FvGameObjectModelSerializer::ImportModel(Ogre::DataStreamPtr &kStream,
	FvGameObjectModel *pkDest, const FvString &kImpl)
{
	FvGameObjectModelSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ImportModel(kStream,pkDest);
}
//----------------------------------------------------------------------------
FvXMLGameObjectModelSerializerImpl::FvXMLGameObjectModelSerializerImpl()
{

}
//----------------------------------------------------------------------------
FvXMLGameObjectModelSerializerImpl::~FvXMLGameObjectModelSerializerImpl()
{

}
//----------------------------------------------------------------------------
bool FvXMLGameObjectModelSerializerImpl::ExportModel(
	const FvGameObjectModel *pkModel, const FvString& kFileName)
{
	return true;
}
//----------------------------------------------------------------------------
bool FvXMLGameObjectModelSerializerImpl::ImportModel(
	Ogre::DataStreamPtr &kStream, FvGameObjectModel *pkDest)
{
	FvXMLSectionPtr spScene = FvXMLSection::OpenSection(kStream);
	if(spScene == NULL)
		return false;

	pkDest->m_kBoudingBox.SetBounds(spScene->ReadVector3("boundingBox/min"),
		spScene->ReadVector3("boundingBox/max"));

#ifndef FV_SERVER
	//FvXMLSectionPtr spNodes = spScene->OpenSection("nodes");

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

	//pkDest->m_pkNode->_update(true,true);
#endif // !FV_SERVER

	FvXMLSectionPtr spCollision = spScene->OpenSection("bspCollision");
	if(spCollision)
		this->ReadCollision(spCollision,pkDest);

	return true;
}
//----------------------------------------------------------------------------
#ifndef FV_SERVER
void FvXMLGameObjectModelSerializerImpl::ReadNode(
	FvXMLSectionPtr spSection,SceneNode *pkParent,FvGameObjectModel *pkDest)
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

	std::vector<FvXMLSectionPtr> kAnimations;
	spSection->OpenSections("animations/animation",kAnimations);
	std::vector<FvXMLSectionPtr>::iterator kAnimIt = kAnimations.begin();
	for(;kAnimIt != kAnimations.end(); ++kAnimIt)
		this->ReadAnimation(*kAnimIt,pkCurrent,pkDest);

	std::vector<FvXMLSectionPtr>::iterator kIt = kNodes.begin();
	for(;kIt != kNodes.end(); ++kIt)
		this->ReadNode(*kIt,pkCurrent,pkDest);
}
//----------------------------------------------------------------------------
void FvXMLGameObjectModelSerializerImpl::ReadEntity(
	FvXMLSectionPtr spSection, Ogre::SceneNode *pkNode,
	FvGameObjectModel *pkDest)
{
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);

	FvString kMeshIdentifier = spSection->ReadString("identifier");
	FvString kMeshFile = spSection->ReadString("mesh");

	//FvString kMaterialName;

	//std::vector<FvXMLSectionPtr> kSubentities;
	//spSection->OpenSections("subentities/subentity",kSubentities);
	//FV_ASSERT(kSubentities.size() == 1);

	/*char acBuffer[16] = {0};

	NameValuePairList kParams;
	kParams["mesh"] = kMeshFile;
	sprintf_s(acBuffer, "%d", 80);
	kParams["instance_number"] = acBuffer;
	sprintf_s(acBuffer, "%d", RENDER_QUEUE_MAX);
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
		pkEntity->setRenderQueueGroup(RENDER_QUEUE_MAX);
	}*/

	Ogre::Entity *pkEntity = pkSceneManager->createEntity(
		pkNode->getName() + "_" + kMeshIdentifier,kMeshFile);
	FV_ASSERT(pkEntity);
	pkEntity->setCastShadows(pkDest->m_bCastShadows);
	pkNode->attachObject(pkEntity);
	
	pkDest->m_akNodes[pkDest->m_u32CurrentLodLevel].m_kEntityList.push_back(pkEntity);
	pkEntity->setRenderQueueGroup(RENDER_QUEUE_MAX);
	AnimationStateSet *pkAnimations = pkEntity->getAllAnimationStates();
	if(pkAnimations)
	{
		AnimationStateIterator kIt = pkAnimations->getAnimationStateIterator();
		while (kIt.hasMoreElements())
		{
			AnimationState *pkAnim= kIt.getNext();
			if(pkDest->GetAnimation(pkAnim->getAnimationName()) == NULL)
				pkDest->m_kModelAnimations.insert(
				FvGameObjectModel::ModelAnimations::value_type(pkAnim->getAnimationName(),pkAnim));
		}
	}

	std::vector<FvXMLSectionPtr> kSubentities;
	spSection->OpenSections("subentities/subentity",kSubentities);
	std::vector<FvXMLSectionPtr>::iterator kSubIt = kSubentities.begin();
	for(; kSubIt != kSubentities.end(); ++kSubIt)
	{
		int iIndex = (*kSubIt)->ReadInt("index",-1);
		FvString kMaterialName = (*kSubIt)->ReadString("materialName");
		Ogre::SubEntity *pkSubEntity = pkEntity->getSubEntity(iIndex);
		if(pkSubEntity && !kMaterialName.empty())
			pkSubEntity->setMaterialName(kMaterialName);
	}
}
//----------------------------------------------------------------------------
void FvXMLGameObjectModelSerializerImpl::ReadAnimation(
	FvXMLSectionPtr spSection,Ogre::SceneNode *pkNode,FvGameObjectModel *pkDest)
{
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);

	FvString kAnimationName = spSection->ReadString("name");
	bool bEnable = spSection->ReadBool("enable");
	bool bLoop = spSection->ReadBool("loop");
	FvString kIterpolationMode = spSection->ReadString("interpolationMode");
	FvString kRotationIterpolationMode = spSection->ReadString("rotationInterpolationMode");
	float fLength = spSection->ReadFloat("length");

	if(!pkSceneManager->hasAnimation(pkNode->getName() + kAnimationName))
	{
		std::vector<FvXMLSectionPtr> kKeyFrames;
		spSection->OpenSections("keyframe",kKeyFrames);

		Animation *pkAnimation = pkSceneManager->createAnimation(pkNode->getName() + kAnimationName, fLength);
		if(strcmp(kIterpolationMode.c_str(),"spline") == 0)
			pkAnimation->setInterpolationMode(Animation::IM_SPLINE);
		else
			pkAnimation->setInterpolationMode(Animation::IM_LINEAR);

		if(strcmp(kRotationIterpolationMode.c_str(),"spherical") == 0)
			pkAnimation->setRotationInterpolationMode(Animation::RIM_SPHERICAL);
		else
			pkAnimation->setRotationInterpolationMode(Animation::RIM_LINEAR);

		NodeAnimationTrack *pkAnimationTrack = pkAnimation->createNodeTrack(
			pkAnimation->getNumNodeTracks() + 1, pkNode);

		for (size_t stKeyframeIndex = 0; stKeyframeIndex < kKeyFrames.size(); stKeyframeIndex++)
		{
			float fTime = kKeyFrames[stKeyframeIndex]->ReadFloat("time");
			FvVector3 kPosition = kKeyFrames[stKeyframeIndex]->ReadVector3("translation");
			FvVector3 kScale = kKeyFrames[stKeyframeIndex]->ReadVector3("scale");
			FvQuaternion kQuaternion = kKeyFrames[stKeyframeIndex]->ReadQuaternion("rotation");
			TransformKeyFrame *pkKeyFrame = pkAnimationTrack->createNodeKeyFrame(fTime);
			pkKeyFrame->setTranslate(Vector3(kPosition.x,kPosition.y,kPosition.z));
			pkKeyFrame->setRotation(Quaternion(kQuaternion.w,kQuaternion.x,kQuaternion.y,kQuaternion.z));
			pkKeyFrame->setScale(Vector3(kScale.x,kScale.y,kScale.z));
		}

		AnimationState *pkAnimationState = pkSceneManager->createAnimationState(pkNode->getName() + kAnimationName);
		pkAnimationState->setEnabled(bEnable);
		pkAnimationState->setLoop(bLoop);
		pkDest->m_kModelAnimations.insert(
			FvGameObjectModel::ModelAnimations::value_type(kAnimationName,pkAnimationState));
	}
}
#endif // !FV_SERVER
//----------------------------------------------------------------------------
void FvXMLGameObjectModelSerializerImpl::ReadCollision(
	FvXMLSectionPtr spSection,FvGameObjectModel *pkDest)
{
	FvString kCollisionFile = spSection->ReadString("bsp");
	pkDest->m_spBSPTree = FvBSPProxyManager::getSingleton().load(kCollisionFile,
		pkDest->m_kGroupName);
}
//----------------------------------------------------------------------------
