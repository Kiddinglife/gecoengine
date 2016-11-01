
#ifndef __FvGameMessage_H__
#define __FvGameMessage_H__


#include "FvLogicCommonDefine.h"

#include "FvGameSerialize.h"
#include "FvOwnPtr.h"

#include <FvKernel.h>
#include <vector>
#include <FvMemoryStream.h>

//
////template<class FLAG_TYPE>
//typedef FvUInt8 FLAG_TYPE;
//
//class FV_LOGIC_COMMON_API FvRTTIBaseContent
//{
//protected:
//	FvRTTIBaseContent():m_eType(0){}
//
//public:
//	template <class T>
//	bool GetValue(T& kValue)const
//	{
//		typedef FvRTTIContent<T> Content;
//		if(m_eType == Content::TYPE())
//		{
//			const Content& kRTTI = static_cast<const Content&>(*this);
//			kValue = kRTTI.m_kT;
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	typedef FvRTTIBaseContent* (*pCreate)();
//	static FvRTTIBaseContent* Create(const FvUInt8 eType)
//	{
//		std::vector<pCreate>&kList = LIST();
//		if(eType < kList.size())
//		{
//			pCreate pFunc = kList[eType];
//			FvRTTIBaseContent* pkNew = (*pFunc)();
//			if(pkNew)
//			{
//				pkNew->m_eType = eType;
//			}
//			return pkNew;
//		}
//		else
//		{
//			FvLogBus::Error("FvRTTIBaseContent::Register越界");
//			return NULL;
//		}
//	}
//	template <class T>
//	static void Register()
//	{
//		typedef FvRTTIContent<T> Content;
//		std::vector<pCreate>&kList = LIST();
//		const FvUInt8 uiType = Content::TYPE();
//		if(uiType < kList.size())
//		{
//			kList[uiType] = Content::_Create;
//		}
//		else
//		{
//			FvLogBus::Error("FvRTTIBaseContent::Register越界");
//		}
//	}
//	static void Resize(const FvUInt8 uiSize)
//	{
//		std::vector<pCreate>&kList = LIST();
//		kList.resize(uiSize, &_CreateNULL);
//	}
//
//	virtual bool Init(FvBinaryIStream& kIS) = 0;
//	virtual void Print(std::string& kStr)const = 0;
//private:
//
//	static std::vector<pCreate>& LIST()
//	{
//		static std::vector<pCreate> S_LIST;
//		return S_LIST;
//	}
//
//	static FvRTTIBaseContent* _CreateNULL()
//	{
//		FvLogBus::Error("FvRTTIBaseContent::Type未初始化");
//		return NULL;
//	}
//
//	FvUInt8 m_eType;
//};
//
//template <class T>
//class FvRTTIContent: public FvRTTIBaseContent
//{
//public:
//	static FLAG_TYPE TYPE();
//	virtual bool Init(FvBinaryIStream& kIS)
//	{
//		T kT;
//		if(FvGameSerialize::OutputValue(kIS, kT) == false)
//		{
//			return false;
//		}
//		m_kT = kT;
//		return true;
//	}
//	virtual void Print(std::string& kStr)const
//	{
//		FvGameSerialize::Print<T>(m_kT, kStr);
//	}
//	static FvRTTIBaseContent* _Create()
//	{
//		FvRTTIContent* pkNew = new FvRTTIContent();
//		return pkNew;
//	}
//	T m_kT;
//};
//
//
//
//typedef FvRTTIBaseContent MessageBaseContent;
//
//class FV_LOGIC_COMMON_API FvGameMessageHelper
//{
//public:
//
//	template <class T>
//	static void AddType(FvUInt64& uiFomat, const FvUInt8 uiIdx)
//	{
//		FV_ASSERT(uiIdx < 8);
//		uiFomat += FvGameSerialize::GetType<T>() << uiIdx*8;
//	}
//
//	static void Print(const FvUInt64 uiFomat, FvBinaryIStream& kIS, std::vector<std::string>& kStrList);
//	static void Print(const FvUInt64 uiFomat, FvBinaryIStream& kIS, std::vector<FvOwnPtr<MessageBaseContent>>& kValueList);
//	static void Print(const std::vector<FvOwnPtr<MessageBaseContent>>& kValueList, std::vector<std::string>& kStrList);
//
//
//	template<class T>
//	static bool GetValue(const std::vector<FvOwnPtr<MessageBaseContent>>& kValueList, const FvUInt32 uiIdx, T& kValue)
//	{
//		if(uiIdx >= kValueList.size())
//		{
//			return false;
//		}
//		const FvOwnPtr<MessageBaseContent>& opValue = kValueList[uiIdx];
//		if(opValue.IsNULL())
//		{
//			return false;
//		}
//		return opValue->GetValue(kValue);
//	}
//
//private:
//
//
//	FvGameMessageHelper();
//	~FvGameMessageHelper();
//
//
//};
//
//
//class FvGameMessage
//{
//public:
//
//	static void Invoke(const FvUInt32 uiLogIdx, FvBinaryOStream& kOS)
//	{
//		FvUInt64 uiFomat = 0;
//		kOS << uiLogIdx << uiFomat;
//	}
//
//	template <class Param>
//	static void Invoke(const FvUInt32 uiLogIdx, FvBinaryOStream& kOS, const Param& kParam)
//	{
//		FvUInt64 uiFomat = 0;
//		FvGameMessageHelper::AddType<Param>(uiFomat, 0);
//		kOS << uiLogIdx << uiFomat;
//		FvGameSerialize::InputValue<Param>(kOS, kParam);
//	}
//
//	template <class Param1, class Param2>
//	static void Invoke(const FvUInt32 uiLogIdx, FvBinaryOStream& kOS, const Param1& kParam1, const Param2& kParam2)
//	{
//		FvUInt64 uiFomat = 0;
//		FvGameMessageHelper::AddType<Param1>(uiFomat, 0);
//		FvGameMessageHelper::AddType<Param2>(uiFomat, 1);
//		kOS << uiLogIdx << uiFomat;
//		FvGameSerialize::InputValue<Param1>(kOS, kParam1);
//		FvGameSerialize::InputValue<Param2>(kOS, kParam2);
//	}
//
//	template <class Param1, class Param2, class Param3>
//	static void Invoke(const FvUInt32 uiLogIdx, FvBinaryOStream& kOS, const Param1& kParam1, const Param2& kParam2, const Param3& kParam3)
//	{
//		FvUInt64 uiFomat = 0;
//		FvGameMessageHelper::AddType<Param1>(uiFomat, 0);
//		FvGameMessageHelper::AddType<Param2>(uiFomat, 1);
//		FvGameMessageHelper::AddType<Param3>(uiFomat, 2);
//		kOS << uiLogIdx << uiFomat;
//		FvGameSerialize::InputValue<Param1>(kOS, kParam1);
//		FvGameSerialize::InputValue<Param2>(kOS, kParam2);
//		FvGameSerialize::InputValue<Param3>(kOS, kParam3);
//	}
//
//	template <class Param1, class Param2, class Param3, class Param4>
//	static void Invoke(const FvUInt32 uiLogIdx, FvBinaryOStream& kOS, const Param1& kParam1, const Param2& kParam2, const Param3& kParam3, const Param4& kParam4)
//	{
//		FvUInt64 uiFomat = 0;
//		FvGameMessageHelper::AddType<Param1>(uiFomat, 0);
//		FvGameMessageHelper::AddType<Param2>(uiFomat, 1);
//		FvGameMessageHelper::AddType<Param3>(uiFomat, 2);
//		FvGameMessageHelper::AddType<Param4>(uiFomat, 3);
//		kOS << uiLogIdx << uiFomat;
//		FvGameSerialize::InputValue<Param1>(kOS, kParam1);
//		FvGameSerialize::InputValue<Param2>(kOS, kParam2);
//		FvGameSerialize::InputValue<Param3>(kOS, kParam3);
//		FvGameSerialize::InputValue<Param4>(kOS, kParam4);
//	}
//private:
//
//	FvGameMessage(void);
//	~FvGameMessage(void);
//
//};
//
//namespace GameMessage_Demo
//{
//
//	class LogContent_A
//	{
//	public:
//		FvUInt32 m_uiValue;
//	};
//	class LogContent_B
//	{
//	public:
//		FvUInt32 m_uiValue;
//	};
//	class LogContentType
//	{
//	public:
//		enum _Idx
//		{
//			NONE,
//			LOGCONTENT_A,
//			LOGCONTENT_B,
//		};
//	};
//	static void Test()
//	{
//		//FvGameSerialize::RegPrinter<LogContent_A>(LogContentType::LOGCONTENT_A);
//		//FvGameSerialize::RegPrinter<LogContent_B>(LogContentType::LOGCONTENT_B);
//
//		//FvGameMessage::Invoke<0>(LogContent_A());
//		//FvGameMessage::Invoke<1>(LogContent_A(), LogContent_A());
//		//FvGameMessage::Invoke<2>(LogContent_B());
//		//FvGameMessage::Invoke<3>(LogContent_A(), LogContent_B());
//	}
//
//
//	////+----------------------------------------------------------------------------------------------------
//	/////<TEST>
//	//using namespace GameMessage_Demo;
//	//_LOG_CONTENT_TYPE_(LogContent_A, LogContentType::LOGCONTENT_A);
//	//_LOG_CONTENT_TYPE_(LogContent_B, LogContentType::LOGCONTENT_B);
//
//	//template<>
//	//const std::string FvGameMessageHelper::Print<LogContent_A>(FvMemoryIStream&)
//	//{
//	//	std::string S_STR;
//	//	S_STR.clear();
//	//	S_STR.append("LogContent_A");
//	//	return S_STR;
//	//}
//	//template<>
//	//const std::string FvGameMessageHelper::Print<LogContent_B>(FvMemoryIStream&)
//	//{
//	//	std::string S_STR;
//	//	S_STR.clear();
//	//	S_STR.append("LogContent_B");
//	//	return S_STR;
//	//}
//
//
//}
//



#endif //__FvGameMessage_H__
