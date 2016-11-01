#include "FvCliSvrCommonDefines.h"
#include <FvDebugFilter.h>


struct FV_CLI_SVR_COMMON_API MessageFile : public FvDebugMessageCallback
{
	virtual bool HandleMessage( int iComponentPriority,
		int iMessagePriority, const char *pcFormat, va_list kArgPtr );

	void	OpenLog(bool bVal) { m_bOpenLog=bVal; }

	MessageFile(const char* pcLogFileName);
	~MessageFile();

	HANDLE	m_hFileHandle;
	bool	m_bOpenLog;
};

