//{future header message}
#ifndef __FvUnitAura_H__
#define  __FvUnitAura_H__

#include "FvAura.h"
#include "FvGameUnitStateNotifiers.h"
#include "FvGameUnitValueNotifiers.h"

#include <FvRefList.h>
#include <FvDoubleLink.h>
#include <FvReferencePtr.h>
#include <FvValueNotifier.h>
#include <FvOwnPtr.h>


class FvGameUnitLogic;

class FV_LOGIC_SERVER_API FvUnitAura: public FvAura
{
public:

	void SetTarget(FvGameUnitLogic& kTarget);
	FvGameUnitLogic& Target()const;

	void AttachToSpellFlow(FvRefList<FvUnitAura*>& kList);
	void AttachToTarget(FvRefList<FvUnitAura*>& kList);

	virtual void Active(const bool bUpdate);
	virtual void Deactive(const bool bUpdate);

	bool IsAttributeMatch()const;

	virtual const FvStateCondition* EffectStateCondition()const;
	virtual const FvAuraInfo::ValueRangeArray* EffectValueCondition()const;
	const FvStateCondition* EndStateCondition()const;
	const FvAuraInfo::ValueRangeArray* EndValueCondition()const;

	void OnActiveByChannel(const bool bUpdate);
	void OnDeactiveByChannel(const bool bUpdate);
	FvUInt32 GetChannelID()const{return GetInfo().m_iEffectChannel;}
	FvUInt32 ChannelWeight()const;
	bool IsTopChannel()const;

protected:

	FvUnitAura();
	virtual ~FvUnitAura(void);
	//! 
	void _Active(const bool bUpdate);
	void _Deactive(const bool bUpdate);
	//! Âß¼­
	virtual void _OnStart();
	virtual void _OnEnd();

	virtual void _OnLoad(const bool bUpdate);
	virtual void _OnSave(const bool bUpdate);

	virtual void _OnTick(){}

private:

	FvGameUnitLogic* m_pkTarget;//! ²»»áÎª¿Õ

	FvRefNode2<FvUnitAura*> m_kTargetAttachNode;
	FvRefNode2<FvUnitAura*> m_kSpellFlowAttachNode;
	FvDoubleLinkNode2<FvUnitAura*> m_kChannelNode;

	_GAME_UNIT_STATE_NOTIFIER_FRIEND_CLASS_;
	_GAME_UNIT_VALUE_NOTIFIER_FRIEND_CLASS_;

	void _OnEffectMatch();
	void _OnEffectUnMatch();
	typedef FvGameUnitStateNotifiers<FvUnitAura, &_OnEffectMatch, &_OnEffectUnMatch> EffectStateNotifiers;
	FvOwnPtr<EffectStateNotifiers> m_opEffectStateConditionNotifiers;
	typedef FvGameUnitValueNotifiers<FvAuraInfo::CONDITION_MAX, FvUnitAura, &_OnEffectMatch, &_OnEffectUnMatch> EffectValueNotifiers;
	FvOwnPtr<EffectValueNotifiers> m_opEffectValueConditionNotifiers;
	void _OnEndMatch();
	void _OnEndUnMatch();
	typedef FvGameUnitStateNotifiers<FvUnitAura, &_OnEndMatch, &_OnEndUnMatch> EndStateNotifiers;
	FvOwnPtr<EndStateNotifiers> m_opEndStateConditionNotifiers;
	typedef FvGameUnitValueNotifiers<FvAuraInfo::CONDITION_MAX, FvUnitAura, &_OnEndMatch, &_OnEndUnMatch> EndValueNotifiers;
	FvOwnPtr<EndValueNotifiers> m_opEndValueConditionNotifiers;

};


#endif //__FvUnitAura_H__
