//{future header message}
#ifndef __FvDataObj_H__
#define __FvDataObj_H__

#include "FvDataOwner.h"
#include "FvIndexVector.h"
#include <map>
#include <FvBinaryStream.h>
#include <FvXMLSection.h>
#include <FvNetTypes.h>
#include <FvUniqueID.h>
#include "FvDataPrint.h"


//! 导出函数的前缀
#define FV_C_CALL	__cdecl

template<class, class> class FvObjArray;
template<class, FvUInt32> class FvObjNumFixArray;


enum FvDataTypeID
{
	FVOBJ_BOOL = 0,
	FVOBJ_UINT8,
	FVOBJ_UINT16,
	FVOBJ_UINT32,
	FVOBJ_UINT64,
	FVOBJ_INT8,
	FVOBJ_INT16,
	FVOBJ_INT32,
	FVOBJ_INT64,
	FVOBJ_FLOAT,
	FVOBJ_DOUBLE,
	FVOBJ_STRING,
	FVOBJ_VECTOR2,
	FVOBJ_VECTOR3,
	FVOBJ_VECTOR4,
	FVOBJ_MAILBOX,
	FVOBJ_UDO,
	FVOBJ_BLOB,
	FVOBJ_ARRAY,
	FVOBJ_STRUCT,
	FVOBJ_NUM_FIXARRAY,
	FVOBJ_UNKNOW = 255,
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FvSysDataType
{
public:
	template<class T>
	static FvDataTypeID DataTypeID(const T&);

	template<class T>
	static FvDataTypeID DataTypeID() { T* pkT = NULL; return FvSysDataType::DataTypeID<T>(*pkT); }

	template<class FT, class ST>
	static FvDataTypeID DataTypeID(const FvObjArray<FT, ST>&) { return FVOBJ_ARRAY; }

	template<class T, FvUInt32 SIZE>
	static FvDataTypeID DataTypeID(const FvObjNumFixArray<T, SIZE>&) { return FVOBJ_NUM_FIXARRAY; }

	template<class T>
	static void GetValue(T& kVal, const FvDataObj* pkObj);

	template<class T>
	static void SetValue(const T& kVal, FvDataObj* pkObj);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FV_ENTITYDEF_API FvDataObj : public FvDataOwner
{
public:
	FvDataObj();
	virtual ~FvDataObj();

	virtual FvDataTypeID GetDataTypeID() const = 0;

	template<class T>
	bool GetDataValue(T& kValue) const
	{
		if(GetDataTypeID() == FvSysDataType::DataTypeID<T>(kValue))
		{
			FvSysDataType::GetValue(kValue, this);
			return true;
		}
		else
		{
			return false;
		}
	}
	template<class T>
	const T DataValue(const T& kDefaultValue) const
	{
		if(GetDataTypeID() == FvSysDataType::DataTypeID<T>(kDefaultValue))
		{
			T kValue;
			FvSysDataType::GetValue(kValue, this);
			return kValue;
		}
		else
		{
			return kDefaultValue;
		}
	}
#ifdef FV_SERVER
	template<class T>
	bool SetDataValue(const T& kValue)
	{
		if(GetDataTypeID() == FvSysDataType::DataTypeID<T>(kValue))
		{
			FvSysDataType::SetValue(kValue, this);
			return true;
		}
		else
		{
			return false;
		}
	}
#else
	template<class T>
	bool SetDataValue(const T& kValue);
#endif

protected:
#ifdef FV_SERVER
	FvDataOwner* GetRootDataForLeaf(OpCode uiOpCode);
#endif

	virtual bool SerializeToStream(FvBinaryOStream& kOS) const = 0;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS) = 0;
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const = 0;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection) = 0;

	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend class FvClientAttrib;
	friend class FvBaseAttrib;
	friend class FvCellAttrib;
	friend class FvRobotAttrib;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class TYPE, FvDataTypeID TYPEID>
class FvSimpleDataObj : public FvDataObj
{
public:
	FvSimpleDataObj():FvDataObj(), m_kVal(0) {}
	FvSimpleDataObj(const FvSimpleDataObj& src):FvDataObj(), m_kVal(src.m_kVal) {}
	FvSimpleDataObj(const TYPE v):FvDataObj(), m_kVal(v) {}

	virtual FvDataTypeID GetDataTypeID() const { return TYPEID; }
	static FvDataTypeID DataTypeID() { return TYPEID; }
	static FvSimpleDataObj* OldData(const FvSimpleDataObj& kVal) { ms_kOldData.m_kVal=kVal.m_kVal; return &ms_kOldData; }

	operator TYPE() { return m_kVal; }
	operator const TYPE() const { return m_kVal; }

#ifdef FV_SERVER
	FvSimpleDataObj& operator=(const FvSimpleDataObj& val)
	{
		if(this == &val || m_kVal == val.m_kVal)
			return *this;
		m_kVal = val.m_kVal;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator++()
	{
		++m_kVal;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj  operator++(int)
	{
		TYPE kTmp = m_kVal;
		++m_kVal;
		NofityDataChg();
		return kTmp;
	}
	FvSimpleDataObj& operator--()
	{
		--m_kVal;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj  operator--(int)
	{
		TYPE kTmp = m_kVal;
		--m_kVal;
		NofityDataChg();
		return kTmp;
	}
	FvSimpleDataObj& operator+=(const FvSimpleDataObj& kRight)
	{
		if(kRight.m_kVal == 0)
			return *this;
		m_kVal += kRight.m_kVal;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator-=(const FvSimpleDataObj& kRight)
	{
		if(kRight.m_kVal == 0)
			return *this;
		m_kVal -= kRight.m_kVal;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator*=(const FvSimpleDataObj& kRight)
	{
		if(kRight.m_kVal == 1)
			return *this;
		m_kVal *= kRight.m_kVal;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator/=(const FvSimpleDataObj& kRight)
	{
		FV_ASSERT(kRight.m_kVal != 0);
		if(kRight.m_kVal == 1)
			return *this;
		m_kVal /= kRight.m_kVal;
		NofityDataChg();
		return *this;
	}
#ifndef FV_SIMPLE_DATA_OBJ_IS_FLOAT_TYPE
	FvSimpleDataObj& operator%=(const FvSimpleDataObj& kRight)
	{
		TYPE kTmp = m_kVal % kRight.m_kVal;
		if(m_kVal == kTmp)
			return *this;
		m_kVal = kTmp;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator&=(const FvSimpleDataObj& kRight)
	{
		TYPE kTmp = m_kVal & kRight.m_kVal;
		if(m_kVal == kTmp)
			return *this;
		m_kVal = kTmp;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator^=(const FvSimpleDataObj& kRight)
	{
		TYPE kTmp = m_kVal ^ kRight.m_kVal;
		if(m_kVal == kTmp)
			return *this;
		m_kVal = kTmp;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator|=(const FvSimpleDataObj& kRight)
	{
		TYPE kTmp = m_kVal | kRight.m_kVal;
		if(m_kVal == kTmp)
			return *this;
		m_kVal = kTmp;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator<<=(const FvSimpleDataObj& kRight)
	{
		if(kRight.m_kVal == 0)
			return *this;
		m_kVal <<= kRight.m_kVal;
		NofityDataChg();
		return *this;
	}
	FvSimpleDataObj& operator>>=(const FvSimpleDataObj& kRight)
	{
		if(kRight.m_kVal == 0)
			return *this;
		m_kVal >>= kRight.m_kVal;
		NofityDataChg();
		return *this;
	}
#endif
#endif

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
	{
		FV_ASSERT(uiOpCode == OPCD_MOD);
		if(pkCB)
		{
			ms_kOldData.m_kVal = m_kVal;
			kIS >> m_kVal;
			pkCB->Handle(OPCD_MOD, &ms_kOldData);
		}
		else
		{
			kIS >> m_kVal;
		}
		FV_ASSERT(!kIS.Error());
	}
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const
	{
		kOS << m_kVal;
		return true;
	}
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS)
	{
		kIS >> m_kVal;
		return !kIS.Error();
	}
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const
	{
		return spSection->Set<TYPE>(m_kVal);
	}
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection)
	{
		m_kVal = spSection->As<TYPE>(m_kVal);
		return true;
	}
#ifdef FV_SERVER
	void EqualTo(const FvSimpleDataObj& kRight)
	{
		m_kVal = kRight.m_kVal;
	}
	void NofityDataChg()
	{
		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
		{
			ms_kDataStream << m_kVal;
			pkRoot->NotifyDataChanged();
		}
	}
#endif
	template<class, FvDataTypeID> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const FvSimpleDataObj&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend class OldFvData;

protected:
	TYPE					m_kVal;
	static FvSimpleDataObj	ms_kOldData;
};
template<class TYPE, FvDataTypeID TYPEID>
FvSimpleDataObj<TYPE, TYPEID> FvSimpleDataObj<TYPE, TYPEID>::ms_kOldData;

template<class TYPE, FvDataTypeID TYPEID>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvSimpleDataObj<TYPE, TYPEID>& kVal)
{
	kVal.SerializeToStream(kOS);
	return kOS;
}

typedef FvSimpleDataObj<FvInt8, FVOBJ_INT8> FvObjInt8;
typedef FvSimpleDataObj<FvUInt8, FVOBJ_UINT8> FvObjUInt8;
typedef FvSimpleDataObj<FvInt16, FVOBJ_INT16> FvObjInt16;
typedef FvSimpleDataObj<FvUInt16, FVOBJ_UINT16> FvObjUInt16;
typedef FvSimpleDataObj<FvInt32, FVOBJ_INT32> FvObjInt32;
typedef FvSimpleDataObj<FvUInt32, FVOBJ_UINT32> FvObjUInt32;
typedef FvSimpleDataObj<FvInt64, FVOBJ_INT64> FvObjInt64;
typedef FvSimpleDataObj<FvUInt64, FVOBJ_UINT64> FvObjUInt64;
#define FV_SIMPLE_DATA_OBJ_IS_FLOAT_TYPE
typedef FvSimpleDataObj<float, FVOBJ_FLOAT> FvObjFloat;
typedef FvSimpleDataObj<double, FVOBJ_DOUBLE> FvObjDouble;
#undef FV_SIMPLE_DATA_OBJ_IS_FLOAT_TYPE


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FV_ENTITYDEF_API FvObjBool : public FvDataObj
{
public:
	FvObjBool():FvDataObj(), m_kVal(false) {}
	FvObjBool(const bool kVal):FvDataObj(), m_kVal(kVal) {}
	FvObjBool(const FvObjBool& kSrc):FvDataObj(), m_kVal(kSrc.m_kVal) {}

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_BOOL; }
	static FvDataTypeID DataTypeID() { return FVOBJ_BOOL; }
	static FvObjBool* OldData(const FvObjBool& kVal) { ms_kOldData.m_kVal=kVal.m_kVal; return &ms_kOldData; }

	operator bool() { return m_kVal; }
	operator const bool() const { return m_kVal; }

#ifndef FV_SERVER
protected:
#endif
	FvObjBool& operator=(const FvObjBool& val);

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS);
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	void EqualTo(const FvObjBool& kRight);
#endif

	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend class OldFvData;

protected:
	bool				m_kVal;
	static FvObjBool	ms_kOldData;
};
template<>
FV_ENTITYDEF_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjBool& kVal);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FV_ENTITYDEF_API FvObjString : public FvDataObj
{
public:
	FvObjString():FvDataObj() {}
	FvObjString(const char* pcVal):FvDataObj(), m_kVal(pcVal) {}
	FvObjString(const FvString& kVal):FvDataObj(), m_kVal(kVal) {}
	FvObjString(const FvObjString& kSrc):FvDataObj(), m_kVal(kSrc.m_kVal) {}

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_STRING; }
	static FvDataTypeID DataTypeID() { return FVOBJ_STRING; }
	static FvObjString* OldData(const FvObjString& kVal) { ms_kOldData.m_kVal=kVal.m_kVal; return &ms_kOldData; }

	operator FvString() { return m_kVal; }
	operator const FvString() const { return m_kVal; }

	FvUInt32 size() const { return m_kVal.size(); }
	FvUInt32 length() const { return m_kVal.length(); }
	FvUInt32 capacity() const { return m_kVal.capacity(); }
	bool empty() const { return m_kVal.empty(); }
	const char* c_str() const { return m_kVal.c_str(); }
	void reserve(FvUInt32 uiCap = 0) { m_kVal.reserve(uiCap); }

	bool operator==(const FvObjString& kRight) const { return m_kVal == kRight.m_kVal; }
	bool operator!=(const FvObjString& kRight) const { return m_kVal != kRight.m_kVal; }
	char operator[](FvUInt32 uiIdx) { return m_kVal[uiIdx]; }
	const char operator[](FvUInt32 uiIdx) const { return m_kVal[uiIdx]; }

#ifndef FV_SERVER
protected:
#endif
	FvObjString& operator=(const FvObjString& val);
	FvObjString& operator+=(const FvObjString& val);
	FvObjString& append(const FvObjString& val);
	FvObjString& assign(const FvObjString& val);
	void resize(FvUInt32 uiSize);
	void resize(FvUInt32 uiSize, char c);
	void clear();

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS);
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	void EqualTo(const FvObjString& kRight);
	void NofityDataChg();
#endif

	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend class OldFvData;

protected:
	FvString			m_kVal;
	static FvObjString	ms_kOldData;
};
template<>
FV_ENTITYDEF_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjString& kVal);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FV_ENTITYDEF_API FvObjVector2 : public FvDataObj
{
public:
	FvObjVector2():FvDataObj() {}
	FvObjVector2(const FvVector2& v):FvDataObj(), m_kVal(v) {}
	FvObjVector2(const float x, const float y):FvDataObj(), m_kVal(x, y) {}
	FvObjVector2(const float* pfVector):FvDataObj(), m_kVal(pfVector) {}
	FvObjVector2(const FvObjVector2& src):FvDataObj(), m_kVal(src.m_kVal) {}

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_VECTOR2; }
	static FvDataTypeID DataTypeID() { return FVOBJ_VECTOR2; }
	static FvObjVector2* OldData(const FvObjVector2& kVal) { ms_kOldData.m_kVal=kVal.m_kVal; return &ms_kOldData; }

