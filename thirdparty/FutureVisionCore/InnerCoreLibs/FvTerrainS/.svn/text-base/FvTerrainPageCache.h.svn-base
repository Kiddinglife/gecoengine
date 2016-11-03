//{future header message}
#ifndef __FvTerrainPageCache_H__
#define __FvTerrainPageCache_H__

#include <FvBaseTypes.h>
#include <FvConcurrency.h>
#include <FvSmartPointer.h>

#include "FvTerrain.h"
#include "FvTerrainSettings.h"


class FvTerrainPage;
typedef FvSmartPointer<FvTerrainPage> FvTerrainPagePtr;

class FV_TERRAIN_API FvTerrainPageCacheEntry : public FvReferenceCount
{
	FvTerrainPagePtr m_spPage;

public:
	FvTerrainPageCacheEntry( FvTerrainPagePtr spPage ) :
		m_spPage( spPage )
	{}

	void IncRef() const;
	void DecRef() const;

	FvTerrainPagePtr GetPage() { return m_spPage; }
};
typedef FvSmartPointer<FvTerrainPageCacheEntry> FvTerrainPageCacheEntryPtr;

class FV_TERRAIN_API FvTerrainPageCache
{
public:
	static FvTerrainPageCache &Instance();

	FvTerrainPageCacheEntryPtr FindOrLoad( const FvString &kResourceID, 
		const FvString &kGroup,
		FvTerrainSettingsPtr spSettings );

	void IncRefCacheEntry( const FvTerrainPageCacheEntry &kEntry );
	void DecRefCacheEntry( const FvTerrainPageCacheEntry &kEntry );

private:
	FvTerrainPageCacheEntry *Add( FvTerrainPagePtr spBlock );
	FvTerrainPageCacheEntry *Find( const FvString &kResourceID );

	typedef std::map< FvString, FvTerrainPageCacheEntry* > Map;
	Map m_kMap;

	mutable FvSimpleMutex m_kLock;
};


#endif // __FvTerrainPageCache_H__
