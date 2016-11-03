#include "FvFDBStorageManager.h"

template<> FvFDBStorageManager* Ogre::Singleton<FvFDBStorageManager>::ms_Singleton = 0;

FvFDBStorageManager::FvFDBStorageManager()
{
	mLoadOrder = 350.0f;
	mResourceType = "FDB";

	Ogre::ResourceGroupManager::getSingleton().
		_registerResourceManager(mResourceType, this);
}

FvFDBStorageManager::~FvFDBStorageManager()
{
	Ogre::ResourceGroupManager::getSingleton().
		_unregisterResourceManager(mResourceType);
}

FvFDBStorageManager *FvFDBStorageManager::Create()
{
	return OGRE_NEW FvFDBStorageManager;
}

void FvFDBStorageManager::Destory()
{
	OGRE_DELETE ms_Singleton;
	ms_Singleton = NULL;
}

FvFDBStoragePtr FvFDBStorageManager::prepare( const FvString &kName, const FvString &kGroupName)
{
	FvFDBStoragePtr spFDBStorage(createOrRetrieve(kName,kGroupName,false,0,0).first);
	spFDBStorage->prepare();
	return spFDBStorage;
}

FvFDBStoragePtr FvFDBStorageManager::load( const FvString &kName, const FvString &kGroupName)
{
	FvFDBStoragePtr spFDBStorage(createOrRetrieve(kName,kGroupName,false,0,0).first);
	spFDBStorage->load();
	return spFDBStorage;
}

FvFDBStoragePtr FvFDBStorageManager::createManual( const FvString &kName, const FvString &kGroupName, 
										Ogre::ManualResourceLoader *pkLoader)
{
	return FvFDBStoragePtr(create(kName,kGroupName,true,pkLoader));
}

Ogre::Resource* FvFDBStorageManager::createImpl(const FvString &kName, Ogre::ResourceHandle uiHandle, 
									   const FvString &kGroup, bool bIsManual, Ogre::ManualResourceLoader *pkLoader, 
									   const Ogre::NameValuePairList *pkCreateParams)
{

	return OGRE_NEW FvFDBStorage(this,kName,uiHandle,kGroup,bIsManual,pkLoader);
}