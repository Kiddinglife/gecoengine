#ifdef _WIN32

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <windows.h>
#include <cassert>
#include "FvServiceBase.h"

#define ASSERT assert

FvServiceBase* g_pService = NULL;

#ifdef __cplusplus
extern "C" {
#endif

BOOL FV_SERVERCOMMON_API ServiceIsStopping()
{
    if ( g_pService )
        return g_pService->isStopping();
    else
        return TRUE;
}

DWORD FV_SERVERCOMMON_API ServiceCheckpoint( DWORD dwWaitHint )
{
    if ( g_pService )
        return g_pService->Checkpoint( dwWaitHint );
    else
        return NO_ERROR;
}

DWORD FV_SERVERCOMMON_API ServiceUpdateStatus(
     DWORD dwState,
     DWORD dwWaitHint,
     DWORD dwError )
{
    if ( g_pService )
        return g_pService->UpdateStatus( dwState, dwWaitHint, dwError );
    else
        return NO_ERROR;
}

#ifdef __cplusplus
}
#endif

#define TRACE_SERVICE 0
#if  TRACE_SERVICE
HANDLE g_hFile = INVALID_HANDLE_VALUE;
LPTSTR g_pServiceName = NULL;
#endif // TRACE_SERVICE

FvServiceBase::FvServiceBase( LPTSTR pServiceName, LPTSTR pServiceDisplayName /* = NULL */, DWORD svcType /*= SERVICE_WIN32_OWN_PROCESS*/ ) :
    m_isService( FALSE ), m_isInteractive( FALSE ), m_neverService( FALSE ),
    m_StatusHandle( 0 ), m_dwControlsAccepted( SERVICE_ACCEPT_STOP ),
    dwArgc(0), lpArgv(NULL), m_pDispatchTable( NULL )
{
    ASSERT( pServiceName );
    m_pServiceName = (LPTSTR) HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR)*(lstrlen(pServiceName)+1) );
    lstrcpy( m_pServiceName, pServiceName );

    if ( pServiceDisplayName != NULL )
    {
        m_pServiceDisplayName = (LPTSTR) HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR)*(lstrlen(pServiceDisplayName)+1) );
        lstrcpy( m_pServiceDisplayName, pServiceDisplayName );
    }
    else
    {
        m_pServiceDisplayName = m_pServiceName;
    }

    m_Status.dwServiceType = svcType;
    m_Status.dwCurrentState = SERVICE_START_PENDING;
    m_Status.dwControlsAccepted = 0;
    m_Status.dwWin32ExitCode = NO_ERROR;
    m_Status.dwServiceSpecificExitCode = 0;
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 3000;

#if TRACE_SERVICE
    if ( g_hFile == INVALID_HANDLE_VALUE )
    {
        g_pServiceName = m_pServiceName;

        TCHAR szFileName[100];
        wsprintf( szFileName, TEXT("\\%s.log"), m_pServiceName );

        g_hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL  );
        if ( g_hFile != INVALID_HANDLE_VALUE )
            SetFilePointer( g_hFile, 0, NULL, FILE_END );
    }
#endif // TRACE_SERVICE

}

FvServiceBase::~FvServiceBase()
{
#if TRACE_SERVICE
    if( ( g_hFile != INVALID_HANDLE_VALUE ) &&
        ( g_pServiceName == m_pServiceName ) )
        CloseHandle( g_hFile );
#endif // TRACE_SERVICE

    if ( m_pServiceDisplayName != m_pServiceName )
        HeapFree( GetProcessHeap(), 0, m_pServiceDisplayName );

    HeapFree( GetProcessHeap(), 0, m_pServiceName );
}

DWORD FvServiceBase::Checkpoint( DWORD dwWaitHint /* = 0 */ )
{
    if ( m_isService )
    {

        if ( dwWaitHint != 0 )
            m_Status.dwWaitHint = dwWaitHint;

        m_Status.dwCheckPoint++;

        if ( !::SetServiceStatus(m_StatusHandle, &m_Status) ) 
            return ::GetLastError();
    }

    return(NO_ERROR);

}

