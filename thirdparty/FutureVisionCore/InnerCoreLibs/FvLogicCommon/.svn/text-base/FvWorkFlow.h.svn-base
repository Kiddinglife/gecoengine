//{future header message}
#ifndef __FvWork_H__
#define __FvWork_H__




#include "FvLogicCommonDefine.h"

#include "FvRefList.h"
#include "FvReferencePtr.h"
#include "FvCallback.h"

class FvWorkFlow;

class FV_LOGIC_COMMON_API FvWork
{
public:
	FV_REFERENCE_HEAD;

	enum _State
	{
		DEACTIVE,
		ACTIVE,
		PAUSED,
	};

	void End();

	void Detach();

	_State GetState()const{return m_State;}

	FvCallbackList0<FvWork>& CallbackList(){return m_kCallbackList;}
protected:

	FvWork();
	virtual ~FvWork();

	//void Start();

	//void Pause();
	//void Resume();

private:

	void _AttachToFlow(FvWorkFlow& kFlow);

	void _EndByFlow();
	void _StartByFlow();
	void _ResumeByFlow();
	void _PauseByFlow();

	virtual void _OnStart();
	virtual void _OnEnd();
	virtual void _OnPause();
	virtual void _OnResume();

	_State m_State;
	FvWorkFlow* m_rpFlow;
	FvRefNode2<FvWork*> m_AttachToFlow;
	FvCallbackList0<FvWork> m_kCallbackList;
	friend class FvWorkFlow;
};

//+---------------------------------------------------------------------------------------------------
template <class TWorker,
void (TWorker::*func_OnStart)(), void (TWorker::*func_OnEnd)()>
class FvTWork: public FvWork
{
public:
	FvTWork(TWorker& kWorker):m_kWorker(kWorker){}
	TWorker& m_kWorker;

private:
	virtual void _OnStart(){(m_kWorker.*func_OnStart)();}
	virtual void _OnEnd(){(m_kWorker.*func_OnEnd)();}
};

//+---------------------------------------------------------------------------------------------------
//! 先进先出的WorkFlow

class FV_LOGIC_COMMON_API FvWorkFlow
{
	typedef FvRefNode2<FvWork*> _Node;
	typedef FvRefList<FvWork*, FvRefNode2> _List;
public:
	FvWorkFlow(void);
	~FvWorkFlow(void);

	void Clear();
	bool IsAttach(const FvWork& kWork)const;
	void Attach(FvWork& kWork);
	void Attach(FvWork* pkWork);
	void DoWork(FvWork& kWork);

	void Start();
	void DoNext();
	FvWork* CurrentWork()const;
	const _List& Works()const;

	void PauseCurrent();
	void EndCurrent();

	FvCallbackList0<FvWorkFlow>& CallbackList(){return m_kCallbackList;}

	void _OnWorkEnd(const FvUInt32);
	FvAsynCallback0<FvWork, FvWorkFlow, &FvWorkFlow::_OnWorkEnd> m_kWorkEndAsyn_CB;

private:
	void _StartWork(FvWork& kWork);
	void _EndWork(FvWork& kWork);
	void _PauseWork(FvWork& kWork);
	void _ResumeWork(FvWork& kWork);

	_List m_List;
	FvReferencePtr<FvWork> m_rpCurrentWork;

	FvCallbackList0<FvWorkFlow> m_kCallbackList;

	friend class FvWork;
};


namespace WorkFlow_Demo
{
	class TestWork: public FvWork
	{
	public:
		TestWork(){}
		~TestWork(){}

		FvUInt32 m_uiID;
	};

	static void Test()
	{
		FvWorkFlow kFlow;
		TestWork kWork_1;		kFlow.Attach(kWork_1);	kWork_1.m_uiID = 1;
		TestWork kWork_2;		kFlow.Attach(kWork_2);	kWork_2.m_uiID = 2;
		kFlow.Start();
		//! ...
		kWork_1.End();
		//! ...
		kFlow.Clear();
	}
}
#endif //__FvWork_H__