	operator FvVector2() { return m_kVal; }
	operator const FvVector2() const { return m_kVal; }
	operator const float*() const { return m_kVal; }

	float operator[](FvVector2::Coord eCoord) const { return m_kVal[eCoord]; }

	FvObjVector2 operator+() const { return m_kVal; }
	FvObjVector2 operator-() const { return -m_kVal; }

	bool operator==(const FvObjVector2& kRight) const { return m_kVal==kRight.m_kVal; }
	bool operator!=(const FvObjVector2& kRight) const { return m_kVal!=kRight.m_kVal; }

	float Length() const { return m_kVal.Length(); }
	float LengthSquared() const { return m_kVal.LengthSquared(); }
	FvObjVector2 UnitVector() const { return m_kVal.UnitVector(); }
	float DotProduct(const FvObjVector2& kVector) const { return m_kVal.DotProduct(kVector.m_kVal); }
	float CrossProduct(const FvObjVector2& kVector) const { return m_kVal.CrossProduct(kVector.m_kVal); }
	FvObjVector2 ProjectOnto(const FvObjVector2& kVector) const { return m_kVal.ProjectOnto(kVector.m_kVal); }

#ifndef FV_SERVER
protected:
#endif
	FvObjVector2& operator=(const FvObjVector2& val);
	FvObjVector2& operator+=(const FvObjVector2& kVector);
	FvObjVector2& operator-=(const FvObjVector2& kVector);
	FvObjVector2& operator*=(float fScale);
	FvObjVector2& operator/=(float fScale);

	void SetZero();
	void Set(float fX, float fY);
	void Scale(const FvObjVector2& kVector, float fScale);
	void Normalise();
	void ParallelMultiply(const FvObjVector2& kVector);
	void ParallelMultiply(const FvObjVector2& kVector0, const FvObjVector2& kVector1);
	void Lerp(const FvObjVector2& kVector0, const FvObjVector2& kVector1, float fAlpha);
	void Clamp(const FvObjVector2& kLower, const FvObjVector2& kUpper);
	void ProjectOnto(const FvObjVector2& kVector0, const FvObjVector2& kVector1);

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS);
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	void EqualTo(const FvObjVector2& kRight);
	void NofityDataChg();
#endif

	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend class OldFvData;

protected:
	FvVector2			m_kVal;
	static FvObjVector2	ms_kOldData;
};
template<>
FV_ENTITYDEF_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjVector2& kVal);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FV_ENTITYDEF_API FvObjVector3 : public FvDataObj
{
public:
	FvObjVector3():FvDataObj() {}
	FvObjVector3(const FvVector3& v):FvDataObj(), m_kVal(v) {}
	FvObjVector3(const float x, const float y, const float z):FvDataObj(), m_kVal(x, y, z) {}
	FvObjVector3(const float* pfVector):FvDataObj(), m_kVal(pfVector) {}
	FvObjVector3(const FvObjVector3& src):FvDataObj(), m_kVal(src.m_kVal) {}

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_VECTOR3; }
	static FvDataTypeID DataTypeID() { return FVOBJ_VECTOR3; }
	static FvObjVector3* OldData(const FvObjVector3& kVal) { ms_kOldData.m_kVal=kVal.m_kVal; return &ms_kOldData; }

	operator FvVector3() { return m_kVal; }
	operator const FvVector3() const { return m_kVal; }
	operator const float*() const { return m_kVal; }

	float operator[](FvVector3::Coord eCoord) const { return m_kVal[eCoord]; }

	FvObjVector3 operator+() const { return m_kVal; }
	FvObjVector3 operator-() const { return -m_kVal; }

	bool operator==(const FvObjVector3& kRight) const { return m_kVal==kRight.m_kVal; }
	bool operator!=(const FvObjVector3& kRight) const { return m_kVal!=kRight.m_kVal; }

	float Length() const { return m_kVal.Length(); }
	float LengthSquared() const { return m_kVal.LengthSquared(); }
	FvObjVector3 UnitVector() const { return m_kVal.UnitVector(); }
	float GetYaw() const { return m_kVal.GetYaw(); }
	float GetPitch() const { return m_kVal.GetPitch(); }
	float DotProduct(const FvObjVector3& kVector) const { return m_kVal.DotProduct(kVector.m_kVal); }
	FvObjVector3 CrossProduct(const FvObjVector3& kVector) const { return m_kVal.CrossProduct(kVector.m_kVal); }
	FvObjVector3 ProjectOnto(const FvObjVector3& kVector) const { return m_kVal.ProjectOnto(kVector.m_kVal); }

#ifndef FV_SERVER
protected:
#endif
	FvObjVector3& operator=(const FvObjVector3& val);
	FvObjVector3& operator+=(const FvObjVector3& kVector);
	FvObjVector3& operator-=(const FvObjVector3& kVector);
	FvObjVector3& operator*=(float fScale);
	FvObjVector3& operator/=(float fScale);

	void SetZero();
	void Set(float fX, float fY, float fZ);
	void Scale(const FvObjVector3& kVector, float fScale);
	void Normalise();
	void ParallelMultiply(const FvObjVector3& kVector);
	void ParallelMultiply(const FvObjVector3& kVector0, const FvObjVector3& kVector1);
	void Lerp(const FvObjVector3& kVector0, const FvObjVector3& kVector1, float fAlpha);
	void Clamp(const FvObjVector3& kLower, const FvObjVector3& kUpper);
	void SetPitchYaw(float fPitchInRadians, float fYawInRadians);
	void CrossProduct(const FvObjVector3& kVector0, const FvObjVector3& kVector1);
	void ProjectOnto(const FvObjVector3& kVector0, const FvObjVector3& kVector1);

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS);
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	void EqualTo(const FvObjVector3& kRight);
	void NofityDataChg();
#endif

	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend class OldFvData;

protected:
	FvVector3			m_kVal;
	static FvObjVector3	ms_kOldData;
};
template<>
FV_ENTITYDEF_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjVector3& kVal);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FV_ENTITYDEF_API FvObjVector4 : public FvDataObj
{
public:
	FvObjVector4():FvDataObj() {}
	FvObjVector4(const FvVector4& v):FvDataObj(), m_kVal(v) {}
	FvObjVector4(const float x, const float y, const float z, const float w):FvDataObj(), m_kVal(x, y, z, w) {}
	FvObjVector4(const float* pfVector):FvDataObj(), m_kVal(pfVector) {}
	FvObjVector4(const FvObjVector4& src):FvDataObj(), m_kVal(src.m_kVal) {}

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_VECTOR4; }
	static FvDataTypeID DataTypeID() { return FVOBJ_VECTOR4; }
	static FvObjVector4* OldData(const FvObjVector4& kVal) { ms_kOldData.m_kVal=kVal.m_kVal; return &ms_kOldData; }

	operator FvVector4() { return m_kVal; }
	operator const FvVector4() const { return m_kVal; }
	operator const float*() const { return m_kVal; }

	float operator[](FvVector4::Coord eCoord) const { return m_kVal[eCoord]; }

	FvObjVector4 operator+() const { return m_kVal; }
	FvObjVector4 operator-() const { return -m_kVal; }

	bool operator==(const FvObjVector4& kVector) const { return m_kVal==kVector.m_kVal; }
	bool operator!=(const FvObjVector4& kVector) const { return m_kVal!=kVector.m_kVal; }

	float Length() const { return m_kVal.Length(); }
	float LengthSquared() const { return m_kVal.LengthSquared(); }
	float DotProduct(const FvObjVector4& kVector) const { return m_kVal.DotProduct(kVector.m_kVal); }
	FvOutcode CalculateOutcode() const { return m_kVal.CalculateOutcode(); }
	FvObjVector4 UnitVector() const { return m_kVal.UnitVector(); }

#ifndef FV_SERVER
protected:
#endif
	FvObjVector4& operator=(const FvObjVector4& val);
	FvObjVector4& operator+=(const FvObjVector4& kVector);
	FvObjVector4& operator-=(const FvObjVector4& kVector);
	FvObjVector4& operator*=(float fScale);
	FvObjVector4& operator/=(float fScale);

	void SetZero();
	void Set(float fX, float fY, float fZ, float fW);
	void Scale(const FvObjVector4& kVector, float fScale);
	void Scale(float fScale);
	void Normalise();
	void ParallelMultiply(const FvObjVector4& kVector);
	void ParallelMultiply(const FvObjVector4& kVector0, const FvObjVector4& kVector1);
	void Lerp(const FvObjVector4& kVector0, const FvObjVector4& kVector1, float fAlpha);
	void Clamp(const FvObjVector4& kLower, const FvObjVector4& kUpper);

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS);
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	void EqualTo(const FvObjVector4& kRight);
	void NofityDataChg();
#endif

	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend class OldFvData;

