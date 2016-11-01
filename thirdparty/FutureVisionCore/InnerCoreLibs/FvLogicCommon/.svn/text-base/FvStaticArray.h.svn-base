#pragma once

#include <FvKernel.h>
#include <vector>

template<class T, FvUInt32 CNT>
class FvStaticArray
{
	FV_NOT_COPY_COMPARE(FvStaticArray);
public:
	static FvUInt32 SIZE(){return CNT;}
	FvStaticArray(){}
	FvUInt32 Size()const{return CNT;}

	T m_kDatas[CNT];
	void TransformTo(std::vector<T>& kList)
	{
		for (FvUInt32 uiIdx = 0; uiIdx < CNT; ++uiIdx)
		{
			kList.push_back(m_kDatas[uiIdx]);
		}
	}
	template<bool (T::*IsEmpty)()const>
	void TransformTo(std::vector<T>& kList)
	{
		for (FvUInt32 uiIdx = 0; uiIdx < CNT; ++uiIdx)
		{
			const T& kT = m_kDatas[uiIdx];
			if((kT.*IsEmpty)())
			{
				kList.push_back(m_kDatas[uiIdx]);
			}
		}
	}
};

