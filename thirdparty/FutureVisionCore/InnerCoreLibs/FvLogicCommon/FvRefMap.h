//{future header message}
#ifndef __FvRefMap_H__
#define __FvRefMap_H__

#include "FvKernel.h"
#include "FvDebug.h"

template <class TKey, class TValue>
class FvRefMapNode
{
public:
	template <class, class> friend class FvRefMap;
	typedef FvRefMap<TKey, TValue> Tree;

	enum _Color
	{ 
		COLOR_BLACK,
		COLOR_RED,
	};
	enum _NIL
	{
		E_NONE,
		E_NIL,
	};

public:
	explicit FvRefMapNode(const TKey& v = TKey(), const _Color eC = COLOR_BLACK, const _NIL eN = E_NONE)
		:m_Key(v)
		,m_rpTree(NULL)
		,m_rpLeft(&NIL)
		,m_rpRight(&NIL)
		,m_rpParent(&NIL)
		,m_eColor(eC)
#if _DEBUG
		,m_eNil(eN)
#endif
	{
	}

	FvRefMapNode* Left()const{ FV_ASSERT(m_rpTree != NULL); return m_rpLeft; }
	FvRefMapNode* Right()const{ FV_ASSERT(m_rpTree != NULL); return m_rpRight; }
	FvRefMapNode* Parent()const{ FV_ASSERT(m_rpTree != NULL); return m_rpParent; }
	FvRefMapNode* Predecessor()const//! 邻接值Node
	{
		FV_ASSERT(this != &NIL);
		FvRefMapNode* pkPredecessor = Left();
		while (pkPredecessor->m_rpRight != &FvRefMapNode::NIL)
		{
			pkPredecessor = pkPredecessor->m_rpRight;
		}
		return pkPredecessor;
	}
	FvRefMapNode* Successor()const//! 邻接值Node
	{
		FV_ASSERT(this != &NIL);
		FvRefMapNode* pkSuccessor = Right();
		while (pkSuccessor->m_rpLeft != &FvRefMapNode::NIL)
		{
			pkSuccessor = pkSuccessor->m_rpLeft;
		}
		return pkSuccessor;
	}
	const TKey& Key()const{ return m_Key; }
	_Color Color()const{ return m_eColor; }

	void SetLeft(FvRefMapNode* pkNode){ FV_ASSERT(this == &NIL || this != pkNode);FV_ASSERT(this != &NIL || pkNode == &NIL); m_rpLeft = pkNode;}
	void SetRight(FvRefMapNode* pkNode){ FV_ASSERT(this == &NIL || this != pkNode);FV_ASSERT(this != &NIL || pkNode == &NIL); m_rpRight = pkNode;}
	void SetParent(FvRefMapNode& kNode){ FV_ASSERT(this == &NIL || this != &kNode);/*FV_ASSERT(this != &NIL || &kNode == &NIL);*/ m_rpParent = &kNode;}
	void SetKey(const TKey& key)
	{
		FV_ASSERT(this != &NIL);
		FV_ASSERT(m_rpTree == NULL);
		if(m_rpTree == NULL)
		{
			m_Key = key;
		}
	}
	void Attach(const TKey& key, Tree& kTree)
	{
		if(m_rpTree)
		{
			m_rpTree->_Detach(*this);
		}
		m_Key = key;
		m_rpTree = &kTree;
		m_rpTree->_Attach(*this);
	}
	void Attach(Tree& kTree)
	{
		if(m_rpTree)
		{
			m_rpTree->_Detach(*this);
		}
		m_rpTree = &kTree;
		m_rpTree->_Attach(*this);
	}
	void Detach()
	{
		if(m_rpTree)
		{
			m_rpTree->_Detach(*this);
			_Detach();
			m_rpTree = NULL;
		}
	}
	bool IsAttach()const{ return (m_rpTree != NULL);}

	bool IsNil()const{return (this == &NIL);}

	bool IsLeftChild()const
	{
		FV_ASSERT(m_rpParent);
		return (m_rpParent->m_rpLeft == this);
	}
	bool IsRightChild()const
	{
		FV_ASSERT(m_rpParent);
		return (m_rpParent->m_rpRight == this);
	}		
	bool IsLeftChild(const FvRefMapNode& kNode)const
	{
		return (kNode.m_rpLeft == this);
	}
	bool IsRightChild(const FvRefMapNode& kNode)const
	{
		return (kNode.m_rpRight == this);
	}
	bool IsChild(const FvRefMapNode& kNode)const
	{
		if(kNode.IsNil())
		{
			return true;
		}
		else
		{
			return (kNode.m_rpLeft == this || kNode.m_rpRight == this);
		}
	}