DWORD FvServiceBase::UpdateStatus( DWORD dwState /* = SERVICE_START_PENDING */,
                              DWORD dwWaitHint /* = 0 */, 
                              DWORD dwError /* = NO_ERROR */ )
{

    ASSERT ( dwError == NO_ERROR ||
             ( ( dwState == SERVICE_STOP_PENDING ) || 
               ( dwState == SERVICE_STOPPED ) ) &&
             "Service reported an error, but was not stopping!" );

    if ( dwState == SERVICE_START_PENDING )
        m_Status.dwControlsAccepted = 0;
    else
        m_Status.dwControlsAccepted = m_dwControlsAccepted;

    if ( ( m_Status.dwCurrentState != SERVICE_STOP_PENDING ) &&
         ( dwState == SERVICE_STOP_PENDING ) )
    {
        m_Status.dwCheckPoint = (DWORD) -1;
    }
    m_Status.dwCurrentState = dwState;

    m_Status.dwWaitHint = dwWaitHint; 

    if ( m_Status.dwWin32ExitCode == NO_ERROR )
        m_Status.dwWin32ExitCode = dwError;

    if ( ( dwState == SERVICE_RUNNING ) ||
         ( dwState == SERVICE_STOPPED ) )
    {
        m_Status.dwCheckPoint = (DWORD) -1; 
    }

    return Checkpoint();

}

DWORD FvServiceBase::Start( int argc, char** argv )
{
    ASSERT( g_pService == NULL );
    g_pService = this;

    DWORD dwError = NO_ERROR;

#if TRACE_SERVICE
    {
        char szBuf[256];
        DWORD cbWritten;
        wsprintf( szBuf, "FvServiceBase::Start\n" );
        WriteFile( g_hFile, szBuf, lstrlen( szBuf ), &cbWritten, NULL );
    }
#endif // TRACE_SERVICE

    dwError = CheckSIDs();
    if ( dwError == ERROR_CALL_NOT_IMPLEMENTED )  
        m_isInteractive = TRUE;

    if ( m_isInteractive )
    {
        ::SetConsoleCtrlHandler( control_handler, TRUE );
    }

	if ( m_isService )
    {
		SERVICE_TABLE_ENTRY dispatchTable[] =
        {
            { m_pServiceName, (LPSERVICE_MAIN_FUNCTION)service_main },
            { NULL, NULL }
        };

		if ( !m_pDispatchTable )
        {
            m_pDispatchTable = dispatchTable;
        }

		if ( !::StartServiceCtrlDispatcher(m_pDispatchTable) )
            return ::GetLastError();
	}
    else
    {
#ifdef UNICODE
        lpArgv = ::CommandLineToArgvW(::GetCommandLineW(), &(dwArgc) );
#else
        dwArgc = argc;
        lpArgv = argv;
#endif

        Main();
	}

    ASSERT( g_pService );
    g_pService = NULL;

    return NO_ERROR;
}


BOOL FvServiceBase::SetDispatchEntries( SERVICE_TABLE_ENTRY* pDispatch )
{
    if ( m_pDispatchTable )
        return FALSE;
    else
        m_pDispatchTable = pDispatch;
    return TRUE;
}

void FvServiceBase::StartWithExternalDispatch( DWORD dwArgc, LPTSTR* lpArgv)
{
    ASSERT( g_pService == NULL );
	ASSERT( m_neverService == FALSE );
    g_pService = this;

    m_isService = TRUE;
    FvServiceBase::service_main( dwArgc, lpArgv );
}


void FvServiceBase::RegisterExternalHandler( LPHANDLER_FUNCTION handler )
{
    m_StatusHandle = ::RegisterServiceCtrlHandler( GetName(), handler);
}

void WINAPI FvServiceBase::service_main( DWORD dwArgc, LPTSTR* lpArgv )
{
    ASSERT( g_pService );
    ASSERT( g_pService->m_isService );

#if TRACE_SERVICE
    {
        char szBuf[256];
        DWORD cbWritten;
        wsprintf( szBuf, "Beginning FvServiceBase::service_main %s\n", g_pService->m_pServiceName  );
        WriteFile( g_hFile, szBuf, lstrlen( szBuf ), &cbWritten, NULL );
    }
#endif // TRACE_SERVICE

    g_pService->m_StatusHandle = ::RegisterServiceCtrlHandler( g_pService->m_pServiceName,
                                                               FvServiceBase::service_ctrl);
    if ( !g_pService->m_StatusHandle )
        return;

#if TRACE_SERVICE
    {
        char szBuf[256];
        DWORD cbWritten;
        wsprintf( szBuf, "Service successfully registered\n" );
        WriteFile( g_hFile, szBuf, lstrlen( szBuf ), &cbWritten, NULL );
    }
#endif // TRACE_SERVICE

    g_pService->Checkpoint( 3000 );

    g_pService->dwArgc = dwArgc;
    g_pService->lpArgv = lpArgv;
    g_pService->Main();

    g_pService->UpdateStatus( SERVICE_STOPPED );

#if TRACE_SERVICE
    {
        char szBuf[256];
        DWORD cbWritten;
        wsprintf( szBuf, "Ending FvServiceBase::service_main\n" );
        WriteFile( g_hFile, szBuf, lstrlen( szBuf ), &cbWritten, NULL );
    }
#endif // TRACE_SERVICE

}

