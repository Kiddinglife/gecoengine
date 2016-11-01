#include "FvWatcher.h"
#include "FvDebug.h"

#ifndef FV_ENABLE_WATCHERS
//#define FV_ENABLE_WATCHERS 1
#endif // !FV_ENABLE_WATCHERS

#if FV_ENABLE_WATCHERS

#include "FvWatcher.h"
#include "FvWatcherPathRequest.h"

FvWatcher & FvWatcher::RootWatcher()
{
	static FvWatcherPtr pRoot = NULL;
	if (pRoot == NULL)
		pRoot = new FvDirectoryWatcher();

	return *pRoot;
}

void FvWatcher::Fini() 
{

}


bool FvWatcher::VisitChildren( const void * base, const char * path,
		FvWatcherVisitor & visitor )
{
	FvWatcherPathRequestWatcherVisitor wpr( visitor );
	return this->VisitChildren( base, path, wpr );
}

#include <assert.h>

#define SORTED_WATCHERS

#include "FvDebug.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvKernel", 0 )

FvDirectoryWatcher::FvDirectoryWatcher()
{
}

FvDirectoryWatcher::~FvDirectoryWatcher()
{
}


/*
 *	This method is an override from Watcher.
 */
bool FvDirectoryWatcher::GetAsString( const void * base,
								const char * path,
								FvString & result,
								FvString & desc,
								FvWatcher::Mode & mode ) const
{
	if (IsEmptyPath(path))
	{
		result = "<DIR>";
		mode = WT_DIRECTORY;
		desc = m_kComment;
		return true;
	}
	else
	{
		DirData * pChild = this->FindChild( path );

		if (pChild != NULL)
		{
			const void * addedBase = (const void*)(
				((const FvUIntPtr)base) + ((const FvUIntPtr)pChild->m_pkBase) );
			return pChild->m_spWatcher->GetAsString( addedBase, this->Tail( path ),
				result, desc, mode );
		}
		else
		{
			return false;
		}
	}
	// never gets here
}

bool FvDirectoryWatcher::GetAsStream( const void * base,
								const char * path,
								FvWatcherPathRequestV2 & pathRequest ) const
{
	if (IsEmptyPath(path))
	{
		FvWatcher::Mode mode = WT_DIRECTORY;
		WatcherValueToStream( pathRequest.GetResultStream(), "<DIR>", mode );
		pathRequest.SetResult( m_kComment, mode, this, base );
		return true;
	}
	else if (IsDocPath( path ))
	{
		FvWatcher::Mode mode = FvWatcher::WT_READ_ONLY;
		WatcherValueToStream( pathRequest.GetResultStream(), m_kComment, mode );
		pathRequest.SetResult( m_kComment, mode, this, base );
		return true;
	}
	else
	{
		DirData * pChild = this->FindChild( path );

		if (pChild != NULL)
		{
			const void * addedBase = (const void*)(
				((const FvUIntPtr)base) + ((const FvUIntPtr)pChild->m_pkBase) );
			return pChild->m_spWatcher->GetAsStream( addedBase, this->Tail( path ),
				pathRequest );
		}
		else
		{
			return false;
		}
	}
	// never gets here
}



/*
 *	Override from Watcher.
 */
bool FvDirectoryWatcher::SetFromString( void * base,
	const char * path, const char * value )
{
	DirData * pChild = this->FindChild( path );

	if (pChild != NULL)
	{
		void * addedBase = (void*)( ((FvUIntPtr)base) + ((FvUIntPtr)pChild->m_pkBase) );
		return pChild->m_spWatcher->SetFromString( addedBase, this->Tail( path ),
			value );
	}
	else
	{
		return false;
	}
}

bool FvDirectoryWatcher::SetFromStream( void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest )
{
	DirData * pChild = this->FindChild( path );

	if (pChild != NULL)
	{
		void * addedBase = (void*)( ((FvUIntPtr)base) + ((FvUIntPtr)pChild->m_pkBase) );
		return pChild->m_spWatcher->SetFromStream( addedBase, this->Tail( path ),
			pathRequest );
	}
	else
	{
		return false;
	}
}

