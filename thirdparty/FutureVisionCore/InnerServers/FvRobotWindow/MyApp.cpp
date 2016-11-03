#include "MyApp.h"
#include "stdio.h"
#include "Draw2D.h"

#include <FvRobotMainApp.h>
#include <FvRobotClientApp.h>
#include <FvRobotSpaceDataManager.h>
#include <FvRobotEntity.h>
#include <FvRobotCfg.h>
#include <FvFileLog.h>
#include <FvLogicDllManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreFileSystem.h>

MyApp* g_pkApp(NULL);
extern void ConvertWorldToScreen(FvVector2& p, int& x, int& y);


#define LOGICDLLNAME	"RobotScript.dll"
#define LINE_START		10
#define LINE_STEP		12
#define LINE_BOTTOM		20


MyApp::MyApp()
:m_pkMsgFile(NULL)
,m_iSpaceIdx(0)
,m_bResetWindow4Space(true)
,m_bShowEntityProp(false)
,m_bShowEntityID(false)
,m_bShowEntityPos(false)
,m_bShowRobotID(false)
,m_iResetWindowSize(0)
,m_iShowMode(0)
,m_iInfoPage4Help(0)
,m_iInfoPage4Config(0)
{

}

MyApp::~MyApp()
{
	FV_INFO_MSG( "RobotApp has shut down.\n" );

	Clear();
}

bool MyApp::Init(int argc, char* argv[])
{
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
	{
		goto ERR;
	}
	SetWindowSize(FvRobotCfg::Instance().GetWindowSizeWidth(), FvRobotCfg::Instance().GetWindowSizeHeight());
	SetFramePeriod(FvRobotCfg::Instance().GetWindowFramePeriod());
	SetTimerPeriod(FvRobotCfg::Instance().GetWindowTimerPeriod());
	SetWindowTitle("RobotApp");

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
		m_pkMsgFile = new MessageFile(logName);
		m_pkMsgFile->OpenLog(FvRobotCfg::Instance().GetShouldWriteToLog());
		FvDebugFilter::Instance().AddMessageCallback(m_pkMsgFile);
	}

	/////////////////////////////////////////////////////////////////////////////////
	new FvLogicDllManager();
	if(!FvLogicDllManager::Instance().LoadDll(LOGICDLLNAME))
	{
		FV_ERROR_MSG( "main: Load dll(%s) failed.\n", LOGICDLLNAME);
		goto ERR;
	}

	/////////////////////////////////////////////////////////////////////////////////
	new FvRobotMainApp();
	new FvRobotSpaceDataManager();
	FvRobotMainApp::Instance().SetFileLog(m_pkMsgFile);
	if(!FvRobotMainApp::Instance().Init(argc, argv))
		goto ERR;

	return true;

	/////////////////////////////////////////////////////////////////////////////////
ERR:
	Clear();
	return false;
}

void MyApp::Clear()
{
	if(FvRobotMainApp::pInstance()) delete FvRobotMainApp::pInstance();
	if(FvRobotSpaceDataManager::pInstance()) delete FvRobotSpaceDataManager::pInstance();
	if(FvLogicDllManager::pInstance()) delete FvLogicDllManager::pInstance();
	if(m_pkMsgFile) FvDebugFilter::Instance().DeleteMessageCallback(m_pkMsgFile);
	if(m_pkMsgFile) delete m_pkMsgFile; m_pkMsgFile=NULL;
	if(FvRobotCfg::pInstance()) delete FvRobotCfg::pInstance();
	if(Ogre::ResourceGroupManager::getSingletonPtr()) OGRE_DELETE Ogre::ResourceGroupManager::getSingletonPtr();
	if(Ogre::ArchiveManager::getSingletonPtr()) OGRE_DELETE Ogre::ArchiveManager::getSingletonPtr();
	if(Ogre::LogManager::getSingletonPtr()) OGRE_DELETE Ogre::LogManager::getSingletonPtr();
	FvDebugMsgHelper::Fini();
}

