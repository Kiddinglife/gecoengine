#include "GxData.h"

#include <FvDestroyManager.h>
#include "GxVisualStruct.h"
#include <FvSpellStruct.h>


GxData* GxData::ms_opInstance = NULL;
void GxData::Create()
{
	if(ms_opInstance == NULL)
	{
		ms_opInstance = new GxData();
	}
}

void GxData::Destroy()
{
	FV_SAFE_DELETE(ms_opInstance);
}
void GxData::Reload()
{
	if(ms_opInstance)
	{
		FvDestroyManager::Own(*ms_opInstance);
		ms_opInstance = new GxData();
	}
}


//+---------------------------------------------------------------------------------------------------------------------------------------------
GxData::GxData(void)
{
}

GxData::~GxData(void)
{
}

//+---------------------------------------------------------------------------------------------------------------------------------------------
template<>
 FvFDBStoragePtr& GxData::_DataStore<FvSpellInfo>()
{
	FV_ASSERT_ERROR(ms_opInstance);
	return ms_opInstance->m_spSpellInfoStorage;
}
template<>
 FvFDBStoragePtr& GxData::_DataStore<GxVisualSpellInfoStruct>()
{
	FV_ASSERT_ERROR(ms_opInstance);
	return ms_opInstance->m_spVisualSpellInfoStorage;
}
template<>
 FvFDBStoragePtr& GxData::_DataStore<GxVisualAura>()
{
	FV_ASSERT_ERROR(ms_opInstance);
	return ms_opInstance->m_spVisualAuraStorage;
}
template<>
 FvFDBStoragePtr& GxData::_DataStore<GxVisualHitEffect>()
{
	FV_ASSERT_ERROR(ms_opInstance);
	return ms_opInstance->m_spVisualHitEffectStorage;
}
template<>
 FvFDBStoragePtr& GxData::_DataStore<GxVisualDurationEffectInfo>()
{
	FV_ASSERT_ERROR(ms_opInstance);
	return ms_opInstance->m_spDurationVisualEffectStorage;
}
template<>
 FvFDBStoragePtr& GxData::_DataStore<GxVisualCameraShakeStruct>()
{
	FV_ASSERT_ERROR(ms_opInstance);
	return ms_opInstance->m_spVisualCameraShakeStorage;
}




