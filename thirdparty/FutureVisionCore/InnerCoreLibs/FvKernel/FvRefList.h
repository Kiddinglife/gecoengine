//{future header message}
#ifndef __FvRefList_H__
#define __FvRefList_H__

#include "FvKernel.h"
#include "FvDebug.h"

template <class, template<class >class> class FvRefList;

//+------------------------------------------------------------------------------------------------------------------------
template <class TList, class TNode>
class FvRefRoot
{
public:
	bool IsAttach()const{return (m_rpRefList != NULL);}
	bool IsAttach(const TList& qList)const{return (m_rpRefList == &qList);}
	TNode* Next()const{return m_rpNextNode;}
	TNode* Pre()const {return m_rpPreNode;}
	TList* GetRefList() const { return m_rpRefList; }

protected:
	FvRefRoot():m_rpPreNode(NULL), m_rpNextNode(NULL), m_rpRefList(NULL){}
	~FvRefRoot(){}

	TList* m_rpRefList;
private:

	TNode* m_rpPreNode;
	TNode* m_rpNextNode;

	FV_DEFINE_COPY_CONSTRUCTOR(FvRefRoot);
	FV_DEFINE_COPY(FvRefRoot);
	FV_DEFINE_COMPARE(FvRefRoot);
	template <class, template<class >class> friend class FvRefList;
};
//+------------------------------------------------------------------------------------------------------------------------
template <class T>
class FvRefNode1: public FvRefRoot<FvRefList<T, FvRefNode1>, FvRefNode1<T>>
{
public:
	FvRefNode1(){}
	~FvRefNode1(){Detach();}
	void Detach();
	FvRefNode1* GetNextContent()const;
	T Content(){ return static_cast<T>(this); }
private:
	template <class, template<class >class> friend class FvRefList;
};
//+------------------------------------------------------------------------------------------------------------------------
template <class T>
class FvRefNode2: public FvRefRoot<FvRefList<T, FvRefNode2>, FvRefNode2<T>>
{
public:
	FvRefNode2(T& kT):m_Content(kT){}
	FvRefNode2():m_Content(NULL){}
	template<class U>
	FvRefNode2(U& kU):m_Content(kU){}
	~FvRefNode2(){Detach();}
	void Detach();
	const T& GetNextContent(const T& kDefaultValue)const;
	T& GetNextContent(T& kDefaultValue);
	T& Content(){ return m_Content; }
	T m_Content;
private:
	template <class, template<class >class> friend class FvRefList;
};
//+------------------------------------------------------------------------------------------------------------------------
//! 双向链表
template <class T, template<class> class TNode = FvRefNode2> 
class FvRefList
{
public:
	//+----------------------------------------------
	template <class> friend class FvRefNode1;
	template <class> friend class FvRefNode2;
	typedef TNode<T> Node;
	typedef Node* iterator;

	FvRefList();
	~FvRefList(){Clear();}

	void AttachBack(Node & kNode);
	void AttachFront(Node & kNode);
	void AttachBack(FvRefList & kList);
	void AttachFront(FvRefList & kList);
	void AttachAfter(Node & kFront, Node & kNew);
	void AttachBefore(Node & kAfter, Node & kNew);
	void AttachAfter(Node & kFront, FvRefList & kList);
	void AttachBefore(Node & kAfter, FvRefList & kList);

	iterator GetIterator()const{return m_rpNext;}
	void BeginIterator()const;
	void Next()const{m_rpNext = _GetNext(m_rpNext);}
	void EndIterator()const{m_rpNext = &const_cast<Node&>(static_cast<const Node&>(m_kRoot));}
	bool IsEnd()const{return (m_rpNext == &_Root());}

	iterator GetHead()const{return _Root().m_rpNextNode;}
	iterator GetTail()const{return _Root().m_rpPreNode;}
	static void Next(iterator& iter){iter = _GetNext(iter);}
	bool IsEnd(const iterator iter)const{return (iter == &_Root());}

	void Clear();
	void _Destroy();
	void _DestroyContent();
	FvUInt32 Size()const{return m_uiSize;}
	bool IsEmpty()const{return (_Root().m_Next == &_Root());}

	//+-----------------------------------------------------------------------------
	template<class TVisitor, void (TVisitor::*func)(T&)>
	void Each(TVisitor& qVisitor)const;

	template<void (*func)(T&)>
	void Each()const;

	template<class TContent, void (TContent::*func)()>
	void Each()const;

