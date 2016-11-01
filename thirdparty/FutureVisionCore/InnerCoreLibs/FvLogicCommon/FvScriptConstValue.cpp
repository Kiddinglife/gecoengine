#include "FvScriptConstValue.h"

#include <FvVector3.h>
#include <vector>

std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<float>>> S_LIST_FLOAT;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<FvInt32>>> S_LIST_INT32;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<FvVector3>>> S_LIST_VECTOR3;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<FvString>>> S_LIST_STRING;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<bool>>> S_LIST_BOOL;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<float>>>> S_LIST_FLOATS;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<FvInt32>>>> S_LIST_INT32S;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<FvVector3>>>> S_LIST_VECTOR3S;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<FvString>>>> S_LIST_STRINGS;
std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<bool>>>> S_LIST_BOOLS;

template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<float>>>& ScriptConstValueExport::S_LIST<float>(){return S_LIST_FLOAT;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<FvInt32>>>& ScriptConstValueExport::S_LIST<FvInt32>(){return S_LIST_INT32;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<FvVector3>>>& ScriptConstValueExport::S_LIST<FvVector3>(){return S_LIST_VECTOR3;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<FvString>>>& ScriptConstValueExport::S_LIST<FvString>(){return S_LIST_STRING;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<bool>>>& ScriptConstValueExport::S_LIST<bool>(){return S_LIST_BOOL;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<float>>>>& ScriptConstValueExport::S_LIST<std::vector<float>>(){return S_LIST_FLOATS;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<FvInt32>>>>& ScriptConstValueExport::S_LIST<std::vector<FvInt32>>(){return S_LIST_INT32S;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<FvVector3>>>>& ScriptConstValueExport::S_LIST<std::vector<FvVector3>>(){return S_LIST_VECTOR3S;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<FvString>>>>& ScriptConstValueExport::S_LIST<std::vector<FvString>>(){return S_LIST_STRINGS;}
template<> FV_LOGIC_COMMON_API std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<std::vector<bool>>>>& ScriptConstValueExport::S_LIST<std::vector<bool>>(){return S_LIST_BOOLS;}


