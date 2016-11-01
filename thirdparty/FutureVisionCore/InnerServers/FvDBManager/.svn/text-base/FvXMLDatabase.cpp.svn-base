#include "FvXMLDatabase.h"

#include <FvDebug.h>
#include <FvEntityDescriptionMap.h>
#include <FvEntityDefConstants.h>
#include <FvServerResource.h>
#include "FvDatabase.h"
#include "FvDBEntitydefs.h"
#include "FvDBConfig.h"
#include "FvDBEntityRecoverer.h"
#include <FvServerConfig.h>

FV_DECLARE_DEBUG_COMPONENT(0)

static const char* DATABASE_INFO_SECTION = "_FutureVisionInfo";
static const char* DATABASE_LOGONMAPPING_SECTION = "LogOnMapping";

XMLDatabase::XMLDatabase() :
	m_iMaxID( 0 ),
	m_iNextID( 1 ),
	m_pkEntityDefs( 0 ),
	m_pkNewEntityDefs( 0 )
{
}


XMLDatabase::~XMLDatabase()
{
	this->ShutDown();
}


bool XMLDatabase::Startup( const FvEntityDefs& entityDefs,
		bool /*isFaultRecovery*/, bool isUpgrade, bool isSyncTablesToDefs )
{
	if (isUpgrade)
	{
		FV_WARNING_MSG( "XMLDatabase::Startup: "
						"XML database does not support --upgrade option\n" );
	}

	if (isSyncTablesToDefs)
	{
		FV_WARNING_MSG( "XMLDatabase::Startup: "
						"XML database does not support --sync-tables-to-defs "
						"option\n" );
	}

	m_kNameToIdMaps.resize( entityDefs.GetNumEntityTypes() );

	m_spDBSection = FvServerResource::OpenSection( FvEntityDef::Constants::XmlDatabaseFile() );
	m_pkEntityDefs = &entityDefs;

	if (!m_spDBSection)
	{
		FV_ERROR_MSG( "XMLDatabase::Startup: Could not open %s: Creating it.\n",
				FvEntityDef::Constants::XmlDatabaseFile() );
		m_spDBSection = FvServerResource::OpenSection(
				FvEntityDef::Constants::XmlDatabaseFile(), true );
	}

	FvXMLSectionPtr pInfoSection = m_spDBSection->OpenChild( DATABASE_INFO_SECTION );
	if (pInfoSection)
	{
		m_spLogonMapSection =
				pInfoSection->OpenChild( DATABASE_LOGONMAPPING_SECTION );
		if (m_spLogonMapSection)
		{
			for (FvXMLSectionIterator it = m_spLogonMapSection->Begin();
					it != m_spLogonMapSection->End(); ++it)
			{
				FvEntityTypeID typeID =
					m_pkEntityDefs->GetEntityType( (*it)->ReadString( "type" ) );
				if (typeID != INVALID_TYPEID)
				{
					m_kLogonMap[ (*it)->ReadString( "logOnName" ) ] =
							LogOnMapping( (*it)->ReadString( "password" ),
										typeID,
										(*it)->ReadString( "entityName" ) );
				}
				else
				{
					FV_WARNING_MSG( "Database::init: Logon mapping ignored because"
							" '%s' is not a valid entity type\n",
							(*it)->ReadString( "type" ).c_str() );
				}
			}
		}
		else
		{
			m_spLogonMapSection =
					pInfoSection->NewSection( DATABASE_LOGONMAPPING_SECTION );
		}
	}
	else
	{
		pInfoSection = m_spDBSection->NewSection( DATABASE_INFO_SECTION );
		m_spLogonMapSection =
				pInfoSection->NewSection( DATABASE_LOGONMAPPING_SECTION );
	}

	bool hasEntityWithNoDBID = false;
	bool shouldAssignDBIDs = false;
	for ( int i = 0; i < 2; ++i )
	{
		for (FvXMLSectionIterator sectionIter = m_spDBSection->Begin();
				sectionIter != m_spDBSection->End(); ++sectionIter)
		{
			FvXMLSectionPtr pCurr = *sectionIter;

			FvEntityTypeID typeID =
					m_pkEntityDefs->GetEntityType( pCurr->SectionName() );
			if (typeID == INVALID_TYPEID)
			{
				if (pCurr.GetObject() != pInfoSection.GetObject() && (i == 0))
					FV_WARNING_MSG( "Database::init: '%s' is not a valid entity "
							"type - data ignored\n",
							pCurr->SectionName().c_str() );
				continue;
			}

			FvDatabaseID id = pCurr->Read( "databaseID", FvInt64( 0 ) );
			if (id == 0)
			{
				if (shouldAssignDBIDs)
				{
					id = ++m_iMaxID;
					pCurr->Write( "databaseID", id );
				}
				else
				{
					hasEntityWithNoDBID = true;
					continue;
				}
			}
			else
			{
				if (shouldAssignDBIDs)
					continue;
				else if (id > m_iMaxID)
					m_iMaxID = id;
			}

			IDMap::iterator idIter = m_kIDToData.find( id );
			if (idIter != m_kIDToData.end())
			{
				if (id >= 0)
				{
					FV_WARNING_MSG( "Database::init: '%s' and '%s' have same id "
							"(%"FMT_DBID") - second entity ignored\n",
							idIter->second->SectionName().c_str(),
							pCurr->SectionName().c_str(),
							id );
				}
				continue;
			}
			else
			{
				m_kIDToData[id] = pCurr;
			}

			const FvString& nameProperty =
					m_pkEntityDefs->GetNameProperty( typeID );
			if (!nameProperty.empty())
			{
				FvString entityName = pCurr->ReadString( nameProperty );
				NameMap& nameMap = m_kNameToIdMaps[ typeID ];
				NameMap::const_iterator it = nameMap.find( entityName );
				if (it == nameMap.end())
					nameMap[entityName] = id;
				else
					FV_WARNING_MSG( "XMLDatabase::Startup: Multiple "
							"entities of type '%s' have the same name: '%s' - "
							"second entity will not be retrievable by name\n",
							pCurr->SectionName().c_str(), entityName.c_str() );
			}
		}

		if (hasEntityWithNoDBID)
			shouldAssignDBIDs = true;
		else
			break;
	}

	FV_WATCH( "maxID",			m_iMaxID,			FvWatcher::WT_READ_ONLY );

	return true;
}


