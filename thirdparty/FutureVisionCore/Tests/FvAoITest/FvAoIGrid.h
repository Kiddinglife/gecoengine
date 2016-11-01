//{future header message}
#ifndef __FvAoIGrid_H__
#define __FvAoIGrid_H__

#include "FvAoIUtility.h"
#include "FvAoIObj.h"


#define AOI_GRID_MAKE_CELL_ID(_POS, _ID, _T)							\
	if(_POS < m_iGridInnerMin##_T)										\
		_ID = 0;														\
	else if(_POS > m_iGridInnerMax##_T)									\
		_ID = FvUInt16(m_uiGridIDCnt##_T -1);							\
	else																\
		_ID = FvUInt16((_POS >> m_uiGridSizeBits) - m_iGridIDMin##_T)

class FvAoIGrid
{
public:
	FvAoIGrid();
	~FvAoIGrid();

	bool		Init(FvUInt32 uiGridSizeBits,
						FvInt32 iMinX, FvInt32 iMinY, FvInt32 iMaxX, FvInt32 iMaxY);

	void		AddObject(FvAoIObject* pkObj, FvInt32 iX, FvInt32 iY);
	void		MoveObject(FvAoIObject* pkObj, FvInt32 iX, FvInt32 iY);
	void		RemoveObject(FvAoIObject* pkObj);

	template<typename TVisiter,
		void (TVisiter::*OnVisit)(void* pkObjData)>
	FvUInt32	QueryArea(FvInt32 iX, FvInt32 iY, FvInt32 iRadius, TVisiter* pkVisiter)
	{
		if(iRadius < 2)
			return 0;

		#define AOICELLOBJ(pkNode) (FvAoIObject*)(((char*)pkNode) - sizeof(FvAoIBase))

		FvInt32 iMinX = iX - iRadius +1;
		FvInt32 iMaxX = iX + iRadius -1;
		FvInt32 iMinY = iY - iRadius +1;
		FvInt32 iMaxY = iY + iRadius -1;

		FvUInt16 uiMinIDX(0),uiMaxIDX(0),uiMinIDY(0),uiMaxIDY(0);
		AOI_GRID_MAKE_CELL_ID(iMinX, uiMinIDX, X);
		AOI_GRID_MAKE_CELL_ID(iMaxX, uiMaxIDX, X);
		AOI_GRID_MAKE_CELL_ID(iMinY, uiMinIDY, Y);
		AOI_GRID_MAKE_CELL_ID(iMaxY, uiMaxIDY, Y);

		FV_ASSERT(iMinX <= iMaxX && iMinY <= iMaxY);

		float fRadiusSqr = float(iRadius);
		fRadiusSqr = fRadiusSqr*fRadiusSqr;

		//! TODO: 找一种块判断方式,先判断块是否在圆内,剔除整块区域

		FvUInt32 uiCnt(0);
		for(FvUInt16 j=uiMinIDY; j<=uiMaxIDY; ++j)
		{
			for(FvUInt16 i=uiMinIDX; i<=uiMaxIDX; ++i)
			{
				FvAoIDLNode* pkHead = &(m_pkCells[j * m_uiGridIDCntX + i]);
				FvAoIDLNode* pkNode = pkHead->m_pkNex;
				while(pkNode != pkHead)
				{
					FvAoIObject* pkObj = AOICELLOBJ(pkNode);
					float fDisX = float(pkObj->m_pkPos->m_iX - iX);
					float fDisY = float(pkObj->m_pkPos->m_iY - iY);
					float fDisSqr = fDisX*fDisX + fDisY*fDisY;

					if(fDisSqr < fRadiusSqr)
					{
						((*pkVisiter).*OnVisit)(pkObj->m_pkUserData);
						++uiCnt;
					}

					pkNode = pkNode->m_pkNex;
				}
			}
		}

		return uiCnt;

		#undef AOICELLOBJ
	}

#ifndef FV_DEBUG
protected:
#else
public:
#endif
public:
	FvUInt32	m_uiGridSizeBits;
	FvInt32		m_iGridInnerMinX;
	FvInt32		m_iGridInnerMinY;
	FvInt32		m_iGridInnerMaxX;
	FvInt32		m_iGridInnerMaxY;
	FvInt32		m_iGridIDMinX;
	FvInt32		m_iGridIDMinY;
	FvUInt32	m_uiGridIDCntX;
	FvUInt32	m_uiGridIDCntY;
	FvUInt32	m_uiCellCnt;
	FvInt32*	m_pkBorderX;
	FvInt32*	m_pkBorderY;
	FvAoIDLNode*m_pkCells;
};

#endif//__FvAoIGrid_H__
