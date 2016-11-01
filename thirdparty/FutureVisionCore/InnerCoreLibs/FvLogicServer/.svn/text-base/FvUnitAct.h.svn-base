//{future header message}
#ifndef __FvUnitAct_H__
#define  __FvUnitAct_H__

#include "FvLogicServerDefine.h"

#include <FvTimeNodeEx.h>
#include <FvMemoryNode.h>
#include <FvCallback.h>


class FV_LOGIC_SERVER_API FvUnitAct
{
	FV_MEM_TRACKER(FvUnitAct);
	FV_NOT_COPY_COMPARE(FvUnitAct);
public:

	enum _State
	{
		DEACTIVE,
		ACTIVE,
	};

	virtual void End();
	FvCallbackList0<FvUnitAct>& CallbackList(){return m_kCallbackList;}
	void Active();
	bool IsActive()const;

	void Lock(const float fDuration);
	bool IsLocked()const{return m_kLockTime.IsAttach();}
	void EndLock(){m_kLockTime.Detach();}

protected:
	FvUnitAct();
	~FvUnitAct();

	virtual void _OnEnd() = 0;

	_FRIEND_CLASS_TimeNodeEvent_;
	void _OnLockEndTime(FvTimeEventNodeInterface&){}
	FvTimeEventNode2<FvUnitAct, &_OnLockEndTime> m_kLockTime;

private:

	FvCallbackList0<FvUnitAct> m_kCallbackList;	
	_State m_eState;

};


#endif //__FvUnitAct_H__
