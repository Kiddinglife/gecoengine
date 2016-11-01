#include "FvUserMap.h"
#include "FvCommonMachineGuard.h"
#include <FvSysLog.h>
#include <FvProcess.h>

FvUserMap::FvUserMap()
{
	this->QueryUserConfs();

	m_kNotFound.m_uiUID = m_kNotFound.UID_NOT_FOUND;
	m_kNotFound.OutGoing( true );
}

void FvUserMap::QueryUserConfs()
{
#ifndef _WIN32
	struct passwd *pEnt;
	while ((pEnt = getpwent()) != NULL)
	{
		FvNetUserMessage um;
		um.OutGoing( true );
		um.Init( *pEnt );

		if (this->GetEnv( um, pEnt ))
			this->Add( um );
	}
	endpwent();
#else // !_WIN32

#endif // _WIN32
}

void FvUserMap::Add( const FvNetUserMessage &um )
{
	m_kMap.insert( Map::value_type( um.m_uiUID, um ) );
}

FvNetUserMessage* FvUserMap::Add( struct passwd *ent )
{
	FvNetUserMessage newguy;
#ifndef _WIN32
	newguy.Init( *ent );
#endif // !_WIN32
	newguy.OutGoing( true );

	this->GetEnv( newguy );
	this->Add( newguy );

	return this->Fetch( newguy.m_uiUID );
}

FvNetUserMessage* FvUserMap::Fetch( FvUInt16 uid )
{
	Map::iterator it;
	if ((it = m_kMap.find( uid )) != m_kMap.end())
		return &it->second;
	else
		return NULL;
}

namespace
{

bool IsEmpty( const char *buf )
{
	while (*buf)
	{
		if (!isspace( *buf ))
			return false;
		++buf;
	}

	return true;
}

}


bool FvUserMap::GetEnv( FvNetUserMessage &um, struct passwd *pEnt )
{
	char buf[ 1024 ], acRoot[ 256 ], acResPath[ 1024 ];
	const char *filename = um.GetConfFilename();
	bool done = false;

	if (!pEnt && FvGetPWUID( um.m_uiUID ) == NULL)
	{
		FvSysLog( FV_SYSLOG_ERR, "Uid %d doesn't exist on this system!", um.m_uiUID );
		return false;
	}

	FILE * file;

#ifndef _WIN32
	if ((file = fopen( filename, "r" )) != NULL)
	{
		while (fgets( buf, sizeof(buf)-1, file ) != NULL)
		{
			if (buf[0] != '#' && buf[0] != 0)
			{
				if (sscanf( buf, "%[^;];%s", acRoot, acResPath ) == 2)
				{
					um.m_kRoot = acRoot;
					um.m_kResPath = acResPath;
					done = true;
					break;
				}
				else if (!::IsEmpty( buf ))
				{
					FvSysLog( FV_SYSLOG_ERR, "%s has invalid line '%s'",
						filename, buf );
				}
			}
		}
	}

	if (file != NULL) fclose( file );
	if (done) return true;

#else // !_WIN32

	if ((file = fopen( g_pcMachinedConfFile, "r" )) == NULL)
	{
		return false;
	}

	while (fgets( buf, sizeof(buf)-1, file ) != NULL)
	{
		if(buf[0] == '#' || buf[0] == 0)
			continue;

		if (buf[0] == '[')
		{
			break;
		}

		int file_uid;
		if (sscanf( buf, "%d;%[^;];%s", &file_uid, acRoot, acResPath ) == 3 &&
			file_uid == um.m_uiUID)
		{
			um.m_kRoot = acRoot;
			um.m_kResPath = acResPath;
			done = true;
			break;
		}
	}

	fclose( file );

	if (done)
		return true;
	else
		return false;
#endif // _WIN32
}

bool FvUserMap::SetEnv( const FvNetUserMessage &um )
{
#ifndef _WIN32
	FILE *file = fopen( um.GetConfFilename(), "w" );
	if (file == NULL)
		return false;

	fprintf( file, "%s;%s", um.m_kRoot.c_str(), um.m_kResPath.c_str() );
	fchown( fileno( file ), um.m_uiUID, (gid_t)-1 );
	fclose( file );
#else // !_WIN32

#endif // _WIN32
	return true;
}

void FvUserMap::Flush()
{
	m_kMap.clear();
	this->QueryUserConfs();
}
