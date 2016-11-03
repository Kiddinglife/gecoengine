//{future header message}
#ifndef __FvFastTimer_H__
#define __FvFastTimer_H__

#include "FvLogicCommonDefine.h"

#include "FvSimpleVector.h"
#include "FvMemoryNode.h"

#include <vector>
#include <FvRefList.h>
#include <FvMask.h>


//+------------------------------------------------------------------------------------------------------------
class FvFastTimer;

typedef FvUInt64 FvTime64;

class FV_LOGIC_COMMON_API FvTimeEventNodeInterface: private FvRefNode1<FvTimeEventNodeInterface*>
{
	FV_MEM_TRACKER(FvTimeEventNodeInterface);
	FV_NOT_COPY_COMPARE(FvTimeEventNodeInterface);
public:

	virtual void Exce(FvFastTimer& kTimer) = 0;

	void Detach();
	bool IsAttach()const{return FvRefNode1::IsAttach();}

	FvTime64 GetReserveDuration(const FvFastTimer& kTimer)const;/// 如果定时器还在等待, 返回一定不是0

	void SetTime(const FvUInt64 uiTime){m_uiTime = uiTime;}
	FvTime64 GetTime()const{return m_uiTime;}

protected:

	FvTimeEventNodeInterface();
	~FvTimeEventNodeInterface();

	FvTime64 m_uiTime;

	friend class FvFastTimer;
	friend class TimeRoll;
	friend class FvTimeAssisstant;

};


//+----------------------------------------------------------------------------------------------------

class FV_LOGIC_COMMON_API TimeRoll
{
	FV_MEM_TRACKER(TimeRoll);
	FV_NOT_COPY_COMPARE(TimeRoll);
	friend class FvFastTimer;
public:
	typedef FvTimeEventNodeInterface TimeNode;
	typedef FvRefList<TimeNode*, FvRefNode1> TimeEventList;


	void Init(const FvTime64 uiStartTime, const FvUInt32 uiRollSize, const FvUInt32 uiSpan);
	TimeEventList& GetCurrent();
	FvUInt32 Next();
	FvUInt32 GetSpan()const{return m_uiSpan;}
	FvTime64 GetTimeInf()const{return m_iTimeInf;}
	bool IsInRange(const FvTime64 uiTime)const;

	bool IsRoll()const{return (m_uiIdx == 0);}

	void Add(TimeNode& kNode);

	TimeRoll();
	~TimeRoll();

private:

	FvSimpleVector<TimeEventList> m_kTimeEventList;
	FvUInt32 m_uiIdx;
	FvUInt32 m_uiSpan;

	FvTime64 m_iTimeInf;
	FvTime64 m_iTimeSup;
};

//+----------------------------------------------------------------------------------------------------

class FV_LOGIC_COMMON_API FvFastTimer
{
	FV_MEM_TRACKER(FvFastTimer);
	FV_NOT_COPY_COMPARE(FvFastTimer);

	typedef FvTimeEventNodeInterface TimeNode;
	typedef FvRefList<TimeNode*, FvRefNode1> TimeEventList;

public:

	FvFastTimer(void);
	~FvFastTimer(void);

	void Start(const FvTime64 uiStartTime, const FvUInt32 uiSpan = 10, const FvUInt32 uiRoll_0 = 10, const FvUInt32 uiRoll_1 = 10);
	void End();
	void Update(const FvTime64 uiTime);

	void Add(TimeNode& kNode);
	FvTime64 GetTime()const{return m_uiAccumulateTime;}

private:

	static void _AddEvent(TimeEventList& kList, TimeNode& kNode);
	static void _AddFastEvent(TimeEventList& kList, TimeRoll& kTimeRoll);
	static void _AddEvent(TimeEventList& kList, TimeRoll& kTimeRoll);

	static void _UpdateTimeList(TimeEventList& kList, FvFastTimer& kTimer);
	static void _UpdateTimeList(TimeEventList& kList, const FvUInt64 uiTime);

	TimeRoll m_kTimeRoll_0;
	TimeRoll m_kTimeRoll_1;

	TimeEventList m_kReserveList;

	FvTime64 m_uiAccumulateTime;//! 当前时间
};


//+--------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! FastTime工具
class FV_LOGIC_COMMON_API FvTimeAssisstant
{
public:

	static void SetFrq(FvTimeEventNodeInterface&kNode,  const float fOldFrq, const float fNewFrq, FvFastTimer& kTimer)
	{
		if(kNode.IsAttach() == false)//! 如果回调已经发生过了, 则无法进行重新设置
		{
			return;
		}
		const FvTime64 currentTime = kTimer.GetTime();
		const FvInt64 uiDelta = kNode.m_uiTime - currentTime;
		const FvTime64 uiDeltaTimeOldMod = (uiDelta > 0)? uiDelta: 0;
		const FvTime64 uiDeltaTime = FvTime64(uiDeltaTimeOldMod*fOldFrq);
		const FvTime64 uiDeltaTimeNewMod = FvTime64(uiDeltaTime/fNewFrq);
		kNode.m_uiTime = kTimer.GetTime() + uiDeltaTimeNewMod;
		kTimer.Add(kNode);
	}

	static void Modify(FvTimeEventNodeInterface& kNode, const FvInt64& iDeltaTime, FvFastTimer& kTimer)
	{
		if(kNode.IsAttach() == false)//! 如果回调已经发生过了, 则无法进行重新设置
		{
			return;
		}
		if(FvInt64(kNode.m_uiTime) > -iDeltaTime)
		{
			kNode.m_uiTime += iDeltaTime;
		}
		else
		{
			kNode.m_uiTime = 0;
		}
		kTimer.Add(kNode);
	}

	static bool GetReserve(const FvTimeEventNodeInterface& kNode, FvTime64& uiReserve, const FvFastTimer& kTimer)
	{
		//! 如果还在等待回调, 返回值一定不是0
		if(kNode.IsAttach() == false)
		{
			uiReserve = 0;
			return false;
		}

		if(kNode.m_uiTime > kTimer.GetTime())
		{
			uiReserve = kNode.m_uiTime - kTimer.GetTime();
			return true;
		}
		else
		{
			uiReserve = 1;
			return true;
		}
	}
};

#endif//__FvFastTimer_H__