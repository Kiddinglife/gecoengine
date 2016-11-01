#include "GxScriptSysDataType.h"
#include <FvDataScanf.h>
#include <FvNumFixArrayExportMacro.h>


NUM_FIXARRAY_ITEMNAME_START(AuraValueArray)
	ITEMNAME(VALUE_0)
	ITEMNAME(VALUE_1)
	ITEMNAME(VALUE_2)
	ITEMNAME(VALUE_3)
	ITEMNAME(VALUE_4)
	ITEMNAME(VALUE_5)
	ITEMNAME(VALUE_6)
	ITEMNAME(VALUE_7)
NUM_FIXARRAY_ITEMNAME_END(GX_GAME_API, AuraValueArray, FvInt32, 8)

NUM_FIXARRAY_ITEMNAME_START(ItemBaseValueArray)
	ITEMNAME(VALUE_0)
	ITEMNAME(VALUE_1)
	ITEMNAME(VALUE_2)
	ITEMNAME(VALUE_3)
	ITEMNAME(VALUE_4)
NUM_FIXARRAY_ITEMNAME_END(GX_GAME_API, ItemBaseValueArray, FvInt32, 5)

NUM_FIXARRAY_ITEMNAME_START(ItemExValueArray)
	ITEMNAME(VALUE_0)
	ITEMNAME(VALUE_1)
	ITEMNAME(VALUE_2)
	ITEMNAME(VALUE_3)
	ITEMNAME(VALUE_4)
	ITEMNAME(VALUE_5)
	ITEMNAME(VALUE_6)
	ITEMNAME(VALUE_7)
	ITEMNAME(VALUE_8)
	ITEMNAME(VALUE_9)
NUM_FIXARRAY_ITEMNAME_END(GX_GAME_API, ItemExValueArray, FvInt32, 10)

NUM_FIXARRAY_ITEMNAME_START(ItemEnchantmentArray)
	ITEMNAME(VALUE_0)
	ITEMNAME(VALUE_1)
	ITEMNAME(VALUE_2)
	ITEMNAME(VALUE_3)
	ITEMNAME(VALUE_4)
NUM_FIXARRAY_ITEMNAME_END(GX_GAME_API, ItemEnchantmentArray, FvInt32, 5)

NUM_FIXARRAY_ITEMNAME_START(ItemEnergeArray)
	ITEMNAME(VALUE_0)
	ITEMNAME(VALUE_1)
	ITEMNAME(VALUE_2)
	ITEMNAME(VALUE_3)
	ITEMNAME(VALUE_4)
NUM_FIXARRAY_ITEMNAME_END(GX_GAME_API, ItemEnergeArray, FvInt32, 5)

