#include "FvRobotUtility.h"
#include <FvDebug.h>


IDAllocate::IDAllocate()
:m_iStartID(0)
,m_iEndID(0)
,m_iIndex(0)
{

}

IDAllocate::~IDAllocate()
{

}

bool IDAllocate::Init(int iStartID, int iCount)
{
	if(iCount <= 0 ||
		m_iStartID < m_iEndID)
		return false;

	m_iStartID = iStartID;
	m_iIndex = m_iStartID;
	m_iEndID = m_iStartID + iCount;
	return true;
}

int IDAllocate::Size() const
{
	return (m_iEndID-m_iIndex) + (int)m_kList.size();
}

int IDAllocate::GetID()
{
	int iID(-1);
	if(m_iIndex < m_iEndID)
	{
		iID = m_iIndex;
		++m_iIndex;
	}
	else if(!m_kList.empty())
	{
		iID = m_kList.front();
		m_kList.pop_front();
	}

	return iID;
}

bool IDAllocate::PutID(int iID)
{
	if(iID < m_iStartID || m_iEndID < iID)
		return false;

	m_kList.push_back(iID);
	return true;
}

CreateMgr::CreateMgr()
:m_iCreateCnt(0)
{

}

CreateMgr::~CreateMgr()
{

}

bool CreateMgr::Empty() const
{
	return m_iCreateCnt == 0;
}

int CreateMgr::Size() const
{
	return m_iCreateCnt;
}

bool CreateMgr::Pop(FvString& kTag)
{
	if(m_iCreateCnt == 0)
		return false;

	FV_ASSERT(!m_kCreateInfos.empty());
	FV_ASSERT(m_kCreateInfos.front().first > 0);

	--m_iCreateCnt;
	--m_kCreateInfos.front().first;
	kTag = m_kCreateInfos.front().second;

	if(m_kCreateInfos.front().first == 0)
		m_kCreateInfos.pop_front();

	return true;
}

bool CreateMgr::Push(int iCount, const FvString& kTag)
{
	if(iCount <= 0 ||
		kTag == "")
		return false;

	m_kCreateInfos.push_back(std::make_pair(iCount, kTag));
	m_iCreateCnt += iCount;
	return true;
}

int CreateMgr::Del(int iCount)
{
	if(iCount <= 0)
		return 0;

	int iOldCount = iCount;

	for(; iCount>0 && m_iCreateCnt>0; )
	{
		FV_ASSERT(!m_kCreateInfos.empty());
		int& iNum = m_kCreateInfos.front().first;
		if(iNum > iCount)
		{
			iNum -= iCount;
			m_iCreateCnt -= iCount;
			iCount = 0;
		}
		else
		{
			m_iCreateCnt -= iNum;
			iCount -= iNum;
			m_kCreateInfos.pop_front();
		}
	}

	return iOldCount-iCount;
}

bool CreateMgr::Del(const FvString& kTag)
{
	if(kTag == "")
		return false;

	CreateInfos::iterator iter = m_kCreateInfos.begin();
	while(iter != m_kCreateInfos.end())
	{
		CreateInfos::iterator oldIter = iter++;
		if((*oldIter).second == kTag)
		{
			m_iCreateCnt -= (*oldIter).first;
			m_kCreateInfos.erase(oldIter);
		}
	}

	return true;
}


DeadMgr::DeadMgr(IDAllocate& kIDAllocate)
:m_kIDAllocate(kIDAllocate)
{

}

DeadMgr::~DeadMgr()
{

}

bool DeadMgr::Empty() const
{
	return m_kDeleteInfos.empty();
}

int DeadMgr::Size() const
{
	return (int)m_kDeleteInfos.size();
}

bool DeadMgr::Pop(int& iID, FvString& kTag)
{
	if(m_kDeleteInfos.empty())
		return false;

	iID = m_kDeleteInfos.front().first;
	kTag = m_kDeleteInfos.front().second;
	m_kDeleteInfos.pop_front();
	return true;
}

bool DeadMgr::Push(int iID, const FvString& kTag)
{
	if(kTag == "")
		return false;

	m_kDeleteInfos.push_back(std::make_pair(iID, kTag));
	return true;
}

int DeadMgr::Del(int iCount)
{
	if(iCount <= 0)
		return 0;

	int iOldCount = iCount;

	for(; iCount>0 && !m_kDeleteInfos.empty(); --iCount)
	{
		m_kIDAllocate.PutID(m_kDeleteInfos.front().first);
		m_kDeleteInfos.pop_front();
	}

	return iOldCount-iCount;
}

bool DeadMgr::Del(const FvString& kTag)
{
	if(kTag == "")
		return false;

	DeleteInfos::iterator iter = m_kDeleteInfos.begin();
	while(iter != m_kDeleteInfos.end())
	{
		DeleteInfos::iterator oldIter = iter++;
		if((*oldIter).second == kTag)
		{
			m_kIDAllocate.PutID((*oldIter).first);
			m_kDeleteInfos.erase(oldIter);
		}
	}

	return true;
}


void PrintMemoryStatus(const char* pcTitle)
{
	int DIV = 1024;
	char *divisor = "K";
	int WIDTH = 7;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	FV_INFO_MSG("PrintMemoryStatus ===> %s\n", pcTitle ? pcTitle : "");

	FV_INFO_MSG("\t%ld percent of memory is in use.\n",
		statex.dwMemoryLoad);
	FV_INFO_MSG("\tThere are %*I64d total %sbytes of physical memory.\n",
		WIDTH, statex.ullTotalPhys/DIV, divisor);
	FV_INFO_MSG("\tThere are %*I64d free %sbytes of physical memory.\n",
		WIDTH, statex.ullAvailPhys/DIV, divisor);
	FV_INFO_MSG("\tThere are %*I64d total %sbytes of paging file.\n",
		WIDTH, statex.ullTotalPageFile/DIV, divisor);
	FV_INFO_MSG("\tThere are %*I64d free %sbytes of paging file.\n",
		WIDTH, statex.ullAvailPageFile/DIV, divisor);
	FV_INFO_MSG("\tThere are %*I64x total %sbytes of virtual memory.\n",
		WIDTH, statex.ullTotalVirtual/DIV, divisor);
	FV_INFO_MSG("\tThere are %*I64x free %sbytes of virtual memory.\n",
		WIDTH, statex.ullAvailVirtual/DIV, divisor);
	FV_INFO_MSG("\tThere are %*I64x free %sbytes of extended memory.\n",
		WIDTH, statex.ullAvailExtendedVirtual/DIV, divisor);
}
