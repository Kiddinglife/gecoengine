//{future header message}
#ifndef __FvAnimationShellResource_H__
#define __FvAnimationShellResource_H__

#include <map>
#include <vector>
#include <list>

#include <OgreResource.h>
#include <OgreResourceManager.h>
#include <FvPowerDefines.h>
#include <FvBaseTypes.h>
#include <FvStringMap.h>
#include <FvDebug.h>

#include "FvModelDefines.h"
//
//using namespace Ogre;
//
//class FV_MODEL_API FvAnimationShellResource : public Resource
//{
//public:
//	struct Animation
//	{
//		FvUInt32 m_u32Level;
//		void* m_pvPointer;
//	};
//
//	struct LevelAnimation
//	{
//		LevelAnimation() : m_pu32Priorities(NULL) {}
//		~LevelAnimation()
//		{
//			FV_SAFE_DELETE_ARRAY(m_pu32Priorities);
//		}
//
//		FvString m_kName;
//		FvUInt32* m_pu32Priorities;
//	};
//
//	struct ExtraAnimation
//	{
//		FvString m_kName;
//		FvString m_kPartition;
//	};
//
//	struct Level
//	{
//		~Level()
//		{
//			FV_SAFE_DELETE_ARRAY(m_apkAnimations);
//			FV_SAFE_DELETE_ARRAY(m_pu32DefaultPriorities);
//		}
//
//		FvString m_kName;
//		FvUInt32* m_pu32DefaultPriorities;
//		LevelAnimation* m_apkAnimations;
//		FvUInt32 m_u32AnimationNumber;
//	};
//
//	struct Group
//	{
//		Group() : m_pkAnimationNames(NULL), m_ppkSubGroups(NULL),
//			m_u32AnimationNumber(0), m_u32SubGroupNumber(0), m_fScale(1.0f) {}
//		~Group()
//		{
//			FV_SAFE_DELETE_ARRAY(m_pkAnimationNames);
//			FV_SAFE_DELETE_ARRAY(m_ppkSubGroups);
//		}
//		FvString m_kName;
//		FvString* m_pkAnimationNames;
//		FvUInt32 m_u32AnimationNumber;
//		Group** m_ppkSubGroups;
//		FvUInt32 m_u32SubGroupNumber;
//		float m_fScale;
//	};	
//
//	FvAnimationShellResource(ResourceManager *pkCreator,
//		const FvString &kName, ResourceHandle uiHandle,
//		const FvString &kGroup, bool bIsManual = false,
//		ManualResourceLoader *pkLoader = 0);
//	~FvAnimationShellResource();
//
//	void prepareImpl();
//
//	void unprepareImpl();
//
//	void loadImpl();
//
//	void unloadImpl();
//
//	size_t calculateSize() const;
//
//	Animation GetAnimation(const FvString& kName);
//
//protected:
//	friend class FvAnimationShell;
//
//	DataStreamPtr m_spFreshFromDisk;
//	
//	FvString* m_pkMaskedPartitionArray;
//	FvStringMap<FvUInt32> m_kMaskedPartitionMap;
//	FvUInt32 m_u32MaskedPartitionNumber;
//	FvUInt32 m_u32AllMask;
//
//	Level* m_pkLevelArray;
//	FvUInt32 m_u32LevelNumber;
//	FvStringMap<FvUInt32> m_kLevelMap;
//
//
//	ExtraAnimation* m_pkExtraAnimations;
//	FvUInt32 m_u32ExtraAnimationNumber;
//
//	FvStringMap<Animation> m_kAnimationMap;
//
//	Group* m_pkGroups;
//	FvUInt32 m_u32GroupNumber;
//	FvStringMap<FvUInt32> m_kGroupMap;
//	FvStringMap<std::list<FvUInt32>> m_kAnimationGroupMap;
//	std::map<FvUInt32, std::list<FvUInt32>> m_kGroupGroupMap;
//
//	FvStringMap<FvUInt32> m_kBindingMap;
//
//	FvString* m_pkLoopAnimations;
//	FvUInt32 m_u32LoopAnimationNumber;
//	FvString* m_pkManualAnimations;
//	FvUInt32 m_u32ManualAnimationNumber;
//
//	FvStringMap<FvString> m_kHandleAliasMap;
//};
//
//class FV_MODEL_API FvAnimationShellResourcePtr : public SharedPtr<FvAnimationShellResource> 
//{
//public:
//	FvAnimationShellResourcePtr() : SharedPtr<FvAnimationShellResource>() {}
//	explicit FvAnimationShellResourcePtr(FvAnimationShellResource* rep) : SharedPtr<FvAnimationShellResource>(rep) {}
//	FvAnimationShellResourcePtr(const FvAnimationShellResourcePtr& r) : SharedPtr<FvAnimationShellResource>(r) {} 
//	FvAnimationShellResourcePtr(const ResourcePtr& r);
//	FvAnimationShellResourcePtr& operator=(const ResourcePtr& r);
//protected:
//	void destroy();
//};
//
//class FV_MODEL_API FvAnimationShellResourceManager : public ResourceManager, public Singleton<FvAnimationShellResourceManager>
//{
//public:
//	FvAnimationShellResourceManager();
//	~FvAnimationShellResourceManager();
//
//	static FvAnimationShellResourceManager *Create(const char* pcGroupName);
//	static void Destory();
//
//	static FvAnimationShellResourceManager& getSingleton();
//	static FvAnimationShellResourceManager* getSingletonPtr();
//
//	static const char* GetGroupName();
//
//protected:
//
//	static const char* ms_pcGroupName;
//
//	Resource* createImpl(const String &kName, ResourceHandle uiHandle, 
//		const String &kGroup, bool bIsManual, ManualResourceLoader *pkLoader, 
//		const NameValuePairList *pkCreateParams);
//};

#endif // __FvAnimationShellResource_H__
