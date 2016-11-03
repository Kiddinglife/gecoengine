//{future header message}
#ifndef __FvStringFilter_H__
#define __FvStringFilter_H__

#include <vector>
#include <map>
#include "FvKernel.h"
#include "FvBaseTypes.h"


//! FvStringFilter用于小规模数量关键字的屏蔽
class FV_KERNEL_API FvStringFilter
{
public:
	FvStringFilter();
	~FvStringFilter();

	void	SetMark(char cMark) { m_cMark=cMark; }

	void	AddUnLawKeyWord(const FvString& kANSIWord);											//! kANSIWord为ANSI格式
	void	AddUnLawKeyWords(const FvString& kANSIWordsStream);									//! kANSIWordsStream为ANSI格式,\n分行,一行一个关键字

	bool	HasUnLawKeyWord(const FvString& kUTF8Str) const;									//! kUTF8Str为UTF-8格式
	bool	CheckAndReplaceUnLawKeyWords(const FvString& kUTF8Src, FvString& kUTF8Des) const;	//! kUTF8Src,kUTF8Des为UTF-8格式

protected:
	typedef std::vector<FvString> UnLawKeyWords;
	UnLawKeyWords	m_kUnLawKeyWords;
	char			m_cMark;
};


//! FvMassStringFilter用于大规模数量关键字的屏蔽
class FV_KERNEL_API FvMassStringFilter
{
public:
	FvMassStringFilter();
	~FvMassStringFilter();

	void	SetMark(char cMark) { m_cMark=cMark; }

	void	AddUnLawKeyWords(const FvString& kANSIWordsStream);									//! kANSIWordsStream为ANSI格式,\n分行,一行一个关键字

	bool	HasUnLawKeyWord(const FvString& kUTF8Str) const;									//! kUTF8Str为UTF-8格式
	bool	CheckAndReplaceUnLawKeyWords(const FvString& kUTF8Src, FvString& kUTF8Des) const;	//! kUTF8Src,kUTF8Des为UTF-8格式

protected:
	typedef std::vector<FvString> UnLawKeyWords;
	UnLawKeyWords* FindKeyWords(FvUInt64 uiWord) const;
	typedef std::map<FvUInt64, UnLawKeyWords*> UnLawKeyWordMap;
	void AddUnLawKeyWord(const FvString& kANSIWord, UnLawKeyWordMap& kMap);

protected:
	typedef std::pair<FvUInt64, UnLawKeyWords*> UnLawKeyWordsPair;
	FvUInt32			m_uiUnLawKeyWordsPairArraySize;
	UnLawKeyWordsPair*	m_pkUnLawKeyWordsPairArray;
	char				m_cMark;
};


void FV_KERNEL_API ANSI_To_UTF8(FvString& kStr);
void UTF8_To_ANSI(FvString& kStr);
void ANSI_To_UTF16(const FvString& kSrc, FvWString& kDes);
void UTF16_To_ANSI(const FvWString& kSrc, FvString& kDes);
void UTF8_To_UTF16(const FvString& kSrc, FvWString& kDes);
void UTF16_To_UTF8(const FvWString& kSrc, FvString& kDes);

//! 取出下一个UTF8单字
size_t GetUTF8Word(const char* pcUTF8Str, size_t uiOffset, FvUInt64& kUTF8Word);


//! 统计字符串中的"字数"
//! kStr: UTF8格式字符串
//! iAsciiCoef: Ascii字符代表的"字数"
//! iNotAsciiCoef: 非Ascii字符代表的"字数"
int CalcWordCnt(const FvString& kUTF8Str, int iAsciiCoef=1, int iNotAsciiCoef=1);

FvUInt16 FV_KERNEL_API UTF8_TO_UNICODE(const char* pcUTF8Str,wchar_t* pwcUnicodeStr,FvUInt8 uiUnicodeLen);


namespace StrFilterTest
{
	static void Test()
	{
		//! 统计字符串中的"字数"
		{
			FvString kANSIStr("KMP本质上是构造了DFA并进行了模拟，");
			//! 字符串必须为UTF8格式,转换UTF8格式
			ANSI_To_UTF8(kANSIStr);
			//! 统计字符串中的"字数",ANSI字符算1个,非ANSI字符算2个
			int iCnt = CalcWordCnt(kANSIStr, 1, 2);
		}

		//! 屏蔽字检测
		{
			FvStringFilter kFilter;

			//! 设置屏蔽码,被屏蔽的字显示'#'
			kFilter.SetMark('#');

			//! 添加屏蔽字"ABC",ANSI格式
			kFilter.AddUnLawKeyWord("ABC");

			//! 添加屏蔽字列表
			//! 从文件test.csv中读取,一行一个屏蔽字,ANSI格式
			//! test.csv中内容:
			//! DDD
			//! FFF
			{
				FILE* pkFile = NULL;
				fopen_s(&pkFile, "test.csv", "rt");
				if(!pkFile)
					return;
				fseek(pkFile, 0, SEEK_END);
				size_t uiFileSize = ftell(pkFile);
				char* pkBuf = new char[uiFileSize];
				fseek(pkFile, 0, SEEK_SET);
				fread(pkBuf, uiFileSize, 1, pkFile);
				fclose(pkFile);

				kFilter.AddUnLawKeyWords(pkBuf);
			}

			//! 检查字符串中是否有屏蔽字
			FvString kANSIStr("AAABC哈哈DDFFFDDE");
			//! 字符串必须为UTF8格式,转换UTF8格式
			ANSI_To_UTF8(kANSIStr);
			if(kFilter.HasUnLawKeyWord(kANSIStr))
			{
				//! 包含屏蔽字
				UTF8_To_ANSI(kANSIStr);
				printf("%s\n", kANSIStr.c_str());
			}

			//! 检查并用屏蔽码替换屏蔽字
			FvString kUTF8DesStr;
			if(kFilter.CheckAndReplaceUnLawKeyWords(kANSIStr, kUTF8DesStr))
			{
				//! 包含屏蔽字
				UTF8_To_ANSI(kUTF8DesStr);
				printf("%s\n", kUTF8DesStr.c_str());
			}
		}
	}
}

#endif//__FvStringFilter_H__