bool XMLDatabase::ShutDown()
{
	if (m_spDBSection)
	{
		//FvServerResource::Instance().Save( FvEntityDef::Constants::XmlDatabaseFile() );
		m_spDBSection = (FvXMLSection *)NULL;
	}

	return true;
}


void XMLDatabase::MapLoginToEntityDBKey(
	const FvString & logOnName, const FvString & password,
	FvIDatabase::IMapLoginToEntityDBKeyHandler& handler )
{
	LogonMap::const_iterator it = m_kLogonMap.find( logOnName );
	if (it != m_kLogonMap.end())
	{
		if (password == it->second.m_kPassword)
		{
			handler.OnMapLoginToEntityDBKeyComplete(
					FvDatabaseLoginStatus::LOGGED_ON,
					FvEntityDBKey( it->second.m_kTypeID, 0, it->second.m_kEntityName ) );
		}
		else
		{
			handler.OnMapLoginToEntityDBKeyComplete(
					FvDatabaseLoginStatus::LOGIN_REJECTED_INVALID_PASSWORD,
					FvEntityDBKey( 0, 0 ) );
		}
	}
	else
	{
		handler.OnMapLoginToEntityDBKeyComplete(
				FvDatabaseLoginStatus::LOGIN_REJECTED_NO_SUCH_USER,
				FvEntityDBKey( 0, 0 ) );
	}
}

