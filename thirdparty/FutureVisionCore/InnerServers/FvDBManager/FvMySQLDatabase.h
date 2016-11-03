//{future header message}
#ifndef __FvMySQLDatabase_H__
#define __FvMySQLDatabase_H__

#include <FvNetInterfaces.h>

#include "FvIDatabase.h"

class MySql;
class MySqlThreadResPool;
struct MySqlThreadData;

struct FvDBConfigConnection;
class FvDBConfigServer;

FvUInt32 InitInfoTable( MySql& connection );

class MySqlDatabase : public FvIDatabase, public FvNetTimerExpiryHandler
{
private:
	MySqlDatabase();
public:
	static MySqlDatabase *Create();
	~MySqlDatabase();

	virtual bool Startup( const FvEntityDefs&, bool isFaultRecovery, 
							bool isUpgrade, bool isSyncTablesToDefs );
	virtual bool ShutDown();

	virtual void MapLoginToEntityDBKey(
		const FvString & logOnName, const FvString & password,
		FvIDatabase::IMapLoginToEntityDBKeyHandler& handler );
	virtual void SetLoginMapping( const FvString & username,
		const FvString & password, const FvEntityDBKey& ekey,
		FvIDatabase::ISetLoginMappingHandler& handler );

	virtual void GetEntity( FvIDatabase::IGetEntityHandler& handler );
	virtual void PutEntity( const FvEntityDBKey& ekey, FvEntityDBRecordIn& erec,
		IPutEntityHandler& handler );
	virtual void DelEntity( const FvEntityDBKey & ekey,
		FvIDatabase::IDelEntityHandler& handler );

	virtual void ExecuteRawCommand( const FvString & command,
		FvIDatabase::IExecuteRawCommandHandler& handler );

	virtual void PutIDs( int numIDs, const FvEntityID * ids );
	virtual void GetIDs( int numIDs, FvIDatabase::IGetIDsHandler& handler );

	virtual void WriteSpaceData( FvBinaryIStream& spaceData );

	virtual bool GetSpacesData( FvBinaryOStream& strm );
	virtual void RestoreEntities( FvDBEntityRecoverer& recoverer );

	virtual void SetGameTime( FvTimeStamp gameTime );

	virtual void GetBaseAppMgrInitData(
			IGetBaseAppMgrInitDataHandler& handler );

	virtual void RemapEntityMailboxes( const FvNetAddress& srcAddr,
			const FvBackupHash & destAddrs );

	virtual void AddSecondaryDB( const FvIDatabase::SecondaryDBEntry& entry );
	virtual void UpdateSecondaryDBs( const FvBaseAppIDs& ids,
			IUpdateSecondaryDBshandler& handler );
	virtual void GetSecondaryDBs( FvIDatabase::IGetSecondaryDBsHandler& handler );
	virtual FvUInt32 GetNumSecondaryDBs();
	virtual int ClearSecondaryDBs();

	virtual bool LockDB();
	virtual bool UnlockDB();

	virtual int HandleTimeout( FvNetTimerID id, void * arg );

	MySqlThreadResPool &GetThreadResPool()	{	return *m_pkThreadResPool;	}
	MySqlThreadData& GetMainThreadData();

	int GetMaxSpaceDataSize() const { return m_iMaxSpaceDataSize; }

	FvDBConfigServer &GetServerConfig();

	bool HasFatalConnectionError()	{	return m_kReconnectTimerID != FV_NET_TIMER_ID_NONE;	}
	void OnConnectionFatalError();
	bool RestoreConnectionToDb();

	void OnWriteSpaceOpStarted()	{	++m_iNumWriteSpaceOpsInProgress;	}
	void OnWriteSpaceOpCompleted()	{	--m_iNumWriteSpaceOpsInProgress;	}

	unsigned int WatcherGetNumBusyThreads() const;
	double WatcherGetBusyThreadsMaxElapsedSecs() const;
	double WatcherGetAllOpsCountPerSec() const;
	double WatcherGetAllOpsAvgDurationSecs() const;

private:
	void PrintConfigStatus();

	void CreateSpecialFutureVisionTables( MySql& connection );
	static void InitSpecialFutureVisionTables( MySql& connection,
			const FvEntityDefs& entityDefs );
	static bool CheckSpecialFutureVisionTables( MySql& connection );

	static FvUInt32 GetNumSecondaryDBs( MySql& connection );

private:
	MySqlThreadResPool *m_pkThreadResPool;

	int m_iMaxSpaceDataSize;
	int m_iNumConnections;
	int m_iNumWriteSpaceOpsInProgress;

	FvNetTimerID m_kReconnectTimerID;
	size_t m_stReconnectCount;
};

#endif // __FvMySQLDatabase_H__
