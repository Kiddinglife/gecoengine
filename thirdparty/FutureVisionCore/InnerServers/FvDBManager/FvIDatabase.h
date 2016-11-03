//{future header message}
#ifndef __FvIDatabase_H__
#define __FvIDatabase_H__

#include <FvNetTypes.h>
#include <FvLoginInterface.h>
#include <FvBackupHash.h>

#include <string>
#include <limits>

class FvEntityDescriptionMap;
class FvEntityDefs;
class FvDBEntityRecoverer;

typedef FvLogOnStatus FvDatabaseLoginStatus;
typedef std::vector<FvEntityTypeID> FvTypeIDVec;
typedef std::vector< int > FvBaseAppIDs;

struct FvEntityKey
{
	FvEntityTypeID m_uiTypeID;
	FvDatabaseID m_iDBID;
	FvEntityKey( FvEntityTypeID type, FvDatabaseID id ) : m_uiTypeID( type ), m_iDBID( id ) {}
	bool operator<( const FvEntityKey& other ) const
	{
		return (m_uiTypeID < other.m_uiTypeID) ||
				((m_uiTypeID == other.m_uiTypeID) && (m_iDBID < other.m_iDBID));
	}
};

struct FvEntityDBKey : public FvEntityKey
{
	FvEntityDBKey( FvEntityTypeID typeID, FvDatabaseID dbID,
			const FvString & s = FvString() ) :
		FvEntityKey( typeID, dbID ), m_kName( s ) {}
	explicit FvEntityDBKey( const FvEntityKey& key ) : FvEntityKey( key ), m_kName() {}
	FvString m_kName;
};

class FvEntityDBRecordBase
{
	FvEntityMailBoxRef **m_ppkBaseRef;

public:
	FvEntityDBRecordBase() : m_ppkBaseRef(0)	{}

	void ProvideBaseMB( FvEntityMailBoxRef*& pBaseRef )
	{	m_ppkBaseRef = &pBaseRef;	}
	void UnprovideBaseMB()
	{	m_ppkBaseRef = 0;	}
	bool IsBaseMBProvided() const
	{	return m_ppkBaseRef != NULL;	}
	void SetBaseMB( FvEntityMailBoxRef* pBaseMB )
	{
		FV_ASSERT(IsBaseMBProvided());
		if (pBaseMB)
			**m_ppkBaseRef = *pBaseMB;
		else
			*m_ppkBaseRef = 0;
	}
	FvEntityMailBoxRef* GetBaseMB()
	{
		FV_ASSERT(IsBaseMBProvided());
		return *m_ppkBaseRef;
	}
};

template < class STRM_TYPE >
class FvEntityDBRecord : public FvEntityDBRecordBase
{
	STRM_TYPE *m_pkStrm;

public:
	FvEntityDBRecord() : FvEntityDBRecordBase(), m_pkStrm(0)	{}

	void ProvideStrm( STRM_TYPE& strm )
	{	m_pkStrm = &strm;	}
	void UnprovideStrm()
	{	m_pkStrm = 0;	}
	bool IsStrmProvided() const
	{	return m_pkStrm != 0;	}
	STRM_TYPE& GetStrm()
	{
		FV_ASSERT(IsStrmProvided());
		return *m_pkStrm;
	}
};

typedef FvEntityDBRecord<FvBinaryIStream> FvEntityDBRecordIn;
typedef FvEntityDBRecord<FvBinaryOStream> FvEntityDBRecordOut;

class FvIDatabase
{
public:
	virtual ~FvIDatabase() {}
	virtual bool Startup( const FvEntityDefs& entityDefs, bool isFaultRecovery,
							bool isUpgrade, bool isSyncTablesToDefs ) = 0;
	virtual bool ShutDown() = 0;

	struct IMapLoginToEntityDBKeyHandler
	{
		virtual void OnMapLoginToEntityDBKeyComplete(
			FvDatabaseLoginStatus status, const FvEntityDBKey& ekey ) = 0;
	};

	virtual void MapLoginToEntityDBKey(
		const FvString & username, const FvString & password,
		IMapLoginToEntityDBKeyHandler& handler ) = 0;

