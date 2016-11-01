//{future header message}
#ifndef __FvSysDataType_H__
#define __FvSysDataType_H__

#include "FvDataObj.h"


template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<bool>(const bool&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvUInt8>(const FvUInt8&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvUInt16>(const FvUInt16&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvUInt32>(const FvUInt32&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvUInt64>(const FvUInt64&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvInt8>(const FvInt8&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvInt16>(const FvInt16&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvInt32>(const FvInt32&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvInt64>(const FvInt64&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<float>(const float&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<double>(const double&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvString>(const FvString&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvVector2>(const FvVector2&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvVector3>(const FvVector3&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvVector4>(const FvVector4&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvObjBlob>(const FvObjBlob&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvObjUDO>(const FvObjUDO&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvObjStruct>(const FvObjStruct&);
template<>
FV_ENTITYDEF_API FvDataTypeID FvSysDataType::DataTypeID<FvObjMailBox>(const FvObjMailBox&);


#ifdef FV_SERVER

template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(bool& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const bool& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvUInt8& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvUInt8& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvUInt16& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvUInt16& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvUInt32& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvUInt32& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvUInt64& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvUInt64& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvInt8& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvInt8& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvInt16& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvInt16& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvInt32& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvInt32& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvInt64& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvInt64& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(float& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const float& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(double& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const double& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvString& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvString& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvVector2& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvVector2& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvVector3& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvVector3& kVal, FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvVector4& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::SetValue(const FvVector4& kVal, FvDataObj* pkObj);

#else

template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(bool& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvUInt8& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvUInt16& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvUInt32& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvUInt64& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvInt8& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvInt16& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvInt32& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvInt64& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(float& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(double& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvString& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvVector2& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvVector3& kVal, const FvDataObj* pkObj);
template<>
FV_ENTITYDEF_API void FvSysDataType::GetValue(FvVector4& kVal, const FvDataObj* pkObj);

#endif



template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const bool& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvInt8& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvInt16& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvInt32& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvInt64& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvUInt8& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvUInt16& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvUInt32& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvUInt64& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const float& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const double& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvString& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvVector2& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvVector3& kVal);
template<>
FV_ENTITYDEF_API FvDataObj* OldFvData::Get(const FvVector4& kVal);



#endif//__FvSysDataType_H__
