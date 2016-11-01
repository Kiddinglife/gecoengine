#include "FvExpressManager.h"



FvExpressManager::FvExpressManager(void)
{
}

FvExpressManager::~FvExpressManager(void)
{
	_Clear();
}
void FvExpressManager::ClearAllExpress()
{
	_Clear();
}

template<void (FvExpress::*func)(const float)>
void _Update(FvExpressManager::ExpressList& kExpressList, const float fDeltaTime)
{
	kExpressList.BeginIterator();
	while (!kExpressList.IsEnd())
	{
		FvExpressManager::ExpressList::iterator iter = kExpressList.GetIterator();
		FvExpress* pkExpress = (*iter).m_Content;
		FV_ASSERT(pkExpress);
		kExpressList.Next();
		(pkExpress->*func)(fDeltaTime);
	}
}

void FvExpressManager::Update(const float deltaTime)
{
	::_Update<&FvExpress::Update>(m_ExpressList, deltaTime);
	::_Update<&FvExpress::Update1>(m_ExpressList1, deltaTime);
	::_Update<&FvExpress::Update2>(m_ExpressList2, deltaTime);
}

void _Clear(FvExpressManager::ExpressList& kExpressList)
{
	kExpressList.BeginIterator();
	while (!kExpressList.IsEnd())
	{
		FvExpressManager::ExpressList::iterator iter = kExpressList.GetIterator();
		FvExpress* pkExpress = (*iter).m_Content;
		FV_ASSERT(pkExpress);
		kExpressList.Next();
		pkExpress->End();
	}
}
void FvExpressManager::_Clear()
{
	::_Clear(m_ExpressList);
	::_Clear(m_ExpressList1);
	::_Clear(m_ExpressList2);
	::_Clear(m_IdleList);
}
void FvExpressManager::AttachUpdate(FvExpress& kExpress)
{
	m_ExpressList.AttachBack(kExpress.m_kUpdateNode);
}
void FvExpressManager::AttachUpdate1(FvExpress& kExpress)
{
	m_ExpressList1.AttachBack(kExpress.m_kUpdateNode);
}
void FvExpressManager::AttachUpdate2(FvExpress& kExpress)
{
	m_ExpressList2.AttachBack(kExpress.m_kUpdateNode);
}
void FvExpressManager::AttachIdle(FvExpress& kExpress)
{
	m_IdleList.AttachBack(kExpress.m_kUpdateNode);
}






