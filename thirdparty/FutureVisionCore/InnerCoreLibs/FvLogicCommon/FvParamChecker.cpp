#include "FvParamChecker.h"
#include "FvLogicCommonDefine.h"

#include <FvKernel.h>
#include <string>


template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvInt8>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvUInt8>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvInt16>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvUInt16>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvInt32>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvUInt32>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvInt64>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvUInt64>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<bool>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<float>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<double>(){}
template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<std::string>(){}


//
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvInt8>(const FvInt8&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvUInt8>(const FvUInt8&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvInt16>(const FvInt16&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvUInt16>(const FvUInt16&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvInt32>(const FvInt32&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvUInt32>(const FvUInt32&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvInt64>(const FvInt64&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<FvUInt64>(const FvUInt64&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<bool>(const bool&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<float>(const float&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<double>(const double&){}
//template<> FV_LOGIC_COMMON_API void FvParamChecker::AsynSafeParam<std::string>(const std::string&){}