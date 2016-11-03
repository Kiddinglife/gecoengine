//{future header message}
#ifndef __FvDBConfig_H__
#define __FvDBConfig_H__

#include <string>
#include <vector>

#ifdef FV_USE_MYSQL
#include "FvMySQLWrapper.h"
#endif

#include <FvBaseTypes.h>
#include <FvPowerDefines.h>

struct FvDBConfigConnection
{
	FvDBConfigConnection(): m_uiPort( 0 ) {}
	
	FvString m_kHost;
    unsigned int m_uiPort;
    FvString m_kUsername;
    FvString m_kPassword;
    FvString m_kDatabase;

    FvString GenerateLockName() const;
};

class FvDBConfigServer
{
public:
	struct ServerInfo
	{
		FvString m_kConfigName;
		FvDBConfigConnection m_kConnectionInfo;
	};

private:
	typedef std::vector<ServerInfo> ServerInfos;

	ServerInfos m_kServerInfos;
	ServerInfos::iterator m_kCurServer;

public:
	FvDBConfigServer();

	const ServerInfo &GetCurServer() const
	{
		return *m_kCurServer;
	}

	size_t GetNumServers() const
	{
		return m_kServerInfos.size();
	}

	bool GotoNextServer()
	{
		++m_kCurServer;
		if (m_kCurServer == m_kServerInfos.end())
			m_kCurServer = m_kServerInfos.begin();
		return (m_kCurServer != m_kServerInfos.begin());
	}

	void GotoPrimaryServer()
	{
		m_kCurServer = m_kServerInfos.begin();
	}

private:
	static void ReadConnectionInfo( FvDBConfigConnection& connectionInfo,
			const FvString& parentPath );
};

#endif // __FvDBConfig_H__
