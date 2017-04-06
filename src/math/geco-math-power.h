//{future header message}
#ifndef __GecoMathPower_H__
#define __GecoMathPower_H__

#include "common/geco-plateform.h"
#include "common/ds/geco-bit-stream.h"

#define GECO_FLOAT_POINT(p) ((float*)p)
#define GECO_FLOAT_POINT_CONST(p) ((const float*)p)
#define GECO_FLOAT_POINT_THIS ((float*)this)

#define GECO_MAXFLOAT_F			(3.402823466e+38F)
#define GECO_MINFLOAT_F			(0.00000001F)
#define GECO_MATH_E_F				(2.7182818284590452354f)
#define GECO_MATH_LOG2E_F			(1.4426950408889634074f)
#define GECO_MATH_LOG10E_F		(0.43429448190325182765f)
#define GECO_MATH_LN2_F			(0.69314718055994530942f)
#define GECO_MATH_LN10_F			(2.30258509299404568402f)
#define GECO_MATH_PI_F			(3.14159265358979323846f)
#define GECO_MATH_2PI_F			(6.28318530717958647692f)
#define GECO_MATH_3PI_F			(9.42477796076937971538f)
#define GECO_MATH_PI_2_F			(1.57079632679489661923f)
#define GECO_MATH_PI_4_F			(0.78539816339744830962f)
#define GECO_MATH_3PI_4_F			(2.3561944901923448370E0f)
#define GECO_MATH_SQRTPI_F		(1.77245385090551602792981f)
#define GECO_MATH_1_PI_F			(0.31830988618379067154f)
#define GECO_MATH_2_PI_F			(0.63661977236758134308f)
#define GECO_MATH_2_SQRTPI_F		(1.12837916709551257390f)
#define GECO_MATH_SQRT2_F			(1.41421356237309504880f)
#define GECO_MATH_SQRT1_2_F		(0.70710678118654752440f)
#define GECO_MATH_LN2LO_F			(1.9082149292705877000E-10f)
#define GECO_MATH_LN2HI_F			(6.9314718036912381649E-1f)
#define GECO_MATH_SQRT3_F		   	(1.73205080756887719000f)
#define GECO_MATH_IVLN10_F		(0.43429448190325182765f)
#define GECO_MATH_LOG2_E_F		(0.693147180559945309417f)
#define GECO_MATH_INVLN2_F		(1.4426950408889633870E0f)

#define GecoAtanf atanf
#define GecoCosf cosf
#define GecoSinf sinf
#define GecoTanf tanf
#define GecoTanhf tanhf
#define GecoFrexpf frexpf
#define GecoModff modff
#define GecoCeilf ceilf
#define GecoFabsf fabsf
#define GecoFloorf floorf

#define GecoAcosf acosf
#define GecoAsinf asinf
#define GecoAtan2f atan2f
#define GecoCoshf coshf
#define GecoSinhf sinhf
#define GecoExpf expf
#define GecoLdexpf ldexpf
#define GecoLogf logf
#define GecoLog10f log10f
#define GecoPowf powf
#define GecoFmodf fmodf

#define GecoAtan atan
#define GecoCos cos
#define GecoSin sin
#define GecoTan tan
#define GecoTanh tanh
#define GecoFrexp frexp
#define GecoModf modf
#define GecoCeil ceil
#define GecoFabs fabs
#define GecoFloor floor

#define GecoAcos acos
#define GecoAsin asin
#define GecoAtan2 atan2
#define GecoCosh cosh
#define GecoSinh sinh
#define GecoExp exp
#define GecoLdexp ldexp
#define GecoLog log
#define GecoLog10 log10
#define GecoPow pow
#define GecoFmod fmod

#define GecoDegreeToRadiantf(D)	((D) * 0.01745329251994329576923690768f)
#define GecoRadiantToDegreef(R)	((R) * 57.2957795130823208767981548f)
#define GecoFracf(number)			((number)-(float)((int)(number)))

/**
*	This function returns the value clamped in between the minimum and
*	maximum values supplied.
*
*	@param minValue	The minimum the result can be.
*	@param value	The value to clamp.
*	@param maxValue	The maximum the result can be.
*
*	@return The value clamped between the minimum and maximum values.
*/
#define GecoClampEx(min,val,max)	((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))
#define GecoClamp(mag,val)		(GecoClampEx(-(mag),(val),(mag)))

#define GECO_INTERVAL_MAPPING(x,src_a,src_b,dst_a,dst_b)\
	(((dst_b) - (dst_a)) * ((x) - (src_a)) / ((src_b) - (src_a)) + dst_a)

//用C++模板函数
//#define GECO_SAFE_INTERVAL_MAPPING(x,src_a,src_b,dst_a,dst_b)\
//	(((src_a) == (src_b)) ? (dst_a) : GECO_INTERVAL_MAPPING(x,src_a,src_b,dst_a,dst_b))

#define GECO_LERP(t,a,b) ((a) + ((b) - (a)) * (t))

