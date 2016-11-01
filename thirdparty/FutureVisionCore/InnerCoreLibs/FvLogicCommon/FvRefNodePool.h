//{future header message}
#ifndef __FvRefNodePool_H__
#define __FvRefNodePool_H__


#include "FvRefList.h"

template<class T>
class FvRefNodePool
{
public:

	static T* Create()
	{
		if(S_List.Size() > 0)
		{
			FvRefNode2<T*>* pkIter = S_List.GetHead();
			pkIter->Detach();
			return pkIter->m_Content;
		}
		else
		{
			return new T();
		}
	}
	static void Destroy(T*& p, FvRefNode2<T*>& kNode)
	{
		FV_ASSERT(kNode.m_Content == p);
		if(kNode.m_Content == p)
		{
			p = NULL;
			S_List.AttachBack(kNode);
		}
		else
		{
			delete p;
			p = NULL;
		}
	}	

	template<class TDestroyer, void (TDestroyer::*func)()>
	static void Clear()
	{
		S_List.Each<TDestroyer, func>();
	}

	template<void (*func)(T*&)>
	static void Clear()
	{
		S_List.Each<func>();
	}

	static void Clear()
	{
		S_List.Each<FvRefNodePool::_Delete>();
	}

private:

	static void _Delete(T*& pk)
	{
		if(pk)
		{
			delete pk;
			pk = NULL;
		}
	}

	FvRefNodePool(void);
	~FvRefNodePool(void);

	static FvRefList<T*> S_List;
};

#define _FRIEND_REF_NODE_POOL_ template <class > friend class FvRefNodePool;

template <class T>
FvRefList<T*> FvRefNodePool<T>::S_List;

//+-------------------------------------------------------------------------------------------------
///<使用说明>
namespace RefNodePool_Demo
{

	class TestClass1
	{
	public:
		static void End(TestClass1*& pThis)
		{
			TestClass1* p = pThis;
			pThis = NULL;
			delete p;
		}
		FvRefNode2<TestClass1*> m_kAttachNode;

	private:
		TestClass1(){m_kAttachNode.m_Content = this;}
		~TestClass1(){}
		_FRIEND_REF_NODE_POOL_;
	};

	class TestClass2: public FvRefNode2<TestClass2*>
	{
	public:
		static void End(TestClass2*& pThis)
		{
			TestClass2* p = pThis;
			pThis = NULL;
			delete p;
		}
	private:
		TestClass2(){m_Content = this;}
		~TestClass2(){}
		_FRIEND_REF_NODE_POOL_;
	};

	static void Test()
	{
		{
			TestClass1* pkNew1 = FvRefNodePool<TestClass1>::Create();
			FV_ASSERT(pkNew1);
			TestClass1* pkNew2 = FvRefNodePool<TestClass1>::Create();
			FV_ASSERT(pkNew2);
			FvRefNodePool<TestClass1>::Destroy(pkNew1, pkNew1->m_kAttachNode);
			TestClass1* pkNew3 = FvRefNodePool<TestClass1>::Create();
			FV_ASSERT(pkNew3);
			FvRefNodePool<TestClass1>::Destroy(pkNew2, pkNew2->m_kAttachNode);
			FvRefNodePool<TestClass1>::Destroy(pkNew3, pkNew3->m_kAttachNode);
			FvRefNodePool<TestClass1>::Clear<TestClass1::End>();
		}
		//
		{
			TestClass2* pkNew1 = FvRefNodePool<TestClass2>::Create();
			FV_ASSERT(pkNew1);
			TestClass2* pkNew2 = FvRefNodePool<TestClass2>::Create();
			FV_ASSERT(pkNew2);
			FvRefNodePool<TestClass2>::Destroy(pkNew1, *pkNew1);
			TestClass2* pkNew3 = FvRefNodePool<TestClass2>::Create();
			FV_ASSERT(pkNew3);
			FvRefNodePool<TestClass2>::Destroy(pkNew2, *pkNew2);
			FvRefNodePool<TestClass2>::Destroy(pkNew3, *pkNew3);
			FvRefNodePool<TestClass2>::Clear<TestClass2::End>();
		}

	}
}



#endif //__FvRefNodePool_H__
