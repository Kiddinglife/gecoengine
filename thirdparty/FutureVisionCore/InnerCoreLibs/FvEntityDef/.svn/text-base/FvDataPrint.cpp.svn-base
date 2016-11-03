#include "FvDataPrint.h"


const FvUInt32 uiBufSize = 2048;
static char g_acBuf[uiBufSize];
FvDataPrinter* FvDataPrinter::ms_pkInstance = NULL;
static FvDataPrinter s_Instance;


namespace FvDataPrint
{
	void Print(FvString& kMsg, const char *pcFormat, ...)
	{
		va_list kArgPtr;
		va_start(kArgPtr, pcFormat);

		vsnprintf_s(g_acBuf, sizeof(g_acBuf), _TRUNCATE, pcFormat, kArgPtr);
		kMsg.append(g_acBuf);

		va_end(kArgPtr);
	}
}


FvDataPrinter::FvDataPrinter()
:m_kMsg(uiBufSize, 0)
{
	m_kMsg.clear();
	FV_ASSERT(!ms_pkInstance);
	ms_pkInstance = this;
}

FvDataPrinter& FvDataPrinter::PrintRaw(const char *pcFormat, ...)
{
	va_list kArgPtr;
	va_start(kArgPtr, pcFormat);

	vsnprintf_s(g_acBuf, sizeof(g_acBuf), _TRUNCATE, pcFormat, kArgPtr);
	m_kMsg.append(g_acBuf);

	va_end(kArgPtr);

	return *this;
}

void FvDataPrinter::Output()
{
	FV_INFO_MSG("%s\n", m_kMsg.c_str());
	m_kMsg.clear();
}



namespace FvDataPrint
{
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define SIMPLEDATA_PRINT(_TYPE, _FORMAT)								\
	template<>															\
	void Print(const char* pcName, const _TYPE& kVal, FvString& kMsg)	\
	{																	\
		FV_ASSERT(pcName);												\
		FvDataPrint::Print(kMsg, "%s[%" _FORMAT "]", pcName, kVal);		\
	}

	SIMPLEDATA_PRINT(FvUInt8, "d")
	SIMPLEDATA_PRINT(FvUInt16, "d")
	SIMPLEDATA_PRINT(FvUInt32, "u")
	SIMPLEDATA_PRINT(FvUInt64, "I64u")
	SIMPLEDATA_PRINT(FvInt8, "d")
	SIMPLEDATA_PRINT(FvInt16, "d")
	SIMPLEDATA_PRINT(FvInt32, "d")
	SIMPLEDATA_PRINT(FvInt64, "I64d")
	SIMPLEDATA_PRINT(float, "f")
	SIMPLEDATA_PRINT(double, "lf")


	template<>
	void Print(const char* pcName, const bool& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s[%s]", pcName, kVal ? "true" : "false");
	}

	template<>
	void Print(const char* pcName, const FvString& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s[%s]", pcName, kVal.c_str());
	}

	template<>
	void Print(const char* pcName, const FvVector2& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s[%f %f]", pcName, kVal.x, kVal.y);
	}

	template<>
	void Print(const char* pcName, const FvVector3& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s[%f %f %f]", pcName, kVal.x, kVal.y, kVal.z);
	}

	template<>
	void Print(const char* pcName, const FvVector4& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s[%f %f %f %f]", pcName, kVal.x, kVal.y, kVal.z, kVal.w);
	}


#define POINTER_PRINT(_TYPE)											\
	template<>															\
	void Print(const char* pcName, _TYPE*const& pkVal, FvString& kMsg)	\
	{																	\
		if(pkVal == NULL)												\
		{																\
			FV_ASSERT(pcName);											\
			FvDataPrint::Print(kMsg, "%s[NULL]", pcName);				\
		}																\
		else															\
		{																\
			FvDataPrint::Print(pcName, *pkVal, kMsg);					\
		}																\
	}

