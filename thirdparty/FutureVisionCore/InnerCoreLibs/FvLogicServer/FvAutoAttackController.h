//{future header message}
#ifndef __FvAutoAttackController_H__
#define  __FvAutoAttackController_H__

#include "FvLogicServerDefine.h"

#include "FvUnitAct.h"

#include <FvTickTask.h>
#include <FvTupleCallback.h>


class FV_LOGIC_SERVER_API FvAutoAttackController
{
	FV_NOT_COPY_COMPARE(FvAutoAttackController);
public:

	enum Event
	{
		TARGET_MISS,
		TARGET_ERROR,
		EVENT_SUP,
	};

	virtual ~FvAutoAttackController(void);

	void SetAttackSpan(const FvUInt32 uiSpanTime);
 
	void Start();
	void End();

	void Check();

	void Wait(FvUnitAct& kAction);

	FvTupleCaller<FvAutoAttackController>& GetCaller(){return m_kCaller;}


protected:

	enum _State
	{
		DEACTIVE,
		ACTIVE,
	};

	FvAutoAttackController(void);

	_CALLBACK_FRIEND_CLASS_;
	void _OnActionEnd(const FvUInt32);
	FvAsynCallback0<FvUnitAct, FvAutoAttackController, &_OnActionEnd> m_kActionEndAsyn_CB;

	virtual void _OnEnd();
	virtual void _OnPause(){}
	_State _GetState()const{return m_eState;}
	FvUInt32 _GetSpan()const{return m_uiSpanTime;}

private:


	virtual bool IsEndAttack()const = 0;
	virtual bool OnNextAttackTime() = 0;//! 如果返回False就需要进入TaskCheck

	_FRIEND_CLASS_TimeNodeEvent_;
	void _OnNextAttackTime(FvTimeEventNodeInterface&);
	FvTimeEventNode1<FvAutoAttackController> m_NextAttackTime;

	_FRIEND_CLASS_FvTickTask_;
	void _OnCheckTime(const float);
	FvTickTask<FvAutoAttackController, &_OnCheckTime> m_kCheckTask;

	FvTupleCaller<FvAutoAttackController> m_kCaller;
	FvUInt32 m_uiSpanTime;
	_State m_eState;
public:

	static FvTickTaskList<FvAutoAttackController, &_OnCheckTime> ms_TaskList;
};


#endif //__FvAutoAttackController_H__