protected:
	FvVector4			m_kVal;
	static FvObjVector4	ms_kOldData;
};
template<>
FV_ENTITYDEF_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjVector4& kVal);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace FvXMLData
{
	template<>
	FV_ENTITYDEF_API void Read(FvXMLSectionPtr spSection, FvObjBlob& kVal);
	template<>
	FV_ENTITYDEF_API void Write(FvXMLSectionPtr spSection, const FvObjBlob& kVal);
}

class FV_ENTITYDEF_API FvObjBlob : public FvDataObj, public FvBinaryOStream, public FvBinaryIStream
{
public:
	FvObjBlob(int iSize=DEFSIZE);
	FvObjBlob(const FvString& kSrc);
	FvObjBlob(const FvObjBlob& kSrc);
	virtual ~FvObjBlob();

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_BLOB; }
	static FvDataTypeID DataTypeID() { return FVOBJ_BLOB; }
	static FvObjBlob* OldData(const FvObjBlob& kVal) { return NULL; }

	virtual void*		Reserve(int iBytes);
	virtual int			Size() const;

	virtual const void*	Retrieve(int iBytes);
	virtual int			RemainingLength() const;
	virtual char		Peek();

	void*				Data() const;
	void				Reset();
	void				Rewind();

	FvObjBlob& operator=(const FvString& val);
	FvObjBlob& operator=(const FvObjBlob& val);

#ifdef FV_SERVER
	void				ModifyNotify();	//! 需要调用该接口强制同步属性
#endif

	static const int DEFSIZE = 64;

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS);
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	void EqualTo(const FvObjBlob& kRight);
#endif

	template<class T> friend FvBinaryIStream& operator>>(FvBinaryIStream&, T&);
	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend void FvXMLData::Read<FvObjBlob>(FvXMLSectionPtr, FvObjBlob&);
	friend void FvXMLData::Write<FvObjBlob>(FvXMLSectionPtr, const FvObjBlob&);

protected:
	char*	m_pcBegin;
	char*	m_pcCurr;
	char*	m_pcEnd;
	char*	m_pcRead;
};
template<>
FV_ENTITYDEF_API FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjBlob& kBlob);
template<>
FV_ENTITYDEF_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjBlob& kBlob);
namespace FvDataPrint
{
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvObjBlob& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvObjBlob& kVal, FvString& kMsg);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace FvXMLData
{
	template<>
	FV_ENTITYDEF_API void Read(FvXMLSectionPtr spSection, FvObjMailBox& kVal);
	template<>
	FV_ENTITYDEF_API void Write(FvXMLSectionPtr spSection, const FvObjMailBox& kVal);
}

class FV_ENTITYDEF_API FvObjMailBox : public FvDataObj
{
public:
	FvObjMailBox();
	FvObjMailBox(const FvObjMailBox& src);

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_MAILBOX; }
	static FvDataTypeID DataTypeID() { return FVOBJ_MAILBOX; }
	static FvObjMailBox* OldData(const FvObjMailBox& kVal) { ms_kOldData.m_kMBRef=kVal.m_kMBRef; return &ms_kOldData; }

	bool	IsValid() const;

#ifdef FV_SERVER
	void	Clear();
#endif

	bool operator==(const FvObjMailBox& kMailBox) const;
	bool operator!=(const FvObjMailBox& kMailBox) const;

	FvEntityMailBoxRef::Component GetComponent() const;
	FvEntityTypeID		GetEntityType() const;
	FvEntityID			GetEntityID() const;
	const FvNetAddress&	GetAddr() const;

	const char*	c_str() const;
/**
#ifndef FV_SERVER
protected:
#endif
	FvObjMailBox& operator=(const FvObjMailBox& val);
	void	Set(FvEntityID iEntityID, FvEntityTypeID uiEntityType, const FvNetAddress& kAddr, FvEntityMailBoxRef::Component eComponent);
	void	Set(const FvEntityMailBoxRef& kMBRef);
	void	SetComponent(FvEntityMailBoxRef::Component eComponent);
	void	SetType(FvEntityTypeID uiEntityType);
	void	SetID(FvEntityID iEntityID);
	void	SetAddr(const FvNetAddress& kAddr);
**/

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS);
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	void EqualTo(const FvObjMailBox& kRight);
#endif

	template<class T> friend FvBinaryIStream& operator>>(FvBinaryIStream&, T&);
	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend void FvXMLData::Read<FvObjMailBox>(FvXMLSectionPtr, FvObjMailBox&);
	friend void FvXMLData::Write<FvObjMailBox>(FvXMLSectionPtr, const FvObjMailBox&);

protected:
	FvObjMailBox& operator=(const FvObjMailBox& val);
#ifdef FV_SERVER
	void	MailBoxChangNotify();
#endif

protected:
	FvEntityMailBoxRef	m_kMBRef;
	static char			ms_acBuf[128];
	static FvObjMailBox	ms_kOldData;
};
template<>
FV_ENTITYDEF_API FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjMailBox& kMailBox);
template<>
FV_ENTITYDEF_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjMailBox& kMailBox);
namespace FvDataPrint
{
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvObjMailBox& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvObjMailBox& kVal, FvString& kMsg);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! UDO属性的基类
struct FvUDOAttrib
{
	FvPosition3D	m_kPosition;
	FvDirection3	m_kDirection;


	FvUDOAttrib() {}
	virtual ~FvUDOAttrib() {}

	virtual FvUInt16	GetUDOType() const = 0;
	virtual	bool		DeserializeFromSection(FvXMLSectionPtr spSection) = 0;
};

namespace FvDataPrint
{
	template<>
	FV_ENTITYDEF_API void Print(const char* pcName, const FvObjUDO& kVal, FvString& kMsg);
	template<>
	FV_ENTITYDEF_API void Print(const FvObjUDO& kVal, FvString& kMsg);
}

namespace FvXMLData
{
	template<>
	FV_ENTITYDEF_API void Read(FvXMLSectionPtr spSection, FvObjUDO& kVal);
	template<>
	FV_ENTITYDEF_API void Write(FvXMLSectionPtr spSection, const FvObjUDO& kVal);
}

struct AllUDOExports;
class FV_ENTITYDEF_API FvObjUDO : public FvDataObj
{
public:
	FvObjUDO();
	FvObjUDO(const FvObjUDO& src);
	FvObjUDO(const FvString& kGuid);

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_UDO; }
	static FvDataTypeID DataTypeID() { return FVOBJ_UDO; }
	static FvObjUDO* OldData(const FvObjUDO& kVal) { ms_kOldData.m_spUDOProxy=kVal.m_spUDOProxy; return &ms_kOldData; }

	static void SetAllUDOExports(AllUDOExports* pkAllUDOExports) { ms_pkAllUDOExports=pkAllUDOExports; }

	template<class T>
	static T* UDOAttribCast(const FvObjUDO& kUDO)
	{
		if(!kUDO.IsReady() || T::UDOType()!=kUDO.GetUDOType())
			return NULL;
		return static_cast<T*>(kUDO.GetUDOAttrib());
	}

	bool		IsReady() const;
	FvUInt16	GetUDOType() const;
	FvUDOAttrib*GetUDOAttrib() const;

	bool operator==(const FvObjUDO& kUDO) const;
	bool operator!=(const FvObjUDO& kUDO) const;

#ifndef FV_SERVER
protected:
#endif
	FvObjUDO& operator=(const FvObjUDO& val);
	FvObjUDO& operator=(const FvString& kGuid);
	void Clear();

public:
	enum UDODomain
	{
		NONE = 0x0,
		BASE = 0x1,
		CELL = 0x2,
		CLIENT =0x4,
	};

	//! UDO初始化数据
	struct FvUDOInitData
	{
		FvUniqueID		kGuid;
		FvPosition3D	kPosition;
		FvDirection3	kDirection;
		FvXMLSectionPtr	spPropertiesDS;

		void	Clear() { spPropertiesDS=NULL; }
	};

	//! 
	class FvUDOProxy : public FvReferenceCount
	{
	public:
		virtual ~FvUDOProxy();

		FvUniqueID		kGuid;
		FvUDOAttrib*	pkAttrib;

		typedef std::map<FvUniqueID, FvUDOProxy*> Map;

	protected:
		FvUDOProxy(const FvString& kGuid);
		FvUDOProxy(const FvUniqueID& kGuid);
		static FvUDOProxy*	GetUDOProxy(const FvString& kGuid);
		static FvUDOProxy*	GetUDOProxy(const FvUniqueID& kGuid);
		static FvUDOProxy*	CreateUDOProxy(const FvString& kGuid);
		static FvUDOProxy*	CreateUDOProxy(const FvUniqueID& kGuid);

		friend class FvObjUDO;
		static Map ms_kUDOProxyMap;
	};
	typedef FvSmartPointer<FvUDOProxy> FvUDOProxyPtr;

protected:
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS);
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	void EqualTo(const FvObjUDO& kRight);
#endif

	template<class T> friend FvBinaryIStream& operator>>(FvBinaryIStream&, T&);
	template<class T> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&);
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	friend void FvDataPrint::Print<FvObjUDO>(const char*, const FvObjUDO&, FvString&);
	friend void FvDataPrint::Print<FvObjUDO>(const FvObjUDO&, FvString&);
	friend void FvXMLData::Read<FvObjUDO>(FvXMLSectionPtr, FvObjUDO&);
	friend void FvXMLData::Write<FvObjUDO>(FvXMLSectionPtr, const FvObjUDO&);

protected:
	friend class FvZoneUserDataObject;
	void			InitUDO(FvUInt16 uiType, FvUDOInitData& kInitData);
	void			DestroyUDO();
	static FvUInt16	NameToType(const FvString& kGuid);

	FvUDOProxyPtr	m_spUDOProxy;
	static FvObjUDO	ms_kOldData;

	static AllUDOExports* ms_pkAllUDOExports;
};
template<>
FV_ENTITYDEF_API FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjUDO& kUDO);
template<>
FV_ENTITYDEF_API FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjUDO& kUDO);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum InitStructEnum
{
	INIT_STRUCT_FROM_XML,
};

struct NameAddr
{
	char*		pName;
	FvUInt32	uiAddr;
};

struct PtIdxToID;

class FV_ENTITYDEF_API FvObjStruct : public FvDataObj
{
public:
	FvObjStruct();

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_STRUCT; }
	static FvDataTypeID DataTypeID() { return FVOBJ_STRUCT; }
	static FvObjStruct* OldData(const FvObjStruct& kVal) { return NULL; }

	//! TODO: =操作谨慎使用,理由同FvObjArray的=

	FvUInt32	Size() const;
	FvDataObj*	Get(FvUInt32 uiIdx) const;
	FvUInt32	Size(NameAddr*& pkInfo) const;
	FvDataObj*	Get(NameAddr* pkInfo, FvUInt32 uiIdx) const;

protected:
#ifdef FV_SERVER
	virtual FvDataOwner*GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID);
#endif
	virtual FvDataOwner*DataVassal(FvUInt32 uiDataID);
#ifdef FV_SERVER
	virtual	void		Attach(FvDataOwner* pkOwner, bool bSetOnce);
	virtual	void		Detach();
#endif

	virtual bool SerializeToStream(FvBinaryOStream& kOS) const;
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS);
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const;
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection);
#ifdef FV_SERVER
	template<class T>
	void ChildEqualTo(T& kChild, const T& kRight)
	{
		kChild.EqualTo(kRight);
	}
#endif

	template<class, class> friend class FvObjArray;

