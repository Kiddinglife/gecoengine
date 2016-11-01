#pragma once



#include <FvLogBus.h>
#include <FvDebug.h>
//http://chenm.blogbus.com/logs/50422429.html

namespace RBTree_Private
{
	//template <class TKey, class TValue>
	//class Node
	//{
	//public:
	//	enum _Color
	//	{ 
	//		COLOR_BLACK,
	//		COLOR_RED
	//	};

	//public:
	//	explicit Node(const TKey& v = TKey(), const _Color eC = COLOR_BLACK)
	//		: m_Key(v)
	//		, m_rpLeft(&NIL)
	//		, m_rpRight(&NIL)
	//		, m_rpParent(&NIL)
	//		, m_eColor(eC)
	//	{
	//	}

	//	Node* Left()const{ return m_rpLeft; }
	//	Node* Right()const{ return m_rpRight; }
	//	Node* Parent()const{ return m_rpParent; }
	//	const TKey& Key()const{ return m_Key; }
	//	_Color Color()const{ return m_eColor; }

	//	void SetLeft(Node* pkNode){ FV_ASSERT(this != &NIL); m_rpLeft = pkNode; }
	//	void SetRight(Node* pkNode){ FV_ASSERT(this != &NIL); m_rpRight = pkNode; }
	//	void SetParent(Node& kNode){ /*FV_ASSERT(this != &NIL);*/ m_rpParent = &kNode; }
	//	void SetKey(const TKey& key){ FV_ASSERT(this != &NIL); m_Key = key; }
	//	void SetColor(const _Color eC){ FV_ASSERT(this != &NIL); m_eColor = eC; }

	//	void Detach()
	//	{
	//		m_rpLeft = NULL;
	//		m_rpRight = NULL;
	//		m_rpParent = NULL;
	//	}

	//	operator bool() const
	//	{
	//		return this != &NIL;
	//	}
	//	bool operator !() const
	//	{
	//		return this == &NIL;
	//	}

	//public:
	//	static Node NIL;

	//	TKey m_Key;
	//	TValue m_kValue;
	//private:
	//	Node* m_rpLeft;
	//	Node* m_rpRight;
	//	Node* m_rpParent;
	//	_Color m_eColor;
	//};


	//template <class TKey, class TValue> Node<TKey, TValue> Node<TKey, TValue>::NIL;
}

