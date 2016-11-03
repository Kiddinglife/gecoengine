//{future header message}
#ifndef __FvServerZoneTerrain_H__
#define __FvServerZoneTerrain_H__

#include "FvZone.h"
#include "FvZoneItem.h"
#include "FvZoneSerializerXMLImpl.h"

#include <FvTerrainPage.h>
#include <FvTerrainPageCache.h>

class FvZoneTerrain : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneTerrain )

public:
	FvZoneTerrain();
	~FvZoneTerrain();

	virtual void Toss( FvZone *pkZone );

	FvTerrainPagePtr GetPage()	{ return m_spCacheEntry->GetPage(); }
	const FvBoundingBox &BB()		{ return m_kBB; }

protected:
	FvZoneTerrain( const FvZoneTerrain& );
	FvZoneTerrain& operator=( const FvZoneTerrain& );

	FvTerrainPageCacheEntryPtr m_spCacheEntry;
	FvBoundingBox m_kBB;

	bool Load( FvXMLSectionPtr spSection, FvZone *pkZone );

	void CalculateBB();

	friend class FvZoneTerrainCache;
};

class FvZoneTerrainObstacle;

class FvZoneTerrainCache : public FvZoneCache
{
public:
	FvZoneTerrainCache( FvZone &kZone );
	~FvZoneTerrainCache();

	virtual int Focus();

	void pTerrain( FvZoneTerrain *pkTerrain );

	FvZoneTerrain *pTerrain() { return m_pkTerrain; }
	const FvZoneTerrain *pTerrain() const { return m_pkTerrain; }

	static Instance<FvZoneTerrainCache>	ms_kInstance;

private:
	FvZone *m_pkZone;
	FvZoneTerrain *m_pkTerrain;
	FvSmartPointer<FvZoneTerrainObstacle> m_spObstacle;
};

#endif // __FvServerZoneTerrain_H__