protected:
	FvObjStruct(const FvObjStruct&);
	FvObjStruct& operator=(const FvObjStruct&);
	virtual void GetStructAddr(FvUInt32& uiCnt, NameAddr*& pkInfo) const = 0;
	virtual bool GetPtToID(FvUInt32& uiCnt, FvUInt32*& pBits, FvUInt16*& pIdx, PtIdxToID*& pIDs) const { return false; }
	virtual bool GetIDToAddr(FvUInt32& uiCnt, FvUInt32*& pAddr) const { return false; }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class FT, class ST>
class FvObjArray : public FvDataObj
{
public:
	typedef FvIndexVector<FT, ST> Array;
	typedef typename FvIndexVector<FT, ST>::STVector STVector;
	typedef typename FvIndexVector<FT, ST>::size_type size_type;
	typedef typename FvIndexVector<FT, ST>::difference_type difference_type;
	typedef typename FvIndexVector<FT, ST>::const_iterator const_iterator;
	typedef typename FvIndexVector<FT, ST>::iterator iterator;

	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	template<class, class> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const FvObjArray&);

	FvObjArray()
	:FvDataObj()
	{
		//! 检查T为FvDataObj的子类
		FvDataObj* pkObj = static_cast<FvDataObj*>((FT*)NULL);
	}
	FvObjArray(const STVector& kSrc)
	:FvDataObj(), m_kArray(kSrc)
	{
	}
	FvObjArray(const FvObjArray& kSrc)
	:FvDataObj(), m_kArray(kSrc.m_kArray)
	{
	}
	virtual ~FvObjArray()
	{
	}

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_ARRAY; }
	static FvDataTypeID DataTypeID() { return FVOBJ_ARRAY; }
	static FvObjArray* OldData(const FvObjArray& kVal) { return NULL; }

	operator STVector() const
	{
		return m_kArray;
	}

	bool empty() const				{ return m_kArray.empty(); }
	size_type size() const			{ return m_kArray.size(); }
	size_type capacity() const		{ return m_kArray.capacity(); }
	size_type max_size() const		{ return m_kArray.max_size(); }

	iterator begin()				{ return m_kArray.begin(); }
	const_iterator begin() const	{ return m_kArray.begin(); }
	iterator end()					{ return m_kArray.end(); }
	const_iterator end() const		{ return m_kArray.end(); }

	FT& front()						{ return m_kArray.front(); }
	const FT& front() const			{ return m_kArray.front(); }
	FT& back()						{ return m_kArray.back(); }
	const FT& back() const			{ return m_kArray.back(); }

	void reserve(size_type uiCount)	{ m_kArray.reserve(uiCount); }

	const FT& at(size_type uiPos) const { return m_kArray[uiPos]; }
	FT& at(size_type uiPos)			{ return m_kArray[uiPos]; }

	const FT& operator[](size_type uiPos) const { return m_kArray[uiPos]; }
	FT& operator[](size_type uiPos)	{ return m_kArray[uiPos]; }

#ifdef FV_SERVER

	void resize(size_type uiNewsize)
	{
		size_type cnt = m_kArray.size();
		m_kArray.resize(uiNewsize);

		if(cnt < uiNewsize)
		{
			if(HasOwner())
			{
				iterator itrB = begin() + cnt;
				iterator itrE = end();
				for(; itrB!=itrE; ++itrB)
					itrB->Attach(this, true);

				FvDataOwner* pkRoot(NULL);
				if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
				{
					FvUInt8 uiFlg(0);//! 按默认值,size增长
					ms_kDataStream << FvUInt32(uiNewsize) << uiFlg;
					pkRoot->NotifyDataChanged();
				}
			}
		}
		else if(uiNewsize < cnt)
		{
			FvDataOwner* pkRoot(NULL);
			if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
			{
				FvUInt8 uiFlg(1);//! size减小
				ms_kDataStream << FvUInt32(uiNewsize) << uiFlg;
				pkRoot->NotifyDataChanged();
			}
		}
	}

	void resize(size_type uiNewsize, const ST& kVal)
	{
		size_type cnt = m_kArray.size();
		m_kArray.resize(uiNewsize, kVal);

		if(cnt < uiNewsize)
		{
			if(HasOwner())
			{
				iterator itrB = begin() + cnt;
				iterator itrE = end();
				for(; itrB!=itrE; ++itrB)
					itrB->Attach(this, true);

				FvDataOwner* pkRoot(NULL);
				if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
				{
					FvUInt8 uiFlg(2);//! 按给定值,size增长
					ms_kDataStream << FvUInt32(uiNewsize) << uiFlg << kVal;
					pkRoot->NotifyDataChanged();
				}
			}
		}
		else if(uiNewsize < cnt)
		{
			FvDataOwner* pkRoot(NULL);
			if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
			{
				FvUInt8 uiFlg(1);//! size减小
				ms_kDataStream << FvUInt32(uiNewsize) << uiFlg;
				pkRoot->NotifyDataChanged();
			}
		}
	}

	void resize(size_type uiNewsize, const FT& kVal)
	{
		size_type cnt = m_kArray.size();
		m_kArray.resize(uiNewsize, kVal);

		if(cnt < uiNewsize)
		{
			if(HasOwner())
			{
				iterator itrB = begin() + cnt;
				iterator itrE = end();
				for(; itrB!=itrE; ++itrB)
					itrB->Attach(this, true);

				FvDataOwner* pkRoot(NULL);
				if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
				{
					FvUInt8 uiFlg(2);//! 按给定值,size增长
					ms_kDataStream << FvUInt32(uiNewsize) << uiFlg << kVal;
					pkRoot->NotifyDataChanged();
				}
			}
		}
		else if(uiNewsize < cnt)
		{
			FvDataOwner* pkRoot(NULL);
			if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
			{
				FvUInt8 uiFlg(1);//! size减小
				ms_kDataStream << FvUInt32(uiNewsize) << uiFlg;
				pkRoot->NotifyDataChanged();
			}
		}
	}

	void assign(size_type uiCount, const ST& kVal)
	{
		resize(uiCount, kVal);
	}

	void assign(size_type uiCount, const FT& kVal)
	{
		resize(uiCount, kVal);
	}

	void clear()
	{
		m_kArray.clear();

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_CLR))
		{
			pkRoot->NotifyDataChanged();
		}
	}

	iterator erase(const_iterator kWhere)
	{
		FvUInt32 uiIdx = kWhere - begin();
		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_DEL))
		{
			ms_kDataStream << uiIdx;
			pkRoot->NotifyDataChanged();
		}

		return m_kArray.erase(kWhere);
	}

	void erase(size_type uiIndex)
	{
		m_kArray.erase(uiIndex);

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_DEL))
		{
			ms_kDataStream << uiIndex;
			pkRoot->NotifyDataChanged();
		}
	}

	void erase_value(FT*& pkVal)
	{
		FvUInt32 uiIdx = m_kArray.erase_value(pkVal);
		pkVal = NULL;

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_DEL))
		{
			ms_kDataStream << uiIdx;
			pkRoot->NotifyDataChanged();
		}
	}

	void pop_back()
	{
		m_kArray.pop_back();

		FvUInt32 uiIdx = m_kArray.size();
		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_DEL))
		{
			ms_kDataStream << uiIdx;
			pkRoot->NotifyDataChanged();
		}
	}

	void push_back(const ST& kVal)
	{
		m_kArray.push_back(kVal);
		if(HasOwner())
			m_kArray.back().Attach(this, true);

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_ADD))
		{
			ms_kDataStream << kVal;
			pkRoot->NotifyDataChanged();
		}
	}

	void push_back(const FT& kVal)
	{
		m_kArray.push_back(kVal);
		if(HasOwner())
			m_kArray.back().Attach(this, true);

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_ADD))
		{
			ms_kDataStream << kVal;
			pkRoot->NotifyDataChanged();
		}
	}

	FvObjArray& operator=(const STVector& kRight)
	{
		//! TODO: 输出警告,会导致整个Array更新,谨慎使用
		size_type sum = kRight.size();
		size_type cnt = m_kArray.size();
		size_type minSize = min(cnt, sum);

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		for(size_type idx(0); itrB!=itrE; ++itrB, ++idx)
		{
			itrB->EqualTo(kRight[idx]);
		}

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

			if(HasOwner())
			{
				for(size_type i=cnt; i<sum; ++i)
				{
					m_kArray.push_back(kRight[i]);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(size_type i=cnt; i<sum; ++i)
				{
					m_kArray.push_back(kRight[i]);
				}
			}
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
				m_kArray.pop_back();
		}

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
		{
			FvUInt8 uiFlg(3);//! 全部数据
			ms_kDataStream << FvUInt32(m_kArray.size()) << uiFlg;
			itrB = begin();
			itrE = end();
			for(; itrB!=itrE; ++itrB)
				ms_kDataStream << *itrB;
			pkRoot->NotifyDataChanged();
		}

		return *this;
	}

	FvObjArray& operator=(const FvObjArray& kRight)
	{
		//! TODO: 输出警告,会导致整个Array更新,谨慎使用
		size_type sum = kRight.size();
		size_type cnt = m_kArray.size();
		size_type minSize = min(cnt, sum);

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		const_iterator itrB1 = kRight.begin();
		for(; itrB!=itrE; ++itrB, ++itrB1)
		{
			itrB->EqualTo(*itrB1);
		}

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

			if(HasOwner())
			{
				for(size_type i=cnt; i<sum; ++i, ++itrB1)
				{
					m_kArray.push_back(*itrB1);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(size_type i=cnt; i<sum; ++i, ++itrB1)
				{
					m_kArray.push_back(*itrB1);
				}
			}
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
				m_kArray.pop_back();
		}

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
		{
			FvUInt8 uiFlg(3);//! 全部数据
			ms_kDataStream << FvUInt32(m_kArray.size()) << uiFlg;
			itrB = begin();
			itrE = end();
			for(; itrB!=itrE; ++itrB)
				ms_kDataStream << *itrB;
			pkRoot->NotifyDataChanged();
		}

		return *this;
	}

	//iterator insert(const_iterator _Where, const T& _Val)
	//{
	//	return m_kArray.insert(_Where.m_kItr, new T(_Val));
	//}

	//void swap(FvObjArray& _Right)
	//{
	//	m_kArray.swap(_Right.m_kArray);
	//}
#endif

protected:

#ifdef FV_SERVER
	virtual FvDataOwner*GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID)
	{
		if(!m_pkOwner)
			return NULL;

		FV_ASSERT(!m_kArray.empty());
		FT* pkVal = static_cast<FT*>(pkVassal);
		FvUInt32 uiIndex = m_kArray.PointerToIndex(pkVal);
		FV_ASSERT(0<=uiIndex && uiIndex<(FvUInt32)m_kArray.size());
		ms_kDataPath.push_back(uiIndex);

		return m_pkOwner->GetRootData(uiOpCode, this, uiDataID+1);
	}
