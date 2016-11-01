#include "FvDataScanf.h"


bool FvDataScanfer::ms_bNoErr = true;
FvDataScanfer* FvDataScanfer::ms_pkInstance = NULL;
static FvDataScanfer s_Instance;

FvMemoryOStream FvCallFuncByStrHandler::ms_kOS(2048);


namespace FvDataScanf
{
	bool ParseStrData(const FvString& kStr, FvString& kFuncProtoType, StrData& kData)
	{
		kData.Clear();
		if(kStr.empty())
		{
			kFuncProtoType = "";
			return true;
		}

		char* pcHead = const_cast<char*>(kStr.c_str());
		char* pcEnd = pcHead + kStr.length();
		FV_ASSERT(*pcEnd == 0);

		kFuncProtoType = "";
		kFuncProtoType.reserve(64);
		if(!ParseStrData(pcHead, pcEnd, kFuncProtoType, kData, true, true, 0))
		{
			kFuncProtoType = "";
			kData.Clear();
			return false;
		}
		return true;
	}

	bool IsType(const FvString& kType, const FvString& kFull)
	{
		if(kType == "" && kFull == "")
			return true;

		const char* pcType = kType.c_str();
		const char* pcFull = kFull.c_str();
		while(*pcType != 0 && *pcFull != 0)
		{
			if(*pcType != *pcFull)
			{
				return false;
			}
			else if(*pcType == '<' && *(pcType+1) == '>')
			{
				pcType += 2;
				++pcFull;
				int iMarkCnt(1);
				for(; *pcFull!=0; ++pcFull)
				{
					if(*pcFull == '>')
					{
						--iMarkCnt;
						FV_ASSERT(iMarkCnt >= 0);
						if(iMarkCnt == 0)
							break;
					}
					else if(*pcFull == '<')
					{
						++iMarkCnt;
					}
				}
				FV_ASSERT(*pcFull == '>');
				++pcFull;
			}
			else
			{
				++pcType;
				++pcFull;
			}
		}

		if(*pcType != 0 || *pcFull != 0)
			return false;
		else
			return true;
	}

	bool ParseStrData(char*& pcHead, char* pcEnd, FvString& kFuncProtoType, StrData& kData, bool bNotInExt, bool bNotInVec, char cLeftMark)
	{
		FV_ASSERT(pcHead < pcEnd);

		static FvString kWord(32, 0);
		kWord.clear();

		kData.m_kList.reserve(4);

		int ret = 0;
		while(ret = GetWord(pcHead, pcEnd, kWord))
		{
			if(ret == 1)//! 标记符
			{
				FV_ASSERT(kWord.length() == 1);
				char cVal = kWord[0];

				if(!bNotInVec && cVal != ']')
					return false;

				switch(cVal)
				{
				case '<':	//! Array开始
					{
						kFuncProtoType += '<';
						int sPos = kFuncProtoType.length();
						kData.m_kList.push_back(FvDataScanf::StrData());
						kData.m_kList.back().m_kList.reserve(4);
						if(!ParseStrData(pcHead, pcEnd, kFuncProtoType, kData.m_kList.back(), bNotInExt, bNotInVec, cVal))
							return false;
						//! 检查<>中间的类型
						static FvString s_kArrayType(64, 0);
						s_kArrayType.clear();
						if(!CheckArrayType(kFuncProtoType, sPos, s_kArrayType))
							return false;
						kFuncProtoType = kFuncProtoType.substr(0, sPos) + s_kArrayType + '>';
					}
					break;
				case '>':	//! Array结束
					{
						if(cLeftMark == '<')
							return true;
						else
							return false;
					}
					break;
				case '{':	//! Struct开始
					{
						kFuncProtoType += '{';
						kData.m_kList.push_back(FvDataScanf::StrData());
						kData.m_kList.back().m_kList.reserve(4);
						if(!ParseStrData(pcHead, pcEnd, kFuncProtoType, kData.m_kList.back(), bNotInExt, bNotInVec, cVal))
							return false;
						if(kFuncProtoType[kFuncProtoType.length()-1] == '{')
							return false;
						kFuncProtoType += '}';
					}
					break;
				case '}':	//! Struct结束
					{
						if(cLeftMark == '{')
							return true;
						else
							return false;
					}
					break;
				case '[':	//! Vector开始
					{
						kData.m_kList.push_back(FvDataScanf::StrData());
						kData.m_kList.back().m_kList.reserve(4);
						if(!ParseStrData(pcHead, pcEnd, kFuncProtoType, kData.m_kList.back(), bNotInExt, false, cVal))
							return false;
						FvUInt32 iSize = kData.m_kList.back().Size();
						if(iSize<2 || 4<iSize)
							return false;
						kFuncProtoType += ('0'+iSize);
					}
					break;
				case ']':	//! Vector结束
					{
						if(cLeftMark == '[')
							return true;
						else
							return false;
					}
					break;
				case '(':	//! 多数据外部类型开始
					{
						kData.m_kList.push_back(FvDataScanf::StrData());
						kData.m_kList.back().m_kList.reserve(4);
						int sPos = kFuncProtoType.length();
						if(!ParseStrData(pcHead, pcEnd, kFuncProtoType, kData.m_kList.back(), false, bNotInVec, cVal))
							return false;
						kFuncProtoType = kFuncProtoType.substr(0, sPos) + '#';
					}
					break;
				case ')':	//! 多数据外部类型结束
					{
						if(cLeftMark == '(')
							return true;
						else
							return false;
					}
					break;
				default:
					FV_ASSERT(0);
					break;
				}
			}
			else//! 单词
			{
				FV_ASSERT(ret == 2);

				kData.m_kList.push_back(FvDataScanf::StrData());
				kData.m_kList.back().m_kData = kWord;
				if(bNotInVec)
					kFuncProtoType += '*';
			}
		}

		if(cLeftMark != 0)
			return false;
		else
			return true;
	}

