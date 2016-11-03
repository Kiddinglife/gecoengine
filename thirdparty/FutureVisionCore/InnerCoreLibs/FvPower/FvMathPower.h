//{future header message}
#ifndef __FvMathPower_H__
#define __FvMathPower_H__

#include <math.h>

#include "FvPowerDefines.h"

#define FV_MAXFLOAT_F			(3.402823466e+38F)
#define FV_MINFLOAT_F			(0.00000001F)
#define FV_MATH_E_F				(2.7182818284590452354f)
#define FV_MATH_LOG2E_F			(1.4426950408889634074f)
#define FV_MATH_LOG10E_F		(0.43429448190325182765f)
#define FV_MATH_LN2_F			(0.69314718055994530942f)
#define FV_MATH_LN10_F			(2.30258509299404568402f)
#define FV_MATH_PI_F			(3.14159265358979323846f)
#define FV_MATH_2PI_F			(6.28318530717958647692f)
#define FV_MATH_3PI_F			(9.42477796076937971538f)
#define FV_MATH_PI_2_F			(1.57079632679489661923f)
#define FV_MATH_PI_4_F			(0.78539816339744830962f)
#define FV_MATH_3PI_4_F			(2.3561944901923448370E0f)
#define FV_MATH_SQRTPI_F		(1.77245385090551602792981f)
#define FV_MATH_1_PI_F			(0.31830988618379067154f)
#define FV_MATH_2_PI_F			(0.63661977236758134308f)
#define FV_MATH_2_SQRTPI_F		(1.12837916709551257390f)
#define FV_MATH_SQRT2_F			(1.41421356237309504880f)
#define FV_MATH_SQRT1_2_F		(0.70710678118654752440f)
#define FV_MATH_LN2LO_F			(1.9082149292705877000E-10f)
#define FV_MATH_LN2HI_F			(6.9314718036912381649E-1f)
#define FV_MATH_SQRT3_F		   	(1.73205080756887719000f)
#define FV_MATH_IVLN10_F		(0.43429448190325182765f)
#define FV_MATH_LOG2_E_F		(0.693147180559945309417f)
#define FV_MATH_INVLN2_F		(1.4426950408889633870E0f)

#define FvAtanf atanf
#define FvCosf cosf
#define FvSinf sinf
#define FvTanf tanf
#define FvTanhf tanhf
#define FvFrexpf frexpf
#define FvModff modff
#define FvCeilf ceilf
#define FvFabsf fabsf
#define FvFloorf floorf

#define FvAcosf acosf
#define FvAsinf asinf
#define FvAtan2f atan2f
#define FvCoshf coshf
#define FvSinhf sinhf
#define FvExpf expf
#define FvLdexpf ldexpf
#define FvLogf logf
#define FvLog10f log10f
#define FvPowf powf
#define FvFmodf fmodf

#define FvAtan atan
#define FvCos cos
#define FvSin sin
#define FvTan tan
#define FvTanh tanh
#define FvFrexp frexp
#define FvModf modf
#define FvCeil ceil
#define FvFabs fabs
#define FvFloor floor

#define FvAcos acos
#define FvAsin asin
#define FvAtan2 atan2
#define FvCosh cosh
#define FvSinh sinh
#define FvExp exp
#define FvLdexp ldexp
#define FvLog log
#define FvLog10 log10
#define FvPow pow
#define FvFmod fmod

#define FvDegreeToRadiantf(D)	((D) * 0.01745329251994329576923690768f)
#define FvRadiantToDegreef(R)	((R) * 57.2957795130823208767981548f)
#define FvFracf(number)			((number)-(float)((int)(number)))

#define FvClampEx(min,val,max)	((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))
#define FvClamp(mag,val)		(FvClampEx(-(mag),(val),(mag)))

#define FV_INTERVAL_MAPPING(x,src_a,src_b,dst_a,dst_b)\
	(((dst_b) - (dst_a)) * ((x) - (src_a)) / ((src_b) - (src_a)) + dst_a)

//ÓÃC++Ä£°åº¯Êý
//#define FV_SAFE_INTERVAL_MAPPING(x,src_a,src_b,dst_a,dst_b)\
//	(((src_a) == (src_b)) ? (dst_a) : FV_INTERVAL_MAPPING(x,src_a,src_b,dst_a,dst_b))

#define FV_LERP(t,a,b) ((a) + ((b) - (a)) * (t))

#define FV_EXCHANGE_XOR(a,b)\
	(a) = (a) ^ (b);\
	(b) = (a) ^ (b);\
	(a) = (a) ^ (b)

#define FV_EXCHANGE_CLASS(a,b,class)\
	{\
		class t((a));\
		(a) = (b);\
		(b) = t;\
	}

#define FV_EXCHANGE_BUFFER(a,b,t)\
	t = (a);\
	(a) = (b);\
	(b) = t

#ifdef FV_USE_FAST_SQRT

#define FvSqrtf FvFastSqrtf
#define FvInvSqrtf FvFastInvSqrtf

#else

#define FvSqrtf sqrtf
#define FvInvSqrtf(v) (1.0f / sqrtf((v)))

#endif

typedef enum FvAxisEnumeration
{
	FV_X_AXIS = 0,
	FV_Y_AXIS = 1,
	FV_Z_AXIS = 2,
	FV_X_COORD = 0,
	FV_Y_COORD = 1,
	FV_Z_COORD = 2
} FV_AXIS_ENUMERATION;

typedef enum FvOutcode
{
	FV_OUTCODE_NULL = 0x0,
	FV_OUTCODE_LEFT = 0x1,
	FV_OUTCODE_RIGHT = 0x2,
	FV_OUTCODE_BOTTOM = 0x4,
	FV_OUTCODE_TOP = 0x8,
	FV_OUTCODE_NEAR = 0x10,
	FV_OUTCODE_FAR = 0x20,
	FV_OUTCODE_MASK = 0x3F
} FV_OUT_CODE;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void FV_POWER_API FvSinCosf(float* fpSin, float* fpCos, float fRadians);

float FV_POWER_API FvRoundf(float fValue);

int FV_POWER_API FvRoundfToInt(float fValue);

double FV_POWER_API FvRound(double dValue);

int FV_POWER_API FvRoundToInt(double dValue);

FV_BOOL FV_POWER_API FvPowerOfTwo(unsigned int uiNumber);

float FV_POWER_API FvDecayf(float fSrcValue, float fDstValue,
	float fHalfLife, float fDeltaTime);

double FV_POWER_API FvDecay(double dSrcValue, double dDstValue,
	double dHalfLife, double dDeltaTime);

void FV_POWER_API FvSrand(unsigned int uiSeed);

int FV_POWER_API FvRand();

float FV_POWER_API FvSymmetricRandomf();

float FV_POWER_API FvUnitRandomf(); 

float FV_POWER_API FvRangeRandomf(float fMin, float fMax);

float FV_POWER_API FvFastInvSqrtf(float fValue);

float FV_POWER_API FvFastSqrtf(float fValue);



float FV_POWER_API FvNormaliseAnglef(float fAangle);

float FV_POWER_API FvSameSignAnglef(float fAangle, float fClosest);

float FV_POWER_API FvTurnRangeAnglef(float fFrom, float fTo);

#ifdef __cplusplus
}
#endif // __cplusplus

#include "FvVectorMartixQuaternionPower.h"

#endif /* __FvMathPower_H__ */
