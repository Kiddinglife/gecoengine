#include "FvGameSerialize.h"

#include "FvLogBus.h"

//
//std::map<FvUInt8, FvGameSerialize::pValuePrinter> FvGameSerialize::ms_printList;
//
//FvGameSerialize::pValuePrinter FvGameSerialize::GetValuePrinter(const FvUInt8 uiIdx)
//{
//	std::map<FvUInt8, pValuePrinter>::const_iterator iter = ms_printList.find(uiIdx);
//	if(iter != ms_printList.end())
//	{
//		pValuePrinter pFunc = (*iter).second;
//		return pFunc;
//	}
//	else
//	{
//		FvLogBus::Error("FvGameSerialize::GetPrinter(%d) ›]ÓÐÖ¸¶¨º¯”µ", uiIdx);
//		return NULL;
//	}
//}
//
//#define _INT
//template <class T0>
//static void Print(const T0& kT0, std::string& kStr);