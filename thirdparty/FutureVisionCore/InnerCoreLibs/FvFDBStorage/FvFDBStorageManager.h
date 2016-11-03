//{future header message}
#ifndef __FvFDBStorageManager_H__
#define __FvFDBStorageManager_H__

#include <OgreResourceManager.h>
#include <OgreResourceGroupManager.h>

#include "FvFDBStorage.h"
#include "FvFDBStorageDefine.h"

class FV_FDBSTORAGE_API FvFDBStorageManager : public Ogre::ResourceManager, public Ogre::Singleton<FvFDBStorageManager>
{
public:
	FvFDBStorageManager();
	~FvFDBStorageManager();

	static FvFDBStorageManager *Create();
	static void Destory();

	static FvFDBStorageManager& getSingleton(void);
	static FvFDBStorageManager* getSingletonPtr(void);

	FvFDBStoragePtr prepare( const FvString &kName, 
		const FvString &kGroupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	FvFDBStoragePtr load( const FvString &kName, 
		const FvString &kGroupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	FvFDBStoragePtr createManual( const FvString &kName, 
		const FvString &kGroupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::ManualResourceLoader *pkLoader = 0);

protected:

	Ogre::Resource* createImpl(const FvString &kName, Ogre::ResourceHandle uiHandle, 
		const FvString &kGroup, bool bIsManual, Ogre::ManualResourceLoader *pkLoader, 
		const Ogre::NameValuePairList *pkCreateParams);
};

#include "FvFDBStorageManager.inl"

#endif // __FvFDBStorageManager_H__