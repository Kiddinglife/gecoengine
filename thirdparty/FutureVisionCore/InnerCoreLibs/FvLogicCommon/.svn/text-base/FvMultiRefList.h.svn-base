//{future header message}
#ifndef __FvMultiRefList_H__
#define __FvMultiRefList_H__


#include "FvRefMap.h"
#include "FvDestroyManager.h"

template <class, class> class FvMultiRefNode;
template <class, class> class FvMultiRefList;

//+-----------------------------------------------------------------------------------------------------------------
namespace Private_
{

	//! 双向链表
	template <class TKey, class TValue> class RefList
	{
	public:
		//+----------------------------------------------
		typedef FvMultiRefNode<TKey, TValue> _Node;
		typedef _Node* iterator;

		template <class, class> friend class FvMultiRefList;
		friend class FvDeletable;

		struct _RootNode
		{
			_RootNode():m_rpPreNode(NULL), m_rpNextNode(NULL), m_rpList(NULL)
			{}

			_Node& Root()
			{
				return *(_Node*)(this);
			}	
			const _Node& Root()const
			{
				return *(_Node*)(this);
			}	
			_Node* m_rpPreNode;
			_Node* m_rpNextNode;
			RefList* m_rpList;
		};



		//+----------------------------------------------
		RefList()
			:m_rpNext(NULL)
			,m_uiSize(0)
		{
			m_kRoot.m_rpList = this;
			_Init();
		}
		~RefList()
		{
			Clear();
		}
		void FrushTo(RefList& qToList)
		{
			FV_ASSERT(&qToList != this);
			if(&qToList == this)
			{
				return;
			}
			Insert(*qToList.Root().m_rpPreNode, *this);
		}
		//+-----------------------------------------------------------------------------
		void AttachBack(_Node & qT)
		{
			qT.Detach();
			++m_uiSize;
			qT.m_rpRefList = this;
			_PushBack(*Root().m_rpPreNode, qT);
		}
		void AttachBackSafeIterator(_Node & qT)
		{
			qT.Detach();
			++m_uiSize;
			qT.m_rpRefList = this;
			_PushBack(*Root().m_rpPreNode, qT);
			if(m_rpNext == &Root())
			{
				m_rpNext = &qT;
			}
		}
		//+-----------------------------------------------------------------------------
		void AttachHead(_Node & qT)
		{
			qT.Detach();
			++m_uiSize;
			qT.m_rpRefList = this;
			_PushBefore(*Root().m_rpNextNode, qT);
		}

		//+-----------------------------------------------------------------------------
		void AttachAfter(_Node & qFront, _Node & qNew)
		{
			qNew.Detach();
			++m_uiSize;
			qNew.m_rpRefList = this;
			_PushBack(qFront, qNew);
		}
		//+-----------------------------------------------------------------------------
		void AttachBefore(_Node & qAfter, _Node & qNew)
		{
			qNew.Detach();
			++m_uiSize;
			qNew.m_rpRefList = this;
			_PushBefore(qAfter, qNew);
		}
		//+-----------------------------------------------------------------------------
		//+-----------------------------------------------------------------------------
		void BeginIterator()const
		{
			if(!(IsEnd()))
			{
				//ViLogBus::Warning("当前有迭代循环递归问题");
				FV_ASSERT(0 && "当前有迭代循环递归问题");
			}
			m_rpNext = Root().m_rpNextNode;
			FV_ASSERT(m_rpNext);
		}
		void Next()const
		{
			m_rpNext = _GetNext(m_rpNext);
			FV_ASSERT(m_rpNext);
		}
		TValue GetNextContent(_Node& kNod, const TValue kDefaultValue)const
		{
			iterator iterNext = _GetNext(&kNod);
			if(iterNext == &Root())
			{
				return kDefaultValue;
			}
			else
			{
				return iterNext->m_Content;
			}
		}
		static void Next(iterator& iter)
		{
			iter = _GetNext(iter);
		}
		iterator GetIterator()const
		{
			return m_rpNext;
		}
		bool IsEnd()const
		{
			return (m_rpNext == &Root());
		}
		bool IsEnd(const iterator iter)const
		{
			return (iter == &Root());
		}
		void EndIterator()const
		{
			m_rpNext = &const_cast<_Node&>(m_kRoot.Root());
		}
		iterator GetHead()const
		{
			return Root().m_rpNextNode;
		}
		iterator GetTail()const
		{
			return Root().m_rpPreNode;
		}
		bool Get(const FvUInt32 uiIdx, TValue& kValue)const
		{
			FvUInt32 uiIdx_ = uiIdx;
			iterator pCurrent = Root().m_rpNextNode;
			while (pCurrent != &Root())
			{
				if(uiIdx_ == 0)
				{
					kValue = pCurrent->m_Content;
					return true;
				}
				pCurrent =  pCurrent->m_rpNextNode;
				--uiIdx_;
			}
			return false;
		}
		//+-----------------------------------------------------------------------------
		void Clear()
		{
			iterator pCurrent = Root().m_rpNextNode;
			while (pCurrent != &Root())
			{
				iterator pNext = pCurrent->m_rpNextNode;
				pCurrent->m_rpRefList = NULL;
				pCurrent->m_rpNextNode = NULL;
				pCurrent->m_rpPreNode = NULL;
				pCurrent = pNext;
			}
			_Init();
		}

		void _DestroyContent()
		{
			iterator pCurrent = Root().m_rpNextNode;
			while (pCurrent != &Root())
			{
				TValue pContent = pCurrent->m_Content;
				pCurrent->m_Content = NULL;
				pCurrent = pCurrent->m_rpNextNode;
				delete pContent;
			}
			_Init();
		}
		void _Destroy()
		{
			iterator pCurrent = Root().m_rpNextNode;
			while (pCurrent != &Root())
			{
				iterator pNext = pCurrent->m_rpNextNode;
				pCurrent->m_rpRefList = NULL;
				delete pCurrent;
				pCurrent = pNext;
			}
			_Init();
		}
		FvUInt32 Size()const{return m_uiSize;}

		template<class TVisitor, void (TVisitor::*func)(TValue&)>
		void Each(TVisitor& qVisitor)const
		{
			m_rpNext = Root().m_rpNextNode;
			while (m_rpNext != &Root())
			{
				T* kContent = m_rpNext->m_Content;
				m_rpNext = m_rpNext->m_rpNextNode;
				(qVisitor.*func)(kContent);
			}
		}

		template<void (*func)(TValue&)>
		void Each()const
		{
			m_rpNext = Root().m_rpNextNode;
			while (m_rpNext != &Root())
			{
				T& kContent = m_rpNext->m_Content;
				m_rpNext = m_rpNext->m_rpNextNode;
				(*func)(kContent);
			}
		}

		template<class TContent, void (TContent::*func)()>
		void Each()const
		{
			m_rpNext = Root().m_rpNextNode;
			while (m_rpNext != &Root())
			{
				TContent* rpT = m_rpNext->m_Content;
				m_rpNext = m_rpNext->m_rpNextNode;
				FV_ASSERT(rpT);
				((*rpT).*func)();
			}
		}

		static void Insert(_Node& whereNode, RefList& list);
		static void AttachBack(_Node& whereNode, _Node& newOne);

	protected:
		void _Init()
		{
			Root().m_rpNextNode = &Root();
			Root().m_rpPreNode = &Root();
			m_rpNext = &Root();
			m_uiSize = 0;
		}
		//+-----------------------------------------------------------------------------
		static iterator _GetNext(const iterator iter)
		{
			FV_ASSERT(iter && iter->m_rpNextNode);
			return iter->m_rpNextNode;
		}
		static iterator _GetPre(const iterator iter)
		{
			return iter->m_rpPreNode;
		}
		void _Detach(_Node & qT)
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
			if(m_uiSize == 0)
			{
				m_kKeyNode.Detach();
				FvDestroyManager::Destroy(*this);
			}
		}
		//+----------------------------------------------
		static void _PushBack(_Node& qPreNode, _Node& qNewNode);
		static void _PushBefore(_Node& qNextNode, _Node& qNewNode);
		static void _Link(_Node& pre, _Node& next);
		//+----------------------------------------------
	protected:

