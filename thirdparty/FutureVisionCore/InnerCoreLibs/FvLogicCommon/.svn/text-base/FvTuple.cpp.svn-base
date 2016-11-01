#include "FvTuple.h"

//+-------------------------------------------------------------------------------------------------------------------------------
FvDynamicTuple::FvDynamicTuple(const FvDynamicTuple& kOther)
{
	Append(kOther);
}
//+-------------------------------------------------------------------------------------------------------------------------------
FvDynamicTuple::FvDynamicTuple(const FvBaseTuple& kOther)
{
	Append(kOther);
}
//+-------------------------------------------------------------------------------------------------------------------------------
FvDynamicTuple::~FvDynamicTuple()
{
	Clear();
}
void FvDynamicTuple::operator=(const FvDynamicTuple& kOther)
{
	Clear();
	Append(kOther);
}
void FvDynamicTuple::operator=(const FvBaseTuple& kOther)
{
	Clear();
	Append(kOther);
}
void FvDynamicTuple::Append(const FvDynamicTuple& kOther)
{
	for (std::vector<FvTupleBaseNode*>::const_iterator iter = kOther.m_kList.begin(); iter != kOther.m_kList.end(); ++iter)
	{
		const FvTupleBaseNode* pkNode = (*iter);
		FV_ASSERT(pkNode);
		FvTupleBaseNode* pkNewNode = CreateNode(pkNode->Type());
		FV_ASSERT(pkNewNode);
		pkNewNode->CopyFrom(*pkNode);
		m_kList.push_back(pkNewNode);		
	}
}
void FvDynamicTuple::Append(const FvBaseTuple& kOther)
{
	for(FvUInt32 uiIdx = 0; uiIdx < kOther.Size(); ++uiIdx)
	{
		const FvTupleBaseNode* pkNode = kOther._GetNode(uiIdx);
		FV_ASSERT(pkNode);
		FvTupleBaseNode* pkNewNode = CreateNode(pkNode->Type());
		FV_ASSERT(pkNewNode);
		pkNewNode->CopyFrom(*pkNode);
		m_kList.push_back(pkNewNode);		
	}
}
FvUInt32 FvDynamicTuple::Size()const
{
	return m_kList.size();
}
const FvTupleBaseNode* FvDynamicTuple::_GetNode(const FvUInt32 uiIdx)const
{
	if(uiIdx >= m_kList.size())
	{
		return NULL;
	}
	return m_kList[uiIdx];
}
FvTupleBaseNode* FvDynamicTuple::_GetNode(const FvUInt32 uiIdx)
{
	if(uiIdx >= m_kList.size())
	{
		return NULL;
	}
	return m_kList[uiIdx];
}
FvTupleBaseNode* FvDynamicTuple::CreateNode(const FvUInt32 uiType)
{
	_List& kList = S_List();
	if(uiType < kList.size())
	{
		pCreateFunc pkFunc = kList[uiType];
		FV_ASSERT(pkFunc);
		return (*pkFunc)();
	}
	else
		return NULL;
}
void FvDynamicTuple::SetRegisterTypeCnt(const FvUInt32 uiSize)
{
	_List& kList = S_List();
	kList.resize(uiSize, CreateNULL);
}
void FvDynamicTuple::Clear()
{
	for(std::vector<FvTupleBaseNode*>::const_iterator iter = m_kList.begin(); iter != m_kList.end(); ++iter)
	{
		FvTupleBaseNode* pkNode = (*iter);
		FV_ASSERT_ERROR(pkNode);
		delete pkNode;
	}
	m_kList.clear();
}

void FvDynamicTuple::PrintTo(std::vector<std::string>& kStrList)const
{
	for (std::vector<FvTupleBaseNode*>::const_iterator iter = m_kList.begin(); iter != m_kList.end(); ++iter)
	{
		const FvTupleBaseNode* pkNode = (*iter);
		FV_ASSERT(pkNode);
		kStrList.push_back("");
		pkNode->PrintTo(kStrList.back());
	}
}
FvTupleBaseNode* FvDynamicTuple::CreateNULL()
{
	FV_ASSERT_WARNING(0 && "FvDynamicTuple::CreateNULL");
	return NULL;
}
FvDynamicTuple::_List& FvDynamicTuple::S_List()
{
	static _List S_LIST;
	return S_LIST;
}
FvTupleBaseNode* FvDynamicTuple::AttachNode(const FvUInt32 uiType)
{
	FvTupleBaseNode* pkNode = CreateNode(uiType);
	if(pkNode)
	{
		m_kList.push_back(pkNode);
	}
	return pkNode;
}