void WINAPI FvServiceBase::service_ctrl( DWORD dwCtrlCode )
{
    ASSERT( g_pService );
    g_pService->ServiceCtrl(dwCtrlCode);
}

void WINAPI FvServiceBase::ServiceCtrl( DWORD dwCtrlCode )

{

    switch( dwCtrlCode )
    {
        case SERVICE_CONTROL_STOP:
            onStop();
            break;

        case SERVICE_CONTROL_PAUSE:
            onPause();
            break;

        case SERVICE_CONTROL_CONTINUE:
            onContinue();
            break;

        case SERVICE_CONTROL_INTERROGATE:
            onInterrogate();
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            onShutdown();
            break;

        default:
            onUserControl( dwCtrlCode );
            break;
    }

    ::SetServiceStatus(m_StatusHandle, &(m_Status));
}

DWORD FvServiceBase::Install(LPTSTR pDependencies /* = NULL */,
                        DWORD  svcType /* = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS */,
                        LPTSTR pszPath /*= NULL*/ )
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;

    TCHAR szPath[512];

    SetLastError( NO_ERROR );

    if ( pszPath == NULL )
    {
        if ( GetModuleFileName( NULL, szPath, 512 ) == 0 )
        {
            TCHAR szBuf[256];
            DWORD bytes;
            wsprintf( szBuf, TEXT("Unable to install %s - 0x%x\n"), m_pServiceDisplayName, GetLastError() );
            WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );
            return ::GetLastError();
        }
        pszPath = szPath;
    }

    schSCManager = OpenSCManager(
                        NULL,                   // machine (NULL == local)
                        NULL,                   // database (NULL == default)
                        SC_MANAGER_ALL_ACCESS   // access required
                        );
    if ( schSCManager )
    {
        LPTSTR psz = pDependencies;
        while ( psz && *psz )
        {
            if ( *psz == TEXT('+') )
                *psz = SC_GROUP_IDENTIFIER;
            else if ( *psz == TEXT('~') )
                *psz = TEXT('\0');
            psz++;
        }

        schService = CreateService(
            schSCManager,               // SCManager database
            m_pServiceName,             // name of service
            m_pServiceDisplayName,      // name to display
            SERVICE_ALL_ACCESS,         // desired access
            svcType,                    // service type
            SERVICE_AUTO_START,         // start type
            SERVICE_ERROR_NORMAL,       // error control type
            pszPath,                    // service's binary
            NULL,                       // no load ordering group
            NULL,                       // no tag identifier
            pDependencies,              // dependencies
            NULL,                       // LocalSystem account
            NULL);                      // no password

        if ( schService )
        {
			SERVICE_DESCRIPTION desc;
			desc.lpDescription = "FutureVision Daemon";
			ChangeServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, &desc );

            TCHAR szBuf[256];
            DWORD bytes;

			int r = StartService( schService, 0, NULL );
			if ( r != 0 )
			{
	            wsprintf( szBuf, TEXT("%s installed and started.\n"), m_pServiceDisplayName  );
			}
			else
			{
	            wsprintf( szBuf, TEXT("%s installed but startup failed.\n"), m_pServiceDisplayName  );
			}

            WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );

			CloseServiceHandle(schService);
        }
        else
        {
            TCHAR szBuf[256];
            DWORD bytes;
            wsprintf( szBuf, TEXT("CreateService failed - 0x%x\n"), GetLastError() );
            WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );
        }

        CloseServiceHandle(schSCManager);
    }
    else
    {
        TCHAR szBuf[256];
        DWORD bytes;
        wsprintf( szBuf, TEXT("OpenSCManager failed - 0x%x\n"), GetLastError() );
        WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );
    }
    return ::GetLastError();
}

