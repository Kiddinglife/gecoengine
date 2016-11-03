//{future header message}
#ifndef __FvDataPrint_H__
#define __FvDataPrint_H__

#include <FvBaseTypes.h>
#include <vector>
#include <FvVector4.h>
#include "FvEntityDefDefines.h"


namespace FvDataPrint
{
	FV_ENTITYDEF_API void Print(FvString& kMsg, const char *pcFormat, ...);


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	void Print(const char* pcName, const T& kVal, FvString& kMsg);

	template<class T>
	void Print(const char* pcName, const std::vector<T>& kVec, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		std::vector<T>::size_type uiSize = kVec.size();
		if(uiSize == 0)
		{
			FvDataPrint::Print(kMsg, "%s<Пе>", pcName);
		}
		else
		{
			FvDataPrint::Print(kMsg, "%s<", pcName);
			const T& kVal = kVec[0];
			FvDataPrint::Print("", kVal, kMsg);
			for(std::vector<T>::size_type i(1); i<uiSize; ++i)
			{
				const T& kVal = kVec[i];
				FvDataPrint::Print(", ", kVal, kMsg);
			}
			kMsg.append(">");
		}
	}

	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvUInt8& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvUInt16& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvUInt32& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvUInt64& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvInt8& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvInt16& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvInt32& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvInt64& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const float& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const double& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const bool& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvString& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvVector2& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvVector3& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvVector4& kVal, FvString& kMsg);

	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvUInt8*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvUInt16*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvUInt32*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvUInt64*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvInt8*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvInt16*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvInt32*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvInt64*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, float*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, double*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, bool*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvString*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvVector2*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvVector3*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, FvVector4*const& pkVal, FvString& kMsg);



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	void Print(const T& kVal, FvString& kMsg);

	template<class T>
	void Print(const std::vector<T>& kVec, FvString& kMsg)
	{
		std::vector<T>::size_type uiSize = kVec.size();
		if(uiSize == 0)
		{
			kMsg.append("<>");
		}
		else
		{
			kMsg.append("<");
			const T& kVal = kVec[0];
			FvDataPrint::Print(kVal, kMsg);
			for(std::vector<T>::size_type i(1); i<uiSize; ++i)
			{
				kMsg.append(", ");
				const T& kVal = kVec[i];
				FvDataPrint::Print(kVal, kMsg);
			}
			kMsg.append(">");
		}
	}

	template<>
	FV_ENTITYDEF_API void Print(const FvUInt8& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvUInt16& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvUInt32& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvUInt64& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvInt8& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvInt16& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvInt32& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvInt64& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const float& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const double& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const bool& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvString& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvVector2& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvVector3& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvVector4& kVal, FvString& kMsg);

	template<>
	FV_ENTITYDEF_API void Print(FvUInt8*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvUInt16*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvUInt32*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvUInt64*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvInt8*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvInt16*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvInt32*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvInt64*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(float*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(double*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(bool*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvString*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvVector2*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvVector3*const& pkVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(FvVector4*const& pkVal, FvString& kMsg);

}


class FV_ENTITYDEF_API FvDataPrinter
{
public:
	FvDataPrinter();

	static FvDataPrinter& Instance() { return *ms_pkInstance; }

	FvDataPrinter& PrintRaw(const char *pcFormat, ...);

	template<class T>
	FvDataPrinter& Print(const char* pcName, const T& kVal)
	{
		FvDataPrint::Print(pcName, kVal, m_kMsg);
		return *this;
	}

	void Output();

protected:
	FvString	m_kMsg;

protected:
	static FvDataPrinter*	ms_pkInstance;
};


#endif//__FvDataPrint_H__