#include "ScriptGameData.h"
//#include "VisualStruct.h"
//#include "ScriptEntityInitInfo.h"
//#include "ScriptQuestStruct.h"
//#include "ScriptGossipStruct.h"
//#include "ScriptSkillStruct.h"
//#include "ScriptGameObjectStruct.h"
//#include "ScriptSpaceStruct.h"


#include "GxData.h"
#include "GxVisualStruct.h"

#include <FvSpellStruct.h>
#include <FvScriptConstValue.h>
#include <FvFDBStorageManager.h>
#include <FvSpellStruct.h>
#include <FvLogBus.h>
#include <FvLogicDebug.h>
#include <FvDestroyManager.h>

ScriptGameData* ScriptGameData::ms_opInstance = NULL;
FvFDBStoragePtr ScriptGameData::m_spUIHintStorage;

void ScriptGameData::Create()
{
	if(ms_opInstance == NULL)
	{
		GxData::Create();
		ms_opInstance = new ScriptGameData();
		ms_opInstance->Init();
	}
}

void ScriptGameData::Destroy()
{
	FV_SAFE_DELETE(ms_opInstance);
	GxData::Destroy();
}
void ScriptGameData::Reload()
{
	if(ms_opInstance)
	{
		GxData::Reload();
		FvDestroyManager::Own(*ms_opInstance);
		ms_opInstance = new ScriptGameData();
		ms_opInstance->Init();
	}
}


const char* GetFDBDir(const char* pcName)
{
	static char pcStr[1024+1] = {0};
	sprintf_s(pcStr, 1024, "Locale/LocaleCN/FDBFiles/%s", pcName);
	return pcStr;
};

template <class T>
void Load(FvFDBStoragePtr& spStorage, const char* pcFile)
{
	spStorage = FvFDBStorageManager::getSingleton().load(GetFDBDir(pcFile));
	if(&*spStorage == NULL)
	{
		FvLogBus::Error("缺少文件 >> %s", pcFile);
		return;
	}
	if(sizeof(T) != spStorage->GetHeader().m_uiRecordSize)
	{
		FvLogBus::Error("错误文件格式 >> %s [%d/%d]", pcFile, FvUInt32(sizeof(T)), FvUInt32(spStorage->GetHeader().m_uiRecordSize));
		return;
	}
}

ScriptGameData::ScriptGameData(void)
{	
}

ScriptGameData::~ScriptGameData(void)
{
	//m_WeaponTypes.clear();
}

