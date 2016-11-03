#include "FvAoIGrid.h"



FvAoIGrid::FvAoIGrid()
:m_uiGridSizeBits(0)
,m_iGridInnerMinX(0),m_iGridInnerMinY(0),m_iGridInnerMaxX(0),m_iGridInnerMaxY(0)
,m_iGridIDMinX(0),m_iGridIDMinY(0)
,m_uiGridIDCntX(0),m_uiGridIDCntY(0)
,m_uiCellCnt(0)
,m_pkBorderX(NULL),m_pkBorderY(NULL)
,m_pkCells(NULL)
{

}

FvAoIGrid::~FvAoIGrid()
{
	FV_SAFE_DELETE_ARRAY(m_pkBorderX);
	FV_SAFE_DELETE_ARRAY(m_pkCells);
}

bool FvAoIGrid::Init(FvUInt32 uiGridSizeBits,
				FvInt32 iMinX, FvInt32 iMinY, FvInt32 iMaxX, FvInt32 iMaxY)
{
	if(m_pkBorderX || m_pkCells ||
		iMaxX <= iMinX || iMaxY <= iMinY)
		return false;

	m_uiGridSizeBits = uiGridSizeBits;

	m_iGridIDMinX = iMinX >> uiGridSizeBits;
	m_iGridIDMinY = iMinY >> uiGridSizeBits;
	FvInt32 iGridIDMaxX = iMaxX >> uiGridSizeBits;
	FvInt32 iGridIDMaxY = iMaxY >> uiGridSizeBits;

	if((iGridIDMaxX << uiGridSizeBits) == iMaxX)
		--iGridIDMaxX;
	if((iGridIDMaxY << uiGridSizeBits) == iMaxY)
		--iGridIDMaxY;

	FV_ASSERT(m_iGridIDMinX <= iGridIDMaxX);
	FV_ASSERT(m_iGridIDMinY <= iGridIDMaxY);

	FvInt32 iSize = 0x01 << uiGridSizeBits;
	m_uiGridIDCntX = (FvUInt32)(iGridIDMaxX - m_iGridIDMinX +1);
	m_uiGridIDCntY = (FvUInt32)(iGridIDMaxY - m_iGridIDMinY +1);
	m_pkBorderX = new FvInt32[m_uiGridIDCntX + m_uiGridIDCntY +2];
	m_pkBorderY = m_pkBorderX + m_uiGridIDCntX +1;
	m_uiCellCnt = m_uiGridIDCntX * m_uiGridIDCntY;
	m_pkCells = new FvAoIDLNode[m_uiCellCnt];

	m_iGridInnerMinX = (m_iGridIDMinX << uiGridSizeBits) + iSize;
	m_iGridInnerMinY = (m_iGridIDMinY << uiGridSizeBits) + iSize;
	m_iGridInnerMaxX = (iGridIDMaxX << uiGridSizeBits) -1;
	m_iGridInnerMaxY = (iGridIDMaxY << uiGridSizeBits) -1;

	m_pkBorderX[0] = FVAOI_MIN_POS;
	if(m_uiGridIDCntX > 1)
	{
		FvInt32 iX = m_iGridInnerMinX;
		for(FvUInt32 i=1; i<m_uiGridIDCntX; ++i)
		{
			m_pkBorderX[i] = iX;
			iX += iSize;
		}
	}
	m_pkBorderX[m_uiGridIDCntX] = FVAOI_MAX_POS;

	m_pkBorderY[0] = FVAOI_MIN_POS;
	if(m_uiGridIDCntY > 1)
	{
		FvInt32 iY = m_iGridInnerMinY;
		for(FvUInt32 j=1; j<m_uiGridIDCntY; ++j)
		{
			m_pkBorderY[j] = iY;
			iY += iSize;
		}
	}
	m_pkBorderY[m_uiGridIDCntY] = FVAOI_MAX_POS;

	for(FvUInt32 k=0; k<m_uiCellCnt; ++k)
	{
		FvAoIDLNode& kNode = m_pkCells[k];
		kNode.m_pkPre = kNode.m_pkNex = &kNode;
	}

	return true;
}

void FvAoIGrid::AddObject(FvAoIObject* pkObj, FvInt32 iX, FvInt32 iY)
{
	FV_ASSERT(pkObj);

	FvUInt16 uiIDX(0), uiIDY(0);
	AOI_GRID_MAKE_CELL_ID(iX, uiIDX, X);
	AOI_GRID_MAKE_CELL_ID(iY, uiIDY, Y);

	pkObj->m_uiCellIDX = uiIDX;
	pkObj->m_uiCellIDY = uiIDY;

	FvAoIDLNode& kPre = m_pkCells[uiIDY * m_uiGridIDCntX + uiIDX];
	FvAoIDLNode& kIns = pkObj->m_kCellNode;
	FvAoIDLNode& kNex = *(kPre.m_pkNex);
	kIns.m_pkPre = &kPre;
	kIns.m_pkNex = &kNex;
	kPre.m_pkNex = &kIns;
	kNex.m_pkPre = &kIns;
}

void FvAoIGrid::MoveObject(FvAoIObject* pkObj, FvInt32 iX, FvInt32 iY)
{
	FV_ASSERT(pkObj &&
		pkObj->m_uiCellIDX < m_uiGridIDCntX &&
		pkObj->m_uiCellIDY < m_uiGridIDCntY);

	if(iX < m_pkBorderX[pkObj->m_uiCellIDX])
	{
		RemoveObject(pkObj);
		AddObject(pkObj, iX, iY);
	}
	else if(iX > m_pkBorderX[pkObj->m_uiCellIDX +1])
	{
		RemoveObject(pkObj);
		AddObject(pkObj, iX, iY);
	}
	else if(iY < m_pkBorderY[pkObj->m_uiCellIDY])
	{
		RemoveObject(pkObj);
		AddObject(pkObj, iX, iY);
	}
	else if(iY > m_pkBorderY[pkObj->m_uiCellIDY +1])
	{
		RemoveObject(pkObj);
		AddObject(pkObj, iX, iY);
	}
}

void FvAoIGrid::RemoveObject(FvAoIObject* pkObj)
{
	FV_ASSERT(pkObj &&
		pkObj->m_uiCellIDX < m_uiGridIDCntX &&
		pkObj->m_uiCellIDY < m_uiGridIDCntY);

	FvAoIDLNode& kDrawOut = pkObj->m_kCellNode;
	kDrawOut.m_pkPre->m_pkNex = kDrawOut.m_pkNex;
	kDrawOut.m_pkNex->m_pkPre = kDrawOut.m_pkPre;
}





