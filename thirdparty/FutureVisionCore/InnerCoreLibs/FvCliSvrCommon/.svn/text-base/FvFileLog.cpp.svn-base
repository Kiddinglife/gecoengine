#include "FvFileLog.h"
#include <FvDebug.h>


bool MessageFile::HandleMessage( int iComponentPriority,
		int iMessagePriority, const char *pcFormat, va_list kArgPtr )
{
	if(m_bOpenLog &&
		m_hFileHandle!=INVALID_HANDLE_VALUE &&
		FvDebugFilter::ShouldAccept( iComponentPriority, iMessagePriority ))
	{
		DWORD uiWrited(0);

		SYSTEMTIME nowTime;
		GetLocalTime(&nowTime);
		int iTimeSize(22);

		char sbuf[256];
		char* pBuf = sbuf;
		int bufLen = _vscprintf(pcFormat, kArgPtr) +iTimeSize +1;
		if(bufLen > sizeof(sbuf))
		{
			pBuf = new char[bufLen];
		}

		_snprintf_s(pBuf, 256, iTimeSize, "%02d/%02d %02d:%02d:%02d [%.4s] ",
			nowTime.wMonth, nowTime.wDay, nowTime.wHour, nowTime.wMinute, nowTime.wSecond,
			MessagePrefix(FvDebugMessagePriority(iMessagePriority)));
		vsprintf_s(pBuf+iTimeSize, bufLen-iTimeSize, pcFormat, kArgPtr);

		WriteFile(m_hFileHandle, pBuf, bufLen-1, &uiWrited, NULL);

		if(pBuf != sbuf)
		{
			delete [] pBuf;
		}
	}

	return false;
}

MessageFile::MessageFile(const char* pcLogFileName)
:m_hFileHandle(INVALID_HANDLE_VALUE)
,m_bOpenLog(true)
{
	if(pcLogFileName)
	{
		m_hFileHandle = CreateFile(TEXT(pcLogFileName),	//! file to create
			GENERIC_WRITE,								//! open for writing
			FILE_SHARE_READ,							//! share read
			NULL,										//! default security
			OPEN_ALWAYS,								//! open existing
			FILE_ATTRIBUTE_NORMAL,						//! normal file
			NULL);										//! no attr. template

		if(m_hFileHandle == INVALID_HANDLE_VALUE)
		{
			printf("Could not open file:%s (error %d)\n", pcLogFileName, GetLastError());
			return;
		}
		else
		{
			DWORD uiWrited(0);
			char startMsg[] = "========log start========\n";
			WriteFile(m_hFileHandle, startMsg, strlen(startMsg), &uiWrited, NULL);

			SYSTEMTIME nowTime;
			GetLocalTime(&nowTime);
			char timeStr[32] = {0};
			sprintf_s(timeStr, sizeof(timeStr), "%d/%d/%d %d:%d\n", nowTime.wYear, nowTime.wMonth, nowTime.wDay, nowTime.wHour, nowTime.wMinute);
			WriteFile(m_hFileHandle, timeStr, strlen(timeStr), &uiWrited, NULL);
		}
	}
}

MessageFile::~MessageFile()
{
	if(m_hFileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileHandle);
		m_hFileHandle = INVALID_HANDLE_VALUE;
	}
}