	//! 返回0:失败,1:标记符,2:单词
	int GetWord(char*& pcHead, char* pcEnd, FvString& kWord)
	{
		FV_ASSERT(pcHead <= pcEnd);

		if(pcHead == pcEnd)
			return 0;

		char cVal;
		for(; pcHead!=pcEnd; ++pcHead)
		{
			cVal = *pcHead;
			if(cVal == ' ' || cVal == '\t')
				continue;
			else
				break;
		}

		if(pcHead == pcEnd)
		{
			return 0;
		}
		else if(cVal == '<' ||	//! Array开始
			cVal == '>' ||	//! Array结束
			cVal == '{' ||	//! Struct开始
			cVal == '}' ||	//! Struct结束
			cVal == '[' ||	//! Vector开始
			cVal == ']' ||	//! Vector结束
			cVal == '(' ||	//! 多数据外部类型开始
			cVal == ')')	//! 多数据外部类型结束
		{
			kWord = cVal;
			++pcHead;
			return 1;
		}

		char* pcWord = pcHead;
		++pcHead;
		for(; pcHead!=pcEnd; ++pcHead)
		{
			cVal = *pcHead;
			if(cVal == ' ' || cVal == '\t' ||
				cVal == '<' ||	//! Array开始
				cVal == '>' ||	//! Array结束
				cVal == '{' ||	//! Struct开始
				cVal == '}' ||	//! Struct结束
				cVal == '[' ||	//! Vector开始
				cVal == ']' ||	//! Vector结束
				cVal == '(' ||	//! 外部类型开始
				cVal == ')')	//! 外部类型结束
				break;
		}

		cVal = *pcHead;
		*pcHead = 0;
		kWord = pcWord;
		*pcHead = cVal;
		return 2;
	}

