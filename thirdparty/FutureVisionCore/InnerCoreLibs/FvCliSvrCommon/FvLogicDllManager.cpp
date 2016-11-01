#include "FvLogicDllManager.h"


FV_SINGLETON_STORAGE(FvLogicDllManager);

FvLogicDllManager::FvLogicDllManager()
:m_hDll(NULL)
,m_pFnDllExport(NULL)
,m_pFnInitializeLogic(NULL)
,m_pFnUnInitializeLogic(NULL)
,m_pFnTickDll(NULL)
,m_pFnOnSetGameTime(NULL)
,m_pFnOnBaseAppReady(NULL)
,m_pFnOnBaseAppShuttingDown(NULL)
,m_pFnOnBaseAppShutDown(NULL)
,m_pFnOnCellAppShuttingDown(NULL)
,m_pFnOnBaseAppData(NULL)
,m_pFnOnDelBaseAppData(NULL)
,m_pFnOnCellAppReady(NULL)
,m_pFnOnCellAppData(NULL)
,m_pFnOnDelCellAppData(NULL)
,m_pFnOnGlobalData(NULL)
,m_pFnOnDelGlobalData(NULL)
,m_pFnOnSpaceData(NULL)
,m_pFnOnGlobalAppReady(NULL)
,m_pFnOnGlobalAppShuttingDown(NULL)
,m_pFnOnGlobalAppShutDown(NULL)
,m_pFnUIAddonCreateBindings(NULL)
,m_pFnUIAddonDestroyBindings(NULL)
{

}

FvLogicDllManager::~FvLogicDllManager()
{
	if(m_hDll)
		FreeLibrary(m_hDll);
}

