#include "geco-cmdline.h"

#include <string.h>
#include <malloc.h>

char geco_cmdline_t::ms_acOptionNotFound[] = "option not found";
char geco_cmdline_t::ms_acArgumentRequired[] = "option requires an argument";
char geco_cmdline_t::ms_acArgumentOutOfRange[] = "argument out of range";
char geco_cmdline_t::ms_acFilenameNotFound[] = "filename not found";

geco_cmdline_t::geco_cmdline_t(int iArgc, char** ppcArgv)
{
	m_iArgc = iArgc;
	m_ppcArgv = ppcArgv;
	m_pcCmdline = 0;
	m_pbUsed = 0;

	Initialize();
}

geco_cmdline_t::geco_cmdline_t(const char* pcCmdline)
{
	typedef struct Argument
	{
		char* pcItem;
		Argument* pkNext;
	} Argument;

	m_iArgc = 0;
	m_ppcArgv = 0;
	m_pbUsed = 0;
	m_pcCmdline = 0;

	if (pcCmdline == 0 || strlen(pcCmdline) == 0)
		return;

	size_t stLen = strlen(pcCmdline) + 1;
	m_pcCmdline = (char*) malloc(sizeof(char) * stLen);
	strcpy(m_pcCmdline, pcCmdline);

	for (unsigned int ui = 0; ui < strlen(pcCmdline) + 1; ui++)
	{
		if (m_pcCmdline[ui] == '\r' || m_pcCmdline[ui] == '\n')
		{
			m_pcCmdline[ui] = ' ';
		}
	}

	char* pcToken = m_pcCmdline;
	Argument* pkList = 0;

	while (pcToken && *pcToken)
	{
		m_iArgc++;
		Argument* pkCurrent = (Argument*) malloc(sizeof(Argument));
		pkCurrent->pkNext = pkList;
		pkList = pkCurrent;

		if (pcToken[0] == '\"')
		{
			pkList->pcItem = ++pcToken;

			while (*pcToken != 0 && *pcToken != '\"')
				pcToken++;
		}
		else
		{
			pkList->pcItem = pcToken;
			while (*pcToken != 0 && *pcToken != ' ' && *pcToken != '\t')
				pcToken++;
		}

		if (*pcToken != 0)
			*pcToken++ = 0;

		while (*pcToken != 0 && (*pcToken == ' ' || *pcToken == '\t'))
			pcToken++;
	}

	m_iArgc++;
	m_ppcArgv = (char**) malloc(sizeof(char*) * m_iArgc);
	m_ppcArgv[0] = m_pcCmdline;
	int i = m_iArgc - 1;
	while (pkList)
	{
		m_ppcArgv[i--] = pkList->pcItem;

		Argument* pkSave = pkList->pkNext;
		free(pkList);
		pkList = pkSave;
	}

	Initialize();
}

geco_cmdline_t::~geco_cmdline_t()
{
	free(m_pbUsed);

	if (m_pcCmdline)
	{
		free(m_ppcArgv);
		free(m_pcCmdline);
	}
}

void geco_cmdline_t::Initialize()
{
	m_pbUsed = (bool*) malloc(sizeof(bool) * m_iArgc);
	memset(m_pbUsed, false, m_iArgc * sizeof(bool));

	m_fSmall = 0.0f;
	m_fLarge = 0.0f;
	m_bMinSet = false;
	m_bMaxSet = false;
	m_bInfSet = false;
	m_bSupSet = false;
}

int geco_cmdline_t::ExcessArguments()
{
	if (!this)
		return 0;

	for (int k = 1; k < m_iArgc; k++)
	{
		if (!m_pbUsed[k])
			return k;
	}

	return 0;
}

geco_cmdline_t& geco_cmdline_t::Min(float fValue)
{
	m_fSmall = fValue;
	m_bMinSet = true;
	return *this;
}

geco_cmdline_t& geco_cmdline_t::Max(float fValue)
{
	m_fLarge = fValue;
	m_bMaxSet = true;
	return *this;
}

geco_cmdline_t& geco_cmdline_t::Inf(float fValue)
{
	m_fSmall = fValue;
	m_bInfSet = true;
	return *this;
}

geco_cmdline_t& geco_cmdline_t::Sup(float fValue)
{
	m_fLarge = fValue;
	m_bSupSet = true;
	return *this;
}

int geco_cmdline_t::Boolean(const char* pcName)
{
	if (!this)
		return 0;

	bool bValue = false;
	return Boolean(pcName, bValue);
}

int geco_cmdline_t::Boolean(const char* pcName, bool& bValue)
{
	if (!this)
		return 0;

	int iMatchFound = 0;
	bValue = false;
	bool used;
	char charr;
	for (int k = 1; k < m_iArgc; k++)
	{
		char* pcPtr = m_ppcArgv[k];
		if (!m_pbUsed[k] && pcPtr[0] == '-' && strcmp(pcName, ++pcPtr) == 0)
		{
			m_pbUsed[k] = true;
			iMatchFound = k;
			bValue = true;
			break;
		}
	}

	if (iMatchFound == 0)
		m_pcLastError = ms_acOptionNotFound;

	return iMatchFound;
}

