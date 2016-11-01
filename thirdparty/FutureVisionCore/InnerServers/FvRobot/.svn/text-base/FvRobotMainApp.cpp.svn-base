#include <FvDebug.h>
#include "FvRobotMainApp.h"
#include "FvRobotClientApp.h"
#include "FvRobotMovementController.h"
#include "FvRobotInterface.h"
#include "FvRobotEntity.h"
#include <FvEntityDefConstants.h>
#include <signal.h>
#include <memory>
#include <FvLogicDllManager.h>
#include <FvFileLog.h>
#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreFileSystem.h>


FV_DECLARE_DEBUG_COMPONENT2( "RoBots", 0 )

FV_SINGLETON_STORAGE( FvRobotMainApp )

void IntSignalHandler( int /*sigNum*/ )
{
	FvRobotMainApp * pApp = FvRobotMainApp::pInstance();

	if (pApp != NULL)
	{
		pApp->ShutDown();
	}
}


FvRobotMainApp::FvRobotMainApp()
:m_kNub()
,m_bStop(false)
,m_iTickCnt(0)
,m_iTimerID(0)
,m_itrClientTickIndex(m_kRoBots.end())
,m_kDeadMgr(m_kIDAllocate)
,m_pkMsgFile(NULL)
{
	srand((unsigned int)Timestamp());
}


FvRobotMainApp::~FvRobotMainApp()
{
	m_kNub.CancelTimer(m_iTimerID);

	m_spRobotInSingleMode = NULL;
	m_kRoBots.clear();
}

class AoICfg
{
public:
	enum
	{
		GRIDSIZE = (0x01<<0),
		REFPTLVL = (0x01<<1),
		MINMOVESTEP = (0x01<<2),
		OBJINITSIZE = (0x01<<3),
		OBJINCRSIZE = (0x01<<4),
		POSINITSIZE = (0x01<<5),
		POSINCRSIZE = (0x01<<6),
		EVTINITSIZE = (0x01<<7),
		EVTINCRSIZE = (0x01<<8),
		AOIEXTINITSIZE = (0x01<<9),
		AOIEXTINCRSIZE = (0x01<<10),
		TRAPEXTINITSIZE = (0x01<<11),
		TRAPEXTINCRSIZE = (0x01<<12),
	};
	float		m_fGridSize;
	FvUInt32	m_uiRefPtLvl;
	float		m_fMinMoveStep;
	FvUInt32	m_uiObjInitSize;
	FvUInt32	m_uiObjIncrSize;
	FvUInt32	m_uiPosInitSize;
	FvUInt32	m_uiPosIncrSize;
	FvUInt32	m_uiEvtInitSize;
	FvUInt32	m_uiEvtIncrSize;
	FvUInt32	m_uiAoIExtInitSize;
	FvUInt32	m_uiAoIExtIncrSize;
	FvUInt32	m_uiTrapExtInitSize;
	FvUInt32	m_uiTrapExtIncrSize;
};

class MemReader
{
public:
	MemReader(char* pkBuf, int iBufSize)
		:m_pkBuf(pkBuf),m_pkReadPt(pkBuf),m_pkEnd(pkBuf+iBufSize),m_iBufSize(iBufSize)
	{}

	void	SetReadPt(char* pkReadPt) { m_pkReadPt=pkReadPt; }
	char*	GetReadPt() { return m_pkReadPt; }

	template<typename T>
	void	Read(T& kVal)
	{
		char* pkHead = m_pkReadPt;
		int size = (int)sizeof(T);
		FV_ASSERT(pkHead + size <= m_pkEnd);
		memcpy(&kVal, pkHead, size);
		m_pkReadPt += size;
	}

	void	AddLen(int iLen)
	{
		char* pkHead = m_pkReadPt;
		FV_ASSERT(pkHead + iLen <= m_pkEnd);
		m_pkReadPt += iLen;
	}

	void	SnapTo4()
	{
		int iSize = m_pkReadPt - m_pkBuf;
		iSize = ((iSize +3) >> 2) << 2;
		m_pkReadPt = m_pkBuf + iSize;
		FV_ASSERT(m_pkReadPt <= m_pkEnd);
	}

