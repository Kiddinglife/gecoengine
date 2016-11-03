//{future header message}
#ifndef __FvDataScanf_H__
#define __FvDataScanf_H__

#include <FvBaseTypes.h>
#include <vector>
#include <FvVector4.h>
#include <FvMemoryStream.h>
#include "FvEntityDefDefines.h"


namespace FvDataScanf
{
	class StrData;
	FV_ENTITYDEF_API bool ParseStrData(const FvString& kStr, FvString& kFuncProtoType, StrData& kData);
	bool IsType(const FvString& kType, const FvString& kFull);
	bool ParseStrData(char*& pcHead, char* pcEnd, FvString& kFuncProtoType, StrData& kData, bool bNotInExt, bool bNotInVec, char cLeftMark);
	int GetWord(char*& pcHead, char* pcEnd, FvString& kWord);//! 返回0:失败,1:标记符,2:单词
	bool CheckArrayType(FvString& kStr, int iPos, FvString& kArrayType);
	bool GetType(char*& pcHead, char* pcEnd, FvString& kType);
	bool MergeType(char*& pcFirst, char*& pcSecond, FvString& kMerge);

	class StrData
	{
	public:
		const FvString& GetData() const { return m_kData; }
		FvUInt32 Size() const { return FvUInt32(m_kList.size()); }
		StrData& operator[](FvUInt32 uiIdx)	{ FV_ASSERT(uiIdx<Size()); return m_kList[uiIdx]; }
		void Clear() { m_kList.clear(); }

	protected:
		FvString	m_kData;
		typedef std::vector<StrData> List;
		List		m_kList;
		friend bool ParseStrData(char*&, char*, FvString&, StrData&, bool, bool, char);
	};

	template<class T>
	bool Scanf(StrData& kData, T& kVal);	//! 用Data->Variable而不用Data->Stream,以避免流化出错
											//! TODO: 如果是指针类型,可能会有内存泄漏

	template<class T>
	bool Scanf(StrData& kData, std::vector<T>& kVal)
	{
		FV_ASSERT(kVal.empty());
		FvUInt32 uiSize = kData.Size();
		if(uiSize == 0)
			return true;
		kVal.resize(uiSize);
		FvUInt32 idx(0);
		for(; idx<uiSize; ++idx)
		{
			if(!Scanf(kData[idx], kVal[idx]))
				return false;
		}
		return true;
	}


	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvUInt8& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvUInt16& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvUInt32& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvUInt64& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvInt8& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvInt16& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvInt32& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvInt64& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, float& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, double& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, bool& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvString& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvVector2& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvVector3& kVal);
	template<>
	FV_ENTITYDEF_API bool Scanf(StrData& kData, FvVector4& kVal);


}


class FV_ENTITYDEF_API FvDataScanfer
{
public:
	FvDataScanfer();

	static FvDataScanfer& Instance() { return *ms_pkInstance; }

	FvDataScanfer& Begin()
	{
		ms_bNoErr = true;
		return *this;
	}
	bool End()
	{
		return ms_bNoErr;
	}

	template<class T>
	FvDataScanfer& Scanf(FvDataScanf::StrData& kData, T& kVal)
	{
		if(!ms_bNoErr)
			return *this;
		ms_bNoErr = ms_bNoErr && FvDataScanf::Scanf(kData, kVal);
		return *this;
	}

protected:
	static bool				ms_bNoErr;
	static FvDataScanfer*	ms_pkInstance;
};


class FV_ENTITYDEF_API FvCallFuncByStrHandler
{
public:
	FvCallFuncByStrHandler(bool bToBase, FvUInt16 uiFuncID):m_bToBase(bToBase),m_uiFuncID(uiFuncID){}
	virtual ~FvCallFuncByStrHandler(){}

	bool IsToBase() const { return m_bToBase; }
	FvUInt16 GetFuncID() const { return m_uiFuncID; }
	FvBinaryIStream& GetStream() { return ms_kOS; }

	virtual bool Execute(FvDataScanf::StrData& kData) = 0;

protected:
	void Clear() { ms_kOS.Reset(); }

	bool		m_bToBase;
	FvUInt16	m_uiFuncID;

protected:
	static FvMemoryOStream ms_kOS;
};


class FV_ENTITYDEF_API FvLocalCallFuncByStrHandler
{
public:
	FvLocalCallFuncByStrHandler(){}
	virtual ~FvLocalCallFuncByStrHandler(){}

	virtual bool Execute(FvDataScanf::StrData& kData) = 0;

};



#endif//__FvDataScanf_H__