AuraStruct::AuraStruct()
:m_u32Key(0)
,m_u32SpellID(0)
,m_u16EffectIdx(0)
,m_u64EndTime(0)
,m_u32State(0)
,m_i32Caster(0)
{
}
AuraStruct::AuraStruct(const InitStructEnum)
:m_u32Key(0)
,m_u32SpellID(0)
,m_u16EffectIdx(0)
,m_u64EndTime(0)
,m_u32State(0)
,m_i32Caster(-1)
{
}
AuraStruct::AuraStruct(const FvUInt32 u32Key, const SpellTemplate u32SpellID, const SpellEffectIdx u16EffectIdx, const Time64 u64EndTime, const FvUInt32 u32State, const EntityID i32Caster, const AuraValueArray& kBaseValues, const AuraValueArray& kModValues, const AuraValueArray& kValues)
:m_u32Key(u32Key)
,m_u32SpellID(u32SpellID)
,m_u16EffectIdx(u16EffectIdx)
,m_u64EndTime(u64EndTime)
,m_u32State(u32State)
,m_i32Caster(i32Caster)
,m_kBaseValues(kBaseValues)
,m_kModValues(kModValues)
,m_kValues(kValues)
{
}
template<>
GX_GAME_API FvBinaryIStream& operator>>(FvBinaryIStream& kIS, AuraStruct& kDes)
{
	return kIS >> kDes.m_u32Key >> kDes.m_u32SpellID >> kDes.m_u16EffectIdx >> kDes.m_u64EndTime >> kDes.m_u32State >> kDes.m_i32Caster >> kDes.m_kBaseValues >> kDes.m_kModValues >> kDes.m_kValues;
}
template<>
GX_GAME_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const AuraStruct& kSrc)
{
	return kOS << kSrc.m_u32Key << kSrc.m_u32SpellID << kSrc.m_u16EffectIdx << kSrc.m_u64EndTime << kSrc.m_u32State << kSrc.m_i32Caster << kSrc.m_kBaseValues << kSrc.m_kModValues << kSrc.m_kValues;
}
namespace FvDataPrint
{
	template<>
	GX_GAME_API void Print(const char* pcName, const AuraStruct& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		/*FvDataPrint::Print(kMsg, "%s{", pcName);
		FvDataPrint::Print("Key", kVal.m_u32Key, kMsg);
		FvDataPrint::Print(", SpellID", kVal.m_u32SpellID, kMsg);
		FvDataPrint::Print(", EffectIdx", kVal.m_u16EffectIdx, kMsg);
		FvDataPrint::Print(", EndTime", kVal.m_u64EndTime, kMsg);
		FvDataPrint::Print(", State", kVal.m_u32State, kMsg);
		FvDataPrint::Print(", Caster", kVal.m_i32Caster, kMsg);
		FvDataPrint::Print(", BaseValues", kVal.m_kBaseValues, kMsg);
		FvDataPrint::Print(", ModValues", kVal.m_kModValues, kMsg);
		FvDataPrint::Print(", Values", kVal.m_kValues, kMsg);*/
		kMsg.append("}");
	}
	template<>
	GX_GAME_API void Print(const AuraStruct& kVal, FvString& kMsg)
	{
		/*kMsg.append("{");
		FvDataPrint::Print(kVal.m_u32Key, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u32SpellID, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u16EffectIdx, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u64EndTime, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u32State, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i32Caster, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_kBaseValues, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_kModValues, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_kValues, kMsg);
		kMsg.append("}");*/
	}
}
namespace FvXMLData
{
	template<>
	GX_GAME_API void Read(FvXMLSectionPtr spSection, AuraStruct& kVal)
	{
		FvXMLData::Read("Key", spSection, kVal.m_u32Key);
		FvXMLData::Read("SpellID", spSection, kVal.m_u32SpellID);
		FvXMLData::Read("EffectIdx", spSection, kVal.m_u16EffectIdx);
		FvXMLData::Read("EndTime", spSection, kVal.m_u64EndTime);
		FvXMLData::Read("State", spSection, kVal.m_u32State);
		FvXMLData::Read("Caster", spSection, kVal.m_i32Caster);
		FvXMLData::Read("BaseValues", spSection, kVal.m_kBaseValues);
		FvXMLData::Read("ModValues", spSection, kVal.m_kModValues);
		FvXMLData::Read("Values", spSection, kVal.m_kValues);
	}
	template<>
	GX_GAME_API void Write(FvXMLSectionPtr spSection, const AuraStruct& kVal)
	{
		FvXMLData::Write("Key", spSection, kVal.m_u32Key);
		FvXMLData::Write("SpellID", spSection, kVal.m_u32SpellID);
		FvXMLData::Write("EffectIdx", spSection, kVal.m_u16EffectIdx);
		FvXMLData::Write("EndTime", spSection, kVal.m_u64EndTime);
		FvXMLData::Write("State", spSection, kVal.m_u32State);
		FvXMLData::Write("Caster", spSection, kVal.m_i32Caster);
		FvXMLData::Write("BaseValues", spSection, kVal.m_kBaseValues);
		FvXMLData::Write("ModValues", spSection, kVal.m_kModValues);
		FvXMLData::Write("Values", spSection, kVal.m_kValues);
	}
}

