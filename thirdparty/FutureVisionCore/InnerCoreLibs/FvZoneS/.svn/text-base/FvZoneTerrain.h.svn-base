//{future header message}
#ifndef __FvZoneTerrain_H__
#define __FvZoneTerrain_H__

#include "FvZoneItem.h"
#include "FvZone.h"
#include "FvZoneSerializerXMLImpl.h"

#include <FvTerrainPage.h>

class FV_ZONE_API FvZoneTerrain : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneTerrain )	
public:
	FvZoneTerrain();
	~FvZoneTerrain();

	virtual void Toss( FvZone *pkZone );
	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );
	void VisitTerrain(FvCamera *pkCamera, FvZoneVisitor *pkVisitor);

	virtual FvUInt32 TypeFlags() const;

	FvTerrainPagePtr TerrainPage()				{ return m_spTerrainPage; }
	const FvTerrainPagePtr TerrainPage() const	{ return m_spTerrainPage; }
	const FvBoundingBox &BoundingBox() const	{ return m_kBoundingBox; }

	void CalculateBB();

protected:

	friend class FvZoneTerrainCache;

	bool Load( FvXMLSectionPtr spSection, FvZone *pkZone, FvString *kErrorString = NULL );
	virtual bool AddZBounds( FvBoundingBox &kBB ) const;

	FvZoneTerrain( const FvZoneTerrain& );
	FvZoneTerrain& operator=( const FvZoneTerrain& );

	FvTerrainPagePtr m_spTerrainPage;
	FvBoundingBox m_kBoundingBox;
};

class FvZoneTerrainObstacle;

class FvZoneTerrainCache : public FvZoneCache
{
public:
	FvZoneTerrainCache( FvZone &kZone );
	~FvZoneTerrainCache();

	virtual int Focus();

	void ZoneTerrain( FvZoneTerrain *pkTerrain );

	FvZoneTerrain *ZoneTerrain()				{ return m_pkZoneTerrain; }
	const FvZoneTerrain *ZoneTerrain() const	{ return m_pkZoneTerrain; }

	static Instance<FvZoneTerrainCache>	ms_kInstance;

private:
	FvZone *m_pkZone;
	FvZoneTerrain *m_pkZoneTerrain;
	FvSmartPointer<FvZoneTerrainObstacle> m_spObstacle;
};

#endif // __FvZoneTerrain_H__