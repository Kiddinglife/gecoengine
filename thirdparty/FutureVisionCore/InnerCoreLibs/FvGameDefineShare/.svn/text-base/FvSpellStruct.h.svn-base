//{future header message}
#ifndef __FvSpellStruct_H__
#define  __FvSpellStruct_H__

#include "FvAreaStruct.h"
#include "FvGameUnitInfo.h"
#include "FvStaticArray.h"
#include "FvMiscData.h"

#include <FvKernel.h>

struct FvSpellEquipped
{
	FvSpellEquipped()
		:m_uiItemClass(0), m_uiItemInventoryType(0)
	{}
	FvUInt32 m_uiItemClass;								// 需要目标物品的类型
	FvUInt32 m_uiItemInventoryType;                // 需要目标物品的部位类型
};

struct FvSpellItemReq
{
	FvSpellItemReq()
		:m_iItemTemplate(0), m_iCnt(0)
	{}
	FvInt32 m_iItemTemplate;
	FvInt32 m_iCnt; 
};


//+-----------------------------------------------------------------------------


struct FvSpellValueSet
{
	bool IsEmpty()const{return m_kDeltaValue.IsEmpty();}
	FvIdxValueRange m_kDeltaValue;//! 值
	FvRefIdxValue m_kCasterRefBase;//! 参考释放者释放瞬间的基础值
	FvRefIdxValue m_kCasterRefMod;//! 参考释放者释放瞬间的额外值
	FvRefIdxValue m_kTargetRefBase;//! 参考目标放瞬间的基础值
	FvRefIdxValue m_kTargetRefMod;//! 参考目标放瞬间的额外值
};

struct FvSPellCost
{
	bool IsEmpty()const{return m_kValue.IsEmpty();}
	FvIdxValue m_kValue;
	FvRefIdxValue m_kRefValue;
};

//+-----------------------------------------------------------------------------

struct FvSendWorkInfo
{
	FvInt32 m_iType;
	FvInt32 m_uiDirType;
	FvInt32 m_iDistance;
	FvInt32 m_iSpeed;
	FvInt32 m_iVisual;										
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;
	FvInt32 m_iReserve_2;
	FvInt32 m_iReserve_3;
};

struct FvTargetSelectInfo
{
	static const FvUInt32 CONDITION_MAX = 5;
	FvInt32 m_iCasterMask;										///<FvTargetMask>
	FvAreaRange m_kCasterEffectRange;						
	FvInt32 m_iTargetMask;										///<FvTargetMask>
	FvAreaRange m_kTargetEffectRange;						
	FvInt32 m_iMaxAffectTargets;								// 目标数量
	FvInt32 m_iProbability;
	FvStaticArray<FvIdxValueRange, CONDITION_MAX> m_kCondition;
	FvInt32 m_iStateIdx;///<FvStateCondition>							
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;										
	FvInt32 m_iReserve_2;										
	FvInt32 m_iReserve_3;						
	FvInt32 m_iReserve_4;	
};

struct FvHitEffectInfo
{
private:
	FV_NOT_COPY_COMPARE(FvHitEffectInfo);
public:
	static const FvUInt32 MISC_VALUE_MAX = 5;

	FvHitEffectInfo(){}
	bool IsEmpty()const{return (m_iTemplateId == 0);}

	FvInt32 m_iTemplateId;										///<FvHitEffectTemplate>用来确定C++模板
	FvInt32 m_iType;												///<FvHitEffectType>
	FvInt32 m_iAreaFlag;
	FvInt32 m_iEffectSign;											///<FvEffectSign>
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;										
	FvInt32 m_iReserve_2;										
	FvInt32 m_iReserve_3;						
	FvInt32 m_iReserve_4;	

	FvInt32 m_iDelay;
	FvInt32 m_iReserve_10;			
	FvInt32 m_iReserve_11;		

	FvInt32 m_iVisualID;											// 显示效果ID
	FvInt32 m_iReserve_20;			
	FvInt32 m_iReserve_21;		

	FvInt32 m_iSelectChannel;									///<FvTargetSelectInfo>			
	FvInt32 m_iCreateProbabilityChannel;									
	FvInt32 m_iCreateProbability;			
	FvInt32 m_iReserve_30;			
	FvInt32 m_iReserve_31;						

	FvSpellValueSet m_kValueSet;

	MiscDataArray<MISC_VALUE_MAX> m_kMiscValue;
};


struct FvAuraInfo
{
private:
	FV_NOT_COPY_COMPARE(FvAuraInfo);
public:
	static const FvUInt32 VALUE_MAX = 8;
	static const FvUInt32 CONDITION_MAX = 5;
	static const FvUInt32 UNIT_END_EVENT_MAX = 5;
	static const FvUInt32 MISC_VALUE_MAX = 5;
	typedef FvStaticArray<FvIdxValueRange, CONDITION_MAX> ValueRangeArray;

