#include "FvLodTickTask.h"


#include <FvMath.h>

FvLodTickNode::FvLodTickNode(void)
:m_fSpan(0.0f)
,m_fAccumulateTime(0.0f)
{
	FV_MEM_TRACK;
}

FvLodTickNode::~FvLodTickNode(void)
{
}
void FvLodTickNode::SetSpan(const float fDeltaTime)
{
	m_fSpan = FvMathTool::Max(fDeltaTime, 0.0f);
}
void FvLodTickNode::Exec(const float fDeltaTime)
{
	const float fDeltaTimeMod = FvMathTool::Max(fDeltaTime, m_fSpan);
	m_fAccumulateTime += fDeltaTime;
	//FvLogBus::CritOk("FvLodTickNode::Exec m_fAccumulateTime(%f)", m_fAccumulateTime);
	if(m_fAccumulateTime >= fDeltaTimeMod)
	{
		m_fAccumulateTime -= fDeltaTimeMod;
		_Exec(fDeltaTimeMod);
	}
}
void FvLodTickNode::Detach()
{
	m_fAccumulateTime = 0.0f;
	FvRefNode1::Detach();
}


//+----------------------------------------------------------------------------------------------------
FvLodTickList::FvLodTickList()
{

}
FvLodTickList::~FvLodTickList(){}

void FvLodTickList::Attach(FvLodTickNode& kTask)
{
	if(!kTask.IsAttach())
	{
		m_TaskList.AttachBack(kTask);
	}
}

void FvLodTickList::UpdateList(const float fDeltaTime)
{
	m_TaskList.BeginIterator();
	while (!m_TaskList.IsEnd())
	{
		List::iterator iter = m_TaskList.GetIterator();
		FvLodTickNode* pkTask = static_cast<FvLodTickNode*>(iter);
		m_TaskList.Next();
		FV_ASSERT(pkTask);
		pkTask->Exec(fDeltaTime);
	}
}