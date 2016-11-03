//{future header message}
#ifndef __FvUnitSpell_H__
#define __FvUnitSpell_H__


#include "FvLogicServerDefine.h"

#include "FvTarget.h"
#include "FvSpellFlow.h"
#include "FvSpellTargetEffect.h"
#include "FvWorkFlow.h"
#include "FvUnitAct.h"
#include "FvGameUnitDefine.h"
#include "FvGameUnitStateNotifiers.h"

#include <FvCreator.h>
#include <FvTimeNodeEx.h>
#include <FvTimeFlow.h>
#include <FvSpellStruct.h>
#include <FvSpellDefine.h>
#include <FvVector3.h>
#include <FvReferencePtr.h>
#include <FvKernel.h>
#include <FvValueNotifier.h>



class FvGameUnitAppearance;
class FvGameUnitLogic;

class FV_LOGIC_SERVER_API FvSendWork: public FvWork
{
public:
	FvSendWork();
	virtual ~FvSendWork();

	virtual void Init(FvGameUnitLogic& kCaster, const FvSendWorkInfo& kInfo, const FvTarget& kFocus) = 0;

protected:

	virtual void _OnStart();
	virtual void _OnEnd();
};

class FV_LOGIC_SERVER_API FvUnitSpell: public FvUnitAct
{
public:
	FvUnitSpell();
	~FvUnitSpell(void);

	static const FvUInt32 TARGET_LIST_SIZE = 100;
	typedef FvTargetRefList<100> TargetList;

	enum _State
	{
		IDLE,//! 
		READY,//! 酝酿
		CASTING,//! 释放
		ENDING,
	};

	enum _Event
	{
		EVNET_PRE_CAST,
		EVENT_CAST,
		EVENT_SEND,
		EVENT_ENDING,
		EVENT_ENDED,
		EVENT_SUP,
	};

	
	bool Prepare(FvGameUnitAppearance* pkFocus, const FvSpellID uiSpellId, const FvSpellInfo& kInfo);
	bool Prepare(const FvVector3& kPos, const FvSpellID uiSpellId, const FvSpellInfo& kInfo);
	bool Prepare(const FvTarget& kTarget, const FvSpellID uiSpellId, const FvSpellInfo& kInfo);

	void Delay(const float fDeltaTime);
	bool Cancel();

	void UpGrade(const FvUInt32 uiSlot);

	//! 地表信息检测
	static bool CheckRange(const FvGameUnitAppearance& kCaster, const FvGameUnitAppearance& kObj, const FvSpellInfo& kInfo);
	static bool CheckRange(const FvGameUnitAppearance& kCaster, const FvVector3& kPos, const FvSpellInfo& kInfo);
	
	_State GetState()const;
	const FvSpellInfo* GetSpellInfo()const;
	const FvTarget& GetTarget()const;
	TargetList& GetTargetList(){return m_kTargetList;}

	static FvCreator<FvSendWork, FvUInt32>& SendWorkCreator(){return ms_SendWorkCreator;}
	static FvCreator<FvSpellTargetEffect, FvUInt32>& TargetEffectCreator(){return ms_TargetEffectCreator;}

	bool Check(const FvSpellInfo& kInfo, FvGameUnitAppearance* pkObj)const;
	bool Check(const FvSpellInfo& kInfo, const FvVector3& kPos)const;
	bool Check(const FvSpellInfo& kInfo, const FvTarget&kTarget)const;

	FvGameUnitLogic& Self();
	const FvGameUnitLogic& Self()const;
private:
	//+-----------------------------------------------------------------------------
	void _OnPreCast(const FvUInt32&, FvTimeEventNodeInterface&);
	void _OnCast(const FvUInt32&, FvTimeEventNodeInterface&);
	void _OnSend(const FvUInt32&, FvTimeEventNodeInterface&);
	void _OnSendEnd(const FvUInt32&, FvTimeEventNodeInterface&);
	void _OnEnd(const FvUInt32&, FvTimeEventNodeInterface&);
	FvTimeFlow<FvUnitSpell> m_kTimeFlow;

	_CALLBACK_FRIEND_CLASS_;
	void _OnCastWorkFlowEnd(const FvUInt32);
	FvAsynCallback0<FvWorkFlow, FvUnitSpell, &_OnCastWorkFlowEnd> m_kSendWorkEndAsynCB;
	FvWorkFlow m_kSendWorkFlow;
	
	//+----------------------------------------------------------------------------------------------------

	bool _Prepare(const FvSpellID uiSpellId, const FvSpellInfo& kInfo);
	void _Cast();
	void _Send();
	void _EndSend();


	void _OnCancel();
	FvFrameEndCallback0<FvUnitSpell, &_OnCancel> m_kCancelFrameEnd;

	virtual void _OnEnd();
	void _SetState(const _State eState);

	//+-----------------------------------------------------------------------------
	//! 状态检测

	static bool _CheckCaster(const FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo);
	static bool _CheckTarget(const FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo, const FvTarget&kTarget);

	static bool _CheckTarget(const FvGameUnitLogic& kCaster, const FvGameUnitAppearance& kObj, const FvSpellInfo& kInfo);
	static bool _CheckTarget(const FvGameUnitLogic& kCaster, const FvVector3& kObj, const FvSpellInfo& kInfo);

	static bool _CheckPower(const FvGameUnitAppearance& kObj, const FvSpellInfo& kInfo);
	bool _CheckItemInBag()const;
	bool _CheckItemInEquip()const;
	bool _CheckAuraState()const;


	//+-----------------------------------------------------------------------------
	//! Cost执行
	void _CostPower(const FvSpellInfo& info);
	bool _CostItemInBag()const;
	bool _CostItemInEquip()const;

	//+-----------------------------------------------------------------------------
	void _SendByDefaultSpellFlow(const FvSpellInfo& kInfo);
	void _SendByPySpellFlow(const FvSpellInfo& kInfo);//! python 控制
	void _SendByPySpellDurFlow(const FvSpellInfo& kInfo);//! 持续施法
	void _SendByPySpelInstantFlow(const FvSpellInfo& kInfo);//! 瞬间施法

	const FvSpellInfo* m_rpSpellInfo;

	FvSpellFlow m_DefaultSpellFlow;//! 默认技能发射
	FvReferencePtr<FvSpellFlow> m_rpChannelSpellFlow;

	FvTarget m_kTarget;
	TargetList m_kTargetList;
	_State m_eState;
	FvInt32 m_uiCastReserveTime;
	bool m_bDurationSpell;

	_GAME_UNIT_STATE_NOTIFIER_FRIEND_CLASS_;
	void OnStateOK();
	void OnStateError();
	typedef FvGameUnitStateNotifiers<FvUnitSpell, &OnStateOK, &OnStateError> _StateNotifiers;
	_StateNotifiers m_kCasterStateNotifiers;
	_StateNotifiers m_kFocusStateNotifiers;

	static FvCreator<FvSendWork, FvUInt32> ms_SendWorkCreator;
	static FvCreator<FvSpellTargetEffect, FvUInt32> ms_TargetEffectCreator;
};

#endif __FvUnitSpell_H__