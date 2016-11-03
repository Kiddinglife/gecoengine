#include "FvGameMessage.h"
//
//#include "FvLogBus.h"
////+----------------------------------------------------------------------------------------------------
//void FvGameMessageHelper::Print(const FvUInt64 uiFomat, FvBinaryIStream& kIS, std::vector<FvOwnPtr<MessageBaseContent>>& kValueList)
//{
//	for (FvUInt8 uiIdx = 0; uiIdx < 8; ++uiIdx)
//	{
//		const FvUInt8 uiType = FvUInt8(uiFomat >> uiIdx*8);
//		if(uiType == 0)
//		{
//			break;
//		}
//		MessageBaseContent* pkContent = MessageBaseContent::Create(uiType);
//		kValueList.push_back(pkContent);
//		if(pkContent)
//		{
//			pkContent->Init(kIS);
//		}
//		else
//		{
//			FvLogBus::Warning("发现无法识别的参数");
//		}
//	}
//}
//void FvGameMessageHelper::Print(const std::vector<FvOwnPtr<MessageBaseContent>>& kValueList, std::vector<std::string>& kStrList)
//{
//	for (std::vector<FvOwnPtr<MessageBaseContent>>::const_iterator iter = kValueList.begin(); iter != kValueList.end(); ++iter)
//	{
//		const FvOwnPtr<MessageBaseContent>& opContent = (*iter);
//		if(opContent.IsNULL())
//		{
//			kStrList.push_back(std::string("NONE"));
//		}
//		else
//		{
//			static std::string kStr;
//			kStr.clear();
//			opContent->Print(kStr);
//			kStrList.push_back(kStr);
//		}
//	}
//}
//void FvGameMessageHelper::Print(const FvUInt64 uiFomat, FvBinaryIStream& kIS, std::vector<std::string>& kStrList)
//{
//	static std::vector<FvOwnPtr<MessageBaseContent>> kValueList;
//	kValueList.clear();
//	Print(uiFomat, kIS, kValueList);
//	Print(kValueList, kStrList);
//}
