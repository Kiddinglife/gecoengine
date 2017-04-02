//{future source message}
#include "Geco-math-power.h"

#include <cstdlib>
#include <cstring>

//----------------------------------------------------------------------------
float  GecoRoundf(float fValue)
{
	return GecoFloorf(fValue + 0.5f);
}
//----------------------------------------------------------------------------
int  GecoRoundfToInt(float fValue)
{
	return (int)GecoRoundf(fValue);
}
//----------------------------------------------------------------------------
double  GecoRound(double dValue)
{
	return GecoFloor(dValue + 0.5);
}
//----------------------------------------------------------------------------
int  GecoRoundToInt(double dValue)
{
	return (int)GecoRound(dValue);
}
//----------------------------------------------------------------------------
bool GecoPowerOfTwo(unsigned int uiNumber)
{
	return uiNumber ? ((uiNumber & (-(int)uiNumber)) == uiNumber) : false;
}
//----------------------------------------------------------------------------
float  GecoDecayf(float fSrcValue, float fDstValue,
	float fHalfLife, float fDeltaTime)
{
	if (fHalfLife <= 0.0f)
	{
		return fDstValue;
	}
	else
	{
		return fDstValue + GecoPowf(0.5f, fDeltaTime / fHalfLife) *
			(fSrcValue - fDstValue);
	}
}
//----------------------------------------------------------------------------
double  GecoDecay(double dSrcValue, double dDstValue,
	double dHalfLife, double dDeltaTime)
{
	if (dHalfLife <= 0.0)
	{
		return dDstValue;
	}
	else
	{
		return dDstValue + GecoPow(0.5, dDeltaTime / dHalfLife) *
			(dSrcValue - dDstValue);
	}
}
//----------------------------------------------------------------------------
void  GecoSrand(unsigned int uiSeed)
{
	srand(uiSeed);
}
//----------------------------------------------------------------------------
int  GecoRand()
{
	return rand();
}
//----------------------------------------------------------------------------
float  GecoSymmetricRandomf()
{
	return -1.0f + ((float)GecoRand()) * (2.0f / ((float)RAND_MAX));
}
//----------------------------------------------------------------------------
float  GecoUnitRandomf()
{
	return ((float)GecoRand()) * (1.0f / ((float)RAND_MAX));
}
//----------------------------------------------------------------------------
float  GecoRangeRandomf(float fMin, float fMax)
{
	return fMin + GecoUnitRandomf() * (fMax - fMin);
}
//----------------------------------------------------------------------------
float  GecoFastInvSqrtf(float fValue)
{
	float fHalf = 0.5f*fValue;
	int i = *(int*)&fValue;
	i = 0x5f3759df - (i >> 1);							//What the fuck!
	fValue = *(float*)&i;
	fValue = fValue*(1.5f - fHalf*fValue*fValue);
	return fValue;
}
//----------------------------------------------------------------------------
float  GecoFastSqrtf(float fValue)
{
	return GecoFastInvSqrtf(fValue) * fValue;
}
//----------------------------------------------------------------------------
void  GecoSinCosf(float* fpSin, float* fpCos, float fRadians)
{
	*fpSin = GecoSinf(fRadians);
	*fpCos = GecoCosf(fRadians);
}
//----------------------------------------------------------------------------
float  GecoNormaliseAnglef(float fAangle)
{
	if (fAangle > GECO_MATH_PI_F)
	{
		if (fAangle > GECO_MATH_3PI_F)
		{
			return fAangle -
				GecoFloorf((fAangle + GECO_MATH_PI_F) / GECO_MATH_2PI_F) *
				GECO_MATH_2PI_F;
		}
		else
		{
			return fAangle - GECO_MATH_2PI_F;
		}
	}
	else if (fAangle <= -GECO_MATH_PI_F)
	{
		if (fAangle <= -GECO_MATH_3PI_F)
		{
			return fAangle -
				GecoFloorf((fAangle + GECO_MATH_PI_F) / GECO_MATH_2PI_F) *
				GECO_MATH_2PI_F;
		}
		else
		{
			return fAangle + GECO_MATH_2PI_F;
		}
	}
	else
	{
		return fAangle;
	}
}
//----------------------------------------------------------------------------
float  GecoSameSignAnglef(float fAangle, float fClosest)
{
	if (fClosest > fAangle + GECO_MATH_PI_F)
	{
		return fClosest - 2.0f * GECO_MATH_PI_F;
	}
	else if (fClosest < fAangle - GECO_MATH_PI_F)
	{
		return fClosest + 2.0f * GECO_MATH_PI_F;
	}
	else
	{
		return fClosest;
	}
}
//----------------------------------------------------------------------------
float  GecoTurnRangeAnglef(float fFrom, float fTo)
{
	float fDiff = fTo - fFrom;

	return (fDiff < 0.f) ? fDiff + GECO_MATH_2PI_F : fDiff;
}
//----------------------------------------------------------------------------

