#pragma once
//
//class FvRefBTree
//{
//public:
//	FvRefBTree(void);
//	~FvRefBTree(void);
//};
//1¡¢Í·ÎÄ¼þ RBTree.h
//#ifndef RBTREE__JOHN__DMRC 
//#define RBTREE__JOHN__DMRC 
///********************   Class Node   **********************/ 
//template <class TKey, class TValue> 
//class RBNode
//{ 
//public: 
//	RBNode() 
//	: _bRed(false), _bExist(false), _rpLeft(NULL), _rpRight(NULL), _rpParent(NULL)
//	{} 
//
//	//function 
//	// max & min 
//	RBNode* maximum(); 
//	RBNode* minimum(); 
//	// successor & predecessor 
//	RBNode* successor(); 
//	RBNode* predecessor(); 
//	// exist 
//	bool isExist() { return this ->_bExist; } 
//	bool isNULL() { return !this ->_bExist; } 
//	// red & black ( black = 0 & red = 1 ) 
//	//bool getColor() { return this ->_red; } 
//	bool isRed(){ return this ->_bRed; } 
//	bool isBlack() { return !this ->_bRed; } 
//	//void setColor(bool c) { this ->_red = c; } 
//	void setRed() { this ->_bRed = true; } 
//	void setBlack() { this ->_bRed = false; } 
//	TKey Key(){return _kKey;}
//	//varables 
//
//	TKey _kKey;
//	T _kData; 
//	bool _bRed; 
//	bool _bExist; 
//	RBNode* _rpLeft; 
//	RBNode* _rpRight; 
//	RBNode* _rpParent; 
//};
//
//
//template <class TKey, class TValue> 
//RBNode<TKey, TValue>* RBNode<TKey, TValue>::maximum()
//{ 
//	RBNode* x = this; 
//	while(x ->_rpRight ->isExist()) 
//	{
//		x = x ->_rpRight;
//	}
//	return x; 
//}
//template <class TKey, class TValue> 
//RBNode<TKey, TValue>* RBNode<TKey, TValue>::minimum()
//{ 
//	RBNode* x = this; 
//	while(x ->_rpLeft ->isExist()) 
//	{
//		x = x ->_rpLeft; 
//	}
//	return x; 
//}
//template <class TKey, class TValue> 
//RBNode<TKey, TValue>* RBNode<TKey, TValue>::successor()
//{ 
//	RBNode* y; 
//	RBNode* x; 
//	if(this ->_rpRight ->isExist()) 
//	{
//		return this ->_rpRight ->minimum(); 
//	}
//	x = this; 
//	y = x ->_rpParent; 
//	while(y != NULL && x == y ->_rpRight)
//	{ 
//		x = y; 
//		y = x ->_rpParent; 
//	} 
//	return y; 
//}
//template <class TKey, class TValue> 
//RBNode<TKey, TValue>* RBNode<TKey, TValue>::predecessor()
//{ 
//	RBNode* y; 
//	RBNode* x; 
//	if(this ->_rpLeft ->isExist()) 
//	{
//		return this ->_rpLeft ->maximum();
//	}
//	x = this; 
//	y = x ->_rpParent; 
//	while(y != NULL && x == y ->_rpLeft)
//	{ 
//		x = y; 
//		y = x ->_rpParent; 
//	} 
//	return y; 
//} 
//
//
//template <class TKey, class TValue> 
//class RBTree
//{ 
//public: 
//
//	typedef RBNode<TKey, TValue> Node;
//	RBTree() 
//		:m_rpRoot(NULL)
//	{} 
//	~RBTree(); 
//
//	//bool Insert(T &); 
//	//bool Delete(T &, E); 
//
//	bool Insert(Node& kNode);
//	bool _Detach(Node& kNode);
//
//
//	// debug 
//	Node* GetRoot(){ return m_rpRoot;} 
//	//T GetRootData(){ return m_opRoot ->data;} 
//private: 
//	// delete_tree; 
//	void delete_tree(Node& kNode); 
//	bool delete_node(Node& kNode); 
//	// delete a node; 
//	//bool delete_node(T&, Node<T>*); 
//	// rotates 
//	void left_rotate(Node& kNode); 
//	void right_rotate(Node& kNode); 
//	// fixups 
//	void insert_fixup(Node& kNode); 
//	void delete_fixup(Node& kNode); 
//	Node* m_rpRoot; 
//};
///*******************   Class RBTree   *********************/ 