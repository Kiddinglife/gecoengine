#include "FvLogBus.h"
#include <windows.h>
#include <iostream>
#include <stdarg.h>

FvUInt32 FvLogBus::m_uiOutputMask = FvLogUser::DEFAULT;
FvLogType::Idx FvLogBus::m_uiLogLevel = FvLogType::OK;

#define BUFFER_SIZE 1024

void FvLogBus::SetColor(const LogColor uiIdx)
{
	if(uiIdx < LOG_COLOR_TOTAL)
	{
		static FvUInt32 WinColorFG[LOG_COLOR_TOTAL] =
		{
			0,                                                  // BLACK
			FOREGROUND_RED,                                     // RED
			FOREGROUND_GREEN,                                   // GREEN
			FOREGROUND_RED|FOREGROUND_GREEN,                  // BROWN
			FOREGROUND_BLUE,                                    // BLUE
			FOREGROUND_RED|FOREGROUND_BLUE,// MAGENTA
			FOREGROUND_GREEN|FOREGROUND_BLUE,                 // CYAN
			FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,// WHITE
			// YELLOW
			FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY,
			// RED_BOLD
			FOREGROUND_RED|FOREGROUND_INTENSITY,
			// GREEN_BOLD
			FOREGROUND_GREEN|FOREGROUND_INTENSITY,
			FOREGROUND_BLUE|FOREGROUND_INTENSITY,             // BLUE_BOLD
			// MAGENTA_BOLD
			FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY,
			// CYAN_BOLD
			FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY,
			// WHITE_BOLD
			FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY
		};

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, WinColorFG[uiIdx]);
	}
}

void FvLogBus::Note(const FvUInt32 eMask, const FvLogType::Idx eType, const char * str, ... )
{
	//return;//! add by Uman, 20100810

	if(m_uiOutputMask&eMask)
	{
		SetColor(GREEN);

		if( !str ) return;
		static char S_STR[BUFFER_SIZE + 1] = {0};
		va_list ap;
		va_start(ap, str);
		vprintf_s(str, ap );
		vprintf_s("\n", ap );
		///<>
		const size_t uiSize = _vscprintf(str,ap);
		if(uiSize > BUFFER_SIZE)
		{
			sprintf(S_STR, "%s/n", "buffer out!");
		}
		else
		{
			vsprintf_s(S_STR, BUFFER_SIZE, str, ap );
			FvUInt32 size = (FvUInt32)strlen(S_STR);
			S_STR[size] = '\n';
			S_STR[size + 1] = 0;
		}
		va_end(ap);
		fflush(stdout);

		//#ifdef _DEBUG
		OutputDebugStringA(S_STR);
		//OutputDebugStringA("\n");
		//#endif
	}
}

void FvLogBus::Error(const char * str, ... )
{
	//return;//! add by Uman, 20100810

	SetColor(LRED);

	if( !str ) return;
	static char S_STR[BUFFER_SIZE + 1] = {0};
	va_list ap;
	va_start(ap, str);
	vprintf_s(str, ap );
	vprintf_s("\n", ap );
	///<>
	const size_t uiSize = _vscprintf(str,ap);
	if(uiSize > BUFFER_SIZE)
	{
		sprintf(S_STR, "%s", "buffer out!");
	}
	else
	{
		vsprintf_s(S_STR, BUFFER_SIZE, str, ap );
		FvUInt32 size = (FvUInt32)strlen(S_STR);
		S_STR[size] = '\n';
		S_STR[size + 1] = 0;
	}
	va_end(ap);
	fflush(stdout);

	//#ifdef _DEBUG
	OutputDebugStringA(S_STR);
	//#endif

}


void FvLogBus::Warning(const char * str, ... )
{
	//return;//! add by Uman, 20100810

	SetColor(YELLOW);

	if( !str ) return;
	static char S_STR[BUFFER_SIZE + 1] = {0};
	va_list ap;
	va_start(ap, str);
	vprintf_s(str, ap );
	vprintf_s("\n", ap );
	const size_t uiSize = _vscprintf(str,ap);
	if(uiSize > BUFFER_SIZE)
	{
		sprintf(S_STR, "%s/n", "buffer out!");
	}
	else
	{
		vsprintf_s(S_STR, BUFFER_SIZE, str, ap );
		FvUInt32 size = (FvUInt32)strlen(S_STR);
		S_STR[size] = '\n';
		S_STR[size + 1] = 0;
	}
	//Print(S_STR, size + 1);
	va_end(ap);
	fflush(stdout);

	//#ifdef _DEBUG
	OutputDebugStringA(S_STR);
	//OutputDebugStringA("\n");
	//#endif

}

void FvLogBus::OK(const FvUInt32 eMask, const char * str, ... )
{
	//return;//! add by Uman, 20100810

	if(m_uiOutputMask&eMask)
	{
		SetColor(GREEN);

		if( !str ) return;
		static char S_STR[BUFFER_SIZE + 1] = {0};
		va_list ap;
		va_start(ap, str);
		vprintf_s(str, ap );
		vprintf_s("\n", ap );
		///<>
		const size_t uiSize = _vscprintf(str,ap);
		if(uiSize > BUFFER_SIZE)
		{
			sprintf(S_STR, "%s/n", "buffer out!");
		}
		else
		{
			vsprintf_s(S_STR, BUFFER_SIZE, str, ap );
			FvUInt32 size = (FvUInt32)strlen(S_STR);
			S_STR[size] = '\n';
			S_STR[size + 1] = 0;
		}
		//Print(S_STR, size + 1);
		va_end(ap);
		fflush(stdout);

		//#ifdef _DEBUG
		OutputDebugStringA(S_STR);
		//OutputDebugStringA("\n");
		//#endif
	}
}
void FvLogBus::CritOk(const char * str, ... )
{
	//return;//! add by Uman, 20100810

	SetColor(LGREEN);

	if( !str ) return;
	static char S_STR[BUFFER_SIZE + 1] = {0};
	va_list ap;
	va_start(ap, str);
	vprintf_s(str, ap );
	vprintf_s("\n", ap );
	///<>
	const size_t uiSize = _vscprintf(str,ap);
	if(uiSize > BUFFER_SIZE)
	{
		sprintf(S_STR, "%s/n", "buffer out!");
	}
	else
	{
		vsprintf_s(S_STR, BUFFER_SIZE, str, ap );
		FvUInt32 size = (FvUInt32)strlen(S_STR);
		S_STR[size] = '\n';
		S_STR[size + 1] = 0;
	}
	//Print(S_STR, size + 1);
	va_end(ap);
	fflush(stdout);

	//#ifdef _DEBUG
	OutputDebugStringA(S_STR);
	//OutputDebugStringA("\n");
	//#endif

}

void FvLogBus::SetMask(const FvUInt32 eMask)
{
	m_uiOutputMask= eMask;
}
void FvLogBus::SetLogLevel(const FvLogType::Idx eLevel)
{
	m_uiLogLevel = eLevel;
}