int geco_cmdline_t::Integer(const char* pcName, int& iValue)
{
	if (!this)
		return 0;

	int iMatchFound = 0;
	for (int k = 1; k < m_iArgc; k++)
	{
		char* pcPtr = m_ppcArgv[k];
		if (!m_pbUsed[k] && pcPtr[0] == '-' && strcmp(pcName, ++pcPtr) == 0)
		{
			pcPtr = m_ppcArgv[k + 1];
			if (m_pbUsed[k + 1]
					|| (pcPtr[0] == '-' && !(pcPtr[1] >= '0' && pcPtr[1] <= '9')))
			{
				m_pcLastError = ms_acArgumentRequired;
				return 0;
			}
#if defined(_MSC_VER) && _MSC_VER >= 1400
			sscanf_s(pcPtr, "%d", &iValue);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
			sscanf(pcPtr, "%d", &iValue);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
			if ((m_bMinSet && iValue < m_fSmall)
					|| (m_bMaxSet && iValue > m_fLarge)
					|| (m_bInfSet && iValue <= m_fSmall)
					|| (m_bSupSet && iValue >= m_fLarge))
			{
				m_pcLastError = ms_acArgumentOutOfRange;
				return 0;
			}
			m_pbUsed[k] = true;
			m_pbUsed[k + 1] = true;
			iMatchFound = k;
			break;
		}
	}

	m_bMinSet = false;
	m_bMaxSet = false;
	m_bInfSet = false;
	m_bSupSet = false;

	if (iMatchFound == 0)
		m_pcLastError = ms_acOptionNotFound;

	return iMatchFound;
}

int geco_cmdline_t::Float(const char* pcName, float& fValue)
{
	if (!this)
		return 0;

	int iMatchFound = 0;
	for (int k = 1; k < m_iArgc; k++)
	{
		char* pcPtr = m_ppcArgv[k];
		if (!m_pbUsed[k] && pcPtr[0] == '-' && strcmp(pcName, ++pcPtr) == 0)
		{
			pcPtr = m_ppcArgv[k + 1];
			if (m_pbUsed[k + 1]
					|| (pcPtr[0] == '-' && !(pcPtr[1] >= '0' && pcPtr[1] <= '9')))
			{
				m_pcLastError = ms_acArgumentRequired;
				return 0;
			}
#if defined(_MSC_VER) && _MSC_VER >= 1400
			sscanf_s(pcPtr, "%f", &fValue);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
			sscanf(pcPtr, "%f", &fValue);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
			if ((m_bMinSet && fValue < m_fSmall)
					|| (m_bMaxSet && fValue > m_fLarge)
					|| (m_bInfSet && fValue <= m_fSmall)
					|| (m_bSupSet && fValue >= m_fLarge))
			{
				m_pcLastError = ms_acArgumentOutOfRange;
				return 0;
			}
			m_pbUsed[k] = true;
			m_pbUsed[k + 1] = true;
			iMatchFound = k;
			break;
		}
	}

	m_bMinSet = false;
	m_bMaxSet = false;
	m_bInfSet = false;
	m_bSupSet = false;

	if (iMatchFound == 0)
		m_pcLastError = ms_acOptionNotFound;

	return iMatchFound;
}

int geco_cmdline_t::String(const char* pcName, char* pcValue,
		unsigned int uiBufferLen)
{
	if (!this)
		return 0;

	int iMatchFound = 0;
	for (int k = 1; k < m_iArgc; k++)
	{
		char* pcPtr = m_ppcArgv[k];
		if (!m_pbUsed[k] && pcPtr[0] == '-' && strcmp(pcName, ++pcPtr) == 0)
		{
			pcPtr = m_ppcArgv[k + 1];
			if (m_pbUsed[k + 1] || pcPtr[0] == '-')
			{
				m_pcLastError = ms_acArgumentRequired;
				return 0;
			}
			strcpy(pcValue, pcPtr);
			m_pbUsed[k] = true;
			m_pbUsed[k + 1] = true;
			iMatchFound = k;
			break;
		}
	}

	if (iMatchFound == 0)
		m_pcLastError = ms_acOptionNotFound;

	return iMatchFound;
}

int geco_cmdline_t::Filename(char* pcFilename, unsigned int uiBufferLen)
{
	if (!this)
		return 0;

	int iMatchFound = 0;
	for (int k = 1; k < m_iArgc; k++)
	{
		char* pcPtr = m_ppcArgv[k];
		if (!m_pbUsed[k] && pcPtr[0] != '-')
		{
			strcpy(pcFilename, pcPtr);
			m_pbUsed[k] = true;
			iMatchFound = k;
			break;
		}
	}

	if (iMatchFound == 0)
		m_pcLastError = ms_acFilenameNotFound;

	return iMatchFound;
}
