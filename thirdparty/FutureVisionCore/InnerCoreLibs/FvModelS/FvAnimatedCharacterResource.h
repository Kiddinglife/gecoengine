//{future header message}
#ifndef __FvAnimatedCharacterResource_H__
#define __FvAnimatedCharacterResource_H__

#include <map>
#include <vector>
#include <list>

#include <OgreResource.h>
#include <OgreResourceManager.h>
#include <OgreMesh.h>
#include <OgreSkeleton.h>
#include <OgreMaterial.h>
#include <FvPowerDefines.h>
#include <FvBaseTypes.h>
#include <FvStringMap.h>
#include <FvBoundingBox.h>
#include <FvDebug.h>

#include "FvModelDefines.h"

class FV_MODEL_API FvAnimatedCharacterResource : public Ogre::Resource
{
public:
	typedef std::map<FvInt32, float> BlendMap;
	typedef std::map<std::pair<FvInt32, FvInt32>, float> InterBlendMap;
	typedef std::vector<std::pair<Ogre::Vector4,Ogre::Quaternion>> ShotCamera;

	FvAnimatedCharacterResource(Ogre::ResourceManager *pkCreator,
		const FvString &kName, Ogre::ResourceHandle uiHandle,
		const FvString &kGroup, bool bIsManual = false,
		Ogre::ManualResourceLoader *pkLoader = 0);
	~FvAnimatedCharacterResource();

	void prepareImpl();

	void unprepareImpl();

	void loadImpl();

	void unloadImpl();

	size_t calculateSize() const;

	FvUInt32 GetNumAnimation();

	FvUInt8* GetBonesPartTable(FvUInt32 u32Binding);

	const std::pair<Ogre::Vector4,Ogre::Quaternion>& GetShotCamera(FvUInt32 uiIndex) const;

	struct AnimationKey
	{
		FvString m_kName;
		float m_fTimePos;
	};
	typedef std::vector<AnimationKey> KeySet;

	struct Animation
	{
		FvString m_kName;
		KeySet m_kKeySet;
	};

	struct SkeletonInfo
	{
		Ogre::MeshPtr m_spMainMesh;
		std::vector<Ogre::MaterialPtr> m_kMainMaterials;
		Ogre::SkeletonPtr m_spSkeleton;
		FvUInt32 m_u32BoneNumber;
		FvUInt8* m_pu8PartitionBuffer;
		float* m_pfBlendMaskBuffer;
		FvUInt32 m_u32BlendMaskNumber;
		bool m_bShowMainMesh;
	};

protected:
	friend class FvAnimationState;
	friend class FvAnimatedCharacterBase;
	friend class FvAnimatedCharacterNodePrivateData;
	friend class FvAnimatedEntity;

	Ogre::DataStreamPtr m_spFreshFromDisk;

	std::vector<SkeletonInfo> m_kSkeletonInfo;

	FvStringMap<FvUInt32> m_kBlendMaskIndexes;
	FvString* m_pkBlendMaskNames;

	FvStringMap<FvUInt32> m_kAnimationIndexes;
	Animation* m_pkAnimations;

	BlendMap m_kAnimationBlendMap;
	InterBlendMap m_kAnimationInterBlendMap;

	FvUInt32 m_u32PartNumber;

	typedef std::map<FvString, FvString> AliasName;

	struct Offset
	{
		FvString m_kBoneName;
		Ogre::Quaternion m_kRotation;
		Ogre::Vector3 m_kPosition;
	};

	typedef FvStringMap<Offset> AliasTagPoint;

	AliasTagPoint m_kBoneAlias;
	AliasName m_kAnimationAlias;

	ShotCamera m_kShotCamera;

	struct BoundingBoxParams
	{
		FvString m_kTailPointName;
		FvVector3 m_kMin;
		FvVector3 m_kMax;
	};

	BoundingBoxParams* m_pkBoundingBoxParams;
};

class FV_MODEL_API FvAnimatedCharacterResourcePtr :
	public Ogre::SharedPtr<FvAnimatedCharacterResource> 
{
public:
	FvAnimatedCharacterResourcePtr() : Ogre::SharedPtr<FvAnimatedCharacterResource>() {}
	explicit FvAnimatedCharacterResourcePtr(FvAnimatedCharacterResource* rep) : Ogre::SharedPtr<FvAnimatedCharacterResource>(rep) {}
	FvAnimatedCharacterResourcePtr(const FvAnimatedCharacterResourcePtr& r) : Ogre::SharedPtr<FvAnimatedCharacterResource>(r) {} 
	FvAnimatedCharacterResourcePtr(const Ogre::ResourcePtr& r);
	FvAnimatedCharacterResourcePtr& operator=(const Ogre::ResourcePtr& r);
protected:
	void destroy();
};