void ScriptGameData::Init()
{
	Load<ScriptMessage>(m_spMessageStorage, "Feedback.fdb");

	GxData::Load<FvSpellInfo>(GetFDBDir("Spell.fdb"));
	GxData::Load<GxVisualSpellInfoStruct>(GetFDBDir("VisualSpell.fdb"));
	GxData::Load<GxVisualAura>(GetFDBDir("VisualAura.fdb"));
	GxData::Load<GxVisualHitEffect>(GetFDBDir("VisualHitEffect.fdb"));
	GxData::Load<GxVisualDurationEffectInfo>(GetFDBDir("VisualDurationEffect.fdb"));
	//GxData::Load<GxVisualCameraShakeStruct>(GetFDBDir("VisualCameraShake.fdb"));

	//Load<FvItemInfo>(m_spItemStorage, "Item.fdb");
	//Load<SuitInfo>(m_spSuitInfoStorage, "Suit.fdb");
	//if(&*m_spSuitInfoStorage)
	//{
	//	_PharseSuit(*m_spSuitInfoStorage);
	//}
	Load<VisualItem>(m_spItemVisualStorage, "VisualItem.fdb");
	Load<VisualBody>(m_spBodytorage, "VisualBody.fdb");

	//Load<NPCInfo>(m_spNPCInitStorage, "NPC.fdb");
	//Load<NPCVisualInfo>(m_spNPCVisualStorage, "VisualNPC.fdb");

	//Load<QuestInfo>(m_spQuestStorage, "Quest.fdb");
	//const stdext::hash_map<int,void*>& kQList = m_spQuestStorage->GetNumericIdx();
	//for (stdext::hash_map<int,void*>::const_iterator iter = kQList.begin(); 
	//	iter != kQList.end(); ++iter)
	//{
	//	FvUInt32 uiID = (*iter).first;
	//	void* pkData = (*iter).second;
	//	QuestInfo* pkQuestInfo = (QuestInfo*)pkData;
	//	m_kQuestLevelMap[pkQuestInfo->m_iReqLevelInf].push_back(uiID);
	//}
	//Load<VisualQuest>(m_spVisualQuestStorage, "VisualQuest.fdb");
	//Load<QuestGiveInfo>(m_spQuestGiveInfoStorage, "QuestGive.fdb");
	//const stdext::hash_map<int,void*>& kGList = m_spQuestGiveInfoStorage->GetNumericIdx();
	//for (stdext::hash_map<int,void*>::const_iterator iter = kGList.begin(); 
	//	iter != kGList.end(); ++iter)
	//{
	//	void* pkData = (*iter).second;
	//	QuestGiveInfo* pkQuestGiveInfo = (QuestGiveInfo*)pkData;
	//	FV_ASSERT(pkQuestGiveInfo);
	//	m_kQuestGiveInfoMap[pkQuestGiveInfo->m_iQuestID].push_back(pkQuestGiveInfo->m_iSetId);
	//}
	//Load<QuestCommitInfo>(m_spQuestCommitInfoStorage, "QuestCommit.fdb");
	//const stdext::hash_map<int,void*>& kCList = m_spQuestCommitInfoStorage->GetNumericIdx();
	//for (stdext::hash_map<int,void*>::const_iterator iter = kCList.begin(); 
	//	iter != kCList.end(); ++iter)
	//{
	//	void* pkData = (*iter).second;
	//	QuestCommitInfo* pkQuestCommitInfo = (QuestCommitInfo*)pkData;
	//	FV_ASSERT(pkQuestCommitInfo);
	//	m_kQuestCommitInfoMap[pkQuestCommitInfo->m_iQuestID].push_back(pkQuestCommitInfo->m_iSetId);
	//}

	//Load<GossipInfo>(m_spGossipStorage, "GossipInfo.fdb");

	//Load<LootSetInfo>(m_spLootInfoStorage, "LootSet.fdb");
	//Load<LootSetInfo>(m_spLevelLootInfoStorage, "LevelLootSet.fdb");

	//Load<SaleSetInfo>(m_spSaleInfoStorage, "SaleSet.fdb");
	//Load<TransportSetInfo>(m_spTransportInfoStorage, "TransportSet.fdb");
	//Load<SpellLernSetInfo>(m_spSpellLernInfoStorage, "SpellLearnSet.fdb");
	//Load<RefinePrescription>(m_spRefineInfoStorage, "Refine.fdb");
	//Load<MapPosition>(m_spMapPositionInfoStorage, "MapPosition.fdb");



	//Load<HelpContent>(m_spHelpContentStorage,"HelpContent.fdb");

	//Load<JackarooHelp>(m_spJackarooHelpStorage,"JackarooHelp.fdb");
	//const stdext::hash_map<int,void*>& kJHList = m_spJackarooHelpStorage->GetNumericIdx();
	//for (stdext::hash_map<int,void*>::const_iterator iter = kJHList.begin(); 
	//	iter != kJHList.end(); ++iter)
	//{
	//	void* pkData = (*iter).second;
	//	m_kJackarooHelps.push_back((JackarooHelp*)pkData);
	//}
	//Load<CyclopediaHelp>(m_spCyclopediaHelpStorage,"CyclopediaHelp.fdb");
	//const stdext::hash_map<int,void*>& kCHList = m_spCyclopediaHelpStorage->GetNumericIdx();
	//for (stdext::hash_map<int,void*>::const_iterator iter = kCHList.begin(); 
	//	iter != kCHList.end(); ++iter)
	//{
	//	void* pkData = (*iter).second;
	//	m_kCyclopediaHelps.push_back((CyclopediaHelp*)pkData);
	//}

	//Load<GameObjectInfo>(m_spGameObjectStorage,"GameObjectInfo.fdb");
	//Load<GameObjectDisplayInfo>(m_spGameObjectDisplayStorage,"GameObjectDisplayInfo.fdb");

	//Load<SkillLineAbilityEntry>(m_spSkillLineAbilityStorage,"SkillLineAbilityEntry.fdb");

	//Load<LockEntry>(m_spLockEntryStorage,"LockEntry.fdb");


	//Load<ValueStateCondition>(m_sPlayerConditionStorage, "StateCondition.fdb");

	//Load<TownRegionInfo>(m_spTownRegionInfoStorage, "TownRegion.fdb");
	//Load<CityRegionInfo>(m_spCityRegionInfoStorage, "CityRegion.fdb");

	//_LoadWeapon();
	//_LoadCampRelation();

	//_LoadScriptData();

}

