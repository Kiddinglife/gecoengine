#include "FvDBConfig.h"

#include <FvServerConfig.h>
#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT( 0 );

FvString FvDBConfigConnection::GenerateLockName() const
{
	FvString lockName( "FutureVision ");
	lockName += m_kDatabase;

	return lockName;
}

FvDBConfigServer::FvDBConfigServer()
	: m_kServerInfos( 1 )
{
	ServerInfo& primaryServer = m_kServerInfos.front();
	primaryServer.m_kConfigName = "<primary>";
	primaryServer.m_kConnectionInfo.m_kHost = "localhost";
	primaryServer.m_kConnectionInfo.m_uiPort = 3306;
	primaryServer.m_kConnectionInfo.m_kUsername = "futurevision";
	primaryServer.m_kConnectionInfo.m_kPassword = "futurevision";
	primaryServer.m_kConnectionInfo.m_kDatabase = "";

	ReadConnectionInfo( primaryServer.m_kConnectionInfo, "DBManager" );

	{
		std::vector< FvString > childrenNames;
		FvServerConfig::GetChildrenNames( childrenNames, "DBManager/backupDatabases" );

		m_kServerInfos.reserve( m_kServerInfos.size() + childrenNames.size() );
		const ServerInfo& primaryServer = m_kServerInfos.front();
		for ( std::vector< FvString >::const_iterator iter =
				childrenNames.begin(); iter != childrenNames.end(); ++iter )
		{
			m_kServerInfos.push_back( primaryServer );
			ServerInfo& backupServer = m_kServerInfos.back();
			backupServer.m_kConfigName = *iter;
			ReadConnectionInfo( backupServer.m_kConnectionInfo,
					"DBManager/backupDatabases/" + *iter );
		}
	}

	m_kCurServer = m_kServerInfos.begin();
}

void FvDBConfigServer::ReadConnectionInfo( FvDBConfigConnection& connectionInfo,
		const FvString& parentPath )
{
	FvServerConfig::Update( (parentPath + "/host").c_str(), connectionInfo.m_kHost );
	FvServerConfig::Update( (parentPath + "/port").c_str(), connectionInfo.m_uiPort );
	FvString username = FvServerConfig::Get( (parentPath + "/username").c_str() );
	FvString password = FvServerConfig::Get( (parentPath + "/password").c_str() );
	connectionInfo.m_kUsername = username;
	connectionInfo.m_kPassword = password;

	if ( !FvServerConfig::Update( (parentPath + "/databaseName").c_str(),
			connectionInfo.m_kDatabase ) )
	{
		if ( FvServerConfig::Update( (parentPath + "/name").c_str(),
				connectionInfo.m_kDatabase ) )
		{
			FV_WARNING_MSG( "FvDBConfigServer::ReadConnectionInfo: DBManager/name has been "
					"deprecated, use DBManager/databaseName instead.\n" );
		}
		else
		{
			FV_ERROR_MSG( "FvDBConfigServer::ReadConnectionInfo: DBManager/databaseName "
				   "has not been set.\n" );
		}
	}
}