void MyApp::Tick()
{
	if(!FvRobotMainApp::pInstance())
		return;

	try
	{
		FvRobotMainApp::pInstance()->Nub().ProcessFrame();
	}
	catch (FvNetNubException & ne)
	{
		FvNetAddress addr = FvNetAddress::NONE;
		bool hasAddr = ne.GetAddress( addr );

		//switch (ne.Reason())
		//{
		//default:
		if (hasAddr)
		{
			char buf[ 256 ];
			_snprintf( buf, sizeof( buf ),
				"ProcessFrame( %s )", addr.c_str() );

			FvRobotMainApp::pInstance()->Nub().ReportException( ne , buf );
		}
		else
		{
			FvRobotMainApp::pInstance()->Nub().ReportException(
				ne, "ProcessFrame" );
		}
	}
}

void MyApp::Draw()
{
	if(!FvRobotMainApp::pInstance())
		return;

	//! 修改窗口大小
	if(m_iResetWindowSize)
	{
		int w,h;
		GetWindowSize(w, h);

		if(m_iResetWindowSize & 0x1)
			w = FvRobotCfg::Instance().GetWindowSizeWidth();
		if(m_iResetWindowSize & 0x2)
			h = FvRobotCfg::Instance().GetWindowSizeHeight();

		SetWindowSize(w, h);
		m_iResetWindowSize = 0;
	}

	if(m_iShowMode == 0)
	{
		if(FvRobotCfg::Instance().GetSingleMode())
			DrawSingleMode();
		else
			DrawGlobalMode();
	}
	else if(m_iShowMode == 1)
	{
		DrawHelp();
	}
	else
	{
		DrawConfig();
	}
}

void MyApp::OnKeyDown(int keyVal, bool bRep)
{
	if(keyVal == KV_N)
	{
		++m_iSpaceIdx;
		m_bResetWindow4Space = true;
	}
	else if(keyVal == KV_P)
	{
		m_bShowEntityProp = !m_bShowEntityProp;
	}
	else if(keyVal == KV_I)
	{
		m_bShowEntityID = !m_bShowEntityID;
	}
	else if(keyVal == KV_Z)
	{
		m_bShowEntityPos = !m_bShowEntityPos;
	}
	else if(keyVal == KV_R)
	{
		m_bShowRobotID = !m_bShowRobotID;
	}
	else if(keyVal == KV_H)
	{
		if(m_iShowMode == 1)
			m_iShowMode = 0;
		else
			m_iShowMode = 1;
	}
	else if(keyVal == KV_C)
	{
		if(m_iShowMode == 2)
			m_iShowMode = 0;
		else
			m_iShowMode = 2;
	}
	else if(keyVal == KV_S)
	{
		FvRobotCfg::Instance().SetSingleMode(!FvRobotCfg::Instance().GetSingleMode());
		FvRobotMainApp::Instance().ChangeSingleMode();
	}
	else if(keyVal == KV_PAGE_UP)
	{
		if(m_iShowMode == 1 && m_iInfoPage4Help>0)
			--m_iInfoPage4Help;
		else if(m_iShowMode == 2 && m_iInfoPage4Config>0)
			--m_iInfoPage4Config;
	}
	else if(keyVal == KV_PAGE_DOWN)
	{
		if(m_iShowMode == 1)
			++m_iInfoPage4Help;
		else if(m_iShowMode == 2)
			++m_iInfoPage4Config;
	}

	if(FvRobotCfg::Instance().GetSingleMode())
	{
		FvRobotClientAppPtr spApp = FvRobotMainApp::Instance().GetRobotInSingleMode();
		if(spApp)
			spApp->OnKeyDown(keyVal, bRep);
	}
}

void MyApp::OnKeyUp(int keyVal)
{
	if(FvRobotCfg::Instance().GetSingleMode())
	{
		FvRobotClientAppPtr spApp = FvRobotMainApp::Instance().GetRobotInSingleMode();
		if(spApp)
			spApp->OnKeyUp(keyVal);
	}
}

