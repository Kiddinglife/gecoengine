#include "FvWindowsMachineGuard.h"
#include <FvSysLog.h>

#include <iphlpapi.h>
#include <psapi.h>
#include <sstream>
#include <sstream>

#pragma comment(lib, "Iphlpapi.lib")

typedef BOOL (WINAPI *LPGETSYSTEMTIMES)(LPFILETIME,LPFILETIME,LPFILETIME);
LPGETSYSTEMTIMES s_pfImportedGetSystemTimes;
const char *g_pcMachinedConfFile = ".\\FvMachined.conf";

static void ReportLastError()
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
		);
	FvSysLog( FV_SYSLOG_ERR, "Windows error: %s", (LPCTSTR)lpMsgBuf );
	LocalFree( lpMsgBuf );
}

void InitProcessState( bool bDaemon )
{
	HMODULE hKernel = GetModuleHandle( "kernel32.dll" );
	if ( hKernel )
	{
		s_pfImportedGetSystemTimes = (LPGETSYSTEMTIMES) GetProcAddress( hKernel, "GetSystemTimes" );
	}
}

void CleanupProcessState()
{

}

void GetProcessorSpeeds( std::vector<float> &kSpeeds )
{
	HKEY hkey = NULL;
	if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor"), NULL, KEY_READ, &hkey ) &&
		hkey )
	{
		DWORD ndx = 0;
		TCHAR szCPU[8];
		DWORD lenCPU = sizeof(szCPU) / sizeof(szCPU[0]);
		FILETIME ft;

		while( ERROR_SUCCESS == RegEnumKeyEx( hkey, ndx, szCPU, &lenCPU, NULL, NULL, NULL, &ft ) )
		{
			HKEY hkeyCPU = NULL;
			if ( ERROR_SUCCESS == RegOpenKeyEx( hkey, szCPU, NULL, KEY_QUERY_VALUE, &hkeyCPU ) &&
				hkeyCPU )
			{
				DWORD dwMHz = 0;
				DWORD cbMHz = sizeof(dwMHz);
				DWORD dwType = 0;
				if ( ERROR_SUCCESS == RegQueryValueEx( hkeyCPU, TEXT("~MHz"), NULL, &dwType, (LPBYTE)&dwMHz, &cbMHz ) )
				{
					kSpeeds.push_back( (float)dwMHz );
				}

				RegCloseKey( hkeyCPU );
				ndx++;
			}
		}
		RegCloseKey( hkey );
	}
}

typedef std::map<std::string,std::string> EnvironMap;

static void ReadEnviron( EnvironMap &kEnvMap )
{
	for ( int i=0; _environ[i]; i++ )
	{
		std::string kVar = _environ[i];
		std::string::size_type eqPos = kVar.find('=');
		std::string kVarName = kVar.substr( 0, eqPos );
		std::string kVarValue = kVar.substr( eqPos+1 );
		kEnvMap[kVarName] = kVarValue;
	}
}

static char *MakeEnvironStr( const EnvironMap &kEnvMap )
{
	unsigned length = 0;
	for ( EnvironMap::const_iterator i = kEnvMap.begin(); i != kEnvMap.end(); ++i )
	{
		length += i->first.length();
		length ++;
		length += i->second.length();
		length ++;
	}
	length ++;

	char * result = new char[length];

	char * resultPos = result;
	for ( EnvironMap::const_iterator i = kEnvMap.begin(); i != kEnvMap.end(); ++i )
	{
		memcpy( resultPos, i->first.c_str(), i->first.length() );
		resultPos += i->first.length();
		*resultPos++ = '=';
		memcpy( resultPos, i->second.c_str(), i->second.length() );
		resultPos += i->second.length();
		*resultPos++ = 0;
	}
	*resultPos++ = 0;
	FV_ASSERT(resultPos == result+length);

	return result;
}

