#include "FvTerrainLodController.h"
#include "FvTerrainPage.h"

#include <FvDebug.h>

FvTerrainLodController FvTerrainLodController::ms_kTerrainLodController;

FvTerrainLodController::FvTerrainLodController()
{

}

FvTerrainLodController::~FvTerrainLodController()
{

}

void FvTerrainLodController::NotifyCameraPosition( const FvVector3& kPos )
{
	TerrainPageMap::iterator kIt = m_kTerrainPageMap.begin();
	for(; kIt != m_kTerrainPageMap.end(); ++kIt)
	{
		TerrainPages::iterator kPageIt = kIt->second.begin();
		for(; kPageIt != kIt->second.end(); ++kPageIt)
		{
			FvTerrainPage *pkPage = kPageIt->second;
			if(pkPage)
			{
				pkPage->NotifyCameraPosition(kPos);
				for(FvUInt32 i = 0; i < pkPage->NumTitles() * pkPage->NumTitles() ; i++)
					pkPage->GetRenderable(i)->NotifyCameraPosition(kPos);
			}
		}
	}
}

void FvTerrainLodController::AddPage( FvTerrainPage *pkPage )
{
	FV_ASSERT(pkPage);

	TerrainRenderIndex kIndex(pkPage->NumTitles(),pkPage->PageWidth());
	FvVector3 kCenter = pkPage->WorldPosition(); 
	FvInt32 iGridX = PointToGrid(kCenter.x);
	FvInt32 iGridY = PointToGrid(kCenter.y);
	TerrainPages &kTerrainPages = m_kTerrainPageMap[kIndex];
	TerrainPages::iterator kIt = kTerrainPages.find(TerrainPageIndex(iGridX,iGridY));
	if(kIt == kTerrainPages.end())
	{
		m_kTerrainPageMap[kIndex].insert(TerrainPages::value_type(
			TerrainPageIndex(iGridX,iGridY),pkPage));

		this->LinkNeighbor(pkPage, kIndex, iGridX, iGridY);
	}
	pkPage->SetInLodControl(true);
}

void FvTerrainLodController::DelPage( FvTerrainPage *pkPage )
{
	FV_ASSERT(pkPage);	
	TerrainRenderIndex kIndex(pkPage->NumTitles(),pkPage->PageWidth());
	FvVector3 kCenter = pkPage->WorldPosition(); 
	FvInt32 iGridX = PointToGrid(kCenter.x);
	FvInt32 iGridY = PointToGrid(kCenter.y);
	TerrainPages &kTerrainPages = m_kTerrainPageMap[kIndex];
	TerrainPages::iterator kIt = kTerrainPages.find(TerrainPageIndex(iGridX,iGridY));
	if(kIt != kTerrainPages.end())
	{
		kTerrainPages.erase(kIt);

		this->UnlinkNeighbor(kIndex, iGridX, iGridY);
	}
	pkPage->SetInLodControl(false);
}

