#include "FvStringFilter.h"
#include "FvDebug.h"

const size_t g_uiBufSize = 1024;
static char g_acBuf[g_uiBufSize] = {0};
static wchar_t g_acwBuf[g_uiBufSize] = {0};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void ANSI_To_UTF8(FvString& kStr)
{
	FvWString kTmp;
	ANSI_To_UTF16(kStr, kTmp);
	UTF16_To_UTF8(kTmp, kStr);
}
void UTF8_To_ANSI(FvString& kStr)
{
	FvWString kTmp;
	UTF8_To_UTF16(kStr, kTmp);
	UTF16_To_ANSI(kTmp, kStr);
}
void ANSI_To_UTF16(const FvString& kSrc, FvWString& kDes)
{
	size_t uiSize;
	uiSize = MultiByteToWideChar(CP_ACP, 0, kSrc.c_str(), -1, NULL, 0);

	wchar_t* pcWStr;
	if(uiSize > g_uiBufSize)
		pcWStr = new wchar_t[uiSize];
	else
		pcWStr = g_acwBuf;

	MultiByteToWideChar(CP_ACP, 0, kSrc.c_str(), -1, pcWStr, (int)uiSize);

	kDes = pcWStr;

	if(pcWStr != g_acwBuf)
		delete [] pcWStr;
}
void UTF16_To_ANSI(const FvWString& kSrc, FvString& kDes)
{
	size_t uiSize;
	uiSize = WideCharToMultiByte(CP_ACP, 0, kSrc.c_str(), -1, NULL, 0, NULL, NULL);

	char* pcStr;
	if(uiSize > g_uiBufSize)
		pcStr = new char[uiSize];
	else
		pcStr = g_acBuf;

	WideCharToMultiByte(CP_ACP, 0, kSrc.c_str(), -1, pcStr, (int)uiSize, NULL, NULL);

	kDes = pcStr;

	if(pcStr != g_acBuf)
		delete [] pcStr;
}
void UTF8_To_UTF16(const FvString& kSrc, FvWString& kDes)
{
	size_t uiSize;
	uiSize = MultiByteToWideChar(CP_UTF8, 0, kSrc.c_str(), -1, NULL, 0);

	wchar_t* pcWStr;
	if(uiSize > g_uiBufSize)
		pcWStr = new wchar_t[uiSize];
	else
		pcWStr = g_acwBuf;

	MultiByteToWideChar(CP_UTF8, 0, kSrc.c_str(), -1, pcWStr, (int)uiSize);

	kDes = pcWStr;

	if(pcWStr != g_acwBuf)
		delete [] pcWStr;
}
void UTF16_To_UTF8(const FvWString& kSrc, FvString& kDes)
{
	size_t uiSize;
	uiSize = WideCharToMultiByte(CP_UTF8, 0, kSrc.c_str(), -1, NULL, 0, NULL, NULL);

	char* pcStr;
	if(uiSize > g_uiBufSize)
		pcStr = new char[uiSize];
	else
		pcStr = g_acBuf;

	WideCharToMultiByte(CP_UTF8, 0, kSrc.c_str(), -1, pcStr, (int)uiSize, NULL, NULL);

	kDes = pcStr;

	if(pcStr != g_acBuf)
		delete [] pcStr;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
FvStringFilter::FvStringFilter()
:m_cMark('*')
{
	m_kUnLawKeyWords.reserve(8);
}

FvStringFilter::~FvStringFilter()
{

}

void FvStringFilter::AddUnLawKeyWord(const FvString& kANSIWord)
{
	if(kANSIWord.empty())
		return;

	FvString kTmp(kANSIWord);
	ANSI_To_UTF8(kTmp);
	m_kUnLawKeyWords.push_back(kTmp);
}

void FvStringFilter::AddUnLawKeyWords(const FvString& kANSIWordsStream)
{
	FvString::size_type uiPos = 0;
	FvString::size_type uiPosNext = 0;
	while((uiPosNext = kANSIWordsStream.find("\n", uiPos)) != FvString::npos)
	{
		AddUnLawKeyWord(kANSIWordsStream.substr(uiPos, uiPosNext-uiPos));
		uiPos = uiPosNext +1;
	}
}

bool FvStringFilter::HasUnLawKeyWord(const FvString& kUTF8Str) const
{
	if(kUTF8Str.empty() || m_kUnLawKeyWords.empty())
		return false;

	const FvString* pkIdx = &m_kUnLawKeyWords[0];
	const FvString* pkEnd = pkIdx + m_kUnLawKeyWords.size();
	for(; pkIdx!=pkEnd; ++pkIdx)
	{
		//! TODO: 换成KMP算法,会不会更快?
		if(kUTF8Str.find(*pkIdx) != FvString::npos)
			return true;
	}

	return false;
}

bool FvStringFilter::CheckAndReplaceUnLawKeyWords(const FvString& kUTF8Src, FvString& kUTF8Des) const
{
	if(kUTF8Src.empty() || m_kUnLawKeyWords.empty())
		return false;

	kUTF8Des = kUTF8Src;
	bool bFind = false;

	const FvString* pkIdx = &m_kUnLawKeyWords[0];
	const FvString* pkEnd = pkIdx + m_kUnLawKeyWords.size();
	for(; pkIdx!=pkEnd; ++pkIdx)
	{
		//! TODO: 换成KMP算法,会不会更快?
		FvString::size_type uiPos = 0;
		while((uiPos = kUTF8Src.find(*pkIdx, uiPos)) != FvString::npos)
		{
			bFind = true;
			FvString::size_type uiLen = pkIdx->length();
			kUTF8Des.replace(uiPos, uiLen, uiLen, m_cMark);
			uiPos += uiLen;
		}
	}

	return bFind;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
FvMassStringFilter::FvMassStringFilter()
:m_cMark('*')
,m_uiUnLawKeyWordsPairArraySize(0)
,m_pkUnLawKeyWordsPairArray(NULL)
{

}

FvMassStringFilter::~FvMassStringFilter()
{
	if(m_pkUnLawKeyWordsPairArray)
	{
		for(FvUInt32 i=0; i<m_uiUnLawKeyWordsPairArraySize; ++i)
		{
			delete m_pkUnLawKeyWordsPairArray[i].second;
		}
		delete [] m_pkUnLawKeyWordsPairArray;
	}
}

void FvMassStringFilter::AddUnLawKeyWord(const FvString& kANSIWord, UnLawKeyWordMap& kMap)
{
	if(kANSIWord.empty())
		return;

	FvString kTmp(kANSIWord);
	ANSI_To_UTF8(kTmp);
	FvUInt64 kFirstWord;
	if(GetUTF8Word(kTmp.c_str(), 0, kFirstWord) == -1)
		return;

	UnLawKeyWords* pkUnLawKeyWords = NULL;
	UnLawKeyWordMap::iterator itr = kMap.find(kFirstWord);
	if(itr == kMap.end())
	{
		pkUnLawKeyWords = new UnLawKeyWords();
		kMap.insert(std::make_pair(kFirstWord, pkUnLawKeyWords));
		pkUnLawKeyWords->reserve(8);
		pkUnLawKeyWords->push_back(kTmp);
	}
	else
	{
		pkUnLawKeyWords = itr->second;
		FvString* pkIdx = &(*pkUnLawKeyWords)[0];
		FvString* pkEnd = pkIdx + pkUnLawKeyWords->size();
		for(; pkIdx!=pkEnd; ++pkIdx)
		{
			if(kTmp == *pkIdx)
				return;
		}
		pkUnLawKeyWords->push_back(kTmp);
	}
}

void FvMassStringFilter::AddUnLawKeyWords(const FvString& kANSIWordsStream)
{
	UnLawKeyWordMap kUnLawKeyWordMap;

	FvString::size_type uiPos = 0;
	FvString::size_type uiPosNext = 0;
	while((uiPosNext = kANSIWordsStream.find("\n", uiPos)) != FvString::npos)
	{
		AddUnLawKeyWord(kANSIWordsStream.substr(uiPos, uiPosNext-uiPos), kUnLawKeyWordMap);
		uiPos = uiPosNext +1;
	}

	m_uiUnLawKeyWordsPairArraySize = FvUInt32(kUnLawKeyWordMap.size());
	if(!m_uiUnLawKeyWordsPairArraySize)
		return;

	m_pkUnLawKeyWordsPairArray = new UnLawKeyWordsPair[m_uiUnLawKeyWordsPairArraySize];
	UnLawKeyWordsPair* pkIdx = m_pkUnLawKeyWordsPairArray;
	UnLawKeyWordMap::iterator itrB = kUnLawKeyWordMap.begin();
	UnLawKeyWordMap::iterator itrE = kUnLawKeyWordMap.end();
	for(; itrB!=itrE; ++itrB, ++pkIdx)
	{
		pkIdx->first = itrB->first;
		pkIdx->second = itrB->second;
	}
}

bool FvMassStringFilter::HasUnLawKeyWord(const FvString& kUTF8Str) const
{
	if(kUTF8Str.empty() || !m_uiUnLawKeyWordsPairArraySize)
		return false;

	size_t uiOffset = 0;
	size_t uiOffsetNext = 0;
	FvUInt64 kWord;
	while((uiOffsetNext = GetUTF8Word(kUTF8Str.c_str(), uiOffset, kWord)) != -1)
	{
		UnLawKeyWords* pkUnLawKeyWords = FindKeyWords(kWord);
		if(!pkUnLawKeyWords)
		{
			uiOffset = uiOffsetNext;
			continue;
		}

		const UnLawKeyWords& kUnLawKeyWords = *pkUnLawKeyWords;
		FV_ASSERT(kUnLawKeyWords.size());
		const FvString* pkIdx = &kUnLawKeyWords[0];
		const FvString* pkEnd = pkIdx + kUnLawKeyWords.size();
		for(; pkIdx!=pkEnd; ++pkIdx)
		{
			//! TODO: 换成KMP算法,会不会更快?
			if(kUTF8Str.find(*pkIdx, uiOffset) != FvString::npos)
				return true;
		}

		uiOffset = uiOffsetNext;
	}

	return false;
}

bool FvMassStringFilter::CheckAndReplaceUnLawKeyWords(const FvString& kUTF8Src, FvString& kUTF8Des) const
{
	if(kUTF8Src.empty() || !m_uiUnLawKeyWordsPairArraySize)
		return false;

	kUTF8Des = kUTF8Src;
	bool bFind = false;

	size_t uiOffset = 0;
	size_t uiOffsetNext = 0;
	FvUInt64 kWord;
	UnLawKeyWordMap::const_iterator itr;
	while((uiOffsetNext = GetUTF8Word(kUTF8Des.c_str(), uiOffset, kWord)) != -1)
	{
		if(kWord == m_cMark)
		{
			uiOffset = uiOffsetNext;
			continue;
		}

		UnLawKeyWords* pkUnLawKeyWords = FindKeyWords(kWord);
		if(!pkUnLawKeyWords)
		{
			uiOffset = uiOffsetNext;
			continue;
		}

		size_t uiKeyWordLen = 0;
		const UnLawKeyWords& kUnLawKeyWords = *pkUnLawKeyWords;
		FV_ASSERT(kUnLawKeyWords.size());
		const FvString* pkIdx = &kUnLawKeyWords[0];
		const FvString* pkEnd = pkIdx + kUnLawKeyWords.size();
		for(; pkIdx!=pkEnd; ++pkIdx)
		{
			//! TODO: 换成KMP算法,会不会更快?
			FvString::size_type uiPos = uiKeyWordLen ? uiOffset + uiKeyWordLen : uiOffset;
			FvString::size_type uiLen = pkIdx->length();
			while((uiPos = kUTF8Des.find(*pkIdx, uiPos)) != FvString::npos)
			{
				bFind = true;
				kUTF8Des.replace(uiPos, uiLen, uiLen, m_cMark);
				uiPos += uiLen;
				if(!uiKeyWordLen && uiPos == uiOffset + uiLen)
					uiKeyWordLen = uiLen;
			}
		}

		if(uiKeyWordLen)
			uiOffset += uiKeyWordLen;
		else
			uiOffset = uiOffsetNext;
	}

	return bFind;
}

FvMassStringFilter::UnLawKeyWords* FvMassStringFilter::FindKeyWords(FvUInt64 uiWord) const
{
	if(!m_uiUnLawKeyWordsPairArraySize ||
		uiWord < m_pkUnLawKeyWordsPairArray[0].first ||
		m_pkUnLawKeyWordsPairArray[m_uiUnLawKeyWordsPairArraySize -1].first < uiWord)
		return NULL;

	FvUInt32 uiFirst = 0;
	FvUInt32 uiLast = m_uiUnLawKeyWordsPairArraySize -1;
	FvUInt32 uiMid = 0;
	do
	{
		uiMid = (uiFirst + uiLast) >> 1;

		if(m_pkUnLawKeyWordsPairArray[uiMid].first < uiWord)
		{
			uiFirst = uiMid +1;
		}
		else if(uiWord < m_pkUnLawKeyWordsPairArray[uiMid].first)
		{
			uiLast = uiMid -1;
		}
		else
		{
			return m_pkUnLawKeyWordsPairArray[uiMid].second;
		}
	}
	while(uiFirst <= uiLast);

	return NULL;
}

size_t GetUTF8Word(const char* pcUTF8Str, size_t uiOffset, FvUInt64& kUTF8Word)
{
	pcUTF8Str += uiOffset;
	FvUInt8 cTmp = *(FvUInt8*)pcUTF8Str;
	if(!cTmp)
		return -1;

	if((cTmp & 0x80) == 0)//! 0XXX XXXX
	{
		kUTF8Word = cTmp;
		return uiOffset+1;
	}
	else if((cTmp & 0xE0) == 0xC0)//! 110X XXXX
	{
		kUTF8Word = *(FvUInt16*)pcUTF8Str;
		return uiOffset+2;
	}
	else if((cTmp & 0xF0) == 0xE0)//! 1110 XXXX
	{
		kUTF8Word = (cTmp<<16) + (*(FvUInt16*)(pcUTF8Str+1));
		return uiOffset+3;
	}
	else if((cTmp & 0xF8) == 0xF0)//! 1111 0XXX
	{
		kUTF8Word = *(FvUInt32*)pcUTF8Str;
		return uiOffset+4;
	}
	else if((cTmp & 0xFC) == 0xF8)//! 1111 10XX
	{
		kUTF8Word = ((FvUInt64)cTmp<<32) + (*(FvUInt32*)(pcUTF8Str+1));
		return uiOffset+5;
	}
	else if((cTmp & 0xFE) == 0xFC)//! 1111 110X
	{
		kUTF8Word = ((FvUInt64)(*(FvUInt16*)pcUTF8Str)<<32) + (*(FvUInt32*)(pcUTF8Str+2));
		return uiOffset+6;
	}
	else
	{
		FV_ERROR_MSG("%s, UTF-8 Mark Err\n", __FUNCTION__);
		return -1;
	}
}

int CalcWordCnt(const FvString& kUTF8Str, int iAsciiCoef, int iNotAsciiCoef)
{
	if(kUTF8Str.empty())
		return 0;

	size_t uiOffset = 0;
	size_t uiOffsetNext = 0;
	FvUInt64 kWord;
	int iCnt = 0;
	while((uiOffsetNext = GetUTF8Word(kUTF8Str.c_str(), uiOffset, kWord)) != -1)
	{
		if(kWord < 0x80)
			iCnt += iAsciiCoef;
		else
			iCnt += iNotAsciiCoef;
		uiOffset = uiOffsetNext;
	}

	return iCnt;
}

FvUInt16 UTF8_TO_UNICODE(const char* pcUTF8Str,wchar_t* pwcUnicodeStr,FvUInt8 uiUnicodeLen)
{
	if(uiUnicodeLen <= 0) 
	{
		return -1;
	}

	FvUInt16 usUnicodeCount = 0;

	while(*pcUTF8Str)
	{
		if((*pcUTF8Str & 0x80) == 0)
		{
			++usUnicodeCount;
			if(usUnicodeCount > uiUnicodeLen) 
			{
				return -1;
			}

			*pwcUnicodeStr = (wchar_t)(*pcUTF8Str);
			pcUTF8Str++;			
			pwcUnicodeStr++;
		}
		else if((*pcUTF8Str & 0xE0) == 0xC0)
		{
			++usUnicodeCount;
			if(usUnicodeCount > uiUnicodeLen)
			{
				return -1;
			}

			char uiTemp1 = (*pcUTF8Str & (0xff >> 3));
			pcUTF8Str++;
			char uiTemp2 = (*pcUTF8Str & (0xff >> 2));
			pcUTF8Str++;

			wchar_t wcTemp1 = ((wchar_t)(uiTemp1 >> 2)) << 13;
			wchar_t wcTemp2 = (wchar_t)(((uiTemp1 & (0xff >> 6)) << 6) | uiTemp2);

			*pwcUnicodeStr = wcTemp1 | wcTemp2;
			pwcUnicodeStr++;
		}
		else if((*pcUTF8Str & 0xF0) == 0xE0)
		{
			++usUnicodeCount;
			if(usUnicodeCount > uiUnicodeLen)
			{
				return -1;
			}

			char uiTemp1 = (*pcUTF8Str & (0xff >> 3));
			pcUTF8Str++;
			char uiTemp2 = (*pcUTF8Str & (0xff >> 2));
			pcUTF8Str++;
			char uiTemp3 = (*pcUTF8Str & (0xff >> 2));
			pcUTF8Str++;

			wchar_t wcTemp1 = ((wchar_t)((uiTemp1 << 4) | (uiTemp2 >> 2))) << 8;
			wchar_t wcTemp2 = (wchar_t)(((uiTemp2 & (0xff >> 6)) << 6) | uiTemp3);

			*pwcUnicodeStr = wcTemp1 | wcTemp2;
			pwcUnicodeStr++;
		}
		else
		{
			return -1;
		}
	}
	return usUnicodeCount;
}
