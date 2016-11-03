#include "FvValueMapping.h"

//+----------------------------------------------------------------------------------------------------

bool FvValueMapping::GetValue(const FvInt32 iDeltaValue, FvValueMappingList& kList, FvInt32& iNewDetalValue, FvUInt32& uiIdx)
{
	if(kList.Size() == 0)
	{
		return false;
	}
	FvValueMappingList::iterator iter = kList.GetHead();
	FV_ASSERT(iter);
	const FvValueMapping* pkMapping = (*iter).Content();
	FV_ASSERT(pkMapping);
	uiIdx = pkMapping->Idx();
	return pkMapping->Value(iDeltaValue);
}

//+----------------------------------------------------------------------------------------------------


FvValueMapping::FvValueMapping(void)
:m_uiDestIdx(0)
,m_fScale(1.0f)
{
	FV_MEM_TRACK;
}

FvValueMapping::~FvValueMapping(void)
{
}
//+----------------------------------------------------------------------------------------------------

FvInt32 FvValueMapping::Value(const FvInt32 iDeltaValue)const
{
	return FvInt32(iDeltaValue*m_fScale);
}
FvInt32 FvValueMapping::RevertValue(const FvInt32 iDeltaValue)const
{
	FV_ASSERT(m_fScale != 0.0f);
	return FvInt32(iDeltaValue/m_fScale);
}
float FvValueMapping::Scale()const
{
	return m_fScale;
}
FvUInt32 FvValueMapping::Idx()const
{
	return m_uiDestIdx;
}
void FvValueMapping::Init(const FvUInt32 uiDestIdx, const float fScale)
{
	m_fScale = fScale;
	if(m_fScale == 0.0f)
	{
		m_fScale = 1.0f;
	}
	m_uiDestIdx = uiDestIdx;
}