DWORD FvServiceBase::Remove()
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;
    TCHAR szBuf[256];
    DWORD bytes;

    SetLastError(NO_ERROR);

    schSCManager = OpenSCManager(
                        NULL,                   // machine (NULL == local)
                        NULL,                   // database (NULL == default)
                        SC_MANAGER_ALL_ACCESS   // access required
                        );
    if ( schSCManager )
    {
        schService = OpenService(schSCManager, m_pServiceName, SERVICE_ALL_ACCESS);

        if (schService)
        {
            if ( ControlService( schService, SERVICE_CONTROL_STOP, &m_Status ) )
            {

                wsprintf( szBuf, TEXT("Stopping %s."), m_pServiceDisplayName  );
                WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );
                Sleep( 500 );

                while( QueryServiceStatus( schService, &m_Status ) )
                {
                    if ( m_Status.dwCurrentState == SERVICE_STOP_PENDING )
                    {
                        lstrcpy( szBuf, TEXT(".") );
                        WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );
                        Sleep( 500 );
                    }
                    else
                        break;
                }

                if ( m_Status.dwCurrentState == SERVICE_STOPPED )
                    wsprintf( szBuf, TEXT("\n%s stopped.\n"), m_pServiceDisplayName );
                else
                    wsprintf( szBuf, TEXT("\n%s failed to stop.\n"), m_pServiceDisplayName );

                WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );

            }

            if( DeleteService(schService) )
                wsprintf( szBuf, TEXT("%s removed.\n"), m_pServiceDisplayName );
            else
                wsprintf( szBuf, TEXT("DeleteService failed - 0x%x\n"), GetLastError());

            WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );

            CloseServiceHandle(schService);
        }
        else
        {
            wsprintf( szBuf, TEXT("OpenService failed - 0x%x\n"), GetLastError());
            WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );
        }
        CloseServiceHandle(schSCManager);
    }
    else
    {
        wsprintf( szBuf, TEXT("OpenSCManager failed - 0x%x\n"), GetLastError());
        WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );
    }
    return ::GetLastError();

}