#endif
	virtual FvDataOwner*DataVassal(FvUInt32 uiDataID)
	{
		FV_ASSERT(uiDataID < m_kArray.size());
		return &m_kArray[uiDataID];
	}
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
	{
		if(uiOpCode == OPCD_ADD)
		{
			m_kArray.push_back();
			m_kArray.back().DeserializeFromStream(kIS);
#ifdef FV_SERVER
			FV_ASSERT(HasOwner());
			m_kArray.back().Attach(this, true);
#endif
			if(pkCB)
			{
				if(FvDataCallBack::ms_bOwnCB)
				{
					FvDataCallBack::ms_uiCBIndex = FvUInt32(m_kArray.size() -1);
					pkCB->Handle(OPCD_ADD, NULL);
				}
				else
				{
					pkCB->Handle(OPCD_MOD, NULL);
				}
			}
		}
		else if(uiOpCode == OPCD_DEL)
		{
			FvUInt32 uiIdx;
			kIS >> uiIdx;
			FV_ASSERT(!kIS.Error());
			FV_ASSERT(uiIdx < m_kArray.size());
			if(pkCB)
			{
				if(FvDataCallBack::ms_bOwnCB)
				{
					FvDataCallBack::ms_uiCBIndex = uiIdx;
					FT* pkDelete = m_kArray.erase_no_delete(uiIdx);
					pkCB->Handle(OPCD_DEL, NULL);
					m_kArray.DestroyNode(pkDelete);
				}
				else
				{
					FT* pkDelete = m_kArray.erase_no_delete(uiIdx);
					pkCB->Handle(OPCD_MOD, NULL);
					m_kArray.DestroyNode(pkDelete);
				}
			}
			else
			{
				m_kArray.erase(m_kArray.begin() + uiIdx);
			}
		}
		else if(uiOpCode == OPCD_CLR)
		{
			if(pkCB)
			{
				if(FvDataCallBack::ms_bOwnCB)
				{
					FvUInt32 uiSize(m_kArray.size());
					if(uiSize)
					{
						FT* pkDelete = NULL;
						for(; uiSize>0; --uiSize)
						{
							FvDataCallBack::ms_uiCBIndex = uiSize-1;
							pkDelete = m_kArray.erase_no_delete(uiSize-1);
							pkCB->Handle(OPCD_DEL, NULL);
							m_kArray.DestroyNode(pkDelete);
						}
					}
				}
				else
				{
					FvUInt32 uiSize(m_kArray.size());
					if(uiSize)
					{
						FT* pkDelete = NULL;
						for(; uiSize>0; --uiSize)
						{
							pkDelete = m_kArray.erase_no_delete(uiSize-1);
							pkCB->Handle(OPCD_MOD, NULL);
							m_kArray.DestroyNode(pkDelete);
						}
					}
				}
			}
			else
			{
				m_kArray.clear();
			}
		}
		else
		{
			FvUInt32 uiSize;
			FvUInt8 uiFlg;
			kIS >> uiSize >> uiFlg;
			FV_ASSERT(!kIS.Error());

			if(uiFlg == 0)//! 按默认值,size增长
			{
				size_type sum = uiSize;
				size_type cnt = m_kArray.size();
				FV_ASSERT(cnt < sum);

				m_kArray.reserve(sum);

#ifdef FV_SERVER
				FV_ASSERT(HasOwner());
#endif

				if(pkCB)
				{
					if(FvDataCallBack::ms_bOwnCB)
					{
						FvUInt32 idx(cnt);
						for(; idx<sum; ++idx)
						{
							m_kArray.push_back();
#ifdef FV_SERVER
							m_kArray[idx].Attach(this, true);
#endif
							FvDataCallBack::ms_uiCBIndex = idx;
							pkCB->Handle(OPCD_ADD, NULL);
						}
					}
					else
					{
						FvUInt32 idx(cnt);
						for(; idx<sum; ++idx)
						{
							m_kArray.push_back();
#ifdef FV_SERVER
							m_kArray[idx].Attach(this, true);
#endif
						}
						pkCB->Handle(OPCD_MOD, NULL);
					}
				}
				else
				{
					FvUInt32 idx(cnt);
					for(; idx<sum; ++idx)
					{
						m_kArray.push_back();
#ifdef FV_SERVER
						m_kArray[idx].Attach(this, true);
#endif
					}
				}
			}
			else if(uiFlg == 1)//! size减小
			{
				size_type sum = uiSize;
				size_type cnt = m_kArray.size();
				FV_ASSERT(sum < cnt);

				if(pkCB)
				{
					if(FvDataCallBack::ms_bOwnCB)
					{
						FvInt32 isum(sum);
						FT* pkDelete = NULL;
						for(FvInt32 idx(cnt-1); isum<=idx; --idx)
						{
							FvDataCallBack::ms_uiCBIndex = FvUInt32(idx);
							pkDelete = m_kArray.erase_no_delete(FvUInt32(idx));
							pkCB->Handle(OPCD_DEL, NULL);
							m_kArray.DestroyNode(pkDelete);
						}
					}
					else
					{
						FvInt32 isum(sum);
						FT* pkDelete = NULL;
						for(FvInt32 idx(cnt-1); isum<=idx; --idx)
						{
							pkDelete = m_kArray.erase_no_delete(FvUInt32(idx));
							pkCB->Handle(OPCD_MOD, NULL);
							m_kArray.DestroyNode(pkDelete);
						}
					}
				}
				else
				{
					m_kArray.resize(sum);
				}
			}
			else if(uiFlg == 2)//! 按给定值,size增长
			{
				size_type sum = uiSize;
				size_type cnt = m_kArray.size();
				FV_ASSERT(cnt < sum);

				FT kVal;
				kVal.DeserializeFromStream(kIS);

				m_kArray.reserve(sum);

#ifdef FV_SERVER
				FV_ASSERT(HasOwner());
#endif

				if(pkCB)
				{
					if(FvDataCallBack::ms_bOwnCB)
					{
						FvUInt32 idx(cnt);
						for(; idx<sum; ++idx)
						{
							m_kArray.push_back(kVal);
#ifdef FV_SERVER
							m_kArray[idx].Attach(this, true);
#endif
							FvDataCallBack::ms_uiCBIndex = idx;
							pkCB->Handle(OPCD_ADD, NULL);
						}
					}
					else
					{
						FvUInt32 idx(cnt);
						for(; idx<sum; ++idx)
						{
							m_kArray.push_back(kVal);
#ifdef FV_SERVER
							m_kArray[idx].Attach(this, true);
#endif
						}
						pkCB->Handle(OPCD_MOD, NULL);
					}
				}
				else
				{
					FvUInt32 idx(cnt);
					for(; idx<sum; ++idx)
					{
						m_kArray.push_back(kVal);
#ifdef FV_SERVER
						m_kArray[idx].Attach(this, true);
#endif
					}
				}
			}
			else//! 全部值
			{
				FV_ASSERT(uiFlg == 3);

				size_type sum = uiSize;
				size_type cnt = m_kArray.size();
				size_type minSize = min(sum, cnt);

				if(pkCB)
				{
					if(FvDataCallBack::ms_bOwnCB)
					{
						iterator itrB = begin();
						iterator itrE = begin() + minSize;
						FvUInt32 idx(0);
						for(; itrB!=itrE; ++itrB, ++idx)
						{
							FvDataObj* pkOld = FT::OldData(*itrB);
							itrB->DeserializeFromStream(kIS);
							FvDataCallBack::ms_uiCBIndex = idx;
							pkCB->Handle(OPCD_MOD, pkOld);
						}
					}
					else
					{
						iterator itrB = begin();
						iterator itrE = begin() + minSize;
						FvUInt32 idx(0);
						for(; itrB!=itrE; ++itrB, ++idx)
						{
							FvDataObj* pkOld = FT::OldData(*itrB);
							itrB->DeserializeFromStream(kIS);
							pkCB->Handle(OPCD_MOD, pkOld);
						}
					}
				}
				else
				{
					iterator itrB = begin();
					iterator itrE = begin() + minSize;
					for(; itrB!=itrE; ++itrB)
					{
						itrB->DeserializeFromStream(kIS);
					}
				}

				if(cnt < sum)
				{
					m_kArray.reserve(sum);

					if(pkCB)
					{
						if(FvDataCallBack::ms_bOwnCB)
						{
							for(size_type i=cnt; i<sum; ++i)
							{
								m_kArray.push_back();
								m_kArray.back().DeserializeFromStream(kIS);
#ifdef FV_SERVER
								FV_ASSERT(HasOwner());
								m_kArray.back().Attach(this, true);
#endif
								FvDataCallBack::ms_uiCBIndex = FvUInt32(i);
								pkCB->Handle(OPCD_ADD, NULL);
							}
						}
						else
						{
							for(size_type i=cnt; i<sum; ++i)
							{
								m_kArray.push_back();
								m_kArray.back().DeserializeFromStream(kIS);
#ifdef FV_SERVER
								FV_ASSERT(HasOwner());
								m_kArray.back().Attach(this, true);
#endif
							}
							pkCB->Handle(OPCD_MOD, NULL);
						}
					}
					else
					{
						for(size_type i=cnt; i<sum; ++i)
						{
							m_kArray.push_back();
							m_kArray.back().DeserializeFromStream(kIS);
#ifdef FV_SERVER
							FV_ASSERT(HasOwner());
							m_kArray.back().Attach(this, true);
#endif
						}
					}
				}
				else if(sum < cnt)
				{
					if(pkCB)
					{
						if(FvDataCallBack::ms_bOwnCB)
						{
							FvInt32 isum(sum);
							FT* pkDelete = NULL;
							for(FvInt32 idx(cnt-1); isum<=idx; --idx)
							{
								FvDataCallBack::ms_uiCBIndex = FvUInt32(idx);
								pkDelete = m_kArray.pop_back_no_delete();
								pkCB->Handle(OPCD_DEL, NULL);
								m_kArray.DestroyNode(pkDelete);
							}
						}
						else
						{
							FvInt32 isum(sum);
							FT* pkDelete = NULL;
							for(FvInt32 idx(cnt-1); isum<=idx; --idx)
							{
								pkDelete = m_kArray.pop_back_no_delete();
								pkCB->Handle(OPCD_MOD, NULL);
								m_kArray.DestroyNode(pkDelete);
							}
						}
					}
					else
					{
						for(size_type i=sum; i<cnt; ++i)
						{
							m_kArray.pop_back();
						}
					}
				}
			}
		}
	}
#ifdef FV_SERVER
	virtual	void Attach(FvDataOwner* pkOwner, bool bSetOnce)
	{
		FvDataOwner::Attach(pkOwner);

		iterator itrB = begin();
		iterator itrE = end();
		for(; itrB!=itrE; ++itrB)
			itrB->Attach(this, true);
	}
	virtual	void Detach()
	{
		FvDataOwner::Detach();

		iterator itrB = begin();
		iterator itrE = end();
		for(; itrB!=itrE; ++itrB)
			itrB->Detach();
	}
#endif
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const
	{
		FvUInt32 uiSize = (FvUInt32)m_kArray.size();
		kOS << uiSize;
		const_iterator itrB = begin();
		const_iterator itrE = end();
		for(; itrB!=itrE; ++itrB)
			itrB->SerializeToStream(kOS);
		return true;
	}
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS)
	{
		size_type sum;
		kIS >> sum;
		FV_ASSERT(!kIS.Error());
		size_type cnt = m_kArray.size();
		size_type minSize = min(sum, cnt);

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		for(; itrB!=itrE; ++itrB)
		{
			itrB->DeserializeFromStream(kIS);
		}

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

#ifdef FV_SERVER
			if(HasOwner())
			{
				for(size_type i=cnt; i<sum; ++i)
				{
					m_kArray.push_back();
					m_kArray.back().DeserializeFromStream(kIS);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(size_type i=cnt; i<sum; ++i)
				{
					m_kArray.push_back();
					m_kArray.back().DeserializeFromStream(kIS);
				}
			}
#else
			for(size_type i=cnt; i<sum; ++i)
			{
				m_kArray.push_back();
				m_kArray.back().DeserializeFromStream(kIS);
			}
#endif
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
			{
				m_kArray.pop_back();
			}
		}

		return true;
	}
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const
	{
		const_iterator itrB = begin();
		const_iterator itrE = end();
		for(; itrB!=itrE; ++itrB)
		{
			FvXMLSectionPtr spItem = spSection->NewSection("item");
			itrB->SerializeToSection(spItem);
		}
		return true;
	}
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection)
	{
		std::vector<FvXMLSectionPtr> kItems;
		spSection->OpenSections("item", kItems);

		size_type sum = kItems.size();
		size_type cnt = m_kArray.size();
		size_type minSize = min(sum, cnt);

		std::vector<FvXMLSectionPtr>::iterator itrItem = kItems.begin();

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		for(; itrB!=itrE; ++itrB, ++itrItem)
		{
			if((*itrItem)->GetType() != TiXmlNode::ELEMENT)
				continue;
			itrB->DeserializeFromSection(*itrItem);
		}

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

#ifdef FV_SERVER
			if(HasOwner())
			{
				for(size_type i=cnt; i<sum; ++i, ++itrItem)
				{
					m_kArray.push_back();
					m_kArray.back().DeserializeFromSection(*itrItem);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(size_type i=cnt; i<sum; ++i, ++itrItem)
				{
					m_kArray.push_back();
					m_kArray.back().DeserializeFromSection(*itrItem);
				}
			}
#else
			for(size_type i=cnt; i<sum; ++i, ++itrItem)
			{
				m_kArray.push_back();
				m_kArray.back().DeserializeFromSection(*itrItem);
			}
#endif
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
			{
				m_kArray.pop_back();
			}
		}

		return true;
	}
