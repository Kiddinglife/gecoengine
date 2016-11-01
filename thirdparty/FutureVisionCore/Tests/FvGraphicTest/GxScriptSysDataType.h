//{future header message}
#ifndef __GxScriptSysDataType_H__
#define __GxScriptSysDataType_H__

#include "GxScriptMailBox.h"
#include <FvCommonExportMacro.h>


typedef std::vector<FvInt32> ARRAYINT32;

typedef std::vector<FvInt64> ARRAYINT64;

typedef FvInt32 EntityID;

typedef FvInt64 EntityDBID;

typedef FvUInt64 Time64;

typedef FvUInt16 Time16;

typedef FvUInt32 ItemTemplate;

typedef FvInt16 ItemStack;

typedef FvUInt64 ItemID;

typedef FvUInt32 SpellTemplate;

typedef FvUInt16 SpellEffectIdx;

class  AuraValueArray : public FvObjNumFixArray<FvInt32, 8>
{
public:
	enum Idx
	{
		VALUE_0,
		VALUE_1,
		VALUE_2,
		VALUE_3,
		VALUE_4,
		VALUE_5,
		VALUE_6,
		VALUE_7,
		TOTAL
	};

	virtual const char** GetItemName() const;
};

class  ItemBaseValueArray : public FvObjNumFixArray<FvInt32, 5>
{
public:
	enum Idx
	{
		VALUE_0,
		VALUE_1,
		VALUE_2,
		VALUE_3,
		VALUE_4,
		TOTAL
	};

	virtual const char** GetItemName() const;
};

class  ItemExValueArray : public FvObjNumFixArray<FvInt32, 10>
{
public:
	enum Idx
	{
		VALUE_0,
		VALUE_1,
		VALUE_2,
		VALUE_3,
		VALUE_4,
		VALUE_5,
		VALUE_6,
		VALUE_7,
		VALUE_8,
		VALUE_9,
		TOTAL
	};

	virtual const char** GetItemName() const;
};

class  ItemEnchantmentArray : public FvObjNumFixArray<FvInt32, 5>
{
public:
	enum Idx
	{
		VALUE_0,
		VALUE_1,
		VALUE_2,
		VALUE_3,
		VALUE_4,
		TOTAL
	};

	virtual const char** GetItemName() const;
};

class  ItemEnergeArray : public FvObjNumFixArray<FvInt32, 5>
{
public:
	enum Idx
	{
		VALUE_0,
		VALUE_1,
		VALUE_2,
		VALUE_3,
		VALUE_4,
		TOTAL
	};

	virtual const char** GetItemName() const;
};

struct  AuraStruct
{
	AuraStruct();
	AuraStruct(const InitStructEnum);
	AuraStruct(const FvUInt32 u32Key, const SpellTemplate u32SpellID, const SpellEffectIdx u16EffectIdx, const Time64 u64EndTime, const FvUInt32 u32State, const EntityID i32Caster, const AuraValueArray& kBaseValues, const AuraValueArray& kModValues, const AuraValueArray& kValues);

	FvUInt32 m_u32Key;
	SpellTemplate m_u32SpellID;
	SpellEffectIdx m_u16EffectIdx;
	Time64 m_u64EndTime;
	FvUInt32 m_u32State;
	EntityID m_i32Caster;
	AuraValueArray m_kBaseValues;
	AuraValueArray m_kModValues;
	AuraValueArray m_kValues;
};

struct  VisualAuraStruct
{
	VisualAuraStruct();
	VisualAuraStruct(const InitStructEnum);
	VisualAuraStruct(const SpellTemplate u32SpellID, const SpellEffectIdx u16EffectIdx, const FvUInt16 u16State, const Time64 u64EndTime);

	SpellTemplate m_u32SpellID;
	SpellEffectIdx m_u16EffectIdx;
	FvUInt16 m_u16State;
	Time64 m_u64EndTime;
};

struct  ItemStruct
{
	ItemStruct();
	ItemStruct(const InitStructEnum);
	ItemStruct(const ItemID u64Id, const FvUInt32 u32Template, const EntityDBID i64Owner, const EntityDBID i64Container, const FvUInt8 u8Bag, const FvUInt16 u16BagSlot, const EntityDBID i64Creator, const Time64 u64CreatedTime, const FvUInt16 u16StackCount, const Time64 u64ExpireTime, const Time64 u64RecoverEndTime, const FvInt32 i32Durability, const FvInt32 i32MaxDurability, const FvUInt8 u8Level, const FvUInt32 u32State, const ItemBaseValueArray& kBaseValues, const ItemExValueArray& kExValues, const FvInt32 i32StrengthenValue, const ItemEnchantmentArray& kEnchantmentList, const FvInt32 i32AddValueIdx, const FvInt32 i32AddValue, const FvInt32 i32Reserve0, const FvInt32 i32Reserve1, const ItemEnergeArray& kEnerges);

	ItemID m_u64Id;
	FvUInt32 m_u32Template;
	EntityDBID m_i64Owner;
	EntityDBID m_i64Container;
	FvUInt8 m_u8Bag;
	FvUInt16 m_u16BagSlot;
	EntityDBID m_i64Creator;
	Time64 m_u64CreatedTime;
	FvUInt16 m_u16StackCount;
	Time64 m_u64ExpireTime;
	Time64 m_u64RecoverEndTime;
	FvInt32 m_i32Durability;
	FvInt32 m_i32MaxDurability;
	FvUInt8 m_u8Level;
	FvUInt32 m_u32State;
	ItemBaseValueArray m_kBaseValues;
	ItemExValueArray m_kExValues;
	FvInt32 m_i32StrengthenValue;
	ItemEnchantmentArray m_kEnchantmentList;
	FvInt32 m_i32AddValueIdx;
	FvInt32 m_i32AddValue;
	FvInt32 m_i32Reserve0;
	FvInt32 m_i32Reserve1;
	ItemEnergeArray m_kEnerges;
};


#endif//__GxScriptSysDataType_H__