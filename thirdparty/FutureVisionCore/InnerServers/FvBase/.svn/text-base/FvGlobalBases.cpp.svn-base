#include "FvGlobalBases.h"
#include "FvBaseMailbox.h"
#include "FvBaseEntity.h"
#include "FvBaseEntityManager.h"
#include <../FvBaseAppManager/FvBaseAppManagerInterface.h>


FvGlobalBases::FvGlobalBases()
{
}

FvGlobalBases::~FvGlobalBases()
{
}

FvMailBox* FvGlobalBases::Find(const FvString& kLabel) const
{
	if(kLabel == "")
		return NULL;

	Maps::const_iterator itr = m_kMap.find(kLabel);
	if(itr != m_kMap.end())
		return const_cast<FvMailBox*>(&(itr->second));
	else
		return NULL;
}

void FvGlobalBases::RegisterRequest(const char* pcLabel, FvEntity* pkBase, RegisterGlobalBaseCallBack* pkCallback)
{
	if(!pcLabel || !pcLabel[0] || !pkBase || !pkCallback)
	{
		if(pkCallback)
			pkCallback->HanldeCallBack(false);
		return;
	}

	//! 已存在
	if(Find(pcLabel))
	{
		pkCallback->HanldeCallBack(false);
		return;
	}

	//! 本地先添加
	bool bRet = m_kMap.insert(std::make_pair(pcLabel, pkBase->BaseMBInEntity())).second;
	FV_ASSERT(bRet);

	class RegisterBaseReplyMessageHandler : public FvNetReplyMessageHandler
	{
	public:
		RegisterBaseReplyMessageHandler(RegisterGlobalBaseCallBack* pkCallBack)
		:m_pkCallBack(pkCallBack) {}

		virtual void HandleMessage( const FvNetAddress & source, FvNetUnpackedMessageHeader & header, FvBinaryIStream & data, void * arg )
		{
			FvInt8 bSuccess;
			data >> bSuccess;

			m_pkCallBack->HanldeCallBack(bSuccess);
			delete this;
		}

		virtual void HandleException( const FvNetNubException & exception, void * arg )
		{
			m_pkCallBack->HanldeCallBack(false);
			delete this;
		}

		RegisterGlobalBaseCallBack*	m_pkCallBack;
	};

	FvNetBundle& kBundle = FvEntityManager::Instance().BaseAppMgr().Bundle();
	kBundle.StartRequest(BaseAppMgrInterface::RegisterBaseGlobally, new RegisterBaseReplyMessageHandler(pkCallback));
	kBundle << pcLabel << pkBase->BaseMBInEntity();
	FvEntityManager::Instance().BaseAppMgr().Channel().DelayedSend();
}

bool FvGlobalBases::Deregister(const char* pcLabel, FvEntity* pkBase)
{
	if(!pcLabel || !pcLabel[0] || !pkBase)
		return false;

	//! 存在则先删除
	Maps::iterator itr = m_kMap.find(pcLabel);
	if(itr != m_kMap.end())
	{
		if(itr->second == pkBase->BaseMBInEntity())
			m_kMap.erase(itr);
		else
			return false;
	}
	else
	{
		return false;
	}

	FvNetBundle& kBundle = FvEntityManager::Instance().BaseAppMgr().Bundle();
	kBundle.StartMessage(BaseAppMgrInterface::DeregisterBaseGlobally);
	kBundle << pcLabel;
	FvEntityManager::Instance().BaseAppMgr().Channel().DelayedSend();

	return true;
}

void FvGlobalBases::Add(const FvString& kLabel, FvMailBox& kBaseMB)
{
	m_kMap[kLabel] = kBaseMB;
}

void FvGlobalBases::Remove(const FvString& kLabel)
{
	m_kMap.erase(kLabel);
}

void FvGlobalBases::OnBaseDestroyed(FvEntity* pkBase)
{
	FV_ASSERT(pkBase);

	std::vector<FvString> kLabels;

	Maps::iterator itrB = m_kMap.begin();
	Maps::iterator itrE = m_kMap.end();
	while(itrB != itrE)
	{
		if(itrB->second == pkBase->BaseMBInEntity())
		{
			kLabels.push_back(itrB->first);
			itrB = m_kMap.erase(itrB);
		}
		else
		{
			++itrB;
		}
	}

	if(!kLabels.empty())
	{
		FvNetBundle& kBundle = FvEntityManager::Instance().BaseAppMgr().Bundle();

		for(int i=0; i<(int)kLabels.size(); ++i)
		{
			kBundle.StartMessage(BaseAppMgrInterface::DeregisterBaseGlobally);
			kBundle << kLabels[i];
		}

		FvEntityManager::Instance().BaseAppMgr().Channel().DelayedSend();
	}
}