FvUInt32 StartProcess( const char *pcRoot,
					  const char *pcResPath,
					  const char *pcConfig,
					  const char *pcType,
					  FvUInt16 uiUID,
					  FvUInt16 uiGID,
					  const char *pcHome,
					  int iArgc,
					  const char **pcArgv,
					  FvMachined &kMachined )
{
	char acPath[512];

	strcpy( acPath, pcRoot );
	//strcat( acPath, "\\futurevision\\bin\\server_" );
	strcat( acPath, pcConfig );
	strcat( acPath, "\\" );
	strncat( acPath, pcType, 32 );
	strcat( acPath, ".exe" );

	TCHAR szArgs[MAX_PATH] = TEXT("");
	int iLen = 0;
	for ( int ndx = 1; ndx < iArgc; ndx++ )
	{
		iLen += wsprintf( szArgs+iLen, TEXT( " \"%s\"" ), pcArgv[ndx] );
	}

	EnvironMap kEnvMap;
	ReadEnviron(kEnvMap);
	kEnvMap["FV_ROOT"] = pcRoot;
	kEnvMap["FV_RES_PATH"] = pcResPath;
	std::ostringstream kUIDToString;
	kUIDToString << uiUID;
	kEnvMap["UID"] = kUIDToString.str();
	char * pcEnvStr = MakeEnvironStr(kEnvMap);

	TCHAR szApp[MAX_PATH];
	lstrcpy( szApp, acPath );

	strcpy(acPath, pcRoot);
	//strcat(acPath, "\\futurevision\\bin\\server_" );
	strcat(acPath, pcConfig );

	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	FvSysLog( FV_SYSLOG_INFO, "starting: '%s'\nwith args: '%s'\nand working path: '%s'", szApp, szArgs, acPath );
	if ( CreateProcess( szApp, szArgs, NULL, NULL, TRUE,
		NORMAL_PRIORITY_CLASS |	CREATE_NO_WINDOW, pcEnvStr, acPath, &si, &pi ) )
	{
		CloseHandle( pi.hThread );
		CloseHandle( pi.hProcess );
	}
	else
	{
		ReportLastError();
	}

	delete[] pcEnvStr;
	return pi.dwProcessId;
}

bool CheckProcess( FvUInt32 uiPID )
{
	if(uiPID)
	{
		HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, uiPID );
		if ( !hProcess )
			return false;

		CloseHandle(hProcess);
		return true;
	}

	return false;
}

void CheckCoreDumps( const char *pcRoot, FvNetUserMessage &kUM )
{
	
}

void FvProcessInfo::Init( const FvNetProcessMessage &kPM )
{
	
}

bool UpdateSystemInfoP( FvSystemInfo &kSI )
{
	if ( s_pfImportedGetSystemTimes )
	{
		FILETIME kIdleFileTime, kKernelFileTime, kUserFileTime;
		s_pfImportedGetSystemTimes( &kIdleFileTime, &kKernelFileTime, &kUserFileTime );

		FvUInt64 uiIdleTime = *(FvUInt64*)&kIdleFileTime;
		FvUInt64 uiKernelTime = *(FvUInt64*)&kKernelFileTime;
		FvUInt64 uiUserTime = *(FvUInt64*)&kUserFileTime;

		//gCurMaxCPU = (uiKernelTime + uiKserTime) / gCPUCount;
		//td.curCPU = gCurMaxCPU - uiIdleTime / gCPUCount;
	}

	MEMORYSTATUS kMem;
	GlobalMemoryStatus( &kMem );
	kSI.m_kMem.max.update(kMem.dwTotalPhys);
	kSI.m_kMem.val.update(kMem.dwAvailPhys);

	MIB_IPSTATS kIPStats;
	GetIpStatistics( &kIPStats );
	kSI.m_kPackTotOut.update(kIPStats.dwOutRequests);
	kSI.m_kPackDropOut.update(kIPStats.dwOutDiscards);
	kSI.m_kPackTotIn.update(kIPStats.dwInReceives);
	kSI.m_kPackDropIn.update(kIPStats.dwInDiscards);

	return true;
}

bool UpdateProcessStats( FvProcessInfo &kPI )
{
	kPI.m_uiAffinity = 0;
	return true;
}
