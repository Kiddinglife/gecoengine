////{future header message}
//#include "FvAvatar.h"
//#include <Ogre.h>
//#include <OgreTagPoint.h>
//#include <set>
//#include <FvRenderManager.h>
//#include <OgreOptimisedUtil.h>
//#include <FvDebug.h>
//#include <FvVectorMartixQuaternionPower.h>
//#include <ParticleUniverseSystemManager.h>
//
//class FvAvatarEntityFactory;
//
//float FvAvatar::ms_fDefaultBlendTime = 0.2f;
//static FvAvatarEntityFactory* ms_pkAvatarEntityFactory = NULL;
//
//struct FvAvatarLinkNode
//{
//	FvAvatar::ProviderPtr m_spTag;
//	FvAvatar::AvatarNode::LinkEffectData *m_pkEffectData;
//};
//
////----------------------------------------------------------------------------
//class FvSubMeshShell : public SubMesh
//{
//public:
//	FvUInt32 LodFaceListSize()
//	{
//		return mLodFaceList.size();
//	}
//
//	IndexData* GetIndexData()
//	{
//		return indexData;
//	}
//
//	bool IsNoneExtremityPoints()
//	{
//		return extremityPoints.empty();
//	}
//
//};
////----------------------------------------------------------------------------
//class EntityShell : public Entity
//{
//public:
//	void UpdateAnimation()
//	{
//		updateAnimation();
//	}
//};
////----------------------------------------------------------------------------
//class FvAvatarEntity : public Entity, public Renderable
//{
//public:
//	FvAvatarEntity(const String& kName, const MeshPtr& spMesh,
//		const MaterialPtr& spMaterial) 
//		: Entity(kName, spMesh)
//	{
//		setMaterial(spMaterial);
//		if(!spMesh->sharedVertexData)
//		{
//			FV_CRITICAL_MSG("%s\n", kName);
//		}
//		FV_ASSERT(spMesh->sharedVertexData);
//		FV_ASSERT(spMesh->getNumSubMeshes());
//
//		for(FvUInt32 i(0); i < spMesh->getNumSubMeshes(); ++i)
//		{
//			FvSubMeshShell* pkSubMesh =
//				(FvSubMeshShell*)(spMesh->getSubMesh(i));
//			FV_ASSERT(pkSubMesh);
//			FV_ASSERT(pkSubMesh->useSharedVertices);
//			FV_ASSERT(!pkSubMesh->LodFaceListSize());
//			FV_ASSERT(pkSubMesh->IsNoneExtremityPoints());
//		}
//
//		m_pkIndexData = OGRE_NEW IndexData();
//
//		setVisible(false);
//	}
//
//	virtual ~FvAvatarEntity()
//	{
//		if(m_pkIndexData)
//		{
//			OGRE_DELETE m_pkIndexData;
//			m_pkIndexData = NULL;
//		}
//	}
//
//	virtual const MaterialPtr& getMaterial() const
//	{
//		return (*mSubEntityList.begin())->getMaterial();
//	}
//
//	virtual Technique* getTechnique() const
//	{
//		return (*mSubEntityList.begin())->getTechnique();
//	}
//
//	virtual void getRenderOperation(RenderOperation& kRenderOperation)
//	{
//		kRenderOperation.vertexData = getVertexDataForBinding();
//		kRenderOperation.operationType = RenderOperation::OT_TRIANGLE_LIST;
//		kRenderOperation.useIndexes = true;
//		kRenderOperation.indexData = m_pkIndexData;
//	}
//
//	virtual void getWorldTransforms(Matrix4* pkXform) const
//	{
//		if (!mNumBoneMatrices || !isHardwareAnimationEnabled())
//		{
//			*pkXform = _getParentNodeFullTransform();
//		}
//		else
//		{
//			const Mesh::IndexMap& indexMap = mMesh->sharedBlendIndexToBoneIndexMap;
//			FV_ASSERT(indexMap.size() <= mNumBoneMatrices);
//
//			if (_isSkeletonAnimated())
//			{
//				assert(mBoneWorldMatrices);
//
//				Mesh::IndexMap::const_iterator it, itend;
//				itend = indexMap.end();
//				for (it = indexMap.begin(); it != itend; ++it, ++pkXform)
//				{
//					*pkXform = mBoneWorldMatrices[*it];
//				}
//			}
//			else
//			{
//				std::fill_n(pkXform, indexMap.size(), _getParentNodeFullTransform());
//			}
//		}
//	}
//
//	virtual unsigned short getNumWorldTransforms() const
//	{
//		if (!mNumBoneMatrices || !isHardwareAnimationEnabled())
//		{
//			return 1;
//		}
//		else
//		{
//			const Mesh::IndexMap& indexMap = mMesh->sharedBlendIndexToBoneIndexMap;
//			FV_ASSERT(indexMap.size() <= mNumBoneMatrices);
//
//			return static_cast<unsigned short>(indexMap.size());
//		}
//	}
//
//	virtual Real getSquaredViewDepth(const Camera* cam) const
//	{
//		Node* n = getParentNode();
//		FV_ASSERT(n);
//
//		return n->getSquaredViewDepth(cam);
//	}
//
//	virtual const LightList& getLights() const
//	{
//		return queryLights();
//	}
//
//	virtual bool getCastsShadows() const
//	{
//		return getCastShadows();
//	}
//
//	virtual void _updateRenderQueue(RenderQueue* queue)
//	{
//		if (!mInitialised)
//			return;
//
//		if (mMesh->getStateCount() != mMeshStateCount)
//		{
//			_initialise(true);
//		}
//
//		FV_ASSERT(!mMeshLodIndex);
//
//		if(m_pkIndexData->indexBuffer.getPointer())
//		{
//			if(mRenderQueueIDSet)
//			{
//				queue->addRenderable(this, mRenderQueueID);
//			}
//			else
//			{
//				queue->addRenderable(this);
//			}
//		}
//		else
//		{
//			setVisible(false);
//		}		
//
//		if (hasSkeleton() || hasVertexAnimation())
//		{
//			updateAnimation();
//
//			ChildObjectList::iterator child_itr = mChildObjectList.begin();
//			ChildObjectList::iterator child_itr_end = mChildObjectList.end();
//			for( ; child_itr != child_itr_end; child_itr++)
//			{
//				MovableObject* child = child_itr->second;
//				bool isVisible = child->isVisible();
//				if (isVisible)
//				{
//					child->_updateRenderQueue(queue);
//				}   
//			}
//		}
//
//		if (mDisplaySkeleton && hasSkeleton())
//		{
//			int numBones = mSkeletonInstance->getNumBones();
//			for (int b = 0; b < numBones; ++b)
//			{
//				Bone* bone = mSkeletonInstance->getBone(b);
//				if(mRenderQueueIDSet)
//				{
//					queue->addRenderable(bone, mRenderQueueID);
//				} else {
//					queue->addRenderable(bone);
//				}
//			}
//		}
//	}
//
//	virtual const String& getMovableType(void) const;
//
//	void SetDisplayFlags(FvUInt32 u32Flags)
//	{
//		FV_ASSERT(m_pkIndexData);
//		if(u32Flags)
//		{
//			setVisible(true);
//
//			IndexData* apkIndexDatas[FV_AVATAR_MAX_PARTS];
//			FvUInt32 u32IndexCount(0);
//			FvUInt32 u32SubMeshCount(0);
//
//			for(FvUInt32 i(0), j(0); i < mMesh->getNumSubMeshes(); ++i)
//			{
//				if(FV_MASK_HAS_ANY(u32Flags, FV_MASK(i)))
//				{
//					apkIndexDatas[j] = mMesh->getSubMesh(i)->indexData;
//					FV_ASSERT(apkIndexDatas[j]);
//					FV_ASSERT(apkIndexDatas[j]->indexBuffer->getType() ==
//						HardwareIndexBuffer::IT_16BIT);
//					FV_ASSERT(apkIndexDatas[j]->indexStart == 0);
//					FV_ASSERT(apkIndexDatas[j]->indexCount);
//
//					u32IndexCount += apkIndexDatas[j]->indexCount;
//					++j;
//					++u32SubMeshCount;
//				}
//			}
//
//			if(!u32IndexCount)
//			{
//				setVisible(false);
//				m_pkIndexData->indexStart = 0;
//				m_pkIndexData->indexCount = 0;
//				m_pkIndexData->indexBuffer.setNull();
//				return;
//			}
//
//			FV_ASSERT(u32SubMeshCount);
//
//			m_pkIndexData->indexStart = 0;
//			m_pkIndexData->indexCount = 0;
//			m_pkIndexData->indexBuffer = HardwareBufferManager::getSingleton().
//				createIndexBuffer(HardwareIndexBuffer::IT_16BIT,
//				u32IndexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
//
//			for(FvUInt32 i(0); i < u32SubMeshCount; ++i)
//			{
//				void* pvSrc = apkIndexDatas[i]->indexBuffer->lock(
//					HardwareBuffer::HBL_READ_ONLY);
//
//				m_pkIndexData->indexBuffer->writeData(
//					sizeof(FvUInt16) * m_pkIndexData->indexCount,
//					sizeof(FvUInt16) * apkIndexDatas[i]->indexCount,
//					pvSrc);
//
//				apkIndexDatas[i]->indexBuffer->unlock();
//
//				m_pkIndexData->indexCount += apkIndexDatas[i]->indexCount;
//			}
//
//			FV_ASSERT(m_pkIndexData->indexCount == u32IndexCount);
//		}
//		else
//		{
//			setVisible(false);
//			m_pkIndexData->indexStart = 0;
//			m_pkIndexData->indexCount = 0;
//			m_pkIndexData->indexBuffer.setNull();
//		}
//	}
//
//protected:
//	IndexData* m_pkIndexData;
//};
////----------------------------------------------------------------------------
//class FvAvatarEntityFactory : public MovableObjectFactory
//{
//protected:
//	MovableObject* createInstanceImpl(const String& kName, const NameValuePairList* params)
//	{
//		// must have mesh parameter
//		MeshPtr spMesh;
//		MaterialPtr spMaterial;
//
//		if (params != 0)
//		{
//			{
//				NameValuePairList::const_iterator ni = params->find("mesh");
//				if (ni != params->end())
//				{
//					// Get mesh (load if required)
//					spMesh = MeshManager::getSingleton().load(
//						ni->second,
//						// autodetect group location
//						ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
//				}
//			}
//			
//			{
//				NameValuePairList::const_iterator ni = params->find("material");
//				if (ni != params->end())
//				{
//					// Get mesh (load if required)
//					spMaterial = MaterialManager::getSingleton().load(
//						ni->second,
//						// autodetect group location
//						ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
//				}
//			}
//		}
//
//		if (spMesh.isNull())
//		{
//			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
//				"'mesh' parameter required when constructing an Entity.",
//				"EntityFactory::createInstance");
//		}
//
//		if(spMaterial.isNull())
//		{
//			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
//				"'material' parameter required when constructing an Entity.",
//				"EntityFactory::createInstance");
//		}
//
//		return OGRE_NEW FvAvatarEntity(kName, spMesh, spMaterial);
//	}
//
//public:
//	FvAvatarEntityFactory() {}
//	~FvAvatarEntityFactory() {}
//
//	static String FACTORY_TYPE_NAME;
//
//	const String& getType(void) const
//	{
//		return FACTORY_TYPE_NAME;
//	}
//
//	void destroyInstance(MovableObject* obj)
//	{
//		OGRE_DELETE obj;
//	}
//};
//String FvAvatarEntityFactory::FACTORY_TYPE_NAME = "AvatarEntity";
////----------------------------------------------------------------------------
//const String& FvAvatarEntity::getMovableType( void ) const
//{
//	return FvAvatarEntityFactory::FACTORY_TYPE_NAME;
//}
////----------------------------------------------------------------------------
//FvAvatarSkeleton::FvAvatarSkeleton(
//	FvAvatarResource::SkeletonInfo* pkSkeletonInfo, const char* pcName,
//	bool bShow)
//{
//	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//		_getCurrentSceneManager();
//	FV_ASSERT(pkSkeletonInfo && pkSceneManager);
//
//	m_pkSkeletonInfo = pkSkeletonInfo;
//
//	m_pkMainEntity = pkSceneManager->createEntity(
//		pcName, pkSkeletonInfo->m_spMainMesh->getName());
//	m_pkMainEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAX);
//
//	for(FvUInt32 i(0); i < m_pkMainEntity->getNumSubEntities(); ++i)
//	{
//		if(i < pkSkeletonInfo->m_kMainMaterials.size())
//		{
//			m_pkMainEntity->getSubEntity(i)->setMaterial(pkSkeletonInfo->m_kMainMaterials[i]);
//		}
//		else
//		{
//			m_pkMainEntity->getSubEntity(i)->setMaterialName("BaseWhiteNoLighting");
//		}
//	}
//
//	for(FvUInt32 i(0); i < m_pkMainEntity->getNumSubEntities(); ++i)
//	{
//		m_pkMainEntity->getSubEntity(i)->setCustomParameter(0, Ogre::Vector4(0.5f, 0.5f, 0.5f, 0.5f));
//	}
//
//	m_pkNode = pkSceneManager->createSceneNode(FvString(pcName) + "_Node");
//
//	m_pkNode->attachObject(m_pkMainEntity);
//
//	if(!bShow)
//	{
//		for(FvUInt32 i(0); i < m_pkMainEntity->getNumSubEntities(); ++i)
//		{
//			m_pkMainEntity->getSubEntity(i)->setVisible(false);
//		}
//	}
//}
////----------------------------------------------------------------------------
//FvAvatarSkeleton::~FvAvatarSkeleton()
//{
//	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//		_getCurrentSceneManager();
//	FV_ASSERT(pkSceneManager);
//	m_pkNode->detachObject(m_pkMainEntity);
//	pkSceneManager->destroyEntity(m_pkMainEntity);
//	pkSceneManager->destroySceneNode(m_pkNode);
//
//	LinkMap::iterator kIt = m_kLinkNodes.begin();
//	for(; kIt != m_kLinkNodes.end(); ++kIt)
//		delete kIt->second;
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeleton::AddEntity(Entity* pkEntity)
//{
//	FV_ASSERT(pkEntity);
//	SyncEntity(pkEntity);
//	m_pkNode->attachObject(pkEntity);
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeleton::RemoveEntity(Entity* pkEntity)
//{
//	FV_ASSERT(pkEntity);
//	pkEntity->stopSharingSkeletonInstance();
//	m_pkNode->detachObject(pkEntity);
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeleton::SyncEntity(Entity* pkEntity)
//{
//	FV_ASSERT(pkEntity);
//	if(m_pkMainEntity->getSkeleton() != pkEntity->getSkeleton())
//	{
//		pkEntity->shareSkeletonInstanceWith(m_pkMainEntity);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeleton::AttachNode(SceneNode* pkNode)
//{
//	FV_ASSERT(pkNode);
//	m_pkNode->addChild(pkNode);
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeleton::DetachNode(SceneNode* pkNode)
//{
//	FV_ASSERT(pkNode);
//	m_pkNode->removeChild(pkNode);
//}
////----------------------------------------------------------------------------
//SceneNode* FvAvatarSkeleton::GetSceneNode()
//{
//	return m_pkNode;
//}
////----------------------------------------------------------------------------
//AnimationState* FvAvatarSkeleton::GetAnimationState(
//	const char* pcAnimationName)
//{
//	return m_pkMainEntity->getAnimationState(pcAnimationName);
//}
////----------------------------------------------------------------------------
//AnimationStateSet* FvAvatarSkeleton::GetAllAnimationStates()
//{
//	return m_pkMainEntity->getAllAnimationStates();
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeleton::CreateTagPoint(TagPoint** ppkOut,
//	const char* pcBoneName)
//{
//	FV_ASSERT(ppkOut);
//	SkeletonInstance* pkSkeleton = m_pkMainEntity->getSkeleton();
//	*ppkOut = pkSkeleton->createTagPointOnBone(
//		pkSkeleton->getBone(pcBoneName));
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeleton::FreeTagPoint(TagPoint* pkTag)
//{
//	FV_ASSERT(pkTag);
//	SkeletonInstance* pkSkeleton = m_pkMainEntity->getSkeleton();
//	pkSkeleton->freeTagPoint(pkTag);
//}
////----------------------------------------------------------------------------
//float* FvAvatarSkeleton::GetPartition(FvUInt32 u32Partition)
//{
//	FV_ASSERT(u32Partition < m_pkSkeletonInfo->m_u32PartitonNumber);
//	return &(m_pkSkeletonInfo->m_pfPartitionBuffer[u32Partition *
//		m_pkSkeletonInfo->m_u32BoneNumber]);
//}
////----------------------------------------------------------------------------
//FvUInt32 FvAvatarSkeleton::GetBoneNumber()
//{
//	return m_pkSkeletonInfo->m_u32BoneNumber;
//}
////----------------------------------------------------------------------------
//class FvAnimationStateShell : public AnimationState
//{
//public:
//	void* GetBlendMask()
//	{
//		return mBlendMask;
//	}
//
//	void SetBlendMask(void* pvBlendMask)
//	{
//		mBlendMask = (AnimationState::BoneBlendMask*)pvBlendMask;
//	}
//};
////----------------------------------------------------------------------------
//FvAvatarSkeletonAnimationState::FvAvatarSkeletonAnimationState(
//	FvAvatarSkeleton* pkParent, const char* pcName) :
//	m_pkAnimationState(pkParent->GetAnimationState(pcName)),
//	m_eState(NONE_PARTITION), m_i32ActivePartition(FvAvatar::DO_NOT_USE),
//	m_pkParent(pkParent)
//{
//	FV_ASSERT(m_pkAnimationState);
//	FV_ASSERT(m_pkParent);
//	m_pkAnimationState->createBlendMask(pkParent->GetBoneNumber());
//	m_pvBlendMask = ((FvAnimationStateShell*)m_pkAnimationState)->
//		GetBlendMask();
//	((FvAnimationStateShell*)m_pkAnimationState)->SetBlendMask(NULL);
//
//}
////----------------------------------------------------------------------------
//FvAvatarSkeletonAnimationState::~FvAvatarSkeletonAnimationState()
//{
//	((FvAnimationStateShell*)m_pkAnimationState)->SetBlendMask(m_pvBlendMask);
//	m_pkAnimationState->destroyBlendMask();
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeletonAnimationState::ChangePartition(
//	FvInt32 i32Partition)
//{
//	if(i32Partition < 0)
//	{
//		NonePartition();
//		return;
//	}
//
//	if(m_i32ActivePartition == i32Partition) return;
//
//	if(m_eState == NONE_PARTITION)
//	{
//		((FvAnimationStateShell*)m_pkAnimationState)->SetBlendMask(
//			m_pvBlendMask);
//	}
//
//	float* fpPartition = m_pkParent->GetPartition(i32Partition);
//	FV_ASSERT(fpPartition);
//	m_pkAnimationState->_setBlendMaskData(fpPartition);
//	m_eState = SINGLE_PARTITION;
//	m_i32ActivePartition = i32Partition;
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeletonAnimationState::MorphPartition(float t, FvInt32 x,
//	FvInt32 y)
//{
//	if(x == y)
//	{
//		ChangePartition(x);
//		return;
//	}
//
//	if(m_eState == NONE_PARTITION)
//	{
//		((FvAnimationStateShell*)m_pkAnimationState)->SetBlendMask(
//			m_pvBlendMask);
//	}
//
//	if(x < 0)
//	{
//		if(y < 0)
//		{
//			NonePartition();
//			return;
//		}
//
//		float* fpPartitionY = m_pkParent->GetPartition(y);
//		for(FvUInt32 i(0); i < m_pkParent->GetBoneNumber(); ++i)
//		{
//			m_pkAnimationState->setBlendMaskEntry(i, FV_LERP(t,
//				1.0f, fpPartitionY[i]));
//		}
//	}
//	else if(y < 0)
//	{
//		float* fpPartitionX = m_pkParent->GetPartition(x);
//		for(FvUInt32 i(0); i < m_pkParent->GetBoneNumber(); ++i)
//		{
//			m_pkAnimationState->setBlendMaskEntry(i, FV_LERP(t,
//				fpPartitionX[i], 1.0f));
//		}
//	}
//	else
//	{
//		float* fpPartitionX = m_pkParent->GetPartition(x);
//		float* fpPartitionY = m_pkParent->GetPartition(y);
//
//		for(FvUInt32 i(0); i < m_pkParent->GetBoneNumber(); ++i)
//		{
//			m_pkAnimationState->setBlendMaskEntry(i, FV_LERP(t,
//				fpPartitionX[i], fpPartitionY[i]));
//		}
//	}
//
//	m_eState = MORPH_PARTITION;
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeletonAnimationState::NonePartition()
//{
//	if(m_eState != NONE_PARTITION)
//	{
//		((FvAnimationStateShell*)m_pkAnimationState)->SetBlendMask(
//			NULL);
//		m_eState = NONE_PARTITION;
//		m_i32ActivePartition = FvAvatar::DO_NOT_USE;
//	}
//}
////----------------------------------------------------------------------------
//FvAvatarSkeletonAnimationState::State
//	FvAvatarSkeletonAnimationState::GetState()
//{
//	return m_eState;
//}
////----------------------------------------------------------------------------
//FvInt32 FvAvatarSkeletonAnimationState::GetActivePartition()
//{
//	return m_i32ActivePartition;
//}
////----------------------------------------------------------------------------
//const char* FvAvatarSkeletonAnimationState::GetName()
//{
//	return m_pkAnimationState->getAnimationName().c_str();
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeletonAnimationState::SetLoop(bool bLoop)
//{
//	m_pkAnimationState->setLoop(bLoop);
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeletonAnimationState::SetEnabled(bool bEnabled)
//{
//	if(m_pkAnimationState->getEnabled() != bEnabled)
//	{
//		m_pkAnimationState->setEnabled(bEnabled);
//		if(bEnabled) SetTimePosition(0);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeletonAnimationState::SetWeight(float fWeight)
//{
//	m_pkAnimationState->setWeight(fWeight);
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeletonAnimationState::SetTimePosition(float fTimePos)
//{
//	m_pkAnimationState->setTimePosition(fTimePos);
//}
////----------------------------------------------------------------------------
//bool FvAvatarSkeletonAnimationState::IsLoop()
//{
//	return m_pkAnimationState->getLoop();
//}
////----------------------------------------------------------------------------
//bool FvAvatarSkeletonAnimationState::IsEnabled()
//{
//	return m_pkAnimationState->getEnabled();
//}
////----------------------------------------------------------------------------
//float FvAvatarSkeletonAnimationState::GetWeight()
//{
//	return m_pkAnimationState->getWeight();
//}
////----------------------------------------------------------------------------
//float FvAvatarSkeletonAnimationState::GetTimePosition()
//{
//	return m_pkAnimationState->getTimePosition();
//}
////----------------------------------------------------------------------------
//float FvAvatarSkeletonAnimationState::GetLength()
//{
//	return m_pkAnimationState->getLength();
//}
////----------------------------------------------------------------------------
//void FvAvatarSkeletonAnimationState::AddTime(float fOffset)
//{
//	m_pkAnimationState->addTime(fOffset);
//}
////----------------------------------------------------------------------------
//bool FvAvatarSkeletonAnimationState::IsEnd()
//{
//	return (!IsLoop()) && (m_pkAnimationState->getLength() ==
//		m_pkAnimationState->getTimePosition());
//}
////----------------------------------------------------------------------------
//FvAvatarAnimationState::FvAvatarAnimationState(
//	FvAvatarSkeleton** ppkSkeletons, FvUInt32 u32Number, const char* pcName)
//{
//	FV_ASSERT(u32Number > 0);
//	m_ppkStates = new FvAvatarSkeletonAnimationState*[u32Number];
//	FV_ASSERT(m_ppkStates);
//	
//	for(FvUInt32 i(0); i < u32Number; ++i)
//	{
//		m_ppkStates[i] = new FvAvatarSkeletonAnimationState(
//			ppkSkeletons[i], pcName);
//	}
//
//	m_u32BindingNumber = u32Number;
//	m_u32ActiveBinding = 0;
//	m_eState = m_ppkStates[m_u32ActiveBinding]->GetState();
//	m_i32ActivePartition =
//		m_ppkStates[m_u32ActiveBinding]->GetActivePartition();
//	m_fTimePosition = m_ppkStates[m_u32ActiveBinding]->GetTimePosition();
//	m_fTimeScale = 1.0f;
//	m_fLength = m_ppkStates[m_u32ActiveBinding]->GetLength();
//	m_fWeight = m_ppkStates[m_u32ActiveBinding]->GetWeight();
//	m_fTimeCache = 0;
//	m_fTimeCachePosition = 0;
//
//	m_itNext = m_kKeyEventList.end();
//	m_u32EventMask = 0;
//	m_pkCallback = NULL;
//
//	m_i32Enable = -1;
//	m_eBlendState = BLEND_NONE;
//	m_fBlendTime = 0;
//	m_fInvBlendTime = 0;
//	m_fPassBlendTime = 0;
//	m_i32TargetPartition = 0;
//	m_fBlendPartitionTime = 0;
//	m_fInvBlendPartitionTime = 0;
//	m_fPassBlendPartitionTime = 0;
//	m_bEnableMorphPartition = false;
//	m_bDirtyEventList = false;
//	m_bManualTime = false;
//	m_bCacheTime = false;
//}
////----------------------------------------------------------------------------
//FvAvatarAnimationState::~FvAvatarAnimationState()
//{
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		delete m_ppkStates[i];
//	}
//	delete [] m_ppkStates;
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::Active(FvUInt32 u32Binding)
//{
//	if(m_u32ActiveBinding != u32Binding)
//	{
//		m_eState = m_ppkStates[m_u32ActiveBinding]->GetState();
//		m_u32ActiveBinding = u32Binding;
//		Update();
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::Update()
//{
//	if(m_eState == FvAvatarSkeletonAnimationState::SINGLE_PARTITION)
//	{
//		m_ppkStates[m_u32ActiveBinding]->ChangePartition(
//			m_i32ActivePartition);
//	}
//	else if(m_eState == FvAvatarSkeletonAnimationState::NONE_PARTITION)
//	{
//		m_ppkStates[m_u32ActiveBinding]->NonePartition();
//	}
//
//	m_ppkStates[m_u32ActiveBinding]->SetTimePosition(m_fTimePosition);
//	m_ppkStates[m_u32ActiveBinding]->SetWeight(m_fWeight);
//	m_ppkStates[m_u32ActiveBinding]->SetEnabled(m_i32Enable >= 0);
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::ChangePartition(FvInt32 i32Partition,
//	float fBlendTime)
//{
//	if((fBlendTime <= 0) || m_bManualTime)
//	{
//		m_i32ActivePartition = i32Partition;
//		m_ppkStates[m_u32ActiveBinding]->ChangePartition(i32Partition);
//		m_bEnableMorphPartition = false;
//		return;
//	}
//	else
//	{
//		
//		if(!m_bEnableMorphPartition)
//		{
//			if(m_i32ActivePartition != i32Partition)
//			{
//				m_bEnableMorphPartition = true;
//				m_i32TargetPartition = i32Partition;
//				m_fBlendPartitionTime = fBlendTime;
//				m_fInvBlendPartitionTime = 1.0f / fBlendTime;
//				m_fPassBlendPartitionTime = 0;
//			}
//		}
//		else
//		{
//			if(m_i32TargetPartition != i32Partition)
//			{
//				if(m_i32ActivePartition == i32Partition)
//				{
//					m_i32ActivePartition = m_i32TargetPartition;
//					m_i32TargetPartition = i32Partition;
//					float fWeight =
//						m_fPassBlendPartitionTime * m_fInvBlendPartitionTime;
//					m_fBlendPartitionTime = fBlendTime;
//					m_fInvBlendPartitionTime = 1.0f / fBlendTime;
//					m_fPassBlendPartitionTime = (1 - fWeight) * fBlendTime;
//
//				}
//				else
//				{
//					m_i32ActivePartition = m_i32TargetPartition;
//					m_i32TargetPartition = i32Partition;
//					m_fBlendPartitionTime = fBlendTime;
//					m_fInvBlendPartitionTime = 1.0f / fBlendTime;
//					m_fPassBlendPartitionTime = 0;
//				}
//			}
//		}
//
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::SetLoop(bool bLoop)
//{
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		m_ppkStates[i]->SetLoop(bLoop);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::SetManualTime(bool bManual)
//{
//	m_bManualTime = bManual;
//}
////----------------------------------------------------------------------------
//bool FvAvatarAnimationState::SetEnabled(FvInt32 i32Enable, float fBlendTime)
//{
//	if(i32Enable < 0)
//	{
//		if(m_i32Enable >= 0)
//		{
//			if((m_u32EventMask & ANI_DISABLE) && m_pkCallback)
//			{
//				m_pkCallback->EventCome(this, ANI_DISABLE, "ANI_DISABLE");
//			}
//
//			m_itNext = m_kKeyEventList.begin();
//
//			if((fBlendTime > 0) && (!m_bManualTime))
//			{
//				FV_ASSERT(m_eBlendState != BLEND_ANIMATION_DISABLE);
//				if(m_eBlendState == BLEND_ANIMATION_ENABLE)
//				{
//					float fWeightScale = m_fPassBlendTime * m_fInvBlendTime;
//					m_fBlendTime = fBlendTime;
//					m_fInvBlendTime = 1.0f / fBlendTime;
//					m_fPassBlendTime = (1.0f - fWeightScale) * m_fBlendTime;
//				}
//				else
//				{
//					m_fBlendTime = fBlendTime;
//					m_fInvBlendTime = 1.0f / fBlendTime;
//					m_fPassBlendTime = 0;
//				}
//				m_ppkStates[m_u32ActiveBinding]->SetWeight(
//					(1 - m_fPassBlendTime * m_fInvBlendTime) * m_fWeight);
//				m_eBlendState = BLEND_ANIMATION_DISABLE;
//				m_i32Enable = i32Enable;
//				return true;
//			}
//			else
//			{
//				DisabledAll();
//				m_eBlendState = BLEND_NONE;
//			}
//		}
//	}
//	else
//	{
//		if(m_i32Enable < 0)
//		{
//			if((m_u32EventMask & ANI_ENABLE) && m_pkCallback)
//			{
//				m_pkCallback->EventCome(this, ANI_ENABLE, "ANI_ENABLE");
//			}
//
//			if((fBlendTime > 0) && (!m_bManualTime))
//			{
//				FV_ASSERT(m_eBlendState != BLEND_ANIMATION_ENABLE);
//				if(m_eBlendState == BLEND_ANIMATION_DISABLE)
//				{
//					float fWeightScale = 1.0f -
//						m_fPassBlendTime * m_fInvBlendTime;
//					m_fBlendTime = fBlendTime;
//					m_fInvBlendTime = 1.0f / fBlendTime;
//					m_fPassBlendTime = fWeightScale * m_fBlendTime;
//				}
//				else
//				{
//					m_fBlendTime = fBlendTime;
//					m_fInvBlendTime = 1.0f / fBlendTime;
//					m_fPassBlendTime = 0;
//				}
//				m_ppkStates[m_u32ActiveBinding]->SetWeight(
//					m_fPassBlendTime * m_fInvBlendTime * m_fWeight);
//				m_eBlendState = BLEND_ANIMATION_ENABLE;
//				m_ppkStates[m_u32ActiveBinding]->SetEnabled(true);
//				m_fTimePosition =
//					m_ppkStates[m_u32ActiveBinding]->GetTimePosition();
//			}
//			else
//			{
//				FV_ASSERT(m_eBlendState != BLEND_ANIMATION_ENABLE);
//				m_ppkStates[m_u32ActiveBinding]->SetWeight(m_fWeight);
//				m_ppkStates[m_u32ActiveBinding]->SetEnabled(true);
//				m_eBlendState = BLEND_NONE;
//			}
//		}
//	}
//
//	m_i32Enable = i32Enable;
//
//	return false;
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::SetTimePosition(float fTimePos)
//{
//	m_fTimePosition = fTimePos;
//	m_ppkStates[m_u32ActiveBinding]->SetTimePosition(m_fTimePosition);
//
//	if(m_itNext != m_kKeyEventList.end())
//	{
//		if(fTimePos < (*m_itNext).m_fTimePos)
//		{
//			m_itNext = m_kKeyEventList.begin();
//		}
//	}	
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::SetTimeScale(float fTimeScale)
//{
//	m_fTimeScale = fTimeScale;
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::SetWeight(float fWeight)
//{
//	m_fWeight = fWeight;
//	m_ppkStates[m_u32ActiveBinding]->SetWeight(fWeight);
//}
////----------------------------------------------------------------------------
//bool FvAvatarAnimationState::IsLoop()
//{
//	return m_ppkStates[0]->IsLoop();
//}
////----------------------------------------------------------------------------
//bool FvAvatarAnimationState::IsManualTime()
//{
//	return m_bManualTime;
//}
////----------------------------------------------------------------------------
//FvInt32 FvAvatarAnimationState::GetEnabled()
//{
//	return m_i32Enable;
//}
////----------------------------------------------------------------------------
//float FvAvatarAnimationState::GetTimePosition()
//{
//	return m_fTimePosition;
//}
////----------------------------------------------------------------------------
//float FvAvatarAnimationState::GetTimeScale()
//{
//	return m_fTimeScale;
//}
////----------------------------------------------------------------------------
//float FvAvatarAnimationState::GetWeight()
//{
//	return m_fWeight;
//}
////----------------------------------------------------------------------------
//float FvAvatarAnimationState::GetLength()
//{
//	return m_ppkStates[0]->GetLength();
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::AddTime(float fDeltaTime, bool bIsCache)
//{
//	if(!m_bManualTime)
//	{
//		if(bIsCache)
//		{
//			if(!m_bCacheTime)
//			{
//				m_fTimeCache = 0;
//				m_fTimeCachePosition = m_fTimePosition;
//				m_bCacheTime = true;
//			}
//
//			AddTimeCache(fDeltaTime * m_fTimeScale);
//		}
//		else
//		{
//			if(m_bCacheTime)
//			{
//				AddTimeBlend(fDeltaTime + m_fTimeCache);
//
//				m_ppkStates[m_u32ActiveBinding]->SetTimePosition(
//					m_fTimeCachePosition);
//				m_fTimePosition = m_fTimeCachePosition;
//
//				AddTimePosition(fDeltaTime);
//
//				m_bCacheTime = false;
//			}
//			else
//			{
//				AddTimeBlend(fDeltaTime);
//				AddTimePosition(fDeltaTime);
//			}
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::AddTimeCache(float fDeltaTime)
//{
//	FV_ASSERT(GetEnabled() >= 0);
//
//	m_fTimeCache += fDeltaTime;
//
//	float fLastTimePosition = m_fTimeCachePosition;
//
//	m_fTimeCachePosition += fDeltaTime;
//
//	if(IsLoop())
//	{
//		m_fTimeCachePosition = FvFmodf(m_fTimeCachePosition, GetLength());
//		if(m_fTimeCachePosition < 0)
//			m_fTimeCachePosition += GetLength();     
//	}
//	else
//	{
//		if(m_fTimeCachePosition < 0)
//			m_fTimeCachePosition = 0;
//		else if (m_fTimeCachePosition > GetLength())
//			m_fTimeCachePosition = GetLength();
//	}
//
//	ProcessCallbacks(fLastTimePosition, m_fTimeCachePosition);
//}
//
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::AddTimeBlend(float fDeltaTime)
//{
//	FV_ASSERT(m_eBlendState != BLEND_ANIMATION_DISABLE);
//
//	if(m_eBlendState == BLEND_ANIMATION_ENABLE)
//	{
//		m_fPassBlendTime += fDeltaTime;
//		if(m_fPassBlendTime >= m_fBlendTime)
//		{
//			m_ppkStates[m_u32ActiveBinding]->SetWeight(m_fWeight);
//			m_eBlendState = BLEND_NONE;
//		}
//		else
//		{
//			m_ppkStates[m_u32ActiveBinding]->SetWeight(
//				m_fPassBlendTime * m_fInvBlendTime * m_fWeight);
//		}
//	}
//
//	if(m_bEnableMorphPartition)
//	{
//		m_fPassBlendPartitionTime += fDeltaTime;
//		if(m_fPassBlendPartitionTime >= m_fBlendPartitionTime)
//		{
//			m_i32ActivePartition = m_i32TargetPartition;
//			m_i32TargetPartition = NULL;
//			m_ppkStates[m_u32ActiveBinding]->ChangePartition(
//				m_i32ActivePartition);
//			m_bEnableMorphPartition = false;
//		}
//		else
//		{
//			m_ppkStates[m_u32ActiveBinding]->MorphPartition(
//				m_fPassBlendPartitionTime * m_fInvBlendPartitionTime,
//				m_i32ActivePartition, m_i32TargetPartition);
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::AddTimePosition(float fDeltaTime)
//{
//	float fLastTimePosition = m_fTimePosition;
//	m_ppkStates[m_u32ActiveBinding]->AddTime(fDeltaTime * m_fTimeScale);
//	m_fTimePosition = m_ppkStates[m_u32ActiveBinding]->GetTimePosition();
//
//	ProcessCallbacks(fLastTimePosition, m_fTimePosition);
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::ProcessCallbacks(float fLastTimePos,
//	float fTimePos)
//{
//	if(m_pkCallback)
//	{
//		if(m_u32EventMask & ANI_END)
//		{
//			if(((fTimePos != fLastTimePos) &&
//				(fTimePos == m_ppkStates[m_u32ActiveBinding]->GetLength())) ||
//				(fTimePos < fLastTimePos))
//			{
//				m_pkCallback->EventCome(this, ANI_END, "ANI_END");
//			}
//		}
//
//		if(m_u32EventMask & ANI_START)
//		{
//			if(((fLastTimePos == 0) && (fTimePos != 0)) ||
//				(fTimePos < fLastTimePos))
//			{
//				m_pkCallback->EventCome(this, ANI_START, "ANI_START");
//			}
//		}
//
//		if(m_u32EventMask & ANI_KEY_EVENT)
//		{
//			if(m_bDirtyEventList)
//			{
//				KeyEventList::iterator it = m_kKeyEventList.begin();
//				while(it != m_kKeyEventList.end())
//				{
//					KeyEvent* pkEvent = &(*it);
//					KeyEventList::iterator itTemp = it++;
//					if(pkEvent->m_bNeedToRemove)
//					{
//						if(itTemp == m_itNext)
//						{
//							m_itNext = it;
//						}
//						m_kKeyEventList.erase(itTemp);
//					}
//				}
//				m_bDirtyEventList = false;
//				if(m_kKeyEventList.empty())
//				{
//					m_u32EventMask &= ~ANI_KEY_EVENT;
//					return;
//				}
//			}
//
//			if(m_itNext == m_kKeyEventList.end())
//				m_itNext = m_kKeyEventList.begin();
//
//			if(fTimePos > fLastTimePos)
//			{
//				while(m_itNext != m_kKeyEventList.end())
//				{
//					KeyEvent* pkEvent = &(*m_itNext);
//					FV_ASSERT(!pkEvent->m_bNeedToRemove);
//
//					if(pkEvent->m_fTimePos > fTimePos)
//					{
//						break;
//					}
//					else
//					{
//						if(pkEvent->m_fTimePos >= fLastTimePos)
//						{
//							m_pkCallback->EventCome(this, ANI_KEY_EVENT,
//								pkEvent->m_strName.c_str());
//						}
//						++m_itNext;
//					}
//				}
//			}
//			else if(fTimePos < fLastTimePos)
//			{
//				while(m_itNext != m_kKeyEventList.end())
//				{
//					KeyEvent* pkEvent = &(*m_itNext);
//					FV_ASSERT(!pkEvent->m_bNeedToRemove);
//
//					if(pkEvent->m_fTimePos > fLastTimePos)
//					{
//						m_pkCallback->EventCome(this, ANI_KEY_EVENT,
//							pkEvent->m_strName.c_str());
//					}
//					++m_itNext;
//				}
//
//				for(m_itNext = m_kKeyEventList.begin();
//					m_itNext != m_kKeyEventList.end(); ++m_itNext)
//				{
//					KeyEvent* pkEvent = &(*m_itNext);
//					FV_ASSERT(!pkEvent->m_bNeedToRemove);
//
//					if(pkEvent->m_fTimePos > fTimePos)
//					{
//						break;
//					}
//					else
//					{
//						m_pkCallback->EventCome(this, ANI_KEY_EVENT,
//							pkEvent->m_strName.c_str());
//					}
//				}
//			}
//
//			if(m_itNext == m_kKeyEventList.end())
//				m_itNext = m_kKeyEventList.begin();
//		}
//	}
//}
////----------------------------------------------------------------------------
//bool FvAvatarAnimationState::AddTimeDisable(float fDeltaTime)
//{
//	if(m_eBlendState == BLEND_ANIMATION_DISABLE)
//	{
//		m_fPassBlendTime += fDeltaTime;
//		if(m_fPassBlendTime >= m_fBlendTime)
//		{
//			DisabledAll();
//			m_eBlendState = BLEND_NONE;
//		}
//		else
//		{
//			m_ppkStates[m_u32ActiveBinding]->SetWeight(
//				(1 - m_fPassBlendTime * m_fInvBlendTime) * m_fWeight);
//			return false;
//		}
//	}
//	else
//	{
//		if(m_ppkStates[m_u32ActiveBinding]->IsEnd())
//		{
//			SetTimePosition(0);
//		}
//	}
//	return true;
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::DisabledAll()
//{
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		m_ppkStates[i]->SetEnabled(false);
//	}
//
//	if(m_bEnableMorphPartition)
//	{
//		m_bEnableMorphPartition = false;
//		m_i32ActivePartition = m_i32TargetPartition;
//		m_i32TargetPartition = NULL;
//		m_ppkStates[m_u32ActiveBinding]->ChangePartition(
//			m_i32ActivePartition);
//	}
//}
////----------------------------------------------------------------------------
//const char* FvAvatarAnimationState::GetName()
//{
//	return m_ppkStates[m_u32ActiveBinding]->GetName();
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::SetCallback(AnimationStateCallback* pkCallback)
//{
//	m_pkCallback = pkCallback;
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::RegisterCallback(EventType eType,
//	const char* pcName, float fTimePos)
//{
//	m_u32EventMask |= eType;
//	if(eType == ANI_KEY_EVENT)
//	{
//		KeyEventList::iterator it = m_kKeyEventList.begin();
//
//		KeyEvent kEvent =
//		{
//			pcName,
//			fTimePos,
//			false
//		};
//
//		while(true)
//		{
//			if(it == m_kKeyEventList.end())
//			{
//				m_kKeyEventList.push_back(kEvent);
//				m_itNext = m_kKeyEventList.begin();
//				break;
//			}
//			if((*it).m_fTimePos > kEvent.m_fTimePos)
//			{
//				m_kKeyEventList.insert(it, kEvent);
//
//				if((*m_itNext).m_fTimePos < m_fTimePosition)
//				{
//					if(m_itNext != m_kKeyEventList.begin())
//						--m_itNext;
//				}
//				else
//				{
//					if(kEvent.m_fTimePos >= m_fTimePosition)
//					{
//						if(m_itNext != m_kKeyEventList.begin())
//							--m_itNext;
//					}
//				}
//				break;
//			}
//			++it;
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarAnimationState::UnregisterCallback(EventType eType,
//	const char* pcName)
//{
//	if(eType == ANI_KEY_EVENT)
//	{
//		for(KeyEventList::iterator it = m_kKeyEventList.begin();
//			it != m_kKeyEventList.end(); ++it)
//		{
//			if((*it).m_strName == pcName)
//			{
//				(*it).m_bNeedToRemove = true;
//			}
//		}
//		m_bDirtyEventList = true;
//	}
//	else
//	{
//		m_u32EventMask &= ~eType;
//	}
//}
////----------------------------------------------------------------------------
//FvInt32 FvAvatarAnimationState::GetActivePartition()
//{
//	if(m_bEnableMorphPartition)
//		return m_i32TargetPartition;
//	else
//		return m_i32ActivePartition;
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetGlobalDefaultBlendTime(float fBlendTime)
//{
//	ms_fDefaultBlendTime = fBlendTime;
//}
////----------------------------------------------------------------------------
//float FvAvatar::GetGlobalDefaultBlendTime()
//{
//	return ms_fDefaultBlendTime;
//}
////----------------------------------------------------------------------------
//void FvAvatar::Init()
//{
//	FV_ASSERT(!ms_pkAvatarEntityFactory);
//
//	ms_pkAvatarEntityFactory = OGRE_NEW FvAvatarEntityFactory();
//	FV_ASSERT(ms_pkAvatarEntityFactory);
//	Root::getSingleton().addMovableObjectFactory(ms_pkAvatarEntityFactory);
//}
////----------------------------------------------------------------------------
//void FvAvatar::Term()
//{
//	FV_ASSERT(ms_pkAvatarEntityFactory);
//	Root* pkRoot = Root::getSingletonPtr();
//	if(pkRoot)
//	{
//		pkRoot->removeMovableObjectFactory(ms_pkAvatarEntityFactory);
//	}
//	OGRE_DELETE ms_pkAvatarEntityFactory;
//	ms_pkAvatarEntityFactory = NULL;
//}
////----------------------------------------------------------------------------
//FvAvatar::FvAvatar(const FvAvatarResourcePtr& rspAvatarResource,
//	const char* pcName) : m_kName(pcName), m_pkBoundingBoxParams(NULL),
//	m_kScale(1,1,1)
//{
//	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//		_getCurrentSceneManager();
//	FV_ASSERT(pkSceneManager);
//	FV_ASSERT(rspAvatarResource.getPointer());
//	FV_ASSERT(rspAvatarResource->isLoaded());
//	m_spAvatarResource = rspAvatarResource;
//
//	const FvUInt32 u32SkeletonNumber =
//		m_spAvatarResource->m_u32SkeletonNumber;
//
//	m_ppkAvatarSkeletons = new FvAvatarSkeleton*[u32SkeletonNumber];
//
//	char acNameBuffer[FV_AVATAR_MAX_NAME_LENGTH];
//
//	for(FvUInt32 i(0); i < u32SkeletonNumber; ++i)
//	{
//		sprintf(acNameBuffer, "%s_%d", pcName, i);	
//		m_ppkAvatarSkeletons[i] = new FvAvatarSkeleton(
//			&(m_spAvatarResource->m_kSkeletonInfo[i]), acNameBuffer,
//			m_spAvatarResource->m_kSkeletonInfo[i].m_bShowMainMesh);
//
//		FV_ASSERT(m_ppkAvatarSkeletons[i]);
//	}
//
//	m_u32BindingNumber = u32SkeletonNumber;
//
//	m_u32ActiveBinding = 0;
//
//	m_pkAvatarRootNode = pkSceneManager->createSceneNode(FvString(pcName) + "_Node");
//	FV_ASSERT(m_pkAvatarRootNode);
//	m_pkAvatarRootNode->addChild(
//		m_ppkAvatarSkeletons[m_u32ActiveBinding]->GetSceneNode());
//
//	const FvUInt32 u32ActiveAnimationNumber =
//		m_spAvatarResource->m_u32kActiveAnimationNumber;
//
//	m_pkAnimationStates =
//		new AnimationState[u32ActiveAnimationNumber];
//
//	for(FvUInt32 i(0); i < u32ActiveAnimationNumber; ++i)
//	{
//		const char* pcAnimationName =
//			m_spAvatarResource->m_pkActiveAnimations[i].c_str();
//		m_pkAnimationStates[i].m_pkState = new FvAvatarAnimationState(
//			m_ppkAvatarSkeletons, u32SkeletonNumber, pcAnimationName);
//		FV_ASSERT(m_pkAnimationStates[i].m_pkState);
//		m_pkAnimationStates[i].m_pkState->SetLoop(
//			m_spAvatarResource->m_pbAnimationsLoop[i]);
//
//		m_pkAnimationStates[i].m_i32Index = i;
//		m_pkAnimationStates[i].m_pkCallback = NULL;
//	}
//
//	m_u32ActiveAnimationNumber = u32ActiveAnimationNumber;
//
//	for(FvUInt32 i(0); i < FV_AVATAR_ANI_SET_SIZE; ++i)
//	{
//		m_akAnimationSet[i].m_i32Partition = DO_NOT_USE;
//		m_akAnimationSet[i].m_pkAnimationState = NULL;
//		m_akAnimationSet[i].m_bIsDirtyPartition = false;
//	}
//
//	m_u32AnimationUsed = 0;
//	m_fCacheDeltaTime = 0;
//	m_kAvatarGlobalParts.resize(m_spAvatarResource->m_u32PartNumber);
//	m_fDefaultBlendTime = GetGlobalDefaultBlendTime();
//	m_pkCallback = NULL;
//	m_bCacheTime = false;
//
//	if(m_spAvatarResource->m_pkBoundingBoxParams)
//	{
//		m_pkBoundingBoxParams = new BoundingBoxParams;
//
//		m_pkBoundingBoxParams->m_spPoint = ReferenceProvider(
//			m_spAvatarResource->m_pkBoundingBoxParams->m_kTailPointName.c_str());
//		UpdateBoundingBox();
//	}
//
//	
//}
////----------------------------------------------------------------------------
//void FvAvatar::UpdateBoundingBox()
//{
//	if(m_pkBoundingBoxParams)
//	{
//		FvVector3 kMin, kMax;
//
//		kMin.ParallelMultiply(m_spAvatarResource->m_pkBoundingBoxParams->m_kBaseBoundingBox.MinBounds(), m_kScale);
//		kMax.ParallelMultiply(m_spAvatarResource->m_pkBoundingBoxParams->m_kBaseBoundingBox.MaxBounds(), m_kScale);
//
//		m_pkBoundingBoxParams->m_kBoundingBox.SetBounds(
//			kMin + m_pkBoundingBoxParams->m_spPoint->GetVector3(),
//			kMax + m_pkBoundingBoxParams->m_spPoint->GetVector3());
//	}
//}
////----------------------------------------------------------------------------
//FvAvatar::~FvAvatar()
//{
//	FV_SAFE_DELETE(m_pkBoundingBoxParams);
//
//	m_kAvatarNodeList.BeginIterator();
//	while (!m_kAvatarNodeList.IsEnd())
//	{
//		FvRefList<AvatarNode*>::iterator iter =
//			m_kAvatarNodeList.GetIterator();
//
//		m_kAvatarNodeList.Next();
//
//		PutOff(iter->m_Content);
//	}
//
//	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//		_getCurrentSceneManager();
//	FV_ASSERT(pkSceneManager);
//	FV_ASSERT(m_ppkAvatarSkeletons);
//	FV_ASSERT(m_pkAvatarRootNode);
//
//	m_pkAvatarRootNode->removeAllChildren();
//
//	for(FvUInt32 i(0); i < m_u32ActiveAnimationNumber; ++i)
//	{
//		FV_SAFE_DELETE(m_pkAnimationStates[i].m_pkState);
//		FV_SAFE_DELETE(m_pkAnimationStates[i].m_pkCallback);
//	}
//	FV_SAFE_DELETE_ARRAY(m_pkAnimationStates);
//	
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		FV_SAFE_DELETE(m_ppkAvatarSkeletons[i]);
//	}
//	FV_SAFE_DELETE_ARRAY(m_ppkAvatarSkeletons);
//
//	pkSceneManager->destroySceneNode(
//		m_pkAvatarRootNode);
//
//	for(BindingProviderMap::iterator it = m_kProviderMap.begin();
//		it != m_kProviderMap.end(); ++it)
//	{
//		FV_ASSERT(it->second);
//		delete it->second;
//	}
//
//	if(m_spRootProvider)
//	{
//		m_spRootProvider->Detach0();
//		m_spRootProvider->Detach1();
//	}
//
//	for(FastProviders::iterator kFIt = m_kFastProviders.begin();
//		kFIt != m_kFastProviders.end(); ++kFIt)
//	{
//		FV_ASSERT(*kFIt);
//		(*kFIt)->m_pkAvatar = NULL;
//	}
//	m_kFastProviders.clear();
//}
////----------------------------------------------------------------------------
//SceneNode* FvAvatar::GetSceneNode()
//{
//	return m_pkAvatarRootNode;
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetAnimationUsedNumber(FvUInt32 u32Number)
//{
//#ifdef FV_MODEL_INFO
//	//FV_INFO_MSG("%p->FvAvatar::SetAnimationUsedNumber(%u)\n", this, u32Number);
//#endif
//	//FV_TRACE_MSG("%p->FvAvatar::SetAnimationUsedNumber(%u)\n", this, u32Number);
//
//	if(u32Number < m_u32AnimationUsed)
//	{
//		for(FvUInt32 i = u32Number; i < m_u32AnimationUsed; ++i)
//		{
//			if(m_akAnimationSet[i].m_pkAnimationState)
//			{
//				float fBlendTime = GetAnimationBlendTime(
//					m_akAnimationSet[i].m_pkAnimationState->m_i32Index,
//					DO_NOT_USE);
//
//				DisableAnimation(m_akAnimationSet[i].m_pkAnimationState,
//					fBlendTime);
//				m_akAnimationSet[i].m_pkAnimationState = NULL;
//				m_akAnimationSet[i].m_i32Partition = DO_NOT_USE;
//				m_akAnimationSet[i].m_bIsDirtyPartition = false;
//			}
//		}
//	}
//	m_u32AnimationUsed = u32Number;
//}
////----------------------------------------------------------------------------
//void FvAvatar::DisableAnimation(AnimationState* pkState, float fBlendTime)
//{
//	if(pkState)
//	{
//		if(pkState->m_pkState->SetEnabled(DO_NOT_USE, fBlendTime))
//		{
//			m_kDisabledBlendingAnimationStates.push_back(pkState);
//		}
//	}
//}
////----------------------------------------------------------------------------
//float FvAvatar::GetPartitionBlendTime(FvInt32 i32Partition0,
//	FvInt32 i32Partition1)
//{
//	std::pair<FvInt32,FvInt32> kChangeId;
//	kChangeId.first = i32Partition0;
//	kChangeId.second = i32Partition1;
//
//	FvAvatarResource::BlendMap::iterator it =
//		m_spAvatarResource->m_kPartitionBlendMap.find(
//		kChangeId);
//
//	if(it != m_spAvatarResource->m_kPartitionBlendMap.end())
//	{
//		return it->second;
//	}
//	else
//	{
//		return m_fDefaultBlendTime;
//	}
//}
////----------------------------------------------------------------------------
//float FvAvatar::GetAnimationBlendTime(FvInt32 i32Animation0,
//	FvInt32 i32Animation1)
//{
//	std::pair<FvInt32,FvInt32> kChangeId;
//	kChangeId.first = i32Animation0;
//	kChangeId.second = i32Animation1;
//
//	FvAvatarResource::BlendMap::iterator it =
//		m_spAvatarResource->m_kAnimationBlendMap.find(
//		kChangeId);
//
//	if(it != m_spAvatarResource->m_kAnimationBlendMap.end())
//	{
//		return it->second;
//	}
//	else
//	{
//		return m_fDefaultBlendTime;
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetPartitionReal(FvUInt32 u32Pos)
//{
//	FV_ASSERT(u32Pos < m_u32AnimationUsed);
//	FV_ASSERT(m_akAnimationSet[u32Pos].m_pkAnimationState);
//
//	FvInt32 i32SrcPartition = m_akAnimationSet[u32Pos].m_pkAnimationState->
//		m_pkState->GetActivePartition();
//	FvInt32 i32DstPartition = m_akAnimationSet[u32Pos].m_i32Partition;
//
//	if(i32SrcPartition != i32DstPartition)
//	{
//		float fBlendTime = GetPartitionBlendTime(i32SrcPartition,
//			i32DstPartition);
//
//		m_akAnimationSet[u32Pos].m_pkAnimationState->m_pkState->
//			ChangePartition(i32DstPartition, fBlendTime);
//	}
//
//	m_akAnimationSet[u32Pos].m_bIsDirtyPartition = false;
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetPartition(FvUInt32 u32Pos, FvInt32 i32Partition)
//{
//#ifdef FV_MODEL_INFO
//	//FV_INFO_MSG("%p->FvAvatar::SetPartition(%u,%X)\n", this, u32Pos, i32Partition);
//#endif
//
//	//FV_TRACE_MSG("%p->FvAvatar::SetPartition(%u,%X)\n", this, u32Pos, i32Partition);
//
//	FV_ASSERT(u32Pos < m_u32AnimationUsed);
//	m_akAnimationSet[u32Pos].m_i32Partition = (i32Partition < FvInt32(m_spAvatarResource->m_kSkeletonInfo[0].m_u32PartitonNumber)) ? i32Partition : DO_NOT_USE;
//	m_akAnimationSet[u32Pos].m_bIsDirtyPartition = true;
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetAnimation(FvUInt32 u32Pos, FvInt32 i32Animation,
//	bool bEnableBlend)
//{
//#ifdef FV_MODEL_INFO
//	//FV_INFO_MSG("%p->FvAvatar::SetAnimation(%u,%d,%d)\n", this, u32Pos, i32Animation, bEnableBlend);
//#endif
//
//	//FV_TRACE_MSG("%p->FvAvatar::SetAnimation(%u,%d,%d)\n", this, u32Pos, i32Animation, bEnableBlend);
//
//	FV_ASSERT(u32Pos < m_u32AnimationUsed);
//
//	FvInt32 i32LastAnimation = m_akAnimationSet[u32Pos].m_pkAnimationState
//		? m_akAnimationSet[u32Pos].m_pkAnimationState->m_i32Index
//		: DO_NOT_USE;
//
//	float fBlendTime(0);
//
//	if(bEnableBlend)
//	{
//		fBlendTime = GetAnimationBlendTime(
//			i32LastAnimation, i32Animation);
//	}
//
//	if(m_akAnimationSet[u32Pos].m_pkAnimationState)
//	{
//		if(m_akAnimationSet[u32Pos].m_pkAnimationState->m_i32Index ==
//			i32Animation) return;
//	}
//	else
//	{
//		if(i32Animation < 0) return;
//	}
//
//	AnimationState* pkChangeState = NULL;
//
//	if(i32Animation >= 0)
//	{
//		pkChangeState = &(m_pkAnimationStates[i32Animation]);
//	}
//
//	DisableAnimation(m_akAnimationSet[u32Pos].m_pkAnimationState,
//		fBlendTime);
//
//	m_akAnimationSet[u32Pos].m_pkAnimationState = pkChangeState;
//	m_akAnimationSet[u32Pos].m_bIsDirtyPartition = true;
//
//	if(pkChangeState)
//	{
//		FvInt32 i32Enable = pkChangeState->m_pkState->GetEnabled();
//
//		if(i32Enable >= 0)
//		{
//			m_akAnimationSet[i32Enable].m_pkAnimationState = NULL;
//		}
//
//		pkChangeState->m_pkState->SetEnabled(u32Pos, fBlendTime);
//		
//		pkChangeState->m_pkState->Active(m_u32ActiveBinding);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetAnimations(FvUInt32 u32Number, ...)
//{
//	va_list kArgPtr;
//	va_start(kArgPtr, u32Number);
//
//	FvInt32 m_ai32Partition[FV_AVATAR_ANI_SET_SIZE];
//	FvInt32	m_ai32Animation[FV_AVATAR_ANI_SET_SIZE];
//	bool m_abIsNew[FV_AVATAR_ANI_SET_SIZE];
//
//	if(m_u32AnimationUsed < u32Number)
//	{
//		SetAnimationUsedNumber(u32Number);
//	}
//
//	for(FvUInt32 i(0); i < u32Number; ++i)
//	{
//		m_ai32Partition[i] = va_arg(kArgPtr, FvInt32);
//		m_ai32Animation[i] = va_arg(kArgPtr, FvInt32);
//
//		FvInt32 i32Enable = GetEnable(m_ai32Animation[i]);
//		if(i32Enable >= 0)
//		{
//			FV_EXCHANGE_CLASS(m_akAnimationSet[i].m_i32Partition,
//				m_akAnimationSet[i32Enable].m_i32Partition, FvInt32);
//			FV_EXCHANGE_CLASS(m_akAnimationSet[i].m_pkAnimationState,
//				m_akAnimationSet[i32Enable].m_pkAnimationState,
//				AnimationState*);
//			if(m_akAnimationSet[i].m_pkAnimationState)
//			{
//				m_akAnimationSet[i].m_pkAnimationState->m_pkState->
//					SetEnabled(i, 0);
//			}
//			if(m_akAnimationSet[i32Enable].m_pkAnimationState)
//			{
//				m_akAnimationSet[i32Enable].m_pkAnimationState->m_pkState->
//					SetEnabled(i32Enable, 0);
//			}
//
//			SetPartition(i, m_ai32Partition[i]);
//
//			m_abIsNew[i] = false;
//		}
//		else
//		{
//			m_abIsNew[i] = true;
//		}
//	}
//
//	for(FvUInt32 i(0); i < u32Number; ++i)
//	{
//		if(m_abIsNew[i])
//		{
//			m_akAnimationSet[i].m_i32Partition = m_ai32Partition[i];
//			SetAnimation(i, m_ai32Animation[i]);
//		}
//	}
//
//	SetAnimationUsedNumber(u32Number);
//
//	va_end(kArgPtr);
//}
////----------------------------------------------------------------------------
//void FvAvatar::_AddAnimation(FvInt32 i32Animation, FvInt32 i32Partition)
//{
//	FV_ASSERT(i32Animation >= 0 &&
//		(i32Animation < (FvInt32)m_u32ActiveAnimationNumber));
//	FV_ASSERT(m_pkAnimationStates[i32Animation].m_pkState->GetEnabled() < 0);
//	FV_ASSERT(m_u32AnimationUsed < FV_AVATAR_ANI_SET_SIZE);
//	const FvUInt32 u32Pos = m_u32AnimationUsed++;
//	SetPartition(u32Pos, i32Partition);
//	SetAnimation(u32Pos, i32Animation);
//}
////----------------------------------------------------------------------------
//void FvAvatar::_DelAnimation(FvInt32 i32Animation)
//{
//	FV_ASSERT(i32Animation >= 0 &&
//		(i32Animation < (FvInt32)m_u32ActiveAnimationNumber));
//	const FvUInt32 u32Pos =
//		m_pkAnimationStates[i32Animation].m_pkState->GetEnabled();
//	FV_ASSERT(u32Pos >= 0 && u32Pos < m_u32AnimationUsed);
//
//	if(u32Pos != m_u32AnimationUsed - 1)
//	{
//		const FvUInt32 u32LastAnimation = GetAnimation(m_u32AnimationUsed - 1);
//		SetAnimation(u32Pos, u32LastAnimation);
//		FV_ASSERT(!m_akAnimationSet[m_u32AnimationUsed - 1].m_pkAnimationState);
//	}
//	
//	SetAnimationUsedNumber(m_u32AnimationUsed - 1);
//}
////----------------------------------------------------------------------------
//void FvAvatar::_ChangeAnimation(FvInt32 i32Old, FvInt32 i32New)
//{
//	FV_ASSERT(i32Old >= 0 &&
//		(i32Old < (FvInt32)m_u32ActiveAnimationNumber));
//	const FvUInt32 u32Pos =
//		m_pkAnimationStates[i32Old].m_pkState->GetEnabled();
//	FV_ASSERT(u32Pos >= 0 && u32Pos < m_u32AnimationUsed);
//	SetAnimation(u32Pos, i32New);
//}
////----------------------------------------------------------------------------
//void FvAvatar::_ChangePartition(FvInt32 i32Animation, FvInt32 i32Partition)
//{
//	FV_ASSERT(i32Animation >= 0 &&
//		(i32Animation < (FvInt32)m_u32ActiveAnimationNumber));
//	const FvUInt32 u32Pos =
//		m_pkAnimationStates[i32Animation].m_pkState->GetEnabled();
//	FV_ASSERT(u32Pos >= 0 && u32Pos < m_u32AnimationUsed);
//	SetPartition(u32Pos, i32Partition);
//}
////----------------------------------------------------------------------------
//void FvAvatar::AddTime(float fDeltaTime)
//{
//	for(FvUInt32 i(0); i < m_u32AnimationUsed; ++i)
//	{
//		if(m_akAnimationSet[i].m_pkAnimationState)
//		{
//			if(m_akAnimationSet[i].m_bIsDirtyPartition)
//			{
//				SetPartitionReal(i);
//			}
//
//			m_akAnimationSet[i].m_pkAnimationState->m_pkState->
//				AddTime(fDeltaTime, m_bCacheTime);
//		}
//	}
//
//	if(m_bCacheTime)
//	{
//		m_fCacheDeltaTime += fDeltaTime;
//	}
//	else
//	{
//		if(m_fCacheDeltaTime)
//		{
//			fDeltaTime += m_fCacheDeltaTime;
//			m_fCacheDeltaTime = 0;
//		}
//
//		AnimationStateList::iterator it =
//			m_kDisabledBlendingAnimationStates.begin();
//
//		while(it != m_kDisabledBlendingAnimationStates.end())
//		{
//			AnimationStateList::iterator temp = it;
//			++it;
//			if((*temp)->m_pkState->AddTimeDisable(fDeltaTime))
//			{
//				m_kDisabledBlendingAnimationStates.erase(temp);
//			}
//		}
//	}
//
//	for(BindingProviderMap::iterator it = m_kProviderMap.begin();
//		it != m_kProviderMap.end(); ++it)
//	{
//		it->second->UpdateBinding(m_u32ActiveBinding, GetSceneNode(), true);
//	}
//
//	if(m_spRootProvider)
//	{
//		GetSceneNode()->_update(false, false);
//	}
//
//	FV_ASSERT(m_ppkAvatarSkeletons[m_u32ActiveBinding]);
//	FvAvatarSkeleton::LinkMap::iterator kIt = m_ppkAvatarSkeletons[m_u32ActiveBinding]->m_kLinkNodes.begin();
//	for (; kIt != m_ppkAvatarSkeletons[m_u32ActiveBinding]->m_kLinkNodes.end(); ++kIt)
//	{
//		Ogre::SceneNode *pkNode = kIt->first;
//		FvAvatarLinkNode *pkLinkNode = kIt->second;
//		FV_ASSERT(pkNode && pkLinkNode);
//		pkNode->setPosition(
//			pkLinkNode->m_spTag->GetData0() + pkLinkNode->m_spTag->GetData1() * pkLinkNode->m_pkEffectData->m_kPosition);
//		pkNode->setOrientation(
//			pkLinkNode->m_spTag->GetData1() * pkLinkNode->m_pkEffectData->m_kQuaternion);
//	}
//
//	UpdateBoundingBox();
//}
////----------------------------------------------------------------------------
//FvInt32 FvAvatar::GetPartition(const char* pcPartitionName)
//{
//	FvInt32 i32Partition = DO_NOT_USE;
//	if(pcPartitionName)
//	{
//		FvStringMap<FvUInt32>::iterator it =
//			m_spAvatarResource->m_kPartitionNames.find(pcPartitionName);
//		if(it != m_spAvatarResource->m_kPartitionNames.end())
//		{
//			i32Partition = it->second;
//		}
//	}
//
//	return i32Partition;
//}
////----------------------------------------------------------------------------
//FvInt32 FvAvatar::GetAnimation(const char* pcAnimationName)
//{
//	FvInt32 i32Animation = DO_NOT_USE;
//	if(pcAnimationName)
//	{
//		FvStringMap<FvUInt32>::iterator it =
//			m_spAvatarResource->m_kAnimationNames.find(pcAnimationName);
//		if(it != m_spAvatarResource->m_kAnimationNames.end())
//		{
//			i32Animation = it->second;
//		}
//	}
//	return i32Animation;
//}
////----------------------------------------------------------------------------
//FvInt32 FvAvatar::GetPartition(FvUInt32 u32Pos)
//{
//	return m_akAnimationSet[u32Pos].m_i32Partition;
//}
////----------------------------------------------------------------------------
//FvInt32 FvAvatar::GetAnimation(FvUInt32 u32Pos)
//{
//	return m_akAnimationSet[u32Pos].m_pkAnimationState
//		? m_akAnimationSet[u32Pos].m_pkAnimationState->m_i32Index : DO_NOT_USE;
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetCacheTime(bool bIsCache)
//{
//	m_bCacheTime = bIsCache;
//}
////----------------------------------------------------------------------------
//bool FvAvatar::IsCacheTime()
//{
//	return m_bCacheTime;
//}
////----------------------------------------------------------------------------
//void FvAvatar::Active(FvUInt32 u32Binding)
//{
//	FV_ASSERT(u32Binding >=0 && u32Binding < m_u32BindingNumber);
//	if(m_u32ActiveBinding != u32Binding)
//	{
//		m_pkAvatarRootNode->removeAllChildren();
//		m_pkAvatarRootNode->addChild(
//			m_ppkAvatarSkeletons[u32Binding]->GetSceneNode());
//		for(FvUInt32 i(0); i < m_u32AnimationUsed; ++i)
//		{
//			if(m_akAnimationSet[i].m_pkAnimationState)
//			{
//				m_akAnimationSet[i].m_pkAnimationState->m_pkState->
//					Active(u32Binding);
//			}
//		}
//		for(AnimationStateList::iterator it =
//			m_kDisabledBlendingAnimationStates.begin(); it !=
//			m_kDisabledBlendingAnimationStates.end(); ++it)
//		{
//			(*it)->m_pkState->Active(u32Binding);
//		}
//		m_u32ActiveBinding = u32Binding;
//	}
//}
////----------------------------------------------------------------------------
//FvUInt32 FvAvatar::GetBindingNumber()
//{
//	return m_u32BindingNumber;
//}
////----------------------------------------------------------------------------
//FvUInt32 FvAvatar::GetActiveBinding()
//{
//	return m_u32ActiveBinding;
//}
////----------------------------------------------------------------------------
//FvUInt32 FvAvatar::GetAnimationNumber()
//{
//	return m_u32ActiveAnimationNumber;
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetWeight(FvUInt32 u32Animation, float fWeight)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//	m_pkAnimationStates[u32Animation].m_pkState->SetWeight(fWeight);
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetTimeScale(FvUInt32 u32Animation, float fTimeScale)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//	m_pkAnimationStates[u32Animation].m_pkState->SetTimeScale(fTimeScale);
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetLoop(FvUInt32 u32Animation, bool bLoop)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//	m_pkAnimationStates[u32Animation].m_pkState->SetLoop(bLoop);
//}
////----------------------------------------------------------------------------
//void FvAvatar::EnableCallback(FvUInt32 u32Animation)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//	AnimationState* pkState = &m_pkAnimationStates[u32Animation];
//	if(!pkState->m_pkCallback)
//	{
//		pkState->m_pkCallback = new AnimationStateCallback(this);
//		FV_ASSERT(pkState->m_pkCallback);
//	}
//	pkState->m_pkState->SetCallback(pkState->m_pkCallback);
//}
////----------------------------------------------------------------------------
//void FvAvatar::DisableCallback(FvUInt32 u32Animation)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//	m_pkAnimationStates[u32Animation].m_pkState->SetCallback(NULL);
//}
////----------------------------------------------------------------------------
//void FvAvatar::RegisterCallback(FvUInt32 u32Animation,
//	FvAvatarAnimationState::EventType eType, const char* pcName)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//
//	if(eType == FvAvatarAnimationState::ANI_KEY_EVENT)
//	{
//		FV_ASSERT(pcName);
//		FvAvatarResource::KeyMap::iterator it =
//			(m_spAvatarResource->m_pkAnimationKey[u32Animation]).find(pcName);
//
//		if(it != (m_spAvatarResource->m_pkAnimationKey[u32Animation]).end())
//		{
//			AnimationState* pkState = &m_pkAnimationStates[u32Animation];
//			FV_ASSERT(pkState->m_pkCallback);
//			pkState->m_pkState->RegisterCallback(eType, pcName, it->second);
//		}
//	}
//	else
//	{
//		AnimationState* pkState = &m_pkAnimationStates[u32Animation];
//		FV_ASSERT(pkState->m_pkCallback);
//		pkState->m_pkState->RegisterCallback(eType, pcName, 0);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::UnregisterCallback(FvUInt32 u32Animation,
//	FvAvatarAnimationState::EventType eType, const char* pcName)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//	AnimationState* pkState = &m_pkAnimationStates[u32Animation];
//	FV_ASSERT(pkState->m_pkCallback);
//	pkState->m_pkState->UnregisterCallback(eType, pcName);
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetCallback(FvUInt32 u32Animation, AvatarCallback* pkCallback)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//	AnimationState* pkState = &m_pkAnimationStates[u32Animation];
//	pkState->m_pkCallback->SetCallback(pkCallback);
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetCallback(AvatarCallback* pkCallback)
//{
//	m_pkCallback = pkCallback;
//}
////----------------------------------------------------------------------------
//FvInt32 FvAvatar::GetEnable(FvInt32 i32Animation)
//{
//	FV_ASSERT(i32Animation >= 0);
//	FV_ASSERT(i32Animation < ((int)m_u32ActiveAnimationNumber));
//	return m_pkAnimationStates[i32Animation].m_pkState->GetEnabled();
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetTimePosition(FvUInt32 u32Pos, float fTimePos)
//{
//#ifdef FV_MODEL_INFO
//	//FV_INFO_MSG("%p->FvAvatar::SetTimePosition(%u,%f)\n", this, u32Pos, fTimePos);
//#endif
//
//	//FV_TRACE_MSG("%p->FvAvatar::SetTimePosition(%u,%f)\n", this, u32Pos, fTimePos);
//
//	FV_ASSERT(u32Pos < m_u32AnimationUsed);
//
//	if(m_akAnimationSet[u32Pos].m_pkAnimationState)
//	{
//		m_akAnimationSet[u32Pos].m_pkAnimationState->m_pkState->SetTimePosition(
//			fTimePos);
//	}
//}
////----------------------------------------------------------------------------
//float FvAvatar::GetTimePosition(FvUInt32 u32Pos)
//{
//	FV_ASSERT(u32Pos < m_u32AnimationUsed);
//	
//	if(m_akAnimationSet[u32Pos].m_pkAnimationState)
//		return m_akAnimationSet[u32Pos].m_pkAnimationState->m_pkState->GetTimePosition();
//	else
//		return 0;
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetManualTime(FvUInt32 u32Animation, bool bIsManualTime)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//
//	m_pkAnimationStates[u32Animation].m_pkState->SetManualTime(
//		bIsManualTime);
//}
////----------------------------------------------------------------------------
//float FvAvatar::GetLength(FvUInt32 u32Animation)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//	
//	return m_pkAnimationStates[u32Animation].m_pkState->GetLength();
//}
////----------------------------------------------------------------------------
//bool FvAvatar::IsLoop(FvUInt32 u32Animation)
//{
//	FV_ASSERT(u32Animation < m_u32ActiveAnimationNumber);
//
//	return m_pkAnimationStates[u32Animation].m_pkState->IsLoop();
//}
////----------------------------------------------------------------------------
//bool FvAvatar::IsManualTime(FvUInt32 u32Pos)
//{
//	FV_ASSERT(u32Pos < m_u32AnimationUsed);
//	FV_ASSERT(m_akAnimationSet[u32Pos].m_pkAnimationState);
//
//	return m_akAnimationSet[u32Pos].m_pkAnimationState->m_pkState->
//		IsManualTime();
//}
////----------------------------------------------------------------------------
//const char* FvAvatar::GetAnimationName(FvInt32 i32Animation)
//{
//	FV_ASSERT((i32Animation >= 0) &&
//		(i32Animation < (int)m_u32ActiveAnimationNumber));
//	return m_pkAnimationStates[i32Animation].m_pkState->GetName();
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetDefaultBlendTime(float fTime)
//{
//	m_fDefaultBlendTime = fTime;
//}
////----------------------------------------------------------------------------
//float FvAvatar::GetDefaultBlendTime()
//{
//	return m_fDefaultBlendTime;
//}
////----------------------------------------------------------------------------
//FvAvatar::AnimationStateCallback::AnimationStateCallback(FvAvatar* pkAvatar)
//{
//	FV_ASSERT(pkAvatar);
//	m_pkParent = pkAvatar;
//	m_pkCallback = NULL;
//}
////----------------------------------------------------------------------------
//FvAvatar::AnimationStateCallback::~AnimationStateCallback()
//{
//
//}
////----------------------------------------------------------------------------
//void FvAvatar::AnimationStateCallback::EventCome(
//	FvAvatarAnimationState* pkAnimationState,
//	FvAvatarAnimationState::EventType eType, const char* pcMessage)
//{
//	FV_ASSERT(m_pkParent);
//	FV_ASSERT(pkAnimationState->GetEnabled() >= 0);
//
//	FvInt32 i32Animation =
//		m_pkParent->m_akAnimationSet[pkAnimationState->GetEnabled()].
//		m_pkAnimationState->m_i32Index;
//
//	if(m_pkCallback)
//		m_pkCallback->EventCome(m_pkParent, i32Animation,
//		eType, pcMessage);
//
//	if(m_pkParent->m_pkCallback)
//		m_pkParent->m_pkCallback->EventCome(m_pkParent,
//		i32Animation, eType, pcMessage);
//}
////----------------------------------------------------------------------------
//void FvAvatar::AnimationStateCallback::SetCallback(AvatarCallback* pkCallback)
//{
//	m_pkCallback = pkCallback;
//}
////----------------------------------------------------------------------------
//class FvAvatar::AvatarNodePrivateData
//{
//public:
//	struct PartEntity
//	{
//		PartEntity()
//		{
//			FvZeroMemory(m_apkEntities, sizeof(m_apkEntities));
//			m_u32DisplayFlags = 0;
//		}
//
//		~PartEntity()
//		{
//			Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//				_getCurrentSceneManager();
//			FV_ASSERT(pkSceneManager);
//			for(FvUInt32 i(0); i < FV_AVATAR_MAX_BINDING_NUMBER; ++i)
//			{
//				if(m_apkEntities[i])
//				{
//					pkSceneManager->destroyMovableObject(
//						m_apkEntities[i]);
//					m_apkEntities[i] = NULL;
//
//				}
//			}
//		}
//
//		FvAvatarEntity* m_apkEntities[FV_AVATAR_MAX_BINDING_NUMBER];
//		FvUInt32 m_u32Priority;
//		struct Node : public FvRefNode2<void*>
//		{
//			Node() : FvRefNode2<void*>()
//			{
//
//			}
//
//			Node(const Node& kNode) : FvRefNode2<void*>()
//			{
//
//			}
//
//			Node& operator = (const Node& kNode)
//			{
//				return *this;
//			}
//
//			FvUInt32 m_u32LocalPart;
//			FvUInt32 m_u32GlobalPart;
//		};
//		std::vector<Node> m_kLocalParts;
//
//		void Add(FvUInt32 u32Flag)
//		{
//			FV_MASK_ADD(m_u32DisplayFlags, u32Flag);
//		}
//
//		void Del(FvUInt32 u32Flag)
//		{
//			FV_MASK_DEL(m_u32DisplayFlags, u32Flag);
//		}
//
//		void Update(FvUInt32 u32BindingNumber)
//		{
//			for(FvUInt32 i(0); i < u32BindingNumber; ++i)
//			{
//				FV_ASSERT(m_apkEntities[i]);
//				m_apkEntities[i]->SetDisplayFlags(m_u32DisplayFlags);
//			}
//		}
//
//	private:
//		FvUInt32 m_u32DisplayFlags;
//	};
//
//	struct LinkEntity
//	{
//		LinkEntity()
//		{
//			FvZeroMemory(m_apkEntities, sizeof(m_apkEntities));
//		}
//
//		~LinkEntity()
//		{
//			Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//				_getCurrentSceneManager();
//			FV_ASSERT(pkSceneManager);
//			for(FvUInt32 i(0); i < FV_AVATAR_MAX_BINDING_NUMBER; ++i)
//			{
//				if(m_apkEntities[i])
//				{
//					pkSceneManager->destroyMovableObject(
//						m_apkEntities[i]);
//					m_apkEntities[i] = NULL;
//
//				}
//			}
//		}
//
//		Ogre::Entity* m_apkEntities[FV_AVATAR_MAX_BINDING_NUMBER];
//		FvString m_kTagName;
//	};
//
//	struct LinkEffect
//	{
//		LinkEffect()
//		{
//			FvZeroMemory(m_apkNodes, sizeof(m_apkNodes));
//			FvZeroMemory(m_apkEffects, sizeof(m_apkEffects));
//		}
//
//		~LinkEffect()
//		{
//			ParticleUniverse::ParticleSystemManager *pkManager = 
//				ParticleUniverse::ParticleSystemManager::getSingletonPtr();
//			Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//				_getCurrentSceneManager();
//			FV_ASSERT(pkSceneManager && pkManager);
//			for(FvUInt32 i(0); i < FV_AVATAR_MAX_BINDING_NUMBER; ++i)
//			{
//				if(!m_apkNodes[i]) continue;
//				FV_ASSERT(m_apkEffects[i]);
//				m_apkNodes[i]->detachObject(m_apkEffects[i]);
//				pkManager->destroyParticleSystem(m_apkEffects[i], pkSceneManager);
//				pkSceneManager->destroySceneNode(m_apkNodes[i]);
//			}
//		}
//
//		Ogre::SceneNode *m_apkNodes[FV_AVATAR_MAX_BINDING_NUMBER];
//		ParticleUniverse::ParticleSystem *m_apkEffects[FV_AVATAR_MAX_BINDING_NUMBER];
//		FvString m_kTagName;
//	};
//
//	struct Cache
//	{
//		enum State
//		{
//			CACHE_NONE,
//			CACHE_PART,
//			CACHE_LINK
//		};
//		const char* m_pcName;
//		const char* m_pcMesh;
//		const char* m_pcMaterial;
//		BackgroundProcessTicket m_ulMeshTicket;
//		BackgroundProcessTicket m_ulMaterialTicket;
//		FvUInt32 m_u32Entity;
//		FvUInt32 m_u32Binding;
//		bool m_bMeshEnabled;
//		bool m_bMaterialEnabled;
//		State m_eState;
//	};
//
//	enum LoadState
//	{
//		LOAD_NONE,
//		LOAD_ENTITY,
//		LOAD_MATERIAL
//	};
//
//	class LoadListener : public ResourceBackgroundQueue::Listener
//	{
//	public:
//		LoadListener(FvAvatar::AvatarNodePrivateData* pkParent) :
//			m_pkParent(pkParent), m_u32NeedCallback(0) {}
//
//		virtual ~LoadListener()
//		{
//			if(m_pkParent) m_pkParent->m_pkLoadListener = NULL;
//		}
//
//		virtual void operationCompleted(BackgroundProcessTicket ticket,
//			const BackgroundProcessResult& result)
//		{
//			--m_u32NeedCallback;
//			if(m_pkParent)
//			{
//				if(!m_pkParent->operationCompleted(ticket, result))
//					return;
//			}
//
//			if(!m_u32NeedCallback)
//				delete this;
//		}
//
//		FvAvatar::AvatarNodePrivateData* m_pkParent;
//		FvUInt32 m_u32NeedCallback;
//
//	};
//
//	class MaterialListener : public ResourceBackgroundQueue::Listener,
//		public FvRefNode2<MaterialListener*>
//	{
//	public:
//		MaterialListener(FvAvatar::AvatarNodePrivateData* pkParent,
//			FvUInt32 u32EntityNumber) :
//			m_pkParent(pkParent), m_kMaterialDatas(u32EntityNumber)
//		{
//			m_pkParent->m_kMaterialListeners.AttachBack(*this);
//		}
//
//		virtual ~MaterialListener()
//		{
//			Detach();
//		}
//
//		virtual void operationCompleted(BackgroundProcessTicket ticket,
//			const BackgroundProcessResult& result)
//		{
//			FV_ASSERT(!result.error);
//			if(m_pkParent)
//			{
//				bool bRes = true;
//
//				for(FvUInt32 i(0); i < m_kMaterialDatas.size(); ++i)
//				{
//					if(!m_kMaterialDatas[i].m_bIsEnable)
//					{
//						if(m_kMaterialDatas[i].m_ulMaterialTicket == ticket)
//						{
//							m_kMaterialDatas[i].m_spMaterial =
//								MaterialManager::getSingleton().getByName(
//								m_kMaterialDatas[i].m_kName);
//							FV_ASSERT(!m_kMaterialDatas[i].m_spMaterial.isNull());
//							m_kMaterialDatas[i].m_bIsEnable = true;
//						}
//						else
//						{
//							bRes = false;
//						}
//					}
//				}
//
//				if(bRes)
//				{
//					for(FvUInt32 i(0); i < m_kMaterialDatas.size(); ++i)
//					{
//						FV_ASSERT(m_kMaterialDatas[i].m_pkEntity);
//						FV_ASSERT(!m_kMaterialDatas[i].m_spMaterial.isNull());
//						m_kMaterialDatas[i].m_pkEntity->setMaterial(
//							m_kMaterialDatas[i].m_spMaterial);
//					}
//				}
//				else
//				{
//					return;
//				}
//			}
//			
//			delete this;
//		}
//
//		struct MaterialData
//		{
//			FvString m_kName;
//			MaterialPtr m_spMaterial;
//			BackgroundProcessTicket m_ulMaterialTicket;
//			Ogre::Entity* m_pkEntity;
//			bool m_bIsEnable;
//		};
//
//		FvAvatar::AvatarNodePrivateData* m_pkParent;
//		std::vector<MaterialData> m_kMaterialDatas;
//	};
//
//	AvatarNodePrivateData(FvAvatar* pkAvatar,
//		AvatarNode* pkNode) : m_pkAvatar(pkAvatar),
//		m_pkNode(pkNode), m_kPartEntities(
//		pkNode->m_kPartEntityDatas.size()),
//		m_kLinkEntities(pkNode->m_kLinkEntityDatas.size()),
//		m_kLinkEffects(pkNode->m_kLinkEffectDatas.size()),
//		m_pkLoadListener(NULL)
//	{
//		m_kCache.m_u32Entity = 0;
//		m_kCache.m_u32Binding = 0;
//		if(m_kPartEntities.size())
//		{
//			m_kCache.m_eState = Cache::CACHE_PART;
//		}
//		else if(m_kLinkEntities.size())
//		{
//			m_kCache.m_eState = Cache::CACHE_LINK;
//		}
//		else
//		{
//			m_kCache.m_eState = Cache::CACHE_NONE;
//		}
//
//		for(FvUInt32 i(0); i < m_kPartEntities.size(); ++i)
//		{
//			std::vector<FvUInt32>& kParts =
//				m_pkNode->m_kPartEntityDatas[i].m_kParts;
//			m_kPartEntities[i].m_kLocalParts.resize(kParts.size());
//			for(FvUInt32 j(0); j < m_kPartEntities[i].m_kLocalParts.size(); ++j)
//			{
//				m_kPartEntities[i].m_kLocalParts[j].m_Content = &m_kPartEntities[i];
//				m_kPartEntities[i].m_kLocalParts[j].m_u32LocalPart = j;
//				m_kPartEntities[i].m_kLocalParts[j].m_u32GlobalPart = kParts[j];
//			}
//			FvUInt32 u32Priority =
//				m_pkNode->m_kPartEntityDatas[i].m_u32Priority;
//			m_kPartEntities[i].m_u32Priority = u32Priority;
//		}
//
//		for(FvUInt32 i(0); i < m_kLinkEntities.size(); ++i)
//		{
//			if(!m_pkAvatar->GetAvatarResource()->HasAliasName(m_pkNode->m_kLinkEntityDatas[i].m_pcNameOfLink))
//				m_kLinkEntities[i].m_kTagName = m_pkNode->m_kLinkEntityDatas[i].m_pcNameOfLink;
//			else
//				m_kLinkEntities[i].m_kTagName = m_pkAvatar->GetAvatarResource()->GetAliasName(m_pkNode->m_kLinkEntityDatas[i].m_pcNameOfLink);
//		}	
//
//		for(FvUInt32 i(0); i < m_kLinkEffects.size(); ++i)
//		{
//			m_kLinkEffects[i].m_kTagName =
//				m_pkNode->m_kLinkEffectDatas[i].m_pcNameOfLink;
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				ParticleUniverse::ParticleSystemManager *pkManager = 
//					ParticleUniverse::ParticleSystemManager::getSingletonPtr();
//				Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//					_getCurrentSceneManager();
//				FV_ASSERT(!m_kLinkEffects[i].m_apkNodes[j] && 
//					!m_kLinkEffects[i].m_apkEffects[j]);
//				FV_ASSERT(pkSceneManager && pkManager);
//				m_kLinkEffects[i].m_apkNodes[j] = pkSceneManager->createSceneNode();
//				m_kLinkEffects[i].m_apkEffects[j] = pkManager->createParticleSystem(
//					Ogre::String(m_pkNode->m_kLinkEffectDatas[i].m_apcNames[j]),
//					Ogre::String(m_pkNode->m_kLinkEffectDatas[i].m_apcEffects[j]),
//					pkSceneManager);
//				m_kLinkEffects[i].m_apkEffects[j]->setRenderQueueGroup(89);	
//				m_kLinkEffects[i].m_apkNodes[j]->attachObject(m_kLinkEffects[i].m_apkEffects[j]);
//			}
//		}
//
//		m_eState = LOAD_ENTITY;
//		m_eRenderState = FvAvatar::AvatarNode::NORMAL;
//	}
//
//	~AvatarNodePrivateData()
//	{
//		if(m_pkLoadListener)
//			m_pkLoadListener->m_pkParent = NULL;
//
//		FvRefList<MaterialListener*>::iterator iter =
//			m_kMaterialListeners.GetHead();
//		while (!m_kMaterialListeners.IsEnd(iter))
//		{
//			(*iter).m_Content->m_pkParent = NULL;
//			FvRefList<MaterialListener*>::Next(iter);
//		}
//
//		m_kMaterialListeners.Clear();
//	}
//
//	void RefreshMaterials()
//	{
//		FV_ASSERT(m_eState != LOAD_ENTITY);
//		MaterialListener* pkListener = new MaterialListener(
//			this, m_pkAvatar->GetBindingNumber() * (m_kPartEntities.size() +
//			m_kLinkEntities.size()));
//		m_kMaterialListeners.AttachBack(*pkListener);
//		
//		FvUInt32 n(0);
//
//		for(FvUInt32 i(0); i < m_kPartEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				pkListener->m_kMaterialDatas[n].m_kName =
//					m_pkNode->m_kPartEntityDatas[i].m_apcMaterials[j];
//
//				pkListener->m_kMaterialDatas[n].m_ulMaterialTicket =
//					ResourceBackgroundQueue::getSingleton().load(
//					MaterialManager::getSingleton().getResourceType(),
//					pkListener->m_kMaterialDatas[n].m_kName,
//					FvAvatarResourceManager::GetGroupName(),
//					false, 0, 0, pkListener);
//
//				pkListener->m_kMaterialDatas[n].m_pkEntity =
//					m_kPartEntities[i].m_apkEntities[j];
//
//				pkListener->m_kMaterialDatas[n].m_bIsEnable = false;
//				++n;
//			}
//		}
//
//		for(FvUInt32 i(0); i < m_kLinkEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				pkListener->m_kMaterialDatas[n].m_kName =
//					m_pkNode->m_kLinkEntityDatas[i].m_apcMaterials[j];
//
//				pkListener->m_kMaterialDatas[n].m_ulMaterialTicket =
//					ResourceBackgroundQueue::getSingleton().load(
//					MaterialManager::getSingleton().getResourceType(),
//					pkListener->m_kMaterialDatas[n].m_kName,
//					FvAvatarResourceManager::GetGroupName(),
//					false, 0, 0, pkListener);
//
//				pkListener->m_kMaterialDatas[n].m_pkEntity =
//					m_kLinkEntities[i].m_apkEntities[j];
//
//				pkListener->m_kMaterialDatas[n].m_bIsEnable = false;
//				++n;
//			}
//		}
//	}
//
//	void RefreshLink()
//	{
//		for(FvUInt32 i(0); i < m_kLinkEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				FV_ASSERT(m_kLinkEntities[i].m_apkEntities[j]);
//
//				if(m_kLinkEntities[i].m_kTagName == "")
//				{
//					m_pkAvatar->m_ppkAvatarSkeletons[i]->GetSceneNode()->detachObject(
//						m_kLinkEntities[i].m_apkEntities[j]);
//				}
//				else
//				{
//					m_pkAvatar->m_ppkAvatarSkeletons[i]->m_pkMainEntity->
//						detachObjectFromBone(m_kLinkEntities[i].m_apkEntities[j]);
//				}
//
//				if(!m_pkAvatar->GetAvatarResource()->HasAliasName(m_pkNode->m_kLinkEntityDatas[i].m_pcNameOfLink))
//					m_kLinkEntities[i].m_kTagName = m_pkNode->m_kLinkEntityDatas[i].m_pcNameOfLink;
//				else
//					m_kLinkEntities[i].m_kTagName = m_pkAvatar->GetAvatarResource()->GetAliasName(m_pkNode->m_kLinkEntityDatas[i].m_pcNameOfLink);
//		
//				if(m_kLinkEntities[i].m_kTagName == "")
//				{
//					m_pkAvatar->m_ppkAvatarSkeletons[i]->GetSceneNode()->attachObject(
//						m_kLinkEntities[i].m_apkEntities[j]);
//				}
//				else
//				{
//					/*m_pkAvatar->m_ppkAvatarSkeletons[i]->m_pkMainEntity->
//						detachObjectFromBone(m_kLinkEntities[i].m_apkEntities[j]);*/
//
//					m_pkAvatar->m_ppkAvatarSkeletons[i]->m_pkMainEntity->attachObjectToBone(
//						m_kLinkEntities[i].m_kTagName,
//						m_kLinkEntities[i].m_apkEntities[j]);
//				}
//			}
//		}
//
//		for(FvUInt32 i(0); i < m_kLinkEffects.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				Ogre::SceneNode *pkNode = m_kLinkEffects[i].m_apkNodes[j];
//				if(!pkNode) continue;
//				FvAvatarSkeleton::LinkMap::iterator kFind = m_pkAvatar->m_ppkAvatarSkeletons[i]->m_kLinkNodes.find(pkNode);
//				if(kFind != m_pkAvatar->m_ppkAvatarSkeletons[i]->m_kLinkNodes.end())
//				{
//					const FvAvatar::ProviderPtr &spTagPoint = m_pkAvatar->ReferenceProvider(m_pkNode->m_kLinkEffectDatas[i].m_pcNameOfLink);
//					if(spTagPoint)
//					{
//						m_kLinkEffects[i].m_kTagName = m_pkNode->m_kLinkEffectDatas[i].m_pcNameOfLink;
//						kFind->second->m_spTag = spTagPoint;
//					}
//				}
//			}
//		}
//	}
//
//	void Load()
//	{
//		if(!m_pkLoadListener)
//		{
//			m_pkLoadListener = new LoadListener(this);
//		}
//
//		FV_ASSERT(m_eState == LOAD_ENTITY);
//
//		const FvUInt32 u32Entity = m_kCache.m_u32Entity;
//		const FvUInt32 u32Binding = m_kCache.m_u32Binding;
//		FV_ASSERT(m_pkNode);
//		FV_ASSERT(m_pkAvatar);
//		FV_ASSERT(u32Binding < m_pkAvatar->GetBindingNumber());
//
//		if(m_kCache.m_eState == Cache::CACHE_PART)
//		{
//			FV_ASSERT(u32Entity < m_pkNode->m_kPartEntityDatas.size());
//			FV_ASSERT(!m_kPartEntities[u32Entity].m_apkEntities[u32Binding]);
//
//			m_kCache.m_pcName =
//				m_pkNode->m_kPartEntityDatas[u32Entity].m_apcNames[u32Binding];
//			m_kCache.m_pcMesh =
//				m_pkNode->m_kPartEntityDatas[u32Entity].m_apcMeshes[u32Binding];
//			m_kCache.m_pcMaterial =
//				m_pkNode->m_kPartEntityDatas[u32Entity].m_apcMaterials[u32Binding];
//
//			m_kCache.m_ulMaterialTicket =
//				ResourceBackgroundQueue::getSingleton().load(
//				MaterialManager::getSingleton().getResourceType(),
//				m_kCache.m_pcMaterial, FvAvatarResourceManager::GetGroupName(),
//				false, 0, 0, m_pkLoadListener);
//			m_kCache.m_ulMeshTicket =
//				ResourceBackgroundQueue::getSingleton().load(
//				MeshManager::getSingleton().getResourceType(),
//				m_kCache.m_pcMesh, FvAvatarResourceManager::GetGroupName(),
//				false, 0, 0, m_pkLoadListener);
//
//			m_pkLoadListener->m_u32NeedCallback = 2;
//
//			m_kCache.m_bMeshEnabled = false;
//			m_kCache.m_bMaterialEnabled = false;
//		}
//		else if(m_kCache.m_eState == Cache::CACHE_LINK)
//		{
//			FV_ASSERT(u32Entity < m_pkNode->m_kLinkEntityDatas.size());
//			FV_ASSERT(!m_kLinkEntities[u32Entity].m_apkEntities[u32Binding]);
//
//			m_kCache.m_pcName =
//				m_pkNode->m_kLinkEntityDatas[u32Entity].m_apcNames[u32Binding];
//			m_kCache.m_pcMesh =
//				m_pkNode->m_kLinkEntityDatas[u32Entity].m_apcMeshes[u32Binding];
//			m_kCache.m_pcMaterial =
//				m_pkNode->m_kLinkEntityDatas[u32Entity].m_apcMaterials[u32Binding];
//
//			m_kCache.m_ulMaterialTicket =
//				ResourceBackgroundQueue::getSingleton().load(
//				MaterialManager::getSingleton().getResourceType(),
//				m_kCache.m_pcMaterial, FvAvatarResourceManager::GetGroupName(),
//				false, 0, 0, m_pkLoadListener);
//			m_kCache.m_ulMeshTicket =
//				ResourceBackgroundQueue::getSingleton().load(
//				MeshManager::getSingleton().getResourceType(),
//				m_kCache.m_pcMesh, FvAvatarResourceManager::GetGroupName(),
//				false, 0, 0, m_pkLoadListener);
//
//			m_pkLoadListener->m_u32NeedCallback = 2;
//
//			m_kCache.m_bMeshEnabled = false;
//			m_kCache.m_bMaterialEnabled = false;
//		}	
//		else
//		{
//			m_pkNode->m_pkData = NULL;
//			delete this;
//		}
//	}
//
//	bool operationCompleted(BackgroundProcessTicket ticket,
//		const BackgroundProcessResult& result)
//	{
//		FV_ASSERT(!result.error);
//		Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
//			_getCurrentSceneManager();
//		FV_ASSERT(pkSceneManager);
//		FV_ASSERT(m_pkNode);
//		FV_ASSERT(m_pkAvatar);
//		FV_ASSERT(m_eState == LOAD_ENTITY);
//		FV_ASSERT(m_kCache.m_eState != Cache::CACHE_NONE);
//
//		bool bIsFinished = false;
//		if(m_kCache.m_ulMeshTicket == ticket)
//		{
//			m_kCache.m_bMeshEnabled = true;
//			if(m_kCache.m_bMaterialEnabled)
//				bIsFinished = true;
//		}
//		if(m_kCache.m_ulMaterialTicket == ticket)
//		{
//			m_kCache.m_bMaterialEnabled = true;
//			if(m_kCache.m_bMeshEnabled)
//				bIsFinished = true;
//		}
//
//		if(bIsFinished)
//		{
//			NameValuePairList kParams;
//			kParams["mesh"] = m_kCache.m_pcMesh;
//			kParams["material"] = m_kCache.m_pcMaterial;
//
//			bool bIsPart = m_kCache.m_eState == Cache::CACHE_PART;
//
//			if(bIsPart)
//			{
//				FvAvatarEntity* pkEntity = static_cast<FvAvatarEntity*>(
//					pkSceneManager->createMovableObject(
//					m_kCache.m_pcName, FvAvatarEntityFactory::FACTORY_TYPE_NAME,
//					&kParams));
//
//				pkEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAX);
//
//				pkEntity->setCustomParameter(0, Ogre::Vector4(0.5f, 0.5f, 0.5f, 0.5f));
//
//				m_kPartEntities[m_kCache.m_u32Entity].
//					m_apkEntities[m_kCache.m_u32Binding] = pkEntity;
//			}
//			else
//			{
//				Ogre::Entity* pkEntity = pkSceneManager->createEntity(
//					m_kCache.m_pcName, m_kCache.m_pcMesh);
//				MaterialPtr spMaterial = MaterialManager::getSingleton().getByName(
//					m_kCache.m_pcMaterial);
//				FV_ASSERT(!spMaterial.isNull());
//				pkEntity->setMaterial(spMaterial);
//				pkEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAX);
//
//				for(FvUInt32 i(0); i < pkEntity->getNumSubEntities(); ++i)
//				{
//					pkEntity->getSubEntity(i)->setCustomParameter(0, Ogre::Vector4(0.5f, 0.5f, 0.5f, 0.5f));
//				}
//
//				m_kLinkEntities[m_kCache.m_u32Entity].m_apkEntities[m_kCache.m_u32Binding] =
//					pkEntity;
//			}			
//
//			m_pkNode->LoadFinishBinding(m_kCache.m_u32Entity,
//				m_kCache.m_u32Binding, bIsPart);
//
//			bool bFinish = false;
//
//			++m_kCache.m_u32Binding;
//			if(m_kCache.m_u32Binding >= m_pkAvatar->GetBindingNumber())
//			{
//				m_kCache.m_u32Binding = 0;
//
//				m_pkNode->LoadFinishEntity(m_kCache.m_u32Entity, bIsPart);
//
//				++m_kCache.m_u32Entity;
//
//				if(bIsPart)
//				{
//					if(m_kCache.m_u32Entity >=
//						m_pkNode->m_kPartEntityDatas.size())
//					{
//						m_kCache.m_u32Entity = 0;
//
//						if(m_pkNode->m_kLinkEntityDatas.size())
//						{
//							m_kCache.m_eState = Cache::CACHE_LINK;
//						}
//						else
//						{
//							bFinish = true;
//						}
//						
//					}
//				}
//				else
//				{
//					if(m_kCache.m_u32Entity >=
//						m_pkNode->m_kLinkEntityDatas.size())
//					{
//						m_kCache.m_u32Entity = 0;
//
//						bFinish = true;
//					}
//				}
//
//				if(bFinish)
//				{
//					if(m_pkNode->LoadFinishAll())
//					{
//						PutOn();
//					}
//					else
//					{
//						m_pkAvatar->m_kAvatarNodeCacheList.AttachBack(*m_pkNode);
//					}
//
//					return true;
//				}
//				else
//				{
//					Load();
//				}				
//			}
//			
//		}
//
//		return false;
//	}
//
//	void PutOn()
//	{
//		m_eState = LOAD_NONE;
//		m_pkAvatar->Link(this);
//		m_pkAvatar->m_kAvatarNodeList.AttachBack(*m_pkNode);
//	}
//
//	void SetGroup(FvUInt32 u32Group)
//	{
//		for(FvUInt32 i(0); i < m_kPartEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				m_kPartEntities[i].m_apkEntities[j]->setRenderQueueGroup(u32Group);
//			}
//		}
//
//		for(FvUInt32 i(0); i < m_kLinkEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				m_kLinkEntities[i].m_apkEntities[j]->setRenderQueueGroup(u32Group);
//			}
//		}
//	}
//
//	void UpdateRenderQueue(Ogre::RenderQueue* pkQueue, const Ogre::Vector4& kID)
//	{
//		for(FvUInt32 i(0); i < m_kPartEntities.size(); ++i)
//		{
//			m_kPartEntities[i].m_apkEntities[m_pkAvatar->GetActiveBinding()]->setCustomParameter(4, kID);
//			m_kPartEntities[i].m_apkEntities[m_pkAvatar->GetActiveBinding()]->_updateRenderQueue(pkQueue);
//		}
//
//		for(FvUInt32 i(0); i < m_kLinkEntities.size(); ++i)
//		{
//			for(FvUInt32 k(0);
//				k < m_kLinkEntities[i].m_apkEntities[m_pkAvatar->GetActiveBinding()]->getNumSubEntities();
//				++k)
//			{
//				m_kLinkEntities[i].m_apkEntities[m_pkAvatar->GetActiveBinding()]->getSubEntity(k)->setCustomParameter(4, kID);
//			}
//
//			m_kLinkEntities[i].m_apkEntities[m_pkAvatar->GetActiveBinding()]->_updateRenderQueue(pkQueue);
//		}
//	}
//
//	void SetCustomParams(FvUInt32 u32ID, const Ogre::Vector4& kParams)
//	{
//		for(FvUInt32 i(0); i < m_kPartEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				m_kPartEntities[i].m_apkEntities[j]->setCustomParameter(u32ID, kParams);
//			}
//		}
//
//		for(FvUInt32 i(0); i < m_kLinkEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				for(FvUInt32 k(0);
//					k < m_kLinkEntities[i].m_apkEntities[j]->getNumSubEntities();
//					++k)
//				{
//					m_kLinkEntities[i].m_apkEntities[j]->getSubEntity(k)->setCustomParameter(u32ID, kParams);
//				}
//			}
//		}
//	}
//
//	void SetCustomParams(FvUInt32 u32ID, FvUInt32 u32Pos, float fParam)
//	{
//		for(FvUInt32 i(0); i < m_kPartEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				Vector4 kParams = m_kPartEntities[i].m_apkEntities[j]->getCustomParameter(u32ID);
//				kParams[u32Pos] = fParam;
//				m_kPartEntities[i].m_apkEntities[j]->setCustomParameter(u32ID, kParams);
//			}
//		}
//
//		for(FvUInt32 i(0); i < m_kLinkEntities.size(); ++i)
//		{
//			for(FvUInt32 j(0); j < m_pkAvatar->GetBindingNumber(); ++j)
//			{
//				for(FvUInt32 k(0);
//					k < m_kLinkEntities[i].m_apkEntities[j]->getNumSubEntities();
//					++k)
//				{
//					Vector4 kParams = m_kPartEntities[i].m_apkEntities[j]->getCustomParameter(u32ID);
//					kParams[u32Pos] = fParam;
//					m_kLinkEntities[i].m_apkEntities[j]->getSubEntity(k)->setCustomParameter(u32ID, kParams);
//				}
//			}
//		}
//	}
//
//	FvAvatar* m_pkAvatar;
//	AvatarNode* m_pkNode;
//	std::vector<PartEntity> m_kPartEntities;
//	std::vector<LinkEntity> m_kLinkEntities;
//	std::vector<LinkEffect> m_kLinkEffects;
//	Cache m_kCache;
//	LoadState m_eState;
//	LoadListener* m_pkLoadListener;
//	FvRefList<MaterialListener*> m_kMaterialListeners;
//	FvAvatar::AvatarNode::RenderState m_eRenderState;
//};
////----------------------------------------------------------------------------
//void FvAvatar::PutOnCached()
//{
//	m_kAvatarNodeCacheList.BeginIterator();
//	while (!m_kAvatarNodeCacheList.IsEnd())
//	{
//		FvRefList<AvatarNode*>::iterator iter =
//			m_kAvatarNodeCacheList.GetIterator();
//
//		m_kAvatarNodeCacheList.Next();
//
//		iter->m_Content->m_pkData->PutOn();
//	}
//
//	FV_ASSERT(m_kAvatarNodeCacheList.Size() == 0);
//}
////----------------------------------------------------------------------------
//void FvAvatar::PutOn(AvatarNode* pkNode)
//{
//	FV_ASSERT(pkNode);
//	FV_ASSERT(!pkNode->m_pkData);
//	
//	pkNode->m_pkData = new AvatarNodePrivateData(this, pkNode);
//	pkNode->m_pkData->Load();
//}
////----------------------------------------------------------------------------
//void FvAvatar::PutOff(AvatarNode* pkNode)
//{
//	FV_ASSERT(pkNode);
//	if(pkNode->m_pkData)
//	{
//		FV_ASSERT(pkNode->IsOnAvatar(this));
//
//		pkNode->Detach();
//
//		if(pkNode->m_pkData->m_eState != AvatarNodePrivateData::LOAD_ENTITY)
//		{
//			Cut(pkNode->m_pkData);
//		}
//
//		FV_SAFE_DELETE(pkNode->m_pkData);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::Link(AvatarNodePrivateData* pkData)
//{
//	FV_ASSERT(pkData);
//	FV_ASSERT(pkData->m_pkAvatar == this);
//
//	std::set<AvatarNodePrivateData::PartEntity*> kEntityNeedUpdate;
//
//	for(FvUInt32 i(0); i < pkData->m_kPartEntities.size(); ++i)
//	{
//		AvatarNodePrivateData::PartEntity& kEntity = pkData->m_kPartEntities[i];
//
//		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//		{
//			m_ppkAvatarSkeletons[i]->AddEntity(
//				kEntity.m_apkEntities[i]);
//		}
//		
//		for(FvUInt32 j(0); j < kEntity.m_kLocalParts.size(); ++j)
//		{
//			const FvUInt32 u32GlobalPart = kEntity.m_kLocalParts[j].m_u32GlobalPart;
//
//			PVList::iterator iter =
//				m_kAvatarGlobalParts[u32GlobalPart].GetHead();
//
//			if(m_kAvatarGlobalParts[u32GlobalPart].Size())
//			{
//				bool bAttachFlag = false;
//
//				while(!m_kAvatarGlobalParts[u32GlobalPart].IsEnd(iter))
//				{
//					AvatarNodePrivateData::PartEntity* pkDst =
//						(AvatarNodePrivateData::PartEntity*)((*iter).m_Content);
//					if(kEntity.m_u32Priority <= pkDst->m_u32Priority)
//					{
//						if(iter == m_kAvatarGlobalParts[u32GlobalPart].GetHead())
//						{
//							kEntity.Add(FV_MASK(j));
//							pkDst->Del(FV_MASK(((AvatarNodePrivateData::PartEntity::Node*)
//								iter)->m_u32LocalPart));
//
//							kEntityNeedUpdate.insert(&kEntity);
//							kEntityNeedUpdate.insert(pkDst);
//
//						}
//
//						m_kAvatarGlobalParts[u32GlobalPart].AttachBefore(
//							*iter, kEntity.m_kLocalParts[j]);
//						bAttachFlag = true;
//						break;
//					}
//					else
//					{
//						PVList::Next(iter);
//					}
//				}
//
//				if(!bAttachFlag)
//				{
//					m_kAvatarGlobalParts[u32GlobalPart].AttachBack(
//						kEntity.m_kLocalParts[j]);
//				}
//			}
//			else
//			{
//				m_kAvatarGlobalParts[u32GlobalPart].AttachBack(
//					kEntity.m_kLocalParts[j]);
//				kEntity.Add(FV_MASK(j));
//				kEntityNeedUpdate.insert(&kEntity);
//			}
//			
//		}
//	}
//
//	for(std::set<AvatarNodePrivateData::PartEntity*>::iterator it =
//		kEntityNeedUpdate.begin(); it != kEntityNeedUpdate.end(); ++it)
//	{
//		(*it)->Update(m_u32BindingNumber);
//	}
//
//	for(FvUInt32 i(0); i < pkData->m_kLinkEntities.size(); ++i)
//	{
//		for(FvUInt32 j(0); j < GetBindingNumber(); ++j)
//		{
//			FV_ASSERT(pkData->m_kLinkEntities[i].m_apkEntities[j]);
//
//			if(pkData->m_kLinkEntities[i].m_kTagName == "")
//			{
//				m_ppkAvatarSkeletons[j]->GetSceneNode()->attachObject(
//					pkData->m_kLinkEntities[i].m_apkEntities[j]);
//			}
//			else
//			{
//				m_ppkAvatarSkeletons[j]->m_pkMainEntity->attachObjectToBone(
//					pkData->m_kLinkEntities[i].m_kTagName,
//					pkData->m_kLinkEntities[i].m_apkEntities[j]);
//			}
//		}
//	}
//
//	for(FvUInt32 i(0); i < pkData->m_kLinkEffects.size(); ++i)
//	{
//		for(FvUInt32 j(0); j < GetBindingNumber(); ++j)
//		{
//			Ogre::SceneNode *pkNode = pkData->m_kLinkEffects[i].m_apkNodes[j];
//			if(!pkNode) continue;		
//
//			pkData->m_kLinkEffects[i].m_apkEffects[j]->start();
//
//			if(!pkData->m_kLinkEffects[i].m_kTagName.empty())
//			{
//				FvAvatarSkeleton::LinkMap::iterator kFind = 
//					m_ppkAvatarSkeletons[j]->m_kLinkNodes.find(pkNode);
//				const FvAvatar::ProviderPtr &spTagPoint = this->ReferenceProvider(pkData->m_kLinkEffects[i].m_kTagName.c_str());
//				if(spTagPoint && 
//					(kFind == m_ppkAvatarSkeletons[j]->m_kLinkNodes.end()))
//				{
//					FvAvatarLinkNode *pkLinkNode = new FvAvatarLinkNode;
//					pkLinkNode->m_spTag = spTagPoint;
//					pkLinkNode->m_pkEffectData = &pkData->m_pkNode->m_kLinkEffectDatas[i];
//					m_ppkAvatarSkeletons[j]->m_kLinkNodes.insert(FvAvatarSkeleton::LinkMap::value_type(pkNode,pkLinkNode));
//				}
//			}
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::Cut(AvatarNodePrivateData* pkData)
//{
//	FV_ASSERT(pkData);
//	FV_ASSERT(pkData->m_pkAvatar == this);
//
//	std::set<AvatarNodePrivateData::PartEntity*> kEntityNeedUpdate;
//
//	for(FvUInt32 i(0); i < pkData->m_kPartEntities.size(); ++i)
//	{
//		AvatarNodePrivateData::PartEntity& kEntity = pkData->m_kPartEntities[i];
//
//		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//		{
//			m_ppkAvatarSkeletons[i]->RemoveEntity(
//				kEntity.m_apkEntities[i]);
//
//		}
//
//		for(FvUInt32 j(0); j < kEntity.m_kLocalParts.size(); ++j)
//		{
//			const FvUInt32 u32GlobalPart = kEntity.m_kLocalParts[j].m_u32GlobalPart;
//
//			if(&kEntity.m_kLocalParts[j] == m_kAvatarGlobalParts[u32GlobalPart].GetHead())
//			{
//				AvatarNodePrivateData::PartEntity* pkDst = (AvatarNodePrivateData::PartEntity*)
//					kEntity.m_kLocalParts[j].GetNextContent((void*)(NULL));
//				if(pkDst)
//				{
//					pkDst->Add(FV_MASK(((AvatarNodePrivateData::PartEntity::Node*)
//						kEntity.m_kLocalParts[j].Next())->m_u32LocalPart));
//
//					kEntityNeedUpdate.insert(pkDst);
//				}
//				
//			}
//
//			kEntity.m_kLocalParts[j].Detach();
//		}
//	}
//
//	for(std::set<AvatarNodePrivateData::PartEntity*>::iterator it =
//		kEntityNeedUpdate.begin(); it != kEntityNeedUpdate.end(); ++it)
//	{
//		(*it)->Update(m_u32BindingNumber);
//	}
//
//	for(FvUInt32 i(0); i < pkData->m_kLinkEntities.size(); ++i)
//	{
//		for(FvUInt32 j(0); j < GetBindingNumber(); ++j)
//		{
//			FV_ASSERT(pkData->m_kLinkEntities[i].m_apkEntities[j]);
//
//			if(pkData->m_kLinkEntities[i].m_kTagName == "")
//			{
//				m_ppkAvatarSkeletons[j]->GetSceneNode()->detachObject(
//					pkData->m_kLinkEntities[i].m_apkEntities[j]);
//			}
//			else
//			{
//				m_ppkAvatarSkeletons[j]->m_pkMainEntity->detachObjectFromBone(
//					pkData->m_kLinkEntities[i].m_apkEntities[j]);
//			}
//		}
//	}
//
//	for(FvUInt32 i(0); i < pkData->m_kLinkEffects.size(); ++i)
//	{
//		for(FvUInt32 j(0); j < GetBindingNumber(); ++j)
//		{
//			Ogre::SceneNode *pkNode = pkData->m_kLinkEffects[i].m_apkNodes[j];
//			FvAvatarSkeleton::LinkMap::iterator kFind = 
//				m_ppkAvatarSkeletons[j]->m_kLinkNodes.find(pkNode);
//			if(kFind != m_ppkAvatarSkeletons[j]->m_kLinkNodes.end())
//			{
//				delete kFind->second;
//				m_ppkAvatarSkeletons[j]->m_kLinkNodes.erase(kFind);
//			}
//		}
//	}
//}
////----------------------------------------------------------------------------
//const FvAvatar::ProviderPtr& FvAvatar::ReferenceProvider(const char* pcName)
//{
//	if(pcName)
//	{
//		FvString kBoneName;
//		if(!m_spAvatarResource->HasAliasName(pcName))
//			kBoneName = pcName;
//		else
//			kBoneName = m_spAvatarResource->GetAliasName(pcName);
//		BindingProviderMap::iterator it = m_kProviderMap.find(kBoneName.c_str());
//
//		if(it == m_kProviderMap.end())
//		{
//			if(m_ppkAvatarSkeletons[0]->m_pkMainEntity->getSkeleton()->hasBone(kBoneName))
//			{
//				BindingProvider* pkBindingProvider = new BindingProvider();
//
//				it = m_kProviderMap.insert(BindingProviderMap::value_type(
//					kBoneName.c_str(), pkBindingProvider));
//
//				pkBindingProvider->m_spProvider = new Provider();
//				pkBindingProvider->m_spProvider->Attach0(
//					pkBindingProvider->m_kPosition);
//				pkBindingProvider->m_spProvider->Attach1(
//					pkBindingProvider->m_kRotation);
//
//				for(FvUInt32 i(0); i < GetBindingNumber(); ++i)
//				{
//					SkeletonInstance* pkSkeleton = m_ppkAvatarSkeletons[i]->
//						m_pkMainEntity->getSkeleton();
//
//					pkBindingProvider->m_apkBones[i] = pkSkeleton->getBone(kBoneName);
//				}
//
//				pkBindingProvider->UpdateBinding(m_u32ActiveBinding, GetSceneNode(), false);
//
//				return it->second->m_spProvider;
//			}			
//		}
//		else
//		{
//			it->second->UpdateBinding(m_u32ActiveBinding, GetSceneNode(), false);
//			return it->second->m_spProvider;
//		}		
//	}
//
//	if(!m_spRootProvider)
//	{
//		m_spRootProvider = new Provider();
//		m_spRootProvider->Attach0(GetSceneNode()->_getDerivedPosition());
//		m_spRootProvider->Attach1(GetSceneNode()->_getDerivedOrientation());
//	}
//
//	return m_spRootProvider;
//}
////----------------------------------------------------------------------------
//void FvAvatar::ReleaseProvider(const char* pcName)
//{
//	if(pcName)
//	{
//		BindingProviderMap::iterator it = m_kProviderMap.find(pcName);
//		if(it != m_kProviderMap.end())
//		{
//			FV_ASSERT(it->second);
//			delete (it->second);
//			m_kProviderMap.erase(it);
//		}
//	}
//	else
//	{
//		m_spRootProvider->Detach0();
//		m_spRootProvider->Detach1();
//		m_spRootProvider = NULL;
//	}
//}
////----------------------------------------------------------------------------
//FvAvatar::FastProviderPtr FvAvatar::GetFastProvider(const char* pcName, bool bLocal)
//{
//	if(pcName)
//	{
//		FvString kBoneName;
//		if(!m_spAvatarResource->HasAliasName(pcName))
//			kBoneName = pcName;
//		else
//			kBoneName = m_spAvatarResource->GetAliasName(pcName);
//
//		if(m_ppkAvatarSkeletons[0]->m_pkMainEntity->getSkeleton()->hasBone(kBoneName))
//		{
//			FastProviderPtr spProvider = new FastProvider(this);
//			spProvider->m_bLocal = bLocal;
//			for(FvUInt32 i(0); i < GetBindingNumber(); ++i)
//			{
//				SkeletonInstance* pkSkeleton = m_ppkAvatarSkeletons[i]->
//					m_pkMainEntity->getSkeleton();
//
//				spProvider->m_apkBones[i] = pkSkeleton->getBone(kBoneName);
//			}
//			m_kFastProviders.push_back(spProvider);
//			return spProvider;
//		}			
//	}
//
//	return NULL;
//}
////----------------------------------------------------------------------------
//const char* FvAvatar::GetName()
//{
//	return m_kName.c_str();
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetRenderState(AvatarNode::RenderState eState)
//{
//	FvRefList<AvatarNode*>::iterator iter = m_kAvatarNodeList.GetHead();
//	while(!m_kAvatarNodeList.IsEnd(iter))
//	{
//		if(iter->m_Content->GetUpdateStateWithParent())
//		{
//			iter->m_Content->SetRenderState(eState);
//		}
//		
//
//		FvRefList<AvatarNode*>::Next(iter);
//	}
//
//	FvUInt32 u32Group;
//
//	switch(eState)
//	{
//	case AvatarNode::COLOR_EFFECT:
//		u32Group = FvRenderManager::COLOR_EFFECT_GROUP_ID;
//		break;
//	case AvatarNode::FADE_OUT:
//		u32Group = FvRenderManager::FADE_OUT_GROUP_ID;
//		break;
//	case AvatarNode::BLEND:
//		u32Group = FvRenderManager::BLEND_GROUP_ID;
//		break;
//	case AvatarNode::MESH_UI:
//		u32Group = 90;
//		break;
//	case AvatarNode::EDGE_HIGHLIGHT:
//		u32Group = FvRenderManager::EDGE_HIGH_LIGHT;
//		break;
//	case AvatarNode::EDGE_HIGHLIGHT_COLOR_EFFECT:
//		u32Group = FvRenderManager::EDGE_HIGH_LIGHT_COLOR_EFFECT;
//		break;
//	case AvatarNode::EDGE_HIGHLIGHT_BLEND:
//		u32Group = FvRenderManager::EDGE_HIGH_LIGHT_BLEND;
//		break;
//	default:
//		u32Group = Ogre::RENDER_QUEUE_MAX;
//		break;
//	}
//
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		m_ppkAvatarSkeletons[i]->m_pkMainEntity->setRenderQueueGroup(u32Group);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetAddColor(float r, float g, float b)
//{
//	FvRefList<AvatarNode*>::iterator iter = m_kAvatarNodeList.GetHead();
//	while(!m_kAvatarNodeList.IsEnd(iter))
//	{
//		if(iter->m_Content->GetUpdateStateWithParent())
//		{
//			iter->m_Content->SetAddColor(r, g, b);
//		}
//
//		FvRefList<AvatarNode*>::Next(iter);
//	}
//
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		for(FvUInt32 j(0); j < m_ppkAvatarSkeletons[i]->m_pkMainEntity->getNumSubEntities(); ++j)
//		{
//			m_ppkAvatarSkeletons[i]->m_pkMainEntity->getSubEntity(j)->setCustomParameter(2, Ogre::Vector4(r, g, b, 1));
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetMulColor(float r, float g, float b)
//{
//	FvRefList<AvatarNode*>::iterator iter = m_kAvatarNodeList.GetHead();
//	while(!m_kAvatarNodeList.IsEnd(iter))
//	{
//		if(iter->m_Content->GetUpdateStateWithParent())
//		{
//			iter->m_Content->SetMulColor(r, g, b);
//		}
//		
//
//		FvRefList<AvatarNode*>::Next(iter);
//	}
//
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		for(FvUInt32 j(0); j < m_ppkAvatarSkeletons[i]->m_pkMainEntity->getNumSubEntities(); ++j)
//		{
//			m_ppkAvatarSkeletons[i]->m_pkMainEntity->getSubEntity(j)->setCustomParameter(1, Ogre::Vector4(r - 1, g - 1, b - 1, 1));
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetBlendAlpha(float a, float x, float y, float fRefraction)
//{
//	FvRefList<AvatarNode*>::iterator iter = m_kAvatarNodeList.GetHead();
//	while(!m_kAvatarNodeList.IsEnd(iter))
//	{
//		if(iter->m_Content->GetUpdateStateWithParent())
//		{
//			iter->m_Content->SetBlendAlpha(a, x, y, fRefraction);
//		}
//		
//
//		FvRefList<AvatarNode*>::Next(iter);
//	}
//
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		for(FvUInt32 j(0); j < m_ppkAvatarSkeletons[i]->m_pkMainEntity->getNumSubEntities(); ++j)
//		{
//			m_ppkAvatarSkeletons[i]->m_pkMainEntity->getSubEntity(j)->setCustomParameter(7, Ogre::Vector4(fRefraction, x, -y, a));
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetFadeOutAlpha(float a)
//{
//	FvRefList<AvatarNode*>::iterator iter = m_kAvatarNodeList.GetHead();
//	while(!m_kAvatarNodeList.IsEnd(iter))
//	{
//		if(iter->m_Content->GetUpdateStateWithParent())
//		{
//			iter->m_Content->SetFadeOutAlpha(a);
//		}
//		
//
//		FvRefList<AvatarNode*>::Next(iter);
//	}
//
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		for(FvUInt32 j(0); j < m_ppkAvatarSkeletons[i]->m_pkMainEntity->getNumSubEntities(); ++j)
//		{
//			m_ppkAvatarSkeletons[i]->m_pkMainEntity->getSubEntity(j)->setCustomParameter(3, Ogre::Vector4(a, a, a, a));
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetEdgeHighLight(float r, float g, float b, float a)
//{
//	FvRefList<AvatarNode*>::iterator iter = m_kAvatarNodeList.GetHead();
//	while(!m_kAvatarNodeList.IsEnd(iter))
//	{
//		if(iter->m_Content->GetUpdateStateWithParent())
//		{
//			iter->m_Content->SetEdgeHighLight(r, g, b, a);
//		}
//
//		FvRefList<AvatarNode*>::Next(iter);
//	}
//
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		for(FvUInt32 j(0); j < m_ppkAvatarSkeletons[i]->m_pkMainEntity->getNumSubEntities(); ++j)
//		{
//			m_ppkAvatarSkeletons[i]->m_pkMainEntity->getSubEntity(j)->setCustomParameter(6, Ogre::Vector4(r, g, b, a));
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetEdgeHighLightWidth(float fWidth)
//{
//	FvRefList<AvatarNode*>::iterator iter = m_kAvatarNodeList.GetHead();
//	while(!m_kAvatarNodeList.IsEnd(iter))
//	{
//		if(iter->m_Content->GetUpdateStateWithParent())
//		{
//			iter->m_Content->SetEdgeHighLightWidth(fWidth);
//		}
//		
//
//		FvRefList<AvatarNode*>::Next(iter);
//	}
//
//	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
//	{
//		for(FvUInt32 j(0); j < m_ppkAvatarSkeletons[i]->m_pkMainEntity->getNumSubEntities(); ++j)
//		{
//			m_ppkAvatarSkeletons[i]->m_pkMainEntity->getSubEntity(j)->setCustomParameter(8, Ogre::Vector4(fWidth, fWidth, fWidth, fWidth));
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::FindVisibleLinkObjects(Ogre::Camera *pkCamera, Ogre::RenderQueue *pkQueue, 
//						Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
//						bool bIncludeChildren, bool bDisplayNodes, bool bOnlyShadowCasters)
//{
//	FV_ASSERT(m_ppkAvatarSkeletons[m_u32ActiveBinding]);
//	FvAvatarSkeleton::LinkMap::iterator kIt = m_ppkAvatarSkeletons[m_u32ActiveBinding]->m_kLinkNodes.begin();
//	for(; kIt != m_ppkAvatarSkeletons[m_u32ActiveBinding]->m_kLinkNodes.end(); ++kIt)
//	{
//		Ogre::SceneNode *pkNode = kIt->first;
//		FV_ASSERT(pkNode);
//		pkNode->_update(true,false);
//		pkNode->_findVisibleObjects(pkCamera,
//			pkQueue,
//			pkVisibleBounds,
//			true,false,bOnlyShadowCasters);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::VisitPick(Ogre::RenderQueue *pkQueue,
//	const Ogre::Vector4& kColor)
//{
//	FvRefList<AvatarNode*>::iterator iter = m_kAvatarNodeList.GetHead();
//	while(!m_kAvatarNodeList.IsEnd(iter))
//	{
//		if(iter->m_Content->GetUpdateStateWithParent())
//		{
//			iter->m_Content->UpdateRenderQueue(pkQueue, kColor);
//		}
//		
//
//		FvRefList<AvatarNode*>::Next(iter);
//	}
//
//	for(FvUInt32 j(0); j < m_ppkAvatarSkeletons[m_u32ActiveBinding]->m_pkMainEntity->getNumSubEntities(); ++j)
//	{
//		m_ppkAvatarSkeletons[m_u32ActiveBinding]->m_pkMainEntity->getSubEntity(j)->setCustomParameter(4, kColor);
//	}
//
//	m_ppkAvatarSkeletons[m_u32ActiveBinding]->m_pkMainEntity->_updateRenderQueue(pkQueue);
//}
////----------------------------------------------------------------------------
//const FvAvatarResourcePtr& FvAvatar::GetAvatarResource()
//{
//	return m_spAvatarResource;
//}
////----------------------------------------------------------------------------
//void FvAvatar::GetBoundingBox(FvBoundingBox &kBB)
//{
//	if(m_pkBoundingBoxParams)
//	{
//		kBB = m_pkBoundingBoxParams->m_kBoundingBox;
//	}
//	else
//	{
//		const Ogre::AxisAlignedBox &kBound = m_pkAvatarRootNode->_getWorldAABB();
//		kBB.SetBounds(*(FvVector3*)&kBound.getMinimum(),
//			*(FvVector3*)&kBound.getMaximum());
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::GetBoundingBox(FvBoundingBox &kBB, const FvVector3& kScale)
//{
//	if(m_pkBoundingBoxParams)
//	{
//		FvVector3 kMin, kMax;
//		kMin.ParallelMultiply(m_pkBoundingBoxParams->m_kBoundingBox.MinBounds(), kScale);
//		kMax.ParallelMultiply(m_pkBoundingBoxParams->m_kBoundingBox.MaxBounds(), kScale);
//
//		kBB.SetBounds(kMin, kMax);
//	}
//	else
//	{
//		const Ogre::AxisAlignedBox &kBound = m_pkAvatarRootNode->_getWorldAABB();
//		kBB.SetBounds(*(FvVector3*)&kBound.getMinimum(),
//			*(FvVector3*)&kBound.getMaximum());
//	}
//}
////----------------------------------------------------------------------------
//bool FvAvatar::HasBoundingBox()
//{
//	return m_pkBoundingBoxParams ? true : false;
//}
////----------------------------------------------------------------------------
//void FvAvatar::SetScale(const FvVector3& kScale)
//{
//	m_kScale = kScale;
//}
////----------------------------------------------------------------------------
//FvAvatar::AvatarNode::AvatarNode() : m_pkData(NULL),
//	m_bUpdateStateWithParent(true)
//{
//	m_Content = this;
//}
////----------------------------------------------------------------------------
//FvAvatar::AvatarNode::~AvatarNode()
//{
//	if(m_pkData)
//	{
//		FV_ASSERT(m_pkData->m_pkAvatar);
//		m_pkData->m_pkAvatar->PutOff(this);
//	}
//	FV_SAFE_DELETE(m_pkData);
//}
////----------------------------------------------------------------------------
//bool FvAvatar::AvatarNode::IsOnAvatar(FvAvatar* pkAvatar)
//{
//	if(m_pkData)
//	{
//		return m_pkData->m_pkAvatar == pkAvatar;
//	}
//	else
//	{
//		return pkAvatar == NULL;
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::RefreshMaterials()
//{
//	if(m_pkData)
//	{
//		FV_ASSERT(m_pkData->m_pkAvatar);
//		FV_ASSERT(m_pkData->m_eState != AvatarNodePrivateData::LOAD_ENTITY);
//		m_pkData->RefreshMaterials();
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::RefreshLink()
//{
//	if(m_pkData)
//	{
//		FV_ASSERT(m_pkData->m_pkAvatar);
//
//		if(m_pkData->m_eState != AvatarNodePrivateData::LOAD_ENTITY)
//			m_pkData->RefreshLink();
//	}
//}
////----------------------------------------------------------------------------
//FvAvatar* FvAvatar::AvatarNode::GetParent()
//{
//	if(m_pkData)
//	{
//		return m_pkData->m_pkAvatar;
//	}
//	else
//	{
//		return NULL;
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::PutOn(FvAvatar* pkParent)
//{
//	FV_ASSERT(pkParent);
//
//	pkParent->PutOn(this);
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::PutOff()
//{
//	//FV_ASSERT(m_pkData);
//	if(m_pkData)
//	{
//		m_pkData->m_pkAvatar->PutOff(this);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::SetAddColor(float r, float g, float b)
//{
//	FV_ASSERT(m_pkData);
//	m_pkData->SetCustomParams(2, Ogre::Vector4(r, g, b, 1));
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::SetMulColor(float r, float g, float b)
//{
//	FV_ASSERT(m_pkData);
//	m_pkData->SetCustomParams(1, Ogre::Vector4(r - 1, g - 1, b - 1, 0));
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::SetBlendAlpha(float a, float x, float y, float fRefraction)
//{
//	FV_ASSERT(m_pkData);
//	m_pkData->SetCustomParams(7, Ogre::Vector4(fRefraction, x, -y, a));
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::SetFadeOutAlpha(float a)
//{
//	FV_ASSERT(m_pkData);
//	m_pkData->SetCustomParams(3, Ogre::Vector4(a, a, a, a));
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::SetRenderState(RenderState eState)
//{
//	FV_ASSERT(m_pkData);
//
//	if(m_pkData->m_eRenderState != eState)
//	{
//		FvUInt32 u32Group;
//
//		switch(eState)
//		{
//		case COLOR_EFFECT:
//			u32Group = FvRenderManager::COLOR_EFFECT_GROUP_ID;
//			break;
//		case FADE_OUT:
//			u32Group = FvRenderManager::FADE_OUT_GROUP_ID;
//			break;
//		case BLEND:
//			u32Group = FvRenderManager::BLEND_GROUP_ID;
//			break;
//		case MESH_UI:
//			u32Group = 90;
//			break;
//		case EDGE_HIGHLIGHT:
//			u32Group = FvRenderManager::EDGE_HIGH_LIGHT;
//			break;
//		case EDGE_HIGHLIGHT_COLOR_EFFECT:
//			u32Group = FvRenderManager::EDGE_HIGH_LIGHT_COLOR_EFFECT;
//			break;
//		case EDGE_HIGHLIGHT_BLEND:
//			u32Group = FvRenderManager::EDGE_HIGH_LIGHT_BLEND;
//			break;
//		default:
//			u32Group = Ogre::RENDER_QUEUE_MAX;
//			break;
//		}
//		m_pkData->SetGroup(u32Group);
//		m_pkData->m_eRenderState = eState;
//	}
//}
////----------------------------------------------------------------------------
//FvAvatar::AvatarNode::RenderState FvAvatar::AvatarNode::GetRenderState()
//{
//	FV_ASSERT(m_pkData);
//	return m_pkData->m_eRenderState;
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::UpdateRenderQueue(Ogre::RenderQueue* pkQueue,
//	const Ogre::Vector4& kID)
//{
//	FV_ASSERT(m_pkData);
//	m_pkData->UpdateRenderQueue(pkQueue, kID);
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::SetEdgeHighLight(float r, float g, float b, float a)
//{
//	FV_ASSERT(m_pkData);
//	m_pkData->SetCustomParams(6, Ogre::Vector4(r, g, b, a));
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::SetEdgeHighLightWidth(float fWidth)
//{
//	FV_ASSERT(m_pkData);
//	m_pkData->SetCustomParams(8, Ogre::Vector4(fWidth, fWidth, fWidth, fWidth));
//}
////----------------------------------------------------------------------------
//void FvAvatar::AvatarNode::SetUpdateStateWithParent(bool bEnable)
//{
//	m_bUpdateStateWithParent = bEnable;
//}
////----------------------------------------------------------------------------
//bool FvAvatar::AvatarNode::GetUpdateStateWithParent()
//{
//	return m_bUpdateStateWithParent;
//}
////----------------------------------------------------------------------------
//FvAvatar::FastProvider::FastProvider(FvAvatar* pkAvatar) :
//m_pkAvatar(pkAvatar)
//{
//	
//}
////----------------------------------------------------------------------------
//FvVector3 FvAvatar::FastProvider::GetVector3()
//{
//	if(m_pkAvatar == NULL)
//		return FvVector3::ZERO;
//
//	Bone* pkBone = m_apkBones[m_pkAvatar->GetActiveBinding()];
//	FV_ASSERT(pkBone);
//
//	if(m_bLocal)
//	{
//		return *(FvVector3*)&pkBone->_getDerivedPosition();
//	}
//	else
//	{
//		const Quaternion& kParentOrientation = m_pkAvatar->GetSceneNode()->_getDerivedOrientation();
//		const Vector3& kParentScale = m_pkAvatar->GetSceneNode()->_getDerivedScale();
//		const Vector3& kParentPosition = m_pkAvatar->GetSceneNode()->_getDerivedPosition();
//
//		return *(FvVector3*)&(kParentOrientation * (
//			kParentScale * pkBone->_getDerivedPosition()) + kParentPosition);
//	}
//}
////----------------------------------------------------------------------------
//FvQuaternion FvAvatar::FastProvider::GetQuaternion()
//{
//	FvQuaternion kQuat;
//	Quaternion kRotation;
//	if(m_pkAvatar == NULL)
//		return kQuat;
//
//	Bone* pkBone = m_apkBones[m_pkAvatar->GetActiveBinding()];
//	FV_ASSERT(pkBone);
//
//	if(m_bLocal)
//	{
//		kRotation = pkBone->_getDerivedOrientation();
//	}
//	else
//	{
//		const Quaternion& kParentOrientation = m_pkAvatar->GetSceneNode()->_getDerivedOrientation();
//		const Vector3& kParentScale = m_pkAvatar->GetSceneNode()->_getDerivedScale();
//		const Vector3& kParentPosition = m_pkAvatar->GetSceneNode()->_getDerivedPosition();
//		kRotation = kParentOrientation * pkBone->_getDerivedOrientation();
//	}
//
//	kQuat.Set(kRotation.x,kRotation.y,kRotation.z,kRotation.w);
//	return kQuat;
//}
////----------------------------------------------------------------------------
//FvAvatar::BindingProvider::BindingProvider()
//{
//	FvZeroMemory(m_apkBones, sizeof(m_apkBones));
//}
////----------------------------------------------------------------------------
//FvAvatar::BindingProvider::~BindingProvider()
//{
//	if(m_spProvider)
//	{
//		m_spProvider->Detach0();
//		m_spProvider->Detach1();
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatar::BindingProvider::UpdateBinding(FvUInt32 u32Binding,
//	SceneNode* pkParent, bool bClear)
//{
//	Bone* pkBone = m_apkBones[u32Binding];
//	FV_ASSERT(pkBone);
//
//	FV_ASSERT(pkParent);
//
//	if(bClear && (m_spProvider->RefCount() == 1))
//	{
//		return;
//	}
//
//	const Quaternion& parentOrientation = pkParent->_getDerivedOrientation();
//	const Vector3& parentScale = pkParent->_getDerivedScale();
//	const Vector3& parentPosition = pkParent->_getDerivedPosition();
//
//	m_kRotation = parentOrientation * pkBone->_getDerivedOrientation();
//	m_kPosition = parentOrientation * (
//		parentScale * pkBone->_getDerivedPosition()) + parentPosition;
//}
////----------------------------------------------------------------------------
