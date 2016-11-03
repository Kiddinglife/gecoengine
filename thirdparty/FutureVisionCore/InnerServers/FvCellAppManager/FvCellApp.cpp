#include "FvCellApp.h"
#include <FvDebug.h>
#include "FvSpace.h"


FvCellApp::FvCellApp(FvNetNub& kNub, const FvNetAddress& kAddr, FvCellAppID iID, FvUInt8 uiLoadSpaceGeometryIdx)
:FvNetChannelOwner(kNub, kAddr)
,m_iID(iID)
,m_fLoad(0.0f)
,m_uiLoadSpaceGeometryIdx(uiLoadSpaceGeometryIdx)
{
	Channel().IsIrregular(true);
}

FvCellApp::~FvCellApp()
{
}

bool FvCellApp::AddCell(FvCell* pkCell)
{
	FV_ASSERT(pkCell);

	pkCell->SetCellApp(this);
	m_kCells.push_back(pkCell);
	return true;
}

void FvCellApp::EraseCell(FvCell* pkCell)
{
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

void FvCellApp::InformOfLoad( const CellAppMgrInterface::InformOfLoadArgs & args )
{

}


void FvCellApp::UpdateBounds( FvBinaryIStream & data )
{

}


void FvCellApp::ShutDownApp( const CellAppMgrInterface::ShutDownAppArgs & args )
{

}


void FvCellApp::AckCellAppShutDown( const CellAppMgrInterface::AckCellAppShutDownArgs & args )
{

}



