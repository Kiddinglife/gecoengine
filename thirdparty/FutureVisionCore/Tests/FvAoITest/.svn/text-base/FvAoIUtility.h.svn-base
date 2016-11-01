//{future header message}
#ifndef __FvAoIUtility_H__
#define __FvAoIUtility_H__

#include <FvPowerDefines.h>
#include <FvMathPower.h>
#include <FvDebug.h>


inline FvUInt32 EncodeFloat(const float fVal)
{
	FvUInt32 iVal = (FvUInt32&)(fVal);

	if(iVal & 0x80000000)
		iVal = ~iVal;
	else
		iVal |= 0x80000000;

	return iVal;
}

inline float DecodeFloat(const FvUInt32 iVal)
{
	FvUInt32 iTmp;

	if(iVal & 0x80000000)
		iTmp = iVal & ~0x80000000;
	else
		iTmp = ~iVal;

	return (float&)(iTmp);
}

//! 四舍五入到偶数
inline FvInt32 RoundToInt(double dVal)
{
	const double dMagic = 6755399441055744.0;
	dVal += dMagic;
	return *(FvInt32*)&dVal;
}

//! 求大于给定值的最小的2次幂
inline FvUInt32 NextPowerOfTwo(FvUInt32 uiVal)
{
	uiVal |= (uiVal >> 1);
	uiVal |= (uiVal >> 2);
	uiVal |= (uiVal >> 4);
	uiVal |= (uiVal >> 8);
	uiVal |= (uiVal >> 16);
	return uiVal +1;
}

//! 判断是否是2的幂次
inline bool	IsPowerOfTwo(FvUInt32 uiVal) { return ((uiVal&(uiVal-1)) == 0); }

//! 求Log2,uiValue必须为2的幂次,且>0
inline FvUInt32 Log2(FvUInt32 uiVal)
{
	FvUInt32 uiN;
	__asm {
		bsf eax, uiVal
		mov uiN, eax
	}
	return uiN;
}

const FvInt32 FVAOI_MIN_POS = 0x80000000;
const FvInt32 FVAOI_MAX_POS = 0x7FFFFFFF;

#endif//__FvAoIUtility_H__
