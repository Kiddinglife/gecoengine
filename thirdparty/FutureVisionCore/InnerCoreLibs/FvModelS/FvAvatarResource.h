//{future header message}
#ifndef __FvAvatarResource_H__
#define __FvAvatarResource_H__

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

#include "FvModelDefines.h"
//
//using namespace Ogre;
//
//class FV_MODEL_API FvAvatarResource : public Resource
//{
//public:
//	FvAvatarResource(ResourceManager *pkCreator,
//		const FvString &kName, ResourceHandle uiHandle,
//		const FvString &kGroup, bool bIsManual = false,
//		ManualResourceLoader *pkLoader = 0);
//	~FvAvatarResource();
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
//	FvUInt32 GetBoneNumber(FvUInt32 u32Binding);
//
//	void ManualPartition(FvUInt32 u32Number, float** ppfPartition, const char** pcNames);
//
//	bool HasAliasName(const FvString& kName);
//	const FvString& GetAliasName(const FvString& kName) const;
//
//	const std::pair<Ogre::Vector3,Ogre::Quaternion>& GetShotCamera(FvUInt32 uiIndex) const;
//
//protected:
//	friend class FvAvatar;
//	friend class FvAvatarSkeleton;
//
//	DataStreamPtr m_spFreshFromDisk;
//
//	struct AnimationKey
//	{
//		FvString m_kAnimationName;
//		FvString m_kKeyName;
//		float m_fTime;
//	};
//
//	typedef std::map<std::pair<FvInt32, FvInt32>, float> BlendMap;
//	typedef FvStringMap<float> KeyMap;
//	typedef std::list<AnimationKey> KeyList;
//	typedef std::map<FvString,FvString> AliasName;
//	typedef std::vector<std::pair<Ogre::Vector3,Ogre::Quaternion>> ShotCamera;
//
//	struct SkeletonInfo
//	{
//		MeshPtr m_spMainMesh;
//		std::vector<MaterialPtr> m_kMainMaterials;
//		SkeletonPtr m_spSkeleton;
//		float* m_pfPartitionBuffer;
//		FvUInt32 m_u32BoneNumber;
//		FvUInt32 m_u32PartitonNumber;
//		FvStringMap<FvUInt32> m_kBoneNameHandleMap;
//		bool m_bShowMainMesh;
//	} m_kSkeletonInfo[FV_AVATAR_MAX_BINDING_NUMBER];
//	FvUInt32 m_u32SkeletonNumber;
//
//	FvString m_akPartitionNames[FV_AVATAR_MAX_PARTITION];
//	FvStringMap<FvUInt32> m_kPartitionNames;
//	
//	FvStringMap<FvUInt32> m_kAnimationNames;
//	FvString* m_pkActiveAnimations;
//	KeyMap* m_pkAnimationKey;
//	bool* m_pbAnimationsLoop;
//	FvUInt32 m_u32kActiveAnimationNumber;
//	
//	BlendMap m_kPartitionBlendMap;
//	BlendMap m_kAnimationBlendMap;
//
//	FvUInt32 m_u32PartNumber;
//
//	AliasName m_kAliasName;
//	ShotCamera m_kShotCamera;
//
//	struct BoundingBoxParams
//	{
//		FvString m_kTailPointName;
//		FvBoundingBox m_kBaseBoundingBox;
//	};
//
//	BoundingBoxParams* m_pkBoundingBoxParams;
//};
//
//class FV_MODEL_API FvAvatarResourcePtr : public SharedPtr<FvAvatarResource> 
//{
//public:
//	FvAvatarResourcePtr() : SharedPtr<FvAvatarResource>() {}
//	explicit FvAvatarResourcePtr(FvAvatarResource* rep) : SharedPtr<FvAvatarResource>(rep) {}
//	FvAvatarResourcePtr(const FvAvatarResourcePtr& r) : SharedPtr<FvAvatarResource>(r) {} 
//	FvAvatarResourcePtr(const ResourcePtr& r);
//	FvAvatarResourcePtr& operator=(const ResourcePtr& r);
//protected:
//	void destroy();
//};
//
//class FV_MODEL_API FvAvatarResourceManager : public ResourceManager, public Singleton<FvAvatarResourceManager>
//{
//public:
//	FvAvatarResourceManager();
//	~FvAvatarResourceManager();
//
//	static FvAvatarResourceManager *Create(const char* pcGroupName);
//	static void Destory();
//
//	static FvAvatarResourceManager& getSingleton();
//	static FvAvatarResourceManager* getSingletonPtr();
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

#endif // __FvAvatarResource_H__
