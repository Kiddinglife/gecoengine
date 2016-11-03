#pragma once

//{future header message}
#ifndef __FvSpellFlow_H__
#define __FvSpellFlow_H__

#include "FvLogicServerDefine.h"

#include "FvSpellCasterInfo.h"
#include "FvTarget.h"

#include <FvTimeNodeEx.h>
#include <FvTimeFlow.h>
#include <FvSpellStruct.h>
#include <FvSpellDefine.h>
#include <FvKernel.h>
#include <FvMemoryNode.h>
#include <FvOwnPtr.h>
#include <FvReferencePtr.h>

class FvGameUnitLogic;
class FvGameUnitAppearance;

class FvUnitAura;
class FvAreaAura;
class FvUnitHitEffect;
class FvAreaHitEffect;
class FvUnitAuraController;
class FvAuraOwnInterface;
class FvAreaTargetSelector;

namespace Private_FvSpellFlow
{
	class FV_LOGIC_SERVER_API SpellTargetList
	{
	public:

		typedef FvTargetList<10000> TargetList;
		void Select(const FvGameUnitLogic& kCaster, const FvTarget& kFocus, const FvTargetSelectInfo& kInfo, FvAreaTargetSelector& kTargetSelector);
		void Select(const FvGameUnitLogic& kCaster, FvGameUnitAppearance& kFocus, const FvAreaRange& kRange, const FvUInt32 uiMask, FvAreaTargetSelector& kTargetSelector);
		void Select(const FvGameUnitLogic& kCaster, const FvVector3& kFocus, const FvAreaRange& kRange, const FvUInt32 uiMask, FvAreaTargetSelector& kTargetSelector);

		FvUInt32 GetObjCnt()const;
		FvGameUnitAppearance& _GetTarget(const FvUInt32 uiIdx)const;
		void GetTarget(std::vector<FvGameUnitAppearance*>& kObjList)const;
		template<class T>
		void GetTarget(std::vector<T*>& kObjList)const;
		template<class T, template<class>class Ptr>
		void GetTarget(std::vector<Ptr<T>>& kObjList)const;

		const std::vector<FvVector3>& PosList()const{return m_kPosList;}
	private:

		template<class T>
		static T* DynamicCast(FvGameUnitAppearance& kObj);///<需要特化>

		void RandomSelect(const FvUInt32 uiCnt);
		void AddObj(FvGameUnitAppearance& kObj);

		TargetList m_kTargetList;
		FvUInt32 m_uiRandomStartIdx;
		FvUInt32 m_uiCnt;
		std::vector<FvVector3> m_kPosList;
		const FvStaticArray<FvIdxValueRange, FvTargetSelectInfo::CONDITION_MAX>* m_rpValueCondition;
		const FvStateCondition* m_rpStateCondition;
	};
}

class FV_LOGIC_SERVER_API FvSpellFlow: private FvRefNode1<FvSpellFlow*>
{
	FV_MEM_TRACKER(FvSpellFlow);
	FV_NOT_COPY_COMPARE(FvSpellFlow);
public:
	FV_REFERENCE_HEAD;

	typedef Private_FvSpellFlow::SpellTargetList TargetList;

	FvSpellFlow(void);
	virtual ~FvSpellFlow(void);

	static void SetTargetSelector(FvAreaTargetSelector* pkSelector);

	static bool MakeSpellFlow(FvSpellFlow& kDefaultSpellFlow, FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo, const FvSpellID uiSpellId, const FvTarget& kTarget);//! Focus可以是Caster
	static void ClearFreeFlow();

	//+----------------------------------------------------------------------------------
	///<初始化>
	void Init(FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo, const FvSpellID uiSpellId, const FvTarget& kTarget);//! Focus可以是Caster
	void Clear();
	virtual void End(){}

	//+----------------------------------------------------------------------------------
	///<技能执行>
	void Fresh();
	void FreshEffect(const FvUInt32 uiSelectIdx);
	void CreateUnitAura(FvGameUnitAppearance& kTarget, const FvUInt32 uiEffectIdx);
	void CreateUnitHitEffect(FvGameUnitAppearance& kTarget, const FvUInt32 uiEffectIdx);
	void CreateAreaAura(const FvVector3& kPos, const FvUInt32 uiEffectIdx);
	void CreateAreaHitEffect(const FvVector3& kPos, const FvUInt32 uiEffectIdx);
	void CloseOwnAuras();

	const TargetList& UpdateTargetSelector(const FvGameUnitLogic& kCaster, const FvUInt32 uiSelectIdx)const;
	const TargetList& FreshEffect(const FvGameUnitLogic& kCaster, const FvUInt32 uiSelectIdx);

