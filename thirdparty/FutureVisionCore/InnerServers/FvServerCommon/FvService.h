//{future header message}
#ifndef __FvService_H__
#define __FvService_H__

#include <FvProcess.h>

#include "FvServerCommon.h"
#include "FvServerResource.h"
#include "FvServerConfig.h"

void FV_SERVERCOMMON_API ServiceParseCommandLine( int argc, char **argv );

#ifdef _WIN32
#ifndef FV_CONFIG
#define FV_CONFIG Unknown
#endif
#endif

#define START_MSG( name )													\
	FV_INFO_MSG( "---- %-10s "												\
			"Version: %s. "													\
			"Config: %s. "													\
			"Built: %s %s. "												\
			"UID: %d. "														\
			"PID: %d ----\n",												\
		name, "inner version",												\
		FV_CONFIG, __TIME__, __DATE__, 										\
		FvServerConfig::Get( "UserID", FvUInt16(1) ),						\
		FvGetPID() );

#define START_MSG_WITH_USERID( name, userid )								\
	FV_INFO_MSG( "---- %-10s "												\
	"Version: %s. "															\
	"Config: %s. "															\
	"Built: %s %s. "														\
	"UID: %d. "																\
	"PID: %d ----\n",														\
	name, "inner version",													\
	FV_CONFIG, __TIME__, __DATE__, 											\
	userid,																	\
	FvGetPID() );

#ifndef _WIN32  // WIN32PORT

#define FUTUREVISION_SERVICE_MAIN 							\
ServiceMain( int argc, char * argv[] );						\
int main( int argc, char * argv[] )							\
{															\
	FvServerResource kResource;								\
	FvServerResource::Init( argc, (const char **)argv );	\
	FvServerConfig::Init( argc, argv );						\
	ServiceParseCommandLine( argc, argv );					\
	return ServiceMain( argc, argv );						\
}															\
int ServiceMain

#define FUTUREVISION_SERVICE_MAIN_NO_RESMGR					\
ServiceMain( int argc, char * argv[] );						\
int main( int argc, char * argv[] )							\
{															\
	ServiceParseCommandLine( argc, argv );					\
	return ServiceMain( argc, argv );						\
}															\
int ServiceMain

#define FV_SERVICE_CHECK_POINT( MSECS )
#define FV_SERVICE_UPDATE_STATUS( STATE, WAIT_HINT, ERROR_CODE )

#else

#define FUTUREVISION_SERVICE_MAIN							\
RealServiceMain( int argc, char * argv[] );					\
int ServiceMain( int argc, char * argv[] )					\
{															\
	FvServerResource kResource;								\
	FvServerResource::Init( argc, argv );					\
	FvServerConfig::Init( argc, argv );						\
	ServiceParseCommandLine( argc, argv );					\
	return RealServiceMain( argc, argv );					\
}															\
int RealServiceMain

#define FUTUREVISION_SERVICE_MAIN_NO_RESMGR					\
RealServiceMain( int argc, char * argv[] );					\
int ServiceMain( int argc, char * argv[] )					\
{															\
	ServiceParseCommandLine( argc, argv );					\
	return RealServiceMain( argc, argv );					\
}															\
int RealServiceMain

#define FV_SERVICE_CHECK_POINT( MSECS )	ServiceCheckpoint( MSECS );
#define FV_SERVICE_UPDATE_STATUS( STATE, WAIT_HINT, ERROR_CODE )	\
	ServiceUpdateStatus( STATE, WAIT_HINT, ERROR_CODE );

#ifndef FV_SERVERCOMMON_EXPORT

#include "FvServiceBase.h"																	
																							
int ServiceMain( int argc, char * argv[] );													
void ServiceStop();																			
extern char szServiceDependencies[];														
			
class FutureVisionService : public FvServiceBase											
{																							
	protected:																				
																							
		HANDLE m_hStopEvent;																
		HANDLE m_hThread;																	
																							
		static DWORD WINAPI StopThreadProc( LPVOID pThis )									
			{																				
				DWORD ret = ( (FutureVisionService*)pThis )->StopThread();					
				ExitThread(ret);															
				return ret;																	
			}																				
																							
		DWORD StopThread()																	
			{																				
				WaitForSingleObject( m_hStopEvent, INFINITE );								
				onStop();																	
				return 0;																	
			}																				
																							
	public:																					
		FutureVisionService( LPTSTR pServiceName, LPTSTR pDisplayName ) :					
					FvServiceBase( pServiceName, pDisplayName ),							
					m_hStopEvent( NULL ),													
					m_hThread( NULL )														
			{																		
				SetControlsAccepted(
					GetControlsAccepted() | SERVICE_ACCEPT_SHUTDOWN );
			}											
																							
	   ~FutureVisionService()																
			{																				
				SetEvent( m_hStopEvent );													
				WaitForSingleObject( m_hThread, 1000 );										
				CloseHandle( m_hThread );													
				CloseHandle( m_hStopEvent );												
			}


