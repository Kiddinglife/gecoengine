//{future source message}
#include <stdlib.h>
#include <string.h>

#include "FvMathPower.h"

//----------------------------------------------------------------------------
float FV_POWER_API FvRoundf(float fValue)
{
	return FvFloorf(fValue + 0.5f);
}
//----------------------------------------------------------------------------
int FV_POWER_API FvRoundfToInt(float fValue)
{
	return (int)FvRoundf(fValue);
}
//----------------------------------------------------------------------------
double FV_POWER_API FvRound(double dValue)
{
	return FvFloor(dValue + 0.5);
}
//----------------------------------------------------------------------------
int FV_POWER_API FvRoundToInt(double dValue)
{
	return (int)FvRound(dValue);
}
//----------------------------------------------------------------------------
FV_BOOL FV_POWER_API FvPowerOfTwo(unsigned int uiNumber)
{
	return uiNumber ? ((uiNumber & (- (int)uiNumber)) == uiNumber) : FV_FALSE;
}
//----------------------------------------------------------------------------
float FV_POWER_API FvDecayf(float fSrcValue, float fDstValue,
	float fHalfLife, float fDeltaTime)
{
	if (fHalfLife <= 0.0f)
	{
		return fDstValue;
	}
	else
	{
		return fDstValue + FvPowf(0.5f, fDeltaTime / fHalfLife) *
			(fSrcValue - fDstValue);
	}
}
//----------------------------------------------------------------------------
double FV_POWER_API FvDecay(double dSrcValue, double dDstValue,
	double dHalfLife, double dDeltaTime)
{
	if (dHalfLife <= 0.0)
	{
		return dDstValue;
	}
	else
	{
		return dDstValue + FvPow(0.5, dDeltaTime / dHalfLife) *
			(dSrcValue - dDstValue);
	}
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSrand(unsigned int uiSeed)
{
	srand(uiSeed);
}
//----------------------------------------------------------------------------
int FV_POWER_API FvRand()
{
	return rand();
}
//----------------------------------------------------------------------------
float FV_POWER_API FvSymmetricRandomf()
{
	return -1.0f + ((float)FvRand()) * (2.0f / ((float)RAND_MAX));
}
//----------------------------------------------------------------------------
float FV_POWER_API FvUnitRandomf()
{
	return ((float)FvRand()) * (1.0f / ((float)RAND_MAX));
}
//----------------------------------------------------------------------------
float FV_POWER_API FvRangeRandomf(float fMin, float fMax)
{
	return fMin + FvUnitRandomf() * (fMax - fMin);
}
//----------------------------------------------------------------------------
float FV_POWER_API FvFastInvSqrtf(float fValue)
{
	float fHalf = 0.5f*fValue;
	int i  = *(int*)&fValue;
	i = 0x5f3759df - (i >> 1);							//What the fuck!
	fValue = *(float*)&i;
	fValue = fValue*(1.5f - fHalf*fValue*fValue);
	return fValue;
}
//----------------------------------------------------------------------------
float FV_POWER_API FvFastSqrtf(float fValue)
{
	return FvFastInvSqrtf(fValue) * fValue;
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSinCosf(float* fpSin, float* fpCos, float fRadians)
{
	*fpSin = FvSinf(fRadians);
	*fpCos = FvCosf(fRadians);
}
//----------------------------------------------------------------------------
float FV_POWER_API FvNormaliseAnglef(float fAangle)
{
	if (fAangle > FV_MATH_PI_F)
	{
		if (fAangle > FV_MATH_3PI_F)
		{
			return fAangle -
				FvFloorf((fAangle + FV_MATH_PI_F) / FV_MATH_2PI_F) *
				FV_MATH_2PI_F;
		}
		else
		{
			return fAangle - FV_MATH_2PI_F;
		}
	}
	else if (fAangle <= -FV_MATH_PI_F)
	{
		if (fAangle <= -FV_MATH_3PI_F)
		{
			return fAangle -
				FvFloorf((fAangle + FV_MATH_PI_F) / FV_MATH_2PI_F) *
				FV_MATH_2PI_F;
		}
		else
		{
			return fAangle + FV_MATH_2PI_F;
		}
	}
	else
	{
		return fAangle;
	}
}
//----------------------------------------------------------------------------
float FV_POWER_API FvSameSignAnglef(float fAangle, float fClosest)
{
	if (fClosest > fAangle + FV_MATH_PI_F)
	{
		return fClosest - 2.0f * FV_MATH_PI_F;
	}
	else if (fClosest < fAangle - FV_MATH_PI_F)
	{
		return fClosest + 2.0f * FV_MATH_PI_F;
	}
	else
	{
		return fClosest;
	}
}
//----------------------------------------------------------------------------
float FV_POWER_API FvTurnRangeAnglef(float fFrom, float fTo)
{
	float fDiff = fTo - fFrom;

	return (fDiff < 0.f) ? fDiff + FV_MATH_2PI_F : fDiff;
}
//----------------------------------------------------------------------------
