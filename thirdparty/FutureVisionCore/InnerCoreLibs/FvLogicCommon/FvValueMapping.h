//{future header message}
#ifndef __FvValueMapping_H__
#define __FvValueMapping_H__

#include "FvLogicCommonDefine.h"
#include "FvMemoryNode.h"
#include <FvRefList.h>

class FvValueMapping;
typedef FvRefList<FvValueMapping*, FvRefNode1> FvValueMappingList;

class FV_LOGIC_COMMON_API FvValueMapping: public FvRefNode1<FvValueMapping*>
{
	FV_MEM_TRACKER(FvValueMapping);
public:
	FvValueMapping(void);
	~FvValueMapping(void);

	static bool GetValue(const FvInt32 iDeltaValue, FvValueMappingList& kList, FvInt32& iNewDetalValue, FvUInt32& uiIdx);

	FvInt32 Value(const FvInt32 iDeltaValue)const;
	FvInt32 RevertValue(const FvInt32 iDeltaValue)const;
	FvUInt32 Idx()const;
	float Scale()const;

	void Init(const FvUInt32 uiDestIdx, const float fScale);

private:

	FvUInt32 m_uiDestIdx;
	float m_fScale;

};


namespace ValueMapping_Demo
{
	static void Test()
	{
		FvValueMappingList kList;
		FvValueMapping kFvValueMapping_0;	kFvValueMapping_0.Init(0, 1.0f); kList.AttachBack(kFvValueMapping_0);
		FvValueMapping kFvValueMapping_1;	kFvValueMapping_1.Init(1, 2.0f); kList.AttachBack(kFvValueMapping_1);
		FvValueMapping kFvValueMapping_2;	kFvValueMapping_2.Init(2, 0.5f); kList.AttachBack(kFvValueMapping_2);

		FvInt32 iDeltaValue = -100;
		FvInt32 iDeltaValueReserve = iDeltaValue;
		FvInt32 uiIdx = 0;

		kList.BeginIterator();
		while (!kList.IsEnd())
		{
			FvValueMappingList::iterator iter = kList.GetIterator();
			FvValueMapping* pkMapping = static_cast<FvValueMapping*>(iter);
			kList.Next();
			FV_ASSERT(pkMapping);
			const FvInt32 iMappingDeltaValue = pkMapping->Value(iDeltaValueReserve);
			const FvInt32 iModLimit = -30;
			const FvInt32 iMappingDeltaValueMod = (iMappingDeltaValue > iModLimit)?iMappingDeltaValue:iModLimit;
			iDeltaValueReserve -= pkMapping->RevertValue(iMappingDeltaValueMod);
			if(pkMapping->Value(iDeltaValueReserve) == 0)
			{
				iDeltaValueReserve = 0;
				kList.EndIterator();
				break;
			}
		}

		const FvInt32 iDeltaValueMod = iDeltaValue - iDeltaValueReserve;

	}
}


#endif //__FvValueMapping_H__
