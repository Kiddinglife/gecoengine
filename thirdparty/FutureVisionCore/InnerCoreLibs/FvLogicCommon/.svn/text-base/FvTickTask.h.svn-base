//{future header message}
#ifndef __FvTickTask_H__
#define  __FvTickTask_H__

#include "FvLogicCommonDefine.h"
#include "FvTimeNodeEx.h"
#include "FvMemoryNode.h"
#include <FvRefList.h>



template<class T, void(T::*func)(const float)>
class FvTickTask: public FvRefNode1<FvTickTask<T, func>*>
{
	FV_MEM_TRACKER(FvTickTask);
	FV_NOT_COPY_COMPARE(FvTickTask);
public:

	FvTickTask(T& kObj):m_kObj(kObj){FV_MEM_TRACK;}
	void Exec(const float fDeltaTime){(m_kObj.*func)(fDeltaTime);}
private:

	T& m_kObj;
};


template<class T, void(T::*func)(const float)>
class FvTickTaskList
{
public:
	typedef FvTickTask<T, func> Task;
	typedef FvRefList<Task*, FvRefNode1> List;
	void AttachBack(Task& kTask)
	{
		if(!kTask.IsAttach())
		{
			m_TaskList.AttachBack(kTask);
		}
	}

	void UpdateList(const float fDeltaTime)
	{
		m_TaskList.BeginIterator();
		while (!m_TaskList.IsEnd())
		{
			List::iterator iter = m_TaskList.GetIterator();
			Task* pkTask = static_cast<Task*>(iter);
			m_TaskList.Next();
			FV_ASSERT(pkTask);
			pkTask->Exec(fDeltaTime);
		}
	}

	FvTickTaskList()
		:
#pragma warning(push)
#pragma warning(disable: 4355)
		m_NextUpdateTime(*this)
#pragma warning(pop)
		{}

	void SetUpdateSpan(const FvUInt32 uiDeltaTime)
	{
		m_UpdateSpan = uiDeltaTime;
		FvGameTimer::SetTime(m_UpdateSpan, m_NextUpdateTime);
	}

private:

	void _OnUpdateTime(FvTimeEventNodeInterface&)
	{
		FV_ASSERT(FvGameTimer::Time() > m_uiLastTime);
		const float fDeltaTime = 0.01f*(FvGameTimer::Time() - m_uiLastTime);
		UpdateList(fDeltaTime);
		m_uiLastTime = FvGameTimer::Time();
		FvGameTimer::SetTime(m_UpdateSpan, m_NextUpdateTime);
	}
	List m_TaskList;

	FvUInt32 m_UpdateSpan;
	FvUInt64 m_uiLastTime;
	_FRIEND_CLASS_TimeNodeEvent_;
	FvTimeEventNode2<FvTickTaskList, &_OnUpdateTime> m_NextUpdateTime;

};
#define _FRIEND_CLASS_FvTickTask_ \
	template<class T, void(T::*func)(const float)> friend class FvTickTask;\
	template<class T, void(T::*func)(const float)> friend class FvTickTaskList;



class FV_LOGIC_COMMON_API FvTickNodeInterface: public FvRefNode1<FvTickNodeInterface*>
{
	FV_MEM_TRACKER(FvTickNodeInterface);
public:
	virtual void Exec(const float fDeltaTime) = 0;
protected:
	FvTickNodeInterface()
	{
		FV_MEM_TRACK;
	}
	~FvTickNodeInterface(){}
};

class FV_LOGIC_COMMON_API FvBaseTickTaskList
{
public:
	typedef FvRefList<FvTickNodeInterface*, FvRefNode1> List;

	FvBaseTickTaskList(){}
	~FvBaseTickTaskList(){}

	void AttachBack(FvTickNodeInterface& kTask)
	{
		if(!kTask.IsAttach())
		{
			m_TaskList.AttachBack(kTask);
		}
	}

	void UpdateList(const float fDeltaTime)
	{
		m_TaskList.BeginIterator();
		while (!m_TaskList.IsEnd())
		{
			List::iterator iter = m_TaskList.GetIterator();
			FvTickNodeInterface* pkTask = static_cast<FvTickNodeInterface*>(iter);
			m_TaskList.Next();
			FV_ASSERT(pkTask);
			pkTask->Exec(fDeltaTime);
		}
	}
private:

	List m_TaskList;
};

template<class T, void(T::*func)(const float)>
class FvTickNode: public FvTickNodeInterface
{
public:

	FvTickNode(T& kT):m_kObj(kT){}
	~FvTickNode(){}

	void Exec(const float fDeltaTime)	{(m_kObj.*func)(fDeltaTime);}
private:

	T& m_kObj;
};


#endif //__FvTickTask_H__
