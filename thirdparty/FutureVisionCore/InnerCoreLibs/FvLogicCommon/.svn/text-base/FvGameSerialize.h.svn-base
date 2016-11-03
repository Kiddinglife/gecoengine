
#ifndef __FvGameSerialize_H__
#define __FvGameSerialize_H__


//#include "FvLogicCommonDefine.h"
//#include "FvKeyVector.h"
//#include <FvKernel.h>
//#include <FvDebug.h>
//#include <string>
//#include <map>
//#include "FvRefList.h"
//#include <vector>
//#include <list>
//
//class FvBinaryOStream;
//class FvBinaryIStream;
//
//#define _CHECK_IS_SIZE_(kIS, TYPE)	if(kIS.RemainingLength() < sizeof(TYPE)){return false;}
//
//template<class ResultType>class RPCResultHooker;
//
//
//class FV_LOGIC_COMMON_API FvGameSerialize
//{
//public:
//
//	//typedef void (*pStrPrinter)(FvBinaryIStream&, std::string&);
//	//typedef void (*pValuePrinter)(FvBinaryIStream&, FvRTTIBaseContent<FvUInt8>&);
//
//	//static pStrPrinter GetStrPrinter(const FvUInt8 uiIdx);
//	//static pValuePrinter GetValuePrinter(const FvUInt8 uiIdx);
//
//	template <class T>
//	static FvUInt8 GetType();
//
//	template<class T>
//	static T GetValue(const char* pcValue);
//
//	//+----------------------------------------------------------------------------------------------------
//	template <class T>
//	static void InputValue(FvBinaryOStream& kOS, const T& kT);
//
//	template <class T>
//	static void InputValue(FvBinaryOStream& kOS, const RPCResultHooker<T>& kPack)
//	{
//		kPack.InputValue(kOS);
//	}
//	template<class TFirst, class TSecond>
//	static void InputValue(FvBinaryOStream& kOS, const FvPair<TFirst, TSecond>& kPair)
//	{
//		InputValue(kOS, kPair._first);
//		InputValue(kOS, kPair._second);
//	}
//
//	template <class T>
//	static void InputValue(FvBinaryOStream& kOS, const std::vector<T>& kArray)
//	{
//		const FvUInt32 max = (FvUInt32)kArray.size();
//		kOS << max;
//		for (std::vector<T>::const_iterator iter = kArray.begin(); iter != kArray.end(); ++iter)
//		{
//			const T& kT = (*iter);
//			InputValue(kOS, kT);
//		}
//	}
//
//	template <class T>
//	static void InputValue(FvBinaryOStream& kOS, const std::list<T>& kArray)
//	{
//		const FvUInt32 max = (FvUInt32)kArray.size();
//		kOS << max;
//		for (std::vector<T>::const_iterator iter = kArray.begin(); iter != kArray.end(); ++iter)
//		{
//			const T& kT = (*iter);
//			InputValue(kOS, kT);
//		}
//	}
//
//	template <class Key, class T>
//	static void InputValue(FvBinaryOStream& kOS, const std::map<Key, T>& kT);
//
//	//+----------------------------------------------------------------------------------------------------
//	template <class T>
//	static bool OutputValue(FvBinaryIStream& kIS, T& kT);
//
//	template <class T>
//	static bool OutputValue(FvBinaryIStream& kIS, RPCResultHooker<T>& kPack)
//	{
//		return kPack.OutputValue(kIS);
//	}
//
//	template<class TFirst, class TSecond>
//	static bool OutputValue(FvBinaryIStream& kIS, FvPair<TFirst, TSecond>& kPair)
//	{
//		if(OutputValue(kIS, kPair._first) == false)
//		{
//			return false;
//		}
//		if(OutputValue(kIS, kPair._second) == false)
//		{
//			return false;
//		}
//		return true;
//	}
//
//	template <class T>
//	static bool OutputValue(FvBinaryIStream& kIS, std::vector<T>& kArray)
//	{
//		kArray.clear();
//		_CHECK_IS_SIZE_(kIS, FvUInt32);
//		FvUInt32 max = 0;
//		kIS >> max;
//		kArray.reserve(max);
//		T kValue;
//		for(FvUInt32 i=0; i < max; i++)
//		{
//			if(OutputValue(kIS, kValue) == false)
//			{
//				kArray.clear();
//				return false;
//			}
//			kArray.push_back(kValue);
//		}
//		return true;
//	}
//
//	template <class T>
//	static bool OutputValue(FvBinaryIStream& kIS, std::list<T>& kArray)
//	{
//		kArray.clear();
//		_CHECK_IS_SIZE_(kIS, FvUInt32);
//		FvUInt32 max = 0;
//		kIS >> max;
//		kArray.reserve(max);
//		T kValue;
//		for(FvUInt32 i=0; i < max; i++)
//		{
//			if(OutputValue(kIS, kValue) == false)
//			{
//				kArray.clear();
//				return false;
//			}
//			kArray.push_back(kValue);
//		}
//		return true;
//	}
//
//	template <class Key, class T>
//	static bool OutputValue(FvBinaryIStream& kIS, std::map<Key, T>& kT);
//
//	//+---------------------------------------------------------------------------------------------------
//
//	//template <class T>
//	//static void RegPrinter(const FvUInt8 uiIdx)
//	//{
//	//	std::map<FvUInt8, pValuePrinter>::const_iterator iter = ms_printList.find(uiIdx);
//	//	if(iter != ms_printList.end())
//	//	{
//	//		FV_ASSERT(0 && "∑¢œ÷÷ÿ∏¥◊¢≤·");
//	//	}
//	//	else
//	//	{
//	//		ms_printList[uiIdx] = &FvGameSerialize::Print<T>;
//	//	}
//	//}
//
//	template <class T>
//	static const char* Print();
//
//	template <class T>
//	static void Print(FvBinaryIStream& kIS, std::string& kStr);
//
//	//+----------------------------------------------------------------------------------------------------
//	template <class T>
//	static void Print(const T& kT, std::string& kStr);
//	template <class T>
//	static void Print(const T* pkT, std::string& kStr)
//	{
//		if(pkT == NULL)
//		{
//			kStr.append("NULL");
//		}
//		else
//		{
//			Print(*pkT, kStr);
//		}
//	}
//
//	template <class T0, class T1>
//	static void Print(const T0& kT0, const T1& kT1, std::string& kStr)
//	{
//		Print(kT0, kStr);
//		kStr.append(", ");
//		Print(kT1, kStr);
//	}
//	template <class T0, class T1, class T2>
//	static void Print(const T0& kT0, const T1& kT1, const T2& kT2, std::string& kStr)
//	{
//		Print(kT0, kT1, kStr);
//		kStr.append(", ");
//		Print(kT2, kStr);
//	}
//	template <class T0, class T1, class T2, class T3>
//	static void Print(const T0& kT0, const T1& kT1, const T2& kT2, const T3& kT3, std::string& kStr)
//	{
//		Print(kT0, kT1, kT2, kStr);
//		kStr.append(", ");
//		Print(kT3, kStr);
//	}
//	template <class T0, class T1, class T2, class T3, class T4>
//	static void Print(const T0& kT0, const T1& kT1, const T2& kT2, const T3& kT3, const T4& kT4, std::string& kStr)
//	{
//		Print(kT0, kT1, kT2, kT3, kStr);
//		kStr.append(", ");
//		Print(kT4, kStr);
//	}
//	template <class T>
//	static void Print(const RPCResultHooker<T>& kPack, std::string& kStr)
//	{
//		kStr.append("RPCResultHooker");
//	}
//	template<class TFirst, class TSecond>
//	static void Print(const FvPair<TFirst, TSecond>& kPair, std::string& kStr)
//	{
//		kStr.append("<");
//		Print(kPair._first, kStr);
//		kStr.append(", ");
//		Print(kPair._second, kStr);
//		kStr.append(">");
//	}
//
//	template <class T>
//	static void Print(const std::vector<T>& kTList, std::string& kStr)
//	{
//		if(kTList.size() == 0)
//		{
//			kStr.append("vector<ø’>");
//		}
//		else
//		{
//			kStr.append("vector<");
//			std::vector<T>::const_iterator iter = kTList.begin();
//			while(true)
//			{
//				const T& kT = (*iter);
//				Print(kT, kStr);
//				++iter;
//				if(iter != kTList.end())
//				{
//					kStr.append(", ");
//				}
//				else
//				{
//					break;
//				}
//			}
//			kStr.append(">");
//		}		
//	}
//	template <class T>
//	static void Print(const std::list<T>& kTList, std::string& kStr)
//	{
//		if(kTList.size() == 0)
//		{
//			kStr.append("list<ø’>");
//		}
//		else
//		{
//			kStr.append("list<");
//			std::list<T>::const_iterator iter = kTList.begin();
//			while(true)
//			{
//				const T& kT = (*iter);
//				Print(kT, kStr);
//				++iter;
//				if(iter != kTList.end())
//				{
//					kStr.append(", ");
//				}
//				else
//				{
//					break;
//				}
//			}
//			kStr.append(">");
//		}		
//	}
//	//+----------------------------------------------------------------------------------------------------
//	template<class From, class To>
//	static const To* Transform(const From& kFrom);
//	template<class From, class To>
//	static const To* Transform(const From* pkFrom)
//	{
//		if(pkFrom == NULL)
//			return NULL;
//		else 
//			return Transform<From, To>(*pkFrom);
//	}
//	//+----------------------------------------------------------------------------------------------------
//
//
//private:
//
//	FvGameSerialize();
//	~FvGameSerialize();
//
//
//	//static std::map<FvUInt8, pValuePrinter> ms_printList;
//
//};
//
//
//#define _SERIALIZE_TYPE_(IDX, TYPE) \
//	template <> FvUInt8 FvGameSerialize::GetType<TYPE>()\
//{\
//	return IDX;\
//}\
//	template <> const char* FvGameSerialize::Print<TYPE>()\
//{\
//	return #TYPE;\
//}
//
//#define _SERIALIZE_PTR_(IDX, CLASS) \
//	class CLASS;\
//	template <> FvUInt8 FvGameSerialize::GetType<CLASS*>()\
//{\
//	return IDX;\
//}\
//	template <> const char* FvGameSerialize::Print<CLASS>()\
//{\
//	return #CLASS;\
//}
//
//#define _SERIALIZE_PRINT_(CLASS) \
//class CLASS;\
//	template <> const char* FvGameSerialize::Print<CLASS>()\
//{\
//	return #CLASS;\
//}
//
//#define _MEM_SERIALIZE_(TYPE)\
//	template<>\
//	void FvGameSerialize::InputValue<TYPE>(FvBinaryOStream & b, const TYPE& fValue)\
//{\
//	b << fValue;\
//}\
//	template<>\
//	bool FvGameSerialize::OutputValue<TYPE>(FvBinaryIStream & b, TYPE& fValue)\
//{\
//	_CHECK_IS_SIZE_(b, TYPE);\
//	b >> fValue;\
//	return true;\
//}
//
//#define _INT_PRINT_(TYPE)\
//template<>\
//void FvGameSerialize::Print<TYPE>(const TYPE& kT, std::string& kStr)\
//{\
//	static char S_STR[1024] = {0};\
//	sprintf_s(S_STR, sizeof(S_STR), "%d", FvInt32(kT));\
//	kStr.append(S_STR);\
//}
//
//#define _FLOAT_PRINT_(TYPE)\
//template<>\
//void FvGameSerialize::Print<TYPE>(const TYPE& kT, std::string& kStr)\
//{\
//	static char S_STR[1024] = {0};\
//	sprintf_s(S_STR, sizeof(S_STR), "%f", float(kT));\
//	kStr.append(S_STR);\
//}
//


//#define _MEM_SERIALIZE_(TYPE, IDX)\
//	_SERIALIZE_TYPE_(TYPE, IDX)\
//	template<>\
//	void FvGameSerialize::InputValue<TYPE>(FvBinaryOStream & b, const TYPE& fValue)\
//{\
//	b << fValue;\
//}\
//	template<>\
//	void FvGameSerialize::OutputValue<TYPE>(FvBinaryIStream & b, TYPE& fValue)\
//{\
//	b >> fValue;\
//}

#endif //__FvGameSerialize_H__