	void ReadAoI(AoICfg& kAoI)
	{
		FvUInt32 uiFlg;
		Read(uiFlg);

#define AOI_READ(_FLG, _VAL)\
	if(uiFlg & AoICfg::_FLG)\
		{					\
			Read(kAoI._VAL);\
		}

		AOI_READ(GRIDSIZE, m_fGridSize);
		AOI_READ(REFPTLVL, m_uiRefPtLvl);
		AOI_READ(MINMOVESTEP, m_fMinMoveStep);
		AOI_READ(OBJINITSIZE, m_uiObjInitSize);
		AOI_READ(OBJINCRSIZE, m_uiObjIncrSize);
		AOI_READ(POSINITSIZE, m_uiPosInitSize);
		AOI_READ(POSINCRSIZE, m_uiPosIncrSize);
		AOI_READ(EVTINITSIZE, m_uiEvtInitSize);
		AOI_READ(EVTINCRSIZE, m_uiEvtIncrSize);
		AOI_READ(AOIEXTINITSIZE, m_uiAoIExtInitSize);
		AOI_READ(AOIEXTINCRSIZE, m_uiAoIExtIncrSize);
		AOI_READ(TRAPEXTINITSIZE, m_uiTrapExtInitSize);
		AOI_READ(TRAPEXTINCRSIZE, m_uiTrapExtIncrSize);

#undef AOI_READ
	}

protected:
	char*	m_pkBuf;
	char*	m_pkReadPt;
	char*	m_pkEnd;
	int		m_iBufSize;
};

bool FvRobotMainApp::Init(int iArgc, char* pcArgv[])
{
	FV_INFO_MSG("-------------- Robot Start ------------\n");

	//////////////////////////////////////////////////////////////////////////////////
	{
		void* pkEntityExports(NULL);
		if(!FvLogicDllManager::Instance().DllExport(pkEntityExports))
			return false;
		m_pkEntityExports = (FvAllRobotEntityExports*)pkEntityExports;
		if(!FvLogicDllManager::Instance().InitializeLogic())
			return false;
	}

	//////////////////////////////////////////////////////////////////////////////////
	{
		FvString kFileName = "ServerConfig/Spaces.cfg";
		Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().openResource(kFileName);
		if(spDataStream.isNull())
		{
			FV_ERROR_MSG("%s, Open File:%s Failed\n", __FUNCTION__, kFileName.c_str());
			return false;
		}

		int iBufSize = (int)spDataStream->size();
		if(iBufSize <= 0)
		{
			FV_ERROR_MSG("%s, File:%s size is Zero\n", __FUNCTION__, kFileName.c_str());
			return false;
		}
		char* pkSpaceCfgBuf = new char[iBufSize];
		spDataStream->read(pkSpaceCfgBuf, iBufSize);

		//! TODO: 读文件代码太恶心了...写代码很低效

		MemReader kMR(pkSpaceCfgBuf, iBufSize);

		FvUInt32 uiSpaceCnt;
		kMR.Read(uiSpaceCnt);
		m_kSpaceList.reserve(uiSpaceCnt);

		for(FvUInt16 uiSpaceType(0); uiSpaceType<uiSpaceCnt; ++uiSpaceType)
		{
			FvUInt32 uiSpaceSize;
			kMR.Read(uiSpaceSize);
			char* pkSpacePt = kMR.GetReadPt();
			FvUInt32 uiTmp;

			kMR.Read(uiTmp);//! bGlobalSpace

			SpaceInfo kSpaceInfo;
			kSpaceInfo.uiSpaceType = uiSpaceType;

			FvVector4 kRect;
			kMR.Read(kRect.x);
			kMR.Read(kRect.y);
			kMR.Read(kRect.z);
			kMR.Read(kRect.w);
			kSpaceInfo.kSpaceRect = kRect;

			kMR.Read(uiTmp);
			kMR.AddLen(uiTmp);

			kMR.Read(uiTmp);
			kMR.AddLen(uiTmp);

			AoICfg kAoICfg;
			kMR.ReadAoI(kAoICfg);

			FvUInt32 uiCellCnt;
			kMR.Read(uiCellCnt);
			FV_ASSERT(uiCellCnt);
			kSpaceInfo.kCells.reserve(uiCellCnt);
			for(FvUInt32 i=0; i<uiCellCnt; ++i)
			{
				FvUInt32 uiCellSize;
				kMR.Read(uiCellSize);
				char* pkCellPt = kMR.GetReadPt();

				FvVector4 kRect;
				kMR.Read(kRect.x);
				kMR.Read(kRect.y);
				kMR.Read(kRect.z);
				kMR.Read(kRect.w);
				kSpaceInfo.kCells.push_back(kRect);

				kMR.SetReadPt(pkCellPt + uiCellSize);
			}

			kMR.SetReadPt(pkSpacePt + uiSpaceSize);

			m_kSpaceList.push_back(kSpaceInfo);
		}

		delete [] pkSpaceCfgBuf;
	}

	//////////////////////////////////////////////////////////////////////////////////
	{
		int iTimeOut = 1000000/FvRobotCfg::Instance().GetTickFreq()/FvRobotCfg::Instance().GetTickFragments();
		m_iTimerID = m_kNub.RegisterTimer(iTimeOut, this);
		signal(SIGINT, ::IntSignalHandler);
		BotsInterface::RegisterWithNub(m_kNub);
	}

	//////////////////////////////////////////////////////////////////////////////////
	m_kIDAllocate.Init(FvRobotCfg::Instance().GetStartID(), FvRobotCfg::Instance().GetMaxCount());
	if(FvRobotCfg::Instance().GetCount() > 0)
		m_kCreateMgr.Push(FvRobotCfg::Instance().GetCount(), FvRobotCfg::Instance().GetTag());

	PrintMemoryStatus("After Main Init");

	return true;
}