	//+-----------------------------------------------------------------------------
	///<后门接口>但是依旧是安全的
	void AttachBackSafeIterator(Node & kNode);

protected:
	static iterator _GetNext(const iterator iter){ FV_ASSERT(iter && iter->m_rpNextNode); return iter->m_rpNextNode; }
	static iterator _GetPre(const iterator iter){return iter->m_rpPreNode;	}
	void _Detach(Node & qT);
	static void _PushBefore(Node& kNext, Node& kNew);
	static void _PushAfter(Node& kFront, Node& kNew);
	static void _Link(Node& pre, Node& next);

protected:
	Node& _Root(){return static_cast<Node&>(m_kRoot);}	
	const Node& _Root()const{return static_cast<const Node&>(m_kRoot);}

private:
	void _Init();

	mutable iterator m_rpNext;
	FvRefRoot<FvRefList, Node> m_kRoot;
	FvUInt32 m_uiSize;

	FV_DEFINE_COPY_CONSTRUCTOR(FvRefList);
	FV_DEFINE_COPY(FvRefList);
	FV_DEFINE_COMPARE(FvRefList);
};


//+------------------------------------------------------------------------------------------------------------------------
template<class T>
class FvRefListNameSpace
{
public:
	typedef FvRefList<T*, FvRefNode1> List1;
	typedef FvRefList<T*, FvRefNode2> List2;
};

#include "FvRefList.inl"

//+------------------------------------------------------------------------------------------------------------------------
//+------------------------------------------------------------------------------------------------------------------------
///<测试用例>
namespace FvRefList_Demo
{
	class _Class
	{
	public:
		void Do(){}
		static void S_Do(_Class*&){}
		void Destroy(){delete this;}
		static void S_Destroy(_Class*& pkThis)
		{
			_Class* pkThis_ = pkThis;
			pkThis = NULL;
			delete pkThis_; 
		}
	};

	static void TestAttach()
	{
		typedef FvRefNode2<int> Node;
		typedef FvRefList<int, FvRefNode2> List;

		List kLink;
		Node kNode1; kNode1.m_Content = 1;
		Node kNode2; kNode2.m_Content = 2;
		Node kNode3; kNode3.m_Content = 3;
		Node kNode4; kNode4.m_Content = 4;

		///<插入>
		kLink.AttachBack(kNode1);///<1>
		kLink.AttachAfter(kNode1, kNode2);///<1, 2>
		kLink.AttachFront(kNode3);///<3, 1, 2>
		kLink.AttachBefore(kNode3, kNode4);///<4, 3, 1, 2>

		kLink.AttachBack(kNode1);///<4, 3, 2, 1>
		kLink.AttachFront(kNode2);///<2, 4, 3, 1>

		///<迭代>
		List::iterator iter = kLink.GetHead();
		while (!kLink.IsEnd(iter))
		{
			//int& kValue = (*iter).m_Content;
			List::Next(iter);
		}

		///<访问>
		//TestVisitor kVisitor;
		//kLink.Each<TestVisitor, &TestVisitor::Do2>(kVisitor);
		//kLink.Each<TestVisitor::Do1>();

		///<移除>
		kNode1.Detach();
		FV_ASSERT(kNode1.IsAttach() == false);

		///<解散>
		FV_ASSERT(kLink.Size() != 0);
		kLink.Clear();
		FV_ASSERT(kLink.Size() == 0);

		List kLink_1;
		kLink_1.AttachBack(kNode1);
		kLink_1.AttachFront(kNode2);
		List kLink_2;
		kLink_2.AttachBack(kNode3);
		kLink_2.AttachFront(kNode4);

		kLink_1.AttachBack(kLink_2);

		kLink_2.AttachBack(kNode3);
		kLink_2.AttachFront(kNode4);
		kLink_1.AttachFront(kLink_2);

	}
	static void Test()
	{
		TestAttach();

		//! 内置迭代器
		typedef FvRefList<_Class*, FvRefNode2> List;
		List kList;
		kList.BeginIterator();
		while (!kList.IsEnd())
		{
			List::iterator iter = kList.GetIterator();
			_Class* pkObj = (*iter).m_Content;
			FV_ASSERT(pkObj);
			kList.Next();
			///<使用>
			pkObj;
			///</使用>
		}

		//! 内置访问
		kList.Each<_Class::S_Do>();
		kList.Each<_Class::S_Destroy>();//! 可以直接调用析构

		//! 内置访问
		kList.Each<_Class, &_Class::Do>();
		kList.Each<_Class, &_Class::Destroy>();//! 可以直接调用析构

		//! 外置迭代器
		List::iterator iter = kList.GetHead();
		while (!kList.IsEnd(iter))
		{
			_Class* pkObj = (*iter).m_Content;
			FV_ASSERT(pkObj);
			List::Next(iter);		
			///<使用>
			pkObj;
			///</使用>
		}
	}
}

#endif //__FvRefList_H__

