//{future header message}

#ifndef __FvLodTickTask_H__
#define  __FvLodTickTask_H__

#include "FvLogicCommonDefine.h"
#include "FvTimeNodeEx.h"
#include "FvMemoryNode.h"
#include <FvRefList.h>


class FV_LOGIC_COMMON_API FvLodTickNode: private FvRefNode1<FvLodTickNode*>
{
	FV_MEM_TRACKER(FvLodTickNode);
public:
	void SetSpan(const float fDeltaTime);
	void Exec(const float fDeltaTime);

	void Detach();
	bool IsAttach()const{return FvRefNode1::IsAttach();}

protected:

	virtual void _Exec(const float fDeltaTime) = 0;

	FvLodTickNode();
	~FvLodTickNode();

private:
	float m_fSpan;
	float m_fAccumulateTime;

	friend class FvLodTickList;
};

class FV_LOGIC_COMMON_API FvLodTickList
{
public:
	typedef FvRefList<FvLodTickNode*, FvRefNode1> List;
	FvLodTickList();
	~FvLodTickList();

	void Attach(FvLodTickNode& kTask);
	void UpdateList(const float fDeltaTime);
private:

	List m_TaskList;
};

template<class T, void (T::*func)(const float)>
class FvTLodTickNode: public FvLodTickNode
{
public:
	FvTLodTickNode(T& kT):m_kT(kT){}
	~FvTLodTickNode(){}

protected:

	virtual void _Exec(const float fDeltaTime)
	{
		(m_kT.*func)(fDeltaTime);
	}

private:
	T& m_kT;
};


#include "FvLogBus.h"

namespace LodTick_Demo
{
	class TestClass
	{
	public:
		TestClass():
#pragma warning(push)
#pragma warning(disable: 4355)
			m_kNode(*this)
#pragma warning(pop)
			{

			}

		void Tick(const float fDeltaTime)
		{
			FvLogBus::CritOk("TestClass::Tick(%f)", fDeltaTime);
		}

		FvTLodTickNode<TestClass, &TestClass::Tick> m_kNode;

	};


	static void Test()
	{
		TestClass kTestClass;
		kTestClass.m_kNode.SetSpan(0.2f);
		float fTime = 0.0f;
		while(fTime < 10.0f)
		{
			kTestClass.m_kNode.Exec(0.12f);
			fTime += 0.12f;
		}
	}
}

#endif //__FvLodTickTask_H__