void FvRobotMainApp::Run()
{
	for (;;)
	{
		try
		{
			m_kNub.ProcessContinuously();
			if (m_bStop) break;

			FV_TRACE_MSG( "MainApp::run: "
				"processContinuously returned normally (looping back in)\n" );
		}
		catch (FvNetNubException & ne)
		{
			FV_WARNING_MSG( "MainApp::run: "
				"processContinuously returned unexpectedly (%s).\n",
				NetReasonToString( (FvNetReason)ne.Reason() ) );
		}
	}

	FV_TRACE_MSG( "MainApp::run: stopped due to user request\n" );
}

void FvRobotMainApp::ShutDown()
{
	m_bStop = true;
	m_kNub.BreakProcessing();
}

FvRobotCfg::Result& FvRobotMainApp::DoCmd(const char* pcCmd)
{
	FvRobotCfg::Result& kResult = FvRobotCfg::Instance().ParseCmd(pcCmd);
	if(kResult.empty())
		return kResult;

	bool bResetTimer(false);

	for(int i=0; i<(int)kResult.size(); ++i)
	{
		switch(kResult[i])
		{
		case FvRobotCfg::SingleMode:
			{
				ChangeSingleMode();
			}
			break;
		case FvRobotCfg::ShouldWriteToConsole:
			{
				g_iShouldWriteToConsole = FvRobotCfg::Instance().GetShouldWriteToConsole();
			}
			break;
		case FvRobotCfg::ShouldWriteToLog:
			{
				FV_ASSERT(m_pkMsgFile);
				m_pkMsgFile->OpenLog(FvRobotCfg::Instance().GetShouldWriteToLog());
			}
			break;
		case FvRobotCfg::TickFreq:
		case FvRobotCfg::TickFragments:
			bResetTimer = true;
			break;
		case FvRobotCfg::PacketLossRatio:
			{
				RoBots::const_iterator iter = m_kRoBots.begin();
				while(iter != m_kRoBots.end())
				{
					(*iter)->SetConnectionLossRatio(FvRobotCfg::Instance().GetPacketLossRatio());
					++iter;
				}
			}
			break;
		case FvRobotCfg::NetworkLatency:
			{
				RoBots::const_iterator iter = m_kRoBots.begin();
				while(iter != m_kRoBots.end())
				{
					(*iter)->SetConnectionLatency(FvRobotCfg::Instance().GetMinNetworkLatency(), FvRobotCfg::Instance().GetMaxNetworkLatency());
					++iter;
				}
			}
			break;
		case FvRobotCfg::UpdateMove:
			{
				const FvString& kTag = FvRobotCfg::Instance().GetTag4UpdateMove();

				RoBots::const_iterator iter = m_kRoBots.begin();
				while(iter != m_kRoBots.end())
				{
					if(kTag.empty() || kTag==(*iter)->Tag())
						(*iter)->SetMovementController(FvRobotCfg::Instance().GetCtrllerType(), FvRobotCfg::Instance().GetCtrllerData());
					++iter;
				}
			}
			break;
		case FvRobotCfg::AddBots:
			{
				const FvString& kTag = FvRobotCfg::Instance().GetTag4AddRobots();
				int iCnt = FvRobotCfg::Instance().GetAddRobots();
				int iRemain = m_kIDAllocate.Size() - m_kCreateMgr.Size();
				FV_ASSERT(iRemain >= 0);
				if(iCnt > iRemain)
					iCnt = iRemain;

				if(iCnt > 0)
					m_kCreateMgr.Push(iCnt, kTag=="" ? FvRobotCfg::Instance().GetTag() : kTag);
			}
			break;
		case FvRobotCfg::DelBots:
			{
				const FvString& kTag = FvRobotCfg::Instance().GetTag4DelRobots();
				int iCnt = FvRobotCfg::Instance().GetDelRobots();
				bool bResetSingleRobot(false);

				//! 不判断Tag
				if(kTag == "")
				{
					//! 先从已创建的Robot里删除
					for(; iCnt>0 && !m_kRoBots.empty(); --iCnt)
					{
						RoBots::iterator iter = m_kRoBots.begin();
						if(iter == m_itrClientTickIndex)
						{
							++m_itrClientTickIndex;
						}
						m_kIDAllocate.PutID((*iter)->RobotID());
						if(*iter == m_spRobotInSingleMode.GetObject())
						{
							bResetSingleRobot = true;
							m_spRobotInSingleMode = NULL;
						}
						m_kRoBots.pop_front();
					}

					//! 还没删够,则从未创建的Robot里删除
					if(iCnt > 0)
					{
						iCnt -= m_kCreateMgr.Del(iCnt);
					}

					//! 还没删够,则从死亡的Robot里删除
					if(iCnt > 0)
					{
						iCnt -= m_kDeadMgr.Del(iCnt);
					}
				}
				else//! 删除该Tag的所有对象
				{
					//! 从已创建的Robot里删除
					{
						RoBots::iterator iter = m_kRoBots.begin();
						while(iter != m_kRoBots.end())
						{
							RoBots::iterator oldIter = iter++;
							if((*oldIter)->Tag() == kTag)
							{
								if(oldIter == m_itrClientTickIndex)
								{
									++m_itrClientTickIndex;
								}
								m_kIDAllocate.PutID((*oldIter)->RobotID());
								if(*oldIter == m_spRobotInSingleMode.GetObject())
								{
									bResetSingleRobot = true;
									m_spRobotInSingleMode = NULL;
								}
								m_kRoBots.erase(oldIter);
							}
						}
					}

					//! 从未创建的Robot里删除
					m_kCreateMgr.Del(kTag);

					//! 从死亡的Robot里删除
					m_kDeadMgr.Del(kTag);
				}

				if(bResetSingleRobot && !m_kRoBots.empty())
					m_spRobotInSingleMode = m_kRoBots.front();
			}
			break;
		case FvRobotCfg::SingleID:
			{
				m_spRobotInSingleMode = FindApp(FvRobotCfg::Instance().GetSingleID());
				if(!m_spRobotInSingleMode && !m_kRoBots.empty())
					m_spRobotInSingleMode = m_kRoBots.front();
			}
			break;
		case FvRobotCfg::PrintMemory:
			{
				PrintMemoryStatus("RunTime");
			}
			break;
		}
	}

	if(bResetTimer)
	{
		FV_ASSERT(m_iTimerID);
		m_kNub.TimerIntervalTime(m_iTimerID) = StampsPerSecond()/FvRobotCfg::Instance().GetTickFreq()/FvRobotCfg::Instance().GetTickFragments();
	}

	return kResult;
}

