//{future header message}
#ifndef __FvGameTimer_H__
#define __FvGameTimer_H__

#include "FvLogicCommonDefine.h"

#include "FvFastTimer.h"
#include "FvTimeNodeEx.h"

#include "FvTickTask.h"
#include "FvLodTickTask.h"



//! µ¥¼þ

class FV_LOGIC_COMMON_API FvGameTimer
{
public:

	static void Create(const FvTime64 uiTime, const FvUInt32 uiSpan = 10, const FvUInt32 uiRoll_0 = 10, const FvUInt32 uiRoll_1 = 10);
	static void Destroy();

	//+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T>
	static void SetTime(const FvUInt32& uiDeltaTime, FvTimeEventNode1<T>& kTimeNode, void (T::*callbackFunc)(FvTimeEventNodeInterface&))
	{
		kTimeNode.SetTime(FvUInt64(Time() + uiDeltaTime));
		kTimeNode.SetFunc(callbackFunc);
		Timer().Add(kTimeNode);
	}	
	template<class T, class Param>
	static void SetTime(const FvUInt32& uiDeltaTime, FvTimeEventNodeEx1<T, Param>& kTimeNode, void (T::*callbackFunc)(const Param&, FvTimeEventNodeInterface&))
	{
		kTimeNode.SetTime(FvUInt64(Time() + uiDeltaTime));
		kTimeNode.SetFunc(callbackFunc);
		Timer().Add(kTimeNode);
	}
	static void SetTime(const FvUInt32& uiDeltaTime, FvRelocatableTimeNode& kTimeNode)
	{
		kTimeNode.SetTime(FvUInt64(Time() + uiDeltaTime));
		Timer().Add(kTimeNode);
	}

	//+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T>
	static void SetTime(const FvUInt64& uiDeltaTime, FvTimeEventNode1<T>& kTimeNode, void (T::*callbackFunc)(FvTimeEventNodeInterface&))
	{
		kTimeNode.SetTime(Time() + uiDeltaTime);
		kTimeNode.SetFunc(callbackFunc);
		Timer().Add(kTimeNode);
	}
	template<class T, class Param>
	static void SetTime(const FvUInt64& uiDeltaTime, FvTimeEventNodeEx1<T, Param>& kTimeNode, void (T::*callbackFunc)(const Param&, FvTimeEventNodeInterface&))
	{
		kTimeNode.SetTime(FvUInt64(Time() + uiDeltaTime));
		kTimeNode.SetFunc(callbackFunc);
		Timer().Add(kTimeNode);
	}
	static void SetTime(const FvUInt64& uiDeltaTime, FvRelocatableTimeNode& kTimeNode)
	{
		kTimeNode.SetTime(Time() + uiDeltaTime);
		Timer().Add(kTimeNode);
	}

	//+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T>
	static void SetTime(const float& fDeltaTime, FvTimeEventNode1<T>& kTimeNode, void (T::*callbackFunc)(FvTimeEventNodeInterface&))
	{
		const FvUInt64 uiDeltaTime = (fDeltaTime > 0.0f) ? FvUInt64(fDeltaTime*100): 1;
		kTimeNode.SetTime(Time() + uiDeltaTime);
		kTimeNode.SetFunc(callbackFunc);
		Timer().Add(kTimeNode);
	}
	template<class T, class Param>
	static void SetTime(const float& fDeltaTime, FvTimeEventNodeEx1<T, Param>& kTimeNode, void (T::*callbackFunc)(const Param&, FvTimeEventNodeInterface&))
	{
		const FvUInt64 uiDeltaTime = (fDeltaTime > 0.0f) ? FvUInt64(fDeltaTime*100): 1;
		kTimeNode.SetTime(FvUInt64(Time() + uiDeltaTime));
		kTimeNode.SetFunc(callbackFunc);
		Timer().Add(kTimeNode);
	}
	static void SetTime(const float& fDeltaTime, FvRelocatableTimeNode& kTimeNode)
	{
		const FvUInt64 uiDeltaTime = (fDeltaTime > 0.0f) ? FvUInt64(fDeltaTime*100): 1;
		kTimeNode.SetTime(Time() + uiDeltaTime);
		Timer().Add(kTimeNode);
	}

	//+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	static void ModifyTime(const float& fDeltaTime, FvTimeEventNodeInterface& kTimeNode)
	{
		if(kTimeNode.IsAttach())
		{
			FvTimeAssisstant::Modify(kTimeNode, FvInt64(fDeltaTime*100), Timer());
		}
	}
	static void ModifyTime(const FvInt32& iDeltaTime, FvTimeEventNodeInterface& kTimeNode)
	{
		if(kTimeNode.IsAttach())
		{
			FvTimeAssisstant::Modify(kTimeNode, FvInt64(iDeltaTime), Timer());
		}
	}

	//+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	static FvTime64 Time();
	static double LocalAccumulateTime();
	static void Update(const float& fDeltaTime);
	static void Update(const FvTime64& uiDeltaTime);

	static FvFastTimer& Timer();
	
	static FvBaseTickTaskList& SimpleTaskList()
	{
		static FvBaseTickTaskList S_LIST;
		return S_LIST;
	}
	static FvLodTickList& LodTaskList()
	{
		static FvLodTickList S_LIST;
		return S_LIST;
	}

private:

	FvGameTimer(void);
	~FvGameTimer(void);

	static FvFastTimer* ms_opTimer;
	static FvTime64 ms_uiGameStartTime;
	static double ms_fLocalAccumulateTime;
	static FvTime64 ms_uiLocalAccumulateTime;

};

#endif// __FvGameTimer_H__
