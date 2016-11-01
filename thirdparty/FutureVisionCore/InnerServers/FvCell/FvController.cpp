#include "FvController.h"
#include "FvCellEntity.h"


FvController::FvController() : 
m_uiID(0),
m_bIsNew(false),
m_pkParent(NULL),
m_pkEntity(NULL),
m_uiMemoryOffset(0)
{
	
}

FvController::FvController(FvEntity* pkEntity) :
m_uiID(0),
m_bIsNew(false),
m_pkParent(NULL),
m_pkEntity(pkEntity)
{
	FV_ASSERT(m_pkEntity);
	if(m_pkEntity->m_kFreeIDs.empty())
	{
		m_uiID = m_pkEntity->m_kControllers.size();
		m_pkEntity->m_kControllers.push_back(this);
	}
	else
	{
		m_uiID = m_pkEntity->m_kFreeIDs.front();
		m_pkEntity->m_kFreeIDs.pop();
		FV_ASSERT(m_pkEntity->m_kControllers[m_uiID] == NULL);
		m_pkEntity->m_kControllers[m_uiID] = this;
	}
	m_uiMemoryOffset = (FvUInt32(this) - FvUInt32(pkEntity));
}

FvController::FvController(FvController* pkParent) :
m_uiID(0),
m_bIsNew(false),
m_pkParent(pkParent),
m_pkEntity(pkParent->GetEntity())
{
	FV_ASSERT(m_pkParent && m_pkEntity);
	if(m_pkEntity->m_kFreeIDs.empty())
	{
		m_uiID = m_pkEntity->m_kControllers.size();
		m_pkEntity->m_kControllers.push_back(this);
	}
	else
	{
		m_uiID = m_pkEntity->m_kFreeIDs.front();
		m_pkEntity->m_kFreeIDs.pop();
		FV_ASSERT(m_pkEntity->m_kControllers[m_uiID] == NULL);
		m_pkEntity->m_kControllers[m_uiID] = this;
	}

	m_uiMemoryOffset = (FvUInt32(this) - FvUInt32(pkParent));
}

FvUInt16 FvControllerFactory::ms_uiTypeIdentify = 0;
std::map<FvUInt32,FvControllerFactory::Creator> FvControllerFactory::ms_kCreators;