bool FvDirectoryWatcher::VisitChildren( const void * base, const char * path,
	FvWatcherPathRequest & pathRequest ) 
{
	bool handled = false;

	pathRequest.AddWatcherCount( m_kContainer.size() );

	if (IsEmptyPath(path))
	{
		Container::iterator iter = m_kContainer.begin();

		while (iter != m_kContainer.end())
		{
			const void * addedBase = (const void*)(
				((const FvUIntPtr)base) + ((const FvUIntPtr)(*iter).m_pkBase) );

			if (!pathRequest.AddWatcherPath( addedBase, NULL,
									(*iter).m_kLabel, *(*iter).m_spWatcher ))
			{
				break;
			}

			iter++;
		}

		handled = true;
	}
	else
	{
		DirData * pChild = this->FindChild( path );

		if (pChild != NULL)
		{
			const void * addedBase = (const void*)(
				((const FvUIntPtr)base) + ((const FvUIntPtr)pChild->m_pkBase) );

			handled = pChild->m_spWatcher->VisitChildren( addedBase,
				this->Tail( path ), pathRequest );
		}
	}

	return handled;
}

bool FvDirectoryWatcher::AddChild( const char * path, FvWatcherPtr pChild,
	void * withBase )
{
	bool wasAdded = false;

	if (IsEmptyPath( path ))
	{
		FV_ERROR_MSG( "DirectoryWatcher::addChild: "
			"tried to add unnamed child (no trailing slashes please)\n" );
	}
	else if (strchr(path,'/') == NULL)
	{
		if (this->FindChild( path ) == NULL)
		{
			DirData		newDirData;
			newDirData.m_spWatcher = pChild;
			newDirData.m_pkBase = withBase;
			newDirData.m_kLabel = path;

#ifdef SORTED_WATCHERS
			Container::iterator iter = m_kContainer.begin();
			while ((iter != m_kContainer.end()) &&
					(iter->m_kLabel < newDirData.m_kLabel))
			{
				++iter;
			}
			m_kContainer.insert( iter, newDirData );
#else
			m_kContainer.push_back( newDirData );
#endif
			wasAdded = true;
		}
		else
		{
			FV_ERROR_MSG( "DirectoryWatcher::addChild: "
				"tried to replace existing watcher %s\n", path );
		}
	}
	else										
	{
		DirData * pFound = this->FindChild( path );

		if (pFound == NULL)
		{
			char * pSeparator = strchr( (char*)path, FV_WATCHER_SEPARATOR );

			int compareLength = (pSeparator == NULL) ?
				strlen( (char*)path ) : (pSeparator - path);

			FvWatcher * pChildWatcher = new FvDirectoryWatcher();

			DirData		newDirData;
			newDirData.m_spWatcher = pChildWatcher;
			newDirData.m_pkBase = NULL;
			newDirData.m_kLabel = FvString( path, compareLength );

#ifdef SORTED_WATCHERS
			Container::iterator iter = m_kContainer.begin();
			while ((iter != m_kContainer.end()) &&
					(iter->m_kLabel < newDirData.m_kLabel))
			{
				++iter;
			}
			pFound = &*m_kContainer.insert( iter, newDirData );
#else
			m_kContainer.push_back( newDirData );
			pFound = &m_kContainer.back();
#endif
		}

		if (pFound != NULL)
		{
			wasAdded = pFound->m_spWatcher->AddChild( this->Tail( path ),
				pChild, withBase );
		}
	}

	return wasAdded;
}

bool FvDirectoryWatcher::RemoveChild( const char * path )
{
	if (path == NULL)
		return false;

	char * pSeparator = strchr( (char*)path, FV_WATCHER_SEPARATOR );

	unsigned int compareLength =
		(pSeparator == NULL) ? strlen( path ) : (pSeparator - path);

	if (compareLength != 0)
	{
		Container::iterator iter = m_kContainer.begin();

		while (iter != m_kContainer.end())
        {
			if (compareLength == (*iter).m_kLabel.length() &&
				strncmp(path, (*iter).m_kLabel.data(), compareLength) == 0)
			{
				if( pSeparator == NULL )
				{
					m_kContainer.erase( iter );
					return true;
				}
				else
				{
					DirData* pChild = const_cast<DirData*>(&(const DirData &)*iter);
					return pChild->m_spWatcher->RemoveChild( Tail( path ) );
				}
			}
			iter++;
		}
	}

	return false;
}

FvDirectoryWatcher::DirData * FvDirectoryWatcher::FindChild( const char * path ) const
{
	if (path == NULL)
	{
		return NULL;
	}

	char * pSeparator = strchr( (char*)path, FV_WATCHER_SEPARATOR );

	unsigned int compareLength =
		(pSeparator == NULL) ? strlen( path ) : (pSeparator - path);

    DirData * pChild = NULL;

	if (compareLength != 0)
	{
		Container::const_iterator iter = m_kContainer.begin();

		while (iter != m_kContainer.end() && pChild == NULL)
        {
			if (compareLength == (*iter).m_kLabel.length() &&
				strncmp(path, (*iter).m_kLabel.data(), compareLength) == 0)
			{
				pChild = const_cast<DirData*>(&(const DirData &)*iter);
			}
			iter++;
		}
	}

	return pChild;
}