//
//template <class TKey, class TValue>
//class RBTree
//{
//public:
//	typedef RBTree_Private::Node<TKey, TValue> Node;
//
//	RBTree() 
//		:m_rpRoot(&Node::NIL)
//		,m_uiSize(0)
//	{}
//	~RBTree()
//	{}
//
//public:
//
//	FvUInt32 Height() const{return Height(*m_rpRoot);}
//	FvUInt32 Height(const Node& kNode) const;
//
//	bool Insert(Node& kNode);
//	void Detach(Node& kNode);
//
//
//	void _DetachTree(Node& kNode);
//	bool _DetachNode(Node& kNode);
//
//	void _LeftRotate(Node& kNode);
//	void _RightRotate(Node& kNode);
//
//	void _InsertFixup(Node& kNode);
//	void _DeleteFixup(Node& kNode);
//private:
//
//	Node* m_rpRoot;
//	FvUInt32 m_uiSize;
//
//};
//
//template <class TKey, class TValue>
//FvUInt32 RBTree<TKey, TValue>::Height(const Node& kNode)const
//{
//	Node* rpNode = &kNode;
//	FvUInt32 uiHeight = 0;
//	while (rpNode) 
//	{
//		if (rpNode->Color() == Node::COLOR_BLACK) 
//		{
//			++uiHeight;
//		}
//		rpNode = rpNode->Left();
//	}
//	return uiHeight;
//}
//
//template <class TKey, class TValue>
//bool RBTree<TKey, TValue>::Insert(Node& kNode)
//{
//	kNode.SetColor(Node::COLOR_RED);
//	Node* y = &Node::NIL;    // y 是 x 的父节点
//	Node* x = m_rpRoot;
//	while (x != &Node::NIL) 
//	{
//		y = x;
//
//		if (kNode.Key() == x->Key()) 
//		{
//			return false;
//		}
//
//		if (kNode.Key() < x->Key())
//		{
//			x = x->Left();
//		}
//		else
//		{
//			x = x->Right();
//		}
//	}
//
//	kNode.SetParent(*y);
//
//	if (y == &Node::NIL) 
//	{
//		m_rpRoot = &kNode;
//		kNode.SetParent(Node::NIL);
//	}
//	else 
//	{
//		if(kNode.Key() < y->Key()) 
//		{
//			y->SetLeft(&kNode);
//		}
//		else 
//		{
//			y->SetRight(&kNode);
//		}
//	}
//
//	kNode.SetLeft(&Node::NIL);
//	kNode.SetRight(&Node::NIL);
//
//	_InsertFixup(kNode);
//
//	++m_uiSize;
//	return true;
//}
//template <class TKey, class TValue>
//void RBTree<TKey, TValue>::Detach(Node& kNode)
//{
//	Node* y = &Node::NIL;
//	Node* x = &Node::NIL;
//	if (kNode.Left() == &Node::NIL || kNode.Right() == &Node::NIL) 
//	{
//		y = &kNode;
//	}else 
//	{
//		//y = this->successor(z);
//		y = kNode.Right();
//		while (y->Left() != &Node::NIL) 
//		{
//			y = y->Left();
//		}
//	}
//
//	// x 是 y的唯一孩子
//	if (y->Left() != &Node::NIL) 
//	{
//		x = y->Left();
//	}
//	else 
//	{
//		x = y->Right();
//	}
//
//	x->SetParent(*(y->Parent()));
//
//	if(y->Parent() == &Node::NIL) 
//	{
//		m_rpRoot = x;
//	}
//	else 
//	{
//		if (y == y->Parent()->Left()) 
//		{
//			y->Parent()->SetLeft(x);
//		}
//		else 
//		{
//			y->Parent()->SetRight(x);
//		}
//	}
//
//	if (y != &kNode) 
//	{
//		//kNode.SetKey(y->Key());
//		//
//		kNode.Left()->SetParent(*y);
//		y->SetLeft(kNode.Left());
//		//
//		kNode.Right()->SetParent(*y);
//		y->SetRight(kNode.Right());
//		//
//		if(kNode.Parent()->Left() == &kNode)
//		{
//			kNode.Parent()->SetLeft(y);
//		}
//		else
//		{
//			kNode.Parent()->SetRight(y);
//		}
//		y->SetParent(*kNode.Parent());
//		kNode.Detach();
//	}
//
//	if (y->Color() == Node::COLOR_BLACK)
//	{
//		_DeleteFixup(*x);
//	}
//	--m_uiSize;
//}
//
//template <class TKey, class TValue>
//void RBTree<TKey, TValue>::_LeftRotate(Node& kNode)//(Node* x)
//{
//	Node* y = kNode.Right();
//
//	// 把y的左子数变成x的右子树
//	kNode.SetRight(y->Left());
//
//	if (y->Left() != &Node::NIL) 
//	{
//		y->Left()->SetParent(kNode);
//	}
//
//	if (y != &Node::NIL) 
//	{
//		y->SetParent(*kNode.Parent());
//	}
//
//	if (kNode.Parent() == &Node::NIL) 
//	{
//		m_rpRoot = y;
//	}
//	else 
//	{
//		if (&kNode == kNode.Parent()->Left()) 
//		{
//			kNode.Parent()->SetLeft(y);
//		}
//		else 
//		{
//			kNode.Parent()->SetRight(y);
//		}
//	}
//
//	// 把x变成y的左子节点
//	y->SetLeft(&kNode);
//	if (&kNode != &Node::NIL) 
//	{
//		kNode.SetParent(*y);
//	}
//}
//template <class TKey, class TValue>
//void RBTree<TKey, TValue>::_RightRotate(Node& kNode)
//{
//	Node* y = kNode.Left();
//
//	// 把y的右子数变成x的左子树
//	kNode.SetLeft(y->Right());
//
//	if (y->Right() != &Node::NIL) 
//	{
//		y->Right()->SetParent(kNode);
//	}
//
//	if (y != &Node::NIL) 
//	{
//		y->SetParent(*kNode.Parent());
//	}
//
//	if (kNode.Parent() == &Node::NIL) 
//	{
//		m_rpRoot = y;
//	}
//	else 
//	{
//		if (&kNode == kNode.Parent()->Left())
//		{
//			kNode.Parent()->SetLeft(y);
//		}
//		else 
//		{
//			kNode.Parent()->SetRight(y);
//		}
//	}
//
//	y->SetRight(&kNode);
//	if (&kNode != &Node::NIL) 
//	{
//		kNode.SetParent(*y);
//	}
//}
//
//
//template <class TKey, class TValue>
//void RBTree<TKey, TValue>::_InsertFixup(Node& kNode)
//{
//	Node* y = &Node::NIL;
//	Node* z = &kNode;
//	while (z != m_rpRoot && z->Parent()->Color() == Node::COLOR_RED) 
//	{
//		if (z->Parent() == z->Parent()->Parent()->Left()) 
//		{
//			y = z->Parent()->Parent()->Right();
//			if (y->Color() == Node::COLOR_RED) 
//			{
//				z->Parent()->SetColor(Node::COLOR_BLACK);
//				y->SetColor(Node::COLOR_BLACK);
//				z->Parent()->Parent()->SetColor(Node::COLOR_RED);
//				z = z->Parent()->Parent();
//			}
//			else 
//			{
//				if (z == z->Parent()->Right()) 
//				{
//					z = z->Parent();
//					_LeftRotate(*z);
//				}
//
//				z->Parent()->SetColor(Node::COLOR_BLACK);
//				z->Parent()->Parent()->SetColor(Node::COLOR_RED);
//				_RightRotate(*(z->Parent()->Parent()));
//			}
//		}
//		else 
//		{
//			y = z->Parent()->Parent()->Left();
//			if (y->Color() == Node::COLOR_RED) 
//			{
//				z->Parent()->SetColor(Node::COLOR_BLACK);
//				y->SetColor(Node::COLOR_BLACK);
//				z->Parent()->Parent()->SetColor(Node::COLOR_RED);
//				z = z->Parent()->Parent();
//			}
//			else 
//			{
//				if (z == z->Parent()->Left()) 
//				{
//					z = z->Parent();
//					_RightRotate(*z);
//				}
//
//				z->Parent()->SetColor(Node::COLOR_BLACK);
//				z->Parent()->Parent()->SetColor(Node::COLOR_RED);
//				_LeftRotate(*(z->Parent()->Parent()));
//			}
//		}
//	}
//
//	m_rpRoot->SetColor(Node::COLOR_BLACK);
//}
//
//template <class TKey, class TValue>
//void RBTree<TKey, TValue>::_DeleteFixup(Node& kNode)
//{
//	Node* x = &kNode;
//	Node* w = &Node::NIL;
//	while (x != m_rpRoot && x->Color() == Node::COLOR_BLACK) 
//	{
//		if (x == x->Parent()->Left())
//		{
//			w = x->Parent()->Right();
//			if (w->Color() == Node::COLOR_RED) 
//			{
//				w->SetColor(Node::COLOR_BLACK);
//				x->Parent()->SetColor(Node::COLOR_RED);
//				_LeftRotate(*(x->Parent()));
//				w = x->Parent()->Right();
//			}
//			if (w->Left()->Color() == Node::COLOR_BLACK && w->Right()->Color() == Node::COLOR_BLACK) 
//			{
//				w->SetColor(Node::COLOR_RED);
//				x = x->Parent();
//			}
//			else 
//			{
//				if (w->Right()->Color() == Node::COLOR_BLACK) 
//				{
//					w->Left()->SetColor(Node::COLOR_BLACK);
//					w->SetColor(Node::COLOR_RED);
//					_RightRotate(*w);
//					w = x->Parent()->Right();
//				}
//				w->SetColor(x->Parent()->Color());
//				x->Parent()->SetColor(Node::COLOR_BLACK);
//				w->Right()->SetColor(Node::COLOR_BLACK);
//				_LeftRotate(*(x->Parent()));
//				x = m_rpRoot;
//			}
//		} 
//		else 
//		{
//			w = x->Parent()->Left();
//			if (w->Color() == Node::COLOR_RED) 
//			{
//				w->SetColor(Node::COLOR_BLACK);
//				x->Parent()->SetColor(Node::COLOR_RED);
//				_RightRotate(*(x->Parent()));
//				w = x->Parent()->Left();
//			}
//			if (w->Right()->Color() == Node::COLOR_BLACK && w->Left()->Color() == Node::COLOR_BLACK) 
//			{
//				w->SetColor(Node::COLOR_RED);
//				x = x->Parent();
//			} 
//			else 
//			{
//				if (w->Left()->Color() == Node::COLOR_BLACK) 
//				{
//					w->Right()->SetColor(Node::COLOR_BLACK);
//					w->SetColor(Node::COLOR_RED);
//					_LeftRotate(*w);
//					w = x->Parent()->Left();
//				}
//				w->SetColor(x->Parent()->Color());
//				x->Parent()->SetColor(Node::COLOR_BLACK);
//				w->Left()->SetColor(Node::COLOR_BLACK);
//				_RightRotate(*(x->Parent()));
//				x = m_rpRoot;
//			}
//		}
//	}
//
//	x->SetColor(Node::COLOR_BLACK);
//}