void FvRobotMainApp::ChangeSingleMode()
{
	if(FvRobotCfg::Instance().GetSingleMode())
	{
		m_spRobotInSingleMode = FindApp(FvRobotCfg::Instance().GetSingleID());
		if(!m_spRobotInSingleMode && !m_kRoBots.empty())
			m_spRobotInSingleMode = m_kRoBots.front();
	}
	else
	{
		m_spRobotInSingleMode = NULL;
	}
}

bool FvRobotMainApp::AddBot(const FvString& kTag, int iRobotID)
{
	if(kTag == "")
		return false;

	char numStr[10];
	FvSNPrintf(numStr, sizeof(numStr), "%d", iRobotID);
	FvString kName = FvRobotCfg::Instance().GetUsernamePrefix() + numStr;

	FvVector3 kBirthPos;
	FvVector4& kRobotBirthRect = FvRobotCfg::Instance().GetBirthRect();
	kBirthPos.x = kRobotBirthRect.x + (kRobotBirthRect.y-kRobotBirthRect.x)*rand()/RAND_MAX;
	kBirthPos.y = kRobotBirthRect.z + (kRobotBirthRect.w-kRobotBirthRect.z)*rand()/RAND_MAX;
	kBirthPos.z = 0.0f;

	FV_INFO_MSG("%s, Pos[%f,%f,%f]\n", __FUNCTION__, kBirthPos.x, kBirthPos.y, kBirthPos.z);

	m_kRoBots.push_back(new FvRobotClientApp(m_kNub, iRobotID, kTag, kName, FvRobotCfg::Instance().GetPassword(), kBirthPos));
	return true;
}