void MyApp::OnCmd(const char* pcCmd)
{
	FvRobotCfg::Result& kResult = FvRobotMainApp::Instance().DoCmd(pcCmd);

	if(kResult.empty())
		return;

	for(int i=0; i<(int)kResult.size(); ++i)
	{
		switch(kResult[i])
		{
		case FvRobotCfg::SingleMode:
			{
				m_bResetWindow4Space = true;
			}
			break;
		case FvRobotCfg::WindowSizeWidth:
			{
				m_iResetWindowSize |= 0x1;
			}
			break;
		case FvRobotCfg::WindowSizeHeight:
			{
				m_iResetWindowSize |= 0x2;
			}
			break;
		case FvRobotCfg::WindowFrameFreq:
			{
				SetFramePeriod(FvRobotCfg::Instance().GetWindowFramePeriod());
			}
			break;
		case FvRobotCfg::WindowTimerFreq:
			{
				SetTimerPeriod(FvRobotCfg::Instance().GetWindowTimerPeriod());
			}
			break;
		}
	}
}

void MyApp::DrawGlobalMode()
{
	FvRobotMainApp::SpaceList& kSpaceList = FvRobotMainApp::pInstance()->GetSpaceList();
	FvRobotMainApp::SpaceEntityList& kSpaceEntityList = FvRobotMainApp::pInstance()->GetSpaceEntityList();

	int lineStart(LINE_START);
	int lineStep(LINE_STEP);

	//! 显示地图信息
	DrawString(0, lineStart, CLR_WHITE, "-------Global Mode-------");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ShowHelpInfo: H");
	lineStart += lineStep;
	if(1)
	{
		int iCtrlers(0), iOthers(0);
		if(kSpaceEntityList.size())
		{
			FvRobotMainApp::SpaceEntity* pkSpaceEntity(NULL);
			if(m_iSpaceIdx >= (int)kSpaceEntityList.size())
			{
				m_bResetWindow4Space = true;
				m_iSpaceIdx = m_iSpaceIdx % kSpaceEntityList.size();
			}
			DrawString(0, lineStart, CLR_WHITE, "Spaces:%d,Idx:%d", kSpaceEntityList.size(), m_iSpaceIdx);
			lineStart += lineStep;
			int idx(0);
			FvRobotMainApp::SpaceEntityList::iterator itrB = kSpaceEntityList.begin();
			FvRobotMainApp::SpaceEntityList::iterator itrE = kSpaceEntityList.end();
			while(itrB != itrE)
			{
				if(idx == m_iSpaceIdx)
				{
					pkSpaceEntity = &(*itrB);
					break;
				}
				++itrB;
				++idx;
			}
			DrawString(0, lineStart, CLR_WHITE, "id:%d, type:%d", pkSpaceEntity->m_iSpaceID, pkSpaceEntity->m_uiSpaceType);
			lineStart += lineStep;

			//! 显示Space
			{
				const FvRobotMainApp::SpaceInfo* pkSpaceInfo = FvRobotMainApp::Instance().FindSpaceInfo(pkSpaceEntity->m_uiSpaceType);
				FV_ASSERT(pkSpaceInfo);
				if(m_bResetWindow4Space)
				{
					m_bResetWindow4Space = false;
					SetWindowFit2Rect(pkSpaceInfo->kSpaceRect);
				}

				for(int i=0; i<(int)pkSpaceInfo->kCells.size(); ++i)
				{
					DrawRect(pkSpaceInfo->kCells[i], CLR_YELLOW);
				}
				DrawRect(pkSpaceInfo->kSpaceRect, CLR_YELLOW);
			}

			//! 显示实体
			if(1)
			{
				FvRobotMainApp::Entities& kEntityMap = pkSpaceEntity->m_kEntities;
				//! 显示实体数量
				DrawString(0, lineStart, CLR_WHITE, "Entities:%d", kEntityMap.size());
				lineStart += lineStep;
				FvRobotMainApp::Entities::iterator itrB = kEntityMap.begin();
				FvRobotMainApp::Entities::iterator itrE = kEntityMap.end();
				while(itrB != itrE)
				{
					FvEntity* pkEntity = itrB->second;
					p2Clr clr;
					const FvVector3& p3 = pkEntity->GetPos();
					FvVector2 p2(p3.x, p3.y);
					int x,y;
					char charID[32] = {0};
					char robotID[32] = {0};
					char charPos[64] = {0};
					bool bConvertPos(false);
					if(m_bShowEntityID)
					{
						bConvertPos = true;
						sprintf(charID, "%d", pkEntity->GetEntityID());
					}
					if(m_bShowRobotID)
					{
						bConvertPos = true;
						sprintf(robotID, "%d", pkEntity->GetRobotID());
					}
					if(m_bShowEntityPos)
					{
						bConvertPos = true;
						sprintf(charPos, "%.2f,%.2f,%.2f", p3.x, p3.y, p3.z);
					}
					if(bConvertPos)
					{
						ConvertWorldToScreen(p2, x, y);
					}

					if(pkEntity->IsCtrler())//! ctrler
					{
						++iCtrlers;
						clr = CLR_RED;
						DrawPoint(p3, clr);
						if(m_bShowEntityID)
						{
							DrawString(x, y, clr, charID);
						}
						if(m_bShowRobotID)
						{
							DrawString(x, y-lineStep, clr, robotID);
						}
						if(m_bShowEntityPos)
						{
							DrawString(x, y+lineStep, clr, charPos);
						}
					}
					else//! others
					{
						++iOthers;
						clr = CLR_BLUE;
						DrawPoint(p3, clr);
						if(m_bShowEntityID)
						{
							DrawString(x, y, clr, charID);
						}
						if(m_bShowEntityPos)
						{
							DrawString(x, y+lineStep, clr, charPos);
						}
					}

					//! 显示非Ctrler实体的属性,测试Lod
					if(m_bShowEntityProp)
					{
/**
						FvEntity* pkOther = pkEntity;
						DrawString(0, lineStart, CLR_WHITE, "-------------");
						lineStart += lineStep;
						DrawString(0, lineStart, CLR_WHITE, "Entity:%d", pkOther->GetEntityID());
						lineStart += lineStep;
						const FvEntityDescription* pkEntityDes = pkOther->GetEntityDes();
						FvUInt32 cnt = pkEntityDes->PropertyCount();
						for(FvUInt32 i=0; i<cnt; ++i)
						{
							FvDataDescription* pkDataDes = pkEntityDes->Property(i);
							FvObjPtr spObj = pkOther->FindProperty(i);
							if(!spObj)
								continue;
							switch(spObj->RTTI())
							{
							case FVOBJ_RTTI_INT8:
								{
									FvInt8 v = FVOBJ_GETVAL(spObj, FvObjInt8);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_UINT8:
								{
									FvUInt8 v = FVOBJ_GETVAL(spObj, FvObjUInt8);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_INT16:
								{
									FvInt16 v = FVOBJ_GETVAL(spObj, FvObjInt16);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_UINT16:
								{
									FvUInt16 v = FVOBJ_GETVAL(spObj, FvObjUInt16);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_INT32:
								{
									FvInt32 v = FVOBJ_GETVAL(spObj, FvObjInt32);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_UINT32:
								{
									FvUInt32 v = FVOBJ_GETVAL(spObj, FvObjUInt32);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_INT64:
								{
									FvInt64 v = FVOBJ_GETVAL(spObj, FvObjInt64);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%I64d", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_UINT64:
								{
									FvUInt64 v = FVOBJ_GETVAL(spObj, FvObjUInt64);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%I64u", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_FLOAT:
								{
									float v = FVOBJ_GETVAL(spObj, FvObjFloat);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%f", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_DOUBLE:
								{
									double v = FVOBJ_GETVAL(spObj, FvObjDouble);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%f", pkDataDes->Name().c_str(), v);
								}
								break;
							case FVOBJ_RTTI_STRING:
								{
									FvString v = FVOBJ_GETVAL(spObj, FvObjString);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%s", pkDataDes->Name().c_str(), v.c_str());
								}
								break;
							case FVOBJ_RTTI_MAILBOX:
								{
									FVOBJ_CONVERT_P(spObj, FvObjMailBox, pMB);
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%s", pkDataDes->Name().c_str(), pMB->c_str());
								}
								break;
							default:
								{
									DrawString(0, lineStart, CLR_WHITE, "PropName:%s", pkDataDes->Name().c_str());
								}
								break;
							}
							lineStart += lineStep;
						}
**/
					}

					++itrB;
				}
			}

			DrawString(0, lineStart, CLR_WHITE, "Ctrler:%d, Others:%d", iCtrlers, iOthers);
			lineStart += lineStep;
		}
		else
		{
			DrawString(0, lineStart, CLR_WHITE, "Spaces:%d,Idx:%d", 0, 0);
			lineStart += lineStep;
			DrawString(0, lineStart, CLR_WHITE, "Entities:%d", 0);
			lineStart += lineStep;
			DrawString(0, lineStart, CLR_WHITE, "Ctrler:%d, Others:%d", 0, 0);
			lineStart += lineStep;
		}
	}
}

