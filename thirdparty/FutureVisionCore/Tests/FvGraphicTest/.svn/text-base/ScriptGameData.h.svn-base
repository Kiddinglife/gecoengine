#ifndef __GameData_H__
#define __GameData_H__

#include "VisualStruct.h"

#include <FvOwnPtr.h>
#include <FvFDBStorage.h>
#include <map>
#include <FvXMLSection.h>
#include <OgreDataStream.h>

class FvSpellInfo;


class ScriptGameData
{
public:

	static void Create();
	static void Destroy();
	static void Reload();

	template <class T>
	static const T* GetData(const FvUInt32 idx);

	template <class T>
	static const T& _GetData(const FvUInt32 idx);

	static const FvSpellInfo* GetSpellInfo(const FvUInt32 idx);
	static const VisualEffectIdx* GetVisualEffectIdx(const char* pcName);

	static bool GetFuncName(const FvString& strName, FvString& strFuncName);

private:
	friend class FvDeletable;

	ScriptGameData(void);
	~ScriptGameData(void);

	void Init();

	static ScriptGameData* ms_opInstance;

	template<class T>
	static const FvFDBStoragePtr& _DataStore();

	//void _LoadScriptData();

private:

	//void _PharseSuit(FvFDBStorage& kData);
	//void _SetItemSuit(const FvInt32 iItem, const FvInt32 iSuit);

	//void _LoadWeapon();
	//void _LoadCampRelation();
	

	//+----------------------------------------------------------------------------------------------------
	//! ITEM
	FvFDBStoragePtr m_spItemStorage;
	FvFDBStoragePtr m_spSuitInfoStorage;
	FvFDBStoragePtr m_spItemVisualStorage;

	//+----------------------------------------------------------------------------------------------------
	//! ITEM_BODY
	FvFDBStoragePtr m_spBodytorage;

	//+----------------------------------------------------------------------------------------------------
	//! NPC_INIT
	FvFDBStoragePtr m_spNPCInitStorage;
	FvFDBStoragePtr m_spNPCVisualStorage;

	//+----------------------------------------------------------------------------------------------------
	//! GOSSIP
	FvFDBStoragePtr m_spGossipStorage;
	//! LOOT
	FvFDBStoragePtr m_spLootInfoStorage;
	//DataSet<LootSetInfo> m_LootData;

	//+----------------------------------------------------------------------------------------------------
	//! LEVEL_LOOT
	FvFDBStoragePtr m_spLevelLootInfoStorage;

	//+----------------------------------------------------------------------------------------------------
	//! SALE
	FvFDBStoragePtr m_spSaleInfoStorage;

	//+----------------------------------------------------------------------------------------------------
	//! TRANSPORT
	FvFDBStoragePtr m_spTransportInfoStorage;

	//+----------------------------------------------------------------------------------------------------
	//! SPELL_LERN
	FvFDBStoragePtr m_spSpellLernInfoStorage;

	//+----------------------------------------------------------------------------------------------------
	//! REFINE
	FvFDBStoragePtr m_spRefineInfoStorage;

	//+----------------------------------------------------------------------------------------------------
	//! MAP_POSITION
	FvFDBStoragePtr m_spMapPositionInfoStorage;

	//+----------------------------------------------------------------------------------------------------
	//! Œ‰∆˜¿‡–Õ
	Ogre::DataStreamPtr m_spWeaponTypeStream;
	FvXMLSectionPtr m_spWeaponTypeXML;
	//std::map<FvUInt32, FvOwnPtr<VisualWeaponType>> m_WeaponTypes;


	//+----------------------------------------------------------------------------------------------------
	//! UI Hint
	static FvFDBStoragePtr m_spUIHintStorage;

	//+----------------------------------------------------------------------------------------------------
	//! Message
	FvFDBStoragePtr m_spMessageStorage;

	//+----------------------------------------------------------------------------------------------------
	//! VisualHitEffectEffect
	FvFDBStoragePtr m_spScriptVisualHitEffectStorage;

	//+----------------------------------------------------------------------------------------------------
	//! JackarooHelp
	FvFDBStoragePtr m_spJackarooHelpStorage;
	//std::vector<JackarooHelp*> m_kJackarooHelps;

	//+----------------------------------------------------------------------------------------------------
	//! CyclopediaHelp
	FvFDBStoragePtr m_spCyclopediaHelpStorage;
	//std::vector<CyclopediaHelp*> m_kCyclopediaHelps;

	//+----------------------------------------------------------------------------------------------------
	//! HelpContent
	FvFDBStoragePtr m_spHelpContentStorage;

	//+----------------------------------------------------------------------------------------------------
	//! GameObject
	FvFDBStoragePtr m_spGameObjectStorage;
	FvFDBStoragePtr m_spGameObjectDisplayStorage;

	//+----------------------------------------------------------------------------------------------------
	//! SkillLineAbility
	FvFDBStoragePtr m_spSkillLineAbilityStorage;

	//+----------------------------------------------------------------------------------------------------
	//! LockEntry
	FvFDBStoragePtr m_spLockEntryStorage;

	//+----------------------------------------------------------------------------------------------------
	//! PlayerCondition
	FvFDBStoragePtr m_sPlayerConditionStorage;

	//+----------------------------------------------------------------------------------------------------
	//! SpaceRegion
	FvFDBStoragePtr m_spTownRegionInfoStorage;
	FvFDBStoragePtr m_spCityRegionInfoStorage;


	//CampRelation m_kCampRelation;

	//std::map<FvString, FvString> m_GMIndictionList;
};

template <class T>
const T*ScriptGameData::GetData(const FvUInt32 idx)
{
	if(ms_opInstance == NULL)
	{
		return NULL;
	}
	const FvFDBStoragePtr& datas = _DataStore<T>();
	return datas->LookupEntry<T>(idx);
}

template <class T>
const T& ScriptGameData::_GetData(const FvUInt32 idx)
{
	if(ms_opInstance == NULL)
	{
		static T S_T;
		return S_T;
	}
	const FvFDBStoragePtr& datas = _DataStore<T>();
	const T* pkData = datas->LookupEntry<T>(idx);
	if(pkData)
	{
		return *pkData;
	}
	pkData = datas->LookupEntry<T>(0);
	if(pkData)
	{
		return *pkData;
	}
	static T S_T;
	return S_T;
}
#endif // __GameData_H__