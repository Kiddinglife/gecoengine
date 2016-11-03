//{future header message}
#ifndef __FvSpellDefine_H__
#define __FvSpellDefine_H__



#include <FvMask.h>
#include <FvPowerDefines.h>
#include "FvGameUnitDefine.h"


//+-----------------------------------------------------------------------------

class FvEffectTickType
{
public:
	enum Idx
	{
		TIME,
		EVENT,
	};
};

class FvSpellSelectRalation
{
public:
	enum
	{
		/*1*/SELF_FRIEND		= FvUnitSociety::FRIEND,
		/*2*/SELF_ENEMY		= FvUnitSociety::ENEMY,
		/*3*/SELF_NEUTRAL	= FvUnitSociety::NEUTRAL,
		/*4*/SELF_TEAM		= FvUnitSociety::TEAM,
		/***/
		/*12*/ROUND_FRIEND		= FvUnitSociety::TOTAL + FvUnitSociety::FRIEND,
		/*13*/ROUND_ENEMY		= FvUnitSociety::TOTAL + FvUnitSociety::ENEMY,
		/*14*/ROUND_NEUTRAL	= FvUnitSociety::TOTAL + FvUnitSociety::NEUTRAL,
		/*15*/ROUND_TEAM		= FvUnitSociety::TOTAL + FvUnitSociety::TEAM,
		/****/
		/*22*/POS		= FvUnitSociety::TOTAL + FvUnitSociety::TOTAL,

		SELF_MASK = _MASK_(SELF_FRIEND) + _MASK_(SELF_ENEMY) + _MASK_(SELF_NEUTRAL) + _MASK_(SELF_TEAM),
		ROUND_MASK = _MASK_(ROUND_FRIEND) + _MASK_(ROUND_ENEMY) + _MASK_(ROUND_NEUTRAL) + _MASK_(ROUND_TEAM),
	};

	static FvUInt32 GetSelfRelation(const FvUInt32 uiMask)
	{
		const FvUInt32 uiSelfMask = uiMask&SELF_MASK;
		return uiSelfMask;
	}
	static FvUInt32 GetRoundRelation(const FvUInt32 uiMask)
	{
		const FvUInt32 uiRoundMask = (uiMask&ROUND_MASK) >> FvUnitSociety::TOTAL;
		return uiRoundMask;
	}
};


class FvSkillType
{
public:
	enum Idx
	{
		NONE,
		ACTIVE,
		PASSIVENESS,
	};
};

class FvFacingFlag
{
public:
	enum Idx
	{
		NONE,
		FOCUS,
	};
};


class FvInterruptMask
{
public:
	enum Idx
	{
		MOVE,
		HIT,
		TOTAL,
	};
};


class FvEffectSign
{
public:
	enum Idx
	{
		NONE,
		GOOD,
		BAD,
	};
};

class FvAuraEndTickType
{
public:
	enum Idx
	{
		NOWAY,//! 不可以被驱散
		BASE,//! 可以被驱散(直接终止)
		TICKALL,//! 可以被驱散(结束前TickAll)
	};
};

class FvEffectCreateType
{
public:
	enum Idx
	{
		CASTED = 0,//! 被动启动(需要操作才能产生应用到目标)
		AUTO = 3,//! 自动启动(学习就产生光环应用到自己)	
	};
};

class FvAuraAttachType
{
public:
	enum Idx
	{
		FREE,//
		BORROW,//
	};
};

class FvItemSpellType
{
public:
	enum Idx
	{
		NONE_ACTION,
		ACTION,
	};
};


//+-----------------------------------------------------------------------------
class FvAuraState
{
public:
	enum Mask
	{
		ACTIVE = 0X01,
		END = 0X02,
		OWN = 0X04,
	};
};

#endif //__FvSpellDefine_H__