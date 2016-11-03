//{future source message}
#include "FvMathPower.h"

//----------------------------------------------------------------------------
void FV_POWER_API FvZero2f(float* fpDst)
{
	FV_CASSERT(fpDst);
	FvZeroMemoryEx(fpDst, sizeof(float), 2);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvZero3f(float* fpDst)
{
	FV_CASSERT(fpDst);
	FvZeroMemoryEx(fpDst, sizeof(float), 3);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvZero4f(float* fpDst)
{
	FV_CASSERT(fpDst);
	FvZeroMemoryEx(fpDst, sizeof(float), 4);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvZero9f(float* fpDst)
{
	FV_CASSERT(fpDst);
	FvZeroMemoryEx(fpDst, sizeof(float), 9);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvZero16f(float* fpDst)
{
	FV_CASSERT(fpDst);
	FvZeroMemoryEx(fpDst, sizeof(float), 16);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSet2f(float* fpDst, const float* fpSrc)
{
	FV_CASSERT(fpDst);
	FvCrazyCopyEx(fpDst, fpSrc, sizeof(float), 2);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSet3f(float* fpDst, const float* fpSrc)
{
	FV_CASSERT(fpDst);
	FvCrazyCopyEx(fpDst, fpSrc, sizeof(float), 3);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSet4f(float* fpDst, const float* fpSrc)
{
	FV_CASSERT(fpDst);
	FvCrazyCopyEx(fpDst, fpSrc, sizeof(float), 4);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSet9f(float* fpDst, const float* fpSrc)
{
	FV_CASSERT(fpDst);
	FvCrazyCopyEx(fpDst, fpSrc, sizeof(float), 9);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSet16f(float* fpDst, const float* fpSrc)
{
	FV_CASSERT(fpDst);
	FvCrazyCopyEx(fpDst, fpSrc, sizeof(float), 16);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvAdd2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] + fpSrc1[0];
	fpDst[1] = fpSrc0[1] + fpSrc1[1];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvAdd3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] + fpSrc1[0];
	fpDst[1] = fpSrc0[1] + fpSrc1[1];
	fpDst[2] = fpSrc0[2] + fpSrc1[2];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvAdd4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] + fpSrc1[0];
	fpDst[1] = fpSrc0[1] + fpSrc1[1];
	fpDst[2] = fpSrc0[2] + fpSrc1[2];
	fpDst[3] = fpSrc0[3] + fpSrc1[3];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvAdd9f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	FvAdd3f(fpDst, fpSrc0, fpSrc1);
	FvAdd3f(fpDst + 3, fpSrc0 + 3, fpSrc1 + 3);
	FvAdd3f(fpDst + 6, fpSrc0 + 6, fpSrc1 + 6);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvAdd16f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	FvAdd4f(fpDst, fpSrc0, fpSrc1);
	FvAdd4f(fpDst + 4, fpSrc0 + 4, fpSrc1 + 4);
	FvAdd4f(fpDst + 8, fpSrc0 + 8, fpSrc1 + 8);
	FvAdd4f(fpDst + 12, fpSrc0 + 12, fpSrc1 + 12);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSub2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] - fpSrc1[0];
	fpDst[1] = fpSrc0[1] - fpSrc1[1];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSub3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] - fpSrc1[0];
	fpDst[1] = fpSrc0[1] - fpSrc1[1];
	fpDst[2] = fpSrc0[2] - fpSrc1[2];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSub4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] - fpSrc1[0];
	fpDst[1] = fpSrc0[1] - fpSrc1[1];
	fpDst[2] = fpSrc0[2] - fpSrc1[2];
	fpDst[3] = fpSrc0[3] - fpSrc1[3];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSub9f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	FvSub3f(fpDst, fpSrc0, fpSrc1);
	FvSub3f(fpDst + 3, fpSrc0 + 3, fpSrc1 + 3);
	FvSub3f(fpDst + 6, fpSrc0 + 6, fpSrc1 + 6);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvSub16f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	FvSub4f(fpDst, fpSrc0, fpSrc1);
	FvSub4f(fpDst + 4, fpSrc0 + 4, fpSrc1 + 4);
	FvSub4f(fpDst + 8, fpSrc0 + 8, fpSrc1 + 8);
	FvSub4f(fpDst + 12, fpSrc0 + 12, fpSrc1 + 12);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMul2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] * fpSrc1[0];
	fpDst[1] = fpSrc0[1] * fpSrc1[1];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMul3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] * fpSrc1[0];
	fpDst[1] = fpSrc0[1] * fpSrc1[1];
	fpDst[2] = fpSrc0[2] * fpSrc1[2];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMul4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] * fpSrc1[0];
	fpDst[1] = fpSrc0[1] * fpSrc1[1];
	fpDst[2] = fpSrc0[2] * fpSrc1[2];
	fpDst[3] = fpSrc0[3] * fpSrc1[3];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMul9f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	FvMul3f(fpDst, fpSrc0, fpSrc1);
	FvMul3f(fpDst + 3, fpSrc0 + 3, fpSrc1 + 3);
	FvMul3f(fpDst + 6, fpSrc0 + 6, fpSrc1 + 6);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMul16f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	FvMul4f(fpDst, fpSrc0, fpSrc1);
	FvMul4f(fpDst + 4, fpSrc0 + 4, fpSrc1 + 4);
	FvMul4f(fpDst + 8, fpSrc0 + 8, fpSrc1 + 8);
	FvMul4f(fpDst + 12, fpSrc0 + 12, fpSrc1 + 12);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvDiv2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] / fpSrc1[0];
	fpDst[1] = fpSrc0[1] / fpSrc1[1];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvDiv3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] / fpSrc1[0];
	fpDst[1] = fpSrc0[1] / fpSrc1[1];
	fpDst[2] = fpSrc0[2] / fpSrc1[2];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvDiv4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc0[0] / fpSrc1[0];
	fpDst[1] = fpSrc0[1] / fpSrc1[1];
	fpDst[2] = fpSrc0[2] / fpSrc1[2];
	fpDst[3] = fpSrc0[3] / fpSrc1[3];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvDiv9f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	FvDiv3f(fpDst, fpSrc0, fpSrc1);
	FvDiv3f(fpDst + 3, fpSrc0 + 3, fpSrc1 + 3);
	FvDiv3f(fpDst + 6, fpSrc0 + 6, fpSrc1 + 6);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvDiv16f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	FvDiv4f(fpDst, fpSrc0, fpSrc1);
	FvDiv4f(fpDst + 4, fpSrc0 + 4, fpSrc1 + 4);
	FvDiv4f(fpDst + 8, fpSrc0 + 8, fpSrc1 + 8);
	FvDiv4f(fpDst + 12, fpSrc0 + 12, fpSrc1 + 12);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvScale2f(float* fpDst, const float* fpSrc, float fScale)
{
	FV_CASSERT(fpSrc);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc[0] * fScale;
	fpDst[1] = fpSrc[1] * fScale;
}
//----------------------------------------------------------------------------
void FV_POWER_API FvScale3f(float* fpDst, const float* fpSrc, float fScale)
{
	FV_CASSERT(fpSrc);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc[0] * fScale;
	fpDst[1] = fpSrc[1] * fScale;
	fpDst[2] = fpSrc[2] * fScale;
}
//----------------------------------------------------------------------------
void FV_POWER_API FvScale4f(float* fpDst, const float* fpSrc, float fScale)
{
	FV_CASSERT(fpSrc);
	FV_CASSERT(fpDst);

	fpDst[0] = fpSrc[0] * fScale;
	fpDst[1] = fpSrc[1] * fScale;
	fpDst[2] = fpSrc[2] * fScale;
	fpDst[3] = fpSrc[3] * fScale;
}
//----------------------------------------------------------------------------
void FV_POWER_API FvScale9f(float* fpDst, const float* fpSrc, float fScale)
{
	FV_CASSERT(fpSrc);
	FV_CASSERT(fpDst);

	FvScale3f(fpDst, fpSrc, fScale);
	FvScale3f(fpDst + 3, fpSrc + 3, fScale);
	FvScale3f(fpDst + 6, fpSrc + 6, fScale);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvScale16f(float* fpDst, const float* fpSrc, float fScale)
{
	FV_CASSERT(fpSrc);
	FV_CASSERT(fpDst);

	FvScale4f(fpDst, fpSrc, fScale);
	FvScale4f(fpDst + 4, fpSrc + 4, fScale);
	FvScale4f(fpDst + 8, fpSrc + 8, fScale);
	FvScale4f(fpDst + 12, fpSrc + 12, fScale);
}
//----------------------------------------------------------------------------
float FV_POWER_API FvLengthSqr2f(const float* fpSrc)
{
	FV_CASSERT(fpSrc);

	return FvDot2f(fpSrc, fpSrc);
}
//----------------------------------------------------------------------------
float FV_POWER_API FvLengthSqr3f(const float* fpSrc)
{
	FV_CASSERT(fpSrc);

	return FvDot3f(fpSrc, fpSrc);
}
//----------------------------------------------------------------------------
float FV_POWER_API FvLengthSqr4f(const float* fpSrc)
{
	FV_CASSERT(fpSrc);

	return FvDot4f(fpSrc, fpSrc);
}
//----------------------------------------------------------------------------
float FV_POWER_API FvLength2f(const float* fpSrc)
{
	FV_CASSERT(fpSrc);

	return FvSqrtf(FvLengthSqr2f(fpSrc));
}
//----------------------------------------------------------------------------
float FV_POWER_API FvLength3f(const float* fpSrc)
{
	FV_CASSERT(fpSrc);

	return FvSqrtf(FvLengthSqr3f(fpSrc));
}
//----------------------------------------------------------------------------
float FV_POWER_API FvLength4f(const float* fpSrc)
{
	FV_CASSERT(fpSrc);

	return FvSqrtf(FvLengthSqr4f(fpSrc));
}
//----------------------------------------------------------------------------
void FV_POWER_API FvNormalize2f(float* fpDst, const float* fpSrc)
{
	FV_CASSERT(fpSrc);
	FV_CASSERT(fpDst);

	FvScale2f(fpDst, fpSrc, FvInvSqrtf(FvLengthSqr2f(fpSrc)));
}
//----------------------------------------------------------------------------
void FV_POWER_API FvNormalize3f(float* fpDst, const float* fpSrc)
{
	FV_CASSERT(fpSrc);
	FV_CASSERT(fpDst);

	FvScale3f(fpDst, fpSrc, FvInvSqrtf(FvLengthSqr3f(fpSrc)));
}
//----------------------------------------------------------------------------
void FV_POWER_API FvNormalize4f(float* fpDst, const float* fpSrc)
{
	FV_CASSERT(fpSrc);
	FV_CASSERT(fpDst);

	FvScale4f(fpDst, fpSrc, FvInvSqrtf(FvLengthSqr4f(fpSrc)));
}
//----------------------------------------------------------------------------
float FV_POWER_API FvDot2f(const float* fpSrc0, const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);

	return fpSrc0[0] * fpSrc1[0] + fpSrc0[1] * fpSrc1[1];
}
//----------------------------------------------------------------------------
float FV_POWER_API FvDot3f(const float* fpSrc0, const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);

	return fpSrc0[0] * fpSrc1[0] + fpSrc0[1] * fpSrc1[1]
	+ fpSrc0[2] * fpSrc1[2];
}
//----------------------------------------------------------------------------
float FV_POWER_API FvDot4f(const float* fpSrc0, const float* fpSrc1)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);

	return fpSrc0[0] * fpSrc1[0] + fpSrc0[1] * fpSrc1[1]
	+ fpSrc0[2] * fpSrc1[2] + fpSrc0[3] * fpSrc1[3];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvLerp2f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1, float t)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = FV_LERP(t, fpSrc0[0], fpSrc1[0]);
	fpDst[1] = FV_LERP(t, fpSrc0[1], fpSrc1[1]);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvLerp3f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1, float t)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = FV_LERP(t, fpSrc0[0], fpSrc1[0]);
	fpDst[1] = FV_LERP(t, fpSrc0[1], fpSrc1[1]);
	fpDst[2] = FV_LERP(t, fpSrc0[2], fpSrc1[2]);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvLerp4f(float* fpDst, const float* fpSrc0,
	const float* fpSrc1, float t)
{
	FV_CASSERT(fpSrc1);
	FV_CASSERT(fpSrc0);
	FV_CASSERT(fpDst);

	fpDst[0] = FV_LERP(t, fpSrc0[0], fpSrc1[0]);
	fpDst[1] = FV_LERP(t, fpSrc0[1], fpSrc1[1]);
	fpDst[2] = FV_LERP(t, fpSrc0[2], fpSrc1[2]);
	fpDst[3] = FV_LERP(t, fpSrc0[3], fpSrc1[3]);
}
//----------------------------------------------------------------------------
float FV_POWER_API FvVector2Crossf(const float* fpVec0, const float* fpVec1)
{
	return fpVec0[0] * fpVec1[1] - fpVec0[1] * fpVec1[0];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvVector3Crossf(float* fpOut, const float* fpVec0,
	const float* fpVec1)
{
	float afVec0[3];
	float afVec1[3];

	FV_CASSERT(fpVec1);
	FV_CASSERT(fpVec0);
	FV_CASSERT(fpOut);

	FvSet3f(afVec0, fpVec0);
	FvSet3f(afVec1, fpVec1);

	fpOut[0] = (afVec0[1] * afVec1[2]) - (afVec0[2] * afVec1[1]);
	fpOut[1] = (afVec0[2] * afVec1[0]) - (afVec0[0] * afVec1[2]);
	fpOut[2] = (afVec0[0] * afVec1[1]) - (afVec0[1] * afVec1[0]);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvVector3TransformM4f(float* fpOutVec4, const float* fpVec3,
	const float* fpMat4x4)
{
	float afVec3[3];
	float afMat4x4[16];

	FV_CASSERT(fpMat4x4);
	FV_CASSERT(fpVec3);
	FV_CASSERT(fpOutVec4);

	FvSet3f(afVec3, fpVec3);
	FvSet16f(afMat4x4, fpMat4x4);

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
void FV_POWER_API FvVector3TransformCoordM4f(float* fpOutVec3,
	const float* fpVec3, const float* fpMat4x4)
{
	float afVec3[3];
	float afMat4x4[16];
	float fInvw;
	
	FV_CASSERT(fpMat4x4);
	FV_CASSERT(fpVec3);
	FV_CASSERT(fpOutVec3);

	FvSet3f(afVec3, fpVec3);
	FvSet16f(afMat4x4, fpMat4x4);

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
void FV_POWER_API FvVector3TransformNormalM4f(float* fpOutVec3,
	const float* fpVec3, const float* fpMat4x4)
{
	float afVec3[3];
	float afMat4x4[16];

	FV_CASSERT(fpMat4x4);
	FV_CASSERT(fpVec3);
	FV_CASSERT(fpOutVec3);

	FvSet3f(afVec3, fpVec3);
	FvSet16f(afMat4x4, fpMat4x4);

	fpOutVec3[0] = afVec3[0] * afMat4x4[0] + afVec3[1] * afMat4x4[4] +
		afVec3[2] * afMat4x4[8];
	fpOutVec3[1] = afVec3[0] * afMat4x4[1] + afVec3[1] * afMat4x4[5] +
		afVec3[2] * afMat4x4[9];
	fpOutVec3[2] = afVec3[0] * afMat4x4[2] + afVec3[1] * afMat4x4[6] +
		afVec3[2] * afMat4x4[10];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvVector3SetPitchYawYUpf(float* fpOutVec3, float fPitchInRadians,
	float fYawInRadians)
{
	float fCosPitch = FvCosf(fPitchInRadians);
	float fSinPitch = FvSinf(-fPitchInRadians);

	float fCosYaw = FvCosf(fYawInRadians);
	float fSinYaw = FvSinf(fYawInRadians);

	fpOutVec3[0] = fCosPitch * fSinYaw;
	fpOutVec3[1] = fSinPitch;
	fpOutVec3[2] = fCosPitch * fCosYaw;
}
//----------------------------------------------------------------------------
float FV_POWER_API FvVector3GetPitchYUpf(float* fpVec3)
{
	return -FvAtan2f(fpVec3[1], FvSqrtf(fpVec3[0] * fpVec3[0] +
		fpVec3[2] * fpVec3[2]));
}
//----------------------------------------------------------------------------
float FV_POWER_API FvVector3GetYawYUpf(float* fpVec3)
{
	return FvAtan2f(fpVec3[0], fpVec3[2]);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvVector3SetPitchYawZUpf(float* fpOutVec3, float fPitchInRadians,
	float fYawInRadians)
{
	float fCosPitch = FvCosf(fPitchInRadians);
	float fSinPitch = FvSinf(-fPitchInRadians);

	float fCosYaw = FvCosf(fYawInRadians);
	float fSinYaw = FvSinf(fYawInRadians);

	fpOutVec3[0] = fCosPitch * fSinYaw;
	fpOutVec3[1] = fCosPitch * fCosYaw;
	fpOutVec3[2] = fSinPitch;
}
//----------------------------------------------------------------------------
float FV_POWER_API FvVector3GetPitchZUpf(float* fpVec3)
{
	return -FvAtan2f(fpVec3[2], FvSqrtf(fpVec3[0] * fpVec3[0] +
		fpVec3[1] * fpVec3[1]));
}
//----------------------------------------------------------------------------
float FV_POWER_API FvVector3GetYawZUpf(float* fpVec3)
{
	return FvAtan2f(fpVec3[0], fpVec3[1]);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvVector4Transformf(float* fpOutVec4, const float* fpVec4,
	const float* fpMat4x4)
{
	float afVec4[4];
	float afMat4x4[16];

	FV_CASSERT(fpMat4x4);
	FV_CASSERT(fpVec4);
	FV_CASSERT(fpOutVec4);

	FvSet4f(afVec4, fpVec4);
	FvSet16f(afMat4x4, fpMat4x4);

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
FV_OUT_CODE FV_POWER_API FvVector4Outcode(const float* fpVec4)
{
	FV_OUT_CODE eCode =0;

	if(fpVec4[0] < (-fpVec4[3]))
		eCode |= FV_OUTCODE_LEFT;

	if(fpVec4[0] > fpVec4[3])
		eCode |= FV_OUTCODE_RIGHT;

	if(fpVec4[1] < (-fpVec4[3]))
		eCode |= FV_OUTCODE_BOTTOM;

	if(fpVec4[1] > fpVec4[3])
		eCode |= FV_OUTCODE_TOP;

	if(fpVec4[2] < 0)
		eCode |= FV_OUTCODE_NEAR;

	if(fpVec4[2] > fpVec4[3])
		eCode |= FV_OUTCODE_FAR;

	return eCode;
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMatrixIdenty4f(float* fpOutMat4x4)
{
	FV_CASSERT(fpOutMat4x4);

	fpOutMat4x4[1] = fpOutMat4x4[2] =fpOutMat4x4[3] =
		fpOutMat4x4[4] = fpOutMat4x4[6] = fpOutMat4x4[7] =
		fpOutMat4x4[8] = fpOutMat4x4[9] = fpOutMat4x4[11] =
		fpOutMat4x4[12] = fpOutMat4x4[13] = fpOutMat4x4[14] = 0;
	fpOutMat4x4[0] = fpOutMat4x4[5] = fpOutMat4x4[10] =
		fpOutMat4x4[15] = 1.0f;
}
//----------------------------------------------------------------------------
FV_BOOL FV_POWER_API FvMatrixInverse4f(float* fpOutMat4x4, float* fpDeterminant,
	const float* fpMat4x4)
{
	float afMat4x4[16];
	float v0, v1, v2, v3, v4, v5;
	float t00, t10, t20, t30;
	float fDet, fInvDet;

	FV_CASSERT(fpMat4x4);
	FV_CASSERT(fpOutMat4x4);

	FvSet16f(afMat4x4, fpMat4x4);

	v0 = afMat4x4[8] * afMat4x4[13] - afMat4x4[9] * afMat4x4[12];
	v1 = afMat4x4[8] * afMat4x4[14] - afMat4x4[10] * afMat4x4[12];
	v2 = afMat4x4[8] * afMat4x4[15] - afMat4x4[11] * afMat4x4[12];
	v3 = afMat4x4[9] * afMat4x4[14] - afMat4x4[10] * afMat4x4[13];
	v4 = afMat4x4[9] * afMat4x4[15] - afMat4x4[11] * afMat4x4[13];
	v5 = afMat4x4[10] * afMat4x4[15] - afMat4x4[11] * afMat4x4[14];

	t00 = + (v5 * afMat4x4[5] - v4 * afMat4x4[6] + v3 * afMat4x4[7]);
	t10 = - (v5 * afMat4x4[4] - v2 * afMat4x4[6] + v1 * afMat4x4[7]);
	t20 = + (v4 * afMat4x4[4] - v2 * afMat4x4[5] + v0 * afMat4x4[7]);
	t30 = - (v3 * afMat4x4[4] - v1 * afMat4x4[5] + v0 * afMat4x4[6]);

	fDet = (t00 * afMat4x4[0] + t10 * afMat4x4[1] + t20 * afMat4x4[2] +
		t30 * afMat4x4[3]);

	if(fpDeterminant)
		*fpDeterminant = fDet;

	if(fDet == 0) return FV_FALSE;

	fInvDet = 1.0f / fDet;

	fpOutMat4x4[0] = t00 * fInvDet;
	fpOutMat4x4[4] = t10 * fInvDet;
	fpOutMat4x4[8] = t20 * fInvDet;
	fpOutMat4x4[12] = t30 * fInvDet;

	fpOutMat4x4[1] = - (v5 * afMat4x4[1] - v4 * afMat4x4[2] + v3 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[5] = + (v5 * afMat4x4[0] - v2 * afMat4x4[2] + v1 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[9] = - (v4 * afMat4x4[0] - v2 * afMat4x4[1] + v0 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[13] = + (v3 * afMat4x4[0] - v1 * afMat4x4[1] + v0 *
		afMat4x4[2]) * fInvDet;

	v0 = afMat4x4[4] * afMat4x4[13] - afMat4x4[5] * afMat4x4[12];
	v1 = afMat4x4[4] * afMat4x4[14] - afMat4x4[6] * afMat4x4[12];
	v2 = afMat4x4[4] * afMat4x4[15] - afMat4x4[7] * afMat4x4[12];
	v3 = afMat4x4[5] * afMat4x4[14] - afMat4x4[6] * afMat4x4[13];
	v4 = afMat4x4[5] * afMat4x4[15] - afMat4x4[7] * afMat4x4[13];
	v5 = afMat4x4[6] * afMat4x4[15] - afMat4x4[7] * afMat4x4[14];

	fpOutMat4x4[2] = + (v5 * afMat4x4[1] - v4 * afMat4x4[2] + v3 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[6] = - (v5 * afMat4x4[0] - v2 * afMat4x4[2] + v1 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[10] = + (v4 * afMat4x4[0] - v2 * afMat4x4[1] + v0 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[14] = - (v3 * afMat4x4[0] - v1 * afMat4x4[1] + v0 *
		afMat4x4[2]) * fInvDet;

	v0 = afMat4x4[9] * afMat4x4[4] - afMat4x4[8] * afMat4x4[5];
	v1 = afMat4x4[10] * afMat4x4[4] - afMat4x4[8] * afMat4x4[6];
	v2 = afMat4x4[11] * afMat4x4[4] - afMat4x4[8] * afMat4x4[7];
	v3 = afMat4x4[10] * afMat4x4[5] - afMat4x4[9] * afMat4x4[6];
	v4 = afMat4x4[11] * afMat4x4[5] - afMat4x4[9] * afMat4x4[7];
	v5 = afMat4x4[11] * afMat4x4[6] - afMat4x4[10] * afMat4x4[7];

	fpOutMat4x4[3] = - (v5 * afMat4x4[1] - v4 * afMat4x4[2] + v3 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[7] = + (v5 * afMat4x4[0] - v2 * afMat4x4[2] + v1 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[11] = - (v4 * afMat4x4[0] - v2 * afMat4x4[1] + v0 *
		afMat4x4[3]) * fInvDet;
	fpOutMat4x4[15] = + (v3 * afMat4x4[0] - v1 * afMat4x4[1] + v0 *
		afMat4x4[2]) * fInvDet;

	return FV_TRUE;
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMatrixTranspose4f(float* fpOutMat4x4,
	const float* fpMat4x4)
{
	float afMat4x4[16];

	FV_CASSERT(fpMat4x4);
	FV_CASSERT(fpOutMat4x4);

	FvSet16f(afMat4x4, fpMat4x4);

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
float FV_POWER_API FvMatrixDeterminant4f(const float* fpMat4x4)
{
	float v0, v1, v2, v3, v4, v5;
	float t00, t10, t20, t30;

	FV_CASSERT(fpMat4x4);

	v0 = fpMat4x4[8] * fpMat4x4[13] - fpMat4x4[9] * fpMat4x4[12];
	v1 = fpMat4x4[8] * fpMat4x4[14] - fpMat4x4[10] * fpMat4x4[12];
	v2 = fpMat4x4[8] * fpMat4x4[15] - fpMat4x4[11] * fpMat4x4[12];
	v3 = fpMat4x4[9] * fpMat4x4[14] - fpMat4x4[10] * fpMat4x4[13];
	v4 = fpMat4x4[9] * fpMat4x4[15] - fpMat4x4[11] * fpMat4x4[13];
	v5 = fpMat4x4[10] * fpMat4x4[15] - fpMat4x4[11] * fpMat4x4[14];

	t00 = + (v5 * fpMat4x4[5] - v4 * fpMat4x4[6] + v3 * fpMat4x4[7]);
	t10 = - (v5 * fpMat4x4[4] - v2 * fpMat4x4[6] + v1 * fpMat4x4[7]);
	t20 = + (v4 * fpMat4x4[4] - v2 * fpMat4x4[5] + v0 * fpMat4x4[7]);
	t30 = - (v3 * fpMat4x4[4] - v1 * fpMat4x4[5] + v0 * fpMat4x4[6]);

	return (t00 * fpMat4x4[0] + t10 * fpMat4x4[1] + t20 * fpMat4x4[2] +
		t30 * fpMat4x4[3]);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMatrixMultiply4f(float* fpOutMat4x4,
	const float* fpMat4x4Src0, const float* fpMat4x4Src1)
{
	float afMat4x4Src0[16];
	float afMat4x4Src1[16];

	FV_CASSERT(fpMat4x4Src1);
	FV_CASSERT(fpMat4x4Src0);
	FV_CASSERT(fpOutMat4x4);

	FvSet16f(afMat4x4Src0, fpMat4x4Src0);
	FvSet16f(afMat4x4Src1, fpMat4x4Src1);

	fpOutMat4x4[0] = afMat4x4Src0[0] * afMat4x4Src1[0] + afMat4x4Src0[1] *
		afMat4x4Src1[4] + afMat4x4Src0[2] * afMat4x4Src1[8] +
		afMat4x4Src0[3] * afMat4x4Src1[12];
	fpOutMat4x4[1] = afMat4x4Src0[0] * afMat4x4Src1[1] + afMat4x4Src0[1] *
		afMat4x4Src1[5]	+ afMat4x4Src0[2] * afMat4x4Src1[9] +
		afMat4x4Src0[3] * afMat4x4Src1[13];
	fpOutMat4x4[2] = afMat4x4Src0[0] * afMat4x4Src1[2] + afMat4x4Src0[1] *
		afMat4x4Src1[6] + afMat4x4Src0[2] * afMat4x4Src1[10] +
		afMat4x4Src0[3] * afMat4x4Src1[14];
	fpOutMat4x4[3] = afMat4x4Src0[0] * afMat4x4Src1[3] + afMat4x4Src0[1] *
		afMat4x4Src1[7]	+ afMat4x4Src0[2] * afMat4x4Src1[11] +
		afMat4x4Src0[3] * afMat4x4Src1[15];

	fpOutMat4x4[4] = afMat4x4Src0[4] * afMat4x4Src1[0] + afMat4x4Src0[5] *
		afMat4x4Src1[4]	+ afMat4x4Src0[6] * afMat4x4Src1[8] +
		afMat4x4Src0[7] * afMat4x4Src1[12];
	fpOutMat4x4[5] = afMat4x4Src0[4] * afMat4x4Src1[1] + afMat4x4Src0[5] *
		afMat4x4Src1[5]	+ afMat4x4Src0[6] * afMat4x4Src1[9] +
		afMat4x4Src0[7] * afMat4x4Src1[13];
	fpOutMat4x4[6] = afMat4x4Src0[4] * afMat4x4Src1[2] + afMat4x4Src0[5] *
		afMat4x4Src1[6]	+ afMat4x4Src0[6] * afMat4x4Src1[10] +
		afMat4x4Src0[7] * afMat4x4Src1[14];
	fpOutMat4x4[7] = afMat4x4Src0[4] * afMat4x4Src1[3] + afMat4x4Src0[5] *
		afMat4x4Src1[7]	+ afMat4x4Src0[6] * afMat4x4Src1[11] +
		afMat4x4Src0[7] * afMat4x4Src1[15];

	fpOutMat4x4[8] = afMat4x4Src0[8] * afMat4x4Src1[0] + afMat4x4Src0[9] *
		afMat4x4Src1[4]	+ afMat4x4Src0[10] * afMat4x4Src1[8] +
		afMat4x4Src0[11] * afMat4x4Src1[12];
	fpOutMat4x4[9] = afMat4x4Src0[8] * afMat4x4Src1[1] + afMat4x4Src0[9] *
		afMat4x4Src1[5]	+ afMat4x4Src0[10] * afMat4x4Src1[9] +
		afMat4x4Src0[11] * afMat4x4Src1[13];
	fpOutMat4x4[10] = afMat4x4Src0[8] * afMat4x4Src1[2] + afMat4x4Src0[9] *
		afMat4x4Src1[6]	+ afMat4x4Src0[10] * afMat4x4Src1[10] +
		afMat4x4Src0[11] * afMat4x4Src1[14];
	fpOutMat4x4[11] = afMat4x4Src0[8] * afMat4x4Src1[3] + afMat4x4Src0[9] *
		afMat4x4Src1[7]	+ afMat4x4Src0[10] * afMat4x4Src1[11] +
		afMat4x4Src0[11] * afMat4x4Src1[15];

	fpOutMat4x4[12] = afMat4x4Src0[12] * afMat4x4Src1[0] + afMat4x4Src0[13] *
		afMat4x4Src1[4]	+ afMat4x4Src0[14] * afMat4x4Src1[8] +
		afMat4x4Src0[15] * afMat4x4Src1[12];
	fpOutMat4x4[13] = afMat4x4Src0[12] * afMat4x4Src1[1] + afMat4x4Src0[13] *
		afMat4x4Src1[5]	+ afMat4x4Src0[14] * afMat4x4Src1[9] +
		afMat4x4Src0[15] * afMat4x4Src1[13];
	fpOutMat4x4[14] = afMat4x4Src0[12] * afMat4x4Src1[2] + afMat4x4Src0[13] *
		afMat4x4Src1[6]	+ afMat4x4Src0[14] * afMat4x4Src1[10] +
		afMat4x4Src0[15] * afMat4x4Src1[14];
	fpOutMat4x4[15] = afMat4x4Src0[12] * afMat4x4Src1[3] + afMat4x4Src0[13] *
		afMat4x4Src1[7]	+ afMat4x4Src0[14] * afMat4x4Src1[11] +
		afMat4x4Src0[15] * afMat4x4Src1[15];
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMatrixRotationX4f(float* fpOutMat4x4, float fAngle)
{
	float fSin, fCos;
	
	FV_CASSERT(fpOutMat4x4);

	FvSinCosf(&fSin, &fCos, fAngle);
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
void FV_POWER_API FvMatrixRotationY4f(float* fpOutMat4x4, float fAngle)
{
	float fSin, fCos;

	FV_CASSERT(fpOutMat4x4);

	FvSinCosf(&fSin, &fCos, fAngle);
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
void FV_POWER_API FvMatrixRotationZ4f(float* fpOutMat4x4, float fAngle)
{
	float fSin, fCos;

	FV_CASSERT(fpOutMat4x4);

	FvSinCosf(&fSin, &fCos, fAngle);
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
void FV_POWER_API FvMatrixScaling4f(float* fpOutMat4x4, float fScaleX,
	float fScaleY, float fScaleZ)
{
	FV_CASSERT(fpOutMat4x4);

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
void FV_POWER_API FvMatrixTranslation4f(float* fpOutMat4x4, float x, float y,
	float z)
{
	FV_CASSERT(fpOutMat4x4);

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
void FV_POWER_API FvMatrixRotationYawPitchRoll4f(float* fpOutMat4x4,
	float fYaw, float fPitch, float fRoll)
{
	float afMat4x4Temp[16];

	FV_CASSERT(fpOutMat4x4);
	
	FvMatrixRotationZ4f(fpOutMat4x4, fRoll);
	FvMatrixRotationX4f(afMat4x4Temp, fPitch);
	FvMatrixMultiply4f(fpOutMat4x4, fpOutMat4x4, afMat4x4Temp);
	FvMatrixRotationY4f(afMat4x4Temp, fYaw);
	FvMatrixMultiply4f(fpOutMat4x4, fpOutMat4x4, afMat4x4Temp);
}
//----------------------------------------------------------------------------
void FV_POWER_API FvMatrixRotationQuaternion4f(float* fpOutMat4x4,
	const float* fpQuat)
{
	const float fTx  = 2.0f * fpQuat[0];
	const float fTy  = 2.0f * fpQuat[1];
	const float fTz  = 2.0f * fpQuat[2];
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
void FV_POWER_API FvQuaternionRotationMatrixf(float* fpOutQuat,
	const float* fpMat4x4)
{
	static unsigned int s_uiNext[3] = {1, 2, 0};
	float fTrace = fpMat4x4[0] + fpMat4x4[5] + fpMat4x4[10];
	float fRoot;
	unsigned int i,j,k;

	if (fTrace > 0.0f)
	{
		fRoot = FvSqrtf(fTrace + 1.0f);
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

		fRoot = FvSqrtf(fpMat4x4[i*4+i] - fpMat4x4[j*4+j] -
			fpMat4x4[k*4+k] + 1.0f);
		fpOutQuat[i] = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;
		fpOutQuat[3] = (fpMat4x4[j*4+k] - fpMat4x4[k*4+j]) * fRoot;
		fpOutQuat[j] = (fpMat4x4[i*4+j] + fpMat4x4[j*4+i]) * fRoot;
		fpOutQuat[k] = (fpMat4x4[i*4+k] + fpMat4x4[k*4+i]) * fRoot;
	}
}
//----------------------------------------------------------------------------
void FV_POWER_API FvQuaternionRotationAxisf(float* fpOutQuat,
	const float* fpVec3, float fAngle)
{
	float fSin, fCos;
	FvSinCosf(&fSin, &fCos, 0.5f * fAngle);
	
	fpOutQuat[0] = fSin * fpVec3[0];
	fpOutQuat[1] = fSin * fpVec3[1];
	fpOutQuat[2] = fSin * fpVec3[2];
	fpOutQuat[3] = fCos;
}
//----------------------------------------------------------------------------
void FV_POWER_API FvQuaternionMultiplyf(float* fpOutQuat, const float* fpQuat0,
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
void FV_POWER_API FvQuaternionInversef(float* fpOutQuat, const float* fpQuat)
{
	float fNorm = FvLengthSqr4f(fpQuat);
	if(fNorm > 0)
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
void FV_POWER_API FvMatrixDecomposeWorld4f(float* fpOutQuat,
	float* fpOutVec3Scale, float* fpOutVec3Trans, const float* fpMat4x4)
{
	float afMat4x4[16];
	float fScaleX;
	float fScaleY;
	float fScaleZ;

	FV_CASSERT(fpMat4x4[3] == 0.0f && fpMat4x4[7] == 0.0f &&
		fpMat4x4[11] == 0.0f && fpMat4x4[15] == 1.0f);

	FV_CASSERT(fpMat4x4);
	FV_CASSERT(fpOutVec3Trans);
	FV_CASSERT(fpOutVec3Scale);
	FV_CASSERT(fpOutQuat);

	FvSet16f(afMat4x4, fpMat4x4);

	fpOutVec3Trans[0] = afMat4x4[12];
	fpOutVec3Trans[1] = afMat4x4[13];
	fpOutVec3Trans[2] = afMat4x4[14];

	fScaleX = FvLength3f(&afMat4x4[0]);
	fScaleY = FvLength3f(&afMat4x4[4]);
	fScaleZ = FvLength3f(&afMat4x4[8]);

	FvScale3f(&afMat4x4[0], &afMat4x4[0], 1.0f / fScaleX);
	FvScale3f(&afMat4x4[4], &afMat4x4[4], 1.0f / fScaleY);
	FvScale3f(&afMat4x4[8], &afMat4x4[8], 1.0f / fScaleZ);

	FvQuaternionRotationMatrixf(fpOutQuat, afMat4x4);

	fpOutVec3Scale[0] = fScaleX;
	fpOutVec3Scale[1] = fScaleY;
	fpOutVec3Scale[2] = fScaleZ;

}
//----------------------------------------------------------------------------
