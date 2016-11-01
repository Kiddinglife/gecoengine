#include "FvEntityExport.h"
#include "FvEntityData.h"
#include "FvDataScanf.h"
#include <algorithm>


bool AttribInfo::IsGhostedData() const
{
	return !!(uiDataFlag & FV_DATA_GHOSTED);
}

bool AttribInfo::IsOtherClientData() const
{
	return !!(uiDataFlag & FV_DATA_OTHER_CLIENT);
}

bool AttribInfo::IsOwnClientData() const
{
	return !!(uiDataFlag & FV_DATA_OWN_CLIENT);
}

bool AttribInfo::IsCellData() const
{
	return !this->IsBaseData();
}

bool AttribInfo::IsBaseData() const
{
	return !!(uiDataFlag & FV_DATA_BASE);
}

bool AttribInfo::IsClientOnlyData() const
{
	return !!(uiDataFlag & FV_DATA_CLIENT_ONLY);
}

bool AttribInfo::IsClientServerData() const
{
	return !!(uiDataFlag & (FV_DATA_OTHER_CLIENT | FV_DATA_OWN_CLIENT));
}

bool AttribInfo::IsPersistent() const
{
	return !!(uiDataFlag & FV_DATA_PERSISTENT);
}

bool AttribInfo::IsEditorOnly() const
{
	return !!(uiDataFlag & FV_DATA_EDITOR_ONLY);
}

bool AttribInfo::IsIdentifier() const
{
	return !!(uiDataFlag & FV_DATA_ID);
}

bool AttribInfo::IsOfType(FvDataFlags flags) const
{
	return (uiDataFlag & flags) == flags;
}

FvUInt16 AllUDOExports::NameToType(const char* pcTypeName) const
{
	FV_ASSERT(!uiCnt || pkUDOExport);
	UDOExport* pkExport = pkUDOExport;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++pkExport)
	{
		if(!strcmp(pkExport->pName, pcTypeName))
			return i;
	}

	return 0xFFFF;
}

template<typename C, typename T>
T* FindEntityExportByName(const C& kContainer, const char* pcEntityTypeName)
{
	if(!pcEntityTypeName || !pcEntityTypeName[0])
		return NULL;

	FvUInt16 uiCnt = kContainer.uiEntityCnt;
	T** ppkExport = kContainer.ppkEntityExport;
	for(FvUInt16 i=0; i<uiCnt; ++i, ++ppkExport)
	{
		if(*ppkExport && !strcmp((*ppkExport)->pName, pcEntityTypeName))
			return *ppkExport;
	}
	return NULL;
}

FvClientEntityExport* FvAllClientEntityExports::FindByName(const char* pcEntityTypeName) const
{
	return FindEntityExportByName<FvAllClientEntityExports, FvClientEntityExport>(*this, pcEntityTypeName);
}

FvRobotEntityExport* FvAllRobotEntityExports::FindByName(const char* pcEntityTypeName) const
{
	return FindEntityExportByName<FvAllRobotEntityExports, FvRobotEntityExport>(*this, pcEntityTypeName);
}

FvBaseEntityExport* FvAllBaseEntityExports::FindByName(const char* pcEntityTypeName) const
{
	return FindEntityExportByName<FvAllBaseEntityExports, FvBaseEntityExport>(*this, pcEntityTypeName);
}

FvCellEntityExport* FvAllCellEntityExports::FindByName(const char* pcEntityTypeName) const
{
	return FindEntityExportByName<FvAllCellEntityExports, FvCellEntityExport>(*this, pcEntityTypeName);
}

FvGlobalEntityExport* FvAllGlobalEntityExports::FindByName(const char* pcEntityTypeName) const
{
	return FindEntityExportByName<FvAllGlobalEntityExports, FvGlobalEntityExport>(*this, pcEntityTypeName);
}

FvLocalCallFuncByStrHandler*FvAllClientEntityExports::FindMacroCmdHandler(const FvString& kCmd, const FvString& kProtoType) const
{
	FvString kCapsName = kCmd;
	std::transform(kCapsName.begin(), kCapsName.end(), kCapsName.begin(), toupper);
	LocalCallFuncByStrHandlerMap::const_iterator itr = kMacroCommandMap.find(kCapsName);
	if(itr != kMacroCommandMap.end())
	{
		const LocalCallFuncByStrHandlerList& kList = itr->second;
		LocalCallFuncByStrHandlerList::const_iterator itrB = kList.begin();
		LocalCallFuncByStrHandlerList::const_iterator itrE = kList.end();
		for(; itrB!=itrE; ++itrB)
		{
			if(FvDataScanf::IsType(kProtoType, itrB->first))
				return itrB->second;
		}
		return NULL;
	}
	else
	{
		return NULL;
	}
}

bool FvAllClientEntityExports::HasMacroCmdName(const FvString& kCmd) const
{
	FvString kCapsName = kCmd;
	std::transform(kCapsName.begin(), kCapsName.end(), kCapsName.begin(), toupper);
	LocalCallFuncByStrHandlerMap::const_iterator itr = kMacroCommandMap.find(kCapsName);
	if(itr != kMacroCommandMap.end())
		return true;
	else
		return false;
}