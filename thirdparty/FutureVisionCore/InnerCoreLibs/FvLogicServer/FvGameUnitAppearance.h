//{future header message}
#ifndef __FvGameUnitAppearance_H__
#define __FvGameUnitAppearance_H__

#include "FvLogicServerDefine.h"
#include "FvGameUnitStateNotifiers.h"
#include "FvLogicDebug.h"

#include <FvGeographicObject.h>
#include <FvSpellDefine.h>
#include <FvSpellStruct.h>
#include <FvGameUnitInfo.h>

#include <FvReferencePtr.h>
#include <FvGeographicObject.h>
#include <FvSimpleVector.h>
#include <FvValueNotifier.h>
#include <FvOwnPtr.h>
#include <FvGameMessage.h>
#include <FvValueInherit.h>
#include <FvTuple.h>

class FvGameUnitValueInherit;
class FvBinaryIStream;


class FV_LOGIC_SERVER_API FvGameUnitAppearance: public FvGeographicObject
{
	FV_NOT_COPY_COMPARE(FvGameUnitAppearance);
public:
	FV_REFERENCE_HEAD;

	FvGameUnitAppearance(void);
	~FvGameUnitAppearance(void);

	virtual void OnLogicEvent(const FvUInt32 uiEvent){}

	virtual bool HasAuraState(const FvInt32 eAuraType)const{return false;} 
	virtual FvInt32 GetAuraState()const{return 0;}

	virtual bool HasMoveState(const FvInt32 eMoveType)const{return false;}
	virtual FvInt32 GetMoveState()const{return 0;}

	virtual bool HasActionState(const FvInt32 eMoveType)const{return false;}
	virtual FvInt32 GetActionState()const{return 0;}

	virtual FvUInt32 Team()const{return 0;}
	virtual FvUInt32 Party()const{return 0;}
	virtual FvInt32 GetLevel()const{return 0;}

	FvInt32 GetValue32(const FvRefIdxValue& kValue)const;
	FvInt32 GetValue32(const FvUInt32 uiIdx, const FvInt32 iDefaultVlaue = 0)const;
	FvInt64 GetValue64(const FvUInt32 uiIdx, const FvInt64 iDefaultVlaue = 0)const;
	bool Value32RangeIn(const FvUInt32 uiIdx, const FvInt32 iValueInf, const FvInt32 iValueSup)const;
	bool Value64RangeIn(const FvUInt32 uiIdx, const FvInt32 iValueInf, const FvInt32 iValueSup)const;

	void GetAuraValue(const FvAuraInfo& kInfo, FvAuraValue& kValue)const;
	void GetHitEffectValue(const FvHitEffectInfo& kInfo, FvEffectValue& kValue)const;

	void AttachValue32Listen(const FvUInt32 uiIdx, FvValueCallbackInterface<FvInt32>& kCallback);
	void AttachValue32Listen(const FvUInt32 uiIdx, FvValueRangeNotifier& kCallback);
	void AttachValue64Listen(const FvUInt32 uiIdx, FvValueCallbackInterface<FvInt64>& kCallback);
	void AttachValue32Inherit(const FvUInt32 uiIdx, FvGameUnitValueInherit& kInherit);

	void AttachAuraStateNotifier(FvValueMaskNotifier& kNotifier);
	void AttachActionStateNotifier(FvValueMaskNotifier& kNotifier);
	void AttachMoveStateNotifier(FvValueMaskNotifier& kNotifier);
	void AttachUnitStateNotifier(FvValueMaskNotifier& kNotifier);
	void AttachStateNotifier(FvRefNode1<FvGameUnitStateNotifyInterface*>& kNotifier);
	bool IsMatch(const FvStateCondition& kCondition)const;

	virtual void Clear();

	virtual bool _GetValue32(const FvUInt32 uiIdx, FvInt32& iValue)const = 0;
	virtual bool _GetValue64(const FvUInt32 uiIdx, FvInt64& iValue)const = 0;

