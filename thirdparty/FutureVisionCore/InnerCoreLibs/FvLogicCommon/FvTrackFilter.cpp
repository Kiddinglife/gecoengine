#include "FvTrackFilter.h"

#include "FvLogBus.h"
#include "FvLogicDebug.h"

FvSmoothTime::FvSmoothTime()
:m_fLocal(0.0)
,m_fDrive(0.0)
,m_fIgnoreMinRange(0.5f)
,m_fIgnoreMaxRange(2.0f)
{
	
}	
void FvSmoothTime::SetIgnoreRange(const float fMinRange, const float fMaxRange)
{
	m_fIgnoreMinRange = fMinRange;
	m_fIgnoreMaxRange = fMaxRange;
}

void FvSmoothTime::Start(const double fTime)
{
	m_fDrive = fTime;
	m_fLocal = fTime;
}
float FvSmoothTime::Tick(const float fDeltaTime)
{
	float fDeltaTimeMod = fDeltaTime;
	if(fDeltaTime > 0)
	{
		fDeltaTime;
		m_fDrive += fDeltaTime;
		const double fNewLocalTime = m_fLocal + fDeltaTime;
		if(fNewLocalTime > m_fDrive + m_fIgnoreMinRange)
		{
			fDeltaTimeMod *= (m_fIgnoreMinRange + m_fIgnoreMaxRange)/float(fNewLocalTime - m_fDrive + m_fIgnoreMaxRange);
		}
		else if(fNewLocalTime + m_fIgnoreMinRange < m_fDrive)
		{
			fDeltaTimeMod *= float(m_fDrive - fNewLocalTime + m_fIgnoreMaxRange)/(m_fIgnoreMinRange + m_fIgnoreMaxRange);
		}
		m_fLocal += fDeltaTimeMod;
	}
	else
	{
		fDeltaTimeMod = 0.0f;
	}
	return fDeltaTimeMod;
}
void FvSmoothTime::Drive(const double fTime)
{
	if(m_fDrive > m_fLocal + m_fIgnoreMaxRange)
	{
		Start(fTime);
	}
	else
	{
		m_fDrive = fTime;
	}
}

void FvPositionRotateFilter::Add(const FvVector3& kPos, const float fYaw, const float fMovSpd, const double fExecTime)
{
	m_kList.push_back(Node(kPos, fYaw, fMovSpd, fExecTime));
}
bool FvPositionRotateFilter::Tick(const double fOldTime, const double fNewTime, FvVector3& kPos, float& fYaw)
{
	FV_ASSERT_WARNING(fNewTime > fOldTime);
	double fUpdateTimeLength = fNewTime - fOldTime;
	double fCurTime = fOldTime;
	bool bUpdated = false;
	while(m_kList.size()  && fUpdateTimeLength > 0.0f)
	{
		const Node& kNode = m_kList.front();
		if(_Update(fCurTime, fUpdateTimeLength, kPos, fYaw, kNode))
		{
			m_kList.pop_front();
		}
		bUpdated = true;
	}
	return bUpdated;
}
bool FvPositionRotateFilter::_Update(double& fCurTime, double& fDeltaTime, FvVector3& kPos, float& fYaw, const Node& kNode)
{
	FV_ASSERT_WARNING(fDeltaTime > 0.0f);
	const double fNewTime = fCurTime + fDeltaTime;
	if(fNewTime > kNode.m_fExecTime)
	{
		const float DEBUG_fOldYaw = fYaw;
		kPos = kNode.m_kPosition;
		fYaw = kNode.m_kYaw;
		fDeltaTime -= (kNode.m_fExecTime - fCurTime);
		fCurTime = kNode.m_fExecTime;
		FvLogBus::CritOk("OldPos(%f, %f, %f) Yaw(%f) SetPos NewPos(%f, %f, %f) Yaw(%f)"
			, kPos.x, kPos.y, kPos.z, DEBUG_fOldYaw, kNode.m_kPosition.x, kNode.m_kPosition.y, kNode.m_kPosition.z, fYaw);
		return true;
	}
	else
	{
		const FvVector3 DEBUG_kOldPos = kPos;
		const float DEBUG_fOldYaw = fYaw;

		const float fTimeLength = float(FvMathTool::Max(kNode.m_fExecTime - fCurTime, fDeltaTime));
		FV_ASSERT_ERROR(fTimeLength > 0.0f);
		const float fRate = float(fDeltaTime)/fTimeLength;

		FvAngle kAngle;
		kAngle.Lerp(fYaw, kNode.m_kYaw, fRate);

		if(kNode.m_kPosition != kPos)
		{
			kPos =  kNode.m_kPosition*fRate + kPos*(1.0f - fRate);
		}

		fYaw = kAngle;
		fCurTime += fDeltaTime;
		fDeltaTime = 0.0f;
		return false;
	}
}

//
////+----------------------------------------------------------------------------------------------------
bool FvSmothTimePositionRotateFilter::Tick(const float fDeltaTime, FvVector3& kPos, float& fYaw)
{
	FV_ASSERT_WARNING(fDeltaTime > 0.0f);
	double fCurTime = m_kSmoothTime.Time();
	double fUpdateTimeLength = m_kSmoothTime.Tick(fDeltaTime);
	return m_kPositionRotateFilter.Tick(fCurTime, fCurTime+fUpdateTimeLength, kPos, fYaw);
}