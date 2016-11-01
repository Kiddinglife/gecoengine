#include "FvFastTimer.h"

FvTimeEventNodeInterface::FvTimeEventNodeInterface()
:m_uiTime(0)
{
	FV_MEM_TRACK;
}
FvTimeEventNodeInterface::~FvTimeEventNodeInterface()
{
}
void FvTimeEventNodeInterface::Detach()
{
	FvRefNode1<FvTimeEventNodeInterface*>::Detach();
	m_uiTime = 0;
}
FvTime64 FvTimeEventNodeInterface::GetReserveDuration(const FvFastTimer& kTimer)const
{
	if(IsAttach())
	{
		return (m_uiTime > kTimer.GetTime())? (m_uiTime - kTimer.GetTime()): 1;
	}
	else
	{
		return 0;
	}
}
//+-----------------------------------------------------------------------------------------------------------------------------------
TimeRoll::TimeRoll()
:m_uiIdx(0)
,m_uiSpan(0)
,m_iTimeInf(0)
,m_iTimeSup(0)
{

}
TimeRoll::~TimeRoll()
{

}

void TimeRoll::Init(const FvTime64 uiStartTime, const FvUInt32 uiRollSize, const FvUInt32 uiSpan)
{
	FV_ASSERT(uiRollSize != 0);
	m_kTimeEventList.Resize(uiRollSize);
	m_uiSpan = uiSpan;
	m_iTimeInf = uiStartTime;
	m_iTimeSup = m_iTimeInf + uiSpan*uiRollSize;
}

TimeRoll::TimeEventList& TimeRoll::GetCurrent()
{
	FV_ASSERT(m_uiIdx < m_kTimeEventList.Size());
	return m_kTimeEventList[m_uiIdx];
}
FvUInt32 TimeRoll::Next()
{
	++m_uiIdx;
	if(m_uiIdx == m_kTimeEventList.Size())
	{
		m_uiIdx = 0;
	}
	m_iTimeInf += m_uiSpan;
	m_iTimeSup += m_uiSpan;
	return m_uiIdx;
}
void TimeRoll::Add(TimeNode& kNode)
{
	const FvUInt64 uiTime = kNode.GetTime();
	FV_ASSERT( m_iTimeInf <= uiTime && uiTime < m_iTimeSup);
	FvUInt32 uiSlot = m_uiIdx;
	if(uiTime > m_iTimeInf)
	{
		const FvUInt32 DeltaSlot = FvUInt32((uiTime - m_iTimeInf)/m_uiSpan);
		uiSlot = DeltaSlot + m_uiIdx;
		FV_ASSERT(DeltaSlot < m_kTimeEventList.Size());
		if(uiSlot >= m_kTimeEventList.Size())
		{
			uiSlot -= m_kTimeEventList.Size();
		}
	}
	FV_ASSERT(uiSlot < m_kTimeEventList.Size());
	TimeEventList& kList = m_kTimeEventList[uiSlot];
	kList.AttachBack(kNode);
}
bool TimeRoll::IsInRange(const FvTime64 uiTime)const
{
	return (m_iTimeInf <= uiTime && uiTime < m_iTimeSup);
}

//+-----------------------------------------------------------------------------------------------------------------------------------

FvFastTimer::FvFastTimer(void)
:m_uiAccumulateTime(0)
{

}

FvFastTimer::~FvFastTimer(void)
{

}
void FvFastTimer::Add(TimeNode& kNode)
{
	if(kNode.GetTime() < m_kTimeRoll_0.GetTimeInf())
	{
		kNode.SetTime(m_kTimeRoll_0.GetTimeInf());
	}
	if(m_kTimeRoll_0.IsInRange(kNode.GetTime()))
	{
		m_kTimeRoll_0.Add(kNode);
		return;
	}
	if(m_kTimeRoll_1.IsInRange(kNode.GetTime()))
	{
		m_kTimeRoll_1.Add(kNode);
		return;
	}
	_AddEvent(m_kReserveList, kNode);
}