	virtual bool GetTarget(const FvUInt32 uiMask, FvGameUnitAppearance*& pkTarget){pkTarget = NULL; return false;}
	virtual bool IsMatchRelationMask(const FvUInt32 uiMask, const FvGameUnitAppearance& kObj)const{return false;}

	virtual FvUInt32 GetSpellMessage(const FvGameMessageIdx::Idx uiLogIdx)const = 0;
	virtual void SendMessage(const FvUInt32 uiLogIdx){}
	virtual void SendMessage(const FvUInt32 uiLogIdx, const FvDynamicTuple& kTuple){}

	//+----------------------------------------------------------------------------------------------------
	void ResultMessage(const FvUInt32 uiLogIdx)
	{
		SendMessage(uiLogIdx);
	}

	template <class Param>
	void ResultMessage(const FvUInt32 uiLogIdx, const Param& kParam)
	{
		FvDynamicTuple kTuple;
		kTuple.PushBack(kParam);
		SendMessage(uiLogIdx, kTuple);
	}

	template <class Param1, class Param2>
	void ResultMessage(const FvUInt32 uiLogIdx, const Param1& kParam1, const Param2& kParam2)
	{
		FvDynamicTuple kTuple;
		kTuple.Reserve(2);
		kTuple.PushBack(kParam1);
		kTuple.PushBack(kParam2);
		SendMessage(uiLogIdx, kTuple);
	}

	template <class Param1, class Param2, class Param3>
	void ResultMessage(const FvUInt32 uiLogIdx, const Param1& kParam1, const Param2& kParam2, const Param3& kParam3)
	{
		FvDynamicTuple kTuple;
		kTuple.Reserve(3);
		kTuple.PushBack(kParam1);
		kTuple.PushBack(kParam2);
		kTuple.PushBack(kParam3);
		SendMessage(uiLogIdx, kTuple);
	}

	template <class Param1, class Param2, class Param3, class Param4>
	void ResultMessage(const FvUInt32 uiLogIdx, const Param1& kParam1, const Param2& kParam2, const Param3& kParam3, const Param4& kParam4)
	{
		FvDynamicTuple kTuple;
		kTuple.Reserve(4);
		kTuple.PushBack(kParam1);
		kTuple.PushBack(kParam2);
		kTuple.PushBack(kParam3);
		kTuple.PushBack(kParam4);
		SendMessage(uiLogIdx, kTuple);
	}
protected:

	void _OnMoveStateUpdated(const FvInt32& iNewValue, const FvInt32& iOldValue);
	void _OnActionStateUpdated(const FvInt32& iNewValue, const FvInt32& iOldValue);
	void _OnAuraStateUpdated(const FvInt32& iNewValue, const FvInt32& iOldValue);
	void _OnUnitStateUpdated(const FvInt32& iNewValue, const FvInt32& iOldValue);

	void _OnValue32Updated(const FvUInt32 uiIdx, const FvInt32& iNewValue, const FvInt32& iOldValue);
	void _OnValue64Updated(const FvUInt32 uiIdx, const FvInt64& iNewValue, const FvInt64& iOldValue);

	//+----------------------------------------------------------------------------------------------------
	//! Value变化回调
	FvSimpleVector<FvValueCallbackInterface<FvInt32>::List> m_Value32UpdateListeners;
	FvSimpleVector<FvValueCallbackInterface<FvInt64>::List> m_Value64UpdateListeners;
	FvSimpleVector<FvValueRangeNotifier::List> m_Value32RangeListeners;
	FvSimpleVector<FvValueInheritList<FvInt32>> m_Value32SrcInheritList;

	//! 状态监听器
	FvValueMaskNotifier::List m_MoveStateNotifierList;
	FvValueMaskNotifier::List m_ActionStateNotifierList;
	FvValueMaskNotifier::List m_AuraStateNotifierList;
	FvValueMaskNotifier::List m_UnitStateNotifierList;
	FvGameUnitStateNotifyInterface::List m_kStateNotifierList;

};

#endif //__FvGameUnitAppearance_H__