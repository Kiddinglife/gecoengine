#pragma once

#include <FvKernel.h>
#include <vector>

class FvSTLAssisstant
{
public:	
	template<class T>
	static const T& Lerp(const std::vector<T>& kList, const FvUInt32 uiIdx, const T& kDefaultValue)
	{
		if(kList.size() == 0)
		{
			return kDefaultValue;
		}
		if(uiIdx < kList.size())
		{
			return kList[uiIdx];
		}
		else
		{
			return kList.back();
		}
	}
	template<class T>
	static const T& Clamp(const std::vector<T>& kList, const FvUInt32 uiIdx, const T& kDefaultValue)
	{
		if(kList.size() == 0)
		{
			return kDefaultValue;
		}
		const FvUInt32 uiIdxMod = FvMathTool::Clamp(uiIdx, 0, kList.size() - 1);
		return kList[uiIdxMod];
	}

private:
	FvSTLAssisstant(void);
	~FvSTLAssisstant(void);
};
