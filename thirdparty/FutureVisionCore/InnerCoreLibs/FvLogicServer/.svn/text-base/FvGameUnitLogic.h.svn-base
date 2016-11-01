//{future header message}
#ifndef __FvGameUnitLogic_H__
#define __FvGameUnitLogic_H__

#include "FvLogicServerDefine.h"

#include "FvUnitSpell.h"
#include "FvGameUnitAppearance.h"
#include "FvUnitAuraChannel.h"

#include <FvGameUnitInfo.h>
#include <FvValueMapping.h>
#include <FvOwnPtr.h>
#include <map>

class FvUnitAura;
class FvAreaAura;
class FvUnitHitEffect;
class FvAreaHitEffect;
class FvSpellFlow;
class FvTarget;


class FV_LOGIC_SERVER_API FvGameUnitLogic: public FvGeographicObject
{
	FV_NOT_COPY_COMPARE(FvGameUnitLogic);
public:

	FV_REFERENCE_HEAD;

	static FvGameUnitLogic& Self(FvUnitSpell& kSpell);
	static const FvGameUnitLogic& Self(const FvUnitSpell& kSpell);

	virtual void ChargeTo(const FvVector3& kPos, const float fYaw, const float fDuration){}
	virtual void AimChargeTo(const FvVector3& kPos, const float fYaw, const float fDuration){}
	virtual void TransportTo(const FvVector3& kPos, const float fYaw){}

	bool Prepare(const FvSpellID uiSpellID, const FvSpellInfo& kInfo, FvGameUnitAppearance* pkFocus);
	bool Prepare(const FvSpellID uiSpellID, const FvSpellInfo& kInfo, const FvVector3& kPos);
	bool Prepare(const FvSpellID uiSpellID, const FvSpellInfo& kInfo, const FvTarget& kTarget);
	void CancelSpell();

	virtual void MakeUnitAura(FvGameUnitAppearance& kTarget, const FvSpellAuraInstance& kEffect){}
	virtual void MakeUnitAuras(FvGameUnitAppearance& kTarget, const std::vector<FvSpellAuraInstance>& kEffects){}
	virtual void MakeAreaAura(const FvVector3& kPos, const FvSpellAuraInstance& kEffect){}
	virtual FvAuraOwnInterface* BorrowUnitAura(FvGameUnitAppearance& kTarget, const FvSpellAuraInstance& kEffect){return NULL;}
	virtual FvAuraOwnInterface* BorrowAreaAura(const FvVector3& kPos, const FvSpellAuraInstance& kEffect){return NULL;}
	virtual void MakeUnitHitEffect(FvGameUnitAppearance& kTarget, const FvSpellHitEffectInstance& kEffect){}
	virtual void MakeUnitHitEffects(FvGameUnitAppearance& kTarget, const std::vector<FvSpellHitEffectInstance>& kEffects){}
	virtual void MakeAreaHitEffect(const FvVector3& kPos, const FvSpellHitEffectInstance& kEffect){}

	virtual FvSpellFlow* MakeSpellFlow(const FvSpellInfo& kInfo, const FvSpellID uiSpellId, const FvTarget& kTarget){return NULL;}

	virtual void OnAuraEnd(FvUnitAura& kAura){}

	//+----------------------------------------------------------------------------------------------------
	//! 被技能回调接口
	virtual void OnPrepare(const FvSpellID uiSpellId){}
	virtual void OnPrepareTimeUpdate(const FvSpellID uiSpellId, const float fReserveTime){}
	virtual void OnPreCastWork(const FvSpellID uiSpellId){}
	virtual void OnCast(const FvSpellID uiSpellId){}
	virtual void OnCastTimeUpdate(const FvSpellID uiSpellId, const float fReserveTime){}
	virtual void OnChannel(const FvSpellID uiSpellId){}
	virtual void OnBreakSpell(){}
	virtual void OnCancelSpell(){}
	virtual void OnCastStart(){}
	virtual void OnCastEnd(){}

