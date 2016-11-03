#include "FvDebug.h"
#include "FvBaseTypes.h"
#include "FvDebugFilter.h"
#include "FvConcurrency.h"

#if defined(FV_WIN32)
#include <windows.h>
#elif defined( PLAYSTATION3 )
#else // PLAYSTATION3
#include <unistd.h>
#include <syslog.h>
#endif // !FV_WIN32

FV_DECLARE_DEBUG_COMPONENT2( "FvKernel", 0 );

bool g_bShouldWriteToSyslog = false;
FvString g_kSyslogAppName;

#define FV_DEBUG_BUFSIZ 2048

static FV_THREADLOCAL( bool ) s_bIsCurrentMainThread( false );

FvMainThreadTracker::FvMainThreadTracker()
{
	s_bIsCurrentMainThread = true;
}

bool FvMainThreadTracker::IsCurrentThreadMain()
{
	return s_bIsCurrentMainThread;
}

static FvMainThreadTracker s_kMainThreadTracker;

FV_KERNEL_API const char * DebugMunge( const char *pcPath, const char *pcModule )
{
	static char	acStaticSpace[128];

	const char *pcResult = pcPath;

	const char *pcSeparator;

	pcSeparator = strrchr( pcResult, '\\' );
	if (pcSeparator != NULL)
	{
		pcResult = pcSeparator + 1;
	}

	pcSeparator = strrchr( pcResult, '/' );
	if (pcSeparator != NULL)
	{
		pcResult = pcSeparator + 1;
	}

	strcpy_s(acStaticSpace,128,"debug/");

	if (pcModule != NULL)
	{
		strcat_s( acStaticSpace,128, pcModule );
		strcat_s( acStaticSpace,128, "/" );
	}

	strcat_s(acStaticSpace,128,pcResult);
	return acStaticSpace;
}

void FvDebugMsgHelper::CriticalMessage( const char *pcFormat, ... )
{
	va_list kArgPtr;
	va_start( kArgPtr, pcFormat );
	this->CriticalMessageHelper( false, pcFormat, kArgPtr );
	va_end( kArgPtr );
}

void FvDebugMsgHelper::DevCriticalMessage( const char *pcFormat, ... )
{
	va_list kArgPtr;
	va_start( kArgPtr, pcFormat );
	this->CriticalMessageHelper( true, pcFormat, kArgPtr );
	va_end( kArgPtr );
}

