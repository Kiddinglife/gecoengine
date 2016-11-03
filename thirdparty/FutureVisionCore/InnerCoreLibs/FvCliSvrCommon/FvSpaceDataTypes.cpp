#include "FvSpaceDataTypes.h"
#include <math.h>

const double SECONDS_IN_DAY = 86400.0;	//! 一天多少秒


FvTimeOfDay::FvTimeOfDay()
:m_fServerTimeStart(0.0)
,m_fDayTimeStart(0.0)
,m_fTimeScaling(1.0f)
,m_bRunning(true)
{

}

bool FvTimeOfDay::Start(double fServerTime)
{
	if(m_bRunning)
		return false;
	Set(fServerTime, GetDayTime(fServerTime), m_fTimeScaling);
	m_bRunning = true;
	return true;
}

bool FvTimeOfDay::Stop(double fServerTime)
{
	if(!m_bRunning)
		return false;
	Set(0, GetDayTime(fServerTime), m_fTimeScaling);
	m_bRunning = false;
	return true;
}

bool FvTimeOfDay::Set(double fServerTime, double fDayTime, float fTimeScaling)
{
	fServerTime = fmod(fServerTime, SECONDS_IN_DAY);
	if(fServerTime < 0.0)
		fServerTime += SECONDS_IN_DAY;
	fDayTime = fmod(fDayTime, 24.0);
	if(fDayTime < 0.0)
		fDayTime += 24.0;

	if(m_fServerTimeStart==fServerTime && m_fDayTimeStart==fDayTime && m_fTimeScaling==fTimeScaling)
		return false;

	m_fServerTimeStart = fServerTime;
	m_fDayTimeStart = fDayTime;
	m_fTimeScaling = fTimeScaling;
	return true;
}

double FvTimeOfDay::GetDayTime(double fServerTime) const
{
	if(!m_bRunning)
		return m_fDayTimeStart;

	double fDayTime = (fServerTime-m_fServerTimeStart) * m_fTimeScaling / 3600.0 + m_fDayTimeStart;
	fDayTime = fmod(fDayTime, 24.0);
	if(fDayTime < 0.0)
		fDayTime += 24.0;
	return fDayTime;
}