//+----------------------------------------------------------------------------------------------------
//const LootSetInfo* ScriptGameData::GetLootInfo(const FvUInt32 idx)
//{
//	FV_ASSERT_ERROR(ms_opInstance);
//	const LootSetInfo* pkInfo = ms_opInstance->m_spLootInfoStorage->LookupEntry<LootSetInfo>(idx);
//	if(pkInfo)
//	{
//		return pkInfo;
//	}
//	return ms_opInstance->m_spLevelLootInfoStorage->LookupEntry<LootSetInfo>(idx);
//}
//+----------------------------------------------------------------------------------------------------
template<>
const FvFDBStoragePtr& ScriptGameData::_DataStore<FvSpellInfo>()
{
	return GxData::_DataStore<FvSpellInfo>();
}
template<>
const FvFDBStoragePtr& ScriptGameData::_DataStore<GxVisualSpellInfoStruct>()
{
	return GxData::_DataStore<GxVisualSpellInfoStruct>();
}
template<>
const FvFDBStoragePtr& ScriptGameData::_DataStore<GxVisualAura>()
{
	return GxData::_DataStore<GxVisualAura>();
}
template<>
const FvFDBStoragePtr& ScriptGameData::_DataStore<GxVisualHitEffect>()
{
	return GxData::_DataStore<GxVisualHitEffect>();
}
//+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<SuitInfo>()
//{
//	FV_ASSERT_ERROR(ms_opInstance);
//	return ms_opInstance->m_spSuitInfoStorage;
//}
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<FvItemInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spItemStorage;
//}
template<>
const FvFDBStoragePtr& ScriptGameData::_DataStore<VisualItem>()
{
	FV_ASSERT(ms_opInstance);
	return ms_opInstance->m_spItemVisualStorage;
}
//+----------------------------------------------------------------------------------------------------
template<>
const FvFDBStoragePtr& ScriptGameData::_DataStore<VisualBody>()
{
	FV_ASSERT(ms_opInstance);
	return ms_opInstance->m_spBodytorage;
}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<NPCInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spNPCInitStorage;
//}
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<NPCVisualInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spNPCVisualStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<GossipInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spGossipStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<QuestInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spQuestStorage;
//}
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<VisualQuest>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spVisualQuestStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<ScriptUIHint>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spUIHintStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<ScriptMessage>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spMessageStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<SaleSetInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spSaleInfoStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<SpellLernSetInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spSpellLernInfoStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<TransportSetInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spTransportInfoStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<MapPosition>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spMapPositionInfoStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<RefinePrescription>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spRefineInfoStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<HelpContent>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spHelpContentStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<GameObjectInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spGameObjectStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<GameObjectDisplayInfo>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spGameObjectDisplayStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<SkillLineAbilityEntry>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spSkillLineAbilityStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<LockEntry>()
//{
//	FV_ASSERT(ms_opInstance);
//	return ms_opInstance->m_spLockEntryStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<ValueStateCondition>()
//{
//	FV_ASSERT_ERROR(ms_opInstance);
//	return ms_opInstance->m_sPlayerConditionStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<TownRegionInfo>()
//{
//	FV_ASSERT_ERROR(ms_opInstance);
//	return ms_opInstance->m_spTownRegionInfoStorage;
//}
////+----------------------------------------------------------------------------------------------------
//template<>
//const FvFDBStoragePtr& ScriptGameData::_DataStore<CityRegionInfo>()
//{
//	FV_ASSERT_ERROR(ms_opInstance);
//	return ms_opInstance->m_spCityRegionInfoStorage;
//}

