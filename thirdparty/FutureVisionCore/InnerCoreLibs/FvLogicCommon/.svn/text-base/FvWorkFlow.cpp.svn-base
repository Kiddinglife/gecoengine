#include "FvWorkFlow.h"

#include "FvLogbus.h"

FvWork::FvWork()
:m_rpFlow(NULL)
,m_State(DEACTIVE)
{
	m_AttachToFlow.m_Content = this;
}
FvWork::~FvWork()
{

}
//void FvWork::Pause()
//{
//	if(m_State == ACTIVE)
//	{
//		FvLogBus::CritOk("FvWork::Pause");
//		m_State = PAUSED;
//		_OnPause();
//		//if(m_rpFlow)
//		//{
//		//	Invoke(0, *this, NULL);
//		//}
//	}
//}
//void FvWork::Resume()
//{
//	if(m_State == PAUSED)
//	{
//		FvLogBus::CritOk("FvWork::Resume");
//		m_State = ACTIVE;
//		_OnResume();
//		//if (m_rpFlow)
//		//{
//		//	m_rpFlow->DoWork(*this);
//		//}
//	}
//}
//void FvWork::Start()
//{
//	if(m_State == DEACTIVE)
//	{
//		FvLogBus::CritOk("FvWork::Start");
//		m_State = ACTIVE;
//		_OnStart();
//		if (m_rpFlow)
//		{
//			m_rpFlow->DoWork(*this);
//		}
//	}
//	else if (m_State == PAUSED)
//	{
//		Resume();
//	}
//}
void FvWork::End()
{
	FvLogBus::CritOk("FvWork::End");
	if(m_State != DEACTIVE)
	{
		m_State = DEACTIVE;
		_OnEnd();
		m_AttachToFlow.Detach();
		if(m_rpFlow)
		{
			//m_rpFlow->DoNext();
			m_kCallbackList.Invoke(0, *this);
		}
		m_rpFlow = NULL;
	}
}
void FvWork::_EndByFlow()
{
	if(m_State != DEACTIVE)
	{
		FvLogBus::CritOk("FvWork::_EndByFlow");
		m_State = DEACTIVE;
		_OnEnd();
		Detach();
		m_rpFlow = NULL;
	}
}
void FvWork::_StartByFlow()
{
	if(m_State == DEACTIVE)
	{
		FvLogBus::CritOk("FvWork::_StartByFlow");
		m_State = ACTIVE;
		_OnStart();
	}
}
void FvWork::_ResumeByFlow()
{
	if(m_State == PAUSED)
	{
		FvLogBus::CritOk("FvWork::_ResumeByFlow");
		m_State = ACTIVE;
		_OnResume();
	}
}
void FvWork::_PauseByFlow()
{
	if(m_State == ACTIVE)
	{
		FvLogBus::CritOk("FvWork::_PauseByFlow");
		m_State = PAUSED;
		_OnPause();
	}
}

void FvWork::_AttachToFlow(FvWorkFlow& kFlow)
{
	m_rpFlow = &kFlow;
	kFlow.m_List.AttachBack(m_AttachToFlow);
}
void FvWork::Detach()
{
	m_rpFlow = NULL;
	m_AttachToFlow.Detach();
}
void FvWork::_OnStart(){}
void FvWork::_OnEnd(){}
void FvWork::_OnPause(){}
void FvWork::_OnResume(){}

//+---------------------------------------------------------------------------------------------------

FvWorkFlow::FvWorkFlow(void)
:m_rpCurrentWork(NULL)
#pragma warning(push)
#pragma warning(disable: 4355)
,m_kWorkEndAsyn_CB(*this)
#pragma warning(pop)
{
	//m_kWorkEndAsyn_CB.m_Content = FvCallbackList<FvWork, void>::CreateAsynCallback<FvWorkFlow>(*this, &FvWorkFlow::_OnWorkEnd);
}

FvWorkFlow::~FvWorkFlow(void)
{
	Clear();
}

void FvWorkFlow::Clear()
{
	EndCurrent();
	
	m_List.BeginIterator();
	while (!m_List.IsEnd())
	{
		_Node* iter = m_List.GetIterator();
		FvWork* pkWork = iter->m_Content;
		m_List.Next();
		pkWork->End();
	}
	m_List.Clear();
	m_kWorkEndAsyn_CB.Detach();
}
bool FvWorkFlow::IsAttach(const FvWork& kWork)const
{
	return kWork.m_AttachToFlow.IsAttach(m_List);
}
void FvWorkFlow::Attach(FvWork& kWork)
{
	kWork._AttachToFlow(*this);
}
void FvWorkFlow::Attach(FvWork* pkWork)
{
	if(pkWork)
	{
		pkWork->_AttachToFlow(*this);
	}
}
void FvWorkFlow::DoWork(FvWork& kWork)
{
	if(m_rpCurrentWork == &kWork)
	{
		return;
	}
	PauseCurrent();
	m_List.AttachFront(kWork.m_AttachToFlow);
	_StartWork(kWork);
	m_rpCurrentWork = &kWork;
}
void FvWorkFlow::Start()
{
	if(m_rpCurrentWork)
	{
		_StartWork(*m_rpCurrentWork);
	}
	else
	{
		DoNext();
	}
}

void FvWorkFlow::_OnWorkEnd(const FvUInt32)
{
	FvLogBus::CritOk("FvWorkFlow::_OnWorkEnd");
	m_kWorkEndAsyn_CB.Detach();
	DoNext();
}
void FvWorkFlow::DoNext()
{
	PauseCurrent();
	_List::iterator head = m_List.GetHead();
	if(m_List.IsEnd(head))
	{
		m_kCallbackList.Invoke(0, *this);
		return;
	}
	m_rpCurrentWork = head->m_Content;
	FV_ASSERT(m_rpCurrentWork);
	_StartWork(*m_rpCurrentWork);
}
FvWork* FvWorkFlow::CurrentWork()const
{
	return m_rpCurrentWork;
}
const FvWorkFlow::_List& FvWorkFlow::Works()const
{
	return m_List;
}
void FvWorkFlow::_StartWork(FvWork& qWork)
{
	qWork.m_kCallbackList.Attach(m_kWorkEndAsyn_CB);
	if(qWork.GetState() == FvWork::DEACTIVE)
	{
		qWork._StartByFlow();
	}
	else if(qWork.GetState() == FvWork::PAUSED)
	{
		qWork._ResumeByFlow();
	}
}
void FvWorkFlow::_EndWork(FvWork& kWork)
{
	kWork._EndByFlow();
}
void FvWorkFlow::_PauseWork(FvWork& kWork)
{
	kWork._PauseByFlow();
}
void FvWorkFlow::_ResumeWork(FvWork& kWork)
{
	kWork._ResumeByFlow();
}
void FvWorkFlow::PauseCurrent()
{
	if(m_rpCurrentWork)
	{
		_PauseWork(*m_rpCurrentWork);
		m_rpCurrentWork = NULL;
	}
}
void FvWorkFlow::EndCurrent()
{
	if(m_rpCurrentWork)
	{
		_EndWork(*m_rpCurrentWork);
		m_rpCurrentWork = NULL;
	}
}