void FvTerrainLodController::LinkNeighbor( FvTerrainPage *pkPage, TerrainRenderIndex kIndex,
										  FvInt32 iGridX, FvInt32 iGridY )
{
	FvUInt32 uiTitleNum = kIndex.first;
	for(FvUInt32 j = 0; j < uiTitleNum; j++)
	{		
		for(FvUInt32 i = 0; i < uiTitleNum; i++)
		{
			if( j != uiTitleNum - 1)
			{				
				FvTerrainRenderable *pkRenderableNorth = pkPage->GetRenderable(i,j);
				FvTerrainRenderable *pkRenderableSouth = pkPage->GetRenderable(i,j + 1);
				pkRenderableSouth->SetNeighbor(FvTerrainRenderable::NORTH,pkRenderableNorth);
				pkRenderableNorth->SetNeighbor(FvTerrainRenderable::SOUTH,pkRenderableSouth);
			}
			if( i != uiTitleNum - 1)
			{
				FvTerrainRenderable *pkRenderableWest = pkPage->GetRenderable(i,j);
				FvTerrainRenderable *pkRenderableEast = pkPage->GetRenderable(i + 1,j);
				pkRenderableEast->SetNeighbor(FvTerrainRenderable::WEST,pkRenderableWest);
				pkRenderableWest->SetNeighbor(FvTerrainRenderable::EAST,pkRenderableEast);
			}
		}
	}

	FvTerrainPage *pkNeiPage = NULL;
	if(pkNeiPage = this->GetPage(kIndex,iGridX - 1,iGridY))
	{
		for(FvUInt32 i = 0; i < uiTitleNum; i++)
		{
			FvTerrainRenderable *pkRenderableWest = pkNeiPage->GetRenderable(pkNeiPage->NumTitles() - 1,i);
			FvTerrainRenderable *pkRenderable = pkPage->GetRenderable(0,i);
			if(pkRenderableWest && pkRenderable)
			{
				pkRenderableWest->SetNeighbor(FvTerrainRenderable::EAST,pkRenderable);
				pkRenderable->SetNeighbor(FvTerrainRenderable::WEST,pkRenderableWest);
			}
		}
	}
	if(pkNeiPage = this->GetPage(kIndex,iGridX + 1,iGridY))
	{
		for(FvUInt32 i = 0; i < uiTitleNum; i++)
		{
			FvTerrainRenderable *pkRenderableEast = pkNeiPage->GetRenderable(0,i);
			FvTerrainRenderable *pkRenderable = pkPage->GetRenderable(pkNeiPage->NumTitles() - 1,i);
			if(pkRenderableEast && pkRenderable)
			{
				pkRenderableEast->SetNeighbor(FvTerrainRenderable::WEST,pkRenderable);
				pkRenderable->SetNeighbor(FvTerrainRenderable::EAST,pkRenderableEast);
			}
		}
	}
	if(pkNeiPage = this->GetPage(kIndex,iGridX,iGridY + 1))
	{
		for(FvUInt32 i = 0; i < uiTitleNum; i++)
		{
			FvTerrainRenderable *pkRenderableSouth = pkNeiPage->GetRenderable(i,0);
			FvTerrainRenderable *pkRenderable = pkPage->GetRenderable(i,pkNeiPage->NumTitles() - 1);
			if(pkRenderableSouth && pkRenderable)
			{
				pkRenderableSouth->SetNeighbor(FvTerrainRenderable::NORTH,pkRenderable);
				pkRenderable->SetNeighbor(FvTerrainRenderable::SOUTH,pkRenderableSouth);
			}
		}
	}
	if(pkNeiPage = this->GetPage(kIndex,iGridX,iGridY - 1))
	{
		for(FvUInt32 i = 0; i < uiTitleNum; i++)
		{
			FvTerrainRenderable *pkRenderableNorth = pkNeiPage->GetRenderable(i,pkNeiPage->NumTitles() - 1);
			FvTerrainRenderable *pkRenderable = pkPage->GetRenderable(i,0);
			if(pkRenderableNorth && pkRenderable)
			{
				pkRenderableNorth->SetNeighbor(FvTerrainRenderable::SOUTH,pkRenderable);
				pkRenderable->SetNeighbor(FvTerrainRenderable::NORTH,pkRenderableNorth);
			}
		}
	}
}

void FvTerrainLodController::UnlinkNeighbor( TerrainRenderIndex kIndex,
											FvInt32 iGridX, FvInt32 iGridY )
{
	FvTerrainPage *pkNeiPage = NULL;
	if(pkNeiPage = this->GetPage(kIndex,iGridX - 1,iGridY))
	{
		for(FvUInt32 i = 0; i < pkNeiPage->NumTitles(); i++)
		{
			FvTerrainRenderable *pkRenderableWest = pkNeiPage->GetRenderable(pkNeiPage->NumTitles() - 1,i);
			if( pkRenderableWest )
				pkRenderableWest->SetNeighbor(FvTerrainRenderable::EAST,NULL);
		}
	}
	if(pkNeiPage = this->GetPage(kIndex,iGridX + 1,iGridY))
	{
		for(FvUInt32 i = 0; i < pkNeiPage->NumTitles(); i++)
		{
			FvTerrainRenderable *pkRenderableEast = pkNeiPage->GetRenderable(0,i);
			if( pkRenderableEast )
				pkRenderableEast->SetNeighbor(FvTerrainRenderable::WEST,NULL);
		}
	}
	if(pkNeiPage = this->GetPage(kIndex,iGridX,iGridY + 1))
	{
		for(FvUInt32 i = 0; i < pkNeiPage->NumTitles(); i++)
		{
			FvTerrainRenderable *pkRenderableSouth = pkNeiPage->GetRenderable(i,0);
			if( pkRenderableSouth )
				pkRenderableSouth->SetNeighbor(FvTerrainRenderable::NORTH,NULL);

		}
	}
	if(pkNeiPage = this->GetPage(kIndex,iGridX,iGridY - 1))
	{
		for(FvUInt32 i = 0; i < pkNeiPage->NumTitles(); i++)
		{
			FvTerrainRenderable *pkRenderableNorth = pkNeiPage->GetRenderable(i,pkNeiPage->NumTitles() - 1);
			if( pkRenderableNorth )
				pkRenderableNorth->SetNeighbor(FvTerrainRenderable::SOUTH,NULL);
		}
	}
}

FvTerrainPage *FvTerrainLodController::GetPage(TerrainRenderIndex kIndex,
					   FvInt32 iGridX, FvInt32 iGridY)
{
	TerrainPages &kTerrainPages = m_kTerrainPageMap[kIndex];
	TerrainPages::iterator kIt = kTerrainPages.find(TerrainPageIndex(iGridX,iGridY));
	if(kIt == kTerrainPages.end())
		return NULL;
	return kIt->second;
}