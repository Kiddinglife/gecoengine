//{future header message}
#ifndef __FvGameRandom_H__
#define __FvGameRandom_H__

#include "FvLogicCommonDefine.h"
#include <FvMath.h>
#include <FvDebug.h>
#include <algorithm>
#include <vector>

template<const FvUInt32 RANGE>
class Random
{
public:

	Random():m_uiIdx_1(0),m_uiIdx_2(RANGE/2)
	{
		memset(m_kValue, 0, sizeof(FvInt32)*RANGE);
		Init(0);
	}
	FvInt32 GetValue(const FvInt32& iInf, const FvInt32& iSup) //! 取值范围 [iInf, iSup) 左闭右开
	{
		if(iInf >= iSup)
		{
			return iInf;
		}
		const FvInt32 iRange = iSup - iInf;
		const FvInt32 iRrangeCnt = iRange/RANGE;
		const FvInt32 iRrangeMod = iRrangeCnt*RANGE;
		if(iRrangeMod == iRange)
		{
			const FvInt32 iValue = iInf + _GetValue_1()*(iRrangeCnt-1) + _GetValue_2();
			FV_ASSERT(iValue < iSup);
			return iValue;
		}
		else
		{
			const FvInt32 iResidue = iRange - iRrangeCnt*RANGE;
			const FvInt32 iValue = iInf + _GetValue_1()*iRrangeCnt + _GetValue_2()%iResidue; 
			FV_ASSERT(iValue < iSup);
			return iValue;
		}
	}
	bool GetValue(const float probability)
	{
		const FvUInt32 iSup = FvUInt32(RANGE*probability);
		const FvUInt32 iValue = _GetValue_1();
		return (iValue < iSup);
	}

	void Init(const FvUInt32 seed)
	{ 
		for(FvInt32 uiIdx = 0; uiIdx < RANGE; ++uiIdx)
		{
			m_kValue[uiIdx] = uiIdx;
		}
		int* pkBegin = m_kValue;
		int* pkEnd = m_kValue + RANGE;
		srand(seed);
		std::random_shuffle(pkBegin, pkEnd);
	}

private:

	FvInt32 _GetValue_1()
	{
		++m_uiIdx_1;// += 2;
		m_uiIdx_1 = (m_uiIdx_1 >= RANGE)? 0: m_uiIdx_1;
		return m_kValue[m_uiIdx_1];
	}
	FvInt32 _GetValue_2()
	{
		++m_uiIdx_2;// += 3;
		m_uiIdx_2 = (m_uiIdx_2 >= RANGE)? 0: m_uiIdx_2;
		return m_kValue[m_uiIdx_2];
	}

	FvUInt32 m_uiIdx_1;
	FvUInt32 m_uiIdx_2;
	FvInt32 m_kValue[RANGE];
};


class FV_LOGIC_COMMON_API FvGameRandom
{
public:
	
	static FvInt32 GetValue(const FvInt32& iInf, const FvInt32& iSup);//! 取值范围 [iInf, iSup] 左闭右开
	static FvUInt32 GetValueByRange(const FvUInt32& uiValue, const FvUInt32& uiRange);
	static bool Probability(const float fValue);//! 0 <= fValue && fValue <= 1.0f
	static bool Probability(const FvInt32& iValue);//! 0 <= fValue && fValue <= 100000
	static FvUInt32 Probability(const std::vector<FvUInt32>& rangeList);
	static bool Probability(const std::vector<FvUInt32>& rangeList, FvUInt32& uiIdx);

private:

	FvGameRandom(void);
	~FvGameRandom(void);

	static const FvUInt32 MAX_VALUE = 100000;
	static const float RE_MAX_VALUE;
	static Random<100003> ms_RandomSet;
};

template<FvUInt32 CNT>
class RandomChannel
{
public:

	RandomChannel():m_uiMaxChannel(0){}
	~RandomChannel(){}

	void Add(const FvUInt32 uiChannel, const FvUInt32 uiKey, const FvInt32 uiProbability)
	{
		if(uiChannel >= CNT)
		{
			FvLogBus::CritOk("概率通道溢出");
			return;
		}
		if(uiChannel == 0)
		{
			if(FvGameRandom::Probability(uiProbability))
			{
				m_UnChannelKey.push_back(uiKey);
			}
		}
		else
		{
			m_ProbabilityList[uiChannel].push_back(uiProbability);
			m_KeyList[uiChannel].push_back(uiKey);
			m_uiMaxChannel = FvMathTool::Max(m_uiMaxChannel, uiChannel);
		}
	}
	const std::vector<FvUInt32>& Update()
	{
		m_KeyResult = m_UnChannelKey;
		m_UnChannelKey.clear();
		for (FvUInt32 uiIdx = 0; uiIdx <= m_uiMaxChannel; ++uiIdx)
		{
			FvUInt32 uiResult = 0;
			if(FvGameRandom::Probability(m_ProbabilityList[uiIdx], uiResult))
			{
				FV_ASSERT(uiResult <= m_KeyList[uiIdx].size());
				m_KeyResult.push_back(m_KeyList[uiIdx][uiResult]);
			}
			m_ProbabilityList[uiIdx].clear();
			m_KeyList[uiIdx].clear();
		}
		m_uiMaxChannel = 0;
		return m_KeyResult;
	}

	FvUInt32 GetChannelSize(const FvUInt32 uiChannel)const
	{
		if(uiChannel >= CNT)
		{
			FvLogBus::CritOk("概率通道溢出");
			return 0;
		}
		if(uiChannel == 0)
		{
			return m_UnChannelKey.size();
		}
		else
		{
			return m_ProbabilityList[uiChannel].size();
		}
	}

private:

	std::vector<FvUInt32> m_ProbabilityList[CNT];
	std::vector<FvUInt32> m_KeyList[CNT];
	std::vector<FvUInt32> m_KeyResult;
	std::vector<FvUInt32> m_UnChannelKey;
	FvUInt32 m_uiMaxChannel;

};

//+-----------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------
///<测试用例>

#include "FvLogBus.h"
namespace Random_Demo
{
	static void Test()
	{
		RandomChannel<10> TEST_RandomChannel;
		for (FvUInt32 i = 0; i < 10; ++i)
		{
			FvUInt32 uiCount[10] = {0};
			FvUInt32 uiIdx = 0;
			while(uiIdx < 10000)
			{
				TEST_RandomChannel.Add(0, 0, 1000);
				TEST_RandomChannel.Add(0, 1, 1000);
				TEST_RandomChannel.Add(0, 2, 1000);
				TEST_RandomChannel.Add(0, 3, 1000);
				TEST_RandomChannel.Add(0, 4, 1000);
				TEST_RandomChannel.Add(0, 5, 1000);
				TEST_RandomChannel.Add(0, 6, 1000);
				TEST_RandomChannel.Add(0, 7, 1000);
				TEST_RandomChannel.Add(0, 8, 1000);
				TEST_RandomChannel.Add(0, 9, 1000);
				const std::vector<FvUInt32>& resultIdx = TEST_RandomChannel.Update();
				for (std::vector<FvUInt32>::const_iterator iter = resultIdx.begin(); iter != resultIdx.end(); ++iter)
				{
					//FvLogBus::CritOk("%d", (*iter));
					++uiCount[(*iter)];
				}
				++uiIdx;
			}
			for (FvUInt32 uiIdx = 0; uiIdx < 10; ++uiIdx)
			{
				FvLogBus::CritOk("%d Count %d", uiIdx, uiCount[uiIdx]);
			}
		}
	}
}
#endif //__FvGameRandom_H__