		_RootNode m_kRoot;
		_Node& Root()
		{
			return m_kRoot.Root();
		}	
		const _Node& Root()const
		{
			return m_kRoot.Root();
		}

		mutable iterator m_rpNext;
		FvUInt32 m_uiSize;

		FvRefMapNode<FvUInt32, RefList*> m_kKeyNode;


	private:


		FV_DEFINE_COPY_CONSTRUCTOR(RefList);
		FV_DEFINE_COPY(RefList);
		FV_DEFINE_COMPARE(RefList);


		template <class, class> friend class FvMultiRefNode;
	};

	template <class TKey, class TValue> 
	void RefList<TKey, TValue>::Insert(_Node& whereNode, RefList& list)
	{
		if(list.Size() == 0)
		{
			return;
		}
		FV_ASSERT(whereNode.m_rpRefList);
		RefList& recList = *whereNode.m_rpRefList;
		iterator iter = list.Root().m_rpNextNode;
		while(iter != &list.Root())
		{
			iter->m_rpRefList = &recList;
			iter = iter->m_rpNextNode;
		}
		FV_ASSERT(&recList != &list);
		iterator fromFirst = list.Root().m_rpNextNode;
		iterator fromEnd = list.Root().m_rpPreNode;
		iterator insertNext = whereNode.m_rpNextNode;
		_Link(whereNode, *fromFirst);
		_Link(*fromEnd, *insertNext);	
		recList.m_uiSize += list.m_uiSize;
		list._Init();
	}
	template <class TKey, class TValue> 
	void RefList<TKey, TValue>::AttachBack(_Node& whereNode, _Node& newOne)
	{
		RefList* list = whereNode.m_rpRefList;
		FV_ASSERT(list);
		newOne.Detach();
		++list->m_uiSize;
		newOne.m_rpRefList = list;
		_PushBack(whereNode, newOne);
	}
	template <class TKey, class TValue> 
	void RefList<TKey, TValue>::_PushBack(_Node& qPreNode, _Node& qNewNode)
	{
		iterator pNext = qPreNode.m_rpNextNode;
		qPreNode.m_rpNextNode = &qNewNode;
		qNewNode.m_rpNextNode = pNext;
		qNewNode.m_rpPreNode = &qPreNode;
		FV_ASSERT(pNext);
		pNext->m_rpPreNode = &qNewNode;
	}
	template <class TKey, class TValue> 
	void RefList<TKey, TValue>::_PushBefore(_Node& qNextNode, _Node& qNewNode)
	{
		iterator pPre = qNextNode.m_rpPreNode;
		qNextNode.m_rpPreNode = &qNewNode;
		qNewNode.m_rpPreNode = pPre;
		qNewNode.m_rpNextNode = &qNextNode;
		FV_ASSERT(pPre);
		pPre->m_rpNextNode = &qNewNode;
	}
	template <class TKey, class TValue> 
	void RefList<TKey, TValue>::_Link(_Node& pre, _Node& next)
	{
		pre.m_rpNextNode = &next;
		next.m_rpPreNode = &pre;
	}
}