	bool CheckArrayType(FvString& kStr, int iPos, FvString& kArrayType)
	{
		char* pcStr = const_cast<char*>(kStr.c_str());
		char* pcHead = pcStr + iPos;
		char* pcEnd = pcStr + kStr.length();

		static FvString kType;
		static FvString kMerge[2];
		kType.reserve(64);
		kMerge[0].reserve(64);
		kMerge[1].reserve(64);

		if(!GetType(pcHead, pcEnd, kType))
		{
			kArrayType = "";
			return true;
		}
		if(!GetType(pcHead, pcEnd, kMerge[0]))
		{
			kArrayType = kType;
			return true;
		}

		int idx = 0;
		int idxOther = 1;
		char* pcFirst = NULL;
		char* pcSecond = NULL;

		do
		{
			pcFirst = const_cast<char*>(kType.c_str());
			pcSecond = const_cast<char*>(kMerge[idx].c_str());
			kMerge[idxOther].clear();
			if(!MergeType(pcFirst, pcSecond, kMerge[idxOther]))
				return false;
			idxOther = idx;
			idx = (idx+1)%2;
			if(!GetType(pcHead, pcEnd, kType))
				break;
		}
		while(true);

		kArrayType = kMerge[idx];
		return true;
	}

	bool GetType(char*& pcHead, char* pcEnd, FvString& kType)
	{
		FV_ASSERT(pcHead <= pcEnd);
		if(pcHead == pcEnd)
			return false;

		if(*pcHead == '*')
		{
			kType = '*';
			++pcHead;
			return true;
		}
		else if(*pcHead == '#')
		{
			kType = '#';
			++pcHead;
			return true;
		}
		else if(*pcHead == '<')
		{
			char* pcType = pcHead;
			++pcHead;
			int iMarkCnt(1);
			for(; pcHead<pcEnd; ++pcHead)
			{
				if(*pcHead == '>')
				{
					--iMarkCnt;
					FV_ASSERT(iMarkCnt >= 0);
					if(iMarkCnt == 0)
						break;
				}
				else if(*pcHead == '<')
				{
					++iMarkCnt;
				}
			}

			FV_ASSERT(iMarkCnt == 0);
			++pcHead;
			char cTmp = *pcHead;
			*pcHead = 0;
			kType = pcType;
			*pcHead = cTmp;
			return true;
		}
		else if(*pcHead == '{')
		{
			char* pcType = pcHead;
			++pcHead;
			int iMarkCnt(1);
			for(; pcHead<pcEnd; ++pcHead)
			{
				if(*pcHead == '}')
				{
					--iMarkCnt;
					FV_ASSERT(iMarkCnt >= 0);
					if(iMarkCnt == 0)
						break;
				}
				else if(*pcHead == '{')
				{
					++iMarkCnt;
				}
			}

			FV_ASSERT(iMarkCnt == 0);
			++pcHead;
			char cTmp = *pcHead;
			*pcHead = 0;
			kType = pcType;
			*pcHead = cTmp;
			return true;
		}
		else
		{
			FV_ASSERT('1' < *pcHead && *pcHead < '5');
			kType = *pcHead;
			++pcHead;
			return true;
		}
	}

	bool MergeType(char*& pcFirst, char*& pcSecond, FvString& kMerge)
	{
		FV_ASSERT(pcFirst && pcSecond);
		if(*pcFirst != *pcSecond)
			return false;

		if(*pcFirst == '*' || *pcFirst == '#' || ('1' < *pcFirst && *pcFirst < '5'))
		{
			kMerge += *pcFirst;
			++pcFirst;
			++pcSecond;
			return true;
		}
		else if(*pcFirst == '<')
		{
			kMerge += '<';
			++pcFirst;
			++pcSecond;
			if(*pcFirst == '>')
			{
				++pcFirst;

				char* pcType = pcSecond;
				while(*pcSecond != '>')
					++pcSecond;
				++pcSecond;

				char cTmp = *pcSecond;
				*pcSecond = 0;
				kMerge += pcType;
				*pcSecond = cTmp;
				return true;
			}
			else if(*pcSecond == '>')
			{
				char* pcType = pcFirst;
				while(*pcFirst != '>')
					++pcFirst;
				++pcFirst;

				char cTmp = *pcFirst;
				*pcFirst = 0;
				kMerge += pcType;
				*pcFirst = cTmp;

				++pcSecond;
				return true;
			}
			else
			{
				if(!MergeType(pcFirst, pcSecond, kMerge))
					return false;
				FV_ASSERT(*pcFirst == '>' && *pcSecond == '>');
				kMerge += '>';
				++pcFirst;
				++pcSecond;
				return true;
			}
		}
		else
		{
			FV_ASSERT(*pcFirst == '{');
			kMerge += '{';
			++pcFirst;
			++pcSecond;

			while(*pcFirst != '}' && *pcSecond != '}')
			{
				if(*pcFirst != *pcSecond)
					return false;

				if(*pcFirst == '<' || *pcFirst == '{')
				{
					if(!MergeType(pcFirst, pcSecond, kMerge))
						return false;
				}
				else
				{
					kMerge += *pcFirst;
					++pcFirst;
					++pcSecond;
				}
			}

			if(*pcFirst != '}' || *pcSecond != '}')
				return false;

			kMerge += '}';
			++pcFirst;
			++pcSecond;
			return true;
		}
	}


#define DATASCANF_IMPLEMENT(_BUFTYPE, _FORMAT, _TYPE)					\
	template<>															\
	bool Scanf(StrData& kData, _TYPE& kVal)								\
	{																	\
		_BUFTYPE kBuf = 0;												\
		if(sscanf_s(kData.GetData().c_str(), "%"_FORMAT, &kBuf) != 1)	\
			return false;												\
		kVal = _TYPE(kBuf);												\
		if(_BUFTYPE(kVal) != kBuf)										\
			return false;												\
		return true;													\
	}