void MyApp::DrawSingleMode()
{
	FvRobotClientAppPtr spApp = FvRobotMainApp::Instance().GetRobotInSingleMode();

	int lineStart(LINE_START);
	int lineStep(LINE_STEP);

	//! 显示地图信息
	DrawString(0, lineStart, CLR_WHITE, "-------Single Mode-------");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ShowHelpInfo: H");
	lineStart += lineStep;
	if(!spApp)
	{
		DrawString(0, lineStart, CLR_WHITE, "Err: No Selected Robot");
		lineStart += lineStep;
		return;
	}
	if(1)
	{
		DrawString(0, lineStart, CLR_WHITE, "ServerTime:%.1f", spApp->GetServerConnection()->ServerTime());
		lineStart += lineStep;

		DrawString(0, lineStart, CLR_WHITE, "ServerToDScaling:%.2f,ServerToD:%.2f", spApp->GetToDScaling(), spApp->GetToD());
		lineStart += lineStep;

		DrawString(0, lineStart, CLR_WHITE, "SpaceID:%d, Type:%d, RobotID:%d", spApp->SpaceID(), spApp->SpaceType(), spApp->RobotID());
		lineStart += lineStep;

		//! 显示Space
		const FvRobotMainApp::SpaceInfo* pkSpaceInfo = FvRobotMainApp::Instance().FindSpaceInfo(spApp->SpaceType());
		if(!pkSpaceInfo)
		{
			DrawString(0, lineStart, CLR_WHITE, "Err:No Space Info");
			lineStart += lineStep;
			return;
		}
		else
		{
			if(m_bResetWindow4Space)
			{
				m_bResetWindow4Space = false;
				SetWindowFit2Rect(pkSpaceInfo->kSpaceRect);
			}

			for(int i=0; i<(int)pkSpaceInfo->kCells.size(); ++i)
			{
				DrawRect(pkSpaceInfo->kCells[i], CLR_YELLOW);
			}
			DrawRect(pkSpaceInfo->kSpaceRect, CLR_YELLOW);
		}

		//! 显示实体
		if(1)
		{
			const FvRobotClientApp::EntityMap& kEntityMap = spApp->GetEntityMap();
			//! 显示实体数量
			DrawString(0, lineStart, CLR_WHITE, "Entities:%d", kEntityMap.size());
			lineStart += lineStep;
			FvRobotClientApp::EntityMap::const_iterator itrB = kEntityMap.begin();
			FvRobotClientApp::EntityMap::const_iterator itrE = kEntityMap.end();
			while(itrB != itrE)
			{
				FvEntity* pkEntity = itrB->second;
				p2Clr clr;
				const FvVector3& p3 = pkEntity->GetPos();
				FvVector2 p2(p3.x, p3.y);
				int x,y;
				char charID[32] = {0};
				char robotID[32] = {0};
				char charPos[64] = {0};
				bool bConvertPos(false);
				if(m_bShowEntityID)
				{
					bConvertPos = true;
					sprintf(charID, "%d", pkEntity->GetEntityID());
				}
				if(m_bShowRobotID)
				{
					bConvertPos = true;
					sprintf(robotID, "%d", pkEntity->GetRobotID());
				}
				if(m_bShowEntityPos)
				{
					bConvertPos = true;
					sprintf(charPos, "%.2f,%.2f,%.2f", p3.x, p3.y, p3.z);
				}
				if(bConvertPos)
				{
					ConvertWorldToScreen(p2, x, y);
				}

				if(pkEntity->IsCtrler())//! ctrler
				{
					clr = CLR_RED;
					DrawPoint(p3, clr);
					if(m_bShowEntityID)
					{
						DrawString(x, y, clr, charID);
					}
					if(m_bShowRobotID)
					{
						DrawString(x, y-lineStep, clr, robotID);
					}
					if(m_bShowEntityPos)
					{
						DrawString(x, y+lineStep, clr, charPos);
					}
				}
				else//! others
				{
					clr = CLR_BLUE;
					DrawPoint(p3, clr);
					if(m_bShowEntityID)
					{
						DrawString(x, y, clr, charID);
					}
					if(m_bShowEntityPos)
					{
						DrawString(x, y+lineStep, clr, charPos);
					}
				}

				//! 显示非Ctrler实体的属性,测试Lod
				if(m_bShowEntityProp)
				{
/**
					FvEntity* pkOther = pkEntity;
					DrawString(0, lineStart, CLR_WHITE, "-------------");
					lineStart += lineStep;
					DrawString(0, lineStart, CLR_WHITE, "Entity:%d", pkOther->GetEntityID());
					lineStart += lineStep;
					const FvEntityDescription* pkEntityDes = pkOther->GetEntityDes();
					FvUInt32 cnt = pkEntityDes->PropertyCount();
					for(FvUInt32 i=0; i<cnt; ++i)
					{
						FvDataDescription* pkDataDes = pkEntityDes->Property(i);
						FvObjPtr spObj = pkOther->FindProperty(i);
						if(!spObj)
							continue;
						switch(spObj->RTTI())
						{
						case FVOBJ_RTTI_INT8:
							{
								FvInt8 v = FVOBJ_GETVAL(spObj, FvObjInt8);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_UINT8:
							{
								FvUInt8 v = FVOBJ_GETVAL(spObj, FvObjUInt8);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_INT16:
							{
								FvInt16 v = FVOBJ_GETVAL(spObj, FvObjInt16);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_UINT16:
							{
								FvUInt16 v = FVOBJ_GETVAL(spObj, FvObjUInt16);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_INT32:
							{
								FvInt32 v = FVOBJ_GETVAL(spObj, FvObjInt32);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_UINT32:
							{
								FvUInt32 v = FVOBJ_GETVAL(spObj, FvObjUInt32);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%d", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_INT64:
							{
								FvInt64 v = FVOBJ_GETVAL(spObj, FvObjInt64);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%I64d", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_UINT64:
							{
								FvUInt64 v = FVOBJ_GETVAL(spObj, FvObjUInt64);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%I64u", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_FLOAT:
							{
								float v = FVOBJ_GETVAL(spObj, FvObjFloat);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%f", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_DOUBLE:
							{
								double v = FVOBJ_GETVAL(spObj, FvObjDouble);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%f", pkDataDes->Name().c_str(), v);
							}
							break;
						case FVOBJ_RTTI_STRING:
							{
								FvString v = FVOBJ_GETVAL(spObj, FvObjString);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%s", pkDataDes->Name().c_str(), v.c_str());
							}
							break;
						case FVOBJ_RTTI_MAILBOX:
							{
								FVOBJ_CONVERT_P(spObj, FvObjMailBox, pMB);
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s,Val:%s", pkDataDes->Name().c_str(), pMB->c_str());
							}
							break;
						default:
							{
								DrawString(0, lineStart, CLR_WHITE, "PropName:%s", pkDataDes->Name().c_str());
							}
							break;
						}
						lineStart += lineStep;
					}
**/
				}

				++itrB;
			}
		}
	}
}