	TValue m_kValue;

private:
	void _Detach()
	{
		FV_ASSERT(this != &NIL);
		m_rpLeft = &NIL;
		m_rpRight = &NIL;
		m_rpParent = &NIL;
		m_rpTree = NULL;
	}	
	void SetColor(const _Color eC)
	{
		m_eColor = eC;///NIL可以被设置颜色
	}

	FV_DEFINE_COPY_CONSTRUCTOR(FvRefMapNode);
	FV_DEFINE_COPY(FvRefMapNode);
	FV_DEFINE_COMPARE(FvRefMapNode);

	TKey m_Key;
	Tree* m_rpTree;
	FvRefMapNode* m_rpLeft;
	FvRefMapNode* m_rpRight;
	FvRefMapNode* m_rpParent;
	_Color m_eColor;

#if _DEBUG
	_NIL m_eNil;
#endif
	static FvRefMapNode NIL;
};


template <class TKey, class TValue> FvRefMapNode<TKey, TValue> FvRefMapNode<TKey, TValue>::NIL(TKey(), FvRefMapNode::COLOR_BLACK, FvRefMapNode::E_NIL);

template <class TKey, class TValue>
class FvRefMap
{
public:
	template <class, class> friend class FvRefMapNode;
	typedef FvRefMapNode<TKey, TValue> Node;

	FvRefMap() 
		:m_rpRoot(&Node::NIL)
		,m_uiSize(0)
	{
	}
	~FvRefMap()
	{
		Clear();
	}

public:

	FvUInt32 Height() const{return Height(*m_rpRoot);}
	FvUInt32 Height(const Node& kNode) const;
	FvUInt32 Size()const{return m_uiSize;}


	Node* Find(const TKey& key)const;
	void Clear();
	void DestroyContent();

private:

	bool _Attach(Node& kNode);
	void _Detach(Node& kNode);

	void _DetachTree(Node& kNode);
	void _DestroyTreeContent(Node& kNode);

	void _LeftRotate(Node& kNode);
	void _RightRotate(Node& kNode);

	void _InsertFixup(Node& kNode);
	void _DeleteFixup(Node& kNode);

	Node* m_rpRoot;
	FvUInt32 m_uiSize;

	static void _Occupy(Node& kOld, Node& kNew);
};

template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::Clear()
{
	FV_ASSERT(m_rpRoot);
	if(!m_rpRoot->IsNil())
	{
		_DetachTree(*m_rpRoot);
		m_rpRoot = &Node::NIL;
		m_uiSize = 0;
	}
}
template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::_DetachTree(Node& kNode)
{
	///<暂时用递归>
	if(!kNode.Left()->IsNil())
	{
		_DetachTree(*kNode.Left());
	}
	if(!kNode.Right()->IsNil())
	{
		_DetachTree(*kNode.Right());
	}
	kNode._Detach();
}

template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::DestroyContent()
{
	FV_ASSERT(m_rpRoot);
	if(!m_rpRoot->IsNil())
	{
		_DestroyTreeContent(*m_rpRoot);
		m_rpRoot = &Node::NIL;
		m_uiSize = 0;
	}
}
template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::_DestroyTreeContent(Node& kNode)
{
	///<暂时用递归>
	if(!kNode.Left()->IsNil())
	{
		_DetachTree(*kNode.Left());
	}
	if(!kNode.Right()->IsNil())
	{
		_DetachTree(*kNode.Right());
	}
	kNode._Detach();
	delete kNode.m_kValue;
}

template <class TKey, class TValue>
FvUInt32 FvRefMap<TKey, TValue>::Height(const Node& kNode)const
{
	Node* rpNode = &kNode;
	FvUInt32 uiHeight = 0;
	while (rpNode) 
	{
		if (rpNode->Color() == Node::COLOR_BLACK) 
		{
			++uiHeight;
		}
		rpNode = rpNode->Left();
	}
	return uiHeight;
}

