//{future header message}
#ifndef __FvServiceBase_H__
#define __FvServiceBase_H__

#ifdef _WIN32

#include "FvServerCommon.h"

class FV_SERVERCOMMON_API FvServiceBase
{
  public:
    FvServiceBase( LPTSTR pServiceName, LPTSTR pServiceDisplayName = NULL, DWORD svcType = SERVICE_WIN32_OWN_PROCESS );
    ~FvServiceBase();

    DWORD Checkpoint( DWORD dwWaitHint = 0 );

    DWORD UpdateStatus( DWORD dwState = SERVICE_START_PENDING,
                        DWORD dwWaitHint = 0,
                        DWORD dwError = NO_ERROR );

    BOOL SetDispatchEntries( SERVICE_TABLE_ENTRY* pDispatch );

    DWORD Start( int argc, char** argv);

	void NeverService() { m_neverService = TRUE; }

    void StartWithExternalDispatch( DWORD dwArgc, LPTSTR* lpArgv);

    void RegisterExternalHandler( LPHANDLER_FUNCTION handler );

    DWORD Install( LPTSTR pDependencies = NULL,
                   DWORD svcType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                   LPTSTR pszPath = NULL );
    DWORD Remove();

    virtual void Main() = 0;
    DWORD   dwArgc;
    TCHAR** lpArgv;

    virtual void            onStop(void)=0; 
    virtual void            onPause(void) {};
    virtual void            onContinue(void) {};
    virtual void            onInterrogate(void) {};
    virtual void            onShutdown() {};
    virtual void            onUserControl( DWORD dwCtrl ) {};

    void SetControlsAccepted( DWORD dwControlsAccepted = SERVICE_ACCEPT_STOP )
        { m_dwControlsAccepted = dwControlsAccepted; }

    DWORD GetControlsAccepted()
        { return m_dwControlsAccepted; }

    LPCTSTR GetName()
        { return m_pServiceName; }

    LPCTSTR GetDisplayName()
        { return m_pServiceDisplayName; }

    BOOL isService()
        { return m_isService; }

    BOOL isInteractive()
        { return m_isInteractive; }

    BOOL isStopping()
        { if ( ( m_Status.dwCurrentState == SERVICE_STOP_PENDING ) ||
               ( m_Status.dwCurrentState == SERVICE_STOPPED ) )
             return TRUE;
          else
             return FALSE;
        }

    static void WINAPI service_main( DWORD dwArgc, LPTSTR* lpArgv );


  protected:
    SERVICE_STATUS          m_Status;
    SERVICE_STATUS_HANDLE   m_StatusHandle;
    DWORD                   m_dwControlsAccepted;

    BOOL                    m_isService;
    BOOL                    m_isInteractive;
	BOOL					m_neverService;
    
    LPTSTR                  m_pServiceName;
    LPTSTR                  m_pServiceDisplayName;

    SERVICE_TABLE_ENTRY*    m_pDispatchTable;

    DWORD CheckSIDs(void);
    void WINAPI ServiceCtrl( DWORD dwCtrlCode );


    static void WINAPI service_ctrl( DWORD dwCtrlCode );

    static BOOL WINAPI control_handler( DWORD dwCtrlType );
};


#ifdef __cplusplus
extern "C" {
#endif

BOOL FV_SERVERCOMMON_API ServiceIsStopping();
DWORD FV_SERVERCOMMON_API ServiceCheckpoint( DWORD dwWaitHint );
DWORD FV_SERVERCOMMON_API ServiceUpdateStatus( DWORD dwState, DWORD dwWaitHint, DWORD dwError );

#ifdef __cplusplus
}
#endif

#else // _WIN32

#ifdef __cplusplus
extern "C" {
#endif

#define SERVICE_RUNNING 0
FV_INLINE DWORD ServiceCheckpoint( DWORD dwWaitHint ) { return 0; }
FV_INLINE DWORD ServiceUpdateStatus( DWORD dwState, DWORD dwWaitHint, DWORD dwError ) { return 0; }

#ifdef __cplusplus
}
#endif

#endif // !_WIN32

#endif // __FvServiceBase_H__
