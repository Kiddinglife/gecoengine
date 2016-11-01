//{future header message}
#ifndef __FvMemoryNode_H__
#define __FvMemoryNode_H__

#include "FvLogicCommonDefine.h"

#include <FvRefList.h>
#include <string>
#include "FvLogBus.h"

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class FvMemoryNode;
class FV_LOGIC_COMMON_API FvMemoryNode: private FvRefNode1<FvMemoryNode*>
{
public:
	static void Print();
	typedef FvRefListNameSpace<FvMemoryNode>::List1 List;

	void AddRef();
	void DelRef();
	FvInt32 RefCnt()const;

protected:
	FvMemoryNode();
	~FvMemoryNode();

	FvInt32 m_iCnt;
	static List& S_List(){static List s_kList; return s_kList;}

	virtual  void _Print() = 0;

	//
	template <class> friend class FvRefNode1;
};

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class T>
class TMemoryNode: public FvMemoryNode
{
public:
	TMemoryNode()
	{}
	virtual void _Print()
	{
		if(m_iCnt)
		{
			FvLogBus::Warning("%s  [%d]", T::CLASS_NAME(), m_iCnt);
			FvTMemTrack<T>::Print();
		}
		else
		{
			FvLogBus::CritOk("%s  [%d]", T::CLASS_NAME(), m_iCnt);
		}
	}
private:
	static const char* Name();
};

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class T>
class FvTMemTrack: private FvRefNode1<FvTMemTrack<T>*>
{
public:
	typedef typename FvRefListNameSpace<FvTMemTrack>::List1 List;

	FvTMemTrack():m_rpT(NULL)
	{
		S_LIST.AttachBack(*this);
		S_Node().AddRef();
	}

	~FvTMemTrack()
	{
		S_Node().DelRef();
	}

	void Track(T& kT)
	{
		m_rpT = &kT;
	}

	static void Print()
	{
		List& kLIst= S_LIST;
		List::iterator iter = kLIst.GetHead();
		while (!kLIst.IsEnd(iter))
		{
			FvTMemTrack* pkThis = iter->Content();
			List::Next(iter);		
			FV_ASSERT(pkThis);
		}
	}

	static TMemoryNode<T>& S_Node()
	{
		static TMemoryNode<T> S_NODE;
		return S_NODE;
	}

private:
	T* m_rpT;
	static List S_LIST;
};

template<class T>
typename FvRefListNameSpace<FvTMemTrack<T>>::List1 FvTMemTrack<T>::S_LIST;


//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define FV_MEM_TRACKER(CLASS) \
	FvTMemTrack<CLASS> MEM_TRACKER;\
	static const char* CLASS_NAME(){return #CLASS;}\
	template<class > friend class TMemoryNode;

#define FV_MEM_TRACK 	MEM_TRACKER.Track(*this)


//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///<使用说明>
namespace Mem_Demo
{

	class MemLeakTest
	{
	public:
		FV_MEM_TRACKER(MemLeakTest)
		MemLeakTest()
		{}
		~MemLeakTest()
		{}
	};

	static void Test()
	{
		//MemLeakTest* p = new MemLeakTest();
	}
}

#endif//__FvMemoryNode_H__