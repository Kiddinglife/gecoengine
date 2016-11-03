
FV_INLINE
FvBSPProxy::operator FvBSPTree *()
{
	return m_pkBSPTree;
}

FV_INLINE
FvBSPProxyManager* FvBSPProxyManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

FV_INLINE
FvBSPProxyManager& FvBSPProxyManager::getSingleton(void)
{  
	assert( ms_Singleton );  return ( *ms_Singleton );  
}