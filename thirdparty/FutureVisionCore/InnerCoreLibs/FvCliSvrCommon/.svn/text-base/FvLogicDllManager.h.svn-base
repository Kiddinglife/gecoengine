//{future header message}
#ifndef __FvLogicDllManager_H__
#define __FvLogicDllManager_H__

#include "FvCliSvrCommonDefines.h"
#include <FvSingleton.h>
#include <FvNetTypes.h>

struct lua_State;

typedef void (*PFN_DLLEXPORT)(void*& pkEntityExports);
typedef bool (*PFN_INITIALIZELOGIC)();
typedef void (*PFN_UNINITIALIZELOGIC)();
typedef void (*PFN_TICKDLL)(float deltaTime);
typedef void (*PFN_ONSETGAMETIME)(FvTimeStamp uiTimeStamp, float fUpdateFrequency);
typedef void (*PFN_ONBASEAPPREADY)(bool bIsBootstrap);
typedef void (*PFN_ONAPPSHUTTINGDOWN)(FvUInt64 uiShutdownTime);
typedef void (*PFN_ONAPPSHUTDOWN)(int iStage);
typedef void (*PFN_ONCELLAPPREADY)();
typedef void (*PFN_ONADDDATA)(const FvString& kKey, int value);
typedef void (*PFN_ONDELDATA)(const FvString& kKey);
typedef void (*PFN_ONSPACEDATA)(FvSpaceID iSpaceID, FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData);
typedef void (*PFN_ONGLOBALAPPREADY)();

typedef void (*PFN_UIADDONCREATEBINDINGS)(lua_State *pkState);
typedef void (*PFN_UIADDONDESTROYBINDINGS)(lua_State *pkState);


class FV_CLI_SVR_COMMON_API FvLogicDllManager : public FvSingleton<FvLogicDllManager>
{
public:
	FvLogicDllManager();
	~FvLogicDllManager();

	bool LoadDll(const char* pDllName);
	HINSTANCE GetDLL() { return m_hDll; }
	FARPROC GetFunAddr(const char* pcFunName);

	bool DllExport(void*& pkEntityExports);
	bool InitializeLogic();
	void UnInitializeLogic();
	void TickDll(float deltaTime);
	void OnSetGameTime(FvTimeStamp uiTimeStamp, float fUpdateFrequency);
	void OnBaseAppReady(bool bIsBootstrap);
	void OnBaseAppShuttingDown(FvUInt64 uiShutdownTime);
	void OnBaseAppShutDown(int iStage);
	void OnCellAppShuttingDown(FvUInt64 uiShutdownTime);
	void OnBaseAppData(const FvString& kKey, int value);
	void OnDelBaseAppData(const FvString& kKey);
	void OnCellAppReady();
	void OnCellAppData(const FvString& kKey, int value);
	void OnDelCellAppData(const FvString& kKey);
	void OnGlobalData(const FvString& kKey, int value);
	void OnDelGlobalData(const FvString& kKey);
	void OnSpaceData(FvSpaceID iSpaceID, FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData);
	void OnGlobalAppReady();
	void OnGlobalAppShuttingDown(FvUInt64 uiShutdownTime);
	void OnGlobalAppShutDown(int iStage);

	PFN_UIADDONCREATEBINDINGS GetUIAddonCreateBindings() { return m_pFnUIAddonCreateBindings; }
	PFN_UIADDONDESTROYBINDINGS GetUIAddonDestroyBindings() { return	m_pFnUIAddonDestroyBindings; }

protected:
	HINSTANCE					m_hDll;

	PFN_DLLEXPORT				m_pFnDllExport;
	PFN_INITIALIZELOGIC			m_pFnInitializeLogic;
	PFN_UNINITIALIZELOGIC		m_pFnUnInitializeLogic;
	PFN_TICKDLL					m_pFnTickDll;
	PFN_ONSETGAMETIME			m_pFnOnSetGameTime;
	PFN_ONBASEAPPREADY			m_pFnOnBaseAppReady;
	PFN_ONAPPSHUTTINGDOWN		m_pFnOnBaseAppShuttingDown;
	PFN_ONAPPSHUTDOWN			m_pFnOnBaseAppShutDown;
	PFN_ONAPPSHUTTINGDOWN		m_pFnOnCellAppShuttingDown;
	PFN_ONADDDATA				m_pFnOnBaseAppData;
	PFN_ONDELDATA				m_pFnOnDelBaseAppData;
	PFN_ONCELLAPPREADY			m_pFnOnCellAppReady;
	PFN_ONADDDATA				m_pFnOnCellAppData;
	PFN_ONDELDATA				m_pFnOnDelCellAppData;
	PFN_ONADDDATA				m_pFnOnGlobalData;
	PFN_ONDELDATA				m_pFnOnDelGlobalData;
	PFN_ONSPACEDATA				m_pFnOnSpaceData;
	PFN_ONGLOBALAPPREADY		m_pFnOnGlobalAppReady;
	PFN_ONAPPSHUTTINGDOWN		m_pFnOnGlobalAppShuttingDown;
	PFN_ONAPPSHUTDOWN			m_pFnOnGlobalAppShutDown;

	PFN_UIADDONCREATEBINDINGS	m_pFnUIAddonCreateBindings;
	PFN_UIADDONDESTROYBINDINGS	m_pFnUIAddonDestroyBindings;
};



#endif//__FvLogicDllManager_H__