void FvDebugMsgHelper::CriticalMessageHelper( bool bIsDevAssertion,
										   const char *pcFormat, va_list kArgPtr )
{
	char acBuffer[ FV_DEBUG_BUFSIZ * 2 ];

	FvVSNPrintf( acBuffer, sizeof(acBuffer), pcFormat, kArgPtr );
	acBuffer[sizeof(acBuffer)-1] = '\0';

#if FV_ENABLE_STACK_TRACKER
	if (StackTracker::stackSize() > 0)
	{
		std::string stack = StackTracker::buildReport();

		strcat( acBuffer, "\n" );
		strcat( acBuffer, "Stack trace: " );
		strcat( acBuffer, stack.c_str() );
		strcat( acBuffer, "\n" );
	}
#endif // FV_ENABLE_STACK_TRACKER

#if !defined( _WIN32 ) && !defined( PLAYSTATION3 )
	if (g_bShouldWriteToSyslog)
	{
		syslog( LOG_CRIT, "%s", acBuffer );
	}
#endif // (! _WIN32 && !PLAYSTATION3)

	this->Message( "%s", acBuffer );
	this->MessageBackTrace();

	if (bIsDevAssertion && !FvDebugFilter::Instance().HasDevelopmentAssertions())
	{
		return;
	}

	if (FvDebugFilter::Instance().GetCriticalCallbacks().size() != 0)
	{
		FvDebugFilter::CriticalCallbacks::const_iterator it =
			FvDebugFilter::Instance().GetCriticalCallbacks().begin();
		FvDebugFilter::CriticalCallbacks::const_iterator end =
			FvDebugFilter::Instance().GetCriticalCallbacks().end();

		for (; it!=end; ++it)
		{
			(*it)->HandleCritical( acBuffer );
		}
	}

#ifdef _XBOX360
	{
		OutputDebugString( acBuffer );

		LPCWSTR buttons[] = { L"Exit" };
		XOVERLAPPED         overlapped;					
		MESSAGEBOX_RESULT   result;

		ZeroMemory( &overlapped, sizeof( XOVERLAPPED ) );

		TCHAR tcbuffer[ FV_DEBUG_BUFSIZ * 2 ];
		WCHAR wcbuffer[ FV_DEBUG_BUFSIZ * 2 ];

		vsnprintf( tcbuffer, sizeof(tcbuffer), pcFormat, kArgPtr );
		tcbuffer[sizeof(tcbuffer)-1] = '\0';

		MultiByteToWideChar( CP_UTF8, 0, tcbuffer, -1, wcbuffer, ARRAYSIZE(wcbuffer) );

		DWORD dwRet = XShowMessageBoxUI( 0,
			L"Critical Error",					
			wcbuffer,							
			ARRAYSIZE(buttons),					
			buttons,							
			0,									
			XMB_ERRORICON,						
			&result,							
			&overlapped );

		while( !XHasOverlappedIoCompleted( &overlapped ) )
		{
			extern IDirect3DDevice9 *		g_pd3dDevice;
			g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );
			g_pd3dDevice->Present( 0, 0, NULL, 0 );
		}

		for( int i=0; i<60; i++ )
		{
			extern IDirect3DDevice9 *		g_pd3dDevice;
			g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );
			g_pd3dDevice->Present( 0, 0, NULL, 0 );
		}

		XLaunchNewImage( "", 0 );
	}
#elif defined ( PLAYSTATION3 )
	printf( acBuffer );
	printf( "\n" );
	FV_ENTER_DEBUGGER();
#elif defined(_WIN32)

#if FV_ENABLE_ENTER_DEBUGGER_MESSAGE
	strcat_s( acBuffer,FV_DEBUG_BUFSIZ * 2, "\nDo you want to enter debugger?\nSelect no will exit the program.\n" );

	_set_abort_behavior( 0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT );

	if( FvCriticalErrorHandler::Get() )
	{
		switch( FvCriticalErrorHandler::Get()->Ask( acBuffer ) )
		{
		case FvCriticalErrorHandler::ENTERDEBUGGER:
			FvCriticalErrorHandler::Get()->RecordInfo( false );
			FV_ENTER_DEBUGGER();
			break;
		case FvCriticalErrorHandler::EXITDIRECTLY:
			FvCriticalErrorHandler::Get()->RecordInfo( true );
			abort();
			break;
		}
	}
	else
		abort();
#else // FV_ENABLE_ENTER_DEBUGGER_MESSAGE
	::MessageBox( 0, acBuffer, "Critical Error Occured", MB_ICONHAND | MB_OK );
	abort();
#endif// FV_ENABLE_ENTER_DEBUGGER_MESSAGE

#else // defined(_WIN32)

	char	filename[512],	hostname[256];
	if (gethostname( hostname, sizeof(hostname) ) != 0)
		hostname[0] = 0;

	char exeName[512];
	const char * pExeName = "unknown";

	int len = readlink( "/proc/self/exe", exeName, sizeof(exeName) - 1 );
	if (len > 0)
	{
		exeName[ len ] = '\0';

		char * pTemp = strrchr( exeName, '/' );
		if (pTemp != NULL)
		{
			pExeName = pTemp + 1;
		}
	}

	FvSNPrintf( filename, sizeof(filename), "assert.%s.%s.%d.log", pExeName, hostname, getpid() );

	FILE * assertFile = fopen( filename, "a" );
	fprintf( assertFile, "%s", acBuffer );
	fclose( assertFile );

	*(int*)NULL = 0;
	typedef void(*BogusFunc)();
	((BogusFunc)NULL)();

#endif // defined(_WIN32)
}