void MyApp::DrawHelp()
{
	int lineStart(LINE_START);
	int lineStep(LINE_STEP);

	//! 显示地图信息
	DrawString(0, lineStart, CLR_WHITE, "-------Help Info-------");
	lineStart += lineStep;

	DrawString(0, lineStart, CLR_WHITE, "ShowHelpInfo: H");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ResetSpaceRect: Home");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "Quit: ESC");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ShowConfigValue: C");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "SetSingleMode: S");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ChangeNextSpace: N");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ShowEntityProp: P");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ShowEntityID: I");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ShowRobotID: R");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ShowEntityPos: Z");
	lineStart += lineStep;

	DrawString(0, lineStart, CLR_WHITE, "-------Cmd Info-------");
	lineStart += lineStep;

	int w,h;
	GetWindowSize(w,h);
	int infoHeight = h-lineStart-LINE_BOTTOM;
	if(infoHeight < lineStep)
		return;

	const FvRobotCfg::Items& kItems = FvRobotCfg::Instance().GetItems();
	if(kItems.empty())
		return;

	int iItemsPerPage = infoHeight / lineStep;
	int iPages = (kItems.size() + iItemsPerPage-1) / iItemsPerPage;
	if(m_iInfoPage4Help >= iPages)
		m_iInfoPage4Help = iPages-1;

	int iStart = m_iInfoPage4Help*iItemsPerPage;
	int i = iStart;
	for(; i<iStart+iItemsPerPage && i<(int)kItems.size(); ++i)
	{
		DrawString(0, lineStart, CLR_WHITE, kItems[i].GetCmdStr().c_str());
		lineStart += lineStep;
	}

	if(m_iInfoPage4Help==0 && m_iInfoPage4Help<iPages-1)
	{
		DrawString(0, lineStart, CLR_WHITE, "-------Press Page Down-------");
		lineStart += lineStep;
	}
	else if(0<m_iInfoPage4Help && m_iInfoPage4Help<iPages-1)
	{
		DrawString(0, lineStart, CLR_WHITE, "-------Press Page UP/Down-------");
		lineStart += lineStep;
	}
	else if(0<m_iInfoPage4Help && m_iInfoPage4Help==iPages-1)
	{
		DrawString(0, lineStart, CLR_WHITE, "-------Press Page UP-------");
		lineStart += lineStep;
	}
}

