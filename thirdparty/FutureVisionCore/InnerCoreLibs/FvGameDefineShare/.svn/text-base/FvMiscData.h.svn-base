//{future header message}
#ifndef __FvMiscData_H__
#define  __FvMiscData_H__

#include <FvKernel.h>
#include <FvDebug.h>

#include <map>
#include <string>

struct MiscData
{
	MiscData():m_pcName(NULL), m_iValue(0)
	{}
	char* m_pcName;
	FvInt32 m_iValue;
};

template <FvUInt32 CNT>
class MiscDataArray
{
public:
	~MiscDataArray(){}

	bool GetName(const FvUInt32 uiIdx, std::string& kName)const
	{
		if(uiIdx >= CNT)
		{
			return false;
		}
		const MiscData& kData = m_kDatas[uiIdx];
		if(m_kDatas->m_pcName && m_kDatas->m_pcName[0])
		{
			kName = m_kDatas->m_pcName;
			return true;
		}
		else
		{
			return false;
		}
	}
	const char* Name(const FvUInt32 uiIdx, const char* strDefault = "")const
	{
		if(uiIdx >= CNT)
		{
			return strDefault;
		}
		const MiscData& kData = m_kDatas[uiIdx];
		if(m_kDatas->m_pcName && m_kDatas->m_pcName[0])
		{
			return m_kDatas->m_pcName;
		}
		else
		{
			return strDefault;
		}
	}

	bool GetValue(const char* pcName, FvInt32& iValue)const
	{
		for (FvUInt32 uiIdx = 0; uiIdx < CNT; ++uiIdx)
		{
			const MiscData& kData = m_kDatas[uiIdx];
			if(kData.m_pcName && _strcmpi(pcName, kData.m_pcName) == 0)
			{
				iValue = kData.m_iValue;
				return true;
			}
		}
		//FvLogBus::Warning("MiscDataArray::GetValue没有字段名[%s]", pcName);
		return false;
	}
	FvInt32 Value(const char* pcName, const FvInt32 iDefaultValue = 0)const
	{
		for (FvUInt32 uiIdx = 0; uiIdx < CNT; ++uiIdx)
		{
			const MiscData& kData = m_kDatas[uiIdx];
			if(kData.m_pcName && _strcmpi(pcName, kData.m_pcName) == 0)
			{
				return kData.m_iValue;
			}
		}
		FvLogBus::Warning("MiscDataArray::Value没有字段名[%s]", pcName);
		return iDefaultValue;
	}
	void SetValue(const char* pcName, const FvInt32 iValue)
	{
		for (FvUInt32 uiIdx = 0; uiIdx < CNT; ++uiIdx)
		{
			MiscData& kData = m_kDatas[uiIdx];
			if(kData.m_pcName && _strcmpi(pcName, kData.m_pcName) == 0)
			{
				kData.m_iValue = iValue;
				return;
			}
		}
		FvLogBus::Warning("MiscDataArray::SetValue没有字段名[%s]", pcName);
	}

	void Print(std::string& kOutput)const
	{
		for (FvUInt32 uiIdx = 0; uiIdx < CNT; ++uiIdx)
		{
			const MiscData& kData = m_kDatas[uiIdx];
			if(kData.m_pcName && kData.m_pcName[0] != 0)
			{
				kOutput.append(kData.m_pcName);
				static char VALUE_STR[20] = {0};
				sprintf_s(VALUE_STR, 20, "[%d]", kData.m_iValue);
				kOutput.append(VALUE_STR);
			}
		}
	}
private:
	MiscData m_kDatas[CNT];
};

#endif //__FvMiscData_H__