DWORD FvServiceBase::CheckSIDs(void)
{    
    HANDLE hProcessToken = NULL;
    DWORD groupLength = 50;
    PTOKEN_GROUPS groupInfo = NULL;

    SID_IDENTIFIER_AUTHORITY siaNt = SECURITY_NT_AUTHORITY;
    PSID pInteractiveSid = NULL;
    PSID pServiceSid = NULL;

    DWORD dwRet = NO_ERROR;

    DWORD ndx;

    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hProcessToken))
    {
        dwRet = ::GetLastError();
        goto closedown;
    }

    groupInfo = (PTOKEN_GROUPS)::LocalAlloc(0, groupLength);
    if (groupInfo == NULL)
    {
        dwRet = ::GetLastError();
        goto closedown;
    }

    if (!::GetTokenInformation(hProcessToken, TokenGroups, groupInfo, groupLength, &groupLength))
    {
        if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            dwRet = ::GetLastError();
            goto closedown;
        }

        ::LocalFree(groupInfo);

        groupInfo = (PTOKEN_GROUPS)::LocalAlloc(0, groupLength);
        if (groupInfo == NULL)
        {
            dwRet = ::GetLastError();
            goto closedown;
        }

        if (!GetTokenInformation(hProcessToken, TokenGroups, groupInfo, groupLength, &groupLength)) 
        {
            dwRet = ::GetLastError();
            goto closedown;
        }
    }

    if (!AllocateAndInitializeSid(&siaNt, 1, SECURITY_INTERACTIVE_RID, 0, 0, 0, 0, 0, 0, 0, &pInteractiveSid))
    {
        dwRet = ::GetLastError();
        goto closedown;
    }

    if (!AllocateAndInitializeSid(&siaNt, 1, SECURITY_SERVICE_RID, 0, 0, 0, 0, 0, 0, 0, &pServiceSid))
    {
        dwRet = ::GetLastError();
        goto closedown;
    }

    m_isInteractive = FALSE;
    m_isService = FALSE;

    for (ndx = 0; ndx < groupInfo->GroupCount ; ndx += 1)
    {
        SID_AND_ATTRIBUTES sanda = groupInfo->Groups[ndx];
        PSID pSid = sanda.Sid;

        if (::EqualSid(pSid, pInteractiveSid))
        {
            m_isInteractive = TRUE;
            m_isService = FALSE;
            break;
        }
        else if (::EqualSid(pSid, pServiceSid))
        {
			ASSERT( m_neverService == FALSE );
            m_isService = TRUE;
            m_isInteractive = FALSE;
            break;
        }
    }

    if ( !( m_isService || m_isInteractive ) )
    {
        m_isService = !m_neverService;

        HKEY  hkey = NULL;

        TCHAR szKey[256];
        
        wsprintf( szKey, TEXT("SYSTEM\\CurrentControlSet\\Services\\%s"), m_pServiceName );
        if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                           szKey,
                           0,
                           KEY_READ,
                           &hkey ) == ERROR_SUCCESS )
        {
            DWORD dwType = 0;
            DWORD dwSize = sizeof(DWORD);
            if ( RegQueryValueEx(hkey,
                                 TEXT("Type"),
                                 NULL,
                                 NULL,
                                 (LPBYTE) &dwType,
                                 &dwSize ) == ERROR_SUCCESS )
            {
                if ( dwType & SERVICE_INTERACTIVE_PROCESS )
                {
                    m_isInteractive = TRUE;

                    DWORD dwAllocConsole = 0;
                    DWORD dwAllocSize = sizeof( dwAllocConsole );
                    if ( RegQueryValueEx(hkey,
                                         TEXT("AllocConsole"),
                                         NULL,
                                         NULL,
                                         (LPBYTE) &dwAllocConsole,
                                         &dwAllocSize ) == ERROR_SUCCESS )
                    {
                        if ( dwAllocConsole )
                        {
                            FreeConsole();
                            if ( AllocConsole() )
                            {
                                int hCrt = ::_open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);

                                *stdout = *(::_fdopen(hCrt, "w"));
                                ::setvbuf(stdout, NULL, _IONBF, 0);

                                *stderr = *(::_fdopen(hCrt, "w"));
                                ::setvbuf(stderr, NULL, _IONBF, 0);

                            }
                        }
                    }

                    DWORD dwDebug = 0;
                    DWORD dwDbgSize = sizeof(dwDebug);
                    TCHAR szWindbgPath[256];
                    DWORD dwPathSize = sizeof(szWindbgPath);
                    if (
                       ( RegQueryValueEx(hkey,
                                         TEXT("Debug"),
                                         NULL,
                                         NULL,
                                         (LPBYTE) &dwDebug,
                                         &dwDbgSize ) == ERROR_SUCCESS )
                       &&
                       ( RegQueryValueEx(hkey,
                                         TEXT("WindbgPath"),
                                         NULL,
                                         NULL,
                                         (LPBYTE) szWindbgPath,
                                         &dwPathSize ) == ERROR_SUCCESS )
                       )
                    {
                        if( dwDebug )
                        {
                            PROCESS_INFORMATION pi;
                            STARTUPINFO         si;      

                            si.cb          = sizeof( STARTUPINFO );
                            si.lpReserved  = NULL;
                            si.lpDesktop   = NULL;
                            si.lpTitle     = NULL;
                            si.dwX         = 0;
                            si.dwY         = 0;
                            si.dwXSize     = 0;
                            si.dwYSize     = 0;
                            si.dwFlags     = STARTF_USESHOWWINDOW;
                            si.wShowWindow = SW_SHOWMINNOACTIVE;
                            si.cbReserved2 = 0;
                            si.lpReserved2 = NULL;

                            pi.hProcess  = NULL;

                            TCHAR szCmd[300];
                            wsprintf( szCmd, TEXT("%s -g -p 0x%x"), szWindbgPath, GetCurrentProcessId() );
  
                            if( CreateProcess(
                                   NULL, szCmd,
                                   (LPSECURITY_ATTRIBUTES) NULL,
                                   (LPSECURITY_ATTRIBUTES) NULL,
                                   FALSE,
                                   DETACHED_PROCESS,
                                   (LPVOID) NULL,
                                   (LPTSTR) NULL,
                                   &si, &pi ) )
                            {
                                CloseHandle( pi.hThread );
                                CloseHandle( pi.hProcess );
                            }
                        }
                    }

                }
            }

            RegCloseKey( hkey );
        }
        

    }


    closedown:
        if ( pServiceSid )
            ::FreeSid( pServiceSid );

        if ( pInteractiveSid )
            ::FreeSid( pInteractiveSid );

        if ( groupInfo )
            ::LocalFree( groupInfo );

        if ( hProcessToken )
            ::CloseHandle( hProcessToken );

#if TRACE_SERVICE
    {
        char szBuf[256];
        DWORD cbWritten;
        wsprintf( szBuf, "FvServiceBase::CheckSIDs - Interactive:%ld  Service:%ld  Error:%lu\n", m_isInteractive, m_isService, dwRet );
        WriteFile( g_hFile, szBuf, lstrlen( szBuf ), &cbWritten, NULL );
    }
#endif // TRACE_SERVICE

    return dwRet;
}

BOOL WINAPI FvServiceBase::control_handler ( DWORD dwCtrlType )
{
    ASSERT( g_pService );
    char szBuf[256];
    DWORD bytes;

    switch( dwCtrlType )
    {
        case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
            wsprintf( szBuf, TEXT("Stopping %s.\n"), g_pService->m_pServiceDisplayName);
            WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), szBuf, lstrlen(szBuf)+1, &bytes, NULL );
            g_pService->onStop();
            return TRUE;
            break;

    }
    return FALSE;
}

#endif // _WIN32