VisualAuraStruct::VisualAuraStruct()
:m_u32SpellID(0)
,m_u16EffectIdx(0)
,m_u16State(0)
,m_u64EndTime(0)
{
}
VisualAuraStruct::VisualAuraStruct(const InitStructEnum)
:m_u32SpellID(0)
,m_u16EffectIdx(0)
,m_u16State(0)
,m_u64EndTime(0)
{
}
VisualAuraStruct::VisualAuraStruct(const SpellTemplate u32SpellID, const SpellEffectIdx u16EffectIdx, const FvUInt16 u16State, const Time64 u64EndTime)
:m_u32SpellID(u32SpellID)
,m_u16EffectIdx(u16EffectIdx)
,m_u16State(u16State)
,m_u64EndTime(u64EndTime)
{
}
template<>
GX_GAME_API FvBinaryIStream& operator>>(FvBinaryIStream& kIS, VisualAuraStruct& kDes)
{
	return kIS >> kDes.m_u32SpellID >> kDes.m_u16EffectIdx >> kDes.m_u16State >> kDes.m_u64EndTime;
}
template<>
GX_GAME_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const VisualAuraStruct& kSrc)
{
	return kOS << kSrc.m_u32SpellID << kSrc.m_u16EffectIdx << kSrc.m_u16State << kSrc.m_u64EndTime;
}
namespace FvDataPrint
{
	template<>
	GX_GAME_API void Print(const char* pcName, const VisualAuraStruct& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		/*FvDataPrint::Print(kMsg, "%s{", pcName);
		FvDataPrint::Print("SpellID", kVal.m_u32SpellID, kMsg);
		FvDataPrint::Print(", EffectIdx", kVal.m_u16EffectIdx, kMsg);
		FvDataPrint::Print(", State", kVal.m_u16State, kMsg);
		FvDataPrint::Print(", EndTime", kVal.m_u64EndTime, kMsg);*/
		kMsg.append("}");
	}
	template<>
	GX_GAME_API void Print(const VisualAuraStruct& kVal, FvString& kMsg)
	{
		/*kMsg.append("{");
		FvDataPrint::Print(kVal.m_u32SpellID, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u16EffectIdx, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u16State, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u64EndTime, kMsg);
		kMsg.append("}");*/
	}
}
namespace FvDataScanf
{
	template<>
	GX_GAME_API bool Scanf(StrData& kData, VisualAuraStruct& kVal)
	{
		FV_ASSERT(kData.Size() == 4);
		/*if(!FvDataScanf::Scanf(kData[0], kVal.m_u32SpellID))
			return false;
		if(!FvDataScanf::Scanf(kData[1], kVal.m_u16EffectIdx))
			return false;
		if(!FvDataScanf::Scanf(kData[2], kVal.m_u16State))
			return false;
		if(!FvDataScanf::Scanf(kData[3], kVal.m_u64EndTime))
			return false;*/
		return true;
	}
}
namespace FvXMLData
{
	template<>
	GX_GAME_API void Read(FvXMLSectionPtr spSection, VisualAuraStruct& kVal)
	{
		FvXMLData::Read("SpellID", spSection, kVal.m_u32SpellID);
		FvXMLData::Read("EffectIdx", spSection, kVal.m_u16EffectIdx);
		FvXMLData::Read("State", spSection, kVal.m_u16State);
		FvXMLData::Read("EndTime", spSection, kVal.m_u64EndTime);
	}
	template<>
	GX_GAME_API void Write(FvXMLSectionPtr spSection, const VisualAuraStruct& kVal)
	{
		FvXMLData::Write("SpellID", spSection, kVal.m_u32SpellID);
		FvXMLData::Write("EffectIdx", spSection, kVal.m_u16EffectIdx);
		FvXMLData::Write("State", spSection, kVal.m_u16State);
		FvXMLData::Write("EndTime", spSection, kVal.m_u64EndTime);
	}
}

