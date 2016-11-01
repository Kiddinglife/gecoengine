#include "FvGameRandom.h"


Random<100003> FvGameRandom::ms_RandomSet;
const float FvGameRandom::RE_MAX_VALUE = 1.0f/FvGameRandom::MAX_VALUE;


FvInt32 FvGameRandom::GetValue(const FvInt32& iInf, const FvInt32& iSup)
{
	FvInt32 iValue = ms_RandomSet.GetValue(iInf, iSup+1);
	//FV_ASSERT(iValue < (iSup+1));
	return iValue;
}
FvUInt32 FvGameRandom::GetValueByRange(const FvUInt32& uiValue, const FvUInt32& uiRange)
{
	FvInt32 iRange = uiRange;
	FV_ASSERT(iRange >= 0);
	FvInt32 iMod = FvGameRandom::GetValue(-iRange, iRange);
	iMod += FvInt32(uiValue);
	const FvUInt32 uiNewValue = (iMod>0)?iMod:0;
	return uiNewValue;
}

bool FvGameRandom::Probability(const float fValue)
{
	if(fValue >= 1.0f)
	{
		return true;
	}
	if(fValue <= 0.0f)
	{
		return false;
	}
	return ms_RandomSet.GetValue(fValue);
}
bool FvGameRandom::Probability(const FvInt32& iValue)
{
	if(iValue >= MAX_VALUE)
	{
		return true;
	}
	if(iValue <= 0)
	{
		return false;
	}
	return ms_RandomSet.GetValue(iValue*RE_MAX_VALUE);
}
FvUInt32 FvGameRandom::Probability(const std::vector<FvUInt32>& rangeList)
{
	FvUInt32 uiSup = 0;
	for (std::vector<FvUInt32>::const_iterator iter = rangeList.begin(); iter != rangeList.end(); ++iter)
	{
		uiSup += (*iter);
	}
	FvInt32 iValue = ms_RandomSet.GetValue(0, uiSup);
	FvUInt32 uiIdx = 0;
	for (std::vector<FvUInt32>::const_iterator iter = rangeList.begin(); iter != rangeList.end(); ++iter)
	{
		iValue -= (*iter);
		if(iValue < 0)
		{	
			return uiIdx;
		}
		++uiIdx;
	}
	FV_ASSERT(0);
	return 0;
}
bool FvGameRandom::Probability(const std::vector<FvUInt32>& rangeList, FvUInt32& uiIdx)
{
	FvUInt32 uiRangeSup = 0;
	for (std::vector<FvUInt32>::const_iterator iter = rangeList.begin(); iter != rangeList.end(); ++iter)
	{
		uiRangeSup += (*iter);
	}
	//uiRangeSup = (uiRangeSup > MAX_VALUE)? uiRangeSup: uiSup;//FvMathTool::Max(uiSup, uiRangeSup);
	FvInt32 iValue = ms_RandomSet.GetValue(0, MAX_VALUE);
	FvUInt32 uiIdx_ = 0;
	for (std::vector<FvUInt32>::const_iterator iter = rangeList.begin(); iter != rangeList.end(); ++iter)
	{
		iValue -= (*iter);
		if(iValue < 0)
		{	
			uiIdx = uiIdx_;
			return true;
		}
		++uiIdx_;
	}
	return false;
}