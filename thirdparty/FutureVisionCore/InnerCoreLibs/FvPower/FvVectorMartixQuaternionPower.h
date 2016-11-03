//{future header message}
#ifndef __FvVectorMartixQuaternionPower_H__
#define __FvVectorMartixQuaternionPower_H__

#include "FvPowerDefines.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	void FV_POWER_API FvZero2f(float* fpDst);
	void FV_POWER_API FvZero3f(float* fpDst);
	void FV_POWER_API FvZero4f(float* fpDst);
	void FV_POWER_API FvZero9f(float* fpDst);
	void FV_POWER_API FvZero16f(float* fpDst);

	void FV_POWER_API FvSet2f(float* fpDst, const float* fpSrc);
	void FV_POWER_API FvSet3f(float* fpDst, const float* fpSrc);
	void FV_POWER_API FvSet4f(float* fpDst, const float* fpSrc);
	void FV_POWER_API FvSet9f(float* fpDst, const float* fpSrc);
	void FV_POWER_API FvSet16f(float* fpDst, const float* fpSrc);

	void FV_POWER_API FvAdd2f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvAdd3f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvAdd4f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvAdd9f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvAdd16f(float* fpDst, const float* fpSrc0, const float* fpSrc1);

	void FV_POWER_API FvSub2f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvSub3f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvSub4f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvSub9f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvSub16f(float* fpDst, const float* fpSrc0, const float* fpSrc1);

	void FV_POWER_API FvMul2f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvMul3f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvMul4f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvMul9f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvMul16f(float* fpDst, const float* fpSrc0, const float* fpSrc1);

	void FV_POWER_API FvDiv2f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvDiv3f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvDiv4f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvDiv9f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void FV_POWER_API FvDiv16f(float* fpDst, const float* fpSrc0, const float* fpSrc1);

	void FV_POWER_API FvScale2f(float* fpDst, const float* fpSrc, float fScale);
	void FV_POWER_API FvScale3f(float* fpDst, const float* fpSrc, float fScale);
	void FV_POWER_API FvScale4f(float* fpDst, const float* fpSrc, float fScale);
	void FV_POWER_API FvScale9f(float* fpDst, const float* fpSrc, float fScale);
	void FV_POWER_API FvScale16f(float* fpDst, const float* fpSrc, float fScale);

	float FV_POWER_API FvLengthSqr2f(const float* fpSrc);
	float FV_POWER_API FvLengthSqr3f(const float* fpSrc);
	float FV_POWER_API FvLengthSqr4f(const float* fpSrc);

	float FV_POWER_API FvLength2f(const float* fpSrc);
	float FV_POWER_API FvLength3f(const float* fpSrc);
	float FV_POWER_API FvLength4f(const float* fpSrc);

	void FV_POWER_API FvNormalize2f(float* fpDst, const float* fpSrc);
	void FV_POWER_API FvNormalize3f(float* fpDst, const float* fpSrc);
	void FV_POWER_API FvNormalize4f(float* fpDst, const float* fpSrc);

	float FV_POWER_API FvDot2f(const float* fpSrc0, const float* fpSrc1);
	float FV_POWER_API FvDot3f(const float* fpSrc0, const float* fpSrc1);
	float FV_POWER_API FvDot4f(const float* fpSrc0, const float* fpSrc1);

	void FV_POWER_API FvLerp2f(float* fpDst, const float* fpSrc0, const float* fpSrc1, float t);
	void FV_POWER_API FvLerp3f(float* fpDst, const float* fpSrc0, const float* fpSrc1, float t);
	void FV_POWER_API FvLerp4f(float* fpDst, const float* fpSrc0, const float* fpSrc1, float t);

	float FV_POWER_API FvVector2Crossf(const float* fpVec0, const float* fpVec1);
	void FV_POWER_API FvVector3Crossf(float* fpOut, const float* fpVec0, const float* fpVec1);

	void FV_POWER_API FvVector3TransformM4f(float* fpOutVec4, const float* fpVec3, const float* fpMat4x4);
	void FV_POWER_API FvVector3TransformCoordM4f(float* fpOutVec3, const float* fpVec3, const float* fpMat4x4);
	void FV_POWER_API FvVector3TransformNormalM4f(float* fpOutVec3, const float* fpVec3, const float* fpMat4x4);
	void FV_POWER_API FvVector3TransformNormalM4f(float* fpOutVec3, const float* fpVec3, const float* fpMat4x4);

	void FV_POWER_API FvVector3SetPitchYawYUpf(float* fpOutVec3, float fPitchInRadians, float fYawInRadians);
	float FV_POWER_API FvVector3GetPitchYUpf(float* fpVec3);
	float FV_POWER_API FvVector3GetYawYUpf(float* fpVec3);

	void FV_POWER_API FvVector3SetPitchYawZUpf(float* fpOutVec3, float fPitchInRadians, float fYawInRadians);
	float FV_POWER_API FvVector3GetPitchZUpf(float* fpVec3);
	float FV_POWER_API FvVector3GetYawZUpf(float* fpVec3);

	void FV_POWER_API FvVector4Transformf(float* fpOutVec4, const float* fpVec4, const float* fpMat4x4);
	FV_OUT_CODE FV_POWER_API FvVector4Outcode(const float* fpVec4);

	void FV_POWER_API FvMatrixIdenty4f(float* fpOutMat4x4);
	FV_BOOL FV_POWER_API FvMatrixInverse4f(float* fpOutMat4x4, float* fpDeterminant, const float* fpMat4x4);
	void FV_POWER_API FvMatrixTranspose4f(float* fpOutMat4x4, const float* fpMat4x4);
	float FV_POWER_API FvMatrixDeterminant4f(const float* fpMat4x4);
	void FV_POWER_API FvMatrixMultiply4f(float* fpOutMat4x4, const float* fpMat4x4Src0, const float* fpMat4x4Src1);
	void FV_POWER_API FvMatrixRotationX4f(float* fpOutMat4x4, float fAngle);
	void FV_POWER_API FvMatrixRotationY4f(float* fpOutMat4x4, float fAngle);
	void FV_POWER_API FvMatrixRotationZ4f(float* fpOutMat4x4, float fAngle);
	void FV_POWER_API FvMatrixScaling4f(float* fpOutMat4x4, float fScaleX, float fScaleY, float fScaleZ);
	void FV_POWER_API FvMatrixTranslation4f(float* fpOutMat4x4, float x, float y, float z);
	void FV_POWER_API FvMatrixRotationYawPitchRoll4f(float* fpOutMat4x4, float fYaw, float fPitch, float fRoll);
	void FV_POWER_API FvMatrixRotationQuaternion4f(float* fpOutMat4x4, const float* fpQuat);
	void FV_POWER_API FvMatrixDecomposeWorld4f(float* fpOutQuat, float* fpOutVec3Scale, float* fpOutVec3Trans,
		const float* fpMat4x4);

	void FV_POWER_API FvQuaternionRotationMatrixf(float* fpOutQuat, const float* fpMat4x4);
	void FV_POWER_API FvQuaternionRotationAxisf(float* fpOutQuat, const float* fpVec3, float fAngle);
	void FV_POWER_API FvQuaternionMultiplyf(float* fpOutQuat, const float* fpQuat0, const float* fpQuat1);
	void FV_POWER_API FvQuaternionInversef(float* fpOutQuat, const float* fpQuat);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __FvVectorMartixQuaternionPower_H__ */
