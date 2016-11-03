//#include "FvAnimationShellResource.h"
//#include <FvXMLSection.h>
//
//template<> FvAnimationShellResourceManager* Singleton<FvAnimationShellResourceManager>::ms_Singleton = NULL;
//const char* FvAnimationShellResourceManager::ms_pcGroupName = NULL;
//
////----------------------------------------------------------------------------
//FvAnimationShellResource::FvAnimationShellResource(ResourceManager *pkCreator,
//	const FvString &kName, ResourceHandle uiHandle, const FvString &kGroup,
//	bool bIsManual, ManualResourceLoader *pkLoader)
//	: Resource(pkCreator, kName, uiHandle, kGroup, bIsManual, pkLoader)
//{
//	m_pkMaskedPartitionArray = NULL;
//	m_u32MaskedPartitionNumber = 0;
//	m_pkLevelArray = NULL;
//	m_u32LevelNumber = 0;
//	m_pkExtraAnimations = NULL;
//	m_u32ExtraAnimationNumber = 0;
//	m_pkGroups = NULL;
//	m_u32GroupNumber = 0;
//	m_u32AllMask = 0;
//	m_pkLoopAnimations = NULL;
//	m_u32LoopAnimationNumber = 0;
//	m_pkManualAnimations = NULL;
//	m_u32ManualAnimationNumber = 0;
//}
////----------------------------------------------------------------------------
//FvAnimationShellResource::~FvAnimationShellResource()
//{
//	unload();
//}
////----------------------------------------------------------------------------
//void FvAnimationShellResource::prepareImpl()
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
//void FvAnimationShellResource::unprepareImpl()
//{
//	m_spFreshFromDisk.setNull();
//}
////----------------------------------------------------------------------------
//void FvAnimationShellResource::loadImpl()
//{
//	if(m_spFreshFromDisk.isNull())
//	{
//		FV_CRITICAL_MSG("FvAnimationShellResource::loadImpl file %s not exist!\n",mName.c_str());
//		return;
//	}
//
//	FvXMLSectionPtr spAnimationShellSec =
//		FvXMLSection::OpenSection(m_spFreshFromDisk);
//	FV_ASSERT(spAnimationShellSec);
//
//	std::vector<FvXMLSectionPtr> kHandleAlias;
//	spAnimationShellSec->OpenSections("HandleAlias", kHandleAlias);
//
//	for(FvUInt32 i(0); i < kHandleAlias.size(); ++i)
//	{
//		FvXMLSectionPtr& spSection = kHandleAlias[i];
//		FvString kAlias = spSection->AsAttributeString("name");
//		FvString kRealName = spSection->AsString();
//
//		m_kHandleAliasMap[kAlias] = kRealName;
//	}
//
//	std::vector<FvXMLSectionPtr> kMaskedPartitions;
//	spAnimationShellSec->OpenSections("MaskedPartition", kMaskedPartitions);
//
//	if(kMaskedPartitions.size())
//	{
//		m_pkMaskedPartitionArray = new FvString[kMaskedPartitions.size()];
//		m_u32MaskedPartitionNumber = kMaskedPartitions.size();
//
//		for(FvUInt32 i(0); i < kMaskedPartitions.size(); ++i)
//		{
//			m_pkMaskedPartitionArray[i] = kMaskedPartitions[i]->AsString();
//			FV_ASSERT(m_pkMaskedPartitionArray[i] != "");
//			m_kMaskedPartitionMap.insert(FvStringMap<FvUInt32>::value_type(
//				m_pkMaskedPartitionArray[i].c_str(), i));
//		}
//	}
//	else
//	{
//		m_pkMaskedPartitionArray = new FvString[1];
//		m_u32MaskedPartitionNumber = 1;
//		m_pkMaskedPartitionArray[0] = "Default";
//		
//		m_kMaskedPartitionMap.insert(FvStringMap<FvUInt32>::value_type(
//			m_pkMaskedPartitionArray[0].c_str(), 0));
//	}
//
//	for(FvUInt32 i(0); i < m_u32MaskedPartitionNumber; ++i)
//	{
//		FV_MASK_ADD(m_u32AllMask, FV_MASK(i));
//	}
//
//	std::vector<FvXMLSectionPtr> kLevels;
//	spAnimationShellSec->OpenSections("Level", kLevels);
//
//	if(kLevels.size())
//	{
//		m_pkLevelArray = new Level[kLevels.size()];
//		m_u32LevelNumber = kLevels.size();
//
//		for(FvUInt32 i(0); i < kLevels.size(); ++i)
//		{
//			Level* pkCurrentLevel = m_pkLevelArray + i;
//
//			pkCurrentLevel->m_kName = kLevels[i]->AsAttributeString("name");
//
//			m_kLevelMap.insert(FvStringMap<FvUInt32>::value_type(pkCurrentLevel->m_kName.c_str(), i));
//
//			pkCurrentLevel->m_pu32DefaultPriorities = new FvUInt32[m_u32MaskedPartitionNumber];
//
//			FvInt32 i32DefaultPriority = kLevels[i]->AsAttributeInt("DefaultPriority");
//
//			for(FvUInt32 j(0); j < m_u32MaskedPartitionNumber; ++j)
//			{
//				pkCurrentLevel->m_pu32DefaultPriorities[j] = i32DefaultPriority;
//			}
//			
//			std::vector<FvXMLSectionPtr> kPriorities;
//			kLevels[i]->OpenSections("Priority", kPriorities);
//
//			for(FvUInt32 j(0); j < kPriorities.size(); ++j)
//			{
//				pkCurrentLevel->m_pu32DefaultPriorities[kPriorities[j]->AsAttributeInt("id")] =
//					kPriorities[j]->AsInt(i32DefaultPriority);
//			}
//
//			std::vector<FvXMLSectionPtr> kAnimations;
//			kLevels[i]->OpenSections("Animation", kAnimations);
//
//			FV_ASSERT(kAnimations.size());
//
//			pkCurrentLevel->m_apkAnimations = new LevelAnimation[kAnimations.size()];
//			pkCurrentLevel->m_u32AnimationNumber = kAnimations.size();
//
//			for(FvUInt32 j(0); j < kAnimations.size(); ++j)
//			{
//				pkCurrentLevel->m_apkAnimations[j].m_kName = kAnimations[j]->AsAttributeString("name");
//				
//				std::vector<FvXMLSectionPtr> kAnimationPriorities;
//				kAnimations[j]->OpenSections("Priority", kAnimationPriorities);
//
//				if(kAnimationPriorities.size())
//				{
//					pkCurrentLevel->m_apkAnimations[j].m_pu32Priorities = new FvUInt32[m_u32MaskedPartitionNumber];
//					FvCrazyCopyEx(pkCurrentLevel->m_apkAnimations[j].m_pu32Priorities,
//						pkCurrentLevel->m_pu32DefaultPriorities, sizeof(FvUInt32), m_u32MaskedPartitionNumber);
//
//					for(FvUInt32 k(0); k < kAnimationPriorities.size(); ++k)
//					{
//						pkCurrentLevel->m_apkAnimations[j].m_pu32Priorities[kAnimationPriorities[k]->AsAttributeInt("id")] = kAnimationPriorities[k]->AsInt();
//					}
//				}
//
//				Animation kAnimation = {i, &(pkCurrentLevel->m_apkAnimations[j])};
//
//				m_kAnimationMap.insert(FvStringMap<Animation>::value_type(
//					pkCurrentLevel->m_apkAnimations[j].m_kName.c_str(), kAnimation));
//			}
//
//		}
//	}
//
//	std::vector<FvXMLSectionPtr> kBindingAnimations;
//	spAnimationShellSec->OpenSections("Binding", kBindingAnimations);
//
//	if(kBindingAnimations.size())
//	{
//		for(FvUInt32 i(0); i < kBindingAnimations.size(); ++i)
//		{
//			std::vector<FvXMLSectionPtr> kAnimationNames;
//			kBindingAnimations[i]->OpenSections("Animation", kAnimationNames);
//
//			for(FvUInt32 j(0); j < kAnimationNames.size(); ++j)
//			{
//				FvString kName = kAnimationNames[j]->AsAttributeString("name");
//				m_kBindingMap[kName] = i;
//			}
//		}
//	}
//
//	std::vector<FvXMLSectionPtr> kExtraAnimations;
//	spAnimationShellSec->OpenSections("ExtraAnimation", kExtraAnimations);
//
//	if(kExtraAnimations.size())
//	{
//		m_pkExtraAnimations = new ExtraAnimation[kExtraAnimations.size()];
//		m_u32ExtraAnimationNumber = kExtraAnimations.size();
//
//		for(FvUInt32 i(0); i < kExtraAnimations.size(); ++i)
//		{
//			m_pkExtraAnimations[i].m_kName = kExtraAnimations[i]->AsString();
//			m_pkExtraAnimations[i].m_kPartition = kExtraAnimations[i]->AsAttributeString("partition");
//
//			Animation kAnimation = {m_u32LevelNumber, &(m_pkExtraAnimations[i])};
//
//			m_kAnimationMap.insert(FvStringMap<Animation>::value_type(
//				m_pkExtraAnimations[i].m_kName.c_str(), kAnimation));
//		}
//	}
//
//	std::vector<FvXMLSectionPtr> kGroups;
//	spAnimationShellSec->OpenSections("Group", kGroups);
//
//	if(kGroups.size())
//	{
//		m_pkGroups = new Group[kGroups.size()];
//		m_u32GroupNumber = kGroups.size();
//
//		for(FvUInt32 i(0); i < kGroups.size(); ++i)
//		{
//			m_pkGroups[i].m_kName = kGroups[i]->AsAttributeString("name");
//
//			m_kGroupMap.insert(FvStringMap<FvUInt32>::value_type(
//				m_pkGroups[i].m_kName.c_str(), i));
//		}
//
//		for(FvUInt32 i(0); i < kGroups.size(); ++i)
//		{
//			{
//				std::vector<FvXMLSectionPtr> kAnimations;
//				kGroups[i]->OpenSections("Animation", kAnimations);
//
//				if(kAnimations.size())
//				{
//					m_pkGroups[i].m_pkAnimationNames = new FvString[kAnimations.size()];
//					m_pkGroups[i].m_u32AnimationNumber = kAnimations.size();
//
//					for(FvUInt32 j(0); j < kAnimations.size(); ++j)
//					{
//						m_pkGroups[i].m_pkAnimationNames[j] = kAnimations[j]->AsAttributeString("name");
//
//						m_kAnimationGroupMap[m_pkGroups[i].m_pkAnimationNames[j]].push_back(i);
//					}
//				}
//			}
//			
//			{
//				std::vector<FvXMLSectionPtr> kSubGroups;
//				kGroups[i]->OpenSections("Group", kSubGroups);
//
//				if(kSubGroups.size())
//				{
//					m_pkGroups[i].m_ppkSubGroups = new Group*[kSubGroups.size()];
//					m_pkGroups[i].m_u32SubGroupNumber = kSubGroups.size();
//
//					for(FvUInt32 j(0); j < kSubGroups.size(); ++j)
//					{
//						FvString kName = kSubGroups[j]->AsAttributeString("name");
//
//						FvStringMap<FvUInt32>::iterator it = m_kGroupMap.find(kName);
//
//						FV_ASSERT(it != m_kGroupMap.end());
//
//						m_pkGroups[i].m_ppkSubGroups[j] = &(m_pkGroups[it->second]);
//
//						m_kGroupGroupMap[it->second].push_back(i);
//					}
//				}
//			}
//			
//		}
//	}
//
//	{
//		std::vector<FvXMLSectionPtr> kLoopAnimations;
//		spAnimationShellSec->OpenSections("LoopAnimation", kLoopAnimations);
//
//		if(kLoopAnimations.size())
//		{
//			m_pkLoopAnimations = new FvString[kLoopAnimations.size()];
//			m_u32LoopAnimationNumber = kLoopAnimations.size();
//
//			for(FvUInt32 i(0); i < kLoopAnimations.size(); ++i)
//			{
//				m_pkLoopAnimations[i] = kLoopAnimations[i]->AsAttributeString("name");
//			}
//		}
//	}
//	
//	{
//		std::vector<FvXMLSectionPtr> kManualAnimations;
//		spAnimationShellSec->OpenSections("ManualAnimation", kManualAnimations);
//
//		if(kManualAnimations.size())
//		{
//			m_pkManualAnimations = new FvString[kManualAnimations.size()];
//			m_u32ManualAnimationNumber = kManualAnimations.size();
//
//			for(FvUInt32 i(0); i < kManualAnimations.size(); ++i)
//			{
//				m_pkManualAnimations[i] = kManualAnimations[i]->AsAttributeString("name");
//			}
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAnimationShellResource::unloadImpl()
//{
//	FV_SAFE_DELETE_ARRAY(m_pkMaskedPartitionArray);
//	FV_SAFE_DELETE_ARRAY(m_pkLevelArray);
//	FV_SAFE_DELETE_ARRAY(m_pkExtraAnimations);
//	FV_SAFE_DELETE_ARRAY(m_pkGroups);
//	FV_SAFE_DELETE_ARRAY(m_pkLoopAnimations);
//	FV_SAFE_DELETE_ARRAY(m_pkManualAnimations);
//	
//	m_kMaskedPartitionMap.clear();
//	m_kLevelMap.clear();
//	m_kAnimationMap.clear();
//	m_kGroupMap.clear();
//	m_kAnimationGroupMap.clear();
//	m_kGroupGroupMap.clear();
//	m_kBindingMap.clear();
//
//	m_u32MaskedPartitionNumber = 0;
//	m_u32LevelNumber = 0;
//	m_u32ExtraAnimationNumber = 0;
//	m_u32GroupNumber = 0;
//	m_u32LoopAnimationNumber = 0;
//	m_u32ManualAnimationNumber = 0;
//}
////----------------------------------------------------------------------------
//size_t FvAnimationShellResource::calculateSize() const
//{
//	return m_spFreshFromDisk.isNull() ? 0 : m_spFreshFromDisk->size();
//}
////----------------------------------------------------------------------------
//FvAnimationShellResource::Animation
//FvAnimationShellResource::GetAnimation(const FvString& kName)
//{
//	FvStringMap<Animation>::iterator it = m_kAnimationMap.find(kName);
//	FV_ASSERT(it != m_kAnimationMap.end());
//	return it->second;
//}
////----------------------------------------------------------------------------
//FvAnimationShellResourcePtr::FvAnimationShellResourcePtr(const ResourcePtr& r) :
//SharedPtr<FvAnimationShellResource>()
//{
//	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
//	{
//		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
//			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
//			pRep = static_cast<FvAnimationShellResource*>(r.getPointer());
//		pUseCount = r.useCountPointer();
//		if (pUseCount)
//		{
//			++(*pUseCount);
//		}
//	}
//}
////----------------------------------------------------------------------------
//FvAnimationShellResourcePtr& FvAnimationShellResourcePtr::operator=(
//	const ResourcePtr& r)
//{
//	if (pRep == static_cast<FvAnimationShellResource*>(r.getPointer()))
//		return *this;
//	release();
//	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
//	{
//		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
//			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
//			pRep = static_cast<FvAnimationShellResource*>(r.getPointer());
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
//void FvAnimationShellResourcePtr::destroy()
//{
//	Ogre::SharedPtr<FvAnimationShellResource>::destroy();
//}
////----------------------------------------------------------------------------
//FvAnimationShellResourceManager::FvAnimationShellResourceManager()
//{
//	mLoadOrder = 350.0f;
//	mResourceType = "AnimationShellResource";
//
//	ResourceGroupManager::getSingleton()._registerResourceManager(
//		mResourceType, this);
//}
////----------------------------------------------------------------------------
//FvAnimationShellResourceManager::~FvAnimationShellResourceManager()
//{
//	ResourceGroupManager::getSingleton()._unregisterResourceManager(
//		mResourceType);
//}
////----------------------------------------------------------------------------
//FvAnimationShellResourceManager * FvAnimationShellResourceManager::Create(
//	const char* pcGroupName)
//{
//	ms_pcGroupName = pcGroupName;
//	return OGRE_NEW FvAnimationShellResourceManager;
//}
////----------------------------------------------------------------------------
//void FvAnimationShellResourceManager::Destory()
//{
//	OGRE_DELETE ms_Singleton;
//	ms_Singleton = NULL;
//}
////----------------------------------------------------------------------------
//FvAnimationShellResourceManager& FvAnimationShellResourceManager::getSingleton()
//{
//	FV_ASSERT(ms_Singleton);
//	return (*ms_Singleton);
//}
////----------------------------------------------------------------------------
//FvAnimationShellResourceManager* FvAnimationShellResourceManager::getSingletonPtr()
//{
//	return ms_Singleton;
//}
////----------------------------------------------------------------------------
//Resource* FvAnimationShellResourceManager::createImpl(const String &kName,
//	ResourceHandle uiHandle, const String &kGroup, bool bIsManual,
//	ManualResourceLoader *pkLoader, const NameValuePairList *pkCreateParams)
//{
//	return OGRE_NEW FvAnimationShellResource(this, kName, uiHandle, kGroup,
//		bIsManual, pkLoader);
//}
////----------------------------------------------------------------------------
//const char* FvAnimationShellResourceManager::GetGroupName()
//{
//	FV_ASSERT(ms_pcGroupName);
//	return ms_pcGroupName;
//}
////----------------------------------------------------------------------------