	static bool IsNotEmpty(const ValueRangeArray& kCondition);

	FvAuraInfo(){}
	bool IsEmpty()const{return (m_iTemplateId == 0);}

	FvInt32 m_iAuraId;
	FvInt32 m_iTemplateId;										///<FvAuraTemplate>用来确定C++模板
	FvInt32 m_iType;													///<FvAuraType>
	FvInt32 m_iAreaFlag;
	FvInt32 m_iEffectSign;												///<FvEffectSign>
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;										
	FvInt32 m_iReserve_2;										
	FvInt32 m_iReserve_3;						
	FvInt32 m_iReserve_4;	

	///<显示>
	FvInt32 m_iVisualID;											// 显示效果ID
	FvInt32 m_iReserve_10;			
	FvInt32 m_iReserve_11;		

	///<创建>
	FvInt32 m_iSelectChannel;									///<FvTargetSelectInfo>		
	FvInt32 m_iCreateChannel;									///<FvTargetSelectInfo>				
	FvInt32 m_iCreateProbability;					
	FvInt32 m_iReserve_20;			
	FvInt32 m_iReserve_21;

	///<创建>
	FvInt32 m_iCloseEffectChannel;									/// 关闭指定的效果通道
	FvInt32 m_iEffectChannel;										/// 效果通道
	FvInt32 m_iEffectChannelWeight;								// 通道权重
	FvInt32 m_iAttachType;											///<FvEffectCreateType>
	FvInt32 m_iMaxAuras;		
	FvInt32 m_iReserve_30;			
	FvInt32 m_iReserve_31;		
	FvInt32 m_iReserve_32;		
	FvInt32 m_iReserve_33;		
	FvInt32 m_iReserve_34;		

	///<作用时间>
	FvInt32 m_iTimeDefer;													// 时间延迟
	FvInt32 m_iTickType;													/// <EffectTickType>Tick类型 时间/事件
	FvInt32 m_iTickCnt;														// Tick 次数
	FvInt32 m_iAmplitude;													// 属性周期
	FvInt32 m_iReserve_40;			
	FvInt32 m_iReserve_41;		
				
	///<作用/结束条件>
	ValueRangeArray m_kEffectCondition;
	FvInt32 m_iEffectStateIdx;///<FvStateCondition>

	ValueRangeArray m_kEndCondition;
	FvInt32 m_iEndStateIdx;///<FvStateCondition>

	FvInt32 m_iUnitEndEvents[UNIT_END_EVENT_MAX];

	FvInt32 m_iReserve_50;			
	FvInt32 m_iReserve_51;		
	FvInt32 m_iReserve_52;		
	FvInt32 m_iReserve_53;		
	FvInt32 m_iReserve_54;	
	FvInt32 m_iReserve_55;			
	FvInt32 m_iReserve_56;		
	FvInt32 m_iReserve_57;		
	FvInt32 m_iReserve_58;		
	FvInt32 m_iReserve_59;		

	///<状态影响>
	FvInt32 m_iActionStateMask;
	FvInt32 m_iAuraStateMask;
	FvInt32 m_iReserve_60;			
	FvInt32 m_iReserve_61;		
	FvInt32 m_iReserve_62;		

	///<值>
	FvSpellValueSet m_kValueSet[VALUE_MAX];
	FvInt32 m_iReserve_70;			
	FvInt32 m_iReserve_71;		
	FvInt32 m_iReserve_72;		
	FvInt32 m_iReserve_73;		
	FvInt32 m_iReserve_74;				

	///<脚本值>
	MiscDataArray<MISC_VALUE_MAX> m_kMiscValue;
};


struct FvSpellTravel
{
	FvSpellTravel():m_iType(0), m_iSpeed(0), m_iRange(0){}

	FvInt32 m_iType;
	FvInt32 m_iSpeed;
	FvInt32 m_iRange;//! 飞行范围
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;										
	FvInt32 m_iReserve_2;										
	FvInt32 m_iReserve_3;						
	FvInt32 m_iReserve_4;					
	FvInt32 m_iReserve_5;	
};

//+-----------------------------------------------------------------------------
struct FvSpellProcInfo
{
	static const FvUInt32 REQ_ITEM_MAX = 10;
	static const FvUInt32 EQUIPPED_MAX = 10;
	static const FvUInt32 COST_MAX = 5;
	static const FvUInt32 CONDITION_MAX = 5;
	static const FvUInt32 MISC_VALUE_MAX = 5;

	FvSpellProcInfo(){}

