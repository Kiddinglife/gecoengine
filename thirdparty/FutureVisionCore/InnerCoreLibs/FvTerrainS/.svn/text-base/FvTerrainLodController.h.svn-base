//{future header message}
#ifndef __FvTerrainLodController_H__
#define __FvTerrainLodController_H__

#include "FvTerrain.h"
#include "FvTerrainPage.h"
#include "FvTerrainRenderable.h"

#include <FvVector3.h>
#include <vector>

class FvTerrainPage;

class FV_TERRAIN_API FvTerrainLodController
{
public:
	FvTerrainLodController();
	~FvTerrainLodController();

	void NotifyCameraPosition( const FvVector3& kPos );

	void AddPage( FvTerrainPage *pkPage );
	void DelPage( FvTerrainPage *pkPage );

	static FvInt32 PointToGrid(float fPoint);

	static FvTerrainLodController &Instance();

private:

	void LinkNeighbor( FvTerrainPage *pkPage, TerrainRenderIndex kIndex,
		FvInt32 iGridX, FvInt32 iGridY );
	void UnlinkNeighbor( TerrainRenderIndex kIndex,
		FvInt32 iGridX, FvInt32 iGridY );

	FvTerrainPage *GetPage(TerrainRenderIndex kIndex,
							FvInt32 iGridX, FvInt32 iGridY);

	typedef std::pair<FvInt32, FvInt32> TerrainPageIndex;
	typedef std::map<TerrainPageIndex,FvTerrainPage*> TerrainPages;
	typedef std::map<TerrainRenderIndex,TerrainPages> TerrainPageMap;
	TerrainPageMap m_kTerrainPageMap;

	static FvTerrainLodController ms_kTerrainLodController;
};

#include "FvTerrainLodController.inl"

#endif // __FvTerrainLodController_H__
