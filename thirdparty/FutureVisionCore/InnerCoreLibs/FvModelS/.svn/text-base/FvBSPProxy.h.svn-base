//{future header message}
#ifndef __FvBSPProxy_H__
#define __FvBSPProxy_H__

#include <OgreResource.h>
#include <OgreResourceManager.h>

#include <FvBSP.h>

#include "FvModelDefines.h"

using namespace Ogre;

class FV_MODEL_API FvBSPProxy : public Resource
{
public:
	FvBSPProxy(ResourceManager *pkCreator, const FvString &kName, ResourceHandle uiHandle,
		const FvString &kGroup, bool bIsManual = false, ManualResourceLoader *pkLoader = 0);
	~FvBSPProxy();

	void loadImpl(void);

	void unloadImpl(void);

	void prepareImpl(void);

	void unprepareImpl(void);

	size_t calculateSize(void) const;

	operator FvBSPTree *();

protected:

    DataStreamPtr m_spFreshFromDisk;
	FvBSPTree *m_pkBSPTree;
};    

class FV_MODEL_API FvBSPProxyPtr : public SharedPtr<FvBSPProxy> 
{
public:
	FvBSPProxyPtr() : SharedPtr<FvBSPProxy>() {}
	explicit FvBSPProxyPtr(FvBSPProxy* rep) : SharedPtr<FvBSPProxy>(rep) {}
	FvBSPProxyPtr(const FvBSPProxyPtr& r) : SharedPtr<FvBSPProxy>(r) {} 
	FvBSPProxyPtr(const ResourcePtr& r);
	FvBSPProxyPtr& operator=(const ResourcePtr& r);
protected:
	void destroy(void);
};

class FV_MODEL_API FvBSPProxyManager : public ResourceManager, public Singleton<FvBSPProxyManager>
{
public:
	FvBSPProxyManager();
	~FvBSPProxyManager();

	static FvBSPProxyManager *Create();
	static void Destory();

	static FvBSPProxyManager& getSingleton(void);
	static FvBSPProxyManager* getSingletonPtr(void);

protected:

	Resource* createImpl(const String& name, ResourceHandle handle, 
		const String& group, bool isManual, ManualResourceLoader* loader, 
		const NameValuePairList* createParams);
};

#include "FvBSPProxy.inl"

#endif // __FvBSPProxy_H__
