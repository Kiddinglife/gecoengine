//{future header message}
#ifndef __FvMath_H__
#define __FvMath_H__

#include <FvPowerDefines.h>
#include <FvGlobalMacros.h>
#include <FvBaseTypes.h>
#include <FvMathPower.h>

#ifdef FV_AS_STATIC_LIBS
#define FV_MATH_API
#else
#ifdef FV_MATH_EXPORT
#define FV_MATH_API FV_Proper_Export_Flag
#else
#define FV_MATH_API FV_Proper_Import_Flag
#endif
#endif

class FV_MATH_API FvMathTool
{
public:

	//+------------------------------------------------------------------
	static void NormalAngle(float & fAngle);// 把角度映射到(-PI, PI]上来.
	// 线性插值
	template <typename T>
	static  T Lerp(T const & qFrom, T const & qTo, const float fPercent)
	{
		return qFrom + (qTo - qFrom) * fPercent;
	}

	template <typename T>
	static  T LerpTo(T const & qFrom, T const & qTo, const float fDistance)
	{
		T dir = qTo - qFrom;
		dir.Normalise();
		return qFrom + dir * fDistance;
	}

	template <typename T>
	static T Sign(const T val)
	{
		return (val > 0)? T(1): T(-1);
	}
	template <typename T>
	static  bool InRange(const T& inf, const T& value, const T& sup)
	{
		return ((inf <= value) && (value <= sup));
	}
	template <typename T>
	static  T Max(const T& a, const T& b)
	{
		return (a > b)? a: b;
	}
	template <typename T>
	static  T Min(const T& a, const T& b)
	{
		return (a < b)? a: b;
	}
	template <typename T>
	static int Round(const T& val)
	{
		return val > (T)0.0f ? (int)(val + (T)0.5f) : (int)(val - (T)0.5f);
	}
	// 限制 val 在 low 和 high 之间
	template <typename T>
	static  T const & Clamp(T const & val, T const & low, T const & high)
	{
		return std::max(low, std::min(high, val));
	}
	template<typename T>
	static void Swap(T& t_1, T& t_2)
	{
		const T t = t_1;
		t_1 = t_2;
		t_2 = t;
	}
	//+------------------------------------------------------------------
	// 环绕处理
	template <typename T>
	static  T Wrap(T const & val, T const & low, T const & high)
	{
		T ret(val);
		T rang(high - low);

		while (ret >= high)
		{
			ret -= rang;
		}
		while (ret < low)
		{
			ret += rang;
		}
		return ret;
	}

	//+------------------------------------------------------------------
	// 镜像处理
	template <typename T>
	static  T Mirror(T const & val, T const & low, T const & high)
	{
		T ret(val);
		T rang(high - low);
		while ((ret > high) || (ret < low))
		{
			if (ret > high)
			{
				ret = 2 * high - val;
			}
			else
			{
				if (ret < low)
				{
					ret = 2 * low - val;
				}
			}
		}
		return ret;
	}
};


class FV_MATH_API FvMath2DTool
{
public:
	static const float PI;
	static const float HALF_PI;

	//! 正前方的向量[0.0f, -1.0f]
	static const float FRONT_X;
	static const float FRONT_Y;


	

	//+-------------------------------------------------------------------------------------------------
	//! 二维长度计算
	template <typename T>
	static float Length(const T fDX, const T fDY)
	{
		return FvSqrtf(float((fDX)*(fDX)) + float((fDY)*(fDY)));
	}
	template <typename T>
	static T Length2(const T fDX, const T fDY)
	{
		return (fDX*fDX + fDY*fDY);
	}
	template <typename T>
	static T Distance(const T fX_1, const T fY_1, const T fX_2, const T fY_2)
	{ 
		return Length(fX_1 - fX_2 , fY_1 - fY_2);
	}
	template <typename T>
	static float Distance(const T kPos1, const T kPos2)
	{
		return Length(kPos1.x - kPos2.x , kPos1.y - kPos2.y);
	}
	template <typename T>
	static T Distance2(const T fX_1, const T fY_1, const T fX_2, const T fY_2)
	{
		return Length2(fX_1 - fX_2 , fY_1 - fY_2);
	}
	template <typename T>
	static float Distance2(const T kPos1, const T kPos2)
	{
		return Length2(kPos1.x - kPos2.x , kPos1.y - kPos2.y);
	}
	
	// 旋转 [逆时针, 左正右负]
	static float GetAngle(const float fX, const float fY);
	static void CaculateAngle(const float fAngle, float& fX, float& fY);
	//+------------------------------------------------------------------
	static float GetRotateAngle(const float fSrcX, const float fSrcy, const float fDesX, const float fDesY);// 得到两个矢量的夹角
	static void Rotate(const float fSrcX, const float fSrcy, const float fRotateAngle, float& fDesX, float& fDesy);// 将一个矢量旋转一定角度
	static void Rotate(float& fX, float& fY, const float fRotateAngle);// 将一个矢量旋转一定角度
	static void RotateRight90(float& x, float& y);
	static void RotateLeft90(float& x, float& y);
	static int GetSide(const float fromX, const float fromY, const float toX, const float toY, const float x, const float y);//! +1 左侧, -1 右侧 0 线上


};


namespace FvMath2DTool_Demo
{
	static void Test()
	{

	}
}


#endif /* __FvMath_H__ */