ItemStruct::ItemStruct()
:m_u64Id(0)
,m_u32Template(0)
,m_i64Owner(0)
,m_i64Container(0)
,m_u8Bag(0)
,m_u16BagSlot(0)
,m_i64Creator(0)
,m_u64CreatedTime(0)
,m_u16StackCount(0)
,m_u64ExpireTime(0)
,m_u64RecoverEndTime(0)
,m_i32Durability(0)
,m_i32MaxDurability(0)
,m_u8Level(0)
,m_u32State(0)
,m_i32StrengthenValue(0)
,m_i32AddValueIdx(0)
,m_i32AddValue(0)
,m_i32Reserve0(0)
,m_i32Reserve1(0)
{
}
ItemStruct::ItemStruct(const InitStructEnum)
:m_u64Id(0)
,m_u32Template(0)
,m_i64Owner(-1)
,m_i64Container(-1)
,m_u8Bag(0)
,m_u16BagSlot(0)
,m_i64Creator(-1)
,m_u64CreatedTime(0)
,m_u16StackCount(0)
,m_u64ExpireTime(0)
,m_u64RecoverEndTime(0)
,m_i32Durability(0)
,m_i32MaxDurability(0)
,m_u8Level(0)
,m_u32State(0)
,m_i32StrengthenValue(0)
,m_i32AddValueIdx(0)
,m_i32AddValue(0)
,m_i32Reserve0(0)
,m_i32Reserve1(0)
{
}
ItemStruct::ItemStruct(const ItemID u64Id, const FvUInt32 u32Template, const EntityDBID i64Owner, const EntityDBID i64Container, const FvUInt8 u8Bag, const FvUInt16 u16BagSlot, const EntityDBID i64Creator, const Time64 u64CreatedTime, const FvUInt16 u16StackCount, const Time64 u64ExpireTime, const Time64 u64RecoverEndTime, const FvInt32 i32Durability, const FvInt32 i32MaxDurability, const FvUInt8 u8Level, const FvUInt32 u32State, const ItemBaseValueArray& kBaseValues, const ItemExValueArray& kExValues, const FvInt32 i32StrengthenValue, const ItemEnchantmentArray& kEnchantmentList, const FvInt32 i32AddValueIdx, const FvInt32 i32AddValue, const FvInt32 i32Reserve0, const FvInt32 i32Reserve1, const ItemEnergeArray& kEnerges)
:m_u64Id(u64Id)
,m_u32Template(u32Template)
,m_i64Owner(i64Owner)
,m_i64Container(i64Container)
,m_u8Bag(u8Bag)
,m_u16BagSlot(u16BagSlot)
,m_i64Creator(i64Creator)
,m_u64CreatedTime(u64CreatedTime)
,m_u16StackCount(u16StackCount)
,m_u64ExpireTime(u64ExpireTime)
,m_u64RecoverEndTime(u64RecoverEndTime)
,m_i32Durability(i32Durability)
,m_i32MaxDurability(i32MaxDurability)
,m_u8Level(u8Level)
,m_u32State(u32State)
,m_kBaseValues(kBaseValues)
,m_kExValues(kExValues)
,m_i32StrengthenValue(i32StrengthenValue)
,m_kEnchantmentList(kEnchantmentList)
,m_i32AddValueIdx(i32AddValueIdx)
,m_i32AddValue(i32AddValue)
,m_i32Reserve0(i32Reserve0)
,m_i32Reserve1(i32Reserve1)
,m_kEnerges(kEnerges)
{
}
template<>
GX_GAME_API FvBinaryIStream& operator>>(FvBinaryIStream& kIS, ItemStruct& kDes)
{
	return kIS >> kDes.m_u64Id >> kDes.m_u32Template >> kDes.m_i64Owner >> kDes.m_i64Container >> kDes.m_u8Bag >> kDes.m_u16BagSlot >> kDes.m_i64Creator >> kDes.m_u64CreatedTime >> kDes.m_u16StackCount >> kDes.m_u64ExpireTime >> kDes.m_u64RecoverEndTime >> kDes.m_i32Durability >> kDes.m_i32MaxDurability >> kDes.m_u8Level >> kDes.m_u32State >> kDes.m_kBaseValues >> kDes.m_kExValues >> kDes.m_i32StrengthenValue >> kDes.m_kEnchantmentList >> kDes.m_i32AddValueIdx >> kDes.m_i32AddValue >> kDes.m_i32Reserve0 >> kDes.m_i32Reserve1 >> kDes.m_kEnerges;
}
template<>
GX_GAME_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const ItemStruct& kSrc)
{
	return kOS << kSrc.m_u64Id << kSrc.m_u32Template << kSrc.m_i64Owner << kSrc.m_i64Container << kSrc.m_u8Bag << kSrc.m_u16BagSlot << kSrc.m_i64Creator << kSrc.m_u64CreatedTime << kSrc.m_u16StackCount << kSrc.m_u64ExpireTime << kSrc.m_u64RecoverEndTime << kSrc.m_i32Durability << kSrc.m_i32MaxDurability << kSrc.m_u8Level << kSrc.m_u32State << kSrc.m_kBaseValues << kSrc.m_kExValues << kSrc.m_i32StrengthenValue << kSrc.m_kEnchantmentList << kSrc.m_i32AddValueIdx << kSrc.m_i32AddValue << kSrc.m_i32Reserve0 << kSrc.m_i32Reserve1 << kSrc.m_kEnerges;
}
namespace FvDataPrint
{
	template<>
	GX_GAME_API void Print(const char* pcName, const ItemStruct& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		/*FvDataPrint::Print(kMsg, "%s{", pcName);
		FvDataPrint::Print("Id", kVal.m_u64Id, kMsg);
		FvDataPrint::Print(", Template", kVal.m_u32Template, kMsg);
		FvDataPrint::Print(", Owner", kVal.m_i64Owner, kMsg);
		FvDataPrint::Print(", Container", kVal.m_i64Container, kMsg);
		FvDataPrint::Print(", Bag", kVal.m_u8Bag, kMsg);
		FvDataPrint::Print(", BagSlot", kVal.m_u16BagSlot, kMsg);
		FvDataPrint::Print(", Creator", kVal.m_i64Creator, kMsg);
		FvDataPrint::Print(", CreatedTime", kVal.m_u64CreatedTime, kMsg);
		FvDataPrint::Print(", StackCount", kVal.m_u16StackCount, kMsg);
		FvDataPrint::Print(", ExpireTime", kVal.m_u64ExpireTime, kMsg);
		FvDataPrint::Print(", RecoverEndTime", kVal.m_u64RecoverEndTime, kMsg);
		FvDataPrint::Print(", Durability", kVal.m_i32Durability, kMsg);
		FvDataPrint::Print(", MaxDurability", kVal.m_i32MaxDurability, kMsg);
		FvDataPrint::Print(", Level", kVal.m_u8Level, kMsg);
		FvDataPrint::Print(", State", kVal.m_u32State, kMsg);
		FvDataPrint::Print(", BaseValues", kVal.m_kBaseValues, kMsg);
		FvDataPrint::Print(", ExValues", kVal.m_kExValues, kMsg);
		FvDataPrint::Print(", StrengthenValue", kVal.m_i32StrengthenValue, kMsg);
		FvDataPrint::Print(", EnchantmentList", kVal.m_kEnchantmentList, kMsg);
		FvDataPrint::Print(", AddValueIdx", kVal.m_i32AddValueIdx, kMsg);
		FvDataPrint::Print(", AddValue", kVal.m_i32AddValue, kMsg);
		FvDataPrint::Print(", Reserve0", kVal.m_i32Reserve0, kMsg);
		FvDataPrint::Print(", Reserve1", kVal.m_i32Reserve1, kMsg);
		FvDataPrint::Print(", Energes", kVal.m_kEnerges, kMsg);
		kMsg.append("}");*/
	}
	template<>
	GX_GAME_API void Print(const ItemStruct& kVal, FvString& kMsg)
	{
	/*	kMsg.append("{");
		FvDataPrint::Print(kVal.m_u64Id, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u32Template, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i64Owner, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i64Container, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u8Bag, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u16BagSlot, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i64Creator, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u64CreatedTime, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u16StackCount, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u64ExpireTime, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u64RecoverEndTime, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i32Durability, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i32MaxDurability, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u8Level, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_u32State, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_kBaseValues, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_kExValues, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i32StrengthenValue, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_kEnchantmentList, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i32AddValueIdx, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i32AddValue, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i32Reserve0, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_i32Reserve1, kMsg);
		kMsg.append(", ");
		FvDataPrint::Print(kVal.m_kEnerges, kMsg);
		kMsg.append("}");*/
	}
}
namespace FvXMLData
{
	template<>
	GX_GAME_API void Read(FvXMLSectionPtr spSection, ItemStruct& kVal)
	{
		FvXMLData::Read("Id", spSection, kVal.m_u64Id);
		FvXMLData::Read("Template", spSection, kVal.m_u32Template);
		FvXMLData::Read("Owner", spSection, kVal.m_i64Owner);
		FvXMLData::Read("Container", spSection, kVal.m_i64Container);
		FvXMLData::Read("Bag", spSection, kVal.m_u8Bag);
		FvXMLData::Read("BagSlot", spSection, kVal.m_u16BagSlot);
		FvXMLData::Read("Creator", spSection, kVal.m_i64Creator);
		FvXMLData::Read("CreatedTime", spSection, kVal.m_u64CreatedTime);
		FvXMLData::Read("StackCount", spSection, kVal.m_u16StackCount);
		FvXMLData::Read("ExpireTime", spSection, kVal.m_u64ExpireTime);
		FvXMLData::Read("RecoverEndTime", spSection, kVal.m_u64RecoverEndTime);
		FvXMLData::Read("Durability", spSection, kVal.m_i32Durability);
		FvXMLData::Read("MaxDurability", spSection, kVal.m_i32MaxDurability);
		FvXMLData::Read("Level", spSection, kVal.m_u8Level);
		FvXMLData::Read("State", spSection, kVal.m_u32State);
		FvXMLData::Read("BaseValues", spSection, kVal.m_kBaseValues);
		FvXMLData::Read("ExValues", spSection, kVal.m_kExValues);
		FvXMLData::Read("StrengthenValue", spSection, kVal.m_i32StrengthenValue);
		FvXMLData::Read("EnchantmentList", spSection, kVal.m_kEnchantmentList);
		FvXMLData::Read("AddValueIdx", spSection, kVal.m_i32AddValueIdx);
		FvXMLData::Read("AddValue", spSection, kVal.m_i32AddValue);
		FvXMLData::Read("Reserve0", spSection, kVal.m_i32Reserve0);
		FvXMLData::Read("Reserve1", spSection, kVal.m_i32Reserve1);
		FvXMLData::Read("Energes", spSection, kVal.m_kEnerges);
	}
	template<>
	GX_GAME_API void Write(FvXMLSectionPtr spSection, const ItemStruct& kVal)
	{
		FvXMLData::Write("Id", spSection, kVal.m_u64Id);
		FvXMLData::Write("Template", spSection, kVal.m_u32Template);
		FvXMLData::Write("Owner", spSection, kVal.m_i64Owner);
		FvXMLData::Write("Container", spSection, kVal.m_i64Container);
		FvXMLData::Write("Bag", spSection, kVal.m_u8Bag);
		FvXMLData::Write("BagSlot", spSection, kVal.m_u16BagSlot);
		FvXMLData::Write("Creator", spSection, kVal.m_i64Creator);
		FvXMLData::Write("CreatedTime", spSection, kVal.m_u64CreatedTime);
		FvXMLData::Write("StackCount", spSection, kVal.m_u16StackCount);
		FvXMLData::Write("ExpireTime", spSection, kVal.m_u64ExpireTime);
		FvXMLData::Write("RecoverEndTime", spSection, kVal.m_u64RecoverEndTime);
		FvXMLData::Write("Durability", spSection, kVal.m_i32Durability);
		FvXMLData::Write("MaxDurability", spSection, kVal.m_i32MaxDurability);
		FvXMLData::Write("Level", spSection, kVal.m_u8Level);
		FvXMLData::Write("State", spSection, kVal.m_u32State);
		FvXMLData::Write("BaseValues", spSection, kVal.m_kBaseValues);
		FvXMLData::Write("ExValues", spSection, kVal.m_kExValues);
		FvXMLData::Write("StrengthenValue", spSection, kVal.m_i32StrengthenValue);
		FvXMLData::Write("EnchantmentList", spSection, kVal.m_kEnchantmentList);
		FvXMLData::Write("AddValueIdx", spSection, kVal.m_i32AddValueIdx);
		FvXMLData::Write("AddValue", spSection, kVal.m_i32AddValue);
		FvXMLData::Write("Reserve0", spSection, kVal.m_i32Reserve0);
		FvXMLData::Write("Reserve1", spSection, kVal.m_i32Reserve1);
		FvXMLData::Write("Energes", spSection, kVal.m_kEnerges);
	}
}