class FV_MODEL_API FvAnimatedCharacterResourceManager :
	public Ogre::ResourceManager, public Ogre::Singleton<FvAnimatedCharacterResourceManager>
{
public:
	FvAnimatedCharacterResourceManager();
	~FvAnimatedCharacterResourceManager();

	static FvAnimatedCharacterResourceManager *Create(const char* pcGroupName);
	static void Destory();

	static FvAnimatedCharacterResourceManager& getSingleton();
	static FvAnimatedCharacterResourceManager* getSingletonPtr();

	static const char* GetGroupName();

protected:

	static const char* ms_pcGroupName;

	Ogre::Resource* createImpl(const Ogre::String &kName, Ogre::ResourceHandle uiHandle, 
		const Ogre::String &kGroup, bool bIsManual, Ogre::ManualResourceLoader *pkLoader, 
		const Ogre::NameValuePairList *pkCreateParams);
};

//----------------------------------------------------------------------------

class FV_MODEL_API FvAnimationSetupResource : public Ogre::Resource
{
public:
	struct Animation
	{
		FvString m_kName;
		FvUInt32 m_u32Level;
		bool m_bLoop;
		bool m_bManual;
		bool m_bContinuous;
		FvString m_kBlendMask;
	};

	struct LevelAnimation : Animation
	{
		LevelAnimation() : m_pu32Priorities(NULL) {}
		~LevelAnimation()
		{
			FV_SAFE_DELETE_ARRAY(m_pu32Priorities);
		}

		FvUInt32 m_u32Binding;
		FvUInt32* m_pu32Priorities;
	};

	struct Level
	{
		~Level()
		{
			FV_SAFE_DELETE_ARRAY(m_apkAnimations);
			FV_SAFE_DELETE_ARRAY(m_pu32DefaultPriorities);
		}

		FvString m_kName;
		FvUInt32* m_pu32DefaultPriorities;
		LevelAnimation* m_apkAnimations;
		FvUInt32 m_u32AnimationNumber;
	};

	struct Group
	{
		Group() : m_pkAnimationNames(NULL), m_u32AnimationNumber(0) {}
		~Group()
		{
			FV_SAFE_DELETE_ARRAY(m_pkAnimationNames);
		}
		FvString m_kName;
		FvString* m_pkAnimationNames;
		FvUInt32 m_u32AnimationNumber;
	};


	FvAnimationSetupResource(Ogre::ResourceManager *pkCreator,
		const FvString &kName, Ogre::ResourceHandle uiHandle,
		const FvString &kGroup, bool bIsManual = false,
		Ogre::ManualResourceLoader *pkLoader = 0);
	~FvAnimationSetupResource();

	void prepareImpl();

	void unprepareImpl();

	void loadImpl();

	void unloadImpl();

	size_t calculateSize() const;

protected:
	friend class FvAnimatedCharacter;

	Ogre::DataStreamPtr m_spFreshFromDisk;
	FvUInt32 m_u32PartitionNumber;

	Level* m_pkLevelArray;
	FvStringMap<FvUInt32> m_kLevelMap;
	Animation* m_pkExtraAnimations;
	FvUInt32 m_u32ExtraAnimationNumber;

	FvStringMap<Animation*> m_kAnimationMap;
	Group* m_pkGroups;
	FvStringMap<FvUInt32> m_kGroupMap;

};

class FV_MODEL_API FvAnimationSetupResourcePtr :
	public Ogre::SharedPtr<FvAnimationSetupResource> 
{
public:
	FvAnimationSetupResourcePtr() : Ogre::SharedPtr<FvAnimationSetupResource>() {}
	explicit FvAnimationSetupResourcePtr(FvAnimationSetupResource* rep) : Ogre::SharedPtr<FvAnimationSetupResource>(rep) {}
	FvAnimationSetupResourcePtr(const FvAnimationSetupResourcePtr& r) : Ogre::SharedPtr<FvAnimationSetupResource>(r) {} 
	FvAnimationSetupResourcePtr(const Ogre::ResourcePtr& r);
	FvAnimationSetupResourcePtr& operator=(const Ogre::ResourcePtr& r);
protected:
	void destroy();
};

class FV_MODEL_API FvAnimationSetupResourceManager :
	public Ogre::ResourceManager, public Ogre::Singleton<FvAnimationSetupResourceManager>
{
public:
	FvAnimationSetupResourceManager();
	~FvAnimationSetupResourceManager();

	static FvAnimationSetupResourceManager *Create(const char* pcGroupName);
	static void Destory();

	static FvAnimationSetupResourceManager& getSingleton();
	static FvAnimationSetupResourceManager* getSingletonPtr();

	static const char* GetGroupName();

protected:

	static const char* ms_pcGroupName;

	Ogre::Resource* createImpl(const Ogre::String &kName, Ogre::ResourceHandle uiHandle, 
		const Ogre::String &kGroup, bool bIsManual, Ogre::ManualResourceLoader *pkLoader, 
		const Ogre::NameValuePairList *pkCreateParams);
};

#endif // __FvAnimatedCharacterResource_H__