void XMLDatabase::SetLoginMapping( const FvString & username,
	const FvString & password, const FvEntityDBKey& ekey,
	ISetLoginMappingHandler& handler )
{
	IDMap::const_iterator iterData = m_kIDToData.find( ekey.m_iDBID );
	FV_ASSERT( iterData != m_kIDToData.end() )
	FV_ASSERT( this->FindEntityByName( ekey.m_uiTypeID, ekey.m_kName ) == ekey.m_iDBID );

	FvXMLSectionPtr pSection;
	if ( m_kLogonMap.find( username ) != m_kLogonMap.end() )
	{
		for ( FvXMLSectionIterator it = m_spLogonMapSection->Begin();
				it != m_spLogonMapSection->End(); ++it )
		{
			if ((*it)->ReadString( "logOnName" ) == username)
			{
				pSection = *it;
				break;
			}
		}
	}

	m_kLogonMap[ username ] = LogOnMapping( password, ekey.m_uiTypeID, ekey.m_kName );

	const FvString& typeName =
			m_pkEntityDefs->GetEntityDescription( ekey.m_uiTypeID ).Name();
	if (!pSection)
		pSection = m_spLogonMapSection->NewSection( "item" );
	pSection->WriteString( "logOnName", username );
	pSection->WriteString( "password", password );
	pSection->WriteString( "type", typeName );
	pSection->WriteString( "entityName", ekey.m_kName );

	handler.OnSetLoginMappingComplete();
}

void XMLDatabase::GetEntity( FvIDatabase::IGetEntityHandler& handler )
{
	const FvEntityDefs& entityDefs = *m_pkEntityDefs;

	FvEntityDBKey&		ekey = handler.GetKey();
	FvEntityDBRecordOut&	erec = handler.OutRec();

	FV_ASSERT( m_spDBSection );

	bool isOK = true;

	const FvEntityDescription& desc =
		entityDefs.GetEntityDescription( ekey.m_uiTypeID );

	bool lookupByName = (!ekey.m_iDBID);
	if (lookupByName)
		ekey.m_iDBID = this->FindEntityByName( ekey.m_uiTypeID, ekey.m_kName );

	isOK = (ekey.m_iDBID != 0);
	if (isOK)
	{
		IDMap::const_iterator iterData = m_kIDToData.find( ekey.m_iDBID );
		if ( iterData != m_kIDToData.end() )
		{
			FvXMLSectionPtr pData = iterData->second;
			if (!lookupByName)
			{
				const FvString& nameProperty =
						entityDefs.GetNameProperty( ekey.m_uiTypeID );
				if (!nameProperty.empty())
					ekey.m_kName = pData->ReadString( nameProperty );
			}

			if (erec.IsStrmProvided())
			{
				const FvString* pPasswordOverride = handler.GetPasswordOverride();
                if (pPasswordOverride)
				{
					bool isBlobPasswd = (entityDefs.
						GetPropertyType( ekey.m_uiTypeID, "password" ) == "BLOB");
					FvXMLSectionPtr pPasswordSection = pData->OpenChild( "password" );
					if (pPasswordSection)
					{
						FvString existingPassword = pPasswordSection->AsString();
						if (isBlobPasswd)
							pPasswordSection->SetBlob( *pPasswordOverride );
						else
							pPasswordSection->SetString( *pPasswordOverride );
						FV_ASSERT(0 && "Code is abandoned!");
						//! remove by Uman, 20101120, 接口废弃, 因为属性的遍历顺序已改变
						//desc.AddSectionToStream( pData, erec.GetStrm(),
						//	FvEntityDescription::FV_BASE_DATA |
						//	FvEntityDescription::FV_CELL_DATA |
						//	FvEntityDescription::FV_ONLY_PERSISTENT_DATA	);
						pPasswordSection->SetString( existingPassword );
					}
					else
					{
						if (isBlobPasswd)
							pData->WriteBlob( "password", *pPasswordOverride );
						else
							pData->WriteString( "password", *pPasswordOverride );
						FV_ASSERT(0 && "Code is abandoned!");
						//! remove by Uman, 20101120, 接口废弃, 因为属性的遍历顺序已改变
						//desc.AddSectionToStream( pData, erec.GetStrm(),
						//	FvEntityDescription::FV_BASE_DATA |
						//	FvEntityDescription::FV_CELL_DATA |
						//	FvEntityDescription::FV_ONLY_PERSISTENT_DATA );
						pData->DelChild( "password" );
					}
				}
				else
				{
					FV_ASSERT(0 && "Code is abandoned!");
					//! remove by Uman, 20101120, 接口废弃, 因为属性的遍历顺序已改变
					//desc.AddSectionToStream( pData, erec.GetStrm(),
					//	FvEntityDescription::FV_BASE_DATA |
					//	FvEntityDescription::FV_CELL_DATA |
					//	FvEntityDescription::FV_ONLY_PERSISTENT_DATA	);
				}

				if (desc.HasCellScript())
				{
					FvVector3 position = pData->ReadVector3( "position" );
					const FvVector3 direction_ = pData->ReadVector3( "direction" );
					FvDirection3 direction(direction_.x, direction_.y, direction_.z);
					FvSpaceID spaceID = pData->ReadInt( "spaceID" );
					erec.GetStrm() << position << direction << spaceID;
				}
			}
		}
		else
		{
			isOK = false;
		}

		if (isOK && erec.IsBaseMBProvided() && erec.GetBaseMB())
		{
			ActiveSet::iterator iter = m_kActiveSet.find( ekey.m_iDBID );

			if (iter != m_kActiveSet.end())
				erec.SetBaseMB(&iter->second.m_kBaseRef);
			else
				erec.SetBaseMB( 0 );
		}
	}

	handler.OnGetEntityComplete( isOK );
}