	POINTER_PRINT(FvUInt8)
	POINTER_PRINT(FvUInt16)
	POINTER_PRINT(FvUInt32)
	POINTER_PRINT(FvUInt64)
	POINTER_PRINT(FvInt8)
	POINTER_PRINT(FvInt16)
	POINTER_PRINT(FvInt32)
	POINTER_PRINT(FvInt64)
	POINTER_PRINT(float)
	POINTER_PRINT(double)
	POINTER_PRINT(bool)
	POINTER_PRINT(FvString)
	POINTER_PRINT(FvVector2)
	POINTER_PRINT(FvVector3)
	POINTER_PRINT(FvVector4)



//////////////////////////////////////////////////////////////////////////////////////////////////////
#define SIMPLEDATA_PRINT_NONAME(_TYPE, _FORMAT)		\
	template<>										\
	void Print(const _TYPE& kVal, FvString& kMsg)	\
	{												\
		FvDataPrint::Print(kMsg, "%" _FORMAT, kVal);\
	}

	SIMPLEDATA_PRINT_NONAME(FvUInt8, "d")
	SIMPLEDATA_PRINT_NONAME(FvUInt16, "d")
	SIMPLEDATA_PRINT_NONAME(FvUInt32, "u")
	SIMPLEDATA_PRINT_NONAME(FvUInt64, "I64u")
	SIMPLEDATA_PRINT_NONAME(FvInt8, "d")
	SIMPLEDATA_PRINT_NONAME(FvInt16, "d")
	SIMPLEDATA_PRINT_NONAME(FvInt32, "d")
	SIMPLEDATA_PRINT_NONAME(FvInt64, "I64d")
	SIMPLEDATA_PRINT_NONAME(float, "f")
	SIMPLEDATA_PRINT_NONAME(double, "lf")


	template<>
	void Print(const bool& kVal, FvString& kMsg)
	{
		FvDataPrint::Print(kMsg, "%s", kVal ? "true" : "false");
	}

	template<>
	void Print(const FvString& kVal, FvString& kMsg)
	{
		FvDataPrint::Print(kMsg, "%s", kVal.c_str());
	}

	template<>
	void Print(const FvVector2& kVal, FvString& kMsg)
	{
		FvDataPrint::Print(kMsg, "[%f %f]", kVal.x, kVal.y);
	}

	template<>
	void Print(const FvVector3& kVal, FvString& kMsg)
	{
		FvDataPrint::Print(kMsg, "[%f %f %f]", kVal.x, kVal.y, kVal.z);
	}

	template<>
	void Print(const FvVector4& kVal, FvString& kMsg)
	{
		FvDataPrint::Print(kMsg, "[%f %f %f %f]", kVal.x, kVal.y, kVal.z, kVal.w);
	}


#define POINTER_PRINT_NONAME(_TYPE)					\
	template<>										\
	void Print(_TYPE*const& pkVal, FvString& kMsg)	\
	{												\
		if(pkVal == NULL)							\
		{											\
			kMsg.append("NULL");					\
		}											\
		else										\
		{											\
			FvDataPrint::Print(*pkVal, kMsg);		\
		}											\
	}

	POINTER_PRINT_NONAME(FvUInt8)
	POINTER_PRINT_NONAME(FvUInt16)
	POINTER_PRINT_NONAME(FvUInt32)
	POINTER_PRINT_NONAME(FvUInt64)
	POINTER_PRINT_NONAME(FvInt8)
	POINTER_PRINT_NONAME(FvInt16)
	POINTER_PRINT_NONAME(FvInt32)
	POINTER_PRINT_NONAME(FvInt64)
	POINTER_PRINT_NONAME(float)
	POINTER_PRINT_NONAME(double)
	POINTER_PRINT_NONAME(bool)
	POINTER_PRINT_NONAME(FvString)
	POINTER_PRINT_NONAME(FvVector2)
	POINTER_PRINT_NONAME(FvVector3)
	POINTER_PRINT_NONAME(FvVector4)


}

