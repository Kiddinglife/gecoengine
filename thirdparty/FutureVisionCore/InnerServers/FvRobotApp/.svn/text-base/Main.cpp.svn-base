#include <FvRobotClientApp.h>
#include <FvRobotMainApp.h>
#include <FvRobotSpaceDataManager.h>
#include <FvFileLog.h>
#include <FvLogicDllManager.h>
#include <FvRobotCfg.h>
#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreFileSystem.h>


#define LOGICDLLNAME	"RobotScript.dll"


int main( int argc, char * argv[] )
{
	/////////////////////////////////////////////////////////////////////////////////
	MessageFile* pkMsgFile(NULL);

	/////////////////////////////////////////////////////////////////////////////////
	Ogre::LogManager* pkLog(NULL);
	Ogre::ArchiveManager* pkArchive(NULL);
	Ogre::ResourceGroupManager* pkGroupManager(NULL);
	pkLog = OGRE_NEW Ogre::LogManager;
	pkArchive = OGRE_NEW Ogre::ArchiveManager;
	pkArchive->addArchiveFactory(OGRE_NEW Ogre::FileSystemArchiveFactory);
	pkGroupManager = OGRE_NEW Ogre::ResourceGroupManager;
#ifdef FV_SHIPPING
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Data","FileSystem");
#else // FV_SHIPPING
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../Data","FileSystem");
#endif // !FV_SHIPPING

	/////////////////////////////////////////////////////////////////////////////////
	new FvRobotCfg();
	if(!FvRobotCfg::Instance().Init(argc, argv))
		goto ERR;

	/////////////////////////////////////////////////////////////////////////////////
	g_iShouldWriteToConsole = FvRobotCfg::Instance().GetShouldWriteToConsole();

	/////////////////////////////////////////////////////////////////////////////////
	{
		SYSTEMTIME nowTime;
		GetLocalTime(&nowTime);
		srand(nowTime.wMilliseconds);
		char logName[128] = {0};
		sprintf(logName, "Robot_%04d%02d%02d_%02d%02d%02d_%05u.log",
			nowTime.wYear, nowTime.wMonth, nowTime.wDay, nowTime.wHour, nowTime.wMinute, rand());
		pkMsgFile = new MessageFile(logName);
		pkMsgFile->OpenLog(FvRobotCfg::Instance().GetShouldWriteToLog());
		FvDebugFilter::Instance().AddMessageCallback(pkMsgFile);
	}

	/////////////////////////////////////////////////////////////////////////////////
	new FvLogicDllManager();
	if(!FvLogicDllManager::Instance().LoadDll(LOGICDLLNAME))
	{
		FV_ERROR_MSG( "main: Load dll(%s) failed.\n", LOGICDLLNAME);
		goto ERR;
	}

	/////////////////////////////////////////////////////////////////////////////////
	{//! 放里面,控制析构顺序
		FvRobotMainApp kApp;
		kApp.SetFileLog(pkMsgFile);
		FvRobotSpaceDataManager kSpaceDataMgr;

		if(kApp.Init( argc, argv ))
		{
			kApp.Run();
		}
	}

ERR:
	/////////////////////////////////////////////////////////////////////////////////
	if(FvLogicDllManager::pInstance()) delete FvLogicDllManager::pInstance();
	if(pkMsgFile) FvDebugFilter::Instance().DeleteMessageCallback(pkMsgFile);
	if(pkMsgFile) delete pkMsgFile;
	if(FvRobotCfg::pInstance()) delete FvRobotCfg::pInstance();
	if(pkGroupManager) OGRE_DELETE pkGroupManager;
	if(pkArchive) OGRE_DELETE pkArchive;
	if(pkLog) OGRE_DELETE pkLog;
	FvDebugMsgHelper::Fini();

	return 0;
}

// main.cpp