void XMLDatabase::PutEntity( const FvEntityDBKey& ekey, FvEntityDBRecordIn& erec,
							 FvIDatabase::IPutEntityHandler& handler )
{
	FV_ASSERT( m_spDBSection );

	const FvEntityDefs& entityDefs = *m_pkEntityDefs;
	const FvEntityDescription& desc =
		entityDefs.GetEntityDescription( ekey.m_uiTypeID );
	const FvString& nameProperty = entityDefs.GetNameProperty( ekey.m_uiTypeID );

	bool isOK = true;
	bool definitelyExists = false;
	bool isExisting = (ekey.m_iDBID != 0);
	FvDatabaseID dbID = ekey.m_iDBID;

	if (erec.IsStrmProvided())
	{
		FvXMLSectionPtr pOldProps;
		FvString oldName;
		if (isExisting)
		{
			IDMap::const_iterator iterData = m_kIDToData.find( dbID );
			if (iterData != m_kIDToData.end())
			{
				pOldProps = iterData->second;
				if (!nameProperty.empty())
					oldName = pOldProps->ReadString( nameProperty );
			}
			else
			{
				isOK = false;
			}
		}
		else
		{
			dbID = ++m_iMaxID;
		}

		FvXMLSectionPtr pProps = m_spDBSection->NewSection( desc.Name() );
		//! add by Uman, 20101120, 接口废弃, 因为属性的遍历顺序已改变
		FV_ASSERT(0 && "Code is abandoned!");
		//desc.ReadStreamToSection( erec.GetStrm(),
		//	FvEntityDescription::FV_BASE_DATA | FvEntityDescription::FV_CELL_DATA |
		//	FvEntityDescription::FV_ONLY_PERSISTENT_DATA, pProps );

		if (desc.HasCellScript())
		{
			FvVector3				position;
			FvDirection3			direction;
			FvSpaceID				spaceID;

			erec.GetStrm() >> position >> direction >> spaceID;
			pProps->WriteVector3( "position", position );
			pProps->WriteVector3( "direction", *(FvVector3 *)&direction );
			pProps->WriteInt( "spaceID", spaceID );
		}

		if (isOK && !nameProperty.empty())
		{
			NameMap& 	nameMap = m_kNameToIdMaps[ ekey.m_uiTypeID ];
			FvString newName = pProps->ReadString( nameProperty );

			if (!isExisting || (oldName != newName))
			{
				NameMap::const_iterator it = nameMap.find( newName );
				if (it == nameMap.end())
				{
					if (isExisting)
					 	nameMap.erase(oldName);
					nameMap[newName] = dbID;
				}
				else
				{
					FV_WARNING_MSG( "XMLDatabase::PutEntity: '%s' entity named"
						" '%s' already exists\n", desc.Name().c_str(),
						newName.c_str() );
					isOK = false;
				}
			}
		}

		if (isOK)
		{
			pProps->Write( "databaseID", dbID );

			if (isExisting)
				m_spDBSection->DelChild( pOldProps );

			m_kIDToData[dbID] = pProps;

			definitelyExists = true;
		}
		else
		{
			m_spDBSection->DelChild( pProps );
		}
	}

	if (isOK && erec.IsBaseMBProvided())
	{
		if (!definitelyExists)
		{
			isOK = (m_kIDToData.find( dbID ) != m_kIDToData.end());
		}

		if (isOK)
		{
			ActiveSet::iterator iter = m_kActiveSet.find( dbID );
			FvEntityMailBoxRef* pBaseMB = erec.GetBaseMB();

			if (pBaseMB)
			{
				if (iter != m_kActiveSet.end())
					iter->second.m_kBaseRef = *pBaseMB;
				else
					m_kActiveSet[ dbID ].m_kBaseRef = *pBaseMB;
			}
			else
			{
				if (iter != m_kActiveSet.end())
					m_kActiveSet.erase( iter );
			}
		}
	}

	handler.OnPutEntityComplete( isOK, dbID );
}

