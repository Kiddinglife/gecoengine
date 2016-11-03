//#include "FvAvatarResource.h"
//#include "FvAvatar.h"
//#include <FvKernel.h>
//#include <FvXMLSection.h>
//#include <OgreMeshManager.h>
//#include <OgreSkeletonManager.h>
//#include <OgreMaterialManager.h>
//#include <OgreAnimation.h>
//#include <OgreBone.h>
//
//template<> FvAvatarResourceManager* Singleton<FvAvatarResourceManager>::ms_Singleton = NULL;
//const char* FvAvatarResourceManager::ms_pcGroupName = NULL;
//
////----------------------------------------------------------------------------
//FvAvatarResource::FvAvatarResource(ResourceManager *pkCreator,
//	const FvString &kName, ResourceHandle uiHandle, const FvString &kGroup,
//	bool bIsManual, ManualResourceLoader *pkLoader) :
//	Resource(pkCreator, kName, uiHandle, kGroup, bIsManual, pkLoader)
//{
//	m_u32SkeletonNumber = 0;
//	for(FvUInt32 i(0); i < FV_AVATAR_MAX_BINDING_NUMBER; ++i)
//	{
//		m_kSkeletonInfo[i].m_pfPartitionBuffer = NULL;
//		m_kSkeletonInfo[i].m_u32BoneNumber = 0;
//		m_kSkeletonInfo[i].m_u32PartitonNumber = 0;
//		m_kSkeletonInfo[i].m_bShowMainMesh = false;
//	}
//	m_pkActiveAnimations = NULL;
//	m_pkAnimationKey = NULL;
//	m_pbAnimationsLoop = NULL;
//	m_u32PartNumber = 0;
//	m_pkBoundingBoxParams = NULL;
//}
////----------------------------------------------------------------------------
//FvAvatarResource::~FvAvatarResource()
//{
//	unload();
//}
////----------------------------------------------------------------------------
//void FvAvatarResource::prepareImpl()
//{
//	bool bFileExist = ResourceGroupManager::getSingleton().resourceExists(
//		mGroup,mName);
//	if(bFileExist)
//	{
//		m_spFreshFromDisk =
//			ResourceGroupManager::getSingleton().openResource(
//			mName, mGroup, true, this);
//
//		m_spFreshFromDisk = DataStreamPtr(OGRE_NEW MemoryDataStream(
//			mName,m_spFreshFromDisk));
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarResource::unprepareImpl()
//{
//	m_spFreshFromDisk.setNull();
//}
////----------------------------------------------------------------------------
//void FvAvatarResource::loadImpl()
//{
//	FV_ASSERT(!m_u32SkeletonNumber);
//	if(m_spFreshFromDisk.isNull())
//	{
//		FV_CRITICAL_MSG("FvAvatarResource::loadImpl file %s not exist!\n",mName.c_str());
//		return;
//	}
//
//	FvXMLSectionPtr spAvatarSec =
//		FvXMLSection::OpenSection(m_spFreshFromDisk);
//	FV_ASSERT(spAvatarSec);
//	
//	std::vector<FvXMLSectionPtr> kAvatarSkeletonSecs;
//	spAvatarSec->OpenSections("AvatarSkeleton", kAvatarSkeletonSecs);
//	FV_ASSERT(kAvatarSkeletonSecs.size() > 0);
//	m_u32SkeletonNumber = kAvatarSkeletonSecs.size();
//
//	if(m_u32SkeletonNumber > FV_AVATAR_MAX_BINDING_NUMBER)
//		m_u32SkeletonNumber = FV_AVATAR_MAX_BINDING_NUMBER;
//
//	for(FvUInt32 i(0); i < m_u32SkeletonNumber; ++i)
//	{
//		FvString kMeshName = kAvatarSkeletonSecs[i]->ReadString("Mesh");
//		FV_ASSERT(kMeshName != "");
//
//		m_kSkeletonInfo[i].m_bShowMainMesh =
//			kAvatarSkeletonSecs[i]->GetAttributeBool("Mesh", "show");
//
//		m_kSkeletonInfo[i].m_spMainMesh = MeshManager::getSingleton().
//			load(kMeshName, mGroup);
//		FV_ASSERT(m_kSkeletonInfo[i].m_spMainMesh.getPointer());
//
//		std::vector<FvXMLSectionPtr> kMaterials;
//
//		kAvatarSkeletonSecs[i]->OpenSections("Material", kMaterials);
//
//		m_kSkeletonInfo[i].m_kMainMaterials.resize(kMaterials.size());
//
//		for(FvUInt32 j(0); j < kMaterials.size(); ++j)
//		{
//			FvString kMaterialName = kMaterials[j]->AsString("BaseWhiteNoLighting");
//			m_kSkeletonInfo[i].m_kMainMaterials[j] = Ogre::MaterialManager::getSingleton().load(kMaterialName, mGroup);
//		}
//
//		FvString kSkeletonName = kAvatarSkeletonSecs[i]->ReadString(
//			"Skeleton");
//		if(kSkeletonName == "")
//		{
//			m_kSkeletonInfo[i].m_spSkeleton =
//				m_kSkeletonInfo[i].m_spMainMesh->getSkeleton();
//		}
//		else
//		{
//			m_kSkeletonInfo[i].m_spSkeleton =
//				SkeletonManager::getSingleton().load(
//				kSkeletonName, mGroup);
//			m_kSkeletonInfo[i].m_spMainMesh->setSkeletonName(
//				kSkeletonName);
//		}
//		FV_ASSERT(m_kSkeletonInfo[i].m_spSkeleton.getPointer());
//
//		m_kSkeletonInfo[i].m_spSkeleton->setBlendMode(ANIMBLEND_CUMULATIVE);
//		
//		m_kSkeletonInfo[i].m_u32BoneNumber =
//			m_kSkeletonInfo[i].m_spSkeleton->getNumBones();
//
//		std::vector<FvXMLSectionPtr> kPartitionSecs;
//		kAvatarSkeletonSecs[i]->OpenSections("Partition", kPartitionSecs);
//
//		if(i == 0)
//		{
//			m_kSkeletonInfo[i].m_u32PartitonNumber = kPartitionSecs.size();
//			if(m_kSkeletonInfo[i].m_u32PartitonNumber)
//			{
//				m_kSkeletonInfo[i].m_pfPartitionBuffer = new float[
//					m_kSkeletonInfo[i].m_u32BoneNumber *
//					m_kSkeletonInfo[i].m_u32PartitonNumber];
//
//				for(FvUInt32 j(0);
//					j < m_kSkeletonInfo[i].m_u32PartitonNumber; ++j)
//				{
//					m_akPartitionNames[j] = kPartitionSecs[j]->
//						AsAttributeString("name");
//					FV_ASSERT(m_akPartitionNames[j] != "");
//					m_kPartitionNames.insert(
//						FvStringMap<FvUInt32>::value_type(
//						m_akPartitionNames[j].c_str(), j));
//				}
//			}
//		}
//		else
//		{
//			m_kSkeletonInfo[i].m_u32PartitonNumber =
//				m_kSkeletonInfo[0].m_u32PartitonNumber;
//			if(m_kSkeletonInfo[i].m_u32PartitonNumber)
//			{
//				m_kSkeletonInfo[i].m_pfPartitionBuffer = new float[
//					m_kSkeletonInfo[i].m_u32BoneNumber *
//						m_kSkeletonInfo[i].m_u32PartitonNumber];
//			}
//		}
//
//		for(FvUInt32 j(0); j < m_kSkeletonInfo[i].m_spSkeleton->getNumBones(); ++j)
//		{
//			m_kSkeletonInfo[i].m_kBoneNameHandleMap.insert(FvStringMap<FvUInt32>::value_type(
//				m_kSkeletonInfo[i].m_spSkeleton->getBone(j)->getName().c_str(),j));			
//		}
//
//		for(FvUInt32 j(0); j < kPartitionSecs.size(); ++j)
//		{
//			FvString kName = kPartitionSecs[j]->AsAttributeString("name");
//			FvStringMap<FvUInt32>::iterator it = m_kPartitionNames.find(
//				kName);
//			if(it == m_kPartitionNames.end()) continue;
//
//			FvUInt32 u32Id = it->second;
//
//			float* fCurrentBuffer = &m_kSkeletonInfo[i].m_pfPartitionBuffer[
//				u32Id * m_kSkeletonInfo[i].m_u32BoneNumber];
//
//			float fDefaultValue = kPartitionSecs[j]->AsAttributeFloat(
//				"default");
//
//			for(FvUInt32 k(0); k < m_kSkeletonInfo[i].m_u32BoneNumber; ++k)
//			{
//				fCurrentBuffer[k] = fDefaultValue;
//			}
//
//			std::vector<FvXMLSectionPtr> kBoneSecs;
//
//			kPartitionSecs[j]->OpenSections("Bone", kBoneSecs);
//
//			for(FvUInt32 k(0); k < kBoneSecs.size(); ++k)
//			{
//				FvStringMap<FvUInt32>::iterator it =
//					m_kSkeletonInfo[i].m_kBoneNameHandleMap.find(
//					kBoneSecs[k]->AsAttributeString("name"));
//
//				if(it != m_kSkeletonInfo[i].m_kBoneNameHandleMap.end())
//				{
//					FvUInt32 u32BoneId = it->second;
//
//					fCurrentBuffer[u32BoneId] = kBoneSecs[k]->AsFloat();
//				}
//			}
//		}
//	}
//
//	FvXMLSectionPtr spPartitionPropertySec = spAvatarSec->OpenSection(
//		"PartitionProperty");
//	if(spPartitionPropertySec)
//	{
//		std::vector<FvXMLSectionPtr> kPartitionSecs;
//		spPartitionPropertySec->OpenSections("Partition", kPartitionSecs);
//
//		for(FvUInt32 i(0); i < kPartitionSecs.size(); ++i)
//		{
//			FvString kName = kPartitionSecs[i]->AsAttributeString("name");
//			FvInt32 i32IdSrc;
//			if(kName == "")
//			{
//				i32IdSrc = FvAvatar::DO_NOT_USE;
//			}
//			else
//			{
//				FvStringMap<FvUInt32>::iterator it =
//					m_kPartitionNames.find(kName);
//				FV_ASSERT(it != m_kPartitionNames.end());
//				i32IdSrc = it->second;
//			}
//
//			std::vector<FvXMLSectionPtr> kTargetSecs;
//			kPartitionSecs[i]->OpenSections("Blend", kTargetSecs);
//
//			for(FvUInt32 j(0); j < kTargetSecs.size(); ++j)
//			{
//				FvString kTargetName = kTargetSecs[j]->AsAttributeString(
//					"name");
//
//				if(kTargetName == kName) continue;
//
//				FvInt32 i32IdTarget;
//				if(kTargetName == "")
//				{
//					i32IdTarget = FvAvatar::DO_NOT_USE;
//				}
//				else
//				{
//					FvStringMap<FvUInt32>::iterator it =
//						m_kPartitionNames.find(kTargetName);
//					FV_ASSERT(it != m_kPartitionNames.end());
//					i32IdTarget = it->second;
//				}
//				float fTime = kTargetSecs[j]->AsFloat();
//				
//				m_kPartitionBlendMap.insert(BlendMap::value_type(
//					std::pair<FvInt32, FvInt32>(i32IdSrc, i32IdTarget),
//					fTime));
//			}
//		}
//	}
//
//	FvXMLSectionPtr spAnimationMapSec = spAvatarSec->OpenSection(
//		"AnimationMap");
//
//	m_u32kActiveAnimationNumber = m_kSkeletonInfo[0].m_spSkeleton->
//		getNumAnimations();
//
//	bool bAutoFill = true;
//
//	if(spAnimationMapSec)
//	{
//		std::vector<FvXMLSectionPtr> kAnimationSecs;
//		spAnimationMapSec->OpenSections("Animation", kAnimationSecs);
//
//		bAutoFill = spAnimationMapSec->AsAttributeBool("auto", true);
//
//		if(!bAutoFill)
//		{
//			m_u32kActiveAnimationNumber = kAnimationSecs.size();
//		}
//
//		m_pkActiveAnimations = new FvString[m_u32kActiveAnimationNumber];
//		m_pkAnimationKey = new KeyMap[m_u32kActiveAnimationNumber];
//		m_pbAnimationsLoop = new bool[m_u32kActiveAnimationNumber];
//
//		FvZeroMemoryEx(m_pbAnimationsLoop, sizeof(bool),
//			m_u32kActiveAnimationNumber);
//
//		for(FvUInt32 i(0); i < kAnimationSecs.size(); ++i)
//		{
//			FvString kName = kAnimationSecs[i]->AsAttributeString("name");
//			bool bIsLoop = kAnimationSecs[i]->AsAttributeBool("loop");
//			FvUInt32 u32Id = kAnimationSecs[i]->AsInt();
//
//			FV_ASSERT(u32Id < m_u32kActiveAnimationNumber);
//			FV_ASSERT(m_pkActiveAnimations[u32Id] == "");
//
//			m_pkActiveAnimations[u32Id] = kName;
//			m_pbAnimationsLoop[u32Id] = bIsLoop;
//			m_kAnimationNames.insert(FvStringMap<FvUInt32>::value_type(
//				kName.c_str(), u32Id));
//			
//		}
//	}
//	else
//	{
//		m_pkActiveAnimations = new FvString[m_u32kActiveAnimationNumber];
//		m_pkAnimationKey = new KeyMap[m_u32kActiveAnimationNumber];
//		m_pbAnimationsLoop = new bool[m_u32kActiveAnimationNumber];
//
//		FvZeroMemoryEx(m_pbAnimationsLoop, sizeof(bool),
//			m_u32kActiveAnimationNumber);
//	}
//
//	if(bAutoFill)
//	{
//		for(FvUInt32 i(0), j(0); i < m_u32kActiveAnimationNumber; ++i)
//		{
//			if(m_pkActiveAnimations[i] != "") continue;
//
//			FvString kName;
//
//			while(true)
//			{
//				const FvString& rkName = m_kSkeletonInfo[0].m_spSkeleton->
//					getAnimation(j)->getName();
//				if(m_kAnimationNames.find(rkName) == m_kAnimationNames.end())
//				{
//					kName = rkName;
//					break;
//				}
//				else
//				{
//					++j;
//					if(j >= m_kSkeletonInfo[0].m_spSkeleton->
//						getNumAnimations())
//					{
//						kName = "";
//						break;
//					}
//				}
//			}
//
//			if(kName == "")
//			{
//				break;
//			}
//			else
//			{
//				m_pkActiveAnimations[i] = kName;
//				m_kAnimationNames.insert(FvStringMap<FvUInt32>::value_type(
//					kName.c_str(), i));
//			}
//			
//		}
//	}
//
//	FvXMLSectionPtr spAnimationPropertySec = spAvatarSec->OpenSection(
//		"AnimationProperty");
//	if(spAnimationPropertySec)
//	{
//		std::vector<FvXMLSectionPtr> kAnimationSecs;
//		spAnimationPropertySec->OpenSections("Animation", kAnimationSecs);
//		for(FvUInt32 i(0); i < kAnimationSecs.size(); ++i)
//		{
//			FvString kSrcName = kAnimationSecs[i]->AsAttributeString("name");
//
//			FvInt32 i32SrcId;
//
//			if(kSrcName == "")
//			{
//				i32SrcId = FvAvatar::DO_NOT_USE;
//			}
//			else
//			{
//				FvStringMap<FvUInt32>::iterator it = m_kAnimationNames.find(
//					kSrcName);
//				if(it == m_kAnimationNames.end())
//				{
//					FV_CRITICAL_MSG("Blend Src Animation(%s) is none\n", kSrcName.c_str());
//				}
//
//				//FV_ASSERT(it != m_kAnimationNames.end());
//				i32SrcId = it->second;
//			}			
//
//			std::vector<FvXMLSectionPtr> kBlendSecs;
//			kAnimationSecs[i]->OpenSections("Blend", kBlendSecs);
//
//			for(FvUInt32 j(0); j < kBlendSecs.size(); ++j)
//			{
//				FvString kDstName = kBlendSecs[j]->AsAttributeString("name");
//
//				if(kSrcName == kDstName) continue;
//
//				FvInt32 i32DstId;
//				if(kDstName == "")
//				{
//					i32DstId = FvAvatar::DO_NOT_USE;
//				}
//				else
//				{
//					FvStringMap<FvUInt32>::iterator it =
//						m_kAnimationNames.find(kDstName);
//
//					if(it == m_kAnimationNames.end())
//					{
//						FV_CRITICAL_MSG("Blend Dst Animation(%s) is none\n", kDstName.c_str());
//					}
//
//					//FV_ASSERT(it != m_kAnimationNames.end());
//					i32DstId = it->second;
//				}
//
//				float fTime = kBlendSecs[j]->AsFloat();
//
//				m_kAnimationBlendMap.insert(BlendMap::value_type(
//					std::pair<FvInt32, FvInt32>(i32SrcId, i32DstId),
//					fTime));
//			}
//
//			if(i32SrcId < 0) continue;
//
//			std::vector<FvXMLSectionPtr> kKeySecs;
//			kAnimationSecs[i]->OpenSections("Key", kKeySecs);
//			for(FvUInt32 j(0); j < kKeySecs.size(); ++j)
//			{
//				FvString kKeyName =
//					kKeySecs[j]->AsAttributeString("name").c_str();
//				float fTime = kKeySecs[j]->AsFloat();
//
//				m_pkAnimationKey[i32SrcId].insert(
//					KeyMap::value_type(kKeyName.c_str(), fTime));
//			}
//		}
//	}
//
//	m_u32PartNumber = (FvUInt32)spAvatarSec->ReadInt("PartNumber");
//	FvXMLSectionPtr spAliasName = spAvatarSec->OpenSection("AliasName");
//	if(spAliasName)
//	{
//		for(FvXMLSectionIterator kIt = spAliasName->Begin(); kIt != spAliasName->End(); ++kIt)
//			m_kAliasName.insert(AliasName::value_type((*kIt)->SectionName(),(*kIt)->AsString()));
//	}
//
//	FvXMLSectionPtr spShotCamera = spAvatarSec->OpenSection("ShotCamera");
//	if(spShotCamera)
//	{
//		m_kShotCamera.reserve(spShotCamera->CountChildren());
//		for(FvXMLSectionIterator kIt = spShotCamera->Begin(); kIt != spShotCamera->End(); ++kIt)
//		{
//			FvVector3 kPosition = (*kIt)->ReadVector3("Position");
//			FvQuaternion kQuaternion = (*kIt)->ReadQuaternion("Quaternion");
//			m_kShotCamera.push_back(std::pair<Ogre::Vector3,Ogre::Quaternion>(
//				*(Ogre::Vector3*)&kPosition,
//				Ogre::Quaternion(kQuaternion.w,kQuaternion.x,kQuaternion.y,kQuaternion.z)));
//		}
//	}
//
//	FvXMLSectionPtr spBoundingBoxParams = spAvatarSec->OpenSection("BoundingBoxParams");
//	if(spBoundingBoxParams)
//	{
//		m_pkBoundingBoxParams = new BoundingBoxParams;
//		m_pkBoundingBoxParams->m_kTailPointName = spBoundingBoxParams->ReadString("TailPointName", "");
//
//		FvVector3 kMin = spBoundingBoxParams->ReadVector3("Min");
//		FvVector3 kMax = spBoundingBoxParams->ReadVector3("Max");
//
//		m_pkBoundingBoxParams->m_kBaseBoundingBox.SetBounds(kMin, kMax);
//	}
//}
////----------------------------------------------------------------------------
//void FvAvatarResource::unloadImpl()
//{
//	FV_ASSERT(m_u32SkeletonNumber);
//
//	for(FvUInt32 i(0); i < FV_AVATAR_MAX_BINDING_NUMBER; ++i)
//	{
//		m_kSkeletonInfo[i].m_spMainMesh.setNull();
//		m_kSkeletonInfo[i].m_spSkeleton.setNull();
//		FV_SAFE_DELETE_ARRAY(m_kSkeletonInfo[i].m_pfPartitionBuffer);
//	}
//
//	m_u32SkeletonNumber = 0;
//
//	m_kPartitionNames.clear();
//	m_kAnimationNames.clear();
//	m_kPartitionBlendMap.clear();
//	m_kAnimationBlendMap.clear();
//
//	FV_SAFE_DELETE_ARRAY(m_pkActiveAnimations);
//	FV_SAFE_DELETE_ARRAY(m_pkAnimationKey);
//	FV_SAFE_DELETE_ARRAY(m_pbAnimationsLoop);
//	FV_SAFE_DELETE(m_pkBoundingBoxParams);
//}
////----------------------------------------------------------------------------
//size_t FvAvatarResource::calculateSize() const
//{
//	return m_spFreshFromDisk.isNull() ? 0:m_spFreshFromDisk->size();
//}
////----------------------------------------------------------------------------
//FvUInt32 FvAvatarResource::GetBoneNumber(FvUInt32 u32Binding)
//{
//	return m_kSkeletonInfo[u32Binding].m_spSkeleton->getNumBones();
//}
////----------------------------------------------------------------------------
//void FvAvatarResource::ManualPartition(FvUInt32 u32Number,
//	float** ppfPartition, const char** pcNames)
//{
//	for(FvUInt32 i(0); i < FV_AVATAR_MAX_BINDING_NUMBER; ++i)
//	{
//		FV_SAFE_DELETE_ARRAY(m_kSkeletonInfo[i].m_pfPartitionBuffer);
//		m_kSkeletonInfo[i].m_pfPartitionBuffer = new float[
//			m_kSkeletonInfo[i].m_u32BoneNumber * u32Number];
//
//		FvCrazyCopyEx(m_kSkeletonInfo[i].m_pfPartitionBuffer,
//			ppfPartition[i], sizeof(float),
//			m_kSkeletonInfo[i].m_u32BoneNumber * u32Number);
//	}
//
//	m_kPartitionNames.clear();
//
//	for(FvUInt32 i(0); i < u32Number; ++i)
//	{
//		m_akPartitionNames[i] = pcNames[i];
//		m_kPartitionNames.insert(FvStringMap<FvUInt32>::value_type(
//			m_akPartitionNames[i].c_str(), i));
//	}
//}
////----------------------------------------------------------------------------
//bool FvAvatarResource::HasAliasName(const FvString& kName)
//{
//	return (m_kAliasName.find(kName) != m_kAliasName.end());
//}
////----------------------------------------------------------------------------
//const FvString& FvAvatarResource::GetAliasName(const FvString& kName) const
//{
//	AliasName::const_iterator kFind = m_kAliasName.find(kName);
//	if(m_kAliasName.find(kName) != m_kAliasName.end())
//		return kFind->second;
//
//	static FvString s_kNone;
//	return s_kNone;
//}
////----------------------------------------------------------------------------
//const std::pair<Ogre::Vector3,Ogre::Quaternion>& FvAvatarResource::GetShotCamera(FvUInt32 uiIndex) const
//{
//	if(uiIndex < m_kShotCamera.size())
//		return m_kShotCamera[uiIndex];
//
//	static std::pair<Ogre::Vector3,Ogre::Quaternion> s_kNone;
//	return s_kNone;
//}
////----------------------------------------------------------------------------
//FvAvatarResourcePtr::FvAvatarResourcePtr(const ResourcePtr& r) :
//	SharedPtr<FvAvatarResource>()
//{
//	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
//	{
//		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
//			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
//			pRep = static_cast<FvAvatarResource*>(r.getPointer());
//		pUseCount = r.useCountPointer();
//		if (pUseCount)
//		{
//			++(*pUseCount);
//		}
//	}
//}
////----------------------------------------------------------------------------
//FvAvatarResourcePtr& FvAvatarResourcePtr::operator=(const ResourcePtr& r)
//{
//	if (pRep == static_cast<FvAvatarResource*>(r.getPointer()))
//		return *this;
//	release();
//	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
//	{
//		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
//			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
//			pRep = static_cast<FvAvatarResource*>(r.getPointer());
//		pUseCount = r.useCountPointer();
//		if (pUseCount)
//		{
//			++(*pUseCount);
//		}
//	}
//	else
//	{
//		assert(r.isNull() && "RHS must be null if it has no mutex!");
//		setNull();
//	}
//	return *this;
//}
////----------------------------------------------------------------------------
//void FvAvatarResourcePtr::destroy()
//{
//	Ogre::SharedPtr<FvAvatarResource>::destroy();
//}
////----------------------------------------------------------------------------
//FvAvatarResourceManager::FvAvatarResourceManager()
//{
//	mLoadOrder = 350.0f;
//	mResourceType = "AvatarResource";
//
//	ResourceGroupManager::getSingleton()._registerResourceManager(
//		mResourceType, this);
//}
////----------------------------------------------------------------------------
//FvAvatarResourceManager::~FvAvatarResourceManager()
//{
//	ResourceGroupManager::getSingleton()._unregisterResourceManager(
//		mResourceType);
//}
////----------------------------------------------------------------------------
//FvAvatarResourceManager * FvAvatarResourceManager::Create(
//	const char* pcGroupName)
//{
//	ms_pcGroupName = pcGroupName;
//	return OGRE_NEW FvAvatarResourceManager;
//}
////----------------------------------------------------------------------------
//void FvAvatarResourceManager::Destory()
//{
//	OGRE_DELETE ms_Singleton;
//	ms_Singleton = NULL;
//}
////----------------------------------------------------------------------------
//FvAvatarResourceManager& FvAvatarResourceManager::getSingleton()
//{
//	FV_ASSERT(ms_Singleton);
//	return (*ms_Singleton);
//}
////----------------------------------------------------------------------------
//FvAvatarResourceManager* FvAvatarResourceManager::getSingletonPtr()
//{
//	return ms_Singleton;
//}
////----------------------------------------------------------------------------
//Resource* FvAvatarResourceManager::createImpl(const String &kName,
//	ResourceHandle uiHandle, const String &kGroup, bool bIsManual,
//	ManualResourceLoader *pkLoader, const NameValuePairList *pkCreateParams)
//{
//	return OGRE_NEW FvAvatarResource(this, kName, uiHandle, kGroup,
//		bIsManual, pkLoader);
//}
////----------------------------------------------------------------------------
//const char* FvAvatarResourceManager::GetGroupName()
//{
//	FV_ASSERT(ms_pcGroupName);
//	return ms_pcGroupName;
//}
////----------------------------------------------------------------------------