namespace
{
	const char * const pcPrefixes[] =
	{
		"TRACE: ",
		"DEBUG: ",
		"INFO: ",
		"NOTICE: ",
		"WARNING: ",
		"ERROR: ",
		"CRITICAL: ",
		"HACK: ",
		NULL
	};
}

bool FvDebugMsgHelper::ms_bShowErrorDialogs = true;
FvSimpleMutex* FvDebugMsgHelper::ms_pkMutex = NULL;

void FvDebugMsgHelper::ShouldWriteToSyslog( bool bState )
{
	g_bShouldWriteToSyslog = bState;
}

void FvDebugMsgHelper::ShowErrorDialogs( bool bShow )
{
	if (!ms_pkMutex) ms_pkMutex = new FvSimpleMutex;
	ms_pkMutex->Grab();
	ms_bShowErrorDialogs = bShow;
	ms_pkMutex->Give();
}

bool FvDebugMsgHelper::ShowErrorDialogs()
{
	if (!ms_pkMutex) ms_pkMutex = new FvSimpleMutex;
	ms_pkMutex->Grab();
	bool bShowErrorDialogs = ms_bShowErrorDialogs;
	ms_pkMutex->Give();
	return bShowErrorDialogs;
}

void FvDebugMsgHelper::Fini()
{
	if (ms_pkMutex)
		delete ms_pkMutex;
	ms_pkMutex = NULL;

	FvDebugFilter::Fini();
}

void FvDebugMsgHelper::Message( const char *pcFormat, ... )
{
	bool bHandled = false;

	if (!FvDebugFilter::ShouldAccept( m_iComponentPriority, m_iMessagePriority ))
	{
		return;
	}

	va_list kArgPtr;
	va_start( kArgPtr, pcFormat );


#if !defined( _WIN32 ) && !defined( PLAYSTATION3 )
	if ((g_bShouldWriteToSyslog) &&
		( (m_iMessagePriority == FV_DEBUG_MESSAGE_PRIORITY_ERROR) ||
	      (m_iMessagePriority == FV_DEBUG_MESSAGE_PRIORITY_CRITICAL) ))
	{
		char acBuffer[ FV_DEBUG_BUFSIZ * 2 ];
		vsnprintf( acBuffer, sizeof(acBuffer), pcFormat, kArgPtr );
		buffer[sizeof(buffer)-1] = '\0';

		syslog( LOG_CRIT, "%s", acBuffer );
	}
#endif // (! _WIN32 && !PLAYSTATION3)

	FvDebugFilter::DebugCallbacks::const_iterator kIt =
		FvDebugFilter::Instance().GetMessageCallbacks().begin();
	FvDebugFilter::DebugCallbacks::const_iterator kEnd =
		FvDebugFilter::Instance().GetMessageCallbacks().end();

	for (; kIt!=kEnd; ++kIt)
	{
		if (!bHandled)
		{
			bHandled = (*kIt)->HandleMessage(
				m_iComponentPriority, m_iMessagePriority, pcFormat, kArgPtr );
		}
	}

	if (!bHandled)
	{
		if (0 <= m_iMessagePriority &&
			m_iMessagePriority < int(sizeof(pcPrefixes) / sizeof(pcPrefixes[0])) &&
			pcPrefixes[m_iMessagePriority] != NULL)
		{
			FvVDPrintfPriority( m_iComponentPriority, m_iMessagePriority,
					pcFormat, kArgPtr,
					pcPrefixes[m_iMessagePriority] );
		}
		else
		{
			FvVDPrintfPriority( m_iComponentPriority, m_iMessagePriority,
				pcFormat, kArgPtr, NULL );
		}
	}
	va_end( kArgPtr );
}


#ifdef unix
#define MAX_DEPTH 50
#include <execinfo.h>