	DATASCANF_IMPLEMENT(int, "d", FvUInt8)
	DATASCANF_IMPLEMENT(int, "d", FvUInt16)
	DATASCANF_IMPLEMENT(int, "u", FvUInt32)
	DATASCANF_IMPLEMENT(FvUInt64, "I64u", FvUInt64)
	DATASCANF_IMPLEMENT(int, "d", FvInt8)
	DATASCANF_IMPLEMENT(int, "d", FvInt16)
	DATASCANF_IMPLEMENT(int, "d", FvInt32)
	DATASCANF_IMPLEMENT(FvInt64, "I64d", FvInt64)
	DATASCANF_IMPLEMENT(float, "f", float)
	DATASCANF_IMPLEMENT(double, "lf", double)

#undef DATASCANF_IMPLEMENT


	template<>
	bool Scanf(StrData& kData, bool& kVal)
	{
		int kBuf = 0;
		if(sscanf_s(kData.GetData().c_str(), "%d", &kBuf) != 1)
			return false;
		kVal = kBuf ? true : false;
		return true;
	}

	template<>
	bool Scanf(StrData& kData, FvString& kVal)
	{
		kVal = kData.GetData();
		return true;
	}

	template<>
	bool Scanf(StrData& kData, FvVector2& kVal)
	{
		if(kData.Size() != 2)
			return false;
		if(sscanf_s(kData[0].GetData().c_str(), "%f", &kVal[0]) != 1)
			return false;
		if(sscanf_s(kData[1].GetData().c_str(), "%f", &kVal[1]) != 1)
			return false;
		return true;
	}

	template<>
	bool Scanf(StrData& kData, FvVector3& kVal)
	{
		if(kData.Size() != 3)
			return false;
		if(sscanf_s(kData[0].GetData().c_str(), "%f", &kVal[0]) != 1)
			return false;
		if(sscanf_s(kData[1].GetData().c_str(), "%f", &kVal[1]) != 1)
			return false;
		if(sscanf_s(kData[2].GetData().c_str(), "%f", &kVal[2]) != 1)
			return false;
		return true;
	}

	template<>
	bool Scanf(StrData& kData, FvVector4& kVal)
	{
		if(kData.Size() != 4)
			return false;
		if(sscanf_s(kData[0].GetData().c_str(), "%f", &kVal[0]) != 1)
			return false;
		if(sscanf_s(kData[1].GetData().c_str(), "%f", &kVal[1]) != 1)
			return false;
		if(sscanf_s(kData[2].GetData().c_str(), "%f", &kVal[2]) != 1)
			return false;
		if(sscanf_s(kData[3].GetData().c_str(), "%f", &kVal[3]) != 1)
			return false;
		return true;
	}

}


FvDataScanfer::FvDataScanfer()
{
	FV_ASSERT(!ms_pkInstance);
	ms_pkInstance = this;
}


