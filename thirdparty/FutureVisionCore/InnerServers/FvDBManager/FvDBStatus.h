//{future header message}
#ifndef __FvDBStatus_H__
#define __FvDBStatus_H__

#include <FvBaseTypes.h>

#define DBSTATUS_WATCHER_STATUS_DETAIL_PATH "statusDetail"

class FvDBStatus
{
public:
	enum Status
	{
		STARTING,
		STARTUP_CONSOLIDATING,
		WAITING_FOR_APPS,
		RESTORING_STATE,
		RUNNING,
		SHUTTING_DOWN,
		SHUTDOWN_CONSOLIDATING
	};

public:
	FvDBStatus( Status status = STARTING, 
			const FvString& detail = "Starting" );
	
	Status GetStatus() const 				{ return Status( m_iStatus ); }
	const FvString &GetDetail() const 	{ return m_kDetail; }
	void SetDetail( const FvString &detail ) { m_kDetail = detail; }
	
	void RegisterWatchers();
	
	void Set( Status status, const FvString& detail );

	bool HasStarted() const		{ return m_iStatus == RUNNING; }
	
private:
	int m_iStatus;
	FvString m_kDetail;
};

#endif 	// __FvDBStatus_H__
