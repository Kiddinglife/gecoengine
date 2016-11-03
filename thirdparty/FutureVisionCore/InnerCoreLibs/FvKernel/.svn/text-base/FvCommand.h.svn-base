//{future header message}
#ifndef __FvCommand_H__
#define __FvCommand_H__

#include "FvKernel.h"

class FV_KERNEL_API FvCommand
{
public:
	FvCommand(int iArgc, char** ppcArgv);
	FvCommand(const char* pcCmdline);
	~FvCommand();
	int ExcessArguments();  

	FvCommand& Min(float fValue);
	FvCommand& Max(float fValue);
	FvCommand& Inf(float fValue);
	FvCommand& Sup(float fValue);

	int Boolean(const char* pcName); 
	int Boolean(const char* pcName, bool& bValue);
	int Integer(const char* pcName, int& iValue);
	int Float(const char* pcName, float& fValue);
	int String(const char* pcName, char* pcValue, unsigned int uiBufferLen);
	int Filename(char* pcFilename, unsigned int uiBufferLen);

	FV_INLINE const char* GetLastError() { return m_pcLastError; }

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

#endif // __FvCommand_H__