void FvDebugMsgHelper::MessageBackTrace()
{
	void ** traceBuffer = new void*[MAX_DEPTH];
	FvUInt32 depth = backtrace( traceBuffer, MAX_DEPTH );
	char ** traceStringBuffer = backtrace_symbols( traceBuffer, depth );
	for (FvUInt32 i = 0; i < depth; i++)
	{
		this->message( "Stack: #%d %s\n", i, traceStringBuffer[i] );
	}
#ifdef ENABLE_MEMTRACKER
	raw_free( traceStringBuffer );
#else
	free( traceStringBuffer );
#endif
	delete[] traceBuffer;
}

#else // unix

void FvDebugMsgHelper::MessageBackTrace()
{
}

#endif // !unix

int g_iBackEndPID = 0;
int g_iBackEndUID = 0;
bool g_bIsBackEndProcess = false;

void InitBackEndProcess( int iUID, int uPID )
{
	g_bIsBackEndProcess = true;
	g_iBackEndPID = uPID;
	g_iBackEndUID = iUID;
}


char __scratch[] = "DebugLibTestString Tue Nov 29 11:54:35 EST 2005";

#if defined(_WIN32) && !defined(_XBOX)

static BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
	char acClassName[1024];
	GetClassName( hWnd, acClassName, sizeof( acClassName ) );
	if( strcmp( acClassName, "tooltips_class32" ) &&
		strcmp( acClassName, "#32770" ) )
	{
		DWORD iProcessID;
		GetWindowThreadProcessId( hWnd, &iProcessID );
		if( iProcessID == GetCurrentProcessId() )
		{
			*(HWND*)lParam = hWnd;
			return FALSE;
		}
	}
	return TRUE;
}

static HWND GetDefaultParent()
{
	HWND hWnd = NULL;
	EnumWindows( EnumWindowsProc, (LPARAM)&hWnd );
	return hWnd;
}

class FvDefaultCriticalErrorHandler : public FvCriticalErrorHandler
{
	virtual Result Ask( const char *pcMsg )
	{
		if( ::MessageBox( GetDefaultParent(), pcMsg, "Critical Error", MB_YESNO ) == IDYES )
			return ENTERDEBUGGER;

		return EXITDIRECTLY;
	}
	virtual void RecordInfo( bool bWillExit )
	{}
}
FvDefaultCriticalErrorHandler;

FvCriticalErrorHandler* FvCriticalErrorHandler::ms_pkHandler = &FvDefaultCriticalErrorHandler;

#else // (_WIN32 && !_XBOX)

FvCriticalErrorHandler* FvCriticalErrorHandler::ms_pkHandler;

#endif // !(_WIN32 && !_XBOX)

FvCriticalErrorHandler* FvCriticalErrorHandler::Get()
{
	return ms_pkHandler;
}

FvCriticalErrorHandler* FvCriticalErrorHandler::Set( FvCriticalErrorHandler *pkHandler )
{
	FvCriticalErrorHandler *pkOld = ms_pkHandler;
	ms_pkHandler = pkHandler;
	return pkOld;
}


#ifdef WIN32
#if FV_ENABLE_STACK_TRACKER

#define FV_EXCEPTION_CASE(e) case e: return #e

static const char* ExceptionCodeAsString(DWORD iCode)
{
	switch(iCode)
	{
	FV_EXCEPTION_CASE( EXCEPTION_ACCESS_VIOLATION );
	FV_EXCEPTION_CASE( EXCEPTION_INVALID_HANDLE );
	FV_EXCEPTION_CASE( EXCEPTION_STACK_OVERFLOW );
	FV_EXCEPTION_CASE( EXCEPTION_PRIV_INSTRUCTION );
	FV_EXCEPTION_CASE( EXCEPTION_ILLEGAL_INSTRUCTION );
	default:
		return "EXCEPTION";
	}
}

DWORD ExceptionFilter(DWORD iExceptionCode)
{
	FV_CRITICAL_MSG("The FutureVision Client has encountered an unhandled exception and must close (%s : 0x%08X)\n", 
		ExceptionCodeAsString(iExceptionCode), iExceptionCode);
	return EXCEPTION_CONTINUE_SEARCH;
}

#endif // FV_ENABLE_STACK_TRACKER
#endif // WIN32