bool FvLogicDllManager::LoadDll(const char* pDllName)
{
	if(m_hDll) return false;

	m_hDll = LoadLibraryEx( pDllName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	if(!m_hDll)
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
		::MessageBox( NULL, (char*)lpMsgBuf, "", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		LocalFree( lpMsgBuf );
		return false;
	}

	m_pFnDllExport					= (PFN_DLLEXPORT)GetProcAddress(m_hDll, "DllExport");
	m_pFnInitializeLogic			= (PFN_INITIALIZELOGIC)GetProcAddress(m_hDll, "InitializeLogic");
	m_pFnUnInitializeLogic			= (PFN_UNINITIALIZELOGIC)GetProcAddress(m_hDll, "UnInitializeLogic");
	m_pFnTickDll					= (PFN_TICKDLL)GetProcAddress(m_hDll, "TickDll");
	m_pFnOnSetGameTime				= (PFN_ONSETGAMETIME)GetProcAddress(m_hDll, "OnSetGameTime");
	m_pFnOnBaseAppReady				= (PFN_ONBASEAPPREADY)GetProcAddress(m_hDll, "OnBaseAppReady");
	m_pFnOnBaseAppShuttingDown		= (PFN_ONAPPSHUTTINGDOWN)GetProcAddress(m_hDll, "OnBaseAppShuttingDown");
	m_pFnOnBaseAppShutDown			= (PFN_ONAPPSHUTDOWN)GetProcAddress(m_hDll, "OnBaseAppShutDown");
	m_pFnOnCellAppShuttingDown		= (PFN_ONAPPSHUTTINGDOWN)GetProcAddress(m_hDll, "OnCellAppShuttingDown");
	m_pFnOnBaseAppData				= (PFN_ONADDDATA)GetProcAddress(m_hDll, "OnBaseAppData");
	m_pFnOnDelBaseAppData			= (PFN_ONDELDATA)GetProcAddress(m_hDll, "OnDelBaseAppData");
	m_pFnOnCellAppReady				= (PFN_ONCELLAPPREADY)GetProcAddress(m_hDll, "OnCellAppReady");
	m_pFnOnCellAppData				= (PFN_ONADDDATA)GetProcAddress(m_hDll, "OnCellAppData");
	m_pFnOnDelCellAppData			= (PFN_ONDELDATA)GetProcAddress(m_hDll, "OnDelCellAppData");
	m_pFnOnGlobalData				= (PFN_ONADDDATA)GetProcAddress(m_hDll, "OnGlobalData");
	m_pFnOnDelGlobalData			= (PFN_ONDELDATA)GetProcAddress(m_hDll, "OnDelGlobalData");
	m_pFnOnSpaceData				= (PFN_ONSPACEDATA)GetProcAddress(m_hDll, "OnSpaceData");
	m_pFnOnGlobalAppReady			= (PFN_ONGLOBALAPPREADY)GetProcAddress(m_hDll, "OnGlobalAppReady");
	m_pFnOnGlobalAppShuttingDown	= (PFN_ONAPPSHUTTINGDOWN)GetProcAddress(m_hDll, "OnGlobalAppShuttingDown");
	m_pFnOnGlobalAppShutDown		= (PFN_ONAPPSHUTDOWN)GetProcAddress(m_hDll, "OnGlobalAppShutDown");

	m_pFnUIAddonCreateBindings		= (PFN_UIADDONCREATEBINDINGS)GetProcAddress(m_hDll, "UIAddonCreateBindings");
	m_pFnUIAddonDestroyBindings		= (PFN_UIADDONDESTROYBINDINGS)GetProcAddress(m_hDll, "UIAddonDestroyBindings");

	return true;
}

FARPROC FvLogicDllManager::GetFunAddr(const char* pcFunName)
{
	if(!m_hDll || !pcFunName || !pcFunName[0])
		return NULL;

	return GetProcAddress(m_hDll, pcFunName);
}

bool FvLogicDllManager::DllExport(void*& pkEntityExports)
{
	if(!m_pFnDllExport)
		return false;

	m_pFnDllExport(pkEntityExports);
	return true;
}

bool FvLogicDllManager::InitializeLogic()
{
	if(!m_pFnInitializeLogic)
		return false;

	m_pFnInitializeLogic();
	return true;
}

void FvLogicDllManager::UnInitializeLogic()
{
	if(m_pFnUnInitializeLogic)
		m_pFnUnInitializeLogic();
}

void FvLogicDllManager::TickDll(float deltaTime)
{
	if(m_pFnTickDll)
		m_pFnTickDll(deltaTime);
}

void FvLogicDllManager::OnSetGameTime(FvTimeStamp uiTimeStamp, float fUpdateFrequency)
{
	if(m_pFnOnSetGameTime)
		m_pFnOnSetGameTime(uiTimeStamp, fUpdateFrequency);
}

void FvLogicDllManager::OnBaseAppReady(bool bIsBootstrap)
{
	if(m_pFnOnBaseAppReady)
		m_pFnOnBaseAppReady(bIsBootstrap);
}

void FvLogicDllManager::OnBaseAppShuttingDown(FvUInt64 uiShutdownTime)
{
	if(m_pFnOnBaseAppShuttingDown)
		m_pFnOnBaseAppShuttingDown(uiShutdownTime);
}

void FvLogicDllManager::OnBaseAppShutDown(int iStage)
{
	if(m_pFnOnBaseAppShutDown)
		m_pFnOnBaseAppShutDown(iStage);
}

void FvLogicDllManager::OnCellAppShuttingDown(FvUInt64 uiShutdownTime)
{
	if(m_pFnOnCellAppShuttingDown)
		m_pFnOnCellAppShuttingDown(uiShutdownTime);
}

void FvLogicDllManager::OnBaseAppData(const FvString& kKey, int value)
{
	if(m_pFnOnBaseAppData)
		m_pFnOnBaseAppData(kKey, value);
}

void FvLogicDllManager::OnDelBaseAppData(const FvString& kKey)
{
	if(m_pFnOnDelBaseAppData)
		m_pFnOnDelBaseAppData(kKey);
}

void FvLogicDllManager::OnCellAppReady()
{
	if(m_pFnOnCellAppReady)
		m_pFnOnCellAppReady();
}

void FvLogicDllManager::OnCellAppData(const FvString& kKey, int value)
{
	if(m_pFnOnCellAppData)
		m_pFnOnCellAppData(kKey, value);
}

void FvLogicDllManager::OnDelCellAppData(const FvString& kKey)
{
	if(m_pFnOnDelCellAppData)
		m_pFnOnDelCellAppData(kKey);
}

void FvLogicDllManager::OnGlobalData(const FvString& kKey, int value)
{
	if(m_pFnOnGlobalData)
		m_pFnOnGlobalData(kKey, value);
}

void FvLogicDllManager::OnDelGlobalData(const FvString& kKey)
{
	if(m_pFnOnDelGlobalData)
		m_pFnOnDelGlobalData(kKey);
}

void FvLogicDllManager::OnSpaceData(FvSpaceID iSpaceID, FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData)
{
	if(m_pFnOnSpaceData)
		m_pFnOnSpaceData(iSpaceID, kEntryID, uiKey, kData);
}

void FvLogicDllManager::OnGlobalAppReady()
{
	if(m_pFnOnGlobalAppReady)
		m_pFnOnGlobalAppReady();
}

void FvLogicDllManager::OnGlobalAppShuttingDown(FvUInt64 uiShutdownTime)
{
	if(m_pFnOnGlobalAppShuttingDown)
		m_pFnOnGlobalAppShuttingDown(uiShutdownTime);
}

void FvLogicDllManager::OnGlobalAppShutDown(int iStage)
{
	if(m_pFnOnGlobalAppShutDown)
		m_pFnOnGlobalAppShutDown(iStage);
}


