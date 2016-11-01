#include "FvAnimatedEntity.h"
#include <OgreSubEntity.h>
#include <FvDebug.h>
#include <OgreAnimationState.h>
#include <OgreNode.h>
#include <OgreSkeletonInstance.h>
#include <OgreRoot.h>
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>
#include <OgreMaterialManager.h>
#include <OgreTagPoint.h>
#include "FvAnimatedCharacter.h"

using namespace Ogre;

#define __MM_LOAD_PS(p)                                                             \
	(*(__m128*)(p))
#define __MM_SELECT(v, fp)                                                          \
	_mm_shuffle_ps((v), (v), _MM_SHUFFLE((fp),(fp),(fp),(fp)))
#define __MM_STORE_PS(p, v)                                                         \
	(*(__m128*)(p) = (v))
#define __MM_ACCUM4_PS(a, b, c, d)                                                  \
	_mm_add_ps(_mm_add_ps(a, b), _mm_add_ps(c, d))

//----------------------------------------------------------------------------
void ConcatenateAffineMatrices(
	const Matrix4& baseMatrix,
	const Matrix4* pSrcMat,
	Matrix4* pDstMat,
	size_t numMatrices)
{
	// Load base matrix, unaligned
	__m128 m0 = _mm_loadu_ps(baseMatrix[0]);
	__m128 m1 = _mm_loadu_ps(baseMatrix[1]);
	__m128 m2 = _mm_loadu_ps(baseMatrix[2]);
	__m128 m3 = _mm_loadu_ps(baseMatrix[3]);        // m3 should be equal to (0, 0, 0, 1)

	for (size_t i = 0; i < numMatrices; ++i)
	{
		// Load source matrix, aligned
		__m128 s0 = __MM_LOAD_PS((*pSrcMat)[0]);
		__m128 s1 = __MM_LOAD_PS((*pSrcMat)[1]);
		__m128 s2 = __MM_LOAD_PS((*pSrcMat)[2]);

		++pSrcMat;

		__m128 t0, t1, t2, t3;

		// Concatenate matrix, and store results

		// Row 0
		t0 = _mm_mul_ps(__MM_SELECT(m0, 0), s0);
		t1 = _mm_mul_ps(__MM_SELECT(m0, 1), s1);
		t2 = _mm_mul_ps(__MM_SELECT(m0, 2), s2);
		t3 = _mm_mul_ps(m0, m3);    // Compiler should optimise this out of the loop
		__MM_STORE_PS((*pDstMat)[0], __MM_ACCUM4_PS(t0,t1,t2,t3));

		// Row 1
		t0 = _mm_mul_ps(__MM_SELECT(m1, 0), s0);
		t1 = _mm_mul_ps(__MM_SELECT(m1, 1), s1);
		t2 = _mm_mul_ps(__MM_SELECT(m1, 2), s2);
		t3 = _mm_mul_ps(m1, m3);    // Compiler should optimise this out of the loop
		__MM_STORE_PS((*pDstMat)[1], __MM_ACCUM4_PS(t0,t1,t2,t3));

		// Row 2
		t0 = _mm_mul_ps(__MM_SELECT(m2, 0), s0);
		t1 = _mm_mul_ps(__MM_SELECT(m2, 1), s1);
		t2 = _mm_mul_ps(__MM_SELECT(m2, 2), s2);
		t3 = _mm_mul_ps(m2, m3);    // Compiler should optimise this out of the loop
		__MM_STORE_PS((*pDstMat)[2], __MM_ACCUM4_PS(t0,t1,t2,t3));

		// Row 3
		__MM_STORE_PS((*pDstMat)[3], m3);

		++pDstMat;
	}
}
//----------------------------------------------------------------------------
void ConcatenateAffineMatrices(
	const Matrix4& baseMatrix,
	const Matrix4* pSrcMat,
	Matrix4* pDstMat,
	size_t numMatrices,
	unsigned char* pucBoneSwitch)
{
	// Load base matrix, unaligned
	__m128 m0 = _mm_loadu_ps(baseMatrix[0]);
	__m128 m1 = _mm_loadu_ps(baseMatrix[1]);
	__m128 m2 = _mm_loadu_ps(baseMatrix[2]);
	__m128 m3 = _mm_loadu_ps(baseMatrix[3]);        // m3 should be equal to (0, 0, 0, 1)

	for (size_t i = 0; i < numMatrices; ++i)
	{
		if(pucBoneSwitch[i])
		{
			// Load source matrix, aligned
			__m128 s0 = __MM_LOAD_PS((*pSrcMat)[0]);
			__m128 s1 = __MM_LOAD_PS((*pSrcMat)[1]);
			__m128 s2 = __MM_LOAD_PS((*pSrcMat)[2]);

			__m128 t0, t1, t2, t3;

			// Concatenate matrix, and store results

			// Row 0
			t0 = _mm_mul_ps(__MM_SELECT(m0, 0), s0);
			t1 = _mm_mul_ps(__MM_SELECT(m0, 1), s1);
			t2 = _mm_mul_ps(__MM_SELECT(m0, 2), s2);
			t3 = _mm_mul_ps(m0, m3);    // Compiler should optimise this out of the loop
			__MM_STORE_PS((*pDstMat)[0], __MM_ACCUM4_PS(t0,t1,t2,t3));

			// Row 1
			t0 = _mm_mul_ps(__MM_SELECT(m1, 0), s0);
			t1 = _mm_mul_ps(__MM_SELECT(m1, 1), s1);
			t2 = _mm_mul_ps(__MM_SELECT(m1, 2), s2);
			t3 = _mm_mul_ps(m1, m3);    // Compiler should optimise this out of the loop
			__MM_STORE_PS((*pDstMat)[1], __MM_ACCUM4_PS(t0,t1,t2,t3));

			// Row 2
			t0 = _mm_mul_ps(__MM_SELECT(m2, 0), s0);
			t1 = _mm_mul_ps(__MM_SELECT(m2, 1), s1);
			t2 = _mm_mul_ps(__MM_SELECT(m2, 2), s2);
			t3 = _mm_mul_ps(m2, m3);    // Compiler should optimise this out of the loop
			__MM_STORE_PS((*pDstMat)[2], __MM_ACCUM4_PS(t0,t1,t2,t3));

			// Row 3
			__MM_STORE_PS((*pDstMat)[3], m3);
		}

		++pSrcMat;
		++pDstMat;
	}
}
//----------------------------------------------------------------------------
FvAnimatedEntity::FvAnimatedEntity(const FvString& kName,
	const MeshPtr& spMesh) : Entity(kName, spMesh), m_pkParent(NULL),
	m_i32BoundingBoxTargetBone(-1), m_bShow(true),m_bMoved(true)
{
	FV_ASSERT(hasSkeleton());
	DisableBoundingBoxTargetBone();
}
//----------------------------------------------------------------------------
FvAnimatedEntity::~FvAnimatedEntity()
{
	for(FvStringMap<Ogre::TagPoint*>::iterator it = m_kTagPoints.begin();
		it != m_kTagPoints.end(); ++it)
	{
		mSkeletonInstance->freeTagPoint(it->second);
	}
	m_kTagPoints.clear();
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::_updateRenderQueue(RenderQueue* queue)
{
	if (!mInitialised)
		return;

	if (mMesh->getStateCount() != mMeshStateCount)
	{
		_initialise(true);
	}

	if(m_bShow)
	{
		SubEntityList::iterator i, iend;
		iend = mSubEntityList.end();
		for (i = mSubEntityList.begin(); i != iend; ++i)
		{
			if((*i)->isVisible())
			{
				if(mRenderQueueIDSet)
				{
					queue->addRenderable(*i, mRenderQueueID);
				}
				else
				{
					queue->addRenderable(*i);
				}
			}
		}
	}

	UpdateAnimationNew();

	if(m_pkParent)
	{
		m_pkParent->m_kCallbackList.BeginIterator();
		while (!m_pkParent->m_kCallbackList.IsEnd())
		{
			FvAnimatedCharacterBase::Callback* obj = static_cast<FvAnimatedCharacterBase::Callback*>(m_pkParent->m_kCallbackList.GetIterator());
			m_pkParent->m_kCallbackList.Next();
			FV_ASSERT(obj);
			///<使用>
			obj->CallbackBeforeRender();
			///</使用>
		}
	}	

	FvRefList<FvComponentEntity*>::iterator iter = m_kVisibleComponentList.GetHead();
	while (!m_kVisibleComponentList.IsEnd(iter))
	{
		FvComponentEntity* obj = (*iter).m_Content;
		FvRefList<FvComponentEntity*>::Next(iter);		

		obj->_updateRenderQueue(queue);
	}

	for(std::list<Ogre::MovableObject*>::iterator it = m_kAttachedObjects.begin();
		it != m_kAttachedObjects.end(); ++it)
	{
		MovableObject* pkChild = *it;
		if(pkChild->isVisible())
		{
			pkChild->_updateRenderQueue(queue);
		}
	}

	for(std::list<Ogre::MovableObject*>::iterator it = m_kLinkedObjects.begin();
		it != m_kLinkedObjects.end(); ++it)
	{
		MovableObject* pkChild = *it;
		if(pkChild->isVisible())
		{
			pkChild->_updateRenderQueue(queue);
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::_notifyCurrentCamera(Ogre::Camera* cam)
{
	MovableObject::_notifyCurrentCamera(cam);

	for(std::list<Ogre::MovableObject*>::iterator it = m_kAttachedObjects.begin();
		it != m_kAttachedObjects.end(); ++it)
	{
		MovableObject* pkChild = *it;
		if(pkChild->isVisible())
		{
			pkChild->_notifyCurrentCamera(cam);
		}
	}

	for(std::list<Ogre::MovableObject*>::iterator it = m_kLinkedObjects.begin();
		it != m_kLinkedObjects.end(); ++it)
	{
		MovableObject* pkChild = *it;
		if(pkChild->isVisible())
		{
			pkChild->_notifyCurrentCamera(cam);
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::UpdateAnimationNew()
{
	if (!mInitialised)
		return;
	
	bool animationDirty = (mFrameAnimationLastUpdated != mAnimationState->getDirtyFrameNumber());

	unsigned char* pucBonesSwitchTable = mSkeletonInstance->GetBonesSwitchTable();

	if(animationDirty || m_bMoved)
	{
		CacheBoneMatrices();

		mFrameAnimationLastUpdated = mAnimationState->getDirtyFrameNumber();

		mLastParentXform = _getParentNodeFullTransform();

		if(_isSkeletonAnimated())
		{
			if (!mBoneWorldMatrices)
			{
				mBoneWorldMatrices =
					static_cast<Matrix4*>(OGRE_MALLOC_SIMD(sizeof(Matrix4) * mNumBoneMatrices, MEMCATEGORY_ANIMATION));
			}

			if(pucBonesSwitchTable)
			{
				ConcatenateAffineMatrices(
					mLastParentXform,
					mBoneMatrices,
					mBoneWorldMatrices,
					mNumBoneMatrices,
					pucBonesSwitchTable);
			}
			else
			{
				ConcatenateAffineMatrices(
					mLastParentXform,
					mBoneMatrices,
					mBoneWorldMatrices,
					mNumBoneMatrices);
			}
		}
	}

	m_bMoved = false;
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::CacheBoneMatrices()
{
	unsigned long currentFrameNumber = Root::getSingleton().getNextFrameNumber();
	if (*mFrameBonesLastUpdated  != currentFrameNumber)
	{
		mSkeletonInstance->UpdateAnimation(*mAnimationState, mBoneMatrices);
		*mFrameBonesLastUpdated  = currentFrameNumber;
	}
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::SetShow(bool bIsShow)
{
	m_bShow = bIsShow;
}
//----------------------------------------------------------------------------
bool FvAnimatedEntity::GetShow()
{
	return m_bShow;
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::SetBonesSwitchTable(FvUInt8* pu8BonesSwitchTable)
{
	mSkeletonInstance->SetBonesSwitchTable(pu8BonesSwitchTable);
}
//----------------------------------------------------------------------------
FvUInt8* FvAnimatedEntity::GetBonesSwitchTable()
{
	return mSkeletonInstance->GetBonesSwitchTable();
}
//----------------------------------------------------------------------------
const String& FvAnimatedEntity::getMovableType( void ) const
{
	return FvAnimatedEntityFactory::FACTORY_TYPE_NAME;
}
//----------------------------------------------------------------------------
const AxisAlignedBox& FvAnimatedEntity::getBoundingBox( void ) const
{
	return mFullBoundingBox;
}
//----------------------------------------------------------------------------
const AxisAlignedBox& FvAnimatedEntity::getWorldBoundingBox(
	bool derive) const
{
	if (derive)
	{
		mWorldAABB = this->getBoundingBox();

		if(m_i32BoundingBoxTargetBone < 0)
		{
			mWorldAABB.transformAffine(_getParentNodeFullTransform());
		}
		else
		{
			const Vector3& kWorldPos = mSkeletonInstance->getBone(m_i32BoundingBoxTargetBone)->_getDerivedPosition();

			mWorldAABB.getMinimum().x += kWorldPos.x;
			mWorldAABB.getMaximum().x += kWorldPos.x;

			mWorldAABB.getMinimum().y += kWorldPos.y;
			mWorldAABB.getMaximum().y += kWorldPos.y;

			mWorldAABB.getMinimum().z += kWorldPos.z;
			mWorldAABB.getMaximum().z += kWorldPos.z;

			mWorldAABB.transformAffine(_getParentNodeFullTransform());
		}
		
	}
	
	return mWorldAABB;
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::SetBoundingBoxTargetBone(FvUInt16 u16TargetBone,
	const Ogre::Vector3& kMax, const Ogre::Vector3& kMin)
{
	m_i32BoundingBoxTargetBone = u16TargetBone;
	mFullBoundingBox.setExtents(kMin, kMax);
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::DisableBoundingBoxTargetBone()
{
	m_i32BoundingBoxTargetBone = -1;

	const AxisAlignedBox& kBound = mMesh->getBounds();
	Vector3 kMax = kBound.getMaximum();
	Vector3 kMin = kBound.getMinimum();

	float fWidth = kMax.x - kMin.x;
	float fHeight = kMax.y - kMin.y;

	if(fWidth > fHeight)
	{
		float fAdd = (fWidth - fHeight) * 0.5;

		kMax.y += fAdd;
		kMin.y -= fAdd;
	}
	else if(fWidth < fHeight)
	{
		float fAdd = (fHeight - fWidth) * 0.5;

		kMax.x += fAdd;
		kMin.x -= fAdd;
	}

	mFullBoundingBox.setExtents(kMin, kMax);
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::AddBoneRef(Ogre::Bone* pkBone)
{
	FV_ASSERT(pkBone);

	FvUInt8* pu8Table = GetBonesSwitchTable();

	if(pu8Table)
	{
		do
		{
			++(pu8Table[pkBone->getHandle()]);
			pkBone = (Ogre::Bone*)(pkBone->getParent());
		}
		while(pkBone);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::DelBoneRef(Ogre::Bone* pkBone)
{
	FV_ASSERT(pkBone);

	FvUInt8* pu8Table = GetBonesSwitchTable();

	if(pu8Table)
	{
		do
		{
			--(pu8Table[pkBone->getHandle()]);
			pkBone = (Ogre::Bone*)(pkBone->getParent());
		}
		while(pkBone);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::Attach(Ogre::MovableObject* pkObject,
	const char* pcName, FvAnimatedCharacterResource::AliasTagPoint& kAlias)
{
	FV_ASSERT(!(pkObject->isAttached()));

	FvStringMap<Ogre::TagPoint*>::iterator it = m_kTagPoints.find(pcName);
	if(it == m_kTagPoints.end())
	{
		FvAnimatedCharacterResource::AliasTagPoint::iterator iter = kAlias.find(pcName);
		if(iter == kAlias.end()) return;
		FvAnimatedCharacterResource::Offset& kOffset = iter->second;
		Bone* pkBone = mSkeletonInstance->getBone(kOffset.m_kBoneName);
		FV_ASSERT(pkBone);
		Ogre::TagPoint* pkTag = mSkeletonInstance->createTagPointOnBone(pkBone, kOffset.m_kRotation, kOffset.m_kPosition);
		pkTag->setParentEntity(this);
		it = m_kTagPoints.insert(FvStringMap<Ogre::TagPoint*>::value_type(pcName, pkTag));

		if(mParentNode) mParentNode->needUpdate();
	}

	AddBoneRef(static_cast<Ogre::Bone*>(it->second->getParent()));
	pkObject->_notifyAttached(it->second, true);
	m_kAttachedObjects.push_back(pkObject);
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::Detach(Ogre::MovableObject* pkObject)
{
	for(std::list<Ogre::MovableObject*>::iterator it = m_kAttachedObjects.begin();
		it != m_kAttachedObjects.end(); ++it)
	{
		if((*it) == pkObject)
		{
			TagPoint* tp = static_cast<TagPoint*>(pkObject->getParentNode());
			DelBoneRef(static_cast<Ogre::Bone*>(tp->getParent()));
			pkObject->_notifyAttached(NULL);
			m_kAttachedObjects.erase(it);
			break;
		}
	}
}
//----------------------------------------------------------------------------
Ogre::TagPoint* FvAnimatedEntity::RefTagPoint(const char* pcName,
	FvAnimatedCharacterResource::AliasTagPoint& kAlias)
{
	FvStringMap<Ogre::TagPoint*>::iterator it = m_kTagPoints.find(pcName);
	if(it == m_kTagPoints.end())
	{
		FvAnimatedCharacterResource::AliasTagPoint::iterator iter = kAlias.find(pcName);
		if(iter == kAlias.end()) return NULL;
		FvAnimatedCharacterResource::Offset& kOffset = iter->second;
		Bone* pkBone = mSkeletonInstance->getBone(kOffset.m_kBoneName);
		if(!pkBone)
		{
			FV_CRITICAL_MSG("Tag on wrong bone!\nEntity:%s\nBone:%s\nTag:%s",
				mName.c_str(),kOffset.m_kBoneName.c_str(),pcName);
		}
		Ogre::TagPoint* pkTag = mSkeletonInstance->createTagPointOnBone(pkBone, kOffset.m_kRotation, kOffset.m_kPosition);
		pkTag->setParentEntity(this);
		it = m_kTagPoints.insert(FvStringMap<Ogre::TagPoint*>::value_type(pcName, pkTag));

		if(mParentNode) mParentNode->needUpdate();
	}

	AddBoneRef(static_cast<Ogre::Bone*>(it->second->getParent()));
	return it->second;
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::FreeTagPoint(Ogre::TagPoint* pkTag)
{
	FV_ASSERT(pkTag);
	DelBoneRef(static_cast<Ogre::Bone*>(pkTag->getParent()));
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::LinkObject(Ogre::MovableObject* pkObject)
{
	FV_ASSERT(!(pkObject->isAttached()));

	m_kLinkedObjects.push_back(pkObject);
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::UnlinkObject(Ogre::MovableObject* pkObject)
{
	FV_ASSERT((pkObject->isAttached()));

	for(std::list<Ogre::MovableObject*>::iterator it = m_kLinkedObjects.begin();
		it != m_kLinkedObjects.end(); ++it)
	{
		if((*it) == pkObject)
		{
			m_kAttachedObjects.erase(it);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedEntity::_notifyMoved()
{
	m_bMoved = true;
}
//----------------------------------------------------------------------------
String FvAnimatedEntityFactory::FACTORY_TYPE_NAME = "AnimatedEntity";
//----------------------------------------------------------------------------
MovableObject* FvAnimatedEntityFactory::createInstanceImpl(
	const String& kName, const NameValuePairList* params)
{
	// must have mesh parameter
	MeshPtr spMesh;

	if (params != 0)
	{
		{
			NameValuePairList::const_iterator ni = params->find("mesh");
			if (ni != params->end())
			{
				// Get mesh (load if required)
				spMesh = MeshManager::getSingleton().load(
					ni->second,
					// autodetect group location
					ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
			}
		}
	}

	if (spMesh.isNull())
	{
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"'mesh' parameter required when constructing an AnimatedEntity.",
			"FvAnimatedEntityFactory::createInstance");
	}

	return OGRE_NEW FvAnimatedEntity(kName, spMesh);
}

//----------------------------------------------------------------------------
const String& FvAnimatedEntityFactory::getType() const
{
	return FACTORY_TYPE_NAME;
}
//----------------------------------------------------------------------------
void FvAnimatedEntityFactory::destroyInstance(MovableObject* obj)
{
	OGRE_DELETE obj;
}
//----------------------------------------------------------------------------
class FvSubMeshShell : public SubMesh
{
public:
	FvUInt32 LodFaceListSize()
	{
		return mLodFaceList.size();
	}

	IndexData* GetIndexData()
	{
		return indexData;
	}

	bool IsNoneExtremityPoints()
	{
		return extremityPoints.empty();
	}

};
//----------------------------------------------------------------------------
FvComponentEntity::FvComponentEntity(const String& kName,
	const MeshPtr& spMesh, const MaterialPtr& spMaterial)
	: Entity(), m_pkParent(NULL)
{
	mName = kName;
	mMesh = spMesh;
	Init();

	m_kComponentNode.m_Content = this;

	setMaterial(spMaterial);
	if(!spMesh->sharedVertexData)
	{
		FV_CRITICAL_MSG("%s\n", kName);
	}
	FV_ASSERT(spMesh->sharedVertexData);
	FV_ASSERT(spMesh->getNumSubMeshes());

	for(FvUInt32 i(0); i < spMesh->getNumSubMeshes(); ++i)
	{
		FvSubMeshShell* pkSubMesh =
			(FvSubMeshShell*)(spMesh->getSubMesh(i));
		FV_ASSERT(pkSubMesh);
		FV_ASSERT(pkSubMesh->useSharedVertices);
		FV_ASSERT(!pkSubMesh->LodFaceListSize());
		FV_ASSERT(pkSubMesh->IsNoneExtremityPoints());
	}

	m_pkIndexData = OGRE_NEW IndexData();
}
//----------------------------------------------------------------------------
void FvComponentEntity::Init()
{
	FV_ASSERT(!mInitialised);
	FV_ASSERT(mMesh->isLoaded());

	buildSubEntityList(mMesh, &mSubEntityList);	

	mInitialised = true;
	mMeshStateCount = mMesh->getStateCount();
}
//----------------------------------------------------------------------------
void FvComponentEntity::SetParent(FvAnimatedEntity* pkParent)
{
	mSkeletonInstance = pkParent->mSkeletonInstance;
	mNumBoneMatrices = pkParent->mNumBoneMatrices;
	mBoneMatrices = pkParent->mBoneMatrices;
	mAnimationState = pkParent->mAnimationState;
	mFrameBonesLastUpdated = pkParent->mFrameBonesLastUpdated;

	if (pkParent->mSharedSkeletonEntities == NULL)
	{
		pkParent->mSharedSkeletonEntities = OGRE_NEW_T(EntitySet, MEMCATEGORY_ANIMATION)();
		pkParent->mSharedSkeletonEntities->insert(pkParent);
	}
	mSharedSkeletonEntities = pkParent->mSharedSkeletonEntities;
	mSharedSkeletonEntities->insert(this);

	reevaluateVertexProcessing();

	mMesh->_compileBoneAssignments();

	m_pkParent = pkParent;
}
//----------------------------------------------------------------------------
FvComponentEntity::~FvComponentEntity()
{
	if(m_pkIndexData)
	{
		OGRE_DELETE m_pkIndexData;
		m_pkIndexData = NULL;
	}
}
//----------------------------------------------------------------------------
const MaterialPtr& FvComponentEntity::getMaterial() const
{
	return (*mSubEntityList.begin())->getMaterial();
}
//----------------------------------------------------------------------------
Technique* FvComponentEntity::getTechnique() const
{
	return (*mSubEntityList.begin())->getTechnique();
}
//----------------------------------------------------------------------------
void FvComponentEntity::getRenderOperation(RenderOperation& kRenderOperation)
{
	kRenderOperation.vertexData = getVertexDataForBinding();
	kRenderOperation.operationType = RenderOperation::OT_TRIANGLE_LIST;
	kRenderOperation.useIndexes = true;
	kRenderOperation.indexData = m_pkIndexData;
}
//----------------------------------------------------------------------------
void FvComponentEntity::getWorldTransforms(Matrix4* pkXform) const
{	
	const Mesh::IndexMap& indexMap = mMesh->sharedBlendIndexToBoneIndexMap;
	FV_ASSERT(indexMap.size() <= mNumBoneMatrices);

	if (m_pkParent->_isSkeletonAnimated())
	{
		const unsigned short* pusIndex = &indexMap[0];
		for(FvUInt32 i(0); i < indexMap.size(); ++i, ++pkXform)
		{
			*pkXform = m_pkParent->mBoneWorldMatrices[pusIndex[i]];
			
		}
	}
	else
	{
		std::fill_n(pkXform, indexMap.size(), m_pkParent->_getParentNodeFullTransform());
	}
}
//----------------------------------------------------------------------------
unsigned short FvComponentEntity::getNumWorldTransforms() const
{
	const Mesh::IndexMap& indexMap = mMesh->sharedBlendIndexToBoneIndexMap;
	FV_ASSERT(indexMap.size() <= mNumBoneMatrices);

	return static_cast<unsigned short>(indexMap.size());
}
//----------------------------------------------------------------------------
Ogre::Real FvComponentEntity::getSquaredViewDepth(const Camera* cam) const
{
	Node* n = getParentNode();
	FV_ASSERT(n);

	return n->getSquaredViewDepth(cam);
}
//----------------------------------------------------------------------------
const LightList& FvComponentEntity::getLights() const
{
	return m_pkParent->queryLights();
}
//----------------------------------------------------------------------------
bool FvComponentEntity::getCastsShadows() const
{
	return getCastShadows();
}
//----------------------------------------------------------------------------
void FvComponentEntity::_updateRenderQueue(RenderQueue* queue)
{
	if (!mInitialised)
		return;

	if (mMesh->getStateCount() != mMeshStateCount)
	{
		_initialise(true);
	}

	if(m_pkIndexData->indexBuffer.getPointer())
	{
		if(mRenderQueueIDSet)
		{
			queue->addRenderable(this, mRenderQueueID);
		}
		else
		{
			queue->addRenderable(this);
		}
	}
	else
	{
		m_kComponentNode.Detach();
	}
}
//----------------------------------------------------------------------------
const String& FvComponentEntity::getMovableType( void ) const
{
	return FvComponentEntityFactory::FACTORY_TYPE_NAME;
}
//----------------------------------------------------------------------------
void FvComponentEntity::SetDisplayFlags(FvUInt32 u32Flags)
{
	FV_ASSERT(m_pkParent);

	if(u32Flags)
	{
		IndexData* apkIndexDatas[FV_COMPONENT_MAX_PARTS];
		FvUInt32 u32IndexCount(0);
		FvUInt32 u32SubMeshCount(0);

		for(FvUInt32 i(0), j(0); i < mMesh->getNumSubMeshes(); ++i)
		{
			if(FV_MASK_HAS_ANY(u32Flags, FV_MASK(i)))
			{
				apkIndexDatas[j] = mMesh->getSubMesh(i)->indexData;
				FV_ASSERT(apkIndexDatas[j]);
				FV_ASSERT(apkIndexDatas[j]->indexBuffer->getType() ==
					HardwareIndexBuffer::IT_16BIT);
				FV_ASSERT(apkIndexDatas[j]->indexStart == 0);
				FV_ASSERT(apkIndexDatas[j]->indexCount);

				u32IndexCount += apkIndexDatas[j]->indexCount;
				++j;
				++u32SubMeshCount;
			}
		}

		if(u32IndexCount)
		{
			m_pkParent->m_kVisibleComponentList.AttachBack(m_kComponentNode);
			FV_ASSERT(u32SubMeshCount);

			m_pkIndexData->indexStart = 0;
			m_pkIndexData->indexCount = 0;
			m_pkIndexData->indexBuffer = HardwareBufferManager::getSingleton().
				createIndexBuffer(HardwareIndexBuffer::IT_16BIT,
				u32IndexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

			for(FvUInt32 i(0); i < u32SubMeshCount; ++i)
			{
				void* pvSrc = apkIndexDatas[i]->indexBuffer->lock(
					HardwareBuffer::HBL_READ_ONLY);

				m_pkIndexData->indexBuffer->writeData(
					sizeof(FvUInt16) * m_pkIndexData->indexCount,
					sizeof(FvUInt16) * apkIndexDatas[i]->indexCount,
					pvSrc);

				apkIndexDatas[i]->indexBuffer->unlock();

				m_pkIndexData->indexCount += apkIndexDatas[i]->indexCount;
			}

			FV_ASSERT(m_pkIndexData->indexCount == u32IndexCount);
			return;
		}
	}
	
	m_kComponentNode.Detach();
	m_pkIndexData->indexStart = 0;
	m_pkIndexData->indexCount = 0;
	m_pkIndexData->indexBuffer.setNull();
}
//----------------------------------------------------------------------------
FvAnimatedEntity* FvComponentEntity::GetParent()
{
	return m_pkParent;
}
//----------------------------------------------------------------------------
String FvComponentEntityFactory::FACTORY_TYPE_NAME = "ComponentEntity";
//----------------------------------------------------------------------------
MovableObject* FvComponentEntityFactory::createInstanceImpl(
	const String& kName, const NameValuePairList* params)
{
	// must have mesh parameter
	MeshPtr spMesh;
	MaterialPtr spMaterial;

	if (params != 0)
	{
		{
			NameValuePairList::const_iterator ni = params->find("mesh");
			if (ni != params->end())
			{
				// Get mesh (load if required)
				spMesh = MeshManager::getSingleton().load(
					ni->second,
					// autodetect group location
					ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
			}
		}

		{
			NameValuePairList::const_iterator ni = params->find("material");
			if (ni != params->end())
			{
				// Get mesh (load if required)
				spMaterial = MaterialManager::getSingleton().load(
					ni->second,
					// autodetect group location
					ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
			}
		}
	}

	if (spMesh.isNull())
	{
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"'mesh' parameter required when constructing an ComponentEntity.",
			"FvComponentEntityFactory::createInstance");
	}

	if(spMaterial.isNull())
	{
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"'material' parameter required when constructing an ComponentEntity.",
			"FvComponentEntityFactory::createInstance");
	}

	return OGRE_NEW FvComponentEntity(kName, spMesh, spMaterial);
}
//----------------------------------------------------------------------------
const String& FvComponentEntityFactory::getType(void) const
{
	return FACTORY_TYPE_NAME;
}
//----------------------------------------------------------------------------
void FvComponentEntityFactory::destroyInstance(MovableObject* obj)
{
	OGRE_DELETE obj;
}
//----------------------------------------------------------------------------
