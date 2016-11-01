//{future header message}
#ifndef __FvDestroyManager_H__
#define __FvDestroyManager_H__


#include "FvLogicCommonDefine.h"
#include "FvMemoryNode.h"
#include <vector>

///<YISA>
//! Òì²½É¾³ý²ßÂÔ


//+------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvDeletable
{
	typedef void (FvDeletable::*funcDestroy)();
public:
	//enum _Type
	//{
	//	NORAML_DEL,
	//	REF_CNT_DEL,
	//	DEL_SUP,
	//};
	FvDeletable():m_pkObj(NULL)
	{
	}

	~FvDeletable()
	{
		FV_ASSERT(m_pkObj == NULL);
		FV_ASSERT(m_pDestroyFunc == NULL);
	}
	template <class T>
	void Destroy()
	{
		FV_ASSERT(m_pkObj);
		T* pkObj = (T*)m_pkObj;
		delete pkObj;
		m_pkObj = NULL;		
	}
	void Detach()
	{
		((*this).*m_pDestroyFunc)();
		m_pDestroyFunc = NULL;
	}
	void Attach(void* pkObj, funcDestroy pFunc)
	{
		FV_ASSERT(m_pkObj == NULL);
		m_pkObj = pkObj;
		m_pDestroyFunc = pFunc;
	}
	bool IsSame(void * pkObj)const
	{
		return pkObj == m_pkObj;
	}
protected:
	void* m_pkObj;
	funcDestroy m_pDestroyFunc;
	FV_MEM_TRACKER(FvDeletable);
};
//
//template<class T>
//class FvDeleteNode: public FvDeletable
//{
//public:
//	virtual void Detach()
//	{
//		FV_ASSERT(m_pkObj);
//		T* pkObj = (T*)m_pkObj;
//		delete pkObj;
//		m_pkObj = NULL;
//	}
//private:
//
//	virtual ~FvDeleteNode()
//	{
//		int k = 0;
//		k = 0;
//	}
//};

//
//template<class T, uint32 REF_CNT>
//class ViDeleteRefCntNode: public FvDeletable
//{
//public:
//	ViDeleteRefCntNode(T& qT):m_qT(&qT)
//	{
//		//FV_ASSERT(qT.ob_refcnt == REF_CNT);
//		//Py_INCREF(m_qT);
//	}
//	virtual ~ViDeleteRefCntNode()
//	{
//		//FV_ASSERT(m_qT->ob_refcnt == 1);
//		//Py_DECREF(m_qT);
//	}
//private:
//	T* m_qT;
//};

//+------------------------------------------------------------------------------------

//+------------------------------------------------------------------------------------

class FV_LOGIC_COMMON_API FvDestroyManager
{
	typedef std::vector<FvDeletable*> List;

public:

	static void Start(){}
	static void End()
	{
		FvDestroyManager& kThis = FvDestroyManager::Instance();
		kThis._UpdateDeleteList();
		kThis._End();
	}

	template<class T>
	static void Destroy(T& qT)
	{
		FvDestroyManager& kThis = FvDestroyManager::Instance();
		if(kThis._CheckDeletable(&qT))
		{
			List& kDeleteList = kThis.m_kDeleteList;
			List::iterator& kEndIter = kThis.m_kEndIter;
			if(kEndIter == kDeleteList.end())
			{
				FvDeletable* pDelete = new FvDeletable();
				pDelete->Attach(&qT, &FvDeletable::Destroy<T>);
				kDeleteList.push_back(pDelete);
				kEndIter = kDeleteList.end();
			}
			else
			{
				FvDeletable* pkDestroy = (*kEndIter);
				pkDestroy->Attach(&qT, &FvDeletable::Destroy<T>);
				++kEndIter;
			}
		}
	}

	template<class T>
	static void Own(T& qT)
	{
		FvDeletable* pDelete = new FvDeletable();
		pDelete->Attach(&qT, &FvDeletable::Destroy<T>);
		FvDestroyManager::Instance().m_kOwnList.push_back(pDelete);
	}

	//template<class T>
	//static void DestroyRefCnt(T& qT)
	//{
	//	ViDeleteRefCntNode<T, 2>* pDelete = new ViDeleteRefCntNode<T, 2>(qT);
	//	s_DeleteList.push_back(pDelete);
	//}
	//template<class T>
	//static void ProxyDestroyRefCnt(T& qT)
	//{
	//	ViDeleteRefCntNode<T, 3>* pDelete = new ViDeleteRefCntNode<T, 3>(qT);
	//	s_DeleteList.push_back(pDelete);
	//}
	static void UpdateDeleteList()
	{
		S_INSTANCE._UpdateDeleteList();
	}
private:

	FvDestroyManager();
	~FvDestroyManager();

	bool _CheckDeletable(void* ptr);
	void _End();
	void _UpdateDeleteList();

	static FvDestroyManager& Instance(){return S_INSTANCE;}
	static FvDestroyManager S_INSTANCE;

	List m_kDeleteList;
	List::iterator m_kEndIter;
	List m_kOwnList;

};

namespace Destroy_Demo
{
	class TestClass
	{
	public:
		TestClass(){}

	private:
		friend class FvDeletable;
		~TestClass(){}

	};

	static void Test()
	{
		TestClass* obj = new TestClass;
		FvDestroyManager::Destroy(*obj);
		FvDestroyManager::UpdateDeleteList();

		FvDestroyManager::End();
	}
}
#endif //__FvDestroyManager_H__