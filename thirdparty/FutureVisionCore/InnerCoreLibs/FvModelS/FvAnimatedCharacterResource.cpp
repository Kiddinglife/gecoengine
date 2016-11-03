#include "FvAnimatedCharacterResource.h"
#include <FvDebug.h>
#include <FvXMLSection.h>
#include <OgreMeshManager.h>
#include <OgreMaterialManager.h>
#include <OgreSkeletonManager.h>
#include <OgreBone.h>
#include <OgreAnimation.h>

using namespace Ogre;

template<> FvAnimatedCharacterResourceManager* Singleton<FvAnimatedCharacterResourceManager>::ms_Singleton = NULL;
const char* FvAnimatedCharacterResourceManager::ms_pcGroupName = NULL;

template<> FvAnimationSetupResourceManager* Singleton<FvAnimationSetupResourceManager>::ms_Singleton = NULL;
const char* FvAnimationSetupResourceManager::ms_pcGroupName = NULL;

//----------------------------------------------------------------------------
FvAnimatedCharacterResource::FvAnimatedCharacterResource(
	Ogre::ResourceManager *pkCreator, const FvString &kName,
	Ogre::ResourceHandle uiHandle, const FvString &kGroup, bool bIsManual,
	Ogre::ManualResourceLoader *pkLoader)
	: Resource(pkCreator, kName, uiHandle, kGroup, bIsManual, pkLoader),
	m_pkBlendMaskNames(NULL), m_pkAnimations(NULL), m_u32PartNumber(0),
	m_pkBoundingBoxParams(NULL)
{
	
}
//----------------------------------------------------------------------------
FvAnimatedCharacterResource::~FvAnimatedCharacterResource()
{
	unload();
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterResource::prepareImpl()
{
	bool bFileExist = ResourceGroupManager::getSingleton().resourceExists(
		mGroup,mName);
	if(bFileExist)
	{
		m_spFreshFromDisk =
			ResourceGroupManager::getSingleton().openResource(
			mName, mGroup, true, this);

		m_spFreshFromDisk = DataStreamPtr(OGRE_NEW MemoryDataStream(
			mName,m_spFreshFromDisk));
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterResource::unprepareImpl()
{
	m_spFreshFromDisk.setNull();
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterResource::loadImpl()
{
	if(m_spFreshFromDisk.isNull())
	{
		FV_CRITICAL_MSG("FvAvatarResource::loadImpl file %s not exist!\n",mName.c_str());
		return;
	}

	FvXMLSectionPtr spAnimatedCharacterSec =
		FvXMLSection::OpenSection(m_spFreshFromDisk);
	FV_ASSERT(spAnimatedCharacterSec);

	std::vector<FvXMLSectionPtr> kBindingSecs;
	spAnimatedCharacterSec->OpenSections("Binding", kBindingSecs);
	FV_ASSERT(kBindingSecs.size() > 0);
	m_kSkeletonInfo.resize(kBindingSecs.size());

	SkeletonInfo* pkInfo = &(m_kSkeletonInfo[0]);

	for(FvUInt32 i(0); i < m_kSkeletonInfo.size(); ++i, ++pkInfo)
	{
		FvString kMeshName = kBindingSecs[i]->ReadString("Mesh");
		FV_ASSERT(kMeshName != "");
		pkInfo->m_bShowMainMesh = kBindingSecs[i]->GetAttributeBool("Mesh", "show");

		pkInfo->m_spMainMesh = MeshManager::getSingleton().load(kMeshName, mGroup);
		FV_ASSERT(m_kSkeletonInfo[i].m_spMainMesh.getPointer());

		std::vector<FvXMLSectionPtr> kMaterials;
		kBindingSecs[i]->OpenSections("Material", kMaterials);
		pkInfo->m_kMainMaterials.resize(kMaterials.size());

		for(FvUInt32 j(0); j < kMaterials.size(); ++j)
		{
			FvString kMaterialName = kMaterials[j]->AsString("BaseWhiteNoLighting");
			pkInfo->m_kMainMaterials[j] = Ogre::MaterialManager::getSingleton().load(kMaterialName, mGroup);
		}

		FvString kSkeletonName = kBindingSecs[i]->ReadString(
			"Skeleton");
		if(kSkeletonName == "")
		{
			pkInfo->m_spSkeleton = pkInfo->m_spMainMesh->getSkeleton();
		}
		else
		{
			pkInfo->m_spSkeleton = SkeletonManager::getSingleton().load(
				kSkeletonName, mGroup);
			pkInfo->m_spMainMesh->setSkeletonName(kSkeletonName);
		}
		FV_ASSERT(pkInfo->m_spSkeleton.getPointer());

		pkInfo->m_u32BoneNumber = pkInfo->m_spSkeleton->getNumBones();
		pkInfo->m_pu8PartitionBuffer = new FvUInt8[pkInfo->m_u32BoneNumber + 1];
		
		std::vector<FvXMLSectionPtr> kPartitionSecs;
		kBindingSecs[i]->OpenSections("Partition", kPartitionSecs);
		FV_ASSERT(kPartitionSecs.size() < 0xFF);

		*(pkInfo->m_pu8PartitionBuffer) = kPartitionSecs.size();
		FvUInt8* pu8PartitionBuffer = pkInfo->m_pu8PartitionBuffer + 1;

		memset(pu8PartitionBuffer, 0, pkInfo->m_u32BoneNumber);

		for(FvUInt32 j(0); j < kPartitionSecs.size(); ++j)
		{
			std::vector<FvXMLSectionPtr> kBoneSecs;
			kPartitionSecs[j]->OpenSections("Bone", kBoneSecs);

			for(FvUInt32 k(0); k < kBoneSecs.size(); ++k)
			{
				Bone* pkBone = pkInfo->m_spSkeleton->getBone(kBoneSecs[k]->AsAttributeString("name"));
				pu8PartitionBuffer[pkBone->getHandle()] = j;
			}
		}

		std::vector<FvXMLSectionPtr> kBlendMaskSecs;
		kBindingSecs[i]->OpenSections("BlendMask", kBlendMaskSecs);

		if(i) FV_ASSERT(kBlendMaskSecs.size() == m_kSkeletonInfo[0].m_u32BlendMaskNumber);

		if(kBlendMaskSecs.size())
		{
			pkInfo->m_u32BlendMaskNumber = kBlendMaskSecs.size();
			pkInfo->m_pfBlendMaskBuffer = new float[pkInfo->m_u32BlendMaskNumber * pkInfo->m_u32BoneNumber];

			if(i == 0)
			{
				m_pkBlendMaskNames = new FvString[pkInfo->m_u32BlendMaskNumber];

				for(FvUInt32 j(0); j < kBlendMaskSecs.size(); ++j)
				{
					m_pkBlendMaskNames[j] = kBlendMaskSecs[j]->AsAttributeString("name");
					m_kBlendMaskIndexes.insert(FvStringMap<FvUInt32>::value_type(m_pkBlendMaskNames[j].c_str(), j));
				}
			}
		}
		else
		{
			pkInfo->m_pfBlendMaskBuffer = NULL;
			pkInfo->m_u32BlendMaskNumber = 0;
			m_pkBlendMaskNames = NULL;
		}
		
		for(FvUInt32 j(0); j < kBlendMaskSecs.size(); ++j)
		{
			FvStringMap<FvUInt32>::iterator it = m_kBlendMaskIndexes.find(kBlendMaskSecs[j]->AsAttributeString("name"));
			if(it == m_kBlendMaskIndexes.end()) continue;

			FvUInt32 u32Id = it->second;

			float* fCurrentBuffer = &(pkInfo->m_pfBlendMaskBuffer[u32Id * pkInfo->m_u32BoneNumber]);

			float fDefaultValue = kBlendMaskSecs[j]->AsAttributeFloat("default");

			for(FvUInt32 k(0); k < pkInfo->m_u32BoneNumber; ++k)
			{
				fCurrentBuffer[k] = fDefaultValue;
			}

			std::vector<FvXMLSectionPtr> kBoneSecs;

			kBlendMaskSecs[j]->OpenSections("Bone", kBoneSecs);

			for(FvUInt32 k(0); k < kBoneSecs.size(); ++k)
			{
				Bone* pkBone = pkInfo->m_spSkeleton->getBone(kBoneSecs[k]->AsAttributeString("name"));
				fCurrentBuffer[pkBone->getHandle()] = kBoneSecs[k]->AsFloat();
			}
		}
	}

	pkInfo = &(m_kSkeletonInfo[0]);

	FvUInt32 u32NumAnimations = pkInfo->m_spSkeleton->getNumAnimations();

	m_pkAnimations = new Animation[u32NumAnimations];

	for(FvUInt32 i(0); i < u32NumAnimations; ++i)
	{
		m_pkAnimations[i].m_kName = pkInfo->m_spSkeleton->getAnimation(i)->getName();
		m_kAnimationIndexes.insert(FvStringMap<FvUInt32>::value_type(m_pkAnimations[i].m_kName.c_str(), i));
	}

	FvXMLSectionPtr spAnimationPropertySec = spAnimatedCharacterSec->OpenSection("AnimationProperty");
	if(spAnimationPropertySec)
	{
		std::vector<FvXMLSectionPtr> kAnimationSecs;
		spAnimationPropertySec->OpenSections("Animation", kAnimationSecs);

		for(FvUInt32 i(0); i < kAnimationSecs.size(); ++i)
		{
			FvString kAnimation = kAnimationSecs[i]->AsAttributeString("name");
			FvStringMap<FvUInt32>::iterator it = m_kAnimationIndexes.find(kAnimation);
			if(it == m_kAnimationIndexes.end()) continue;
			Animation& kAni = m_pkAnimations[it->second];
			
			FvXMLSectionPtr spBlendSec = kAnimationSecs[i]->OpenSection("Blend");
			if(spBlendSec)
			{
				m_kAnimationBlendMap.insert(BlendMap::value_type(it->second, spBlendSec->AsFloat()));
			}

			std::vector<FvXMLSectionPtr> kKeySecs;
			kAnimationSecs[i]->OpenSections("Key", kKeySecs);
			kAni.m_kKeySet.resize(kKeySecs.size());

			for(FvUInt32 j(0); j < kKeySecs.size(); ++j)
			{
				AnimationKey& kKey = kAni.m_kKeySet[j];
				kKey.m_kName = kKeySecs[j]->AsAttributeString("name");
				kKey.m_fTimePos = kKeySecs[j]->AsFloat();
			}
		}

		std::vector<FvXMLSectionPtr> kInterBlendSecs;
		spAnimationPropertySec->OpenSections("InterBlend", kInterBlendSecs);
		for(FvUInt32 i(0); i < kInterBlendSecs.size(); ++i)
		{
			FvString kFrom = kInterBlendSecs[i]->AsAttributeString("from");
			FvString kTo = kInterBlendSecs[i]->AsAttributeString("to");
			float fTimePos = kInterBlendSecs[i]->AsFloat();

			FvInt32 i32From = -1;
			FvInt32 i32To = -1;

			if(kFrom != "")
			{
				FvStringMap<FvUInt32>::iterator it = m_kAnimationIndexes.find(kFrom);
				if(it != m_kAnimationIndexes.end())
				{
					i32From = it->second;
				}
			}
			if(kTo != "")
			{
				FvStringMap<FvUInt32>::iterator it = m_kAnimationIndexes.find(kTo);
				if(it != m_kAnimationIndexes.end())
				{
					i32To = it->second;
				}
			}

			m_kAnimationInterBlendMap.insert(InterBlendMap::value_type(std::make_pair(i32From, i32To), fTimePos));
		}
	}

	m_u32PartNumber = spAnimatedCharacterSec->ReadInt("PartNumber");

	FvXMLSectionPtr spBoneAliasSec = spAnimatedCharacterSec->OpenSection("BoneAlias");
	if(spBoneAliasSec)
	{
		Offset kOffset;
		for(FvXMLSectionIterator kIt = spBoneAliasSec->Begin(); kIt != spBoneAliasSec->End(); ++kIt)
		{
			FvQuaternion kRotation = (*kIt)->AsAttributeQuaternion("Rot", FvQuaternion(FvVector3::ZERO, 1.0f));
			kRotation.Normalise();
			FvVector3 kPosition = (*kIt)->AsAttributeVector3("Pos");

			kOffset.m_kBoneName = (*kIt)->AsString();
			kOffset.m_kPosition = *(Ogre::Vector3*)&kPosition;
			kOffset.m_kRotation.x = kRotation.x;
			kOffset.m_kRotation.y = kRotation.y;
			kOffset.m_kRotation.z = kRotation.z;
			kOffset.m_kRotation.w = kRotation.w;

			m_kBoneAlias.insert(AliasTagPoint::value_type((*kIt)->SectionName().c_str(),kOffset));
		}
	}

	FvXMLSectionPtr spAnimationAliasSec = spAnimatedCharacterSec->OpenSection("AnimationAliasName");
	if(spAnimationAliasSec)
	{
		for(FvXMLSectionIterator kIt = spAnimationAliasSec->Begin(); kIt != spAnimationAliasSec->End(); ++kIt)
			m_kAnimationAlias.insert(AliasName::value_type((*kIt)->SectionName(),(*kIt)->AsString()));
	}

	FvXMLSectionPtr spShotCamera = spAnimatedCharacterSec->OpenSection("ShotCamera");
	if(spShotCamera)
	{
		m_kShotCamera.reserve(spShotCamera->CountChildren());
		for(FvXMLSectionIterator kIt = spShotCamera->Begin(); kIt != spShotCamera->End(); ++kIt)
		{
			FvVector4 kPosition = (*kIt)->ReadVector4("Position");
			FvQuaternion kQuaternion = (*kIt)->ReadQuaternion("Quaternion");
			m_kShotCamera.push_back(std::pair<Ogre::Vector4,Ogre::Quaternion>(
				*(Ogre::Vector4*)&kPosition,
				Ogre::Quaternion(kQuaternion.w,kQuaternion.x,kQuaternion.y,kQuaternion.z)));
		}
	}

	FvXMLSectionPtr spBoundingBoxParams = spAnimatedCharacterSec->OpenSection("BoundingBoxParams");
	if(spBoundingBoxParams)
	{
		m_pkBoundingBoxParams = new BoundingBoxParams;
		m_pkBoundingBoxParams->m_kTailPointName = spBoundingBoxParams->ReadString("TailPointName", "");

		m_pkBoundingBoxParams->m_kMin = spBoundingBoxParams->ReadVector3("Min");
		m_pkBoundingBoxParams->m_kMax = spBoundingBoxParams->ReadVector3("Max");
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterResource::unloadImpl()
{
	for(FvUInt32 i(0); i < m_kSkeletonInfo.size(); ++i)
	{
		SkeletonInfo& kInfo = m_kSkeletonInfo[i];
		FV_SAFE_DELETE_ARRAY(kInfo.m_pu8PartitionBuffer);
		FV_SAFE_DELETE_ARRAY(kInfo.m_pfBlendMaskBuffer);
	}
	m_kSkeletonInfo.clear();
	m_kBlendMaskIndexes.clear();
	FV_SAFE_DELETE_ARRAY(m_pkBlendMaskNames);
	m_kAnimationIndexes.clear();
	FV_SAFE_DELETE_ARRAY(m_pkAnimations);
	m_kAnimationBlendMap.clear();
	m_kAnimationInterBlendMap.clear();
	m_u32PartNumber = 0;
	m_kBoneAlias.clear();
	m_kAnimationAlias.clear();

	FV_SAFE_DELETE(m_pkBoundingBoxParams);
}
//----------------------------------------------------------------------------
size_t FvAnimatedCharacterResource::calculateSize() const
{
	return m_spFreshFromDisk.isNull() ? 0:m_spFreshFromDisk->size();
}
//----------------------------------------------------------------------------
FvUInt32 FvAnimatedCharacterResource::GetNumAnimation()
{
	return m_kAnimationIndexes.size();
}
//----------------------------------------------------------------------------
FvUInt8* FvAnimatedCharacterResource::GetBonesPartTable(FvUInt32 u32Binding)
{
	FV_ASSERT(u32Binding < m_kSkeletonInfo.size());
	return m_kSkeletonInfo[u32Binding].m_pu8PartitionBuffer;
}
//----------------------------------------------------------------------------
const std::pair<Ogre::Vector4,Ogre::Quaternion>& FvAnimatedCharacterResource::GetShotCamera(FvUInt32 uiIndex) const
{
	if(uiIndex < m_kShotCamera.size())
		return m_kShotCamera[uiIndex];

	static std::pair<Ogre::Vector4,Ogre::Quaternion> s_kNone;
	return s_kNone;
}
//----------------------------------------------------------------------------
FvAnimatedCharacterResourcePtr::FvAnimatedCharacterResourcePtr(
	const Ogre::ResourcePtr& r)
{
	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
	{
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<FvAnimatedCharacterResource*>(r.getPointer());
		pUseCount = r.useCountPointer();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacterResourcePtr& FvAnimatedCharacterResourcePtr::operator = (
	const Ogre::ResourcePtr& r)
{
	if (pRep == static_cast<FvAnimatedCharacterResource*>(r.getPointer()))
		return *this;
	release();
	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
	{
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<FvAnimatedCharacterResource*>(r.getPointer());
		pUseCount = r.useCountPointer();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}
	else
	{
		assert(r.isNull() && "RHS must be null if it has no mutex!");
		setNull();
	}
	return *this;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterResourcePtr::destroy()
{
	SharedPtr<FvAnimatedCharacterResource>::destroy();
}
//----------------------------------------------------------------------------
FvAnimatedCharacterResourceManager::FvAnimatedCharacterResourceManager()
{
	mLoadOrder = 350.0f;
	mResourceType = "AnimatedCharacter";

	ResourceGroupManager::getSingleton()._registerResourceManager(
		mResourceType, this);
}
//----------------------------------------------------------------------------
FvAnimatedCharacterResourceManager::~FvAnimatedCharacterResourceManager()
{
	ResourceGroupManager::getSingleton()._unregisterResourceManager(
		mResourceType);
}
//----------------------------------------------------------------------------
FvAnimatedCharacterResourceManager*
FvAnimatedCharacterResourceManager::Create(const char* pcGroupName)
{
	ms_pcGroupName = pcGroupName;
	return OGRE_NEW FvAnimatedCharacterResourceManager;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterResourceManager::Destory()
{
	OGRE_DELETE ms_Singleton;
	ms_Singleton = NULL;
}
//----------------------------------------------------------------------------
FvAnimatedCharacterResourceManager&
FvAnimatedCharacterResourceManager::getSingleton()
{
	FV_ASSERT(ms_Singleton);
	return (*ms_Singleton);
}
//----------------------------------------------------------------------------
FvAnimatedCharacterResourceManager*
FvAnimatedCharacterResourceManager::getSingletonPtr()
{
	return ms_Singleton;
}
//----------------------------------------------------------------------------
const char* FvAnimatedCharacterResourceManager::GetGroupName()
{
	FV_ASSERT(ms_pcGroupName);
	return ms_pcGroupName;
}
//----------------------------------------------------------------------------
Resource* FvAnimatedCharacterResourceManager::createImpl(const String &kName,
	ResourceHandle uiHandle, const String &kGroup, bool bIsManual,
	ManualResourceLoader *pkLoader,	const NameValuePairList *pkCreateParams)
{
	return OGRE_NEW FvAnimatedCharacterResource(this, kName, uiHandle, kGroup,
		bIsManual, pkLoader);
}
//----------------------------------------------------------------------------
FvAnimationSetupResource::FvAnimationSetupResource(
	Ogre::ResourceManager *pkCreator, const FvString &kName,
	Ogre::ResourceHandle uiHandle, const FvString &kGroup, bool bIsManual,
	Ogre::ManualResourceLoader *pkLoader) : Resource(pkCreator,
	kName, uiHandle, kGroup, bIsManual, pkLoader)
{

}
//----------------------------------------------------------------------------
FvAnimationSetupResource::~FvAnimationSetupResource()
{
	unload();
}
//----------------------------------------------------------------------------
void FvAnimationSetupResource::prepareImpl()
{
	bool bFileExist = ResourceGroupManager::getSingleton().resourceExists(
		mGroup,mName);
	if(bFileExist)
	{
		m_spFreshFromDisk =
			ResourceGroupManager::getSingleton().openResource(
			mName, mGroup, true, this);

		m_spFreshFromDisk = DataStreamPtr(OGRE_NEW MemoryDataStream(
			mName,m_spFreshFromDisk));
	}
}
//----------------------------------------------------------------------------
void FvAnimationSetupResource::unprepareImpl()
{
	m_spFreshFromDisk.setNull();
}
//----------------------------------------------------------------------------
void FvAnimationSetupResource::loadImpl()
{
	if(m_spFreshFromDisk.isNull())
	{
		FV_CRITICAL_MSG("FvAvatarResource::loadImpl file %s not exist!\n",mName.c_str());
		return;
	}

	FvXMLSectionPtr spAnimationSetupSec =
		FvXMLSection::OpenSection(m_spFreshFromDisk);
	FV_ASSERT(spAnimationSetupSec);

	m_u32PartitionNumber = spAnimationSetupSec->ReadInt("PartitionNumber", 1);

	std::vector<FvXMLSectionPtr> kLevels;
	spAnimationSetupSec->OpenSections("Level", kLevels);

	m_pkLevelArray = new Level[kLevels.size()];

	for(FvUInt32 i(0); i < kLevels.size(); ++i)
	{
		FvXMLSectionPtr& spSection = kLevels[i];
		Level& kLevel = m_pkLevelArray[i];

		kLevel.m_kName = spSection->AsAttributeString("name");
		FV_ASSERT(kLevel.m_kName != "");

		m_kLevelMap[kLevel.m_kName] = i;

		FvUInt32 u32DefaultPriority = spSection->AsAttributeInt("DefaultPriority");

		kLevel.m_pu32DefaultPriorities = new FvUInt32[m_u32PartitionNumber];
		for(FvUInt32 j(0); j < m_u32PartitionNumber; ++j)
		{
			kLevel.m_pu32DefaultPriorities[j] = u32DefaultPriority;
		}

		std::vector<FvXMLSectionPtr> kDefaultPriorities;
		spSection->OpenSections("Priority", kDefaultPriorities);

		for(FvUInt32 j(0); j < kDefaultPriorities.size(); ++j)
		{
			kLevel.m_pu32DefaultPriorities[kDefaultPriorities[j]->AsAttributeInt("id")] =
				kDefaultPriorities[j]->AsInt(u32DefaultPriority);
		}

		std::vector<FvXMLSectionPtr> kAnimations;
		spSection->OpenSections("Animation", kAnimations);

		kLevel.m_apkAnimations = new LevelAnimation[kAnimations.size()];
		kLevel.m_u32AnimationNumber = kAnimations.size();

		for(FvUInt32 j(0); j < kAnimations.size(); ++j)
		{
			FvXMLSectionPtr& spAniSection = kAnimations[j];
			LevelAnimation& kLevelAni = kLevel.m_apkAnimations[j];

			kLevelAni.m_kName = spAniSection->AsAttributeString("name");
			kLevelAni.m_u32Level = i;

			if(m_kAnimationMap.find(kLevelAni.m_kName) != m_kAnimationMap.end())
			{
				FV_CRITICAL_MSG("AnimationName: %s has defined\n", kLevelAni.m_kName.c_str());
			}
			//FV_ASSERT(m_kAnimationMap.find(kLevelAni.m_kName) == m_kAnimationMap.end());
			m_kAnimationMap[kLevelAni.m_kName] = &kLevelAni;
			
			kLevelAni.m_bLoop = spAniSection->AsAttributeBool("loop", false);
			kLevelAni.m_bManual = spAniSection->AsAttributeBool("manual", false);
			kLevelAni.m_bContinuous = spAniSection->AsAttributeBool("continuous", false);
			kLevelAni.m_kBlendMask = spAniSection->AsAttributeString("blendmask");
			kLevelAni.m_u32Binding = spAniSection->AsAttributeInt("binding");
			kLevelAni.m_pu32Priorities = new FvUInt32[m_u32PartitionNumber];

			FvCrazyCopyEx(kLevelAni.m_pu32Priorities, kLevel.m_pu32DefaultPriorities, sizeof(FvUInt32), m_u32PartitionNumber);

			std::vector<FvXMLSectionPtr> kPriorities;
			spAniSection->OpenSections("Priority", kPriorities);

			for(FvUInt32 k(0); k < kPriorities.size(); ++k)
			{
				kLevelAni.m_pu32Priorities[kPriorities[k]->AsAttributeInt("id")] =
					kPriorities[k]->AsInt(u32DefaultPriority);
			}
		}
	}

	FV_ASSERT(m_kLevelMap.size() == kLevels.size());

	std::vector<FvXMLSectionPtr> kExtraAnimations;
	spAnimationSetupSec->OpenSections("ExtraAnimation", kExtraAnimations);

	m_pkExtraAnimations = new Animation[kExtraAnimations.size()];
	m_u32ExtraAnimationNumber = kExtraAnimations.size();

	for(FvUInt32 i(0); i < m_u32ExtraAnimationNumber; ++i)
	{
		FvXMLSectionPtr& spExtraSection = kExtraAnimations[i];
		Animation& kExtraAni = m_pkExtraAnimations[i];

		kExtraAni.m_kName = spExtraSection->AsString();
		kExtraAni.m_u32Level = m_kLevelMap.size();
		
		FV_ASSERT(m_kAnimationMap.find(kExtraAni.m_kName) == m_kAnimationMap.end());
		m_kAnimationMap[kExtraAni.m_kName] = &kExtraAni;

		kExtraAni.m_bLoop = spExtraSection->AsAttributeBool("loop", false);
		kExtraAni.m_bManual = spExtraSection->AsAttributeBool("manual", false);
		kExtraAni.m_kBlendMask = spExtraSection->AsAttributeString("blendmask");
	}

	std::vector<FvXMLSectionPtr> kGroups;
	spAnimationSetupSec->OpenSections("Group", kGroups);

	m_pkGroups = new Group[kGroups.size()];

	for(FvUInt32 i(0); i < kGroups.size(); ++i)
	{
		FvXMLSectionPtr& spGroupSection = kGroups[i];
		Group& kGroup = m_pkGroups[i];

		kGroup.m_kName = spGroupSection->AsAttributeString("name");
		m_kGroupMap[kGroup.m_kName] = i;

		std::vector<FvXMLSectionPtr> kGroupAnimations;
		spGroupSection->OpenSections("Animation", kGroupAnimations);

		kGroup.m_pkAnimationNames = new String[kGroupAnimations.size()];
		kGroup.m_u32AnimationNumber = kGroupAnimations.size();

		for(FvUInt32 j(0); j < kGroupAnimations.size(); ++j)
		{
			FvXMLSectionPtr& spGroupAnimation = kGroupAnimations[j];
			kGroup.m_pkAnimationNames[j] = spGroupAnimation->AsAttributeString("name");
		}
	}

	FV_ASSERT(m_kGroupMap.size() == kGroups.size());
}
//----------------------------------------------------------------------------
void FvAnimationSetupResource::unloadImpl()
{
	m_u32PartitionNumber = 0;
	FV_SAFE_DELETE_ARRAY(m_pkLevelArray);
	m_kLevelMap.clear();
	FV_SAFE_DELETE_ARRAY(m_pkExtraAnimations);
	m_u32ExtraAnimationNumber = 0;
	m_kAnimationMap.clear();
	FV_SAFE_DELETE_ARRAY(m_pkGroups);
	m_kGroupMap.clear();

}
//----------------------------------------------------------------------------
size_t FvAnimationSetupResource::calculateSize() const
{
	return m_spFreshFromDisk.isNull() ? 0:m_spFreshFromDisk->size();
}
//----------------------------------------------------------------------------
FvAnimationSetupResourcePtr::FvAnimationSetupResourcePtr(
	const Ogre::ResourcePtr& r)
{
	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
	{
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<FvAnimationSetupResource*>(r.getPointer());
		pUseCount = r.useCountPointer();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}
}
//----------------------------------------------------------------------------
FvAnimationSetupResourcePtr& FvAnimationSetupResourcePtr::operator=(
	const Ogre::ResourcePtr& r)
{
	if (pRep == static_cast<FvAnimationSetupResource*>(r.getPointer()))
		return *this;
	release();
	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
	{
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<FvAnimationSetupResource*>(r.getPointer());
		pUseCount = r.useCountPointer();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}
	else
	{
		assert(r.isNull() && "RHS must be null if it has no mutex!");
		setNull();
	}
	return *this;
}
//----------------------------------------------------------------------------
void FvAnimationSetupResourcePtr::destroy()
{
	SharedPtr<FvAnimationSetupResource>::destroy();
}
//----------------------------------------------------------------------------
FvAnimationSetupResourceManager::FvAnimationSetupResourceManager()
{
	mLoadOrder = 350.0f;
	mResourceType = "AnimationSetup";

	ResourceGroupManager::getSingleton()._registerResourceManager(
		mResourceType, this);
}
//----------------------------------------------------------------------------
FvAnimationSetupResourceManager::~FvAnimationSetupResourceManager()
{
	ResourceGroupManager::getSingleton()._unregisterResourceManager(
		mResourceType);
}
//----------------------------------------------------------------------------
FvAnimationSetupResourceManager* FvAnimationSetupResourceManager::Create(
	const char* pcGroupName)
{
	ms_pcGroupName = pcGroupName;
	return OGRE_NEW FvAnimationSetupResourceManager;
}
//----------------------------------------------------------------------------
void FvAnimationSetupResourceManager::Destory()
{
	OGRE_DELETE ms_Singleton;
	ms_Singleton = NULL;
}
//----------------------------------------------------------------------------
FvAnimationSetupResourceManager& FvAnimationSetupResourceManager::getSingleton()
{
	FV_ASSERT(ms_Singleton);
	return (*ms_Singleton);
}
//----------------------------------------------------------------------------
FvAnimationSetupResourceManager* FvAnimationSetupResourceManager::getSingletonPtr()
{
	return ms_Singleton;
}
//----------------------------------------------------------------------------
const char* FvAnimationSetupResourceManager::GetGroupName()
{
	FV_ASSERT(ms_pcGroupName);
	return ms_pcGroupName;
}
//----------------------------------------------------------------------------
Ogre::Resource* FvAnimationSetupResourceManager::createImpl(
	const Ogre::String &kName, Ogre::ResourceHandle uiHandle,
	const Ogre::String &kGroup, bool bIsManual,
	Ogre::ManualResourceLoader *pkLoader,
	const Ogre::NameValuePairList *pkCreateParams)
{
	return OGRE_NEW FvAnimationSetupResource(this, kName, uiHandle, kGroup,
		bIsManual, pkLoader);
}
//----------------------------------------------------------------------------
