#include "FvDestroyManager.h"
FvDestroyManager FvDestroyManager::S_INSTANCE;

void FvDestroyManager::_End()
{
	for (List::iterator iter = m_kDeleteList.begin(); iter != m_kDeleteList.end(); ++iter)
	{
		FvDeletable* pkDestroy = (*iter);
		FV_ASSERT(pkDestroy);
		delete pkDestroy;
	}
	m_kDeleteList.clear();
	m_kEndIter = m_kDeleteList.begin();
	//
	for (List::iterator iter = m_kOwnList.begin(); iter != m_kOwnList.end(); ++iter)
	{
		FvDeletable* pkDestroy = (*iter);
		FV_ASSERT(pkDestroy);
		pkDestroy->Detach();
		delete pkDestroy;
	}
	m_kOwnList.clear();
}


//+------------------------------------------------------------------------------------
void FvDestroyManager::_UpdateDeleteList()
{
	for (List::iterator iter = m_kDeleteList.begin(); iter != m_kEndIter; ++iter)
	{
		FvDeletable* pkDestroy = (*iter);
		FV_ASSERT(pkDestroy);
		pkDestroy->Detach();
	}
	m_kEndIter = m_kDeleteList.begin();
}

bool FvDestroyManager::_CheckDeletable(void* qT)
{
	for (List::iterator iter = m_kDeleteList.begin(); iter != m_kEndIter; ++iter)
	{
		FvDeletable* pkDestroy = (*iter);
		FV_ASSERT(pkDestroy);
		if(pkDestroy->IsSame(qT))
		{
			return false;
		}
	}
	return true;
}
FvDestroyManager::FvDestroyManager()
{
	m_kDeleteList.reserve(100);
	m_kOwnList.reserve(100);
	m_kEndIter = m_kDeleteList.begin();
}
FvDestroyManager::~FvDestroyManager()
{
	_UpdateDeleteList();
	_End();
}