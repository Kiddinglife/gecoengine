#include "FvDBStatus.h"
#include <FvNetWatcherGlue.h>
#include "FvIDatabase.h"
FvDBStatus::FvDBStatus( Status status, const FvString& detail ) :
	m_iStatus( status ),
	m_kDetail( detail )
{}

void FvDBStatus::RegisterWatchers()
{
	FV_WATCH( "status", m_iStatus, FvWatcher::WT_READ_ONLY,
			"Status of this process. Mainly relevant during startup and "
			"shutdown" );
	FV_WATCH( DBSTATUS_WATCHER_STATUS_DETAIL_PATH, m_kDetail,
			FvWatcher::WT_READ_WRITE,
			"Human readable information about the current status of this "
			"process. Mainly relevant during startup and shutdown." );
	FV_WATCH( "HasStarted", *this, &FvDBStatus::HasStarted );
}

void FvDBStatus::Set( Status status, const FvString& detail )
{
	m_iStatus = status;
	m_kDetail = detail;
}