	void _OnAddAura(FvRefNode2<FvUnitAura*>& kAuraNode);
	void _OnDelAura(FvUnitAura& kAura);
	FvUnitAuraChannelList& AuraChannels(){return m_kUnitAuraChannelList;}
	void RemoveAuraByType(const FvUInt32 uiAuraType);
	void RemoveAuraBySpellId(const FvSpellID uiSpellId);
	bool HasAuraByType(const FvUInt32 uiAuraType)const;
	FvUInt32 GetAuraCntByType(const FvUInt32 uiAuraType)const;
	bool HasAuraBySpellId(const FvSpellID uiSpellId)const;
	bool HasAuraByChannel(const FvUInt32 uiChannel)const;
	FvUInt32 GetAuraCnt(const FvSpellID uiSpellId, const FvSpellEffectIdx uiEffectIdx)const;
	FvUInt32 DispelAuraBySign(const FvEffectSign::Idx sign);
	void DispelRandomAura();
	FvUInt32 DispelAuraByType(const FvUInt32 uiAuraType);
	FvUInt32 DispelAuraBySpell(const FvSpellID uiSpell);
	FvUInt32 DispelAuraByChannel(const FvUnitAura& kAura);
	FvUInt32 TradeAuraBySign(const FvEffectSign::Idx eSign, FvGameUnitAppearance& kReciver);
	FvUInt32 TradeAuraByType(const FvUInt32 uiAuraType, FvGameUnitAppearance& kReciver);
	FvUnitAura* SetAuraDuration(const FvSpellID uiSpell, const FvSpellEffectIdx uiEffectIdx, const FvUInt32 uiDuration);
	void CloseAllAura();
	void CloseAura(const FvSpellID uiSpell);
	void CloseBorrowAura();
	void CloseAuraByChannel(const FvUInt32 uiChannel);

	void _OnAddHitEffect(FvRefNode2<FvUnitHitEffect*>& kHitEffectNode);

	virtual void OnAuraActive(const FvUnitAura& kAura){}
	virtual void OnAuraDeactive(const FvUnitAura& kAura){}
	virtual void OnHitEffect(const FvUnitHitEffect& kHitEffect){}

	virtual void AddAuraState(const FvInt32 eAuraType){}
	virtual void DelAuraState(const FvInt32 eAuraType){}
	void AddAuraStateMask(const FvUInt32 eMask);
	void DelAuraStateMask(const FvUInt32 eMask);

	virtual void AddActionState(const FvInt32 eActionType){}
	virtual void DelActionState(const FvInt32 eActionType){}
	void AddActionStateMask(const FvUInt32 eMask);
	void DelActionStateMask(const FvUInt32 eMask);

	virtual void SetMoveState(const FvInt32 uiState){}

	virtual FvInt32 GetScriptValue(const FvUInt32 uiIdx)const{return 0;}

	FvInt32 ModifyValue32(const FvUInt32 uiIdx, const FvInt32& iDeltaValue);
	FvInt64 ModifyValue64(const FvUInt32 uiIdx, const FvInt64& iDeltaValue);
	void AttachValue32Mapping(const FvUInt32 uiIdx, FvValueMapping& kMapping);
	FvInt32 UpdateValueMapping(const FvUInt32 uiIdx, const FvInt32& iDeltaValue);

	FvGameUnitAppearance& Appearance()const{return m_kAppearance;}
	FvUnitSpell& _Spell(){return m_kSpell;}

protected:

	FvGameUnitLogic(FvGameUnitAppearance& kAppearance);
	~FvGameUnitLogic(void);

	virtual void Save();
	virtual void Clear();

	virtual bool _SetValue32(const FvUInt32 uiIdx, const FvInt32& iNewValue) = 0;
	virtual bool _SetValue64(const FvUInt32 uiIdx, const FvInt64& iNewValue) = 0;

	virtual FvInt32 _MinValue32(const FvUInt32 uiIdx)const = 0;
	virtual FvInt32 _MaxValue32(const FvUInt32 uiIdx)const = 0;
	virtual FvInt32 _Normal32(const FvUInt32 uiIdx, const FvInt32& iValue)const = 0;
	//virtual FvInt64 _Normal64(const FvUInt32 uiIdx, const FvInt64& iValue)const = 0;

	//+----------------------------------------------------------------------------------------------------
	void _SaveClearAuras(const bool bUpdate);
	void _ClearHitEffects();

	//! 技能
	FvRefList<FvUnitHitEffect*> m_HitEffectList;
	FvRefList<FvUnitAura*> m_AuraList;
	FvUnitAuraChannelList m_kUnitAuraChannelList;

private:

	FvGameUnitAppearance& m_kAppearance;

	//+-----------------------------------------------------------------------------
	///<技能>
	FvUnitSpell m_kSpell;

	///<属性修改重定向>
	typedef std::map<FvUInt32, FvOwnPtr<FvValueMappingList>> _ValueMappingList;
	_ValueMappingList m_kValue32Mappings;

	friend class FvDeletable;
	template <class > friend class FvReferencePtr;
};


#endif //__FvGameUnitLogic_H__
