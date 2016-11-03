//{future header message}
#ifndef __FvDoubleLink_H__
#define __FvDoubleLink_H__

#include "FvDebug.h"

//+------------------------------------------------------------------------------------------------------------

template <class TNode>
class FvDoubleLinkRoot
{
	FV_NOT_COPY_COMPARE(FvDoubleLinkRoot);
public:
	bool IsAttach()const{return (m_Pre != NULL);}
	void Detach();

	TNode* Pre()const{return m_Pre;}
	TNode* Next()const{return m_Next;}

protected:
	FvDoubleLinkRoot():m_Pre(NULL), m_Next(NULL){}
	~FvDoubleLinkRoot(){Detach();}

private:

	TNode* m_Pre;
	TNode* m_Next;

	template <class, template<class >class> friend class FvDoubleLink;
};
//+------------------------------------------------------------------------------------------------------------
template <class T>
class FvDoubleLinkNode1: public FvDoubleLinkRoot<FvDoubleLinkNode1<T>>
{
public:
	FvDoubleLinkNode1(){}
	~FvDoubleLinkNode1(){}

	T Content(){ return static_cast<T>(this); }
	template <class, template<class >class> friend class FvDoubleLink;
};
template <class T>
class FvDoubleLinkNode2: public FvDoubleLinkRoot<FvDoubleLinkNode2<T>>
{
public:
	FvDoubleLinkNode2(const T& kT = T()):m_Content(kT){}
	template<class U>
	FvDoubleLinkNode2(U& kU):m_Content(kU){}
	~FvDoubleLinkNode2(){}

	T& Content(){ return m_Content;}
	T m_Content;
	template <class, template<class >class> friend class FvDoubleLink;
};
//+------------------------------------------------------------------------------------------------------------
template <class T, template<class> class TNode = FvDoubleLinkNode2>
class FvDoubleLink
{
	FV_NOT_COPY_COMPARE(FvDoubleLink);
public:
	typedef TNode<T> Node;
	typedef Node* Iterator;

	FvDoubleLink();
	~FvDoubleLink();

	void PushBack(Node& kNode);
	void PushFront(Node & kNode);
	void PushBack(FvDoubleLink& kList);
	void PushFront(FvDoubleLink & kList);
	void Clear();

	static void PushAfter(Node& qPreNode, Node& qNewNode);
	static void PushBefore(Node& qNextNode, Node& qNewNode);
	static void PushAfter(Node& qPreNode, FvDoubleLink& kList);
	static void PushBefore(Node& qNextNode, FvDoubleLink& kList);

	bool IsEmpty()const{ return (RootNode().m_Next == &RootNode());}
	bool IsNotEmpty()const{return !IsEmpty();}
	static FvUInt32 GetSize(const FvDoubleLink& kList);

	bool IsEnd(const Iterator iter)const{return (iter == &RootNode());}

	Iterator GetHead()const{return RootNode().m_Next;}
	static Iterator Next(Iterator& iter)
	{
		FV_ASSERT(iter && iter->m_Next);
		return iter = iter->m_Next;
	}

	Iterator GetTail()const{return RootNode().m_Pre;}
	static Iterator Pre(Iterator& iter)
	{
		FV_ASSERT(iter && iter->m_Pre);
		return iter = iter->m_Pre;
	}

	template<class TVisitor, void (TVisitor::*func)(T&)>
	void Each(TVisitor& qVisitor)const;
	template<void (*func)(T&)>
	void Each()const;

private:

	static void _PushAfter(Node& qPreNode, Node& qNewNode);
	static void _PushBefore(Node& qNextNode, Node& qNewNode);
	static void _PushAfter(Node& qPreNode, FvDoubleLink& kList);
	static void _PushBefore(Node& qNextNode, FvDoubleLink& kList);
	static void _Link(Node& kPre, Node& kNext);

	void _Init();

	Node& RootNode(){return static_cast<Node&>(m_kRoot);}	
	const Node& RootNode()const{return static_cast<const Node&>(m_kRoot);}	
	FvDoubleLinkRoot<Node> m_kRoot;
};

//+------------------------------------------------------------------------------------------------------------
//+------------------------------------------------------------------------------------------------------------
template <class T>
class FvDoubleLinkNode3
{
	FV_NOT_COPY_COMPARE(FvDoubleLinkNode3);
public:

	FvDoubleLinkNode3()
	{
		m_Pre = this;
		m_Next = this;
	}
	~FvDoubleLinkNode3(){Detach();}

	bool IsAttach()const{return (m_Pre != this);}
	void Detach();
	void DetachAll();

	static void PushAfter(FvDoubleLinkNode3& qPreNode, FvDoubleLinkNode3& qNewNode);
	static void PushBefore(FvDoubleLinkNode3& qNextNode, FvDoubleLinkNode3& qNewNode);

	FvDoubleLinkNode3* GetPre()const{return m_Pre;}
	FvDoubleLinkNode3* GetNext()const{return m_Next;}

	T m_Content;

private:

