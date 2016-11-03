#include "FvMemoryNode.h"

//FvMemoryNode::_List FvMemoryNode::s_List;
//
//
//FvMemoryNode::FvMemoryNode()
//:_cnt(0)
//,_inited(false)
//{
//	_node.m_Content = this;
//	_name.append("VI_MEM_LEAK: ");
//}
//
//FvMemoryNode::~FvMemoryNode(void)
//{
//	FV_ASSERT(RefCnt() == 0 && "ÄÚ´æÐ¹Â¶ÁË >> " && typeid(*this).name());
//}
//
//void FvMemoryNode::Attach()
//{
//	s_List.AttachBack(this->_node);
//}
//bool FvMemoryNode::IsAttach()const
//{
//	return _node.IsAttach();
//}
//void FvMemoryNode::Print()
//{
//	s_List.BeginIterator();
//	while (!s_List.IsEnd())
//	{
//		_List::iterator iter = s_List.GetIterator();
//		s_List.Next();
//		//ViLogBus::Warning("%s  [%d]", iter->m_Content->_name.c_str(), iter->m_Content->_cnt);
//	}
//}
//void FvMemoryNode::SetName(const char* name)
//{
//	if(!_inited && name)
//	{
//		_name.append(name);
//		_inited = true;
//	}
//}
//
//void FvMemoryNode::AddRef()
//{
//	++_cnt;
//}
//void FvMemoryNode::DelRef()
//{
//	--_cnt;
//}
//FvInt32 FvMemoryNode::RefCnt()const
//{
//	return _cnt;
//}
//

//+----------------------------------------------------------------------------------------------------

FvMemoryNode::FvMemoryNode()
:m_iCnt(0)
{
	S_List().AttachBack(*this);
}
FvMemoryNode::~FvMemoryNode()
{}
void FvMemoryNode::AddRef()
{
	++m_iCnt;
}
void FvMemoryNode::DelRef()
{
	--m_iCnt;
}
FvInt32 FvMemoryNode::RefCnt()const
{
	return m_iCnt;
}
void FvMemoryNode::Print()
{
	FvRefListNameSpace<FvMemoryNode>::List1& kLIst= S_List();
	FvRefListNameSpace<FvMemoryNode>::List1::iterator iter = kLIst.GetHead();
	while (!kLIst.IsEnd(iter))
	{
		FvMemoryNode* pkThis = static_cast<FvMemoryNode*>(iter);
		FvRefListNameSpace<FvMemoryNode>::List1::Next(iter);		
		FV_ASSERT(pkThis);
		pkThis->_Print();
	}
}


