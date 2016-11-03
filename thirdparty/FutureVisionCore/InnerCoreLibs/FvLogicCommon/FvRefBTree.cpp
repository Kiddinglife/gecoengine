#include "FvRefBTree.h"

//FvRefBTree::FvRefBTree(void)
//{
//}
//
//FvRefBTree::~FvRefBTree(void)
//{
//}

//
//
//template<class TKey, class TData>
//struct RBNode 
//{
//	TKey _ksy;//Key key;
//	int lcount;
//	int rcount;
//	RBNode* _rpLeft;//RBTNode* lchild;
//	RBNode* _rpRight;//RBTNode* rchild;
//	RBNode* _rpParent;//RBTNode* parent;
//	bool color;
//};
//
//template<class TKey, class TData>
//class RBTree 
//{
//
//	typedef RBNode<TKey, TData> Node;
//	const static bool RED = true;
//	const static bool BLACK = false;
//
//
//	Node* m_null;
//	Node* m_root;
//
//	void clear() {//
//		RBTNode* p = m_root;
//		while (p != m_null) {
//			if (p->lchild != m_null) {
//				p = p->lchild;
//			}else if (p->rchild != m_null) {
//				p = p->rchild;
//			}else {
//				RBTNode* temp = p;
//				p = p->parent;
//				if (temp == p->lchild) {
//					p->lchild = m_null;
//				}else {
//					p->rchild = m_null;
//				}
//				delete temp;
//			}
//		}
//		m_root = m_null;
//	}
//	void delFixup(RBTNode* delNode) {
//		RBTNode* p = delNode;
//		while (p != m_root && p->color == BLACK) {
//			if (p == p->parent->lchild) {
//				RBTNode* sibling = p->parent->rchild;
//				if (sibling->color == RED) {
//					sibling->color = BLACK;
//					p->parent->color = RED;
//					leftRotate(p->parent);
//					sibling = p->parent->rchild;
//				}
//				if (sibling->lchild->color == BLACK
//					&& sibling->rchild->color == BLACK
//					) {
//						sibling->color = RED;
//						p = p->parent;
//				}else {
//					if (sibling->rchild->color == BLACK) {
//						sibling->lchild->color = BLACK;
//						sibling->color = RED;
//						rightRotate(sibling);
//						sibling = sibling->parent;
//					}
//					sibling->color = sibling->parent->color;
//					sibling->parent->color = BLACK;
//					sibling->rchild->color = BLACK;
//					leftRotate(sibling->parent);
//					p = m_root;
//				}
//			}else {
//				RBTNode* sibling = p->parent->lchild;
//				if (sibling->color == RED) {
//					sibling->color = BLACK;
//					p->parent->color = RED;
//					rightRotate(p->parent);
//					sibling = p->parent->lchild;
//				}
//				if (sibling->lchild->color == BLACK
//					&& sibling->rchild->color == BLACK
//					) {
//						sibling->color = RED;
//						p = p->parent;
//				}else {
//					if (sibling->lchild->color == BLACK) {
//						sibling->rchild->color = BLACK;
//						sibling->color = RED;
//						leftRotate(sibling);
//						sibling = sibling->parent;
//					}
//					sibling->color = sibling->parent->color;
//					sibling->parent->color = BLACK;
//					sibling->lchild->color = BLACK;
//
//
//
//					rightRotate(sibling->parent);
//					p = m_root;
//				}
//			}
//		}
//		p->color = BLACK;
//	}
//	void insertFixup(RBTNode* insertNode) {
//		RBTNode* p = insertNode;
//		while (p->parent->color == RED) {
//			if (p->parent == p->parent->parent->lchild) {
//				RBTNode* parentRight = p->parent->parent->rchild;
//				if (parentRight->color == RED) {
//					p->parent->color = BLACK;
//					parentRight->color = BLACK;
//					p->parent->parent->color = RED;
//					p = p->parent->parent;
//				}else {
//					if (p == p->parent->rchild) {
//						p = p->parent;
//						leftRotate(p);
//					}
//					p->parent->color = BLACK;
//					p->parent->parent->color = RED;
//					rightRotate(p->parent->parent);
//				}
//			}else {
//				RBTNode* parentLeft = p->parent->parent->lchild;
//				if (parentLeft->color == RED) {
//					p->parent->color = BLACK;
//					parentLeft->color = BLACK;
//					p->parent->parent->color = RED;
//					p = p->parent->parent;
//				}else {
//					if (p == p->parent->lchild) {
//						p = p->parent;
//						rightRotate(p);
//					}
//					p->parent->color = BLACK;
//					p->parent->parent->color = RED;
//					leftRotate(p->parent->parent);
//				}
//			}
//		}
//		m_root->color = BLACK;
//	}
//
//	inline int keyCmp(const Key& key1, const Key& key2) {
//		//比较两个Key的大小。这里可能有更复杂的比较，如字符串比较等。
//		return key1.value - key2.value;
//	}
//
//	inline void leftRotate(RBTNode* node) {
//		//把一个节点向左下方移一格，并让他原来的右子节点代替它的位置。
//		RBTNode* right = node->rchild;
//		node->rchild = right->lchild;
//		node->rcount = right->lcount;
//		node->rchild->parent = node;
//		right->parent = node->parent;
//		if (right->parent == m_null) {
//			m_root = right;
//		}else if (node == node->parent->lchild) {
//			node->parent->lchild = right;
//		}else {
//			node->parent->rchild = right;
//		}
//		right->lchild = node;
//		right->lcount += node->lcount + 1;
//		node->parent = right;
//	}
//
//	inline void rightRotate(RBTNode* node) {
//		//把一个节点向右下方移一格，并让他原来的左子节点代替它的位置。
//		RBTNode* left = node->lchild;
//		node->lchild = left->rchild;
//		node->lcount = left->rcount;
//		node->lchild->parent = node;
//		left->parent = node->parent;
//		if (left->parent == m_null) {
//			m_root = left;
//		}else if (node == node->parent->lchild) {
//			node->parent->lchild = left;
//		}else {
//			node->parent->rchild = left;
//		}
//		left->rchild = node;
//		left->rcount += node->rcount + 1;
//		node->parent = left;
//	}
//
//	RBTNode* treeMax(RBTNode* root) {//找到子树中最大的一个节点
//		RBTNode* result = root;
//		while (result->rchild != m_null) {
//			result = result->rchild;
//		}
//		return result;
//	}
//
//	RBTNode* treeMin(RBTNode* root) {//找到子树中最小的一个节点
//		RBTNode* result = root;
//		while (result->lchild != m_null) {
//			result = result->lchild;
//		}
//		return result;
//	}
//public:
//	RBT() {
//		m_null = new RBTNode;
//		m_null->color = BLACK;
//		m_null->lchild = m_null->rchild = m_null;
//		m_root = m_null;
//	}
//	~RBT() {
//		clear();
//		delete m_null;
//	}
//
//	RBTNode* atIndex(int i) {//找到从小到大排序后下标为i的节点。i从0开始。
//		RBTNode* result = m_root;
//		if (i > result->lcount + result->rcount) {
//			result = NULL;
//		}else {
//			while (i != result->lcount) {
//				if (i < result->lcount) {
//					result = result->lchild;
//				}else {
//					i -= result->lcount + 1;
//					result = result->rchild;
//				}
//			}
//		}
//		return result;
//	}
//
//	void del(RBTNode* node) {//删除一个节点
//		RBTNode* toDel = node;
//		if (node->lchild != m_null && node->rchild != m_null) {
//			toDel = treeNext(node);//找到中序后继：即右子树最左节点
//		}
//		RBTNode* temp = toDel;
//		while (temp->parent != m_null) {
//			if (temp == temp->parent->lchild) {
//				temp->parent->lcount--;
//			}else {
//				temp->parent->rcount--;
//			}
//			temp = temp->parent;
//		}
//		RBTNode* replace = toDel->lchild != m_null? toDel->lchild: toDel->rchild;
//		replace->parent = toDel->parent;
//		if (replace->parent == m_null) {
//			m_root = replace;
//		}else if (toDel == toDel->parent->lchild) {
//			replace->parent->lchild = replace;
//		}else {
//			replace->parent->rchild = replace;
//		}
//		if (toDel != node) {
//			node->key = toDel->key;
//		}
//		if (toDel->color == BLACK) {
//			//修改树，以保持平衡。
//			delFixup(replace);
//		}
//		delete toDel;
//	}
//
//	void insert(const Key& key) {//插入一个节点
//		RBTNode* node = new RBTNode;
//
//		node->key = key;
//		node->lcount = 0;
//		node->rcount = 0;
//		node->lchild = m_null;
//		node->rchild = m_null;
//		node->color = RED;
//
//		RBTNode* p = m_root;
//		RBTNode* leaf = m_null;
//		while (p != m_null) {
//			leaf = p;
//			if (keyCmp(node->key, p->key) < 0) {
//				p->lcount++;
//				p = p->lchild;
//			}else {
//				p->rcount++;
//				p = p->rchild;
//			}
//		}
//		node->parent = leaf;
//		if (leaf == m_null) {//如果是空树。
//			m_root = node;
//		}else if (keyCmp(node->key, leaf->key) < 0) {
//			leaf->lchild = node;
//		}else {
//			leaf->rchild = node;
//		}
//		//修改树，以保持平衡。
//		insertFixup(node);
//	}
//	int nodeCount() {
//		return m_root != m_null? m_root->lcount + m_root->rcount + 1: 0;
//	}
//
//	RBTNode* search(const Key& key) {//按照key查找一个节点。
//		RBTNode* result = m_root;
//		while (result != m_null && keyCmp(key, result->key) != 0) {
//			result = keyCmp(key, result->key) < 0 ? result->lchild : result->rchild;
//		}
//		return result == m_null? NULL: result;
//	}
//
//	void toArray(int* array) {//把树中节点的值放进一个数组。
//		RBTNode* p = treeMin(m_root);
//		int i = 0;
//		while (p != m_null) {
//			array[i] = p->key.value;
//			i++;
//			p = treeNext(p);
//		}
//	}
//
//	RBTNode* treeNext(RBTNode* node) {//一个节点在中序遍列中的下一个节点。后继
//		RBTNode* result;
//		if (node->rchild != m_null) {
//			result = treeMin(node->rchild);
//		}else {
//			result = node->parent;
//			RBTNode* temp = node;
//			while (result != m_null && temp == result->rchild) {
//				temp = result;
//				result = result->parent;
//			}
//		}
//		return result;
//	}
//
//	RBTNode* treePre(RBTNode* node) {//一个节点在中序遍列中的前一个节点。前驱
//		RBTNode* result;
//		if (node->lchild != m_null) {
//			result = treeMax(node->lchild);
//		}else {
//			result = node->parent;
//			RBTNode* temp = node;
//			while (result != m_null && temp == result->lchild) {
//				temp = result;
//				result = result->parent;
//			}
//		}
//		return result;
//	}
//};