void MyApp::DrawConfig()
{
	int lineStart(LINE_START);
	int lineStep(LINE_STEP);

	//! 显示地图信息
	DrawString(0, lineStart, CLR_WHITE, "-------Config Info-------");
	lineStart += lineStep;

	int w,h;
	GetWindowSize(w,h);
	int infoHeight = h-lineStart-LINE_BOTTOM;
	if(infoHeight < lineStep)
		return;

	const FvRobotCfg::Items& kItems = FvRobotCfg::Instance().GetItems();
	if(kItems.empty())
		return;

	int iItemsPerPage = infoHeight / lineStep;
	int iPages = (kItems.size() + iItemsPerPage-1) / iItemsPerPage;
	if(m_iInfoPage4Config >= iPages)
		m_iInfoPage4Config = iPages-1;

	int iStart = m_iInfoPage4Config*iItemsPerPage;
	int i = iStart;
	for(; i<iStart+iItemsPerPage && i<(int)kItems.size(); ++i)
	{
		DrawString(0, lineStart, CLR_WHITE, kItems[i].GetValueStr().c_str());
		lineStart += lineStep;
	}

	if(m_iInfoPage4Config==0 && m_iInfoPage4Config<iPages-1)
	{
		DrawString(0, lineStart, CLR_WHITE, "-------Press Page Down-------");
		lineStart += lineStep;
	}
	else if(0<m_iInfoPage4Config && m_iInfoPage4Config<iPages-1)
	{
		DrawString(0, lineStart, CLR_WHITE, "-------Press Page UP/Down-------");
		lineStart += lineStep;
	}
	else if(0<m_iInfoPage4Config && m_iInfoPage4Config==iPages-1)
	{
		DrawString(0, lineStart, CLR_WHITE, "-------Press Page UP-------");
		lineStart += lineStep;
	}
}

App* CreateApp()
{
	if(!g_pkApp)
		g_pkApp = new MyApp();
	return g_pkApp;
}

void DestroyApp()
{
	if(g_pkApp)
	{
		delete g_pkApp;
		g_pkApp = NULL;
	}
}