	static void _PushAfter(FvDoubleLinkNode3& qPreNode, FvDoubleLinkNode3& qNewNode);
	static void _PushBefore(FvDoubleLinkNode3& qNextNode, FvDoubleLinkNode3& qNewNode);

	FvDoubleLinkNode3* m_Pre;
	FvDoubleLinkNode3* m_Next;
};

//+------------------------------------------------------------------------------------------------------------
template<class T>
class FvDoubleLinkNameSpace
{
public:
	typedef FvDoubleLink<T*, FvDoubleLinkNode1> List1;
	typedef FvDoubleLink<T*, FvDoubleLinkNode2> List2;
};

#include "FvDoubleLink.inl"
//+------------------------------------------------------------------------------------------------------------
//+------------------------------------------------------------------------------------------------------------
//+------------------------------------------------------------------------------------------------------------

namespace DoubleLink_Demo
{
	class TestClass
	{
	public:
		void Do(){}
		int m_iValue;
	};
	class TestVisitor
	{
	public:
		static void Do1(TestClass& kValue){}
		void Do2(TestClass& kValue){}
	};

	static void TestLink1()
	{
		typedef FvDoubleLinkNode1<TestClass> Node;
		typedef FvDoubleLink<TestClass, FvDoubleLinkNode2> Link;

		Link kLink;
		Node kNode1; kNode1;
		Node kNode2; kNode2;
		Node kNode3; kNode3;
		Node kNode4; kNode4;

	}
	static void TestLink2()
	{
		typedef FvDoubleLinkNode2<TestClass> Node;
		typedef FvDoubleLink<TestClass, FvDoubleLinkNode2> Link;

		Link kLink;
		Node kNode1; kNode1.m_Content.m_iValue = 1;
		Node kNode2; kNode2.m_Content.m_iValue = 2;
		Node kNode3; kNode3.m_Content.m_iValue = 3;
		Node kNode4; kNode4.m_Content.m_iValue = 4;

		///<插入>
		kLink.PushBack(kNode1);
		Link::PushAfter(kNode1, kNode2);
		kLink.PushFront(kNode3);
		Link::PushBefore(kNode3, kNode4);

		kLink.PushBack(kNode1);

		{///<正向迭代>
			Link::Iterator iter = kLink.GetHead();
			while (!kLink.IsEnd(iter))
			{
				TestClass& kValue = (*iter).m_Content;
				Link::Next(iter);
				///<使用>
				kValue;
				///</使用>
			}
		}
		{///<反向迭代>
			Link::Iterator iter = kLink.GetTail();
			while (!kLink.IsEnd(iter))
			{
				TestClass& kValue = (*iter).m_Content;
				Link::Pre(iter);
				///<使用>
				kValue;
				///</使用>
			}
		}

		///<访问>
		TestVisitor kVisitor;
		kLink.Each<TestVisitor, &TestVisitor::Do2>(kVisitor);
		kLink.Each<TestVisitor::Do1>();

		///<移除>
		kNode1.Detach();
		FV_ASSERT(kNode1.IsAttach() == false);

		///<解散>
		FV_ASSERT(kLink.IsEmpty() == false);
		kLink.Clear();
		FV_ASSERT(kLink.IsEmpty());

		Link kLink_1;
		kLink_1.PushBack(kNode1);
		kLink_1.PushFront(kNode2);
		Link kLink_2;
		kLink_2.PushBack(kNode3);
		kLink_2.PushFront(kNode4);

		kLink_1.PushBack(kLink_2);

		kLink_2.PushBack(kNode3);
		kLink_2.PushFront(kNode4);
		kLink_1.PushFront(kLink_2);

	}

	static void TestLink3()
	{
		typedef FvDoubleLinkNode3<int> Node;
		Node kNode1; kNode1.m_Content = 1;
		Node kNode2; kNode2.m_Content = 2;
		Node kNode3; kNode3.m_Content = 3;
		Node kNode4; kNode4.m_Content = 4;

		///<插入>
		Node::PushAfter(kNode1, kNode2);///<1, 2>
		Node::PushAfter(kNode1, kNode3);///<1, 3, 2>
		Node::PushBefore(kNode3, kNode4);///<1, 4, 3, 2>
		Node::PushAfter(kNode1, kNode2);///<1, 2, 4, 3>

		///<迭代>
		Node* pkPre = kNode1.GetPre();
		while(pkPre != &kNode1)
		{
			int iValue = pkPre->m_Content;
			pkPre = pkPre->GetPre();
		}

		Node* pkNext = kNode1.GetNext();
		while(pkNext != &kNode1)
		{
			int iValue = pkNext->m_Content;
			pkNext = pkNext->GetNext();
		}

		///<移除>
		kNode1.Detach();
		FV_ASSERT(kNode1.IsAttach() == false);

		///<解散>
		kNode2.DetachAll();
		FV_ASSERT(kNode2.IsAttach() == false);
		FV_ASSERT(kNode2.IsAttach() == false);
		FV_ASSERT(kNode3.IsAttach() == false);
	}
	static void Test()
	{
		TestLink2();
		TestLink3();
	}
}




#endif //__FvDoubleLink_H__