const char * FvDirectoryWatcher::Tail( const char * path )
{
	if (path == NULL)
		return NULL;

	char * pSeparator = strchr( (char*)path, FV_WATCHER_SEPARATOR );

	if (pSeparator == NULL)
		return NULL;

	return pSeparator + 1;
}

FvWatcher::FvWatcher( const char * comment ) :
	m_kComment( comment )
{
}

FvWatcher::~FvWatcher()
{
}

void FvWatcher::PartitionPath( const FvString path,
		FvString & resultingName,
		FvString & resultingDirectory )
{
	int pos = path.find_last_of( FV_WATCHER_SEPARATOR );

	if ( 0 <= pos && pos < (int)path.size( ) )
	{
		resultingDirectory	= path.substr( 0, pos + 1 );
		resultingName		= path.substr( pos + 1, path.length() - pos - 1 );
	}
	else
	{
		resultingName		= path;
		resultingDirectory	= "";
	}
}

void watchTypeCount( const char * basePrefix, const char * typeName, int & var )
{
	FvString str = basePrefix;
	str += typeName;
	AddWatcher( str.c_str(), var );
}

FvCallableWatcher::ArgDescription::ArgDescription(
		FvWatcherDataType type, const FvString & description ) :
	type_( type ),
	description_( description )
{
}

FvCallableWatcher::FvCallableWatcher( ExposeHint hint, const char * comment ) :
	FvWatcher( comment ),
	m_eExposeHint( hint )
{
}

void FvCallableWatcher::AddArg( FvWatcherDataType type, const char * description )
{
	m_kArgList.push_back( ArgDescription( type, description ) );
}

bool FvCallableWatcher::GetAsString( const void * base, const char * path,
	FvString & result, FvString & desc, FvWatcher::Mode & mode ) const
{
	if (IsEmptyPath( path ))
	{
		result = "Callable function";
		mode = WT_CALLABLE;
		desc = m_kComment;

		return true;
	}

	return false;
}

bool FvCallableWatcher::SetFromString( void * base, const char * path,
	const char * valueStr )
{
	FV_ERROR_MSG( "PyFunctionWatcher::setFromString: Attempt to call python "
			"function watcher with old protocol (v1) not supported\n" );
	return false;
}

bool FvCallableWatcher::IsArgsPath( const char * path )
{
	if (path == NULL)
		return false;

	return (strcmp(path, "__args__") == 0);
}

bool FvCallableWatcher::IsExposePath( const char * path )
{
	if (path == NULL)
		return false;

	return (strcmp(path, "__expose__") == 0);
}

bool FvCallableWatcher::GetAsStream( const void * base, const char * path,
	FvWatcherPathRequestV2 & pathRequest ) const
{
	if (IsEmptyPath( path ))
	{
		FvWatcher::Mode mode = FvWatcher::WT_CALLABLE;
		FvString value( "Callable function" );
		WatcherValueToStream( pathRequest.GetResultStream(),
			value, mode );
		pathRequest.SetResult( m_kComment, mode, this, base );
		return true;
	}
	else if (IsDocPath( path ))
	{
		// <watcher>/__doc__ handling
		FvWatcher::Mode mode = FvWatcher::WT_READ_ONLY;
		WatcherValueToStream( pathRequest.GetResultStream(), m_kComment, mode );
		pathRequest.SetResult( m_kComment, mode, this, base );
		return true;
	}
	else if (this->IsArgsPath( path ))
	{
		// <watcher>/__args__ handling
		int numArgs = m_kArgList.size();
		FvWatcher::Mode mode = WT_CALLABLE;

		FvBinaryOStream & resultStream = pathRequest.GetResultStream();

		resultStream << (unsigned char)FV_WATCHER_TYPE_TUPLE;
		resultStream << (unsigned char)mode;

		// In order to make decoding of the stream re-useable, we throw the
		// size of the next segment (ie: the count and the tuple types) on
		// before the count of the number of tuple elements. It seems like
		// a tiny bit of bloat but makes the decoding code a lot easier.

		FvMemoryOStream tmpResult;

		// Add the number of arguments for the function
		tmpResult.WriteStringLength( numArgs );

		// Now for each argument add the name / type
		for (int i = 0; i < numArgs; i++)
		{
			// tuple
			tmpResult << (unsigned char)FV_WATCHER_TYPE_TUPLE;
			// mode
			tmpResult << (unsigned char)FvWatcher::WT_READ_ONLY;

			// Create the contents of the argument element tuple
			FvMemoryOStream argStream;
			// count = 2
			argStream.WriteStringLength( 2 ); // Arg name + type
			WatcherValueToStream( argStream, m_kArgList[i].description(),
									FvWatcher::WT_READ_ONLY );
			WatcherValueToStream( argStream, m_kArgList[i].type(),
									FvWatcher::WT_READ_ONLY );

			// size
			tmpResult.WriteStringLength( argStream.Size() );
			tmpResult.AddBlob( argStream.Data(), argStream.Size() );
		}

		// Prefix the entire argument description with the desc size.
		resultStream.WriteStringLength( tmpResult.Size() );
		resultStream.AddBlob( tmpResult.Data(), tmpResult.Size() );
		pathRequest.SetResult( m_kComment, mode, this, base );

		return true;
	}
	else if (this->IsExposePath( path ))
	{
		FvWatcher::Mode mode = FvWatcher::WT_READ_ONLY;
		WatcherValueToStream( pathRequest.GetResultStream(),
				(FvInt32)m_eExposeHint, mode );
		pathRequest.SetResult( m_kComment, mode, this, base );
		return true;
	}

	return false;
}

