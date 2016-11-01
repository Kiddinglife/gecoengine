#include "FvServerConfig.h"

#include <FvDebug.h>
#include <FvDebugFilter.h>

#include "FvServerResource.h"

//#include <sys/types.h>
//#include <pwd.h>

FV_DECLARE_DEBUG_COMPONENT( 0 )

namespace FvServerConfig
{

FV_SERVERCOMMON_API int g_iShouldDebug = 0;
FvString g_kBadFilename;

Container g_kSections;

bool g_bInited = false;

FvServerConfig::SearchIterator::SearchIterator():
		m_kIter( FvXMLSection::EndOfSearch() ),
		m_kChainTail(),
		m_kSearchName(),
		m_kCurrentConfigPath()
{
}

FvServerConfig::SearchIterator::SearchIterator( const FvString &kSearchName,
			const Container &kChain ):
		m_kIter( FvXMLSection::EndOfSearch() ),
		m_kChainTail(),
		m_kSearchName( kSearchName ),
		m_kCurrentConfigPath()
{
	for (Container::const_iterator iConfig = kChain.begin();
			iConfig != kChain.end();
			++iConfig)
	{
		m_kChainTail.push( *iConfig );
	}

	this->FindNext();
}

void FvServerConfig::SearchIterator::FindNext()
{
	do
	{
		if (m_kIter != FvXMLSection::EndOfSearch())
		{
			++m_kIter;
		}
		else if (!m_kChainTail.empty())
		{
			FvXMLSectionPtr nextConfigSection = m_kChainTail.front().first;
			m_kCurrentConfigPath = m_kChainTail.front().second;
			m_kChainTail.pop();
			m_kIter = nextConfigSection->BeginSearch( m_kSearchName );
		}
	} 
	while (!m_kChainTail.empty() && m_kIter == FvXMLSection::EndOfSearch());

	if (m_kChainTail.empty() && m_kIter == FvXMLSection::EndOfSearch())
	{
		m_kCurrentConfigPath = "";
	}
}

FvServerConfig::SearchIterator::SearchIterator( const SearchIterator &kCopy ):
		m_kIter( kCopy.m_kIter ),
		m_kChainTail( kCopy.m_kChainTail ),
		m_kSearchName( kCopy.m_kSearchName ),
		m_kCurrentConfigPath( kCopy.m_kCurrentConfigPath )
{
}

FvXMLSectionPtr FvServerConfig::SearchIterator::operator*()
{
	return *m_kIter;
}

SearchIterator & FvServerConfig::SearchIterator::operator++()
{
	this->FindNext();
	return *this;
}

SearchIterator FvServerConfig::SearchIterator::operator++( int )
{
	SearchIterator kSearchIteratorCopy( *this );
	this->operator++();
	return kSearchIteratorCopy;
}

SearchIterator & FvServerConfig::SearchIterator::operator=(
		const SearchIterator &kCopy )
{
	m_kIter = kCopy.m_kIter;
	m_kChainTail = kCopy.m_kChainTail;
	m_kSearchName = kCopy.m_kSearchName;
	m_kCurrentConfigPath = kCopy.m_kCurrentConfigPath;
	return *this;
}

FV_SERVERCOMMON_API bool IsBad()
{
	if (!g_kBadFilename.empty())
	{
		FvServerResource::OpenSection( g_kBadFilename );
		FV_ERROR_MSG( "Could not open configuration file %s\n",
				g_kBadFilename.c_str() );
		return true;
	}

	return false;
}

void Init()
{
	const char *BASE_FILENAME = "FutureVision.xml";

	g_bInited = true;
	g_kSections.clear();
	FvString filename = BASE_FILENAME;
	FvString kSvrCfgDir = "ServerConfig/";	//! add by Uman, 20100507

	bool isUserFilename = false;
	//struct passwd * pPasswordEntry = ::getpwuid( ::getUserId() );

	//if (pPasswordEntry && (pPasswordEntry->pw_name != NULL))
	//{
	//	filename = FvString( "fv_" ) +
	//		pPasswordEntry->pw_name + ".xml";
	//	isUserFilename = true;
	//}

	while (!filename.empty())
	{
		//! add by Uman, 20100507
		FvString kFileFullName = kSvrCfgDir + filename;
		FvServerResource::Instance().Purge( kFileFullName );
		FvXMLSectionPtr pDS = FvServerResource::OpenSection( kFileFullName );
		//! add end
		//FvServerResource::Instance().Purge( filename );
		//FvXMLSectionPtr pDS = FvServerResource::OpenSection( filename );

		if (pDS)
		{
			FV_INFO_MSG( "FvServerConfig::Init: Adding %s\n", filename.c_str() );
			g_kSections.push_back( std::make_pair( pDS, filename ) );
			filename = pDS->ReadString( "parentFile" );
		}
		else if (isUserFilename && !FvServerResource::FileExists( filename ))
		{
			filename = "FutureVision.xml";
		}
		else
		{
			if (FvServerResource::FileExists( filename ))
			{
				FV_ERROR_MSG( "Could not parse %s\n", filename.c_str() );
			}
			else
			{
				FV_ERROR_MSG( "Could not find %s\n", filename.c_str() );
			}

			g_kBadFilename = filename;
			filename = "";
		}

		isUserFilename = false;
	}

	Update( "debugConfigOptions", g_iShouldDebug );

	FvDebugFilter::Instance().FilterThreshold(
		FvServerConfig::Get( "outputFilterThreshold",
			FvDebugFilter::Instance().FilterThreshold() ) );

	FvDebugFilter::Instance().HasDevelopmentAssertions(
		FvServerConfig::Get( "hasDevelopmentAssertions",
			FvDebugFilter::Instance().HasDevelopmentAssertions() ) );
}

bool ProcessCommandLine( int argc, char * argv[] )
{
	if (!g_kSections.empty())
	{
		FvXMLSectionPtr pDS = g_kSections.front().first;

		for (int i = 0; i < argc - 1; ++i)
		{
			if (argv[i][0] == '+')
			{
				pDS->WriteString( argv[i]+1, argv[ i+1 ] );
				FV_INFO_MSG( "FvServerConfig::Init: "
						"Setting '%s' to '%s' from command line.\n",
					argv[i]+1, argv[ i+1 ] );

				++i;
			}
		}
	}
	else
	{
		FV_ERROR_MSG( "FvServerConfig::Init: No configuration section.\n" );
		return false;
	}

	return true;
}


FV_SERVERCOMMON_API bool Init( int argc, char * argv[] )
{
	if (!g_bInited)
	{
		Init();
	}
	else
	{
		FV_WARNING_MSG( "FvServerConfig::Init: "
				"Already initialised before parse args.\n" );
	}

	return ProcessCommandLine( argc, argv );
}

FV_SERVERCOMMON_API bool Hijack( Container & container, int argc, char * argv[] )
{
	g_bInited = true;
	g_kBadFilename.clear();
	g_kSections.swap( container );
	ProcessCommandLine( argc, argv );
	return true;
}

FvXMLSectionPtr FV_SERVERCOMMON_API GetSection( const char * path, FvXMLSectionPtr defaultValue )
{
	if (!g_bInited)
	{
		Init();
	}

	Container::iterator iter = g_kSections.begin();

	while (iter != g_kSections.end())
	{
		FvXMLSectionPtr pResult = iter->first->OpenSection( path );

		if (pResult)
		{
			if (g_iShouldDebug > 1)
			{
				FV_DEBUG_MSG( "FvServerConfig::GetSection: %s = %s (from %s)\n",
					path, pResult->AsString().c_str(),
					iter->second.c_str() );
			}

			return pResult;
		}

		++iter;
	}

	if (g_iShouldDebug > 1)
	{
		FV_DEBUG_MSG( "FvServerConfig::GetSection: %s was not set.\n", path );
	}

	return defaultValue;
}

FV_SERVERCOMMON_API SearchIterator BeginSearchSections( const char *pcPath )
{
	return SearchIterator( pcPath, g_kSections );
}

FV_SERVERCOMMON_API const SearchIterator & EndSearch()
{
	static SearchIterator s_kEnd;
	return s_kEnd;
}

void FV_SERVERCOMMON_API GetChildrenNames( std::vector< FvString >& childrenNames,
		const char * parentPath )
{
	for ( Container::iterator iter = g_kSections.begin();
			iter != g_kSections.end(); ++iter )
	{
		FvXMLSectionPtr pParent = iter->first->OpenSection( parentPath );

		if (pParent)
		{
			int numChildren = pParent->CountChildren();
			for ( int i = 0; i < numChildren; ++i )
			{
				childrenNames.push_back( pParent->ChildSectionName( i ) );
			}
		}
	}
}

FV_SERVERCOMMON_API FvString Get( const char * path, const char * defaultValue )
{
	FvString defaultValueStr( defaultValue );
	return Get( path, defaultValueStr );
}


FV_SERVERCOMMON_API void PrettyPrint( const char * path,
		const FvString & oldValue, const FvString & newValue,
		const char * typeStr, bool isHit )
{
#ifndef _WIN32
	const char * sectionName = basename( path );
#else
	const char * sectionName = path;
	const char * pCurr = path;
	while (*pCurr != '\0')
	{
		if (*pCurr == '/')
		{
			sectionName = pCurr + 1;
		}
		++pCurr;
	}
#endif

	FvString sorting = (sectionName == path) ? "aatoplevel/" : "";
	sorting += path;

	FvString changedValue = FvString( " " ) + newValue;

	if (oldValue == newValue)
	{
		changedValue = "";
	}

	FV_DEBUG_MSG( "FvServerConfig: %-40s <%s> %s </%s> "
				"<!-- Type: %s%s%s -->\n",
			sorting.c_str(),
			sectionName,
			oldValue.c_str(),
			sectionName,
			typeStr,
			changedValue.c_str(),
			isHit ? "" : " NOT READ" );
}

} 

