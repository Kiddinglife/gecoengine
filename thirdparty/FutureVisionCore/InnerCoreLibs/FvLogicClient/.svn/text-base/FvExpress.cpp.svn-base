#include "FvExpress.h"
#include <FvDestroyManager.h>

FvExpress::FvExpress(void)
{
	m_kUpdateNode.m_Content = this;
	m_kOwnNode.m_Content = this;
}

FvExpress::~FvExpress(void)
{
	
}

void FvExpress::End()
{
	m_kUpdateNode.Detach();
	m_kOwnNode.Detach();
	FvDestroyManager::Destroy(*this);
}
void FvExpress::DetachUpdate()
{
	m_kUpdateNode.Detach();
}


//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FvExpressOwnList::~FvExpressOwnList()
{
	End();
}
void FvExpressOwnList::End()
{
	m_kExpressList.BeginIterator();
	while (!m_kExpressList.IsEnd())
	{
		ExpressList::iterator iter = m_kExpressList.GetIterator();
		FvExpress* pkExpress = (*iter).m_Content;
		FV_ASSERT_ERROR(pkExpress);
		m_kExpressList.Next();
		pkExpress->End();
	}
}
void FvExpressOwnList::Add(FvExpress& kExpress)
{
	m_kExpressList.AttachBack(kExpress.m_kOwnNode);
}