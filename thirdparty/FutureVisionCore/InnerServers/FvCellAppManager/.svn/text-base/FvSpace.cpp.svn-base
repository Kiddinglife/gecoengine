#include "FvSpace.h"
#include <FvDebug.h>
#include "FvCellApp.h"
#include <../FvCell/FvCellAppInterface.h>
#include "FvCellAppManager.h"


FvCell::FvCell(FvUInt16 uiIdx, FvCellRect& kRect)
:m_pkSpace(NULL)
,m_uiIdx(uiIdx)
,m_kRect(kRect)
,m_pkCellApp(NULL)
,m_fLoad(0.0f)
,m_iNumEntities(0)
{
}

FvCell::~FvCell()
{
}

FvSpace::FvSpace(FvSpaceID iID, SpaceInfo* pkSpaceInfo)
:m_uiSpaceType(pkSpaceInfo->m_uiSpaceType)
,m_iID(iID)
,m_kSpacePath(pkSpaceInfo->m_kSpacePath)
,m_bGlobal(pkSpaceInfo->m_bGlobal)
,m_kSpaceKDTree(pkSpaceInfo->m_kKDTree)
{
	m_kCells.resize(pkSpaceInfo->m_uiCellCnt, NULL);
}

FvSpace::~FvSpace()
{
	Cells::iterator itrB = m_kCells.begin();
	Cells::iterator itrE = m_kCells.end();
	while(itrB != itrE)
	{
		delete *itrB;
		++itrB;
	}
	m_kCells.clear();
}

bool FvSpace::AddCell(FvCell* pkCell)
{
	FV_ASSERT(pkCell);

	FvUInt16 uiCellIdx = pkCell->Idx();
	FV_ASSERT(uiCellIdx < (FvUInt16)m_kCells.size() && !m_kCells[uiCellIdx]);

	pkCell->SetSpace(this);
	m_kCells[uiCellIdx] = pkCell;
	return true;
}

void FvSpace::EraseCell(FvCell* pkCell)
{
	FV_ASSERT(pkCell);

	Cells::iterator itrB = m_kCells.begin();
	Cells::iterator itrE = m_kCells.end();
	while(itrB != itrE)
	{
		if(*itrB == pkCell)
		{
			m_kCells.erase(itrB);
			break;
		}
		++itrB;
	}
}

bool FvSpace::AddData(const FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kString)
{
	DataEntryMapKey kKey(kEntryID, uiKey);
	return m_kDataEntryMap.insert(std::make_pair(kKey, kString)).second;
}

void FvSpace::DelData(const FvSpaceEntryID& kEntryID)
{
	DataEntryMap::iterator itrB = m_kDataEntryMap.begin();
	DataEntryMap::iterator itrE = m_kDataEntryMap.end();
	DataEntryMap::iterator itr;
	while(itrB != itrE)
	{
		if(itrB->first.m_kEntryID == kEntryID)
		{
			itr = itrB;
			++itrB;
			m_kDataEntryMap.erase(itr);
		}
		else
		{
			++itrB;
		}
	}
}

FvCell*	FvSpace::FindCell(FvVector3& kPos)
{
	FvUInt16 uiCellIdx = m_kSpaceKDTree.FindCellIdx(kPos.x, kPos.y);
	FV_ASSERT(uiCellIdx < (FvUInt16)m_kCells.size() && m_kCells[uiCellIdx]);
	if(m_kCells[uiCellIdx]->IsInCell(kPos))
		return m_kCells[uiCellIdx];
	else
		return NULL;
}

bool FvSpace::IsEmpty()
{
	Cells::iterator itrB = m_kCells.begin();
	Cells::iterator itrE = m_kCells.end();
	while(itrB != itrE)
	{
		if((*itrB)->GetEntityCnt() > 0)
			return false;
		++itrB;
	}

	return true;
}

void FvSpace::Destroy()
{
	Cells::iterator itrB = m_kCells.begin();
	Cells::iterator itrE = m_kCells.end();
	while(itrB != itrE)
	{
		FvCell* pkCell = *itrB;
		//FV_ASSERT(pkCell->GetEntityCnt() == 0);

		FvCellApp* pkApp = pkCell->GetCellApp();
		pkApp->EraseCell(pkCell);

		pkApp->Bundle().StartMessage(CellAppInterface::ShutDownSpace);
		pkApp->Bundle() << m_iID;
		CellAppInterface::ShutDownSpaceArgs kArgs;
		pkApp->Bundle() << kArgs;
		pkApp->Channel().DelayedSend();

		delete pkCell;
		++itrB;
	}
	m_kCells.clear();
}










