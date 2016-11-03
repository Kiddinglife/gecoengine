#include "FvLogicCommonExport.h"


FvLogicCommonExport::FvLogicCommonExport(void)
{
}

FvLogicCommonExport::~FvLogicCommonExport(void)
{
}

///<导出ExportClass0<bool>
template<> FV_LOGIC_COMMON_API void FvLogicCommonExport::Export_API<bool>(){ExportClass0<bool>::S_VALUE;}
template<> FvUInt32 ExportClass0<bool>::S_VALUE;

///<导出ExportClass1<float>
template<> FV_LOGIC_COMMON_API void FvLogicCommonExport::Export_API<float>(){ExportClass1<float>::S_VALUE;}
template<> FvUInt8 ExportClass1<float>::S_VALUE;
