
//+------------------------------------------------------------------------------------------------------------------------
template <class T>
FvRefNode1<T>* FvRefNode1<T>::GetNextContent()const
{
	if(m_rpRefList == NULL)
	{
		return NULL;
	}
	if(m_rpRefList->IsEnd(Next()))
	{
		return NULL;
	}
	else
	{
		return Next();
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T>
void FvRefNode1<T>::Detach()
{
	if(m_rpRefList)
	{
		m_rpRefList->_Detach(*this);
		m_rpRefList = NULL;
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T>
void FvRefNode2<T>::Detach()
{
	if(m_rpRefList)
	{
		m_rpRefList->_Detach(*this);
		m_rpRefList = NULL;
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T>
const T&  FvRefNode2<T>::GetNextContent(const T& kDefaultValue)const
{
	if(m_rpRefList == NULL)
	{
		return kDefaultValue;
	}
	if(m_rpRefList->IsEnd(Next()))
	{
		return kDefaultValue;
	}
	else
	{
		return Next()->m_Content;
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T>
T&  FvRefNode2<T>::GetNextContent(T& kDefaultValue)
{
	if(m_rpRefList == NULL)
	{
		return kDefaultValue;
	}
	if(m_rpRefList->IsEnd(Next()))
	{
		return kDefaultValue;
	}
	else
	{
		return Next()->m_Content;
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
FvRefList<T, TNode>::FvRefList()
:m_rpNext(NULL)
,m_uiSize(0)
{
	m_kRoot.m_rpRefList = this;
	_Init();
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachBack(Node & kNode)
{
	kNode.Detach();
	++m_uiSize;
	kNode.m_rpRefList = this;
	_PushBefore(_Root(), kNode);
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachBackSafeIterator(Node & kNode)
{
	kNode.Detach();
	++m_uiSize;
	kNode.m_rpRefList = this;
	_PushBefore(_Root(), kNode);
	if(m_rpNext == &_Root())
	{
		m_rpNext = &kNode;
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachFront(Node & kNode)
{
	kNode.Detach();
	++m_uiSize;
	kNode.m_rpRefList = this;
	_PushAfter(_Root(), kNode);
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachBack(FvRefList & kList)
{
	FV_ASSERT(&kList != this);
	if(&kList == this)
	{
		return;
	}
	AttachBefore(_Root(), kList);
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachFront(FvRefList & kList)
{
	FV_ASSERT(&kList != this);
	if(&kList == this)
	{
		return;
	}
	AttachAfter(_Root(), kList);
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachAfter(Node & kFront, Node & kNew)
{
	kNew.Detach();
	++m_uiSize;
	kNew.m_rpRefList = this;
	_PushAfter(kFront, kNew);
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachBefore(Node & kAfter, Node & kNew)
{
	kNew.Detach();
	++m_uiSize;
	kNew.m_rpRefList = this;
	_PushBefore(kAfter, kNew);
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachAfter(Node & kFront, FvRefList & kList)
{
	if(kList.Size() == 0)
	{
		return;
	}
	if(kFront.IsAttach(kList))
	{
		return;
	}
	if(kFront.IsAttach() == false)
	{
		return;
	}
	FV_ASSERT(kFront.m_rpRefList);
	FvRefList& kReceiveList = *kFront.m_rpRefList;
	iterator iter = kList._Root().m_rpNextNode;
	while(iter != &kList._Root())
	{
		iter->m_rpRefList = &kReceiveList;
		iter = iter->m_rpNextNode;
	}
	FV_ASSERT(&kReceiveList != &kList);
	iterator pkFirst = kList._Root().m_rpNextNode;
	iterator pkBack = kList._Root().m_rpPreNode;
	iterator pkNext = kFront.m_rpNextNode;
	_Link(kFront, *pkFirst);
	_Link(*pkBack, *pkNext);	
	kReceiveList.m_uiSize += kList.m_uiSize;
	kList._Init();
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::AttachBefore(Node & kAfter, FvRefList & kList)
{
	if(kList.Size() == 0)
	{
		return;
	}
	if(kAfter.IsAttach(kList))
	{
		return;
	}
	if(kAfter.IsAttach() == false)
	{
		return;
	}
	FV_ASSERT(kAfter.m_rpRefList);
	FvRefList& kReceiveList = *kAfter.m_rpRefList;
	iterator iter = kList._Root().m_rpNextNode;
	while(iter != &kList._Root())
	{
		iter->m_rpRefList = &kReceiveList;
		iter = iter->m_rpNextNode;
	}
	FV_ASSERT(&kReceiveList != &kList);
	iterator pkFirst = kList._Root().m_rpNextNode;
	iterator pkBack = kList._Root().m_rpPreNode;
	iterator pkPre = kAfter.m_rpPreNode;
	_Link(*pkPre, *pkFirst);
	_Link(*pkBack, kAfter);
	kReceiveList.m_uiSize += kList.m_uiSize;
	kList._Init();
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::BeginIterator()const
{
	if(!IsEnd())
	{
		//ViLogBus::Warning("当前有迭代循环递归问题");
		FV_ASSERT(0 && "当前有迭代循环递归问题");
	}
	m_rpNext = _Root().m_rpNextNode;
	FV_ASSERT(m_rpNext);
}
//+------------------------------------------------------------------------------------------------------------------------
//+------------------------------------------------------------------------------------------------------------------------
//template <class T, template<class> class TNode>
//bool FvRefList<T, TNode>::Get(const FvUInt32 uiIdx, T& kT)const
//{
//	FvUInt32 uiIdx_ = uiIdx;
//	iterator pCurrent = _Root().m_rpNextNode;
//	while (!IsEnd(pCurrent))
//	{
//		if(uiIdx_ == 0)
//		{
//			kT = pCurrent->m_Content;
//			return true;
//		}
//		pCurrent =  pCurrent->m_rpNextNode;
//		--uiIdx_;
//	}
//	return false;
//}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::Clear()
{
	iterator pCurrent = _Root().m_rpNextNode;
	while (!IsEnd(pCurrent))
	{
		iterator pNext = pCurrent->m_rpNextNode;
		pCurrent->m_rpRefList = NULL;
		pCurrent->m_rpNextNode = NULL;
		pCurrent->m_rpPreNode = NULL;
		pCurrent = pNext;
	}
	_Init();
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::_DestroyContent()
{
	iterator pCurrent = _Root().m_rpNextNode;
	while (pCurrent != &_Root())
	{
		T pContent = pCurrent->m_Content;
		pCurrent->m_Content = NULL;
		pCurrent = pCurrent->m_rpNextNode;
		delete pContent;
	}
	_Init();
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::_Destroy()
{
	iterator pCurrent = _Root().m_rpNextNode;
	while (pCurrent != &_Root())
	{
		iterator pNext = pCurrent->m_rpNextNode;
		pCurrent->m_rpRefList = NULL;
		delete pCurrent;
		pCurrent = pNext;
	}
	_Init();
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::_Init()
{
	_Root().m_rpNextNode = &_Root();
	_Root().m_rpPreNode = &_Root();
	m_rpNext = &_Root();
	m_uiSize = 0;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
void FvRefList<T, TNode>::_Detach(Node & qT)
{
	FV_ASSERT(qT.m_rpRefList == this);
	if(&qT == m_rpNext)//! 如果是正在迭代的点, 则自动将Next移到下个点
	{
		m_rpNext = _GetNext(m_rpNext);
	}
	qT.m_rpPreNode->m_rpNextNode = qT.m_rpNextNode;
	qT.m_rpNextNode->m_rpPreNode = qT.m_rpPreNode;
	qT.m_rpPreNode = NULL;
	qT.m_rpNextNode = NULL;
	--m_uiSize;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode> 
void FvRefList<T, TNode>::_PushBefore(Node& kNext, Node& kNew)
{
	iterator pPre = kNext.m_rpPreNode;
	kNext.m_rpPreNode = &kNew;
	kNew.m_rpPreNode = pPre;
	kNew.m_rpNextNode = &kNext;
	FV_ASSERT(pPre);
	pPre->m_rpNextNode = &kNew;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode> 
void FvRefList<T, TNode>::_PushAfter(Node& kFront, Node& kNew)
{
	iterator pNext = kFront.m_rpNextNode;
	kFront.m_rpNextNode = &kNew;
	kNew.m_rpNextNode = pNext;
	kNew.m_rpPreNode = &kFront;
	FV_ASSERT(pNext);
	pNext->m_rpPreNode = &kNew;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode> 
void FvRefList<T, TNode>::_Link(Node& pre, Node& next)
{
	pre.m_rpNextNode = &next;
	next.m_rpPreNode = &pre;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
template<class TVisitor, void (TVisitor::*func)(T&)>
void FvRefList<T, TNode>::Each(TVisitor& qVisitor)const
{
	m_rpNext = _Root().m_rpNextNode;
	while (m_rpNext != &_Root())
	{
		T& kContent = m_rpNext->Content();
		m_rpNext = m_rpNext->m_rpNextNode;
		(qVisitor.*func)(kContent);
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
template<void (*func)(T&)>
void FvRefList<T, TNode>::Each()const
{
	m_rpNext = _Root().m_rpNextNode;
	while (m_rpNext != &_Root())
	{
		T& kContent = m_rpNext->Content();
		m_rpNext = m_rpNext->m_rpNextNode;
		(*func)(kContent);
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode>
template<class TContent, void (TContent::*func)()>
void FvRefList<T, TNode>::Each()const
{
	m_rpNext = _Root().m_rpNextNode;
	while (m_rpNext != &_Root())
	{
		TContent* rpT = m_rpNext->Content();
		m_rpNext = m_rpNext->m_rpNextNode;
		FV_ASSERT(rpT);
		((*rpT).*func)();
	}
}
