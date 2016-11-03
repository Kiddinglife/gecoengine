//{future header message}
#ifndef __FvSpace_H__
#define __FvSpace_H__

#include <vector>
#include <map>
#include <FvVector4.h>
#include <FvNetTypes.h>
#include <FvCellRect.h>


class FvSpace;
class FvCellApp;
struct SpaceInfo;
struct SpaceKDTree;


class FvCell
{
public:
	FvCell(FvUInt16 uiIdx, FvCellRect& kRect);
	~FvCell();

	void	SetSpace(FvSpace* pkSpace) { m_pkSpace=pkSpace; }
	void	SetCellApp(FvCellApp* pkCellApp) { m_pkCellApp=pkCellApp; }
	void	InformOfLoad(float fLoad, FvInt32 iNumEntities) { m_fLoad=fLoad; m_iNumEntities=iNumEntities; }
	bool	IsInCell(FvVector3& kPos) { return m_kRect.IsPointIn(kPos); }
	FvUInt16 Idx() { return m_uiIdx; }
	FvSpace*GetSpace() { return m_pkSpace; }
	FvCellApp* GetCellApp() { return m_pkCellApp; }
	FvInt32	GetEntityCnt() { return m_iNumEntities; }

protected:
	FvSpace*		m_pkSpace;
	FvCellApp*		m_pkCellApp;
	FvUInt16		m_uiIdx;
	FvCellRect		m_kRect;
	float			m_fLoad;//! TODO:区分load和numEntities是否是考虑到纯玩家的人数和包括npc的人数?
	FvInt32			m_iNumEntities;
};


class FvSpace
{
public:
	FvSpace(FvSpaceID iID, SpaceInfo* pkSpaceInfo);
	~FvSpace();

	bool	AddCell(FvCell* pkCell);
	void	EraseCell(FvCell* pkCell);
	bool	AddData(const FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kString);
	void	DelData(const FvSpaceEntryID& kEntryID);
	FvCell*	FindCell(FvVector3& kPos);
	FvUInt16 SpaceType() const { return m_uiSpaceType; }
	FvSpaceID SpaceID() const { return m_iID; }
	typedef std::vector<FvCell*> Cells;
	Cells&	GetCells() { return m_kCells; }
	bool	IsEmpty();
	void	Destroy();
	const FvString& SpacePath() const { return m_kSpacePath; }
	bool	IsGlobal() const { return m_bGlobal; }

protected:
	FvUInt16		m_uiSpaceType;
	FvSpaceID		m_iID;
	FvString		m_kSpacePath;
	bool			m_bGlobal;
	Cells			m_kCells;
	SpaceKDTree&	m_kSpaceKDTree;

	struct DataEntryMapKey
	{
		FvSpaceEntryID	m_kEntryID;
		FvUInt16		m_uiKey;

		DataEntryMapKey(const FvSpaceEntryID& kEntryID, FvUInt16 uiKey)
		:m_kEntryID(kEntryID),m_uiKey(uiKey){}
		bool operator < (const DataEntryMapKey& kOther) const
		{ return m_uiKey < kOther.m_uiKey ||
			(m_uiKey == kOther.m_uiKey && m_kEntryID < kOther.m_kEntryID); }
	};
	typedef std::map<DataEntryMapKey, FvString> DataEntryMap;
	DataEntryMap	m_kDataEntryMap;
};



#endif//__FvSpace_H__
