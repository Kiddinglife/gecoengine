//{future header message}
#ifndef __FvHitEffect_H__
#define  __FvHitEffect_H__

#include "FvLogicServerDefine.h"

#include "FvSpellCasterInfo.h"

#include <FvSpellStruct.h>
#include <FvReferencePtr.h>
#include <FvKernel.h>
#include <FvSpellDefine.h>
#include <FvSpellStruct.h>

#include <FvTimeNodeEx.h>
#include <FvMemoryNode.h>



//+-----------------------------------------------------------------------------
class FvGameUnitAppearance;

//+-----------------------------------------------------------------------------
class FV_LOGIC_SERVER_API FvHitEffect
{
	FV_MEM_TRACKER(FvHitEffect);
	FV_NOT_COPY_COMPARE(FvHitEffect);

public:
	enum _State
	{
		START,
		END,
	};

	static FvInt32 GetCost(const FvGameUnitAppearance& kObj, const FvIdxValueRange& kCost, const FvRefIdxValue& kRef0Cost, const FvRefIdxValue& kRef1Cost);

	void End();

	virtual void SetCasterValue(const FvEffectValue& kValue){m_kCasterValue = kValue;}
	const FvEffectValue& GetCasterValue()const{return m_kCasterValue;}

	void SetHanldeTime(const FvUInt32 iDeltaMS);

	const FvHitEffectInfo& GetInfo()const;
	void SetInfo(const FvHitEffectInfo& kInfo){m_rpInfo = &kInfo;}

	bool GetMiscValue(const char* pcName, FvInt32& iValue)const;
	FvInt32 MiscValue(const char* pcName, const FvInt32 iDefaultValue = 0)const;

	void SetSpellId(const FvSpellID uiSpell, const FvSpellEffectIdx uiEffectIdx);	
	FvSpellID SpellId()const;
	FvSpellEffectIdx EffectIdx()const;

protected:
	FvHitEffect();
	virtual ~FvHitEffect(void);

	//+-----------------------------------------------------------------------------
	_State _GetState()const;
	void _SetState(const _State eState);

	//+-----------------------------------------------------------------------------
	_FRIEND_CLASS_TimeNodeEvent_;
	void OnEffect(FvTimeEventNodeInterface& kNode);
	FvTimeEventNode2<FvHitEffect, &OnEffect> m_kEffectTimeEvent;

private:
	//+-----------------------------------------------------------------------------
	virtual void _Handle(){}
	virtual void _OnEnd(){}

	//+-----------------------------------------------------------------------------
	_State m_eState;

	FvEffectValue m_kCasterValue;

	//+-----------------------------------------------------------------------------
	const FvHitEffectInfo* m_rpInfo;/// ²»»áÎª¿Õ
	FvSpellID m_uiSpellId;
	FvSpellEffectIdx m_uiEffectIdx;

	friend class FvDeletable;

};

#endif //__FvHitEffect_H__