template <class TKey, class TValue>
FvRefMapNode<TKey, TValue>* FvRefMap<TKey, TValue>::Find(const TKey& key)const
{
	Node* pkNode = m_rpRoot;
	while (true) 
	{
		if(pkNode->IsNil())
		{
			return NULL;
		}
		if (pkNode->Key() == key) 
		{
			return pkNode;
		}
		if (key < pkNode->Key())
		{
			pkNode = pkNode->Left();
		}
		else
		{
			pkNode = pkNode->Right();
		}
	}
}
template <class TKey, class TValue>
bool FvRefMap<TKey, TValue>::_Attach(Node& kNode)
{
	kNode.SetColor(Node::COLOR_RED);
	Node* y = &Node::NIL;    // y 是 x 的父节点
	Node* x = m_rpRoot;
	while (x != &Node::NIL) 
	{
		y = x;
		if (kNode.Key() == x->Key()) 
		{
			return false;
		}
		if (kNode.Key() < x->Key())
		{
			x = x->Left();
		}
		else
		{
			x = x->Right();
		}
	}

	kNode.SetParent(*y);

	if (y == &Node::NIL) 
	{
		m_rpRoot = &kNode;
		kNode.SetParent(Node::NIL);
	}
	else 
	{
		kNode.SetParent(*y);
		if(kNode.Key() < y->Key()) 
		{
			y->SetLeft(&kNode);
		}
		else 
		{
			y->SetRight(&kNode);
		}
	}

	kNode.SetLeft(&Node::NIL);
	kNode.SetRight(&Node::NIL);

	_InsertFixup(kNode);

	++m_uiSize;
	return true;
}
template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::_Occupy(Node& kOld, Node& kNew)
{
	FV_ASSERT(kOld.IsNil() == false);
	FV_ASSERT(&kOld != &kNew);

	if(kNew.IsNil())
	{
		FV_ASSERT(kOld.Left()->IsNil());
		FV_ASSERT(kOld.Right()->IsNil());
		//
		if(kOld.IsLeftChild())
		{
			kOld.Parent()->SetLeft(&Node::NIL);
		}
		else
		{
			kOld.Parent()->SetRight(&Node::NIL);
		}
	}
	else
	{
		if(kOld.Left() == &kNew)
		{
			kOld.Right()->SetParent(kNew);
			kNew.SetRight(kOld.Right());
		}
		else if(kOld.Right() == &kNew)
		{
			kOld.Left()->SetParent(kNew);
			kNew.SetLeft(kOld.Left());
		}
		else
		{
			kOld.Left()->SetParent(kNew);
			kNew.SetLeft(kOld.Left());
			//
			kOld.Right()->SetParent(kNew);
			kNew.SetRight(kOld.Right());
		}//
		if(kOld.Parent()->IsNil())
		{

		}
		else
		{
			if(kOld.IsLeftChild())
			{
				kOld.Parent()->SetLeft(&kNew);
			}
			else
			{
				kOld.Parent()->SetRight(&kNew);
			}
		}
		kNew.SetParent(*kOld.Parent());
	}
	kNew.SetColor(kOld.Color());
}

template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::_Detach(Node& kNode)
{
	Node* y = &Node::NIL;
	Node* x = &Node::NIL;
	if (kNode.Left() == &Node::NIL || kNode.Right() == &Node::NIL) 
	{
		y = &kNode;
	}
	else 
	{
		y = kNode.Successor();
	}

	// x 是 y的唯一孩子
	if (y->Left() != &Node::NIL) 
	{
		x = y->Left();
	}
	else 
	{
		x = y->Right();
	}

	x->SetParent(*(y->Parent()));

	if(y->Parent() == &Node::NIL) 
	{
		m_rpRoot = x;
	}
	else 
	{
		if (y->IsLeftChild()) 
		{
			y->Parent()->SetLeft(x);
		}
		else 
		{
			y->Parent()->SetRight(x);
		}
	}

	if (y != &kNode) 
	{
		_Occupy(kNode, *y);

		FV_ASSERT(y->IsChild(*y->Parent()));
		if(!y->Left()->IsNil())
		{
			FV_ASSERT(y->Left()->IsLeftChild(*y));
		}
		if(!y->Right()->IsNil())
		{
			FV_ASSERT(y->Right()->IsRightChild(*y));
		}
	}

	if (y->Color() == Node::COLOR_BLACK)
	{
		_DeleteFixup(*x);
	}
	--m_uiSize;
}

