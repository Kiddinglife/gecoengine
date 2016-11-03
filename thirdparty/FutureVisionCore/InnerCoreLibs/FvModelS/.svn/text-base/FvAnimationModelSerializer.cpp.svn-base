#include "FvAnimationModelSerializer.h"
#include <Ogre.h>


FvAnimationModelSerializer::FvAnimationModelSerializer()
{
	m_kImplementations.insert(std::make_pair("node", new FvXMLAnimationModelSerializerImpl));
}

FvAnimationModelSerializer::~FvAnimationModelSerializer()
{
	for (FvAnimationModelSerializerImplMap::iterator kIt = m_kImplementations.begin();
		 kIt != m_kImplementations.end(); ++kIt)
	{
		delete kIt->second;
	}
	m_kImplementations.clear();
}

bool FvAnimationModelSerializer::ExportModel( const FvAnimationModel* pkModel, const FvString& kFileName, 
											 const FvString& kImpl /*= "node"*/ )
{
	FvAnimationModelSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if (kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s", kImpl.c_str());
		return false;
	}
	return kIt->second->ExportModel(pkModel, kFileName);
}

bool FvAnimationModelSerializer::ImportModel( Ogre::DataStreamPtr& kStream, FvAnimationModel* pkDest, 
											 const FvString& kImpl /*= "node"*/ )
{
	FvAnimationModelSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ImportModel(kStream,pkDest);
}

FvXMLAnimationModelSerializerImpl::FvXMLAnimationModelSerializerImpl()
{

}

FvXMLAnimationModelSerializerImpl::~FvXMLAnimationModelSerializerImpl()
{

}

bool FvXMLAnimationModelSerializerImpl::ExportModel( 
	const FvAnimationModel* pkModel, const FvString& kFileName )
{
	return true;
}

bool FvXMLAnimationModelSerializerImpl::ImportModel( 
	Ogre::DataStreamPtr& kStream, FvAnimationModel* pkDest )
{
	FvXMLSectionPtr spScene = FvXMLSection::OpenSection(kStream);
	if(spScene == NULL)
		return false;

	pkDest->m_kBoundingBox.SetBounds(spScene->ReadVector3("boundingBox/min"),
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

void FvXMLAnimationModelSerializerImpl::ReadNode( 
	FvXMLSectionPtr spSection, SceneNode* pkParent, FvAnimationModel* pkDest )
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

void FvXMLAnimationModelSerializerImpl::ReadEntity( 
	FvXMLSectionPtr spSection, Ogre::SceneNode* pkNode, FvAnimationModel* pkDest )
{
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);

	FvString kMeshIdentifier = spSection->ReadString("identifier");
	FvString kMeshFile = spSection->ReadString("mesh");

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
				pkDest->m_kModelAnimations.insert(std::make_pair(pkAnim->getAnimationName(),pkAnim));
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

void FvXMLAnimationModelSerializerImpl::ReadAnimation( 
	FvXMLSectionPtr spSection, Ogre::SceneNode* pkNode, FvAnimationModel* pkDest )
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
		pkDest->m_kModelAnimations.insert(std::make_pair(kAnimationName,pkAnimationState));
	}
}

void FvXMLAnimationModelSerializerImpl::ReadCollision( 
	FvXMLSectionPtr spSection, FvAnimationModel* pkDest )
{
	FvString kCollisionFile = spSection->ReadString("bsp");
	pkDest->m_spBSPTree = FvBSPProxyManager::getSingleton().load(kCollisionFile,
		pkDest->m_kGroupName);
}