	struct ISetLoginMappingHandler
	{
		virtual void OnSetLoginMappingComplete() = 0;
	};

	virtual void SetLoginMapping( const FvString & username,
		const FvString & password, const FvEntityDBKey& ekey,
		ISetLoginMappingHandler& handler ) = 0;

	struct IGetEntityHandler
	{
		virtual FvEntityDBKey &GetKey() = 0;

		virtual FvEntityDBRecordOut &OutRec() = 0;

		virtual const FvString *GetPasswordOverride() const {	return 0;	}

		virtual void OnGetEntityComplete( bool isOK ) = 0;
	};

	virtual void GetEntity( IGetEntityHandler& handler ) = 0;

	struct IPutEntityHandler
	{
		virtual void OnPutEntityComplete( bool isOK, FvDatabaseID dbID ) = 0;
	};

	virtual void PutEntity( const FvEntityDBKey& ekey,
		FvEntityDBRecordIn& erec, IPutEntityHandler& handler ) = 0;

	struct IDelEntityHandler
	{
		virtual void OnDelEntityComplete( bool isOK ) = 0;
	};

	virtual void DelEntity( const FvEntityDBKey & ekey,
		IDelEntityHandler& handler ) = 0;

	virtual void SetGameTime( FvTimeStamp time ) {};

	class IGetBaseAppMgrInitDataHandler
	{
	public:

		virtual void OnGetBaseAppMgrInitDataComplete( FvTimeStamp gameTime,
				FvInt32 maxAppID ) = 0;
	};

	virtual void GetBaseAppMgrInitData(
			IGetBaseAppMgrInitDataHandler& handler ) = 0;

	struct IExecuteRawCommandHandler
	{
		virtual FvBinaryOStream &Response() = 0;

		virtual void OnExecuteRawCommandComplete() = 0;
	};
	virtual void ExecuteRawCommand( const FvString & command,
		IExecuteRawCommandHandler& handler ) = 0;

	virtual void PutIDs( int count, const FvEntityID * ids ) = 0;

	struct IGetIDsHandler
	{
		virtual FvBinaryOStream &GetIDStrm() = 0;

		virtual void ResetStrm() = 0;

		virtual void OnGetIDsComplete() = 0;
	};
	virtual void GetIDs( int count, IGetIDsHandler& handler ) = 0;

	virtual void WriteSpaceData( FvBinaryIStream& spaceData ) = 0;

	virtual bool GetSpacesData( FvBinaryOStream& strm ) = 0;

	virtual void RestoreEntities( FvDBEntityRecoverer& recoverer ) = 0;

	virtual void RemapEntityMailboxes( const FvNetAddress& srcAddr,
			const FvBackupHash & destAddrs ) = 0;

	struct SecondaryDBEntry
	{
		FvNetAddress m_kAddr;
		FvInt32 m_iAppID;
		FvString m_kLocation;

		SecondaryDBEntry( FvUInt32 ip, FvInt16 port, FvInt32 id,
				const FvString& loc ) :
			m_kAddr( ip, port ), m_iAppID(id), m_kLocation( loc )
		{}
		SecondaryDBEntry() : m_iAppID( 0 ) {}
	};
	typedef std::vector< SecondaryDBEntry > SecondaryDBEntries;

	virtual void AddSecondaryDB( const SecondaryDBEntry& entry ) = 0;

	class IUpdateSecondaryDBshandler
	{
	public:
		virtual void OnUpdateSecondaryDBsComplete(
				const SecondaryDBEntries& removedEntries ) = 0;
	};

	virtual void UpdateSecondaryDBs( const FvBaseAppIDs& ids,
			IUpdateSecondaryDBshandler& handler ) = 0;

	struct IGetSecondaryDBsHandler
	{
		virtual void OnGetSecondaryDBsComplete(
				const SecondaryDBEntries& entries ) = 0;
	};
	virtual void GetSecondaryDBs( IGetSecondaryDBsHandler& handler ) = 0;

	virtual FvUInt32 GetNumSecondaryDBs() = 0;

	virtual int ClearSecondaryDBs() = 0;

	virtual bool LockDB() = 0;

	virtual bool UnlockDB() = 0;
};

#endif // __FvIDatabase_H__