FvNoArgCallableWatcher::FvNoArgCallableWatcher( Function func,
		ExposeHint hint, const char * comment ) :
	FvCallableWatcher( hint, comment ),
	m_pfFunc( func )
{
}

bool FvNoArgCallableWatcher::SetFromStream( void * base,
		const char * path,
		FvWatcherPathRequestV2 & pathRequest )
{
	// TODO: Should check that there are not too many arguments.
	FvString value = "No value";
	FvString output = "";

	if ((*m_pfFunc)( output, value ))
	{
		FvBinaryOStream & resultStream = pathRequest.GetResultStream();
		resultStream << (FvUInt8)FV_WATCHER_TYPE_TUPLE;
		resultStream << (FvUInt8)WT_READ_ONLY;

		const int TUPLE_COUNT = 2;

		int resultSize = FvBinaryOStream::CalculatePackedIntSize( TUPLE_COUNT ) +
			this->TupleElementStreamSize( output.size() ) +
			this->TupleElementStreamSize( value.size() );

		resultStream.WriteStringLength( resultSize );

		resultStream.WriteStringLength( TUPLE_COUNT );

		WatcherValueToStream( resultStream, output, WT_READ_ONLY );
		WatcherValueToStream( resultStream, value, WT_READ_ONLY );

		pathRequest.SetResult( "", WT_READ_ONLY, this, base );

		return true;
	}
	else
	{
		FV_WARNING_MSG( "NoArgCallableWatcher::setFromStream: "
				"Function call failed\n" );
		return false;
	}
}

FvDereferenceWatcher::FvDereferenceWatcher( FvWatcherPtr watcher, void * withBase ) :
	m_spWatcher( watcher ),
	m_uiSB( (FvUIntPtr)withBase )
{ }

bool FvDereferenceWatcher::GetAsString( const void * base, const char * path,
									  FvString & result, FvString & desc,
									  FvWatcher::Mode & mode ) const
{ 
	return (base == NULL) ? false :
		m_spWatcher->GetAsString( (void*)(m_uiSB + Dereference(base)), path, 
							   result, desc, mode ); 
}

bool FvDereferenceWatcher::SetFromString( void * base, const char * path,
										const char * valueStr )
{ 
	return (base == NULL) ? false :
		m_spWatcher->SetFromString( (void*)(m_uiSB + Dereference(base)), path, 
								 valueStr ); 
}

bool FvDereferenceWatcher::GetAsStream( const void * base, const char * path,
									  FvWatcherPathRequestV2 & pathRequest ) const
{ 
	return (base == NULL) ? false :
		m_spWatcher->GetAsStream( (void*)(m_uiSB + Dereference(base)), path, 
							   pathRequest ); 
}

bool FvDereferenceWatcher::SetFromStream( void * base, const char * path,
										FvWatcherPathRequestV2 & pathRequest )
{
	return (base == NULL) ? false :
		m_spWatcher->SetFromStream( (void*)(m_uiSB + Dereference(base)), path, 
								 pathRequest ); 
}

bool FvDereferenceWatcher::VisitChildren( const void * base, const char * path,
										FvWatcherPathRequest & pathRequest ) 
{
	return (base == NULL) ? false :
		m_spWatcher->VisitChildren( (void*)(m_uiSB + Dereference(base)), path, 
								 pathRequest ); 
}


bool FvDereferenceWatcher::AddChild( const char * path, FvWatcherPtr pChild,
								   void * withBase )
{ 
	return m_spWatcher->AddChild( path, pChild, withBase ); 
}

#endif // FV_ENABLE_WATCHERS
