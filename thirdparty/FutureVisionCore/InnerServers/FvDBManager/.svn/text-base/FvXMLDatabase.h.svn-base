//{future header message}
#ifndef __FvXMLDatabase_H__
#define __FvXMLDatabase_H__

#include "FvIDatabase.h"
#include <FvXMLSection.h>
#include <map>

class FvEntityDescriptionMap;
class FvEntityDefs;

class XMLDatabase : public FvIDatabase
{
public:
	XMLDatabase();
	~XMLDatabase();

	virtual bool Startup( const FvEntityDefs&,
				bool isFaultRecovery, bool isUpgrade, bool isSyncTablesToDefs );
	virtual bool ShutDown();

	virtual void MapLoginToEntityDBKey(
		const FvString & logOnName, const FvString & password,
		FvIDatabase::IMapLoginToEntityDBKeyHandler& handler );
	virtual void SetLoginMapping( const FvString & username,
		const FvString & password, const FvEntityDBKey& ekey,
		ISetLoginMappingHandler& handler );

	virtual void GetEntity( FvIDatabase::IGetEntityHandler& handler );
	virtual void PutEntity( const FvEntityDBKey& ekey, FvEntityDBRecordIn& erec,
		FvIDatabase::IPutEntityHandler& handler );
	virtual void DelEntity( const FvEntityDBKey & ekey,
		FvIDatabase::IDelEntityHandler& handler );

	virtual void  GetBaseAppMgrInitData(
			IGetBaseAppMgrInitDataHandler& handler );

	virtual void ExecuteRawCommand( const FvString & command,
		IExecuteRawCommandHandler& handler );

	virtual void PutIDs( int count, const FvEntityID * ids );
	virtual void GetIDs( int count, IGetIDsHandler& handler );
	virtual void WriteSpaceData( FvBinaryIStream& spaceData );

	virtual bool GetSpacesData( FvBinaryOStream& strm );
	virtual void RestoreEntities( FvDBEntityRecoverer& recoverer );

	virtual void RemapEntityMailboxes( const FvNetAddress& srcAddr,
			const FvBackupHash & destAddrs );

	virtual void AddSecondaryDB( const SecondaryDBEntry& entry );
	virtual void UpdateSecondaryDBs( const FvBaseAppIDs& ids,
			IUpdateSecondaryDBshandler& handler );
	virtual void GetSecondaryDBs( IGetSecondaryDBsHandler& handler );
	virtual FvUInt32 GetNumSecondaryDBs();
	virtual int ClearSecondaryDBs();

	virtual bool LockDB() 	{ return true; };
	virtual bool UnlockDB()	{ return true; };

private:
	bool DeleteEntity( FvDatabaseID, FvEntityTypeID );
	FvDatabaseID FindEntityByName( FvEntityTypeID, const FvString & name ) const;
	bool Commit();

	typedef std::map< FvString, FvDatabaseID > NameMap;
	typedef std::vector< NameMap >				NameMapVec;
	typedef std::map< FvDatabaseID, FvXMLSectionPtr > IDMap;

	FvXMLSectionPtr	m_spDBSection;
	NameMapVec m_kNameToIdMaps;
	IDMap m_kIDToData;

	struct LogOnMapping
	{
		FvString m_kPassword;
		FvEntityTypeID m_kTypeID;
		FvString m_kEntityName;

		LogOnMapping() {}
		LogOnMapping( const FvString& pass, FvEntityTypeID type,
				const FvString& name ) :
			m_kPassword( pass ), m_kTypeID( type ), m_kEntityName( name )
		{}
	};
	typedef std::map< FvString, LogOnMapping > LogonMap;
	LogonMap m_kLogonMap;
	FvXMLSectionPtr	m_spLogonMapSection;

	FvDatabaseID m_iMaxID;

	class ActiveSetEntry
	{
	public:
		ActiveSetEntry()
			{
				m_kBaseRef.m_kAddr.m_uiIP = 0;
				m_kBaseRef.m_kAddr.m_uiPort = 0;
				m_kBaseRef.m_iID = 0;
			}
		FvEntityMailBoxRef m_kBaseRef;
	};

	typedef std::map< FvDatabaseID, ActiveSetEntry > ActiveSet;
	ActiveSet m_kActiveSet;
	std::vector<FvEntityID> m_kSpareIDs;
	FvEntityID m_iNextID;

	const FvEntityDefs *m_pkEntityDefs;
	const FvEntityDefs *m_pkNewEntityDefs;
};

#endif // __FvXMLDatabase_H__