#define GECO_EXCHANGE_XOR(a,b)\
	(a) = (a) ^ (b);\
	(b) = (a) ^ (b);\
	(a) = (a) ^ (b)

#define GECO_EXCHANGE_CLASS(a,b,class)\
	{\
		class t((a));\
		(a) = (b);\
		(b) = t;\
	}

#define GECO_EXCHANGE_BUFFER(a,b,t)\
	t = (a);\
	(a) = (b);\
	(b) = t

#ifdef GECO_USE_FAST_SQRT
#define GecoSqrtf GecoFastSqrtf
#define GecoInvSqrtf GecoFastInvSqrtf
#else
#define GecoSqrtf sqrtf
#define GecoInvSqrtf(v) (1.0f / sqrtf((v)))
#endif

#define GECO_X_AXIS 0
#define GECO_Y_AXIS 1
#define GECO_Z_AXIS  2
#define GECO_X_COORD 0
#define GECO_Y_COORD 1
#define GECO_Z_COORD 2
typedef uint GECO_AXIS_ENUMERATION;

#define GECO_OUTCODE_NULL 0x0
#define GECO_OUTCODE_LEFT  0x1
#define GECO_OUTCODE_RIGHT  0x2
#define GECO_OUTCODE_BOTTOM  0x4
#define GECO_OUTCODE_TOP  0x8
#define GECO_OUTCODE_NEAR  0x10
#define GECO_OUTCODE_FAR  0x20
#define GECO_OUTCODE_MASK  0x3
typedef uint GECO_OUT_CODE;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	void GECOAPI GecoSinCosf(float* fpSin, float* fpCos, float fRadians);
	float GECOAPI GecoRoundf(float fValue);
	int GECOAPI GecoRoundfToInt(float fValue);
	double GECOAPI GecoRound(double dValue);
	int GECOAPI GecoRoundToInt(double dValue);
	bool GECOAPI GecoPowerOfTwo(unsigned int uiNumber);
	float GECOAPI GecoDecayf(float fSrcValue, float fDstValue,
		float fHalfLife, float fDeltaTime);
	double GECOAPI GecoDecay(double dSrcValue, double dDstValue,
		double dHalfLife, double dDeltaTime);
	void GECOAPI GecoSrand(unsigned int uiSeed);
	int GECOAPI GecoRand();
	float GECOAPI GecoSymmetricRandomf();
	float GECOAPI GecoUnitRandomf();
	float GECOAPI GecoRangeRandomf(float fMin, float fMax);
	float GECOAPI GecoFastInvSqrtf(float fValue);
	float GECOAPI GecoFastSqrtf(float fValue);
	float GECOAPI GecoNormaliseAnglef(float fAangle);
	float GECOAPI GecoSameSignAnglef(float fAangle, float fClosest);
	float GECOAPI GecoTurnRangeAnglef(float fFrom, float fTo);

	void GECOAPI GecoZero2f(float* fpDst);
	void GECOAPI GecoZero3f(float* fpDst);
	void GECOAPI GecoZero4f(float* fpDst);
	void GECOAPI GecoZero9f(float* fpDst);
	void GECOAPI GecoZero16f(float* fpDst);

	void GECOAPI GecoSet2f(float* fpDst, const float* fpSrc);
	void GECOAPI GecoSet3f(float* fpDst, const float* fpSrc);
	void GECOAPI GecoSet4f(float* fpDst, const float* fpSrc);
	void GECOAPI GecoSet9f(float* fpDst, const float* fpSrc);
	void GECOAPI GecoSet16f(float* fpDst, const float* fpSrc);

	void GECOAPI GecoAdd2f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoAdd3f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoAdd4f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoAdd9f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoAdd16f(float* fpDst, const float* fpSrc0, const float* fpSrc1);

	void GECOAPI GecoSub2f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoSub3f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoSub4f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoSub9f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoSub16f(float* fpDst, const float* fpSrc0, const float* fpSrc1);

	void GECOAPI GecoMul2f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoMul3f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoMul4f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoMul9f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoMul16f(float* fpDst, const float* fpSrc0, const float* fpSrc1);

	void GECOAPI GecoDiv2f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoDiv3f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoDiv4f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoDiv9f(float* fpDst, const float* fpSrc0, const float* fpSrc1);
	void GECOAPI GecoDiv16f(float* fpDst, const float* fpSrc0, const float* fpSrc1);

	void GECOAPI GecoScale2f(float* fpDst, const float* fpSrc, float fScale);
	void GECOAPI GecoScale3f(float* fpDst, const float* fpSrc, float fScale);
	void GECOAPI GecoScale4f(float* fpDst, const float* fpSrc, float fScale);
	void GECOAPI GecoScale9f(float* fpDst, const float* fpSrc, float fScale);
	void GECOAPI GecoScale16f(float* fpDst, const float* fpSrc, float fScale);

	float GECOAPI GecoLengthSqr2f(const float* fpSrc);
	float GECOAPI GecoLengthSqr3f(const float* fpSrc);
	float GECOAPI GecoLengthSqr4f(const float* fpSrc);

	float GECOAPI GecoLength2f(const float* fpSrc);
	float GECOAPI GecoLength3f(const float* fpSrc);
	float GECOAPI GecoLength4f(const float* fpSrc);

	void GECOAPI GecoNormalize2f(float* fpDst, const float* fpSrc);
	void GECOAPI GecoNormalize3f(float* fpDst, const float* fpSrc);
	void GECOAPI GecoNormalize4f(float* fpDst, const float* fpSrc);

	float GECOAPI GecoDot2f(const float* fpSrc0, const float* fpSrc1);
	float GECOAPI GecoDot3f(const float* fpSrc0, const float* fpSrc1);
	float GECOAPI GecoDot4f(const float* fpSrc0, const float* fpSrc1);

	void GECOAPI GecoLerp2f(float* fpDst, const float* fpSrc0, const float* fpSrc1, float t);
	void GECOAPI GecoLerp3f(float* fpDst, const float* fpSrc0, const float* fpSrc1, float t);
	void GECOAPI GecoLerp4f(float* fpDst, const float* fpSrc0, const float* fpSrc1, float t);

	float GECOAPI GecoVector2Crossf(const float* fpVec0, const float* fpVec1);
	void GECOAPI GecoVector3Crossf(float* fpOut, const float* fpVec0, const float* fpVec1);

	void GECOAPI GecoVector3TransformM4f(float* fpOutVec4, const float* fpVec3, const float* fpMat4x4);
	void GECOAPI GecoVector3TransformCoordM4f(float* fpOutVec3, const float* fpVec3, const float* fpMat4x4);
	void GECOAPI GecoVector3TransformNormalM4f(float* fpOutVec3, const float* fpVec3, const float* fpMat4x4);
	void GECOAPI GecoVector3TransformNormalM4f(float* fpOutVec3, const float* fpVec3, const float* fpMat4x4);

	void GECOAPI GecoVector3SetPitchYawYUpf(float* fpOutVec3, float fPitchInRadians, float fYawInRadians);
	float GECOAPI GecoVector3GetPitchYUpf(float* fpVec3);
	float GECOAPI GecoVector3GetYawYUpf(float* fpVec3);

	void GECOAPI GecoVector3SetPitchYawZUpf(float* fpOutVec3, float fPitchInRadians, float fYawInRadians);
	float GECOAPI GecoVector3GetPitchZUpf(float* fpVec3);
	float GECOAPI GecoVector3GetYawZUpf(float* fpVec3);

	void GECOAPI GecoVector4Transformf(float* fpOutVec4, const float* fpVec4, const float* fpMat4x4);
	GECO_OUT_CODE GECOAPI GecoVector4Outcode(const float* fpVec4);

	void GECOAPI GecoMatrixIdenty4f(float* fpOutMat4x4);
	bool GECOAPI GecoMatrixInverse4f(float* fpOutMat4x4, float* fpDeterminant, const float* fpMat4x4);
	void GECOAPI GecoMatrixTranspose4f(float* fpOutMat4x4, const float* fpMat4x4);
	float GECOAPI GecoMatrixDeterminant4f(const float* fpMat4x4);
	void GECOAPI GecoMatrixMultiply4f(float* fpOutMat4x4, const float* fpMat4x4Src0, const float* fpMat4x4Src1);
	void GECOAPI GecoMatrixRotationX4f(float* fpOutMat4x4, float fAngle);
	void GECOAPI GecoMatrixRotationY4f(float* fpOutMat4x4, float fAngle);
	void GECOAPI GecoMatrixRotationZ4f(float* fpOutMat4x4, float fAngle);
	void GECOAPI GecoMatrixScaling4f(float* fpOutMat4x4, float fScaleX, float fScaleY, float fScaleZ);
	void GECOAPI GecoMatrixTranslation4f(float* fpOutMat4x4, float x, float y, float z);
	void GECOAPI GecoMatrixRotationYawPitchRoll4f(float* fpOutMat4x4, float fYaw, float fPitch, float fRoll);
	void GECOAPI GecoMatrixRotationQuaternion4f(float* fpOutMat4x4, const float* fpQuat);
	void GECOAPI GecoMatrixDecomposeWorld4f(float* fpOutQuat, float* fpOutVec3Scale, float* fpOutVec3Trans,
		const float* fpMat4x4);

	void GECOAPI GecoQuaternionRotationMatrixf(float* fpOutQuat, const float* fpMat4x4);
	void GECOAPI GecoQuaternionRotationAxisf(float* fpOutQuat, const float* fpVec3, float fAngle);
	void GECOAPI GecoQuaternionMultiplyf(float* fpOutQuat, const float* fpQuat0, const float* fpQuat1);
	void GECOAPI GecoQuaternionInversef(float* fpOutQuat, const float* fpQuat);
#ifdef __cplusplus
}
#endif // __cplusplus

//#include "GecoVectorMartixQuaternionPower.h"

#endif /* __GecoMathPower_H__ */