FvRobotMainApp::SpaceEntity* FvRobotMainApp::FindSpaceEntity(FvSpaceID iSpaceID)
{
	SpaceEntityList::iterator itrB = m_kSpaceEntityList.begin();
	SpaceEntityList::iterator itrE = m_kSpaceEntityList.end();
	while(itrB != itrE)
	{
		if(itrB->m_iSpaceID == iSpaceID)
			return &(*itrB);
		++itrB;
	}

	return NULL;
}

int FvRobotMainApp::HandleTimeout( FvNetTimerID, void * )
{
	static bool s_bInTick = false;

	if(s_bInTick)
	{
		FV_WARNING_MSG( "MainApp::HandleTimeout: Called recursively\n" );
		return 0;
	}
	s_bInTick = true;

	++m_iTickCnt;


	////////////////////////////////////////////////////////////////////////////////////////////////
	if(m_iTickCnt % FvRobotCfg::Instance().GetCreateTick() == 0)
	{
		int i=0;
		int iID(0);
		FvString kTag;
		int iRobotCreatePerTick = FvRobotCfg::Instance().GetCreateTick();

		if(i < iRobotCreatePerTick &&
			!m_kCreateMgr.Empty())
		{
			for(; i<iRobotCreatePerTick && !m_kCreateMgr.Empty(); ++i)
			{
				iID = m_kIDAllocate.GetID();
				m_kCreateMgr.Pop(kTag);
				AddBot(kTag, iID);
			}
		}

		if(FvRobotCfg::Instance().GetAutoReConnect() &&
			i < iRobotCreatePerTick &&
			!m_kDeadMgr.Empty())
		{
			for(; i<iRobotCreatePerTick && !m_kDeadMgr.Empty(); ++i)
			{
				m_kDeadMgr.Pop(iID, kTag);
				AddBot(kTag, iID);
			}
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////////////
	static int s_iRemainder = 0;
	int iNumberToUpdate = (m_kRoBots.size() + s_iRemainder) / FvRobotCfg::Instance().GetTickFragments();
	s_iRemainder = (m_kRoBots.size() + s_iRemainder) % FvRobotCfg::Instance().GetTickFragments();
	float fDeltaTime = 1.0f / FvRobotCfg::Instance().GetTickFreq();

	bool bResetSingleRobot(false);
	while(iNumberToUpdate-- > 0 && !m_kRoBots.empty())
	{
		if(m_itrClientTickIndex == m_kRoBots.end())
		{
			m_itrClientTickIndex = m_kRoBots.begin();
		}
		RoBots::iterator iter = m_itrClientTickIndex++;

		if(!(*iter)->Tick(fDeltaTime))
		{
			m_kDeadMgr.Push((*iter)->RobotID(), (*iter)->Tag());
			if(*iter == m_spRobotInSingleMode.GetObject())
			{
				bResetSingleRobot = true;
				m_spRobotInSingleMode = NULL;
			}
			m_kRoBots.erase(iter);
		}
	}

	if(bResetSingleRobot && !m_kRoBots.empty())
		m_spRobotInSingleMode = m_kRoBots.front();
	if(FvRobotCfg::Instance().GetSingleMode() && !m_spRobotInSingleMode && !m_kRoBots.empty())
		m_spRobotInSingleMode = m_kRoBots.front();

	s_bInTick = false;
	return 0;
}


namespace
{
typedef std::map<FvString, FvRobotMovementFactory*> MovementFactories;
MovementFactories* s_pMovementFactories;
}

FvRobotMovementController* FvRobotMainApp::CreateMovementController(float& fSpeed, FvVector3& kPosition,
													const FvString& kCtrllerType, const FvString& kCtrllerData)
{
	if(s_pMovementFactories != NULL)
	{
		MovementFactories::iterator iter = s_pMovementFactories->find(kCtrllerType);
		if(iter != s_pMovementFactories->end())
		{
			return iter->second->Create(kCtrllerData, fSpeed, kPosition);
		}
	}

	return NULL;
}


void FvRobotMainApp::AddFactory( const FvString & name, FvRobotMovementFactory & factory )
{
	if(s_pMovementFactories == NULL)
	{
		s_pMovementFactories = new MovementFactories;
	}

	(*s_pMovementFactories)[ name ] = &factory;
}

FvRobotClientApp* FvRobotMainApp::FindApp(int iRobotID) const
{
	RoBots::const_iterator iter = m_kRoBots.begin();

	while(iter != m_kRoBots.end())
	{
		FvRobotClientApp* pApp = iter->Get();

		if(pApp && pApp->RobotID() == iRobotID)
		{
			return pApp;
		}

		++iter;
	}

	return NULL;
}

void FvRobotMainApp::AddEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);

	Entities* pkEntities(NULL);
	{
		SpaceEntity* pkSpaceEntity = FindSpaceEntity(pkEntity->GetSpaceID());
		if(pkSpaceEntity)
		{
			pkEntities = &(pkSpaceEntity->m_kEntities);
		}
		else
		{
			SpaceEntity kSpaceEntity;
			kSpaceEntity.m_iSpaceID = pkEntity->GetSpaceID();
			m_kSpaceEntityList.push_back(kSpaceEntity);
			pkEntities = &(m_kSpaceEntityList.back().m_kEntities);
		}
	}

	{
		Entities::iterator itr = pkEntities->find(pkEntity->GetEntityID());
		if(itr != pkEntities->end())
		{
			FvEntity* pkHeader = itr->second;
			FV_ASSERT(pkHeader);
			if(pkHeader->IsCtrler())
			{
				FV_ASSERT(!pkEntity->IsCtrler());
				pkEntity->m_pkEntityPre = pkHeader;
				pkEntity->m_pkEntityNex = pkHeader->m_pkEntityNex;
				pkHeader->m_pkEntityNex = pkEntity;
				if(pkEntity->m_pkEntityNex)
					pkEntity->m_pkEntityNex->m_pkEntityPre = pkEntity;
			}
			else
			{
				pkEntity->m_pkEntityNex = pkHeader;
				pkHeader->m_pkEntityPre = pkEntity;
				(*pkEntities)[pkEntity->GetEntityID()] = pkEntity;
			}
		}
		else
		{
			(*pkEntities)[pkEntity->GetEntityID()] = pkEntity;
		}
	}
}

