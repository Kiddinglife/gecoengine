#pragma once

#include "GxScriptDefine.h"

#include <FvFDBStorage.h>
#include <FvLogicDebug.h>
#include <OgreResourceManager.h>
#include <FvFDBStorageManager.h>

class  GxData
{
public:

	static void Create();
	static void Destroy();
	static void Reload();

	template <class T>
	static const T* Get(const FvUInt32 uiIdx);
	template <class T>
	static void Load(const std::string& pcFile);
	template<class T>
	static FvFDBStoragePtr& _DataStore();

private:
	friend class FvDeletable;
	static GxData* ms_opInstance;

	GxData(void);
	~GxData(void);


	//+----------------------------------------------------------------------------------------------------
	//! SPELL
	FvFDBStoragePtr m_spSpellInfoStorage;
	FvFDBStoragePtr m_spVisualSpellInfoStorage;
	FvFDBStoragePtr m_spVisualAuraStorage;
	FvFDBStoragePtr m_spVisualHitEffectStorage;

	//+----------------------------------------------------------------------------------------------------
	//! ScriptDurationVisualEffect
	FvFDBStoragePtr m_spDurationVisualEffectStorage;

	//+----------------------------------------------------------------------------------------------------
	//! VisualCameraShake
	FvFDBStoragePtr m_spVisualCameraShakeStorage;

};


template <class T>
const T* GxData::Get(const FvUInt32 uiIdx)
{
	FvFDBStoragePtr& spDB = _DataStore<T>();
	return spDB->LookupEntry<T>(uiIdx);
}
template <class T>
void GxData::Load(const std::string& pcFile)
{
	FvFDBStoragePtr& spDB = _DataStore<T>();
	spDB = FvFDBStorageManager::getSingleton().load(pcFile.c_str());
	if(&*spDB == NULL)
	{
		FvLogBus::Error("缺少文件 >> %s", pcFile);
		return;
	}
	if(sizeof(T) != spDB->GetHeader().m_uiRecordSize)
	{
		FvLogBus::Error("错误文件格式 >> %s", pcFile);
		return;
	}
}