//
//
//template <class TKey, class TValue> 
//RBTree<TKey, TValue>::~RBTree()
//{ 
//	if(m_rpRoot)
//	{
//		delete_tree(*m_rpRoot);
//	}
//}
//template <class TKey, class TValue> 
//void RBTree<TKey, TValue>::delete_tree(Node& kNode)
//{
//	if(kNode._rpleft) 
//	{
//		delete_tree(kNode._rpleft);
//	}
//	if(kNode._rpright) 
//	{
//		delete_tree(kNode._rpright);
//	}
//}
//// Insert & fixup 
//template <class TKey, class TValue> 
//bool RBTree<TKey, TValue>::Insert(Node& kNode)
//{ 
//	if(m_rpRoot == NULL)
//	{
//		m_rpRoot = kNode;
//		return;
//	}
//	//m_rpRoot ->parent = this ->root; 
//	//this ->root ->right = new Node<T>(); 
//	//this ->root ->right ->parent = this ->root; 
//	//if(m_rpRoot == NULL || this ->root ->isNULL()){ 
//	//	if(this ->root) 
//	//		delete this ->root; 
//	//	this ->root = new Node<T>(t); 
//	//	this ->root ->left = new Node<T>(); 
//	//	this ->root ->left ->parent = this ->root; 
//	//	this ->root ->right = new Node<T>(); 
//	//	this ->root ->right ->parent = this ->root; 
//	//	this ->root ->setBlack(); 
//	//	return true; 
//	//} 
//	Node* rpNode = m_rpRoot; 
//	while(1)
//	{ 
//		if(kNode._kKey == rpNode->_kKey) 
//		{
//			return false; 
//		}
//		if(kNode._kKey == rpNode->_kKey)
//		{ 
//			if(p ->left ->isNULL())
//			{ 
//				delete p ->left; 
//				p ->left = new Node<T>(t); 
//				p ->left ->parent = p; 
//				p ->left ->left = new Node<T>(); 
//				p ->left ->left ->parent = p ->left; 
//				p ->left ->right = new Node<T>(); 
//				p ->left ->right ->parent = p ->left; 
//				this ->insert_fixup(p ->left); 
//				return true; 
//			} 
//			else 
//				p = p ->left; 
//		} 
//		else{ 
//			if(p ->right ->isNULL()){ 
//				delete p ->right; 
//				p ->right = new Node<T>(t); 
//				p ->right ->parent = p; 
//				p ->right ->left = new Node<T>(); 
//				p ->right ->left ->parent = p ->left; 
//				p ->right ->right = new Node<T>(); 
//				p ->right ->right ->parent = p ->left; 
//				this ->insert_fixup(p ->right); 
//				return true; 
//			} 
//			else 
//				p = p ->right; 
//		} 
//	} 
//	return false; 
//}
//template <typename T, typename E> 
//void RBTree<T, E>::insert_fixup(Node<T>* z){ 
//	Node<T>* y;
//	while(z ->parent && z ->parent ->isRed()){ 
//		if(z ->parent == z ->parent ->parent ->left){ 
//			y = z ->parent ->parent ->right; 
//			if(y ->isRed()){ 
//				//case 1 
//				z ->parent ->setBlack(); 
//				y ->setBlack(); 
//				z ->parent ->parent ->setRed(); 
//				z = z ->parent ->parent; 
//			} 
//			else{  
//				if(z == z ->parent ->right){ 
//					// case 2 ( fall through to case 3 ) 
//					z = z ->parent; 
//					this ->left_rotate(z); 
//				} 
//				// case 3 
//				z ->parent ->setBlack(); 
//				z ->parent ->parent ->setRed(); 
//				this ->right_rotate(z ->parent ->parent); 
//			} 
//		} 
//		else{ 
//			y = z ->parent ->parent ->left; 
//			if(y ->isRed()){ 
//				// case 4 
//				z ->parent ->setBlack(); 
//				y ->setBlack(); 
//				z ->parent ->parent ->setRed(); 
//				z = z ->parent ->parent; 
//			} 
//			else{  
//				if(z == z ->parent ->left){ 
//					// case 5 ( fall through to case 6 ) 
//					z = z ->parent; 
//					this ->right_rotate(z); 
//				} 
//				// case 6 
//				z ->parent ->setBlack(); 
//				z ->parent ->parent ->setRed(); 
//				this ->left_rotate(z ->parent ->parent); 
//			} 
//		} 
//	} 
//	this ->root ->setBlack(); 
//} 
//// delete & fixup 
//template <typename T, typename E> 
//bool RBTree<T, E>::Delete(T &t, E e){ 
//	Node<T>* p = this ->root; 
//	while(p ->isExist()){ 
//		if(p ->data == e) 
//			return this ->delete_node(t, p); 
//		else if(e < p ->data) 
//			p = p ->left; 
//		else 
//			p = p ->right; 
//	} 
//	return false; 
//}
//
//template <typename T, typename E> 
//bool RBTree<T, E>::delete_node(T& t, Node<T>* z){ 
//	Node<T>* y; 
//	Node<T>* x;
//	if(z ->left ->isNULL() || z ->right ->isNULL()) 
//		y = z; 
//	else 
//		y = z ->successor(); 
//	if(y ->left ->isExist()){ 
//		x = y ->left; 
//		if(y ->right ->isNULL()) 
//			delete y ->right; 
//	} 
//	else{ 
//		x = y ->right; 
//		if(y ->left ->isNULL()) 
//			delete y ->left; 
//	} 
//	x ->parent = y ->parent; 
//	if(y ->parent == NULL) 
//		this ->root = x; 
//	else if(y == y ->parent ->left) 
//		y ->parent ->left = x; 
//	else 
//		y ->parent ->right = x; 
//	if(y != z){ 
//		T temp = z ->data; 
//		z ->data = y ->data; 
//		y ->data = temp; 
//	} 
//	if(y ->isBlack()) 
//		this ->delete_fixup(x); 
//	t = y ->data;
//	delete y;
//	return true; 
//}
//template <typename T, typename E> 
//void RBTree<T, E>::delete_fixup(Node<T>* x){ 
//	Node<T>* w; 
//	while(x != this ->root && x ->isBlack()){ 
//		if(x == x ->parent ->left){ 
//			w = x ->parent ->right; 
//			if(w ->isRed()){ 
//				// case 1 ( fall through ) 
//				w ->setBlack(); 
//				x ->parent ->setRed(); 
//				this ->left_rotate(x ->parent); 
//				w = x ->parent ->right; 
//			} 
//			if(w ->left ->isBlack() && w ->right ->isBlack()){ 
//				// case 2 ( new circle with x's value changed ) 
//				w ->setRed(); 
//				x = x ->parent; 
//			} 
//			else{ 
//				if(w ->right ->isBlack()){ 
//					// case 3 ( fall through to case 4 ) 
//					w ->left ->setBlack(); 
//					w ->setRed(); 
//					this ->right_rotate(w); 
//					w = x ->parent ->right; 
//				} 
//				// case 4 ( exit the while with x = root ) 
//				w ->setColor(x ->parent ->getColor()); 
//				x ->parent ->setBlack(); 
//				w ->right ->setBlack(); 
//				this ->left_rotate(x ->parent); 
//				x = this ->root; 
//			} 
//		} 
//		else{ 
//			w = x ->parent ->left; 
//			if(w ->isRed()){ 
//				// case 5 ( fall through ) 
//				w ->setBlack(); 
//				x ->parent ->setRed(); 
//				this ->right_rotate(x ->parent); 
//				w = x ->parent ->left; 
//			} 
//			if(w ->left ->isBlack() && w ->right ->isBlack()){ 
//				// case 6 ( new circle with x's value changed ) 
//				w ->setRed(); 
//				x = x ->parent; 
//			} 
//			else{  
//				if(w ->left ->isBlack()){ 
//					// case 7 ( fall through to case 8 ) 
//					w ->right ->setBlack(); 
//					w ->setRed(); 
//					this ->left_rotate(w); 
//					w = x ->parent ->left; 
//				} 
//				// case 8 ( exit the while with x = root) 
//				w ->setColor(x ->parent ->getColor()); 
//				x ->parent ->setBlack(); 
//				w ->left ->setBlack(); 
//				this ->right_rotate(x ->parent); 
//				x = this ->root; 
//			} 
//		} 
//	} 
//	x ->setBlack(); 
//}