#include <assert.h>
//----------------------------------------------------------------------------
void  GecoZero2f(float* fpDst)
{
	assert(fpDst);
	geco_zero_mem_ex(fpDst, sizeof(float), 2);
}
//----------------------------------------------------------------------------
void  GecoZero3f(float* fpDst)
{
	assert(fpDst);
	geco_zero_mem_ex(fpDst, sizeof(float), 3);
}
//----------------------------------------------------------------------------
void  GecoZero4f(float* fpDst)
{
	assert(fpDst);
	geco_zero_mem_ex(fpDst, sizeof(float), 4);
}
//----------------------------------------------------------------------------
void  GecoZero9f(float* fpDst)
{
	assert(fpDst);
	geco_zero_mem_ex(fpDst, sizeof(float), 9);
}
//----------------------------------------------------------------------------
void  GecoZero16f(float* fpDst)
{
	assert(fpDst);
	geco_zero_mem_ex(fpDst, sizeof(float), 16);
}
//----------------------------------------------------------------------------
void  GecoSet2f(float* fpDst, const float* fpSrc)
{
	assert(fpDst);
	geco_mem_cpy_ex(fpDst, fpSrc, sizeof(float), 2);
}
//----------------------------------------------------------------------------
void  GecoSet3f(float* fpDst, const float* fpSrc)
{
	assert(fpDst);
	geco_mem_cpy_ex(fpDst, fpSrc, sizeof(float), 3);
}
//----------------------------------------------------------------------------
void  GecoSet4f(float* fpDst, const float* fpSrc)
{
	assert(fpDst);
	geco_mem_cpy_ex(fpDst, fpSrc, sizeof(float), 4);
}
//----------------------------------------------------------------------------
void  GecoSet9f(float* fpDst, const float* fpSrc)
{
	assert(fpDst);
	geco_mem_cpy_ex(fpDst, fpSrc, sizeof(float), 9);
}
//----------------------------------------------------------------------------
void  GecoSet16f(float* fpDst, const float* fpSrc)
{
	assert(fpDst);
	geco_mem_cpy_ex(fpDst, fpSrc, sizeof(float), 16);
}
//----------------------------------------------------------------------------
void  GecoAdd2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] + fpSrc1[0];
	fpDst[1] = fpSrc0[1] + fpSrc1[1];
}
//----------------------------------------------------------------------------
void  GecoAdd3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] + fpSrc1[0];
	fpDst[1] = fpSrc0[1] + fpSrc1[1];
	fpDst[2] = fpSrc0[2] + fpSrc1[2];
}
//----------------------------------------------------------------------------
void  GecoAdd4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] + fpSrc1[0];
	fpDst[1] = fpSrc0[1] + fpSrc1[1];
	fpDst[2] = fpSrc0[2] + fpSrc1[2];
	fpDst[3] = fpSrc0[3] + fpSrc1[3];
}
//----------------------------------------------------------------------------
void  GecoAdd9f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	GecoAdd3f(fpDst, fpSrc0, fpSrc1);
	GecoAdd3f(fpDst + 3, fpSrc0 + 3, fpSrc1 + 3);
	GecoAdd3f(fpDst + 6, fpSrc0 + 6, fpSrc1 + 6);
}
//----------------------------------------------------------------------------
void  GecoAdd16f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	GecoAdd4f(fpDst, fpSrc0, fpSrc1);
	GecoAdd4f(fpDst + 4, fpSrc0 + 4, fpSrc1 + 4);
	GecoAdd4f(fpDst + 8, fpSrc0 + 8, fpSrc1 + 8);
	GecoAdd4f(fpDst + 12, fpSrc0 + 12, fpSrc1 + 12);
}
//----------------------------------------------------------------------------
void  GecoSub2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] - fpSrc1[0];
	fpDst[1] = fpSrc0[1] - fpSrc1[1];
}
//----------------------------------------------------------------------------
void  GecoSub3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] - fpSrc1[0];
	fpDst[1] = fpSrc0[1] - fpSrc1[1];
	fpDst[2] = fpSrc0[2] - fpSrc1[2];
}
//----------------------------------------------------------------------------
void  GecoSub4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] - fpSrc1[0];
	fpDst[1] = fpSrc0[1] - fpSrc1[1];
	fpDst[2] = fpSrc0[2] - fpSrc1[2];
	fpDst[3] = fpSrc0[3] - fpSrc1[3];
}
//----------------------------------------------------------------------------
void  GecoSub9f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	GecoSub3f(fpDst, fpSrc0, fpSrc1);
	GecoSub3f(fpDst + 3, fpSrc0 + 3, fpSrc1 + 3);
	GecoSub3f(fpDst + 6, fpSrc0 + 6, fpSrc1 + 6);
}
//----------------------------------------------------------------------------
void  GecoSub16f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	GecoSub4f(fpDst, fpSrc0, fpSrc1);
	GecoSub4f(fpDst + 4, fpSrc0 + 4, fpSrc1 + 4);
	GecoSub4f(fpDst + 8, fpSrc0 + 8, fpSrc1 + 8);
	GecoSub4f(fpDst + 12, fpSrc0 + 12, fpSrc1 + 12);
}
//----------------------------------------------------------------------------
void  GecoMul2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] * fpSrc1[0];
	fpDst[1] = fpSrc0[1] * fpSrc1[1];
}
//----------------------------------------------------------------------------
void  GecoMul3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] * fpSrc1[0];
	fpDst[1] = fpSrc0[1] * fpSrc1[1];
	fpDst[2] = fpSrc0[2] * fpSrc1[2];
}
//----------------------------------------------------------------------------
void  GecoMul4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] * fpSrc1[0];
	fpDst[1] = fpSrc0[1] * fpSrc1[1];
	fpDst[2] = fpSrc0[2] * fpSrc1[2];
	fpDst[3] = fpSrc0[3] * fpSrc1[3];
}
//----------------------------------------------------------------------------
void  GecoMul9f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	GecoMul3f(fpDst, fpSrc0, fpSrc1);
	GecoMul3f(fpDst + 3, fpSrc0 + 3, fpSrc1 + 3);
	GecoMul3f(fpDst + 6, fpSrc0 + 6, fpSrc1 + 6);
}
//----------------------------------------------------------------------------
void  GecoMul16f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	GecoMul4f(fpDst, fpSrc0, fpSrc1);
	GecoMul4f(fpDst + 4, fpSrc0 + 4, fpSrc1 + 4);
	GecoMul4f(fpDst + 8, fpSrc0 + 8, fpSrc1 + 8);
	GecoMul4f(fpDst + 12, fpSrc0 + 12, fpSrc1 + 12);
}
//----------------------------------------------------------------------------
void  GecoDiv2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] / fpSrc1[0];
	fpDst[1] = fpSrc0[1] / fpSrc1[1];
}
//----------------------------------------------------------------------------
void  GecoDiv3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] / fpSrc1[0];
	fpDst[1] = fpSrc0[1] / fpSrc1[1];
	fpDst[2] = fpSrc0[2] / fpSrc1[2];
}
//----------------------------------------------------------------------------
void  GecoDiv4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = fpSrc0[0] / fpSrc1[0];
	fpDst[1] = fpSrc0[1] / fpSrc1[1];
	fpDst[2] = fpSrc0[2] / fpSrc1[2];
	fpDst[3] = fpSrc0[3] / fpSrc1[3];
}
//----------------------------------------------------------------------------
void  GecoDiv9f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	GecoDiv3f(fpDst, fpSrc0, fpSrc1);
	GecoDiv3f(fpDst + 3, fpSrc0 + 3, fpSrc1 + 3);
	GecoDiv3f(fpDst + 6, fpSrc0 + 6, fpSrc1 + 6);
}
//----------------------------------------------------------------------------
void  GecoDiv16f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	GecoDiv4f(fpDst, fpSrc0, fpSrc1);
	GecoDiv4f(fpDst + 4, fpSrc0 + 4, fpSrc1 + 4);
	GecoDiv4f(fpDst + 8, fpSrc0 + 8, fpSrc1 + 8);
	GecoDiv4f(fpDst + 12, fpSrc0 + 12, fpSrc1 + 12);
}
//----------------------------------------------------------------------------
void  GecoScale2f(float* fpDst, const float* fpSrc, float fScale)
{
	assert(fpSrc);
	assert(fpDst);

	fpDst[0] = fpSrc[0] * fScale;
	fpDst[1] = fpSrc[1] * fScale;
}
//----------------------------------------------------------------------------
void  GecoScale3f(float* fpDst, const float* fpSrc, float fScale)
{
	assert(fpSrc);
	assert(fpDst);

	fpDst[0] = fpSrc[0] * fScale;
	fpDst[1] = fpSrc[1] * fScale;
	fpDst[2] = fpSrc[2] * fScale;
}
//----------------------------------------------------------------------------
void  GecoScale4f(float* fpDst, const float* fpSrc, float fScale)
{
	assert(fpSrc);
	assert(fpDst);

	fpDst[0] = fpSrc[0] * fScale;
	fpDst[1] = fpSrc[1] * fScale;
	fpDst[2] = fpSrc[2] * fScale;
	fpDst[3] = fpSrc[3] * fScale;
}
//----------------------------------------------------------------------------
void  GecoScale9f(float* fpDst, const float* fpSrc, float fScale)
{
	assert(fpSrc);
	assert(fpDst);

	GecoScale3f(fpDst, fpSrc, fScale);
	GecoScale3f(fpDst + 3, fpSrc + 3, fScale);
	GecoScale3f(fpDst + 6, fpSrc + 6, fScale);
}
//----------------------------------------------------------------------------
void  GecoScale16f(float* fpDst, const float* fpSrc, float fScale)
{
	assert(fpSrc);
	assert(fpDst);

	GecoScale4f(fpDst, fpSrc, fScale);
	GecoScale4f(fpDst + 4, fpSrc + 4, fScale);
	GecoScale4f(fpDst + 8, fpSrc + 8, fScale);
	GecoScale4f(fpDst + 12, fpSrc + 12, fScale);
}
//----------------------------------------------------------------------------
float  GecoLengthSqr2f(const float* fpSrc)
{
	assert(fpSrc);

	return GecoDot2f(fpSrc, fpSrc);
}
//----------------------------------------------------------------------------
float  GecoLengthSqr3f(const float* fpSrc)
{
	assert(fpSrc);

	return GecoDot3f(fpSrc, fpSrc);
}
//----------------------------------------------------------------------------
float  GecoLengthSqr4f(const float* fpSrc)
{
	assert(fpSrc);

	return GecoDot4f(fpSrc, fpSrc);
}
//----------------------------------------------------------------------------
float  GecoLength2f(const float* fpSrc)
{
	assert(fpSrc);

	return GecoSqrtf(GecoLengthSqr2f(fpSrc));
}
//----------------------------------------------------------------------------
float  GecoLength3f(const float* fpSrc)
{
	assert(fpSrc);

	return GecoSqrtf(GecoLengthSqr3f(fpSrc));
}
//----------------------------------------------------------------------------
float  GecoLength4f(const float* fpSrc)
{
	assert(fpSrc);

	return GecoSqrtf(GecoLengthSqr4f(fpSrc));
}
//----------------------------------------------------------------------------
void  GecoNormalize2f(float* fpDst, const float* fpSrc)
{
	assert(fpSrc);
	assert(fpDst);

	GecoScale2f(fpDst, fpSrc, GecoInvSqrtf(GecoLengthSqr2f(fpSrc)));
}
//----------------------------------------------------------------------------
void  GecoNormalize3f(float* fpDst, const float* fpSrc)
{
	assert(fpSrc);
	assert(fpDst);

	GecoScale3f(fpDst, fpSrc, GecoInvSqrtf(GecoLengthSqr3f(fpSrc)));
}
//----------------------------------------------------------------------------
void  GecoNormalize4f(float* fpDst, const float* fpSrc)
{
	assert(fpSrc);
	assert(fpDst);

	GecoScale4f(fpDst, fpSrc, GecoInvSqrtf(GecoLengthSqr4f(fpSrc)));
}
//----------------------------------------------------------------------------
float  GecoDot2f(const float* fpSrc0, const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);

	return fpSrc0[0] * fpSrc1[0] + fpSrc0[1] * fpSrc1[1];
}
//----------------------------------------------------------------------------
float  GecoDot3f(const float* fpSrc0, const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);

	return fpSrc0[0] * fpSrc1[0] + fpSrc0[1] * fpSrc1[1]
		+ fpSrc0[2] * fpSrc1[2];
}
//----------------------------------------------------------------------------
float  GecoDot4f(const float* fpSrc0, const float* fpSrc1)
{
	assert(fpSrc1);
	assert(fpSrc0);

	return fpSrc0[0] * fpSrc1[0] + fpSrc0[1] * fpSrc1[1]
		+ fpSrc0[2] * fpSrc1[2] + fpSrc0[3] * fpSrc1[3];
}
//----------------------------------------------------------------------------
void  GecoLerp2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1, float t)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = GECO_LERP(t, fpSrc0[0], fpSrc1[0]);
	fpDst[1] = GECO_LERP(t, fpSrc0[1], fpSrc1[1]);
}
//----------------------------------------------------------------------------
void  GecoLerp3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1, float t)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = GECO_LERP(t, fpSrc0[0], fpSrc1[0]);
	fpDst[1] = GECO_LERP(t, fpSrc0[1], fpSrc1[1]);
	fpDst[2] = GECO_LERP(t, fpSrc0[2], fpSrc1[2]);
}
//----------------------------------------------------------------------------
void  GecoLerp4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1, float t)
{
	assert(fpSrc1);
	assert(fpSrc0);
	assert(fpDst);

	fpDst[0] = GECO_LERP(t, fpSrc0[0], fpSrc1[0]);
	fpDst[1] = GECO_LERP(t, fpSrc0[1], fpSrc1[1]);
	fpDst[2] = GECO_LERP(t, fpSrc0[2], fpSrc1[2]);
	fpDst[3] = GECO_LERP(t, fpSrc0[3], fpSrc1[3]);
}
//----------------------------------------------------------------------------
float  GecoVector2Crossf(const float* fpVec0, const float* fpVec1)
{
	return fpVec0[0] * fpVec1[1] - fpVec0[1] * fpVec1[0];
}
//----------------------------------------------------------------------------
void  GecoVector3Crossf(float* fpOut, const float* fpVec0,
	const float* fpVec1)
{
	float afVec0[3];
	float afVec1[3];

	assert(fpVec1);
	assert(fpVec0);
	assert(fpOut);

	GecoSet3f(afVec0, fpVec0);
	GecoSet3f(afVec1, fpVec1);

	fpOut[0] = (afVec0[1] * afVec1[2]) - (afVec0[2] * afVec1[1]);
	fpOut[1] = (afVec0[2] * afVec1[0]) - (afVec0[0] * afVec1[2]);
	fpOut[2] = (afVec0[0] * afVec1[1]) - (afVec0[1] * afVec1[0]);
}
//----------------------------------------------------------------------------
void  GecoVector3TransformM4f(float* fpOutVec4, const float* fpVec3,
	const float* fpMat4x4)
{
	float afVec3[3];
	float afMat4x4[16];

	assert(fpMat4x4);
	assert(fpVec3);
	assert(fpOutVec4);

	GecoSet3f(afVec3, fpVec3);
	GecoSet16f(afMat4x4, fpMat4x4);

	fpOutVec4[0] = afVec3[0] * afMat4x4[0] + afVec3[1] * afMat4x4[4] +
		afVec3[2] * afMat4x4[8] + afMat4x4[12];
	fpOutVec4[1] = afVec3[0] * afMat4x4[1] + afVec3[1] * afMat4x4[5] +
		afVec3[2] * afMat4x4[9] + afMat4x4[13];
	fpOutVec4[2] = afVec3[0] * afMat4x4[2] + afVec3[1] * afMat4x4[6] +
		afVec3[2] * afMat4x4[10] + afMat4x4[14];
	fpOutVec4[3] = afVec3[0] * afMat4x4[3] + afVec3[1] * afMat4x4[7] +
		afVec3[2] * afMat4x4[11] + afMat4x4[15];
}
//----------------------------------------------------------------------------
void  GecoVector3TransformCoordM4f(float* fpOutVec3,
	const float* fpVec3, const float* fpMat4x4)
{
	float afVec3[3];
	float afMat4x4[16];
	float fInvw;

	assert(fpMat4x4);
	assert(fpVec3);
	assert(fpOutVec3);

	GecoSet3f(afVec3, fpVec3);
	GecoSet16f(afMat4x4, fpMat4x4);

	fInvw = 1.0f / (afVec3[0] * afMat4x4[3] + afVec3[1] *
		afMat4x4[7] + afVec3[2] * afMat4x4[11] + afMat4x4[15]);

	fpOutVec3[0] = fpOutVec3[1] = fpOutVec3[2] = fInvw;

	fpOutVec3[0] *= afVec3[0] * afMat4x4[0] + afVec3[1] * afMat4x4[4] +
		afVec3[2] * afMat4x4[8] + afMat4x4[12];
	fpOutVec3[1] *= afVec3[0] * afMat4x4[1] + afVec3[1] * afMat4x4[5] +
		afVec3[2] * afMat4x4[9] + afMat4x4[13];
	fpOutVec3[2] *= afVec3[0] * afMat4x4[2] + afVec3[1] * afMat4x4[6] +
		afVec3[2] * afMat4x4[10] + afMat4x4[14];
}
//----------------------------------------------------------------------------
void  GecoVector3TransformNormalM4f(float* fpOutVec3,
	const float* fpVec3, const float* fpMat4x4)
{
	float afVec3[3];
	float afMat4x4[16];

	assert(fpMat4x4);
	assert(fpVec3);
	assert(fpOutVec3);

	GecoSet3f(afVec3, fpVec3);
	GecoSet16f(afMat4x4, fpMat4x4);

	fpOutVec3[0] = afVec3[0] * afMat4x4[0] + afVec3[1] * afMat4x4[4] +
		afVec3[2] * afMat4x4[8];
	fpOutVec3[1] = afVec3[0] * afMat4x4[1] + afVec3[1] * afMat4x4[5] +
		afVec3[2] * afMat4x4[9];
	fpOutVec3[2] = afVec3[0] * afMat4x4[2] + afVec3[1] * afMat4x4[6] +
		afVec3[2] * afMat4x4[10];
}
//----------------------------------------------------------------------------
void  GecoVector3SetPitchYawYUpf(float* fpOutVec3, float fPitchInRadians,
	float fYawInRadians)
{
	float fCosPitch = GecoCosf(fPitchInRadians);
	float fSinPitch = GecoSinf(-fPitchInRadians);

	float fCosYaw = GecoCosf(fYawInRadians);
	float fSinYaw = GecoSinf(fYawInRadians);

	fpOutVec3[0] = fCosPitch * fSinYaw;
	fpOutVec3[1] = fSinPitch;
	fpOutVec3[2] = fCosPitch * fCosYaw;
}
//----------------------------------------------------------------------------
float  GecoVector3GetPitchYUpf(float* fpVec3)
{
	return -GecoAtan2f(fpVec3[1], GecoSqrtf(fpVec3[0] * fpVec3[0] +
		fpVec3[2] * fpVec3[2]));
}
//----------------------------------------------------------------------------
float  GecoVector3GetYawYUpf(float* fpVec3)
{
	return GecoAtan2f(fpVec3[0], fpVec3[2]);
}
//----------------------------------------------------------------------------
void  GecoVector3SetPitchYawZUpf(float* fpOutVec3, float fPitchInRadians,
	float fYawInRadians)
{
	float fCosPitch = GecoCosf(fPitchInRadians);
	float fSinPitch = GecoSinf(-fPitchInRadians);

	float fCosYaw = GecoCosf(fYawInRadians);
	float fSinYaw = GecoSinf(fYawInRadians);

	fpOutVec3[0] = fCosPitch * fSinYaw;
	fpOutVec3[1] = fCosPitch * fCosYaw;
	fpOutVec3[2] = fSinPitch;
}
//----------------------------------------------------------------------------
float  GecoVector3GetPitchZUpf(float* fpVec3)
{
	return -GecoAtan2f(fpVec3[2], GecoSqrtf(fpVec3[0] * fpVec3[0] +
		fpVec3[1] * fpVec3[1]));
}
//----------------------------------------------------------------------------
float  GecoVector3GetYawZUpf(float* fpVec3)
{
	return GecoAtan2f(fpVec3[0], fpVec3[1]);
}
//----------------------------------------------------------------------------
void  GecoVector4Transformf(float* fpOutVec4, const float* fpVec4,
	const float* fpMat4x4)
{
	float afVec4[4];
	float afMat4x4[16];

	assert(fpMat4x4);
	assert(fpVec4);
	assert(fpOutVec4);

	GecoSet4f(afVec4, fpVec4);
	GecoSet16f(afMat4x4, fpMat4x4);

	fpOutVec4[0] = afVec4[0] * afMat4x4[0] + afVec4[1] * afMat4x4[4] +
		afVec4[2] * afMat4x4[8] + afVec4[3] * afMat4x4[12];
	fpOutVec4[1] = afVec4[0] * afMat4x4[1] + afVec4[1] * afMat4x4[5] +
		afVec4[2] * afMat4x4[9] + afVec4[3] * afMat4x4[13];
	fpOutVec4[2] = afVec4[0] * afMat4x4[2] + afVec4[1] * afMat4x4[6] +
		afVec4[2] * afMat4x4[10] + afVec4[3] * afMat4x4[14];
	fpOutVec4[3] = afVec4[0] * afMat4x4[3] + afVec4[1] * afMat4x4[7] +
		afVec4[2] * afMat4x4[11] + afVec4[3] * afMat4x4[15];
}
//----------------------------------------------------------------------------
GECO_OUT_CODE  GecoVector4Outcode(const float* fpVec4)
{
	GECO_OUT_CODE eCode = GECO_OUTCODE_NULL;

	if (fpVec4[0] < (-fpVec4[3]))
		eCode |= GECO_OUTCODE_LEFT;

	if (fpVec4[0] > fpVec4[3])
		eCode |= GECO_OUTCODE_RIGHT;

	if (fpVec4[1] < (-fpVec4[3]))
		eCode |= GECO_OUTCODE_BOTTOM;

	if (fpVec4[1] > fpVec4[3])
		eCode |= GECO_OUTCODE_TOP;

	if (fpVec4[2] < 0)
		eCode |= GECO_OUTCODE_NEAR;

	if (fpVec4[2] > fpVec4[3])
		eCode |= GECO_OUTCODE_FAR;

	return eCode;
}
//----------------------------------------------------------------------------
void  GecoMatrixIdenty4f(float* fpOutMat4x4)
{
	assert(fpOutMat4x4);

	fpOutMat4x4[1] = fpOutMat4x4[2] = fpOutMat4x4[3] =
		fpOutMat4x4[4] = fpOutMat4x4[6] = fpOutMat4x4[7] =
		fpOutMat4x4[8] = fpOutMat4x4[9] = fpOutMat4x4[11] =
		fpOutMat4x4[12] = fpOutMat4x4[13] = fpOutMat4x4[14] = 0;
	fpOutMat4x4[0] = fpOutMat4x4[5] = fpOutMat4x4[10] =
		fpOutMat4x4[15] = 1.0f;
}
//----------------------------------------------------------------------------
bool  GecoMatrixInverse4f(float* fpOutMat4x4, float* fpDeterminant,
	const float* fpMat4x4)
{
	float afMat4x4[16];
	float v0, v1, v2, v3, v4, v5;
	float t00, t10, t20, t30;
	float fDet, fInvDet;

	assert(fpMat4x4);
	assert(fpOutMat4x4);

	GecoSet16f(afMat4x4, fpMat4x4);

	v0 = afMat4x4[8] * afMat4x4[13] - afMat4x4[9] * afMat4x4[12];
	v1 = afMat4x4[8] * afMat4x4[14] - afMat4x4[10] * afMat4x4[12];
	v2 = afMat4x4[8] * afMat4x4[15] - afMat4x4[11] * afMat4x4[12];
	v3 = afMat4x4[9] * afMat4x4[14] - afMat4x4[10] * afMat4x4[13];
	v4 = afMat4x4[9] * afMat4x4[15] - afMat4x4[11] * afMat4x4[13];
	v5 = afMat4x4[10] * afMat4x4[15] - afMat4x4[11] * afMat4x4[14];

	t00 = +(v5 * afMat4x4[5] - v4 * afMat4x4[6] + v3 * afMat4x4[7]);
	t10 = -(v5 * afMat4x4[4] - v2 * afMat4x4[6] + v1 * afMat4x4[7]);
	t20 = +(v4 * afMat4x4[4] - v2 * afMat4x4[5] + v0 * afMat4x4[7]);
	t30 = -(v3 * afMat4x4[4] - v1 * afMat4x4[5] + v0 * afMat4x4[6]);

	fDet = (t00 * afMat4x4[0] + t10 * afMat4x4[1] + t20 * afMat4x4[2] +
		t30 * afMat4x4[3]);

	if (fpDeterminant)
		*fpDeterminant = fDet;

	if (fDet == 0) return false;

	fInvDet = 1.0f / fDet;

	fpOutMat4x4[0] = t00 * fInvDet;
	fpOutMat4x4[4] = t10 * fInvDet;
	fpOutMat4x4[8] = t20 * fInvDet;
	fpOutMat4x4[12] = t30 * fInvDet;

	fpOutMat4x4[1] = -(v5 * afMat4x4[1] - v4 * afMat4x4[2] + v3 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[5] = +(v5 * afMat4x4[0] - v2 * afMat4x4[2] + v1 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[9] = -(v4 * afMat4x4[0] - v2 * afMat4x4[1] + v0 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[13] = +(v3 * afMat4x4[0] - v1 * afMat4x4[1] + v0 *
		afMat4x4[2]) * fInvDet;

	v0 = afMat4x4[4] * afMat4x4[13] - afMat4x4[5] * afMat4x4[12];
	v1 = afMat4x4[4] * afMat4x4[14] - afMat4x4[6] * afMat4x4[12];
	v2 = afMat4x4[4] * afMat4x4[15] - afMat4x4[7] * afMat4x4[12];
	v3 = afMat4x4[5] * afMat4x4[14] - afMat4x4[6] * afMat4x4[13];
	v4 = afMat4x4[5] * afMat4x4[15] - afMat4x4[7] * afMat4x4[13];
	v5 = afMat4x4[6] * afMat4x4[15] - afMat4x4[7] * afMat4x4[14];

	fpOutMat4x4[2] = +(v5 * afMat4x4[1] - v4 * afMat4x4[2] + v3 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[6] = -(v5 * afMat4x4[0] - v2 * afMat4x4[2] + v1 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[10] = +(v4 * afMat4x4[0] - v2 * afMat4x4[1] + v0 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[14] = -(v3 * afMat4x4[0] - v1 * afMat4x4[1] + v0 *
		afMat4x4[2]) * fInvDet;

	v0 = afMat4x4[9] * afMat4x4[4] - afMat4x4[8] * afMat4x4[5];
	v1 = afMat4x4[10] * afMat4x4[4] - afMat4x4[8] * afMat4x4[6];
	v2 = afMat4x4[11] * afMat4x4[4] - afMat4x4[8] * afMat4x4[7];
	v3 = afMat4x4[10] * afMat4x4[5] - afMat4x4[9] * afMat4x4[6];
	v4 = afMat4x4[11] * afMat4x4[5] - afMat4x4[9] * afMat4x4[7];
	v5 = afMat4x4[11] * afMat4x4[6] - afMat4x4[10] * afMat4x4[7];

	fpOutMat4x4[3] = -(v5 * afMat4x4[1] - v4 * afMat4x4[2] + v3 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[7] = +(v5 * afMat4x4[0] - v2 * afMat4x4[2] + v1 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[11] = -(v4 * afMat4x4[0] - v2 * afMat4x4[1] + v0 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[15] = +(v3 * afMat4x4[0] - v1 * afMat4x4[1] + v0 *
		afMat4x4[2]) * fInvDet;

	return true;
}
//----------------------------------------------------------------------------
void  GecoMatrixTranspose4f(float* fpOutMat4x4,
	const float* fpMat4x4)
{
	float afMat4x4[16];

	assert(fpMat4x4);
	assert(fpOutMat4x4);

	GecoSet16f(afMat4x4, fpMat4x4);

	fpOutMat4x4[0] = afMat4x4[0];
	fpOutMat4x4[1] = afMat4x4[4];
	fpOutMat4x4[2] = afMat4x4[8];
	fpOutMat4x4[3] = afMat4x4[12];

	fpOutMat4x4[4] = afMat4x4[1];
	fpOutMat4x4[5] = afMat4x4[5];
	fpOutMat4x4[6] = afMat4x4[9];
	fpOutMat4x4[7] = afMat4x4[13];

	fpOutMat4x4[8] = afMat4x4[2];
	fpOutMat4x4[9] = afMat4x4[6];
	fpOutMat4x4[10] = afMat4x4[10];
	fpOutMat4x4[11] = afMat4x4[14];

	fpOutMat4x4[12] = afMat4x4[3];
	fpOutMat4x4[13] = afMat4x4[7];
	fpOutMat4x4[14] = afMat4x4[11];
	fpOutMat4x4[15] = afMat4x4[15];
}
//----------------------------------------------------------------------------
float  GecoMatrixDeterminant4f(const float* fpMat4x4)
{
	float v0, v1, v2, v3, v4, v5;
	float t00, t10, t20, t30;

	assert(fpMat4x4);

	v0 = fpMat4x4[8] * fpMat4x4[13] - fpMat4x4[9] * fpMat4x4[12];
	v1 = fpMat4x4[8] * fpMat4x4[14] - fpMat4x4[10] * fpMat4x4[12];
	v2 = fpMat4x4[8] * fpMat4x4[15] - fpMat4x4[11] * fpMat4x4[12];
	v3 = fpMat4x4[9] * fpMat4x4[14] - fpMat4x4[10] * fpMat4x4[13];
	v4 = fpMat4x4[9] * fpMat4x4[15] - fpMat4x4[11] * fpMat4x4[13];
	v5 = fpMat4x4[10] * fpMat4x4[15] - fpMat4x4[11] * fpMat4x4[14];

	t00 = +(v5 * fpMat4x4[5] - v4 * fpMat4x4[6] + v3 * fpMat4x4[7]);
	t10 = -(v5 * fpMat4x4[4] - v2 * fpMat4x4[6] + v1 * fpMat4x4[7]);
	t20 = +(v4 * fpMat4x4[4] - v2 * fpMat4x4[5] + v0 * fpMat4x4[7]);
	t30 = -(v3 * fpMat4x4[4] - v1 * fpMat4x4[5] + v0 * fpMat4x4[6]);

	return (t00 * fpMat4x4[0] + t10 * fpMat4x4[1] + t20 * fpMat4x4[2] +
		t30 * fpMat4x4[3]);
}
//----------------------------------------------------------------------------
void  GecoMatrixMultiply4f(float* fpOutMat4x4,
	const float* fpMat4x4Src0, const float* fpMat4x4Src1)
{
	float afMat4x4Src0[16];
	float afMat4x4Src1[16];

	assert(fpMat4x4Src1);
	assert(fpMat4x4Src0);
	assert(fpOutMat4x4);

	GecoSet16f(afMat4x4Src0, fpMat4x4Src0);
	GecoSet16f(afMat4x4Src1, fpMat4x4Src1);

	fpOutMat4x4[0] = afMat4x4Src0[0] * afMat4x4Src1[0] + afMat4x4Src0[1] *
		afMat4x4Src1[4] + afMat4x4Src0[2] * afMat4x4Src1[8] +
		afMat4x4Src0[3] * afMat4x4Src1[12];
	fpOutMat4x4[1] = afMat4x4Src0[0] * afMat4x4Src1[1] + afMat4x4Src0[1] *
		afMat4x4Src1[5] + afMat4x4Src0[2] * afMat4x4Src1[9] +
		afMat4x4Src0[3] * afMat4x4Src1[13];
	fpOutMat4x4[2] = afMat4x4Src0[0] * afMat4x4Src1[2] + afMat4x4Src0[1] *
		afMat4x4Src1[6] + afMat4x4Src0[2] * afMat4x4Src1[10] +
		afMat4x4Src0[3] * afMat4x4Src1[14];
	fpOutMat4x4[3] = afMat4x4Src0[0] * afMat4x4Src1[3] + afMat4x4Src0[1] *
		afMat4x4Src1[7] + afMat4x4Src0[2] * afMat4x4Src1[11] +
		afMat4x4Src0[3] * afMat4x4Src1[15];

	fpOutMat4x4[4] = afMat4x4Src0[4] * afMat4x4Src1[0] + afMat4x4Src0[5] *
		afMat4x4Src1[4] + afMat4x4Src0[6] * afMat4x4Src1[8] +
		afMat4x4Src0[7] * afMat4x4Src1[12];
	fpOutMat4x4[5] = afMat4x4Src0[4] * afMat4x4Src1[1] + afMat4x4Src0[5] *
		afMat4x4Src1[5] + afMat4x4Src0[6] * afMat4x4Src1[9] +
		afMat4x4Src0[7] * afMat4x4Src1[13];
	fpOutMat4x4[6] = afMat4x4Src0[4] * afMat4x4Src1[2] + afMat4x4Src0[5] *
		afMat4x4Src1[6] + afMat4x4Src0[6] * afMat4x4Src1[10] +
		afMat4x4Src0[7] * afMat4x4Src1[14];
	fpOutMat4x4[7] = afMat4x4Src0[4] * afMat4x4Src1[3] + afMat4x4Src0[5] *
		afMat4x4Src1[7] + afMat4x4Src0[6] * afMat4x4Src1[11] +
		afMat4x4Src0[7] * afMat4x4Src1[15];

	fpOutMat4x4[8] = afMat4x4Src0[8] * afMat4x4Src1[0] + afMat4x4Src0[9] *
		afMat4x4Src1[4] + afMat4x4Src0[10] * afMat4x4Src1[8] +
		afMat4x4Src0[11] * afMat4x4Src1[12];
	fpOutMat4x4[9] = afMat4x4Src0[8] * afMat4x4Src1[1] + afMat4x4Src0[9] *
		afMat4x4Src1[5] + afMat4x4Src0[10] * afMat4x4Src1[9] +
		afMat4x4Src0[11] * afMat4x4Src1[13];
	fpOutMat4x4[10] = afMat4x4Src0[8] * afMat4x4Src1[2] + afMat4x4Src0[9] *
		afMat4x4Src1[6] + afMat4x4Src0[10] * afMat4x4Src1[10] +
		afMat4x4Src0[11] * afMat4x4Src1[14];
	fpOutMat4x4[11] = afMat4x4Src0[8] * afMat4x4Src1[3] + afMat4x4Src0[9] *
		afMat4x4Src1[7] + afMat4x4Src0[10] * afMat4x4Src1[11] +
		afMat4x4Src0[11] * afMat4x4Src1[15];

	fpOutMat4x4[12] = afMat4x4Src0[12] * afMat4x4Src1[0] + afMat4x4Src0[13] *
		afMat4x4Src1[4] + afMat4x4Src0[14] * afMat4x4Src1[8] +
		afMat4x4Src0[15] * afMat4x4Src1[12];
	fpOutMat4x4[13] = afMat4x4Src0[12] * afMat4x4Src1[1] + afMat4x4Src0[13] *
		afMat4x4Src1[5] + afMat4x4Src0[14] * afMat4x4Src1[9] +
		afMat4x4Src0[15] * afMat4x4Src1[13];
	fpOutMat4x4[14] = afMat4x4Src0[12] * afMat4x4Src1[2] + afMat4x4Src0[13] *
		afMat4x4Src1[6] + afMat4x4Src0[14] * afMat4x4Src1[10] +
		afMat4x4Src0[15] * afMat4x4Src1[14];
	fpOutMat4x4[15] = afMat4x4Src0[12] * afMat4x4Src1[3] + afMat4x4Src0[13] *
		afMat4x4Src1[7] + afMat4x4Src0[14] * afMat4x4Src1[11] +
		afMat4x4Src0[15] * afMat4x4Src1[15];
}
//----------------------------------------------------------------------------
void  GecoMatrixRotationX4f(float* fpOutMat4x4, float fAngle)
{
	float fSin, fCos;

	assert(fpOutMat4x4);

	GecoSinCosf(&fSin, &fCos, fAngle);
	fpOutMat4x4[0] = 1.0f;
	fpOutMat4x4[1] = 0.0f;
	fpOutMat4x4[2] = 0.0f;
	fpOutMat4x4[3] = 0.0f;
	fpOutMat4x4[4] = 0.0f;
	fpOutMat4x4[5] = fCos;
	fpOutMat4x4[6] = fSin;
	fpOutMat4x4[7] = 0.0f;
	fpOutMat4x4[8] = 0.0f;
	fpOutMat4x4[9] = -fSin;
	fpOutMat4x4[10] = fCos;
	fpOutMat4x4[11] = 0.0f;
	fpOutMat4x4[12] = 0.0f;
	fpOutMat4x4[13] = 0.0f;
	fpOutMat4x4[14] = 0.0f;
	fpOutMat4x4[15] = 1.0f;
}
//----------------------------------------------------------------------------
void  GecoMatrixRotationY4f(float* fpOutMat4x4, float fAngle)
{
	float fSin, fCos;

	assert(fpOutMat4x4);

	GecoSinCosf(&fSin, &fCos, fAngle);
	fpOutMat4x4[0] = fCos;
	fpOutMat4x4[1] = 0.0f;
	fpOutMat4x4[2] = -fSin;
	fpOutMat4x4[3] = 0.0f;
	fpOutMat4x4[4] = 0.0f;
	fpOutMat4x4[5] = 1.0f;
	fpOutMat4x4[6] = 0.0f;
	fpOutMat4x4[7] = 0.0f;
	fpOutMat4x4[8] = fSin;
	fpOutMat4x4[9] = 0.0f;
	fpOutMat4x4[10] = fCos;
	fpOutMat4x4[11] = 0.0f;
	fpOutMat4x4[12] = 0.0f;
	fpOutMat4x4[13] = 0.0f;
	fpOutMat4x4[14] = 0.0f;
	fpOutMat4x4[15] = 1.0f;
}
//----------------------------------------------------------------------------
void  GecoMatrixRotationZ4f(float* fpOutMat4x4, float fAngle)
{
	float fSin, fCos;

	assert(fpOutMat4x4);

	GecoSinCosf(&fSin, &fCos, fAngle);
	fpOutMat4x4[0] = fCos;
	fpOutMat4x4[1] = fSin;
	fpOutMat4x4[2] = 0.0f;
	fpOutMat4x4[3] = 0.0f;
	fpOutMat4x4[4] = -fSin;
	fpOutMat4x4[5] = fCos;
	fpOutMat4x4[6] = 0.0f;
	fpOutMat4x4[7] = 0.0f;
	fpOutMat4x4[8] = 0.0f;
	fpOutMat4x4[9] = 0.0f;
	fpOutMat4x4[10] = 1.0f;
	fpOutMat4x4[11] = 0.0f;
	fpOutMat4x4[12] = 0.0f;
	fpOutMat4x4[13] = 0.0f;
	fpOutMat4x4[14] = 0.0f;
	fpOutMat4x4[15] = 1.0f;
}
//----------------------------------------------------------------------------
void  GecoMatrixScaling4f(float* fpOutMat4x4, float fScaleX,
	float fScaleY, float fScaleZ)
{
	assert(fpOutMat4x4);

	fpOutMat4x4[0] = fScaleX;
	fpOutMat4x4[1] = 0.0f;
	fpOutMat4x4[2] = 0.0f;
	fpOutMat4x4[3] = 0.0f;
	fpOutMat4x4[4] = 0.0f;
	fpOutMat4x4[5] = fScaleY;
	fpOutMat4x4[6] = 0.0f;
	fpOutMat4x4[7] = 0.0f;
	fpOutMat4x4[8] = 0.0f;
	fpOutMat4x4[9] = 0.0f;
	fpOutMat4x4[10] = fScaleZ;
	fpOutMat4x4[11] = 0.0f;
	fpOutMat4x4[12] = 0.0f;
	fpOutMat4x4[13] = 0.0f;
	fpOutMat4x4[14] = 0.0f;
	fpOutMat4x4[15] = 1.0f;
}
//----------------------------------------------------------------------------
void  GecoMatrixTranslation4f(float* fpOutMat4x4, float x, float y,
	float z)
{
	assert(fpOutMat4x4);

	fpOutMat4x4[0] = 1.0f;
	fpOutMat4x4[1] = 0.0f;
	fpOutMat4x4[2] = 0.0f;
	fpOutMat4x4[3] = 0.0f;
	fpOutMat4x4[4] = 0.0f;
	fpOutMat4x4[5] = 1.0f;
	fpOutMat4x4[6] = 0.0f;
	fpOutMat4x4[7] = 0.0f;
	fpOutMat4x4[8] = 0.0f;
	fpOutMat4x4[9] = 0.0f;
	fpOutMat4x4[10] = 1.0f;
	fpOutMat4x4[11] = 0.0f;
	fpOutMat4x4[12] = x;
	fpOutMat4x4[13] = y;
	fpOutMat4x4[14] = z;
	fpOutMat4x4[15] = 1.0f;
}
//----------------------------------------------------------------------------
void  GecoMatrixRotationYawPitchRoll4f(float* fpOutMat4x4,
	float fYaw, float fPitch, float fRoll)
{
	float afMat4x4Temp[16];

	assert(fpOutMat4x4);

	GecoMatrixRotationZ4f(fpOutMat4x4, fRoll);
	GecoMatrixRotationX4f(afMat4x4Temp, fPitch);
	GecoMatrixMultiply4f(fpOutMat4x4, fpOutMat4x4, afMat4x4Temp);
	GecoMatrixRotationY4f(afMat4x4Temp, fYaw);
	GecoMatrixMultiply4f(fpOutMat4x4, fpOutMat4x4, afMat4x4Temp);
}
//----------------------------------------------------------------------------
void  GecoMatrixRotationQuaternion4f(float* fpOutMat4x4,
	const float* fpQuat)
{
	const float fTx = 2.0f * fpQuat[0];
	const float fTy = 2.0f * fpQuat[1];
	const float fTz = 2.0f * fpQuat[2];
	const float fTwx = fTx * fpQuat[3];
	const float fTwy = fTy * fpQuat[3];
	const float fTwz = fTz * fpQuat[3];
	const float fTxx = fTx * fpQuat[0];
	const float fTxy = fTy * fpQuat[0];
	const float fTxz = fTz * fpQuat[0];
	const float fTyy = fTy * fpQuat[1];
	const float fTyz = fTz * fpQuat[1];
	const float fTzz = fTz * fpQuat[2];

	fpOutMat4x4[0] = 1.0f - (fTyy + fTzz);
	fpOutMat4x4[1] = fTxy + fTwz;
	fpOutMat4x4[2] = fTxz - fTwy;
	fpOutMat4x4[3] = 0.0f;
	fpOutMat4x4[4] = fTxy - fTwz;
	fpOutMat4x4[5] = 1.0f - (fTxx + fTzz);
	fpOutMat4x4[6] = fTyz + fTwx;
	fpOutMat4x4[7] = 0.0f;
	fpOutMat4x4[8] = fTxz + fTwy;
	fpOutMat4x4[9] = fTyz - fTwx;
	fpOutMat4x4[10] = 1.0f - (fTxx + fTyy);
	fpOutMat4x4[11] = 0.0f;
	fpOutMat4x4[12] = 0.0f;
	fpOutMat4x4[13] = 0.0f;
	fpOutMat4x4[14] = 0.0f;
	fpOutMat4x4[15] = 1.0f;
}
//----------------------------------------------------------------------------
void  GecoQuaternionRotationMatrixf(float* fpOutQuat,
	const float* fpMat4x4)
{
	static unsigned int s_uiNext[3] = { 1, 2, 0 };
	float fTrace = fpMat4x4[0] + fpMat4x4[5] + fpMat4x4[10];
	float fRoot;
	unsigned int i, j, k;

	if (fTrace > 0.0f)
	{
		fRoot = GecoSqrtf(fTrace + 1.0f);
		fpOutQuat[3] = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;
		fpOutQuat[0] = (fpMat4x4[6] - fpMat4x4[9]) * fRoot;
		fpOutQuat[1] = (fpMat4x4[8] - fpMat4x4[2]) * fRoot;
		fpOutQuat[2] = (fpMat4x4[1] - fpMat4x4[4]) * fRoot;
	}
	else
	{

		i = 0;
		if (fpMat4x4[5] > fpMat4x4[0])
			i = 1;
		if (fpMat4x4[10] > fpMat4x4[0])
			i = 2;
		j = s_uiNext[i];
		k = s_uiNext[j];

		fRoot = GecoSqrtf(fpMat4x4[i * 4 + i] - fpMat4x4[j * 4 + j] -
			fpMat4x4[k * 4 + k] + 1.0f);
		fpOutQuat[i] = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;
		fpOutQuat[3] = (fpMat4x4[j * 4 + k] - fpMat4x4[k * 4 + j]) * fRoot;
		fpOutQuat[j] = (fpMat4x4[i * 4 + j] + fpMat4x4[j * 4 + i]) * fRoot;
		fpOutQuat[k] = (fpMat4x4[i * 4 + k] + fpMat4x4[k * 4 + i]) * fRoot;
	}
}
//----------------------------------------------------------------------------
void  GecoQuaternionRotationAxisf(float* fpOutQuat,
	const float* fpVec3, float fAngle)
{
	float fSin, fCos;
	GecoSinCosf(&fSin, &fCos, 0.5f * fAngle);

	fpOutQuat[0] = fSin * fpVec3[0];
	fpOutQuat[1] = fSin * fpVec3[1];
	fpOutQuat[2] = fSin * fpVec3[2];
	fpOutQuat[3] = fCos;
}
//----------------------------------------------------------------------------
void  GecoQuaternionMultiplyf(float* fpOutQuat, const float* fpQuat0,
	const float* fpQuat1)
{
	fpOutQuat[0] = fpQuat0[3] * fpQuat1[0] + fpQuat0[0] * fpQuat1[3] -
		fpQuat0[1] * fpQuat1[2] + fpQuat0[2] * fpQuat1[1];
	fpOutQuat[1] = fpQuat0[3] * fpQuat1[1] + fpQuat0[1] * fpQuat1[3] -
		fpQuat0[2] * fpQuat1[0] + fpQuat0[0] * fpQuat1[2];
	fpOutQuat[2] = fpQuat0[3] * fpQuat1[2] + fpQuat0[2] * fpQuat1[3] -
		fpQuat0[0] * fpQuat1[1] + fpQuat0[1] * fpQuat1[0];
	fpOutQuat[3] = fpQuat0[3] * fpQuat1[3] - fpQuat0[0] * fpQuat1[0] -
		fpQuat0[1] * fpQuat1[1] - fpQuat0[2] * fpQuat1[2];
}
//----------------------------------------------------------------------------
void  GecoQuaternionInversef(float* fpOutQuat, const float* fpQuat)
{
	float fNorm = GecoLengthSqr4f(fpQuat);
	if (fNorm > 0)
	{
		float fInvNorm = 1.0f / fNorm;
		fpOutQuat[0] = -fInvNorm * fpQuat[0];
		fpOutQuat[1] = -fInvNorm * fpQuat[1];
		fpOutQuat[2] = -fInvNorm * fpQuat[2];
		fpOutQuat[3] = fInvNorm * fpQuat[3];
	}
	else
	{
		fpOutQuat[0] = 0;
		fpOutQuat[1] = 0;
		fpOutQuat[2] = 0;
		fpOutQuat[3] = 0;
	}
}
//----------------------------------------------------------------------------
void  GecoMatrixDecomposeWorld4f(float* fpOutQuat,
	float* fpOutVec3Scale, float* fpOutVec3Trans, const float* fpMat4x4)
{
	float afMat4x4[16];
	float fScaleX;
	float fScaleY;
	float fScaleZ;

	assert(fpMat4x4[3] == 0.0f && fpMat4x4[7] == 0.0f &&
		fpMat4x4[11] == 0.0f && fpMat4x4[15] == 1.0f);

	assert(fpMat4x4);
	assert(fpOutVec3Trans);
	assert(fpOutVec3Scale);
	assert(fpOutQuat);

	GecoSet16f(afMat4x4, fpMat4x4);

	fpOutVec3Trans[0] = afMat4x4[12];
	fpOutVec3Trans[1] = afMat4x4[13];
	fpOutVec3Trans[2] = afMat4x4[14];

	fScaleX = GecoLength3f(&afMat4x4[0]);
	fScaleY = GecoLength3f(&afMat4x4[4]);
	fScaleZ = GecoLength3f(&afMat4x4[8]);

	GecoScale3f(&afMat4x4[0], &afMat4x4[0], 1.0f / fScaleX);
	GecoScale3f(&afMat4x4[4], &afMat4x4[4], 1.0f / fScaleY);
	GecoScale3f(&afMat4x4[8], &afMat4x4[8], 1.0f / fScaleZ);

	GecoQuaternionRotationMatrixf(fpOutQuat, afMat4x4);

	fpOutVec3Scale[0] = fScaleX;
	fpOutVec3Scale[1] = fScaleY;
	fpOutVec3Scale[2] = fScaleZ;

}
//----------------------------------------------------------------------------
