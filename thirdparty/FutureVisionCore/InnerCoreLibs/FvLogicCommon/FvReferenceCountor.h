//{future header message}
#ifndef __FvReferenceCountor_H__
#define __FvReferenceCountor_H__

#include "FvLogicCommonDefine.h"

class FV_LOGIC_COMMON_API FvReferenceCountor
{
public:
	FvReferenceCountor(void);
	~FvReferenceCountor(void);

	bool Add();
	bool Del();
	bool HasCount()const;

private:
	FvUInt32 m_uiCount;
};

template<FvUInt32 CNT>
class FvMaskReferenceCountor
{
public:
	FvMaskReferenceCountor(void)
	{
		memset(m_uiCount, 0, CNT*sizeof(FvUInt32));
	}
	~FvMaskReferenceCountor(void){}

	bool Add(const FvUInt32 uiIdx)
	{
		if(uiIdx >= CNT)
		{
			return false;
		}
		FvUInt32& uiCount = m_uiCount[uiIdx];
		if(uiCount == 0)
		{
			++uiCount;
			return true;
		}
		else
		{
			++uiCount;
			return false;
		}
	}
	bool Del(const FvUInt32 uiIdx)
	{
		if(uiIdx >= CNT)
		{
			return false;
		}
		FvUInt32& uiCount = m_uiCount[uiIdx];
		FV_ASSERT(uiCount > 0 && "没有计数");
		if(uiCount > 0)
		{
			--uiCount;
			if(uiCount == 0)
			{
				return true;
			}
		}
		return false;
	}
	bool HasCount(const FvUInt32 uiIdx)const
	{
		if(uiIdx >= CNT)
		{
			return false;
		}
		return (m_uiCount[uiIdx] > 0);
	}

private:
	FvUInt32 m_uiCount[CNT];
};


#endif //__FvReferenceCountor_H__