//+-----------------------------------------------------------------------------------------------------------------------------------
//! 运行
void FvFastTimer::Start(const FvTime64 uiTime, const FvUInt32 uiSpan /* = 10 */, const FvUInt32 uiRoll_0 /* = 10 */, const FvUInt32 uiRoll_1 /* = 10 */)
{
	m_uiAccumulateTime = uiTime;
	m_kTimeRoll_0.Init(m_uiAccumulateTime, uiRoll_0, uiSpan);
	m_kTimeRoll_1.Init(m_uiAccumulateTime, uiRoll_1, uiSpan*uiRoll_0);
}
void FvFastTimer::Update(const FvTime64 uiTime)
{
	FV_ASSERT(m_uiAccumulateTime <= uiTime);
	if(m_uiAccumulateTime >= uiTime)
	{
		return;
	}
	//! Update迭代
	FvInt64 iUpdateTime = FvInt64(m_uiAccumulateTime);
	const FvInt64 iSpan = m_kTimeRoll_0.GetSpan();
	const FvInt64 iTopTime = FvInt64(uiTime) - iSpan;
	TimeEventList kUpdateList;
	while (iUpdateTime <= iTopTime)
	{
		//! 更新时间
		iUpdateTime += iSpan;
		if(m_kTimeRoll_0.IsRoll())
		{
			//FvLogBus::CritOk("刷入二级列表[%d]", m_kTimeRoll_1.m_uiIdx);
			_AddFastEvent(m_kTimeRoll_1.GetCurrent(), m_kTimeRoll_0);
			if(m_kTimeRoll_1.IsRoll())
			{
				//FvLogBus::CritOk("刷入三级列表");
				_AddEvent(m_kReserveList, m_kTimeRoll_1);
			}
			m_kTimeRoll_1.Next();
		}
		//FvLogBus::CritOk("TIME UPDATE [%d] >> ", iUpdateTime);
		m_uiAccumulateTime = iUpdateTime;
		TimeEventList& kTimeList = m_kTimeRoll_0.GetCurrent();
		m_kTimeRoll_0.Next();
		_UpdateTimeList(kTimeList, *this);
		//FvLogBus::CritOk("TIME UPDATE [%d] << ", iUpdateTime);
	}
}

//+-----------------------------------------------------------------------------------------------------------------------------------
void FvFastTimer::End()
{

}
void FvFastTimer::_AddEvent(TimeEventList& kList, TimeNode& kNode)
{
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		TimeEventList::iterator iter = kList.GetIterator();
		kList.Next();
		TimeNode* pkNode = iter->Content();
		FV_ASSERT(pkNode);
		if(pkNode->m_uiTime > kNode.m_uiTime)
		{
			kList.AttachBefore(*iter, kNode);
			kList.EndIterator();
			return;
		}
	}
	kList.AttachBack(kNode);
}
void FvFastTimer::_AddFastEvent(TimeEventList& kList, TimeRoll& kTimeRoll)
{
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		TimeNode* pkNode = kList.GetIterator()->Content();
		kList.Next();
		FV_ASSERT(pkNode);
		TimeNode& kNode = *pkNode;
		if(kTimeRoll.IsInRange(kNode.GetTime()))
		{
			kTimeRoll.Add(kNode);
		}
		else
		{
			FV_ASSERT(0);
		}
	}
	FV_ASSERT(kList.Size() == 0);
}
void FvFastTimer::_AddEvent(TimeEventList& kList, TimeRoll& kTimeRoll)
{
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		TimeNode* pkNode = kList.GetIterator()->Content();
		kList.Next();
		FV_ASSERT(pkNode);
		TimeNode& kNode = *pkNode;
		if(kTimeRoll.IsInRange(kNode.GetTime()))
		{
			kTimeRoll.Add(kNode);
		}
		else
		{
			kList.EndIterator();
			break;
		}
	}
}
void FvFastTimer::_UpdateTimeList(TimeEventList& kList, FvFastTimer& kTimer)
{
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		TimeNode* pkNode = kList.GetIterator()->Content();
		kList.Next();
		FV_ASSERT(pkNode);
		pkNode->Detach();
		pkNode->Exce(kTimer);
	}
	FV_ASSERT(kList.Size() == 0);
}