	///<定时>
	void SetHitTime(const FvUInt32 uiDeltaTime);

	const FvSpellInfo& GetInfo()const;
	FvGameUnitLogic* GetCaster()const{return m_rpCaster;}
	const FvTarget& GetFocus()const{return m_kFocus;}

protected:

	static void _Init(FvSpellFlow* pkSpellFlow, const float fHitTime, FvGameUnitLogic& kCaster, const FvSpellInfo& kInfo, const FvUInt32 uiSpellId, const FvTarget& kTarget);
	
	void _RecordCasterInfo(const FvSpellInfo& kInfo);

	void _Fresh(FvGameUnitLogic& kCaster, const FvUInt32 uiEffectIdx, const TargetList& ms_kTargetList, const FvAuraInfo& kInfo);
	void _Fresh(FvGameUnitLogic& kCaster, const FvUInt32 uiEffectIdx, const TargetList& ms_kTargetList, const FvHitEffectInfo& kInfo);

	//+----------------------------------------------------------------------------------------------------------------------------------
	void _CreateAreaHitEffect(const FvUInt32 uiEffectId, const FvHitEffectInfo& kInfo, FvGameUnitLogic& kCaster, const FvEffectValue& kRefValue, const FvVector3& kPos);
	void _CreateUnitHitEffect(const FvUInt32 uiEffectId, const FvHitEffectInfo& kInfo, FvGameUnitLogic& kCaster, const FvEffectValue& kRefValue, FvGameUnitAppearance& kTarget);
	void _CreateAreaAura(const FvUInt32 uiEffectId, const FvAuraInfo& kInfo, FvGameUnitLogic& kCaster, const FvAuraValue& kCasterValues, const FvVector3& kPos);
	void _CreateUnitAura(const FvUInt32 uiEffectId, const FvAuraInfo& kInfo, FvGameUnitLogic& kCaster, const FvAuraValue& kCasterValues, FvGameUnitAppearance& kTarget);

private:

	void _OnHitTime(FvTimeEventNodeInterface&);
	FvTimeEventNode2<FvSpellFlow, &_OnHitTime> m_kHitTimeNode;

	FvReferencePtr<FvGameUnitLogic> m_rpCaster;
	FvTarget m_kFocus;

	const FvSpellInfo* m_rpSpellInfo;
	FvUInt32 m_uiSpellId;

	static FvOwnPtr<FvAreaTargetSelector> ms_opAreaTargetSelector;//! 区域目标选择器
	static TargetList ms_kTargetList;

	FvAuraValue m_CasterAuraRefValues[FvSpellInfo::EFFECT_MAX];
	FvEffectValue m_CasterHitEffectRefValues[FvSpellInfo::EFFECT_MAX];

	static bool _CheckState(const FvGameUnitAppearance& kTarget, const FvStateCondition& kCondition);
	std::vector<FvAuraOwnInterface*> m_kOwnAuraList;

	//+----------------------------------------------------------------------------------------------------------------------------------
	//! 管理哪些未被其他模块控制的ViSpellFlow
	static FvRefListNameSpace<FvSpellFlow>::List1 ms_FreeNodeList;//!

	friend class FvDeletable;
};


//+---------------------------------------------------------------------------------------------------------------------------------------------

namespace Private_FvSpellFlow
{
	template<class T>
	void SpellTargetList::GetTarget(std::vector<T*>& kObjList)const
	{
		const FvUInt32 uiSize = GetObjCnt();
		kObjList.reserve(uiSize);
		for (FvUInt32 uiIdx = 0; uiIdx < uiSize; ++uiIdx)
		{
			FvGameUnitAppearance& kTarget = _GetTarget(uiIdx);
			T* pkEntity = DynamicCast<T>(kTarget);
			if(pkEntity)
			{
				kObjList.push_back(pkEntity);
			}
		}
	}
	template<class T, template<class>class Ptr>
	void SpellTargetList::GetTarget(std::vector<Ptr<T>>& kObjList)const
	{
		const FvUInt32 uiSize = GetObjCnt();
		kObjList.reserve(uiSize);
		for (FvUInt32 uiIdx = 0; uiIdx < uiSize; ++uiIdx)
		{
			FvGameUnitAppearance& kTarget = _GetTarget(uiIdx);
			T* pkEntity = DynamicCast<T>(kTarget);
			if(pkEntity)
			{
				kObjList.push_back(pkEntity);
			}
		}
	}
}

#endif //__FvSpellFlow_H__