//template<typename T>
//class RBTreeIterator
//{
//public:
// RBTreeIterator()
//	 : tree_(RBTree_Private::Node<T>::NIL), current_(RBTreeNode<T>::NIL)
// {
// }
//
// RBTreeIterator(RBTree<T>& tree, typename RBTree<T>::Node* current)
//	 : tree_(tree), current_(current)
// {
// }
//
// const T& operator*() const
// {
//	 return current_->value();
// }
//
// T* operator->()
// {
//	 return current_;
// }
//
// bool operator==(const RBTreeIterator& rhs) const
// {
//	 return current_ == rhs.current_;
// }
//
// bool operator!=(const RBTreeIterator& rhs) const
// {
//	 return current_ != rhs.current_;
// }
//
// /// 操作符 ++it
// RBTreeIterator& operator++()
// {
//	 current_ = tree_.successor(current_);
//	 return *this;
// }
//
//private:
// RBTree<T>& tree_;
// typename RBTree<T>::Node* current_;
//};



//namespace RefTree_Demo
//{
//	static void Test()
//	{
//		RBTree<int, int> s;
//		RBTree<int, int>::Node kNodes[1000];
//		for(int i = 1; i < 1000; i ++)
//		{
//			RBTree<int, int>::Node& kNode = kNodes[i];
//			kNode.SetKey(i);
//			s.Insert(kNode);
//		}
//		s.Detach(kNodes[500]);
//	}
//}