	FvInt32 m_iVisualID;										// 显示效果ID
	FvInt32 m_iFocusTargetMask;						///<FvUnitSociety>
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;
	FvInt32 m_iFacingFlag;									///<ViFacingFlag> 朝向标记
	FvInt32 m_iAddStateMask;
	FvInt32 m_iDelStateMask;
	FvInt32 m_iCutDurationMask;
	FvInt32 m_iReserve_3;

	FvSendWorkInfo m_kSendWorkInfo;

	//! 释放者需要的条件
	FvStaticArray<FvIdxValueRange, CONDITION_MAX> m_kCasterCondition;
	FvInt32 m_iCasterStateIdx;///<FvStateCondition>

	//! Focus目标需要的条件
	FvStaticArray<FvIdxValueRange, CONDITION_MAX> m_kFocusCondition;
	FvInt32 m_iFocusStateIdx;///<FvStateCondition>

	FvInt32 m_iCoolChannel;						// 冷却通道
	FvInt32 m_iCoolDuration;						// 冷却时间

	FvInt32 m_iPrepareTime;
	FvInt32 m_iCastTime;								//! 施法前摇
	FvInt32 m_iCastCnt;									//! 施法次数
	FvInt32 m_iCastEndTime;							//! 施法后摇
	FvInt32 m_iActCoolTime;							//! 行为冷却时间

	FvInt32 m_iReserve_4;						// 冷却通道

	FvInt32 m_iMinRange;								// 最小距离
	FvInt32 m_iMaxRange;								// 最大距离

	FvSpellTravel m_KTraveller;							//! 飞行物

	FvSpellEquipped m_kSpellEquipped[EQUIPPED_MAX];
	FvSpellItemReq m_kReqItems[REQ_ITEM_MAX];
	FvUInt32 m_iItemDealType;///<SpellReqItemDealType>
	FvUInt32 m_iReqBagSlot;
	FvSPellCost m_kCost[COST_MAX];

	MiscDataArray<MISC_VALUE_MAX> m_kMiscValue;
private:

	FV_NOT_COPY_COMPARE(FvSpellProcInfo);
};

class FvSpellInfo
{
public:
	static const FvUInt32 EFFECT_MAX = 5;
	static const FvUInt32 SELECT_MAX = 5;

	FvSpellInfo(){}
	bool HasSelect(const FvUInt32 uiSelectIdx)const;
	const FvAuraInfo* GetSelectAura(const FvUInt32 uiSelectIdx, const FvUInt32 uiEffectidx)const;
	const FvHitEffectInfo* GetSelectHitEffect(const FvUInt32 uiSelectIdx, const FvUInt32 uiEffectidx)const;
	const FvAuraInfo* GetAura(const FvUInt32 uiIdx)const;
	const FvHitEffectInfo* GetHitEffect(const FvUInt32 uiIdx)const;

	FvInt32 m_iSpellId;

	FvInt32 m_iReqLevel;
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;
	FvInt32 m_iReserve_2;

	FvSpellProcInfo m_kProcInfo;

	FvTargetSelectInfo m_kTargetSelect[EFFECT_MAX];
	FvHitEffectInfo m_HitEffectInfos[EFFECT_MAX];
	FvInt32 m_AuraIds[EFFECT_MAX];//FvAuraInfo m_AuraInfos[EFFECT_MAX];
private:
	FV_NOT_COPY_COMPARE(FvSpellInfo);
};

typedef const FvAuraInfo * (*pfnGetAura)(const FvUInt32 iAuraId);
extern FV_GAME_DEFINE_SHARE_API pfnGetAura g_pfnGetAura;

typedef FvUInt32 FvSpellID;
typedef FvUInt16 FvSpellEffectIdx;

struct FvEffectValue
{
	FvEffectValue():m_iValueBase(0), m_iValueMod(0){}
	void Clear(){m_iValueBase = 0; m_iValueMod = 0;}

	FvInt32 m_iValueBase;
	FvInt32 m_iValueMod;
};

struct FvAuraValue
{
	FvAuraValue(){}
	void Clear();

	FvEffectValue m_kValues[FvAuraInfo::VALUE_MAX];
};
struct FvSpellAuraInstance
{
	FvSpellAuraInstance():m_uiSpell(0), m_uiEffect(0){}

	FvSpellID m_uiSpell;
	FvSpellEffectIdx m_uiEffect;
	FvAuraValue m_kCasterValue;
};
struct FvSpellHitEffectInstance
{
	FvSpellHitEffectInstance():m_uiSpell(0), m_uiEffect(0){}

	FvSpellID m_uiSpell;
	FvSpellEffectIdx m_uiEffect;
	FvEffectValue m_kCasterValue;
};







#include "FvSpellStruct.inl"




//+-----------------------------------------------------------------------------
#endif //__FvSpellStruct_H__
