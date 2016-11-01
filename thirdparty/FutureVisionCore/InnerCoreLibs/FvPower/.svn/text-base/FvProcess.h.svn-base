//{future header message}
#ifndef __FvProcess_H__
#define __FvProcess_H__

#include "FvPowerDefines.h"

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct passwd 
{
	char    *pw_name;
	char    *pw_passwd;
	int		 pw_uid;
	int		 pw_gid;
	char    *pw_gecos;
	char    *pw_dir;
	char    *pw_shell;
} *LP_FV_PASSWD;

#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	void FV_POWER_API InitBackEndProcess( int iUID, int iPID );
	extern int FV_POWER_EXTERN_VAR g_iBackEndUID;
	extern int FV_POWER_EXTERN_VAR g_iBackEndPID;
	extern int FV_POWER_EXTERN_VAR g_iIsBackEndProcess;
	int FV_POWER_API FvGetUserID();
	int FV_POWER_API FvGetPID();
	LP_FV_PASSWD FV_POWER_API FvGetPWUID(int iUID);
	LP_FV_PASSWD FV_POWER_API FvGetPWNam(const char *pcName);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __FvProcess_H__