void XMLDatabase::DelEntity( const FvEntityDBKey & ekey,
							 FvIDatabase::IDelEntityHandler& handler )
{
	FvDatabaseID dbID = ekey.m_iDBID;
	if (dbID == 0)
		dbID = FindEntityByName( ekey.m_uiTypeID, ekey.m_kName );

	bool isOK = (dbID != 0);
	if (isOK)
	{
		isOK = this->DeleteEntity( dbID, ekey.m_uiTypeID );

		if (isOK)
		{
			ActiveSet::iterator afound = m_kActiveSet.find( dbID );
			if (afound != m_kActiveSet.end())
				m_kActiveSet.erase( afound );
		}
	}

	handler.OnDelEntityComplete(isOK);
}

bool XMLDatabase::Commit()
{
	if (m_spDBSection)
	{
		//return FvServerResource::Instance().Save(
		//		FvEntityDef::Constants::XmlDatabaseFile() );
	}

	return false;
}

bool XMLDatabase::DeleteEntity( FvDatabaseID id, FvEntityTypeID typeID )
{
	FV_ASSERT( m_spDBSection );

	IDMap::iterator dfound = m_kIDToData.find( id );
	if (dfound == m_kIDToData.end()) return false;
	FvXMLSectionPtr pSect = dfound->second;

	const FvString& nameProperty = m_pkEntityDefs->GetNameProperty( typeID );
	if (!nameProperty.empty())
	{
		FvString name = pSect->ReadString( nameProperty );
		m_kNameToIdMaps[ typeID ].erase( name );
	}

	m_kIDToData.erase( dfound );

	m_spDBSection->DelChild( pSect );

	return true;
}


FvDatabaseID XMLDatabase::FindEntityByName( FvEntityTypeID entityTypeID,
		const FvString & name ) const
{
	FV_ASSERT( m_spDBSection );
	const NameMap& nameMap = m_kNameToIdMaps[entityTypeID];
	NameMap::const_iterator it = nameMap.find( name );

	return (it != nameMap.end()) ?  it->second : 0;
}

void XMLDatabase::GetBaseAppMgrInitData(
		IGetBaseAppMgrInitDataHandler& handler )
{
	handler.OnGetBaseAppMgrInitDataComplete( 0, 0 );
}

