#include "MyApp.h"
#include "stdio.h"
#include "Draw2D.h"

#include <../FvCell/FvCellEntityManager.h>
#include <FvTimestamp.h>
#include <FvNetLoggerMessageForwarder.h>
#include <FvXMLSection.h>
#include <FvServerConfig.h>
#include <FvNetWatcherGlue.h>
#include <FvNetPortMap.h>
#include <FvLogicDllManager.h>
#include <FvFileLog.h>
#include <FvServerResource.h>
#include <FvSpace.h>
#include <FvCell.h>


MyApp* g_pkApp(NULL);
extern void ConvertWorldToScreen(FvVector2& p, int& x, int& y);


#define LOGICDLLNAME	"CellScript.dll"


MyApp::MyApp()
:pkServerResource(NULL)
,pkNub(NULL)
,pkMsgFile(NULL)
,pkMgr(NULL)
,m_bSetTitle(false)
,m_iSpaceIdx(0)
,m_bResetWindow4Space(true)
,m_bShowAoi(true)
,m_bShowLod(false)
,m_bShowInnerCellBorder(false)
,m_bShowEntityProp(false)
,m_bShowEntityID(false)
,m_bShowEntityPos(false)
,m_bShowTraps(false)
{

}

MyApp::~MyApp()
{
	FV_INFO_MSG( "CellApp has shut down.\n" );

	pkMgr->Nub().ReportPendingExceptions( true /* reportBelowThreshold */ );
	pkMgr->Nub().ReportPendingExceptions( true /* reportBelowThreshold */ );

	delete pkMgr; pkMgr = NULL;
	delete FvLogicDllManager::pInstance();
	FvDataType::ClearStaticsForReload();
	FvMetaDataType::Fini();
	FV_CLOSE_LOG_WITH_FILE(cellApp, pkMsgFile);
	FvDebugMsgHelper::Fini();
	delete pkNub;
	delete pkServerResource;
}

FvUInt16 GetUserID( int iArgc, char *pcArgv[] )
{
	FvUInt16 uiUserID = FV_USER_ID;

	for (int i = 1; i < iArgc -1; ++i)
	{
		if (_stricmp( pcArgv[i], "-UserID" ) == 0)
		{
			uiUserID = (FvUInt16)atoi( pcArgv[ i + 1 ] );
		}
	}

	return uiUserID;
}

bool MyApp::Init(int argc, char* argv[])
{
	SetWindowTitle("CellApp");

	pkServerResource = new FvServerResource();
	FvServerResource::Init( argc, argv );
	FvServerConfig::Init( argc, argv );

	int iWidth = 64;
	int	iHeight = 72;
	int iFramePeriod = 42;
	int	iTimerPeriod = 10;
	FvServerConfig::Update("cellApp/WindowSizeWidth", iWidth);
	FvServerConfig::Update("cellApp/WindowSizeHeight", iHeight);
	FvServerConfig::Update("cellApp/WindowFramePeriod", iFramePeriod);
	FvServerConfig::Update("cellApp/WindowTimerPeriod", iTimerPeriod);
	SetWindowSize(iWidth, iHeight);
	SetFramePeriod(iFramePeriod);
	SetTimerPeriod(iTimerPeriod);

	FV_WRITE_TO_CONSOLE(cellApp);
	FV_OPEN_LOG_WITH_FILE(cellApp, pkMsgFile);

	FvUInt16 uiUserID = GetUserID(argc, argv);
	pkNub = new FvNetNub(FV_MACHINE_PORT, uiUserID, 0, FV_INTERNAL_INTERFACE(cellApp));

	new FvLogicDllManager();
	if(!FvLogicDllManager::Instance().LoadDll(LOGICDLLNAME))
	{
		delete FvLogicDllManager::pInstance();
		FV_ERROR_MSG( "main: Load dll(%s) failed.\n", LOGICDLLNAME);
		return false;
	}

	pkMgr = new FvEntityManager(*pkNub);
	StampsPerSecond();

	if (!pkMgr->Init( argc, argv ))
	{
		FV_ERROR_MSG( "main: init failed.\n" );
		return false;
	}

	FV_INFO_MSG( "---- CellApp is running ----\n" );

	return true;
}

void MyApp::Tick()
{
	if(!pkMgr)
		return;

	bool gotAnyPackets = false;
	try
	{
		bool gotAPacket = false;
		do
		{
			gotAPacket = pkMgr->Nub().ProcessPendingEvents();

			gotAnyPackets |= gotAPacket;
		}
		while (gotAPacket);
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
				"processContinuously( %s )", addr.c_str() );

			pkMgr->Nub().ReportException( ne , buf );
		}
		else
		{
			pkMgr->Nub().ReportException(
				ne, "processContinuously" );
		}
	}
}