void FvRobotMainApp::RemoveEntity(FvEntity* pkEntity)
{
	FV_ASSERT(pkEntity);

	Entities* pkEntities(NULL);
	SpaceEntityList::iterator itr = m_kSpaceEntityList.begin();
	{
		while(itr != m_kSpaceEntityList.end())
		{
			if(itr->m_iSpaceID == pkEntity->GetSpaceID())
			{
				pkEntities = &(itr->m_kEntities);
				break;
			}
			++itr;
		}

		FV_ASSERT(pkEntities);
	}

	{
		Entities::iterator itr = pkEntities->find(pkEntity->GetEntityID());
		if(itr != pkEntities->end())
		{
			if(pkEntity->m_pkEntityPre || pkEntity->m_pkEntityNex)
			{
				if(pkEntity->m_pkEntityPre)
					pkEntity->m_pkEntityPre->m_pkEntityNex = pkEntity->m_pkEntityNex;
				if(pkEntity->m_pkEntityNex)
					pkEntity->m_pkEntityNex->m_pkEntityPre = pkEntity->m_pkEntityPre;

				if(!pkEntity->m_pkEntityPre)
				{
					FV_ASSERT(pkEntity->m_pkEntityNex);
					(*pkEntities)[pkEntity->GetEntityID()] = pkEntity->m_pkEntityNex;
				}
			}
			else
			{
				pkEntities->erase(pkEntity->GetEntityID());
			}
		}
		else
		{
			FV_ASSERT(0);
		}
	}

	if(pkEntities->empty())
		m_kSpaceEntityList.erase(itr);
}

void FvRobotMainApp::SetSpaceType(FvSpaceID iSpaceID, FvUInt16 uiSpaceType)
{
	SpaceEntity* pkSpaceEntity = FindSpaceEntity(iSpaceID);
	if(pkSpaceEntity)
		pkSpaceEntity->m_uiSpaceType = uiSpaceType;
}

const FvRobotMainApp::SpaceInfo* FvRobotMainApp::FindSpaceInfo(FvUInt16 uiSpaceType) const
{
	for(int i=0; i<(int)m_kSpaceList.size(); ++i)
	{
		if(m_kSpaceList[i].uiSpaceType == uiSpaceType)
			return &m_kSpaceList[i];
	}

	return NULL;
}


#define DEFINE_SERVER_HERE
#include "FvRobotInterface.h"
