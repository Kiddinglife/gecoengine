//{future header message}
#ifndef __FvDocWatcher_H__
#define __FvDocWatcher_H__

#include "FvServerConfig.h"
#include <FvWatcher.h>

////////////#define FV_ENABLE_WATCHERS 1
#if FV_ENABLE_WATCHERS

#include <FvSmartPointer.h>
#include <FvXMLSection.h>

namespace FvWatcherDoc
{
	void InitWatcherDoc( const FvString &appName );
	void SetWatcherDoc( FvWatcherPtr pWatcher, const FvString & path );
}

#undef FV_WATCH
#undef FV_WATCH_REF

#define FV_INIT_WATCHER_DOC FvWatcherDoc::InitWatcherDoc

#ifndef _WIN32
#define FV_WATCH( PATH, ... )														\
	FvWatcherDoc::SetWatcherDoc( ::AddWatcher( PATH, __VA_ARGS__ ), PATH )

#define FV_WATCH_REF( PATH, ... )													\
	FvWatcherDoc::SetWatcherDoc( ::AddReferenceWatcher( PATH, __VA_ARGS__ ), PATH )

#else // !_WIN32

#define FV_WATCH AddWatcherWrap

template <class RETURN_TYPE, class OBJECT_TYPE, class DUMMY_TYPE>
void AddWatcherWrap( const char * path,
			OBJECT_TYPE & rObject,
			RETURN_TYPE (OBJECT_TYPE::*getMethod)() const,
			void (OBJECT_TYPE::*setMethod)(DUMMY_TYPE),
			const char * comment = NULL )
{
	FvWatcherDoc::SetWatcherDoc(
		::AddWatcher( path, rObject, getMethod, setMethod, comment ), path );
}

template <class RETURN_TYPE, class OBJECT_TYPE>	
void AddWatcherWrap( const char * path,
			OBJECT_TYPE & rObject,
			RETURN_TYPE (OBJECT_TYPE::*getMethod)() const,
			const char * comment = NULL )
{
	FvWatcherDoc::SetWatcherDoc(
		::AddWatcher( path, rObject, getMethod, comment ), path );
}

template <class RETURN_TYPE>
void AddWatcherWrap( const char * path,
			RETURN_TYPE (*getFunction)(),
			void (*setFunction)( RETURN_TYPE ) = NULL,
			const char * comment = NULL )
{
	FvWatcherDoc::SetWatcherDoc(
		::AddWatcher( path, getFunction, setFunction, comment ), path );
}

template <class TYPE>
void AddWatcherWrap( const char * path,
			TYPE & rValue,
			FvWatcher::Mode access = FvWatcher::WT_READ_WRITE,
			const char * comment = NULL )
{
	FvWatcherDoc::SetWatcherDoc(
		::AddWatcher( path, rValue, access, comment ), path );
}

#define FV_WATCH_REF AddReferenceWatcherWrap

template <class RETURN_TYPE, class OBJECT_TYPE>
void AddReferenceWatcherWrap( const char * path,
			OBJECT_TYPE & rObject,
			const RETURN_TYPE & (OBJECT_TYPE::*getMethod)() const,
			const char * comment = NULL )
{
	FvWatcherDoc::SetWatcherDoc(
		::AddReferenceWatcher( path, rObject, getMethod, comment ),
		path );
}

template <class RETURN_TYPE, class OBJECT_TYPE, class DUMMY_TYPE>
void AddReferenceWatcherWrap( const char * path,
			OBJECT_TYPE & rObject,
			const RETURN_TYPE & (OBJECT_TYPE::*getMethod)() const,
			void (OBJECT_TYPE::*setMethod)(DUMMY_TYPE),
			const char * comment = NULL )
{
	FvWatcherDoc::SetWatcherDoc(
		::AddReferenceWatcher( path, rObject, getMethod, setMethod, comment ),
		path );
}

#endif // _WIN32

#else // FV_ENABLE_WATCHERS

#define FV_INIT_WATCHER_DOC( x )

#endif // !FV_ENABLE_WATCHERS

#endif // __FvDocWatcher_H__