void MyApp::Draw()
{
	if(!pkMgr || !pkMgr->HasStarted())
		return;

	//! 更换title
	if(!m_bSetTitle)
	{
		m_bSetTitle = true;
		char title[64] = {0};
		sprintf(title, "CellApp:%d", pkMgr->GetAppID());
		SetWindowTitle(title);
	}

	int lineStart(10);
	int lineStep(12);

	//! 显示地图信息
	DrawString(0, lineStart, CLR_WHITE, "-------------");
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "StampsPerSecond:%I64u", StampsPerSecond());
	lineStart += lineStep;
	DrawString(0, lineStart, CLR_WHITE, "ServerTime:%.1f", FvEntityManager::Instance().GameTimeInSeconds());
	lineStart += lineStep;
	if(1)
	{
		int iAois(0), iReals(0), iGhosts(0);
		FvSpace* pkSpace(NULL);
		FvEntityManager::SpaceMap& kSpaceMap = pkMgr->GetSpaceMap();
		if(kSpaceMap.size())
		{
			if(m_iSpaceIdx >= (int)kSpaceMap.size())
			{
				m_bResetWindow4Space = true;
				m_iSpaceIdx = m_iSpaceIdx % kSpaceMap.size();
			}
			DrawString(0, lineStart, CLR_WHITE, "Spaces:%d,Idx:%d", kSpaceMap.size(), m_iSpaceIdx);
			lineStart += lineStep;
			int idx(0);
			FvEntityManager::SpaceMap::iterator itrB = kSpaceMap.begin();
			FvEntityManager::SpaceMap::iterator itrE = kSpaceMap.end();
			while(itrB != itrE)
			{
				if(idx == m_iSpaceIdx)
				{
					pkSpace = itrB->second;
					break;
				}
				++itrB;
				++idx;
			}
			DrawString(0, lineStart, CLR_WHITE, "RunToD:%d,ToDScaling:%.2f,ToD:%.2f", pkSpace->IsToDRunning(), pkSpace->GetToDScaling(), pkSpace->GetToD());
			lineStart += lineStep;
			DrawString(0, lineStart, CLR_WHITE, "id:%d, type:%d, Cells:%d, CellIdx:%d", pkSpace->SpaceID(), pkSpace->SpaceType(), pkSpace->GetCellInfos().size(), pkSpace->GetCellInfo()->m_uiIdx);
			lineStart += lineStep;

			//! 显示Cell
			FvCell* pkCell = pkSpace->GetCell();
			FvCellRect kCellRect = pkCell->GetCellInfo()->m_pkCellData->m_kRect;
			if(m_bResetWindow4Space)
			{
				m_bResetWindow4Space = false;
				FvCellRect outRect(kCellRect, pkCell->CellHysteresisSize());
				FvVector4 rect;
				outRect.ConvertToVector4(rect);
				SetWindowFit2Rect(rect);
			}

			FvCellRect outRect(kCellRect, pkCell->CellHysteresisSize());
			FvCellRect inRect(kCellRect, -pkCell->CellHysteresisSize());
			FvVector4 rect;
			outRect.ConvertToVector4(rect);
			DrawRect(rect, CLR_RED);
			kCellRect.ConvertToVector4(rect);
			DrawRect(rect, CLR_YELLOW);
			if(m_bShowInnerCellBorder)
			{
				inRect.ConvertToVector4(rect);
				DrawRect(rect, CLR_WHITE);
			}

			//! 显示实体
			if(1)
			{
				FvSpace::SpaceEntities& kEntities = pkSpace->GetSpaceEntities();
				//! 显示实体数量
				DrawString(0, lineStart, CLR_WHITE, "Entities:%d", kEntities.size());
				lineStart += lineStep;
				FvSpace::SpaceEntities::iterator itrB = kEntities.begin();
				FvSpace::SpaceEntities::iterator itrE = kEntities.end();
				while(itrB != itrE)
				{
					FvEntity* pkEntity = *itrB;
					p2Clr clr;
					const FvVector3& p3 = pkEntity->GetPos();
					FvVector2 p2(p3.x, p3.y);
					int x,y;
					char charID[32] = {0};
					char charPos[64] = {0};
					bool bConvertPos(false);
					if(m_bShowEntityID)
					{
						bConvertPos = true;
						sprintf(charID, "%d", pkEntity->GetEntityID());
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

					if(pkEntity->IsReal())
					{
						if(pkEntity->HasWitness())//! real+witness
						{
							++iAois;
							clr = CLR_RED;
							DrawPoint(p3, clr);
							if(m_bShowEntityID)
							{
								DrawString(x, y, clr, charID);
							}
							if(m_bShowEntityPos)
							{
								DrawString(x, y+lineStep, clr, charPos);
							}
							//! Aoi
							if(m_bShowAoi)
							{
								float fRadius(0.0f);
								float fHyst(0.0f);
								fRadius = pkEntity->GetAoIVision();
								DrawCircle(p3, fRadius, CLR_RED);
								//DrawCircle(p3, fRadius+fHyst, CLR_WHITE);
							}
							//! Lod
							if(m_bShowLod)
							{
								//const FvDataLoDLevels kLodLevels = pkEntity->GetEntityDes()->LodLevels();
								//p2Clr clr1(0.0f, 1.0f, 0.0f);
								//p2Clr step1(0.0f, -0.12f, 0.0f);
								//p2Clr clr2(1.0f, 1.0f, 0.0f);
								//p2Clr step2(-0.12f, -0.12f, 0.0f);
								//for(FvInt32 i=0; i<kLodLevels.Size(); ++i)
								//{
								//	const FvDataLoDLevel& kLodLevel = kLodLevels.GetLevel(i);
								//	DrawCircle(p3, kLodLevel.Start(), clr1);
								//	DrawCircle(p3, kLodLevel.Start()+kLodLevel.Hyst(), clr2);
								//	clr1.Add(step1);
								//	clr2.Add(step2);
								//}
							}
						}
						else//! real
						{
							++iReals;
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

						//! Traps
						if(m_bShowTraps)
						{
							std::vector<float> kTraps;
							pkEntity->GetTraps(kTraps);
							for(int i=0; i<(int)kTraps.size(); ++i)
							{
								DrawCircle(p3, kTraps[i], CLR_YELLOW);
							}
						}
					}
					else//! ghost
					{
						++iGhosts;
						clr = CLR_GREEN;
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
						//! 新版的FvObj不支持这种操作
/**
						FvEntity* pkOther = pkEntity;
						DrawString(0, lineStart, CLR_WHITE, "-------------");
						lineStart += lineStep;
						DrawString(0, lineStart, CLR_WHITE, "Entity:%d", pkOther->GetEntityID());
						lineStart += lineStep;
						//DrawString(0, lineStart, CLR_WHITE, "Visible:%d", pkOther->IsVisible());
						//lineStart += lineStep;
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
									FVOBJ_CONVERT_P(spObj, FvMailBox, pMB);
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

			DrawString(0, lineStart, CLR_WHITE, "Aoi:%d, Real:%d, Ghost:%d", iAois, iReals, iGhosts);
			lineStart += lineStep;
		}
		else
		{
			DrawString(0, lineStart, CLR_WHITE, "Spaces:%d,Idx:%d", 0, 0);
			lineStart += lineStep;
			DrawString(0, lineStart, CLR_WHITE, "Entities:%d", 0);
			lineStart += lineStep;
			DrawString(0, lineStart, CLR_WHITE, "Aoi:%d, Real:%d, Ghost:%d", 0, 0, 0);
			lineStart += lineStep;
		}
	}
}

void MyApp::OnKeyDown(int keyVal, bool bRep)
{
	//printf("Key Down, %d\n", keyVal);
	if(keyVal == 'N')
	{
		++m_iSpaceIdx;
		m_bResetWindow4Space = true;
	}
	else if(keyVal == 'A')
	{
		m_bShowAoi = !m_bShowAoi;
	}
	else if(keyVal == 'L')
	{
		m_bShowLod = !m_bShowLod;
	}
	else if(keyVal == 'B')
	{
		m_bShowInnerCellBorder = !m_bShowInnerCellBorder;
	}
	else if(keyVal == 'P')
	{
		m_bShowEntityProp = !m_bShowEntityProp;
	}
	else if(keyVal == 'I')
	{
		m_bShowEntityID = !m_bShowEntityID;
	}
	else if(keyVal == 'Z')
	{
		m_bShowEntityPos = !m_bShowEntityPos;
	}
	else if(keyVal == 'T')
	{
		m_bShowTraps = !m_bShowTraps;
	}
}

void MyApp::OnKeyUp(int keyVal)
{
	//printf("Key Up, %d\n", keyVal);
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