#ifdef FV_SERVER
	void EqualTo(const FvObjArray& kRight)
	{
		size_type sum = kRight.size();
		size_type cnt = m_kArray.size();

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

			const_iterator itrB = kRight.begin() + cnt;
			const_iterator itrE = kRight.end();

			if(HasOwner())
			{
				for(; itrB!=itrE; ++itrB)
				{
					m_kArray.push_back(*itrB);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(; itrB!=itrE; ++itrB)
				{
					m_kArray.push_back(*itrB);
				}
			}
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
				m_kArray.pop_back();
		}

		size_type minSize = min(cnt, sum);

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		const_iterator itrB1 = kRight.begin();
		for(; itrB!=itrE; ++itrB, ++itrB1)
		{
			itrB->EqualTo(*itrB1);
		}
	}
#endif

protected:
	Array	m_kArray;
};


//! FT和ST为同一类型特化
template<class FT>
class FvObjArray<FT, FT> : public FvDataObj
{
public:
	typedef FvIndexVector<FT, FT> Array;
	typedef typename FvIndexVector<FT, FT>::FTVector FTVector;
	typedef typename FvIndexVector<FT, FT>::size_type size_type;
	typedef typename FvIndexVector<FT, FT>::difference_type difference_type;
	typedef typename FvIndexVector<FT, FT>::const_iterator const_iterator;
	typedef typename FvIndexVector<FT, FT>::iterator iterator;

	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	template<class, class> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const FvObjArray&);

	FvObjArray()
	:FvDataObj()
	{
		//! 检查T为FvDataObj的子类
		FvDataObj* pkObj = static_cast<FvDataObj*>((FT*)NULL);
	}
	FvObjArray(const FTVector& kSrc)
	:FvDataObj(), m_kArray(kSrc)
	{
	}
	FvObjArray(const FvObjArray& kSrc)
	:FvDataObj(), m_kArray(kSrc.m_kArray)
	{
	}
	virtual ~FvObjArray()
	{
	}

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_ARRAY; }
	static FvDataTypeID DataTypeID() { return FVOBJ_ARRAY; }
	static FvObjArray* OldData(const FvObjArray& kVal) { return NULL; }

	operator FTVector() const
	{
		return m_kArray;
	}

	bool empty() const				{ return m_kArray.empty(); }
	size_type size() const			{ return m_kArray.size(); }
	size_type capacity() const		{ return m_kArray.capacity(); }
	size_type max_size() const		{ return m_kArray.max_size(); }

	iterator begin()				{ return m_kArray.begin(); }
	const_iterator begin() const	{ return m_kArray.begin(); }
	iterator end()					{ return m_kArray.end(); }
	const_iterator end() const		{ return m_kArray.end(); }

	FT& front()						{ return m_kArray.front(); }
	const FT& front() const			{ return m_kArray.front(); }
	FT& back()						{ return m_kArray.back(); }
	const FT& back() const			{ return m_kArray.back(); }

	void reserve(size_type uiCount)	{ m_kArray.reserve(uiCount); }

	const FT& at(size_type uiPos) const { return m_kArray[uiPos]; }
	FT& at(size_type uiPos)			{ return m_kArray[uiPos]; }

	const FT& operator[](size_type uiPos) const { return m_kArray[uiPos]; }
	FT& operator[](size_type uiPos)	{ return m_kArray[uiPos]; }

#ifdef FV_SERVER

	void resize(size_type uiNewsize)
	{
		size_type cnt = m_kArray.size();
		m_kArray.resize(uiNewsize);

		if(cnt < uiNewsize)
		{
			if(HasOwner())
			{
				iterator itrB = begin() + cnt;
				iterator itrE = end();
				for(; itrB!=itrE; ++itrB)
					itrB->Attach(this, true);

				FvDataOwner* pkRoot(NULL);
				if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
				{
					FvUInt8 uiFlg(0);//! 按默认值,size增长
					ms_kDataStream << FvUInt32(uiNewsize) << uiFlg;
					pkRoot->NotifyDataChanged();
				}
			}
		}
		else if(uiNewsize < cnt)
		{
			FvDataOwner* pkRoot(NULL);
			if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
			{
				FvUInt8 uiFlg(1);//! size减小
				ms_kDataStream << FvUInt32(uiNewsize) << uiFlg;
				pkRoot->NotifyDataChanged();
			}
		}
	}

	void resize(size_type uiNewsize, const FT& kVal)
	{
		size_type cnt = m_kArray.size();
		m_kArray.resize(uiNewsize, kVal);

		if(cnt < uiNewsize)
		{
			if(HasOwner())
			{
				iterator itrB = begin() + cnt;
				iterator itrE = end();
				for(; itrB!=itrE; ++itrB)
					itrB->Attach(this, true);

				FvDataOwner* pkRoot(NULL);
				if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
				{
					FvUInt8 uiFlg(2);//! 按给定值,size增长
					ms_kDataStream << FvUInt32(uiNewsize) << uiFlg << kVal;
					pkRoot->NotifyDataChanged();
				}
			}
		}
		else if(uiNewsize < cnt)
		{
			FvDataOwner* pkRoot(NULL);
			if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
			{
				FvUInt8 uiFlg(1);//! size减小
				ms_kDataStream << FvUInt32(uiNewsize) << uiFlg;
				pkRoot->NotifyDataChanged();
			}
		}
	}

	void assign(size_type uiCount, const FT& kVal)
	{
		resize(uiCount, kVal);
	}

	void clear()
	{
		m_kArray.clear();

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_CLR))
		{
			pkRoot->NotifyDataChanged();
		}
	}

	iterator erase(const_iterator kWhere)
	{
		FvUInt32 uiIdx = kWhere - begin();
		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_DEL))
		{
			ms_kDataStream << uiIdx;
			pkRoot->NotifyDataChanged();
		}

		return m_kArray.erase(kWhere);
	}

	void erase(size_type uiIndex)
	{
		m_kArray.erase(uiIndex);

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_DEL))
		{
			ms_kDataStream << uiIndex;
			pkRoot->NotifyDataChanged();
		}
	}

	void erase_value(FT*& pkVal)
	{
		FvUInt32 uiIdx = m_kArray.erase_value(pkVal);
		pkVal = NULL;

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_DEL))
		{
			ms_kDataStream << uiIdx;
			pkRoot->NotifyDataChanged();
		}
	}

	void pop_back()
	{
		m_kArray.pop_back();

		FvUInt32 uiIdx = m_kArray.size();
		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_DEL))
		{
			ms_kDataStream << uiIdx;
			pkRoot->NotifyDataChanged();
		}
	}

	void push_back(const FT& kVal)
	{
		m_kArray.push_back(kVal);
		if(HasOwner())
			m_kArray.back().Attach(this, true);

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_ADD))
		{
			ms_kDataStream << kVal;
			pkRoot->NotifyDataChanged();
		}
	}

	FvObjArray& operator=(const FvObjArray& kRight)
	{
		//! TODO: 输出警告,会导致整个Array更新,谨慎使用
		size_type sum = kRight.size();
		size_type cnt = m_kArray.size();
		size_type minSize = min(cnt, sum);

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		const_iterator itrB1 = kRight.begin();
		for(; itrB!=itrE; ++itrB, ++itrB1)
		{
			itrB->EqualTo(*itrB1);
		}

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

			if(HasOwner())
			{
				for(size_type i=cnt; i<sum; ++i, ++itrB1)
				{
					m_kArray.push_back(*itrB1);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(size_type i=cnt; i<sum; ++i, ++itrB1)
				{
					m_kArray.push_back(*itrB1);
				}
			}
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
				m_kArray.pop_back();
		}

		FvDataOwner* pkRoot(NULL);
		if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
		{
			FvUInt8 uiFlg(3);//! 全部数据
			ms_kDataStream << FvUInt32(m_kArray.size()) << uiFlg;
			itrB = begin();
			itrE = end();
			for(; itrB!=itrE; ++itrB)
				ms_kDataStream << *itrB;
			pkRoot->NotifyDataChanged();
		}

		return *this;
	}

	//iterator insert(const_iterator _Where, const T& _Val)
	//{
	//	return m_kArray.insert(_Where.m_kItr, new T(_Val));
	//}

	//void swap(FvObjArray& _Right)
	//{
	//	m_kArray.swap(_Right.m_kArray);
	//}
#endif

protected:

#ifdef FV_SERVER
	virtual FvDataOwner*GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID)
	{
		if(!m_pkOwner)
			return NULL;

		FV_ASSERT(!m_kArray.empty());
		FT* pkVal = static_cast<FT*>(pkVassal);
		FvUInt32 uiIndex = m_kArray.PointerToIndex(pkVal);
		FV_ASSERT(0<=uiIndex && uiIndex<(FvUInt32)m_kArray.size());
		ms_kDataPath.push_back(uiIndex);

		return m_pkOwner->GetRootData(uiOpCode, this, uiDataID+1);
	}