void XMLDatabase::ExecuteRawCommand( const FvString & command,
	IExecuteRawCommandHandler& handler )
{
	//PyObject * pObj = Script::runString( command.c_str(), false );
	//if (pObj == NULL)
	//{
	//	handler.response() << FvString( "Exception occurred" );

	//	FV_ERROR_MSG( "XMLDatabase::ExecuteRawCommand: encountered exception\n" );
	//	PyErr_Print();
	//	handler.onExecuteRawCommandComplete();
	//	return;
	//}

	//BinaryOStream& stream = handler.response();
	//stream.appendString( "", 0 );	// No error
	//stream << FvInt32( 1 );			// 1 column
	//stream << FvInt32( 1 );			// 1 row

	//PyObject * pString = PyObject_Str( pObj );
	//const char * string = PyString_AsString( pString );
	//unsigned int sz = PyString_Size( pString );

	//stream.appendString( string, sz );

	//Py_DECREF( pObj );
	//Py_DECREF( pString );
	//handler.onExecuteRawCommandComplete();
}


void XMLDatabase::PutIDs( int count, const FvEntityID * ids )
{
	for (int i=0; i<count; i++)
		m_kSpareIDs.push_back( ids[i] );
}


void XMLDatabase::GetIDs( int count, IGetIDsHandler& handler )
{
	FvBinaryOStream& strm = handler.GetIDStrm();
	int counted = 0;
	for ( ; (counted < count) && m_kSpareIDs.size(); ++counted )
	{
		strm << m_kSpareIDs.back();
		m_kSpareIDs.pop_back();
	}
	for ( ; counted < count; ++counted )
	{
		strm << m_iNextID++;
	}

	handler.OnGetIDsComplete();
}


void XMLDatabase::WriteSpaceData( FvBinaryIStream& spaceData )
{
	spaceData.Finish();
}

bool XMLDatabase::GetSpacesData( FvBinaryOStream& strm )
{
	strm << int( 0 );

	return true;
}

void XMLDatabase::RestoreEntities( FvDBEntityRecoverer& recoverer )
{
	recoverer.Start();
}

void XMLDatabase::RemapEntityMailboxes( const FvNetAddress& srcAddr,
		const FvBackupHash & destAddrs )
{
	for ( ActiveSet::iterator iter = m_kActiveSet.begin();
			iter != m_kActiveSet.end(); ++iter )
	{
		if (iter->second.m_kBaseRef.m_kAddr == srcAddr)
		{
			const FvNetAddress& newAddr =
					destAddrs.AddressFor( iter->second.m_kBaseRef.m_iID );
			iter->second.m_kBaseRef.m_kAddr.m_uiIP = newAddr.m_uiIP;
			iter->second.m_kBaseRef.m_kAddr.m_uiPort = newAddr.m_uiPort;
		}
	}
}

void XMLDatabase::AddSecondaryDB( const SecondaryDBEntry& entry )
{
	FV_CRITICAL_MSG( "XMLDatabase::addSecondaryDb: Not implemented!" );
}

void XMLDatabase::UpdateSecondaryDBs( const FvBaseAppIDs& ids,
		IUpdateSecondaryDBshandler& handler )
{
	FV_CRITICAL_MSG( "XMLDatabase::UpdateSecondaryDBs: Not implemented!" );
	handler.OnUpdateSecondaryDBsComplete( SecondaryDBEntries() );
}

void XMLDatabase::GetSecondaryDBs( IGetSecondaryDBsHandler& handler )
{
	FV_CRITICAL_MSG( "XMLDatabase::getSecondaryDBs: Not implemented!" );
	handler.OnGetSecondaryDBsComplete( SecondaryDBEntries() );
}

FvUInt32 XMLDatabase::GetNumSecondaryDBs()
{
	return 0;
}

int XMLDatabase::ClearSecondaryDBs()
{
	// This always succeeds to simplify code from caller.
	// CRITICAL_MSG( "XMLDatabase::ClearSecondaryDBs: Not implemented!" );
	return 0;
}