template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::_LeftRotate(Node& kNode)
{
	Node* y = kNode.Right();

	// 把y的左子数变成x的右子树
	kNode.SetRight(y->Left());

	if (y->Left() != &Node::NIL) 
	{
		y->Left()->SetParent(kNode);
	}

	if (y != &Node::NIL) 
	{
		y->SetParent(*kNode.Parent());
	}

	if (kNode.Parent() == &Node::NIL) 
	{
		m_rpRoot = y;
	}
	else 
	{
		if (kNode.IsLeftChild()) 
		{
			kNode.Parent()->SetLeft(y);
		}
		else 
		{
			kNode.Parent()->SetRight(y);
		}
	}

	// 把x变成y的左子节点
	y->SetLeft(&kNode);
	if (&kNode != &Node::NIL) 
	{
		kNode.SetParent(*y);
	}
}
template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::_RightRotate(Node& kNode)
{
	Node* y = kNode.Left();

	// 把y的右子数变成x的左子树
	kNode.SetLeft(y->Right());

	if (y->Right() != &Node::NIL) 
	{
		y->Right()->SetParent(kNode);
	}

	if (y != &Node::NIL) 
	{
		y->SetParent(*kNode.Parent());
	}

	if (kNode.Parent() == &Node::NIL) 
	{
		m_rpRoot = y;
	}
	else 
	{
		if (kNode.IsLeftChild())
		{
			kNode.Parent()->SetLeft(y);
		}
		else 
		{
			kNode.Parent()->SetRight(y);
		}
	}

	y->SetRight(&kNode);
	if (&kNode != &Node::NIL) 
	{
		kNode.SetParent(*y);
	}
}


template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::_InsertFixup(Node& kNode)
{
	Node* y = &Node::NIL;
	Node* z = &kNode;
	while (z != m_rpRoot && z->Parent()->Color() == Node::COLOR_RED) 
	{
		if (z->Parent()->IsLeftChild()) 
		{
			y = z->Parent()->Parent()->Right();
			if (y->Color() == Node::COLOR_RED) 
			{
				z->Parent()->SetColor(Node::COLOR_BLACK);
				y->SetColor(Node::COLOR_BLACK);
				z->Parent()->Parent()->SetColor(Node::COLOR_RED);
				z = z->Parent()->Parent();
			}
			else 
			{
				if (z->IsRightChild()) 
				{
					z = z->Parent();
					_LeftRotate(*z);
				}

				z->Parent()->SetColor(Node::COLOR_BLACK);
				z->Parent()->Parent()->SetColor(Node::COLOR_RED);
				_RightRotate(*(z->Parent()->Parent()));
			}
		}
		else 
		{
			y = z->Parent()->Parent()->Left();
			if (y->Color() == Node::COLOR_RED) 
			{
				z->Parent()->SetColor(Node::COLOR_BLACK);
				y->SetColor(Node::COLOR_BLACK);
				z->Parent()->Parent()->SetColor(Node::COLOR_RED);
				z = z->Parent()->Parent();
			}
			else 
			{
				if (z->IsLeftChild()) 
				{
					z = z->Parent();
					_RightRotate(*z);
				}

				z->Parent()->SetColor(Node::COLOR_BLACK);
				z->Parent()->Parent()->SetColor(Node::COLOR_RED);
				_LeftRotate(*(z->Parent()->Parent()));
			}
		}
	}

	m_rpRoot->SetColor(Node::COLOR_BLACK);
}

template <class TKey, class TValue>
void FvRefMap<TKey, TValue>::_DeleteFixup(Node& kNode)
{
	Node* x = &kNode;
	Node* w = &Node::NIL;
	while (x != m_rpRoot && x->Color() == Node::COLOR_BLACK) 
	{
		if (x->IsLeftChild())
		{
			w = x->Parent()->Right();
			if (w->Color() == Node::COLOR_RED) 
			{
				w->SetColor(Node::COLOR_BLACK);
				x->Parent()->SetColor(Node::COLOR_RED);
				_LeftRotate(*(x->Parent()));
				w = x->Parent()->Right();
			}
			if (w->Left()->Color() == Node::COLOR_BLACK && w->Right()->Color() == Node::COLOR_BLACK) 
			{
				w->SetColor(Node::COLOR_RED);
				x = x->Parent();
			}
			else 
			{
				if (w->Right()->Color() == Node::COLOR_BLACK) 
				{
					w->Left()->SetColor(Node::COLOR_BLACK);
					w->SetColor(Node::COLOR_RED);
					_RightRotate(*w);
					w = x->Parent()->Right();
				}
				w->SetColor(x->Parent()->Color());
				x->Parent()->SetColor(Node::COLOR_BLACK);
				w->Right()->SetColor(Node::COLOR_BLACK);
				_LeftRotate(*(x->Parent()));
				x = m_rpRoot;
			}
		} 
		else 
		{
			w = x->Parent()->Left();
			if (w->Color() == Node::COLOR_RED) 
			{
				w->SetColor(Node::COLOR_BLACK);
				x->Parent()->SetColor(Node::COLOR_RED);
				_RightRotate(*(x->Parent()));
				w = x->Parent()->Left();
			}
			if (w->Right()->Color() == Node::COLOR_BLACK && w->Left()->Color() == Node::COLOR_BLACK) 
			{
				w->SetColor(Node::COLOR_RED);
				x = x->Parent();
			} 
			else 
			{
				if (w->Left()->Color() == Node::COLOR_BLACK) 
				{
					w->Right()->SetColor(Node::COLOR_BLACK);
					w->SetColor(Node::COLOR_RED);
					_LeftRotate(*w);
					w = x->Parent()->Left();
				}
				w->SetColor(x->Parent()->Color());
				x->Parent()->SetColor(Node::COLOR_BLACK);
				w->Left()->SetColor(Node::COLOR_BLACK);
				_RightRotate(*(x->Parent()));
				x = m_rpRoot;
			}
		}
	}

	x->SetColor(Node::COLOR_BLACK);
}