const FvSpellInfo* ScriptGameData::GetSpellInfo(const FvUInt32 idx)
{
	return GetData<FvSpellInfo>(idx);
}/*
const FvItemInfo* ScriptGameData::GetItemInfo(const FvUInt32 idx)
{
	return GetData<FvItemInfo>(idx);
}
void ScriptGameData::_SetItemSuit(const FvInt32 iItem, const FvInt32 iSuit)
{
	const FvItemInfo* pkInfo = GetItemInfo(iItem);
	if(pkInfo)
	{
		const_cast<FvItemInfo*>(pkInfo)->m_kMiscValue.SetValue("SuitIdx", iSuit);
	}
}
void ScriptGameData::_PharseSuit(FvFDBStorage& kData)
{
	const stdext::hash_map<int,void*>& kList = kData.GetNumericIdx();
	for (stdext::hash_map<int,void*>::const_iterator iter = kList.begin(); iter != kList.end(); ++iter)
	{
		void* pV = (*iter).second;
		SuitInfo* pkSuitInfo = (SuitInfo*)pV;
		FV_ASSERT(pkSuitInfo);
		for (FvUInt32 uiIdx = 0; uiIdx < SuitInfo::ITEM_CNT; ++uiIdx)
		{
			const FvInt32 iItem = pkSuitInfo->m_iItem[uiIdx];
			if(iItem)
			{
				_SetItemSuit(iItem, pkSuitInfo->m_iIdx);
			}
		}
	}
}*/