#endif
	virtual FvDataOwner*DataVassal(FvUInt32 uiDataID)
	{
		FV_ASSERT(uiDataID < m_kArray.size());
		return &m_kArray[uiDataID];
	}
	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
	{
		if(uiOpCode == OPCD_ADD)
		{
			m_kArray.push_back();
			m_kArray.back().DeserializeFromStream(kIS);
#ifdef FV_SERVER
			FV_ASSERT(HasOwner());
			m_kArray.back().Attach(this, true);
#endif
			if(pkCB)
			{
				if(FvDataCallBack::ms_bOwnCB)
				{
					FvDataCallBack::ms_uiCBIndex = FvUInt32(m_kArray.size() -1);
					pkCB->Handle(OPCD_ADD, NULL);
				}
				else
				{
					pkCB->Handle(OPCD_MOD, NULL);
				}
			}
		}
		else if(uiOpCode == OPCD_DEL)
		{
			FvUInt32 uiIdx;
			kIS >> uiIdx;
			FV_ASSERT(!kIS.Error());
			FV_ASSERT(uiIdx < m_kArray.size());
			if(pkCB)
			{
				if(FvDataCallBack::ms_bOwnCB)
				{
					FvDataCallBack::ms_uiCBIndex = uiIdx;
					FT* pkDelete = m_kArray.erase_no_delete(uiIdx);
					pkCB->Handle(OPCD_DEL, NULL);
					m_kArray.DestroyNode(pkDelete);
				}
				else
				{
					FT* pkDelete = m_kArray.erase_no_delete(uiIdx);
					pkCB->Handle(OPCD_MOD, NULL);
					m_kArray.DestroyNode(pkDelete);
				}
			}
			else
			{
				m_kArray.erase(m_kArray.begin() + uiIdx);
			}
		}
		else if(uiOpCode == OPCD_CLR)
		{
			if(pkCB)
			{
				if(FvDataCallBack::ms_bOwnCB)
				{
					FvUInt32 uiSize(m_kArray.size());
					if(uiSize)
					{
						FT* pkDelete = NULL;
						for(; uiSize>0; --uiSize)
						{
							FvDataCallBack::ms_uiCBIndex = uiSize-1;
							pkDelete = m_kArray.erase_no_delete(uiSize-1);
							pkCB->Handle(OPCD_DEL, NULL);
							m_kArray.DestroyNode(pkDelete);
						}
					}
				}
				else
				{
					FvUInt32 uiSize(m_kArray.size());
					if(uiSize)
					{
						FT* pkDelete = NULL;
						for(; uiSize>0; --uiSize)
						{
							pkDelete = m_kArray.erase_no_delete(uiSize-1);
							pkCB->Handle(OPCD_MOD, NULL);
							m_kArray.DestroyNode(pkDelete);
						}
					}
				}
			}
			else
			{
				m_kArray.clear();
			}
		}
		else
		{
			FvUInt32 uiSize;
			FvUInt8 uiFlg;
			kIS >> uiSize >> uiFlg;
			FV_ASSERT(!kIS.Error());

			if(uiFlg == 0)//! 按默认值,size增长
			{
				size_type sum = uiSize;
				size_type cnt = m_kArray.size();
				FV_ASSERT(cnt < sum);

				m_kArray.reserve(sum);

#ifdef FV_SERVER
				FV_ASSERT(HasOwner());
#endif

				if(pkCB)
				{
					if(FvDataCallBack::ms_bOwnCB)
					{
						FvUInt32 idx(cnt);
						for(; idx<sum; ++idx)
						{
							m_kArray.push_back();
#ifdef FV_SERVER
							m_kArray[idx].Attach(this, true);
#endif
							FvDataCallBack::ms_uiCBIndex = idx;
							pkCB->Handle(OPCD_ADD, NULL);
						}
					}
					else
					{
						FvUInt32 idx(cnt);
						for(; idx<sum; ++idx)
						{
							m_kArray.push_back();
#ifdef FV_SERVER
							m_kArray[idx].Attach(this, true);
#endif
						}
						pkCB->Handle(OPCD_MOD, NULL);
					}
				}
				else
				{
					FvUInt32 idx(cnt);
					for(; idx<sum; ++idx)
					{
						m_kArray.push_back();
#ifdef FV_SERVER
						m_kArray[idx].Attach(this, true);
#endif
					}
				}
			}
			else if(uiFlg == 1)//! size减小
			{
				size_type sum = uiSize;
				size_type cnt = m_kArray.size();
				FV_ASSERT(sum < cnt);

				if(pkCB)
				{
					if(FvDataCallBack::ms_bOwnCB)
					{
						FvInt32 isum(sum);
						FT* pkDelete = NULL;
						for(FvInt32 idx(cnt-1); isum<=idx; --idx)
						{
							FvDataCallBack::ms_uiCBIndex = FvUInt32(idx);
							pkDelete = m_kArray.erase_no_delete(FvUInt32(idx));
							pkCB->Handle(OPCD_DEL, NULL);
							m_kArray.DestroyNode(pkDelete);
						}
					}
					else
					{
						FvInt32 isum(sum);
						FT* pkDelete = NULL;
						for(FvInt32 idx(cnt-1); isum<=idx; --idx)
						{
							pkDelete = m_kArray.erase_no_delete(FvUInt32(idx));
							pkCB->Handle(OPCD_MOD, NULL);
							m_kArray.DestroyNode(pkDelete);
						}
					}
				}
				else
				{
					m_kArray.resize(sum);
				}
			}
			else if(uiFlg == 2)//! 按给定值,size增长
			{
				size_type sum = uiSize;
				size_type cnt = m_kArray.size();
				FV_ASSERT(cnt < sum);

				FT kVal;
				kVal.DeserializeFromStream(kIS);

				m_kArray.reserve(sum);

#ifdef FV_SERVER
				FV_ASSERT(HasOwner());
#endif

				if(pkCB)
				{
					if(FvDataCallBack::ms_bOwnCB)
					{
						FvUInt32 idx(cnt);
						for(; idx<sum; ++idx)
						{
							m_kArray.push_back(kVal);
#ifdef FV_SERVER
							m_kArray[idx].Attach(this, true);
#endif
							FvDataCallBack::ms_uiCBIndex = idx;
							pkCB->Handle(OPCD_ADD, NULL);
						}
					}
					else
					{
						FvUInt32 idx(cnt);
						for(; idx<sum; ++idx)
						{
							m_kArray.push_back(kVal);
#ifdef FV_SERVER
							m_kArray[idx].Attach(this, true);
#endif
						}
						pkCB->Handle(OPCD_MOD, NULL);
					}
				}
				else
				{
					FvUInt32 idx(cnt);
					for(; idx<sum; ++idx)
					{
						m_kArray.push_back(kVal);
#ifdef FV_SERVER
						m_kArray[idx].Attach(this, true);
#endif
					}
				}
			}
			else//! 全部值
			{
				FV_ASSERT(uiFlg == 3);

				size_type sum = uiSize;
				size_type cnt = m_kArray.size();
				size_type minSize = min(sum, cnt);

				if(pkCB)
				{
					if(FvDataCallBack::ms_bOwnCB)
					{
						iterator itrB = begin();
						iterator itrE = begin() + minSize;
						FvUInt32 idx(0);
						for(; itrB!=itrE; ++itrB, ++idx)
						{
							FvDataObj* pkOld = FT::OldData(*itrB);
							itrB->DeserializeFromStream(kIS);
							FvDataCallBack::ms_uiCBIndex = idx;
							pkCB->Handle(OPCD_MOD, pkOld);
						}
					}
					else
					{
						iterator itrB = begin();
						iterator itrE = begin() + minSize;
						FvUInt32 idx(0);
						for(; itrB!=itrE; ++itrB, ++idx)
						{
							FvDataObj* pkOld = FT::OldData(*itrB);
							itrB->DeserializeFromStream(kIS);
							pkCB->Handle(OPCD_MOD, pkOld);
						}
					}
				}
				else
				{
					iterator itrB = begin();
					iterator itrE = begin() + minSize;
					for(; itrB!=itrE; ++itrB)
					{
						itrB->DeserializeFromStream(kIS);
					}
				}

				if(cnt < sum)
				{
					m_kArray.reserve(sum);

					if(pkCB)
					{
						if(FvDataCallBack::ms_bOwnCB)
						{
							for(size_type i=cnt; i<sum; ++i)
							{
								m_kArray.push_back();
								m_kArray.back().DeserializeFromStream(kIS);
#ifdef FV_SERVER
								FV_ASSERT(HasOwner());
								m_kArray.back().Attach(this, true);
#endif
								FvDataCallBack::ms_uiCBIndex = FvUInt32(i);
								pkCB->Handle(OPCD_ADD, NULL);
							}
						}
						else
						{
							for(size_type i=cnt; i<sum; ++i)
							{
								m_kArray.push_back();
								m_kArray.back().DeserializeFromStream(kIS);
#ifdef FV_SERVER
								FV_ASSERT(HasOwner());
								m_kArray.back().Attach(this, true);
#endif
							}
							pkCB->Handle(OPCD_MOD, NULL);
						}
					}
					else
					{
						for(size_type i=cnt; i<sum; ++i)
						{
							m_kArray.push_back();
							m_kArray.back().DeserializeFromStream(kIS);
#ifdef FV_SERVER
							FV_ASSERT(HasOwner());
							m_kArray.back().Attach(this, true);
#endif
						}
					}
				}
				else if(sum < cnt)
				{
					if(pkCB)
					{
						if(FvDataCallBack::ms_bOwnCB)
						{
							FvInt32 isum(sum);
							FT* pkDelete = NULL;
							for(FvInt32 idx(cnt-1); isum<=idx; --idx)
							{
								FvDataCallBack::ms_uiCBIndex = FvUInt32(idx);
								pkDelete = m_kArray.pop_back_no_delete();
								pkCB->Handle(OPCD_DEL, NULL);
								m_kArray.DestroyNode(pkDelete);
							}
						}
						else
						{
							FvInt32 isum(sum);
							FT* pkDelete = NULL;
							for(FvInt32 idx(cnt-1); isum<=idx; --idx)
							{
								pkDelete = m_kArray.pop_back_no_delete();
								pkCB->Handle(OPCD_MOD, NULL);
								m_kArray.DestroyNode(pkDelete);
							}
						}
					}
					else
					{
						for(size_type i=sum; i<cnt; ++i)
						{
							m_kArray.pop_back();
						}
					}
				}
			}
		}
	}
#ifdef FV_SERVER
	virtual	void Attach(FvDataOwner* pkOwner, bool bSetOnce)
	{
		FvDataOwner::Attach(pkOwner);

		iterator itrB = begin();
		iterator itrE = end();
		for(; itrB!=itrE; ++itrB)
			itrB->Attach(this, true);
	}
	virtual	void Detach()
	{
		FvDataOwner::Detach();

		iterator itrB = begin();
		iterator itrE = end();
		for(; itrB!=itrE; ++itrB)
			itrB->Detach();
	}
#endif
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const
	{
		FvUInt32 uiSize = (FvUInt32)m_kArray.size();
		kOS << uiSize;
		const_iterator itrB = begin();
		const_iterator itrE = end();
		for(; itrB!=itrE; ++itrB)
			itrB->SerializeToStream(kOS);
		return true;
	}
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS)
	{
		size_type sum;
		kIS >> sum;
		FV_ASSERT(!kIS.Error());
		size_type cnt = m_kArray.size();
		size_type minSize = min(sum, cnt);

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		for(; itrB!=itrE; ++itrB)
		{
			itrB->DeserializeFromStream(kIS);
		}

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

#ifdef FV_SERVER
			if(HasOwner())
			{
				for(size_type i=cnt; i<sum; ++i)
				{
					m_kArray.push_back();
					m_kArray.back().DeserializeFromStream(kIS);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(size_type i=cnt; i<sum; ++i)
				{
					m_kArray.push_back();
					m_kArray.back().DeserializeFromStream(kIS);
				}
			}
#else
			for(size_type i=cnt; i<sum; ++i)
			{
				m_kArray.push_back();
				m_kArray.back().DeserializeFromStream(kIS);
			}
#endif
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
			{
				m_kArray.pop_back();
			}
		}

		return true;
	}
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const
	{
		const_iterator itrB = begin();
		const_iterator itrE = end();
		for(; itrB!=itrE; ++itrB)
		{
			FvXMLSectionPtr spItem = spSection->NewSection("item");
			itrB->SerializeToSection(spItem);
		}
		return true;
	}
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection)
	{
		std::vector<FvXMLSectionPtr> kItems;
		spSection->OpenSections("item", kItems);

		size_type sum = kItems.size();
		size_type cnt = m_kArray.size();
		size_type minSize = min(sum, cnt);

		std::vector<FvXMLSectionPtr>::iterator itrItem = kItems.begin();

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		for(; itrB!=itrE; ++itrB, ++itrItem)
		{
			if((*itrItem)->GetType() != TiXmlNode::ELEMENT)
				continue;
			itrB->DeserializeFromSection(*itrItem);
		}

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

#ifdef FV_SERVER
			if(HasOwner())
			{
				for(size_type i=cnt; i<sum; ++i, ++itrItem)
				{
					m_kArray.push_back();
					m_kArray.back().DeserializeFromSection(*itrItem);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(size_type i=cnt; i<sum; ++i, ++itrItem)
				{
					m_kArray.push_back();
					m_kArray.back().DeserializeFromSection(*itrItem);
				}
			}
#else
			for(size_type i=cnt; i<sum; ++i, ++itrItem)
			{
				m_kArray.push_back();
				m_kArray.back().DeserializeFromSection(*itrItem);
			}
#endif
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
			{
				m_kArray.pop_back();
			}
		}

		return true;
	}
