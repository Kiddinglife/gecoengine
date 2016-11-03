#include "FvTerrainPageCache.h"
#include "FvTerrainPage.h"

void FvTerrainPageCacheEntry::IncRef() const
{
	FvTerrainPageCache::Instance().IncRefCacheEntry( *this );
}

void FvTerrainPageCacheEntry::DecRef() const
{
	FvTerrainPageCache::Instance().DecRefCacheEntry( *this );
}

FvTerrainPageCache& FvTerrainPageCache::Instance()
{
	static FvTerrainPageCache s_kSingleton;
	return s_kSingleton;
}

FvTerrainPageCacheEntryPtr FvTerrainPageCache::FindOrLoad(
		const FvString &kResourceID, const FvString &kGroup,
		FvTerrainSettingsPtr spSettings )
{
	FvTerrainPageCacheEntry *pkExisting = this->Find( kResourceID );
	if (pkExisting)
	{
		return pkExisting;
	}

	FvTerrainPagePtr spNewBlock = new FvTerrainPage(spSettings);
	if (spNewBlock->Load(kResourceID,kGroup,FvVector3::ZERO,FvVector3::ZERO))
	{
		return this->Add( spNewBlock );
	}
	return NULL;
}

FvTerrainPageCacheEntry* FvTerrainPageCache::Add( FvTerrainPagePtr spPage )
{
	FvSimpleMutexHolder kSMH( m_kLock );
	FvTerrainPageCacheEntry *pkEntry = new FvTerrainPageCacheEntry( spPage );
	m_kMap[ spPage->ResourceName() ] = pkEntry;
	return pkEntry;
}

FvTerrainPageCacheEntry* FvTerrainPageCache::Find(
		const FvString& resourceID )
{
	FvSimpleMutexHolder kSMH( m_kLock );
	Map::iterator it = m_kMap.find( resourceID );
	return (it != m_kMap.end()) ?  it->second : NULL;
}

void FvTerrainPageCache::IncRefCacheEntry( const FvTerrainPageCacheEntry &kEntry )
{
	FvSimpleMutexHolder kSMH( m_kLock );
	kEntry.FvReferenceCount::IncRef();
}

void FvTerrainPageCache::DecRefCacheEntry( const FvTerrainPageCacheEntry &kEntry )
{
	FvSimpleMutexHolder kSMH( m_kLock );
	if (kEntry.RefCount() == 1)
	{
		m_kMap.erase( const_cast<FvTerrainPageCacheEntry&>( kEntry ).
				GetPage()->ResourceName() );
	}

	kEntry.FvReferenceCount::DecRef();
}
