//{future header message}
#ifndef __FvCellApp_H__
#define __FvCellApp_H__

#include "FvCellAppManagerInterface.h"
#include <vector>

class FvCell;


class FvCellApp : public FvNetChannelOwner
{
public:
	FvCellApp(FvNetNub& kNub, const FvNetAddress& kAddr, FvCellAppID iID, FvUInt8 uiLoadSpaceGeometryIdx);
	~FvCellApp();

	FvCellAppID	CellAppID() { return m_iID; }
	FvUInt8 LoadSpaceGeometryIdx() { return m_uiLoadSpaceGeometryIdx; }
	int	GetCellCnt() { return m_kCells.size(); }
	float GetLoad() { return m_fLoad; }
	bool AddCell(FvCell* pkCell);
	void EraseCell(FvCell* pkCell);

	void InformOfLoad( const CellAppMgrInterface::InformOfLoadArgs & args );
	void UpdateBounds( FvBinaryIStream & data );
	void ShutDownApp( const CellAppMgrInterface::ShutDownAppArgs & args );
	void AckCellAppShutDown( const CellAppMgrInterface::AckCellAppShutDownArgs & args );

protected:
	FvCellAppID		m_iID;
	FvUInt8			m_uiLoadSpaceGeometryIdx;
	float			m_fLoad;
	typedef std::vector<FvCell*> Cells;
	Cells			m_kCells;
};


#endif//__FvCellApp_H__

