#include "FvGlobalIDPool.h"

#define MAX_ID 0x7FFFFFFE


FvGlobalIDPool::FvGlobalIDPool(FvInt32 iInitNum)
:m_iInitNum(iInitNum),m_iIDIdx(iInitNum)
{

}

FvGlobalIDPool::~FvGlobalIDPool()
{

}

FvInt32 FvGlobalIDPool::GetID()
{
	FvInt32 iID(0);

	if(!m_kFreeIDs.empty())
	{
		iID = m_kFreeIDs.front();
	}
	else if(m_iIDIdx < MAX_ID)
	{
		++m_iIDIdx;
		iID = m_iIDIdx;
	}
	else
	{
		FV_ERROR_MSG("%s, Can't generate id\n", __FUNCTION__);
	}

	return iID;
}

void FvGlobalIDPool::PutID(FvInt32 iID)
{
	FV_ASSERT(m_iInitNum < iID);
	m_kFreeIDs.push(iID);
}

bool FvGlobalIDPool::GetIDsToStream(int iNum, FvBinaryOStream& kStream)
{
	if(iNum <= 0)
		return true;

	for(int i=0; i<iNum; ++i)
	{
		FvInt32 iID = GetID();
		if(iID)
			kStream << iID;
		else
			return false;
	}

	return true;
}

void FvGlobalIDPool::PutIDsFromStream(FvBinaryIStream& kStream)
{
	while(kStream.RemainingLength())
	{
		FvInt32 iID;
		kStream >> iID;
		PutID(iID);
	}
}



