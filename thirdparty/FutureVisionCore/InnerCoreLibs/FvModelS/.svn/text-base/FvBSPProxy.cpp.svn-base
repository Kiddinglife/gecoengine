#include "FvBSPProxy.h"

FvBSPProxy::FvBSPProxy(ResourceManager *pkCreator, const FvString &kName, ResourceHandle uiHandle,
					   const FvString &kGroup, bool bIsManual, ManualResourceLoader *pkLoader) :
Resource(pkCreator, kName, uiHandle, kGroup, bIsManual, pkLoader),
m_pkBSPTree(NULL)
{

}

FvBSPProxy::~FvBSPProxy()
{
	unload();
}

void FvBSPProxy::prepareImpl()
{
	bool bFileExist = ResourceGroupManager::getSingleton().resourceExists(mGroup,mName);
	if(bFileExist)
	{
		m_spFreshFromDisk =
			ResourceGroupManager::getSingleton().openResource(
			mName, mGroup, true, this);

		m_spFreshFromDisk = DataStreamPtr(OGRE_NEW MemoryDataStream(mName,m_spFreshFromDisk));
	}
}

void FvBSPProxy::unprepareImpl()
{
	m_spFreshFromDisk.setNull();
}

void FvBSPProxy::loadImpl(void)
{
	if(m_pkBSPTree == NULL)
	{
		bool bSuccess = false;
		m_pkBSPTree = new FvBSPTree;
		if(!m_spFreshFromDisk.isNull())
		{
			MemoryDataStream &kMemData = *(MemoryDataStream*)m_spFreshFromDisk.get();
			bSuccess = m_pkBSPTree->Load((char*)kMemData.getPtr(),
				kMemData.size());
			m_spFreshFromDisk.setNull();
		}

		if(!bSuccess)
		{
			delete m_pkBSPTree;
			m_pkBSPTree = NULL;
		}
	}
}

void FvBSPProxy::unloadImpl(void)
{
	delete m_pkBSPTree;
	m_pkBSPTree = NULL;
}

size_t FvBSPProxy::calculateSize(void) const
{
	return m_spFreshFromDisk.isNull() ? 0:m_spFreshFromDisk->size();
}

FvBSPProxyPtr::FvBSPProxyPtr(const ResourcePtr& r) : SharedPtr<FvBSPProxy>()
{
	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
	{
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<FvBSPProxy*>(r.getPointer());
		pUseCount = r.useCountPointer();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}
}

FvBSPProxyPtr& FvBSPProxyPtr::operator=(const ResourcePtr& r)
{
	if (pRep == static_cast<FvBSPProxy*>(r.getPointer()))
		return *this;
	release();
	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
	{
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<FvBSPProxy*>(r.getPointer());
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

void FvBSPProxyPtr::destroy(void)
{
	Ogre::SharedPtr<FvBSPProxy>::destroy();
}


template<> FvBSPProxyManager* Singleton<FvBSPProxyManager>::ms_Singleton = 0;

FvBSPProxyManager::FvBSPProxyManager()
{
	mLoadOrder = 350.0f;
	mResourceType = "BSPProxy";

	ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

FvBSPProxyManager::~FvBSPProxyManager()
{
	ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

FvBSPProxyManager *FvBSPProxyManager::Create()
{
	return OGRE_NEW FvBSPProxyManager;
}

void FvBSPProxyManager::Destory()
{
	OGRE_DELETE ms_Singleton;
	ms_Singleton = NULL;
}

Resource* FvBSPProxyManager::createImpl(const String &kName, ResourceHandle uiHandle, 
								  const String &kGroup, bool bIsManual, ManualResourceLoader *pkLoader, 
								  const NameValuePairList *pkCreateParams)
{
	return OGRE_NEW FvBSPProxy(this, kName, uiHandle, kGroup, bIsManual, pkLoader);
}