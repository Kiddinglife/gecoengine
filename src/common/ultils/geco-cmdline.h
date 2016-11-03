#ifndef _SRC_GECO_CMDLINE
#define _SRC_GECO_CMDLINE

//see GECO_ULTILS.test_geco_cmdline_t to leran how to use it
#include "../geco-plateform.h"

class GECO_EXPORT geco_cmdline_t
{
public:
	geco_cmdline_t(int iArgc, char** ppcArgv);
	geco_cmdline_t(const char* pcCmdline);
	~geco_cmdline_t();
	int ExcessArguments();

	geco_cmdline_t& Min(float fValue);
	geco_cmdline_t& Max(float fValue);
	geco_cmdline_t& Inf(float fValue);
	geco_cmdline_t& Sup(float fValue);

	int Boolean(const char* pcName);
	int Boolean(const char* pcName, bool& bValue);
	int Integer(const char* pcName, int& iValue);
	int Float(const char* pcName, float& fValue);
	int String(const char* pcName, char* pcValue, unsigned int uiBufferLen);
	int Filename(char* pcFilename, unsigned int uiBufferLen);

	const char* GetLastError()
	{
		return m_pcLastError;
	}

protected:
	void Initialize();

	int m_iArgc;
	char** m_ppcArgv;
	char* m_pcCmdline;
	bool* m_pbUsed;

	float m_fSmall;
	float m_fLarge;
	bool m_bMinSet;
	bool m_bMaxSet;
	bool m_bInfSet;
	bool m_bSupSet;

	char* m_pcLastError;
	static char ms_acOptionNotFound[];
	static char ms_acArgumentRequired[];
	static char ms_acArgumentOutOfRange[];
	static char ms_acFilenameNotFound[];
};

#endif