template <class TKey, class TValue> 
class FvMultiRefNode
{
public:
	typedef Private_::RefList<TKey, TValue> _List;
	template <class, class> friend class Private_::RefList;

	FvMultiRefNode(TValue& kT):m_rpPreNode(NULL), m_rpNextNode(NULL), m_Content(kT), m_rpRefList(NULL)
	{}
	FvMultiRefNode():m_rpPreNode(NULL), m_rpNextNode(NULL), m_Content(NULL), m_rpRefList(NULL)
	{}
	template<class U>
	FvMultiRefNode(U& kU):m_rpPreNode(NULL), m_rpNextNode(NULL), m_Content(kU), m_rpRefList(NULL)
	{}
	~FvMultiRefNode()
	{
		Detach();
	}
	bool IsAttach()const{return (m_rpRefList != NULL);}
	bool IsAttach(const _List& qList)const{return (m_rpRefList == &qList);}
	void Detach();
	FvMultiRefNode* Next()const{return m_rpNextNode;}
	FvMultiRefNode* Pre()const {return m_rpPreNode;}
	_List* GetRefList() const { return m_rpRefList; }

private:

	FV_DEFINE_COPY_CONSTRUCTOR(FvMultiRefNode);
	FV_DEFINE_COPY(FvMultiRefNode);
	FV_DEFINE_COMPARE(FvMultiRefNode);

	FvMultiRefNode* m_rpPreNode;
	FvMultiRefNode* m_rpNextNode;
	_List* m_rpRefList;

	//+-----------------------------------------------------------------------------

	template <class, template<class >class> friend class FvRefList;

public:

	TValue m_Content;
};

//+-----------------------------------------------------------------------------------------------
template <class TKey, class TValue> 
void FvMultiRefNode<TKey, TValue>::Detach()
{
	if(m_rpRefList)
	{
		m_rpRefList->_Detach(*this);
		m_rpRefList = NULL;
	}
}



template <class TKey, class TValue>
class FvMultiRefList
{
public:
	typedef FvMultiRefNode<TKey, TValue> Node;
	typedef Private_::RefList<TKey, TValue> _List;
	typedef FvRefMap<TKey, _List*> _Map;

	~FvMultiRefList()
	{
		Clear();
	}


	void Attach(const TKey key, Node& kNode)
	{
		_Map::Node* iter = m_kMap.Find(key);
		if(iter == NULL)
		{
			_List* pkList = new _List();
			FV_ASSERT(pkList);
			pkList->AttachBack(kNode);
			pkList->m_kKeyNode.m_kValue = pkList;
			pkList->m_kKeyNode.Attach(key, m_kMap);
		}
		else
		{
			_List* pkList = (*iter).m_kValue;
			FV_ASSERT(pkList);
			pkList->AttachBack(kNode);
		}
	}
	_List* Get(const TKey key)const
	{
		_Map::Node* iter = m_kMap.Find(key);
		if(iter == NULL)
		{
			return NULL;
		}
		else
		{
			_List* pkList = (*iter).m_kValue;
			return pkList;
		}
	}

	void Clear()
	{
		m_kMap.DestroyContent();
	}

private:

	_Map m_kMap;

};


namespace MultiRefList_Demo
{
	static void TEST()
	{
		FvMultiRefList<FvUInt32, FvUInt32> kList;
		FvMultiRefNode<FvUInt32, FvUInt32> kNode1;
		FvMultiRefNode<FvUInt32, FvUInt32> kNode2;
		FvMultiRefList<FvUInt32, FvUInt32>::_List* pkList;
		kList.Attach(1, kNode1);
		kList.Attach(2, kNode2);
		pkList = kList.Get(1);
		kList.Attach(2, kNode1);
		kNode1.Detach();
	}



}






#endif