		virtual void Main()
			{
				Checkpoint(3000);

				TCHAR szEvent[32];
				wsprintf( szEvent, TEXT("machined-proc%d"),
						(unsigned short ) GetCurrentProcessId() );

				m_hStopEvent = CreateEvent( NULL, TRUE, FALSE, szEvent );
				DWORD tid;
				m_hThread = CreateThread( NULL, 4096,
					(LPTHREAD_START_ROUTINE) StopThreadProc,
					this, 0, &tid );

				ServiceMain( (int) dwArgc, lpArgv );
				UpdateStatus( SERVICE_STOP_PENDING, 3000 );
			}

		virtual void onStop(void)
			{
				UpdateStatus( SERVICE_STOP_PENDING, 3000 );
				ServiceStop();
			}
		virtual void onShutdown(void) { onStop(); }

};

void PrintUsage()
{
	 printf( "\narguments:\n" );
	 printf( "  [serviceName]						to run the service\n");
	 printf( "  -install [serviceName] [DisplayName] to install the service\n");
	 printf( "  -remove  [serviceName]			   to remove the service\n" );
	 printf( "\n" );
}

int __cdecl main( int argc, char * argv[] )
{
	char *pcServiceName = NULL;
	char *pcDisplayName = NULL;

	bool bRun = false;
	bool bFromMachineD = false;

	if ( argc > 1 )
	{
		if ( _stricmp( "-machined", argv[1] ) == 0 )
		{
			bFromMachineD = true;
			bRun = true;
		}
		else if ( _stricmp( "-install", argv[1] ) == 0 )
		{
			if ( argc > 2 )
			{
				pcServiceName = argv[2];
			}
			else
			{
				// pcServiceName = argv[0];
				pcServiceName = "FutureVision";
			}

			if ( argc > 3 )
			{
				pcDisplayName = argv[3];
			}
			else
			{
				pcDisplayName = pcServiceName;
			}

			FutureVisionService svc( pcServiceName, pcDisplayName );
			svc.Install(  szServiceDependencies,
						  SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS
						);


			DWORD disp;
			HKEY hkey = NULL;
			char szBuf[MAX_PATH];
			wsprintf( szBuf, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", pcServiceName );
			if ( ERROR_SUCCESS == RegCreateKeyEx( HKEY_LOCAL_MACHINE,
												szBuf,
												0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL,
												&hkey, &disp ) &&
					hkey )
			{

				char szPath[512];
				DWORD length = GetModuleFileName( NULL, szPath, 512 );
				if ( length > 0 )
				{
					DWORD dwType = 7;
					if ( RegSetValueEx( hkey, "EventMessageFile", 0, REG_EXPAND_SZ, (const BYTE*)szPath, length ) ||
							RegSetValueEx( hkey, "TypesSupported", 0, REG_DWORD, (LPBYTE)&dwType, sizeof(dwType) ) )
					{
						printf( "Service was not able to add eventviewer registry entries\n" );
					}
				}

				RegCloseKey( hkey );
			}

		}
		else if ( _stricmp( "-remove", argv[1] ) == 0 )
		{
			if ( argc > 2 )
			{
				pcServiceName = argv[2];
			}
			else
			{
				// pcServiceName = argv[0];
				pcServiceName = "FutureVision";
			}

			FutureVisionService svc( pcServiceName, pcServiceName );
			svc.Remove();

			char szBuf[MAX_PATH];
			wsprintfA( szBuf, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", pcServiceName );
			RegDeleteKey( HKEY_LOCAL_MACHINE, szBuf );

		}
		else if ( ( _stricmp( "-?", argv[1] ) == 0 ) ||  
			( _stricmp( "-help", argv[1] ) == 0 )  ||  ( _stricmp( "--help", argv[1] ) == 0 ) )
		{
			PrintUsage();
		}
		else
		{
			bRun = true;
		}
	}
	else
	{
		bRun = true;
	}

	if ( bRun )
	{
		pcServiceName = argv[0];

		char szBuf[MAX_PATH];
		wsprintf( szBuf, "SYSTEM\\CurrentControlSet\\Services\\%s", pcServiceName );
		HKEY hkey = NULL;
		if ( ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szBuf,
												0, KEY_READ, &hkey ) ) && hkey )
		{
			DWORD type;
			DWORD size = sizeof(szBuf);
			if ( ERROR_SUCCESS == RegQueryValueEx(hkey, "DisplayName", NULL, &type, 
				(LPBYTE)szBuf, &size) && (type == REG_SZ) )
			{
				pcDisplayName = szBuf;
			}
			else
			{
				pcDisplayName = pcServiceName;
			}

			RegCloseKey( hkey );
		}

		{
			char szPath[MAX_PATH];
			DWORD len = GetModuleFileNameA( NULL, szPath, sizeof( szPath ) );
			if ( len )
			{
				while( len )
				{
					len--;
					if ( szPath[len] == '\\' )
					{
						szPath[len] = '\0';
						SetCurrentDirectory(szPath);
						break;
					}
				}
			}
		}


		FutureVisionService kSVC( pcServiceName, pcDisplayName );
		if ( bFromMachineD )
			kSVC.NeverService();
		kSVC.Start(argc, argv);

	}

	ExitProcess(0);
}

#endif // FV_SERVERCOMMON_EXPORT

#endif // _WIN32

#endif // __FvService_H__