#ifdef FV_SERVER
	void EqualTo(const FvObjArray& kRight)
	{
		size_type sum = kRight.size();
		size_type cnt = m_kArray.size();

		if(cnt < sum)
		{
			m_kArray.reserve(sum);

			const_iterator itrB = kRight.begin() + cnt;
			const_iterator itrE = kRight.end();

			if(HasOwner())
			{
				for(; itrB!=itrE; ++itrB)
				{
					m_kArray.push_back(*itrB);
					m_kArray.back().Attach(this, true);
				}
			}
			else
			{
				for(; itrB!=itrE; ++itrB)
				{
					m_kArray.push_back(*itrB);
				}
			}
		}
		else if(sum < cnt)
		{
			for(size_type i=sum; i<cnt; ++i)
				m_kArray.pop_back();
		}

		size_type minSize = min(cnt, sum);

		iterator itrB = begin();
		iterator itrE = begin() + minSize;
		const_iterator itrB1 = kRight.begin();
		for(; itrB!=itrE; ++itrB, ++itrB1)
		{
			itrB->EqualTo(*itrB1);
		}
	}
#endif

protected:
	Array	m_kArray;
};


template<class FT, class ST>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjArray<FT, ST>& kArray)
{
	kArray.SerializeToStream(kOS);
	return kOS;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class OldFvData
{
public:
	template<class T>
	static FvDataObj* Get(const T& kVal);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, FvUInt32 SIZE>
class FvObjNumFixArray : public FvDataObj
{
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	template<class, FvUInt32> friend FvBinaryOStream& operator<<(FvBinaryOStream&, const FvObjNumFixArray&);
	template<class, FvUInt32> friend FvBinaryIStream& operator>>(FvBinaryIStream&, FvObjNumFixArray&);
	template<class, FvUInt32> friend void FvXMLDataRead(FvXMLSectionPtr, FvObjNumFixArray&);
	template<class, FvUInt32> friend void FvXMLDataWrite(FvXMLSectionPtr, const FvObjNumFixArray&);

public:
	FvObjNumFixArray()
	:FvDataObj()
	{
		memset(m_aArray, 0, sizeof(m_aArray));
	}
	FvObjNumFixArray(const FvObjNumFixArray& kSrc)
	:FvDataObj()
	{
		memcpy_s(m_aArray, sizeof(m_aArray), kSrc.m_aArray, sizeof(m_aArray));
	}
	virtual ~FvObjNumFixArray()
	{
	}

	virtual FvDataTypeID GetDataTypeID() const { return FVOBJ_NUM_FIXARRAY; }
	static FvDataTypeID DataTypeID() { return FVOBJ_NUM_FIXARRAY; }
	static FvObjNumFixArray* OldData(const FvObjNumFixArray& kVal) { return NULL; }

	static FvUInt32 Size() { return FvUInt32(SIZE); }

	void InitData(FvUInt32 uiIndex, T kVal)
	{
		FV_ASSERT(uiIndex < SIZE);
#ifdef FV_SERVER
		FV_ASSERT(!HasOwner());
#endif
		if(m_aArray[uiIndex] != kVal)
		{
			m_aArray[uiIndex] = kVal;
		}
	}

	T Get(FvUInt32 uiIndex) const
	{
		FV_ASSERT(uiIndex < SIZE);
		return m_aArray[uiIndex];
	}

#ifdef FV_SERVER

	void Set(FvUInt32 uiIndex, T kVal)
	{
		FV_ASSERT(uiIndex < SIZE);
		if(m_aArray[uiIndex] != kVal)
		{
			if(HasOwner())
			{
				FvDataOwner* pkRoot(NULL);
				if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
				{
					//! TODO: 需要优化idx压缩
					ms_kDataStream << uiIndex << kVal;
					pkRoot->NotifyDataChanged();
				}
			}
			m_aArray[uiIndex] = kVal;
		}
	}

	FvObjNumFixArray& operator=(const FvObjNumFixArray& kRight)
	{
		memcpy_s(m_aArray, sizeof(m_aArray), kRight.m_aArray, sizeof(m_aArray));
		if(HasOwner())
		{
			FvDataOwner* pkRoot(NULL);
			if(pkRoot = GetRootDataForLeaf(OPCD_CLR))//! 这里用CLR表示=
			{
				ms_kDataStream.AddBlob(m_aArray, sizeof(m_aArray));
				pkRoot->NotifyDataChanged();
			}
		}
		return *this;
	}

#endif

	void Print(const char* pcName, FvString& kMsg) const
	{
		if(pcName)
		{
			FvDataPrint::Print(kMsg, "%s<", pcName);
			const char** ppkName = GetItemName();
			T* pkData = (T*)m_aArray;
			T* pkEnd = pkData + SIZE;
			FvDataPrint::Print(*ppkName, *pkData, kMsg);
			++pkData;
			++ppkName;
			for(; pkData!=pkEnd; ++pkData, ++ppkName)
			{
				kMsg.append(", ");
				FvDataPrint::Print(*ppkName, *pkData, kMsg);
			}
			kMsg.append(">");
		}
		else
		{
			kMsg.append("<");
			T* pkData = (T*)m_aArray;
			T* pkEnd = pkData + SIZE;
			FvDataPrint::Print(*pkData, kMsg);
			++pkData;
			for(; pkData!=pkEnd; ++pkData)
			{
				kMsg.append(", ");
				FvDataPrint::Print(*pkData, kMsg);
			}
			kMsg.append(">");
		}
	}

protected:

	virtual void DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
	{
		if(uiOpCode == OPCD_MOD)
		{
			FvUInt32 uiIndex;
			T kVal;
			kIS >> uiIndex >> kVal;
			FV_ASSERT(!kIS.Error());
			FV_ASSERT(uiIndex < SIZE);
			T kOldVal = m_aArray[uiIndex];
			m_aArray[uiIndex] = kVal;

			if(pkCB)
			{
				if(FvDataCallBack::ms_bOwnCB)
				{
					FvDataCallBack::ms_uiCBIndex = uiIndex;
					pkCB->Handle(OPCD_MOD, OldFvData::Get<T>(kOldVal));
				}
				else
				{
					pkCB->Handle(OPCD_MOD, NULL);
				}
			}
		}
		else
		{
			FV_ASSERT(uiOpCode == OPCD_CLR)//! 表示=
			char* pkData = (char*)kIS.Retrieve(sizeof(m_aArray));
			FV_ASSERT(!kIS.Error());
			memcpy_s(m_aArray, sizeof(m_aArray), pkData, sizeof(m_aArray));

			if(pkCB)
			{
				if(FvDataCallBack::ms_bOwnCB)
					pkCB->Handle(OPCD_CLR, NULL);
				else
					pkCB->Handle(OPCD_MOD, NULL);
			}
		}
	}
	virtual bool SerializeToStream(FvBinaryOStream& kOS) const
	{
		kOS.AddBlob(m_aArray, sizeof(m_aArray));
		return true;
	}
	virtual bool DeserializeFromStream(FvBinaryIStream& kIS)
	{
		char* pkData = (char*)kIS.Retrieve(sizeof(m_aArray));
		FV_ASSERT(!kIS.Error());
		memcpy_s(m_aArray, sizeof(m_aArray), pkData, sizeof(m_aArray));
		return true;
	}
	virtual bool SerializeToSection(FvXMLSectionPtr spSection) const
	{
		const char** ppkName = GetItemName();
		const char** ppkEnd = ppkName + SIZE;
		T* pkData = (T*)m_aArray;
		for(; ppkName!=ppkEnd; ++ppkName, ++pkData)
		{
			FvXMLSectionPtr spItem = spSection->NewSection(*ppkName);
			spItem->Set<T>(*pkData);
		}
		return true;
	}
	virtual	bool DeserializeFromSection(FvXMLSectionPtr spSection)
	{
		const char** ppkName = GetItemName();
		const char** ppkEnd = ppkName + SIZE;
		T* pkData = m_aArray;
		for(; ppkName!=ppkEnd; ++ppkName, ++pkData)
		{
			FvXMLSectionPtr spItem = spSection->OpenSection(*ppkName);
			if(spItem)
				*pkData = spItem->As<T>(*pkData);
		}
		return true;
	}
#ifdef FV_SERVER
	void EqualTo(const FvObjNumFixArray& kRight)
	{
		memcpy_s(m_aArray, sizeof(m_aArray), kRight.m_aArray, sizeof(m_aArray));
	}
#endif

	virtual const char** GetItemName() const = 0;

protected:
	T				m_aArray[SIZE];
};

template<class T, FvUInt32 SIZE>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjNumFixArray<T, SIZE>& kArray)
{
	kArray.SerializeToStream(kOS);
	return kOS;
}

template<class T, FvUInt32 SIZE>
FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjNumFixArray<T, SIZE>& kArray)
{
#ifdef FV_SERVER
	FV_ASSERT(!kArray.HasOwner() && "The NumFixArray has DataOwner!");//! 有Owner不能反流化
#endif
	kArray.DeserializeFromStream(kIS);
	return kIS;
}

namespace FvDataPrint
{
	template<class T, FvUInt32 SIZE>
	void Print(const char* pcName, const FvObjNumFixArray<T, SIZE>& kVal, FvString& kMsg)
	{
		kVal.Print(pcName, kMsg);
	}
	template<class T, FvUInt32 SIZE>
	void Print(const FvObjNumFixArray<T, SIZE>& kVal, FvString& kMsg)
	{
		kVal.Print(NULL, kMsg);
	}
}

template<class T, FvUInt32 SIZE>
void FvXMLDataRead(FvXMLSectionPtr spSection, FvObjNumFixArray<T, SIZE>& kVal)
{
#ifdef FV_SERVER
	FV_ASSERT(!kVal.HasOwner() && "The NumFixArray has DataOwner!");//! 有Owner不能反流化
#endif
	kVal.DeserializeFromSection(spSection);
}
template<class T, FvUInt32 SIZE>
void FvXMLDataWrite(FvXMLSectionPtr spSection, const FvObjNumFixArray<T, SIZE>& kVal)
{
	kVal.SerializeToSection(spSection);
}

namespace FvXMLData
{
	template<class T, FvUInt32 SIZE>
	void Read(FvXMLSectionPtr spSection, FvObjNumFixArray<T, SIZE>& kVal)
	{
		FvXMLDataRead<T, SIZE>(spSection, kVal);
	}
	template<class T, FvUInt32 SIZE>
	void Write(FvXMLSectionPtr spSection, const FvObjNumFixArray<T, SIZE>& kVal)
	{
		FvXMLDataWrite<T, SIZE>(spSection, kVal);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif//__FvDataObj_H__
