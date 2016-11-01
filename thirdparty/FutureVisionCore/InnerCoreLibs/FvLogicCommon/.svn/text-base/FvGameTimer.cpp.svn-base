#include "FvGameTimer.h"

#include "FvGameRandom.h"

FvFastTimer* FvGameTimer::ms_opTimer = NULL;
FvTime64 FvGameTimer::ms_uiGameStartTime = FvTime64(-1);
double FvGameTimer::ms_fLocalAccumulateTime = 0.0;
FvTime64 FvGameTimer::ms_uiLocalAccumulateTime = 0;

void FvGameTimer::Create(const FvTime64 uiTime, const FvUInt32 uiSpan /* = 10 */, const FvUInt32 uiRoll_0 /* = 10 */, const FvUInt32 uiRoll_1 /* = 10 */)
{
	if(ms_opTimer == NULL)
	{
		ms_opTimer = new FvFastTimer();
		ms_uiGameStartTime = uiTime;
		ms_fLocalAccumulateTime = 0.0;
		ms_uiLocalAccumulateTime = 0;
		ms_opTimer->Start(uiTime, uiSpan, uiRoll_0, uiRoll_1);
	}
}
void FvGameTimer::Destroy()
{
	FV_SAFE_DELETE(ms_opTimer);
}

FvFastTimer& FvGameTimer::Timer()
{
	FV_ASSERT(ms_opTimer && "时间还未初始化");
	return *ms_opTimer;
}

FvUInt64 FvGameTimer::Time()
{
	if(ms_opTimer)
	{
		return ms_opTimer->GetTime();
	}
	else
	{
		return 0;
	}
}
double FvGameTimer::LocalAccumulateTime()
{
	return ms_fLocalAccumulateTime;
}
void FvGameTimer::Update(const float& fDeltaTime)
{
	if(ms_uiGameStartTime != FvTime64(-1))
	{
		FV_ASSERT(ms_opTimer);
		ms_fLocalAccumulateTime += fDeltaTime;
		ms_uiLocalAccumulateTime = ms_uiGameStartTime + FvTime64(ms_fLocalAccumulateTime*100);
		ms_opTimer->Update(ms_uiLocalAccumulateTime);
	}
	SimpleTaskList().UpdateList(fDeltaTime);
	LodTaskList().UpdateList(fDeltaTime);
}
void FvGameTimer::Update(const FvTime64& uiDeltaTime)
{
	if(ms_uiGameStartTime != FvTime64(-1))
	{
		FV_ASSERT(ms_opTimer);
		const double fOldTime = ms_fLocalAccumulateTime;
		ms_uiLocalAccumulateTime += uiDeltaTime;
		ms_opTimer->Update(ms_uiGameStartTime + ms_uiLocalAccumulateTime);
		ms_fLocalAccumulateTime = ms_uiLocalAccumulateTime*0.01;
		const float fDeltaTime = float(ms_fLocalAccumulateTime - fOldTime);
		SimpleTaskList().UpdateList(fDeltaTime);
		LodTaskList().UpdateList(fDeltaTime);
	}
}