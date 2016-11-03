//{future header message}
#ifndef __FvGameUnitDefine_H__
#define  __FvGameUnitDefine_H__

#include "FvMask.h"

class FvUnitSociety
{
public:
	enum Idx
	{
		SELF,
		FRIEND,
		ENEMY,
		NEUTRAL,
		TEAM,
		GROUND,
		RESERVE_0,
		RESERVE_1,
		RESERVE_2,
		RESERVE_3,
		RESERVE_4,
		TOTAL,
	};
};

//
//class FvDamageType
//{
//public:
//	enum Type
//	{
//		PHYSICS_MELEE,		// 近程物理
//		PHYSICS_RANGE,		// 远程物理
//		PHYSICS_NO_DEFANCE,		// 神圣物理
//		SPELL_MELEE,		// 近程技能
//		SPELL_RANGE,		// 远程技能
//		SPELL_MAGIC,		// 魔法技能
//		SPELL_NO_DEFANCE,	// 神圣技能
//		TOTAL,
//	};
//};
//
//
//#define DMG_TYPE(idx)					(((idx) >= FvDamageType::PHYSICS_MELEE) && ((idx) < FvDamageType::TOTAL))
//#define PHYSICS_DMG_TYPE(idx)		(((idx) >= FvDamageType::PHYSICS_MELEE) && ((idx) <= FvDamageType::PHYSICS_NO_DEFANCE))
//#define SPELL_DMG_TYPE(idx)		(((idx) >= FvDamageType::SPELL_MELEE) && ((idx) <= FvDamageType::SPELL_NO_DEFANCE))

class FvAttackMod
{
public:
	enum 
	{
		NONE,
		MELEE,			// 进程
		RANGE,			// 远程
	};

};

class FvGameMessageIdx
{
public:
	enum Idx
	{
		NONE,
		SPELL_SUCCESS,
		SPELL_FAILED_NO_INFO,
		SPELL_FAILED_BUSY,
		SPELL_FAILED_CASTER_MOVE,
		SPELL_FAILED_CASTER_AURA,
		SPELL_FAILED_CASTER_ACTION,
		SPELL_FAILED_CASTER_POWER,///(Attr)

		SPELL_FAILED_TARGET_ERROR,
		SPELL_FAILED_TARGET_IS_FREE,
		SPELL_FAILED_TARGET_NOT_IN_FRONT,
		SPELL_FAILED_TARGET_OUT_RANGE,

		RESULT_TOTAL,
	};
};

#ifdef FV_AS_STATIC_LIBS
#define FV_GAME_DEFINE_SHARE_API
#else
#ifdef FV_GAME_DEFINE_SHARE_EXPORT
#define FV_GAME_DEFINE_SHARE_API FV_Proper_Export_Flag
#else
#define FV_GAME_DEFINE_SHARE_API FV_Proper_Import_Flag
#endif
#endif

#endif //__FvGameUnitDefine_H__