//
//void ScriptGameData::_LoadWeapon()
//{
//	Ogre::String kInitXml = "Character/WeaponType.xml";
//	m_spWeaponTypeStream = Ogre::ResourceGroupManager::getSingleton().openResource(kInitXml);
//	m_spWeaponTypeXML = FvXMLSection::OpenSection(m_spWeaponTypeStream);
//	FV_ASSERT(m_spWeaponTypeXML);
//	if(m_spWeaponTypeXML)
//	{
//		std::vector<FvXMLSectionPtr> kWeaponTypes;
//		m_spWeaponTypeXML->OpenSections("WeaponType", kWeaponTypes);
//		for(FvUInt32 i = 0; i < kWeaponTypes.size(); ++i)
//		{
//			const FvUInt32 uiType = kWeaponTypes[i]->ReadInt("Type");
//			VisualWeaponType* pTpye = new VisualWeaponType();
//			pTpye->m_pcHoldPos = kWeaponTypes[i]->ReadString("Hold").c_str();
//			pTpye->m_pcBankPos = kWeaponTypes[i]->ReadString("Bank").c_str();
//			pTpye->m_uiAnimType = uiType;
//			m_WeaponTypes[uiType] = pTpye;
//		}
//	}
//
//}
//
//const VisualWeaponType* ScriptGameData::GetWeaponType(const FvUInt32 idx)
//{
//	std::map<FvUInt32, FvOwnPtr<VisualWeaponType>>::const_iterator iter = ms_opInstance->m_WeaponTypes.find(idx);
//	if(iter == ms_opInstance->m_WeaponTypes.end())
//	{
//		return NULL;
//	}
//	else
//		return (*iter).second;
//}
//void ScriptGameData::LoadUIHit()
//{
//	Load<ScriptUIHint>(m_spUIHintStorage, "UIString.fdb");
//}
//const ScriptUIHint* ScriptGameData::GetUIHint(const FvUInt32 idx)
//{
//	if(&*m_spUIHintStorage)
//	{
//		return m_spUIHintStorage->LookupEntry<ScriptUIHint>(idx);
//	}
//	else
//		return NULL;
//}
const VisualEffectIdx* ScriptGameData::GetVisualEffectIdx(const char* pcName)
{
	if(ms_opInstance == NULL) return NULL;
	if(&(*ms_opInstance->m_spScriptVisualHitEffectStorage) == NULL) return NULL;

	const stdext::hash_map<int,void*>& kList = ms_opInstance->m_spScriptVisualHitEffectStorage->GetNumericIdx();
	for (stdext::hash_map<int,void*>::const_iterator iter = kList.begin(); 
		iter != kList.end(); ++iter)
	{
		void* pV = (*iter).second;
		VisualEffectIdx* pkScriptEffect = (VisualEffectIdx*)pV;
		FV_ASSERT(pkScriptEffect);
		if(_strcmpi(pkScriptEffect->m_pcName, pcName) == 0)
		{
			return pkScriptEffect;
		}
	}
	return NULL;
}
//const std::vector<FvUInt32>* ScriptGameData::GetQuestGiveNPC(const FvUInt32 uiQuestID)
//{
//	std::map<FvUInt32,std::vector<FvUInt32>>::const_iterator kFind = ms_opInstance->m_kQuestGiveInfoMap.find(uiQuestID);
//	if(kFind != ms_opInstance->m_kQuestGiveInfoMap.end())
//	{
//		return &kFind->second;
//	}
//	return NULL;
//}
//const std::vector<FvUInt32>* ScriptGameData::GetQuestCommitNPC(const FvUInt32 uiQuestID)
//{
//	std::map<FvUInt32,std::vector<FvUInt32>>::const_iterator kFind = ms_opInstance->m_kQuestCommitInfoMap.find(uiQuestID);
//	if(kFind != ms_opInstance->m_kQuestCommitInfoMap.end())
//	{
//		return &kFind->second;
//	}
//	return NULL;
//}
//std::map<FvUInt32,std::vector<FvUInt32>>& ScriptGameData::GetQuestLevelMap()
//{
//	return ms_opInstance->m_kQuestLevelMap;
//}
//const std::vector<JackarooHelp*>& ScriptGameData::GetJackarooHelps()
//{
//	return ms_opInstance->m_kJackarooHelps;
//}
//const std::vector<CyclopediaHelp*>& ScriptGameData::GetCyclopediaHelps()
//{
//	return ms_opInstance->m_kCyclopediaHelps;
//}
//void ScriptGameData::_LoadScriptData()
//{
//	Ogre::String kInitXml = "Test/Test_EntityInit.xml";
//	//! 测试期初始化变量
//	Ogre::DataStreamPtr spPlayerInitStream = Ogre::ResourceGroupManager::getSingleton().openResource(kInitXml);
//	FvXMLSectionPtr spPlayerInitXML = FvXMLSection::OpenSection(spPlayerInitStream);
//	FV_ASSERT_WARNING(spPlayerInitXML);
//
//	if(spPlayerInitXML)
//	{
//		//+----------------------------------------------------------------------------------------------------
//		{
//			std::vector<FvXMLSectionPtr> kInt32Datas;
//			spPlayerInitXML->OpenSections("Int32Data", kInt32Datas);
//			for(FvUInt32 uiIdx = 0; uiIdx < kInt32Datas.size(); ++uiIdx)
//			{
//				const FvString strName = kInt32Datas[uiIdx]->ReadString("Name");
//				const FvInt32 uiValue = kInt32Datas[uiIdx]->ReadInt("Value");
//				FvScriptConstValueList<FvInt32>::AddValue(strName.c_str(), uiValue);			
//			}
//		}
//		//+----------------------------------------------------------------------------------------------------
//		{
//			std::vector<FvXMLSectionPtr> kFloatDatas;
//			spPlayerInitXML->OpenSections("FloatData", kFloatDatas);
//			for(FvUInt32 uiIdx = 0; uiIdx < kFloatDatas.size(); ++uiIdx)
//			{
//				const FvString strName = kFloatDatas[uiIdx]->ReadString("Name");
//				const float fValue = kFloatDatas[uiIdx]->ReadFloat("Value");
//				FvScriptConstValueList<float>::AddValue(strName.c_str(), fValue);
//			}	
//		}
//		//+----------------------------------------------------------------------------------------------------
//		{
//			std::vector<FvXMLSectionPtr> kIndictionDatas;
//			spPlayerInitXML->OpenSections("Indiction", kIndictionDatas);
//			for(FvUInt32 uiIdx = 0; uiIdx < kIndictionDatas.size(); ++uiIdx)
//			{
//				const FvString strFuncName = kIndictionDatas[uiIdx]->ReadString("FuncName");
//				FvString strAlias = kIndictionDatas[uiIdx]->ReadString("Alias");
//				std::transform(strAlias.begin(), strAlias.end(), strAlias.begin(), toupper);
//				std::map<FvString, FvString>::iterator iter = m_GMIndictionList.find(strAlias);
//				if(iter == m_GMIndictionList.end())
//				{
//					m_GMIndictionList[strAlias] = strFuncName;
//				}
//				else
//				{
//					FvLogBus::Warning("别名冲突%s", strAlias.c_str());
//				}
//			}	
//		}	
//	}
//}
//bool ScriptGameData::GetFuncName(const FvString& strName, FvString& strFuncName)
//{
//	FV_ASSERT_ERROR(ms_opInstance);
//	std::map<FvString, FvString>::iterator iter = ms_opInstance->m_GMIndictionList.find(strName);
//	if(iter != ms_opInstance->m_GMIndictionList.end())
//	{
//		strFuncName = (*iter).second;
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//const CampRelation& ScriptGameData::GetCampRelation()
//{
//	FV_ASSERT_ERROR(ms_opInstance);
//	return ms_opInstance->m_kCampRelation;
//}
//void ScriptGameData::_LoadCampRelation()
//{
//	Ogre::String kInitXml = "Locale/LocaleCN/XML/Camp.xml";
//	Ogre::DataStreamPtr spConfigureStream = Ogre::ResourceGroupManager::getSingleton().openResource(kInitXml);
//	FvXMLSectionPtr spConfigureXML = FvXMLSection::OpenSection(spConfigureStream);
//	FV_ASSERT_WARNING(spConfigureXML);
//	if(spConfigureXML)
//	{
//		std::vector<FvXMLSectionPtr> kCampList;
//		const FvUInt32 uiCnt = spConfigureXML->ReadInt("Cnt");
//		m_kCampRelation.SetSize(uiCnt);
//		spConfigureXML->OpenSections("Camp", kCampList);
//		for(FvUInt32 uiCamp = 0;uiCamp < kCampList.size(); ++uiCamp)
//		{
//			const FvXMLSectionPtr& kCamp = kCampList[uiCamp];
//			std::vector<FvXMLSectionPtr> kRelationList;
//			kCamp->OpenSections("Relation", kRelationList);
//			for(FvUInt32 i = 0; i < kRelationList.size(); ++i)
//			{
//				const CampRelation::Relation iValue = CampRelation::Relation(kRelationList[i]->ReadInt("Value"));
//				m_kCampRelation.SetRelation(uiCamp, i, iValue);
//			}
//		}
//	}
//}