#include <map>
#include "FvGameRandom.h"
#include "FvLogBus.h"
namespace RefMap_Demo
{

	static void TestSpd()
	{
		const FvUInt32 uiTestCnt1 = 100;
		const FvUInt32 uiTestCnt2 = 10000;
		///<>
		FvUInt64 iTime = GetTickCount();
		FvUInt32 uiSize = 0;
		for (int iTestCnt= 0;iTestCnt < uiTestCnt1; ++iTestCnt)
		{
			FvRefMap<int, int> s;
			FvRefMap<int, int>::Node kNodes[uiTestCnt2];
			for(int i = 0; i < uiTestCnt2; i ++)
			{
				FvRefMap<int, int>::Node& kNode = kNodes[i];
				kNode.SetKey(i);
				kNode.Attach(s);
			}
			uiSize += s.Size();
		}
		FvUInt64 iCost_1 = GetTickCount() - iTime;
		iTime = GetTickCount();
		FvLogBus::CritOk("COST1[%d]Size[%d]", FvUInt32(iCost_1), uiSize);
		///<>
		uiSize = 0;
		for (int iTestCnt= 0;iTestCnt < uiTestCnt1; ++iTestCnt)
		{
			std::map<int, int> kMap;
			for(int i = 0; i < uiTestCnt2; i ++)
			{
				kMap[i] = i;
			}
			uiSize +=kMap.size();
		}
		FvUInt64 iCost_2 = GetTickCount() - iTime;
		iTime = GetTickCount();
		FvLogBus::CritOk("COST2[%d]Size[%d]", FvUInt32(iCost_2), uiSize);

	}
	static void TestDetach()
	{
		FvRefMap<int, int> s;
		FvRefMap<int, int>::Node kNodes[1000];
		for(int i = 0; i < 1000; i ++)
		{
			FvRefMap<int, int>::Node& kNode = kNodes[i];
			kNode.SetKey(i);
			kNode.Attach(s);
		}
		while(s.Size())
		{
			FvInt32 uiIdx = FvGameRandom::GetValue(0, 999);
			if(kNodes[uiIdx].IsAttach())
			{
				FvLogBus::CritOk("Detach[%d]", uiIdx);
				kNodes[uiIdx].Detach();
			}
		}
	}
	static void TestDetachAll()
	{
		FvRefMap<int, int> s;
		FvRefMap<int, int>::Node kNodes[1000];
		for(int i = 0; i < 1000; i ++)
		{
			FvRefMap<int, int>::Node& kNode = kNodes[i];
			kNode.SetKey(i);
			kNode.Attach(s);
		}
		s.Clear();
		for(int i = 0; i < 1000; i ++)
		{
			FV_ASSERT(kNodes[i].IsAttach() == false);
		}
	}
	static void TestFind()
	{
		FvRefMap<int, int> s;
		FvRefMap<int, int>::Node kNodes[1000];
		for(int i = 0; i < 1000; i ++)
		{
			FvRefMap<int, int>::Node& kNode = kNodes[i];
			kNode.SetKey(i);
			kNode.Attach(s);
		}
		FvRefMap<int, int>::Node* pkNode200 = s.Find(200);
		while(s.Size())
		{
			FvInt32 uiIdx = FvGameRandom::GetValue(0, 999);
			if(kNodes[uiIdx].IsAttach())
			{
				FvRefMap<int, int>::Node* pkFinded = s.Find(kNodes[uiIdx].m_kValue);
				FV_ASSERT(pkFinded);
				FvLogBus::CritOk("Detach[%d]", uiIdx);
				kNodes[uiIdx].Detach();
			}
		}
	}
	static void Test()
	{
		TestSpd();
	}
}


#endif //__FvRefMap_H__