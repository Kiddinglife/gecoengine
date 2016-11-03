#include "FvProcess.h"

int FV_POWER_EXTERN_VAR g_iBackEndPID = 0;
int FV_POWER_EXTERN_VAR g_iBackEndUID = 0;
int FV_POWER_EXTERN_VAR g_iIsBackEndProcess = 0;

void FV_POWER_API InitBackEndProcess( int iUID, int iPID )
{
	g_iIsBackEndProcess = 1;
	g_iBackEndPID = iPID;
	g_iBackEndUID = iUID;
}

int FV_POWER_API FvGetUserID()
{
#ifdef _WIN32
	// VS2005:
#if _MSC_VER >= 1400
	char uid[16];
	size_t sz;

	if (g_iIsBackEndProcess)
		return g_iBackEndUID;

	return getenv_s( &sz, uid, sizeof( uid ), "UID" ) == 0 ? atoi( uid ) : 0;

	// VS2003:
#elif _MSC_VER < 1400
	if (g_iIsBackEndProcess)
		return g_iBackEndUID;

	char * uid = getenv( "UID" );
	return uid ? atoi( uid ) : 0;
#endif
#elif defined( PLAYSTATION3 )
	if (g_iIsBackEndProcess)
		return g_iBackEndUID;

	return 123;
#else
	if (g_iIsBackEndProcess)
		return g_iBackEndUID;

	// Linux:
	char * uid = getenv( "UID" );
	return uid ? atoi( uid ) : getuid();
#endif
}

int FV_POWER_API FvGetPID()
{
	if (g_iIsBackEndProcess)
		return g_iBackEndPID;

#if defined(unix)
	return getpid();
#elif defined(_XBOX) || defined(_XBOX360) || defined( PLAYSTATION3 )
	return -1;
#else
	return (int) GetCurrentProcessId();
#endif
}

LP_FV_PASSWD FV_POWER_API FvGetPWUID(int iUID)
{
#ifdef _WIN32
	static struct passwd kPW = {"name","pwd",0,0,"","",""};
	kPW.pw_uid = iUID;
	return &kPW;
#else // _WIN32
	return getpwuid(iUID);
#endif // !_WIN32
}

LP_FV_PASSWD FV_POWER_API FvGetPWNam(const char *pcName)
{
#ifdef _WIN32
	static struct passwd kPW = {"name","pwd",0,0,"","",""};
	return &kPW;
#else // _WIN32
	return getpwnam(pcName);
#endif // !_WIN32
}