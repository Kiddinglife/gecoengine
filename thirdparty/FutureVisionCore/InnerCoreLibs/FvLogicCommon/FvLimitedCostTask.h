//{future header message}
#ifndef __FvLimitedCostTask_H__
#define __FvLimitedCostTask_H__

#include "FvMemoryNode.h"
#include <FvKernel.h>
#include <FvRefList.h>

template<class T, FvUInt32 (T::*func)()>
class FvLimitedCostTask: private FvRefNode1<FvLimitedCostTask<T, func>*>
{
	FV_MEM_TRACKER(FvLimitedCostTask);
	FV_NOT_COPY_COMPARE(FvLimitedCostTask);
public:
	typedef typename FvRefListNameSpace<FvLimitedCostTask>::List1 List;
	FvLimitedCostTask(T& kWorker)
		:m_kWoker(kWorker)
	{
		FV_MEM_TRACK;
	}
	~FvLimitedCostTask(void){}

	//+----------------------------------------------------------------------------------------------------
	void Jion()
	{
		if(FvRefNode1::IsAttach() == false)
		{
			S_LIST.AttachBack(*this);
		}
	}
	void Cancel()
	{
		FvRefNode1::Detach();
	}
	bool IsWaiting()const{return FvRefNode1::IsAttach();}

	//+----------------------------------------------------------------------------------------------------
	static void Tick()
	{
		FvUInt32 uiCost = 0;
		//
		S_LIST.BeginIterator();
		while (!S_LIST.IsEnd())
		{
			List::iterator iter = S_LIST.GetIterator();
			FvLimitedCostTask* pkTask = static_cast<FvLimitedCostTask*>(iter);
			S_LIST.Next();
			FV_ASSERT(pkTask);
			iter->Detach();
			uiCost += pkTask->_OnWork();
			if(uiCost >= S_COST_LIMIT)
			{
				S_LIST.EndIterator();
				//FvLogBus::CritOk("FvLimitedCostTask::Tick Cost上限已到");
				break;
			}
		}
	}

	//+----------------------------------------------------------------------------------------------------
	static void SetCost(const FvUInt32 uiCost)
	{
		S_COST_LIMIT = uiCost;
	}

private:

	FvUInt32 _OnWork()
	{
		const FvUInt32 uiCost = (m_kWoker.*func)();
		return uiCost;
	}

	T& m_kWoker;
	
	static FvUInt32 S_COST_LIMIT;
	static List S_LIST;
};



#define FV_LIMITED_COST_TASK_DEFINE(EXPORT_API, CLASS, FUNC)\
	template<>EXPORT_API\
	typename FvRefListNameSpace<FvLimitedCostTask<CLASS, &CLASS::FUNC>>::List1 FvLimitedCostTask<CLASS, &CLASS::FUNC>::S_LIST;\
	template<>EXPORT_API\
	FvUInt32 FvLimitedCostTask<CLASS, &CLASS::FUNC>::S_COST_LIMIT = 1000;

#define FV_LIMITED_COST_TASK_FRIEND_CLASS_\
	template<class T, FvUInt32 (T::*func)()> friend	class FvLimitedCostTask;

namespace LimitedCostTask_Demo
{

	class TestWorker
	{
	public:
		TestWorker():
#pragma warning(push)
#pragma warning(disable: 4355)
			m_kTask(*this)
#pragma warning(pop)
			{}

		FvUInt32 Work(){return 100;}
		FvLimitedCostTask<TestWorker, &Work> m_kTask;
	};


	static void Test()
	{
		FvLimitedCostTask<TestWorker, &TestWorker::Work>::SetCost(1000);

		TestWorker kWorker;
		kWorker.m_kTask.Jion();
		FvLimitedCostTask<TestWorker, &TestWorker::Work>::Tick();
	}

}



#endif //__FvLimitedCostTask_H__