#include "FvObj.h"
#include "FvDataTypes.h"
#include "FvUserDataObject.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FvUInt64 FvObj::m_uiObjCnt[FVOBJ_RTTI_MAX] = {0};

FVOBJ_RTTI_IMPLEMENT(FvObj, FVOBJ_RTTI_OBJ)
FVOBJ_RTTI_IMPLEMENT(FvObjInt8Old, FVOBJ_RTTI_INT8)
FVOBJ_RTTI_IMPLEMENT(FvObjUInt8Old, FVOBJ_RTTI_UINT8)
FVOBJ_RTTI_IMPLEMENT(FvObjInt16Old, FVOBJ_RTTI_INT16)
FVOBJ_RTTI_IMPLEMENT(FvObjUInt16Old, FVOBJ_RTTI_UINT16)
FVOBJ_RTTI_IMPLEMENT(FvObjInt32Old, FVOBJ_RTTI_INT32)
FVOBJ_RTTI_IMPLEMENT(FvObjUInt32Old, FVOBJ_RTTI_UINT32)
FVOBJ_RTTI_IMPLEMENT(FvObjInt64Old, FVOBJ_RTTI_INT64)
FVOBJ_RTTI_IMPLEMENT(FvObjUInt64Old, FVOBJ_RTTI_UINT64)
FVOBJ_RTTI_IMPLEMENT(FvObjFloatOld, FVOBJ_RTTI_FLOAT)
FVOBJ_RTTI_IMPLEMENT(FvObjDoubleOld, FVOBJ_RTTI_DOUBLE)
FVOBJ_RTTI_IMPLEMENT(FvObjVector2Old, FVOBJ_RTTI_VECTOR2)
FVOBJ_RTTI_IMPLEMENT(FvObjVector3Old, FVOBJ_RTTI_VECTOR3)
FVOBJ_RTTI_IMPLEMENT(FvObjVector4Old, FVOBJ_RTTI_VECTOR4)
FVOBJ_RTTI_IMPLEMENT(FvObjStringOld, FVOBJ_RTTI_STRING)
FVOBJ_RTTI_IMPLEMENT(FvObjArrayOld, FVOBJ_RTTI_ARRAY)
FVOBJ_RTTI_IMPLEMENT(FvObjIntMap, FVOBJ_RTTI_INTMAP)
FVOBJ_RTTI_IMPLEMENT(FvObjStrMap, FVOBJ_RTTI_STRMAP)
FVOBJ_RTTI_IMPLEMENT(FvObjStructOld, FVOBJ_RTTI_STRUCT)
FVOBJ_RTTI_IMPLEMENT(FvObjMailBoxOld, FVOBJ_RTTI_MAILBOX)
FVOBJ_RTTI_IMPLEMENT(FvObjBlobOld, FVOBJ_RTTI_BLOB)
FVOBJ_RTTI_IMPLEMENT(FvObjBoolOld, FVOBJ_RTTI_BOOL)
FVOBJ_RTTI_IMPLEMENT(FvObjNumFixArrayOld, FVOBJ_RTTI_NUMFIXARRAY)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FvObj::ClearObjCnt()
{
	if(FVOBJ_RTTI_MAX)
	{
		memset(m_uiObjCnt, 0, sizeof(m_uiObjCnt));
	}
}

void FvObj::PropertyModifyNotify(FvObjPtr spOldVal)
{
	FV_ASSERT(m_pkOwner);
	ChangeVal oldChgVal(NULL, spOldVal);
	ChangeVal newChgVal(NULL, this);
	ChangePath path;
	m_pkOwner->PropertyChanged(FvPropertyOwnerBase::FVPROP_OP_MOD, m_uiOwnerRef, oldChgVal, newChgVal, path);
}

void FvObj::AddObjCnt(int iIndex)
{
	if(iIndex <= FVOBJ_RTTI_OBJ || iIndex >= FVOBJ_RTTI_MAX)
		return;
	++m_uiObjCnt[iIndex];
}

bool operator<(FvObjPtr spObj1, FvObjPtr spObj2)
{
	FV_ASSERT(spObj1 && spObj2 && spObj1->DataType() && spObj2->DataType());

	if (spObj1->DataType()->FvDataType::operator<( *spObj2->DataType() )) return true;
	if (spObj2->DataType()->FvDataType::operator<( *spObj1->DataType() )) return false;

#define FVOBJ_SMALL_OPERATOR(_ID, _CLASS)					\
	case _ID:												\
		{													\
			_CLASS* pObj1 = FVOBJ_CONVERT(spObj1, _CLASS);	\
			_CLASS* pObj2 = FVOBJ_CONVERT(spObj2, _CLASS);	\
			return *pObj1 < *pObj2;							\
		}													\
		break

	switch(spObj1->DataType()->TypeID())
	{
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_INT8, FvObjInt8Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_UINT8, FvObjUInt8Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_INT16, FvObjInt16Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_UINT16, FvObjUInt16Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_INT32, FvObjInt32Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_UINT32, FvObjUInt32Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_INT64, FvObjInt64Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_UINT64, FvObjUInt64Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_FLOAT, FvObjFloatOld);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_DOUBLE, FvObjDoubleOld);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_VECTOR2, FvObjVector2Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_VECTOR3, FvObjVector3Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_VECTOR4, FvObjVector4Old);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_STRING, FvObjStringOld);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_ARRAY, FvObjArrayOld);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_INTMAP, FvObjIntMap);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_STRMAP, FvObjStrMap);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_STRUCT, FvObjStructOld);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_MAILBOX, FvObjMailBoxOld);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_BLOB, FvObjBlobOld);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_USERDATAOBJ, FvUserDataObject);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_BOOL, FvObjBoolOld);
		FVOBJ_SMALL_OPERATOR(FV_DATATYPEID_NUM_FIXARRAY, FvObjNumFixArrayOld);
	default:
		FV_ASSERT(0);
		break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FVOBJ_OPERATOR_SET_IMPLEMENT(_TYPE, _RWTYPE)									\
FvOldSimpleData##_RWTYPE& FvOldSimpleData##_RWTYPE::operator=(_TYPE val)						\
{																						\
	if(m_Val == (_RWTYPE)val)															\
		return *this;																	\
	if(HasOwner())																		\
	{																					\
		FvOldSimpleData##_RWTYPE oldVal(*this);											\
		oldVal.IncRef();																\
		m_Val=(_RWTYPE)val;																\
		PropertyModifyNotify(&oldVal);													\
	}																					\
	else																				\
	{																					\
		m_Val=(_RWTYPE)val;																\
	}																					\
	return *this;																		\
}

#define FVOBJ_SIMPLEDATA_IMPLEMENT(_DATATYPE, _DATATYPE_ID, _CLASSNAME)					\
FvOldSimpleData##_DATATYPE::FvOldSimpleData##_DATATYPE(FvDataType* dt, bool bDefault)			\
:FvObj(dt)																				\
{																						\
	FV_ASSERT(dt && dt->TypeID() == _DATATYPE_ID);										\
	if(bDefault)																		\
		m_Val = FVOBJ_GETVAL(dt->pDefaultValue(), _CLASSNAME);							\
	else																				\
		m_Val = 0;																		\
}																						\
FvOldSimpleData##_DATATYPE::FvOldSimpleData##_DATATYPE(FvDataType* dt, _DATATYPE v)			\
:FvObj(dt), m_Val(v)																	\
{																						\
	FV_ASSERT(dt && dt->TypeID() == _DATATYPE_ID);										\
}																						\
bool FvOldSimpleData##_DATATYPE::operator<( const FvOldSimpleData##_DATATYPE & other ) const	\
{																						\
	return m_Val < other.m_Val;															\
}																						\
FVOBJ_OPERATOR_SET_IMPLEMENT(FvInt8, _DATATYPE)											\
FVOBJ_OPERATOR_SET_IMPLEMENT(FvUInt8, _DATATYPE)										\
FVOBJ_OPERATOR_SET_IMPLEMENT(FvInt16, _DATATYPE)										\
FVOBJ_OPERATOR_SET_IMPLEMENT(FvUInt16, _DATATYPE)										\
FVOBJ_OPERATOR_SET_IMPLEMENT(FvInt32, _DATATYPE)										\
FVOBJ_OPERATOR_SET_IMPLEMENT(FvUInt32, _DATATYPE)										\
FVOBJ_OPERATOR_SET_IMPLEMENT(FvInt64, _DATATYPE)										\
FVOBJ_OPERATOR_SET_IMPLEMENT(FvUInt64, _DATATYPE)										\
FVOBJ_OPERATOR_SET_IMPLEMENT(float, _DATATYPE)											\
FVOBJ_OPERATOR_SET_IMPLEMENT(double, _DATATYPE)


FVOBJ_SIMPLEDATA_IMPLEMENT(FvInt8, FV_DATATYPEID_INT8, FvObjInt8Old)
FVOBJ_SIMPLEDATA_IMPLEMENT(FvUInt8, FV_DATATYPEID_UINT8, FvObjUInt8Old)
FVOBJ_SIMPLEDATA_IMPLEMENT(FvInt16, FV_DATATYPEID_INT16, FvObjInt16Old)
FVOBJ_SIMPLEDATA_IMPLEMENT(FvUInt16, FV_DATATYPEID_UINT16, FvObjUInt16Old)
FVOBJ_SIMPLEDATA_IMPLEMENT(FvInt32, FV_DATATYPEID_INT32, FvObjInt32Old)
FVOBJ_SIMPLEDATA_IMPLEMENT(FvUInt32, FV_DATATYPEID_UINT32, FvObjUInt32Old)
FVOBJ_SIMPLEDATA_IMPLEMENT(FvInt64, FV_DATATYPEID_INT64, FvObjInt64Old)
FVOBJ_SIMPLEDATA_IMPLEMENT(FvUInt64, FV_DATATYPEID_UINT64, FvObjUInt64Old)
FVOBJ_SIMPLEDATA_IMPLEMENT(float, FV_DATATYPEID_FLOAT, FvObjFloatOld)
FVOBJ_SIMPLEDATA_IMPLEMENT(double, FV_DATATYPEID_DOUBLE, FvObjDoubleOld)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjVector2Old::FvObjVector2Old(FvDataType* dataType, bool bDefault)
:FvObj(dataType), m_kVal()
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR2);
	if(bDefault)
		m_kVal = (FvVector2)FVOBJ_GETVAL(dataType->pDefaultValue(), FvObjVector2Old);
}
FvObjVector2Old::FvObjVector2Old(FvDataType* dataType, const FvVector2& v)
:FvObj(dataType), m_kVal(v)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR2);
}
FvObjVector2Old::FvObjVector2Old(FvDataType* dataType, float x, float y)
:FvObj(dataType), m_kVal(x, y)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR2);
}
FvObjVector2Old::FvObjVector2Old(FvDataType* dataType, const float* pfVector)
:FvObj(dataType), m_kVal(pfVector)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR2);
}

bool FvObjVector2Old::operator<( const FvObjVector2Old & other ) const
{
	if(m_kVal.x != other.m_kVal.x)
		return m_kVal.x < other.m_kVal.x;
	else
		return m_kVal.y < other.m_kVal.y;
}

FvObjVector2Old& FvObjVector2Old::operator=(const FvVector2& val)
{
	if(m_kVal == val)
		return *this;
	if(HasOwner())
	{
		FvObjVector2Old oldVal(*this);
		oldVal.IncRef();
		m_kVal = val;
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal = val;
	}
	return *this;
}
FvObjVector2Old& FvObjVector2Old::operator=(const float* val)
{
	if(m_kVal == FvVector2(val))
		return *this;
	if(HasOwner())
	{
		FvObjVector2Old oldVal(*this);
		oldVal.IncRef();
		m_kVal = FvVector2(val);
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal = FvVector2(val);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjVector3Old::FvObjVector3Old(FvDataType* dataType, bool bDefault)
:FvObj(dataType), m_kVal()
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR3);
	if(bDefault)
		m_kVal = (FvVector3)FVOBJ_GETVAL(dataType->pDefaultValue(), FvObjVector3Old);
}
FvObjVector3Old::FvObjVector3Old(FvDataType* dataType, const FvVector3& v)
:FvObj(dataType), m_kVal(v)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR3);
}
FvObjVector3Old::FvObjVector3Old(FvDataType* dataType, float x, float y, float z)
:FvObj(dataType), m_kVal(x, y, z)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR3);
}
FvObjVector3Old::FvObjVector3Old(FvDataType* dataType, const float* pfVector)
:FvObj(dataType), m_kVal(pfVector)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR3);
}

bool FvObjVector3Old::operator<( const FvObjVector3Old & other ) const
{
	if(m_kVal.x != other.m_kVal.x)
		return m_kVal.x < other.m_kVal.x;
	else if(m_kVal.y != other.m_kVal.y)
		return m_kVal.y < other.m_kVal.y;
	else
		return m_kVal.z < other.m_kVal.z;
}
FvObjVector3Old& FvObjVector3Old::operator=(const FvVector3& val)
{
	if(m_kVal == val)
		return *this;
	if(HasOwner())
	{
		FvObjVector3Old oldVal(*this);
		oldVal.IncRef();
		m_kVal = val;
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal = val;
	}
	return *this;
}
FvObjVector3Old& FvObjVector3Old::operator=(const float* val)
{
	if(m_kVal == FvVector3(val))
		return *this;
	if(HasOwner())
	{
		FvObjVector3Old oldVal(*this);
		oldVal.IncRef();
		m_kVal = FvVector3(val);
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal = FvVector3(val);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjVector4Old::FvObjVector4Old(FvDataType* dataType, bool bDefault)
:FvObj(dataType), m_kVal()
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR4);
	if(bDefault)
		m_kVal = (FvVector4)FVOBJ_GETVAL(dataType->pDefaultValue(), FvObjVector4Old);
}
FvObjVector4Old::FvObjVector4Old(FvDataType* dataType, const FvVector4& v)
:FvObj(dataType), m_kVal(v)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR4);
}
FvObjVector4Old::FvObjVector4Old(FvDataType* dataType, float x, float y, float z, float w)
:FvObj(dataType), m_kVal(x, y, z, w)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR4);
}
FvObjVector4Old::FvObjVector4Old(FvDataType* dataType, const float* pfVector)
:FvObj(dataType), m_kVal(pfVector)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_VECTOR4);
}

bool FvObjVector4Old::operator<( const FvObjVector4Old & other ) const
{
	if(m_kVal.x != other.m_kVal.x)
		return m_kVal.x < other.m_kVal.x;
	else if(m_kVal.y != other.m_kVal.y)
		return m_kVal.y < other.m_kVal.y;
	else if(m_kVal.z != other.m_kVal.z)
		return m_kVal.z < other.m_kVal.z;
	else
		return m_kVal.w < other.m_kVal.w;
}
FvObjVector4Old& FvObjVector4Old::operator=(const FvVector4& val)
{
	if(m_kVal == val)
		return *this;
	if(HasOwner())
	{
		FvObjVector4Old oldVal(*this);
		oldVal.IncRef();
		m_kVal = val;
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal = val;
	}
	return *this;
}
FvObjVector4Old& FvObjVector4Old::operator=(const float* val)
{
	if(m_kVal == FvVector4(val))
		return *this;
	if(HasOwner())
	{
		FvObjVector4Old oldVal(*this);
		oldVal.IncRef();
		m_kVal = FvVector4(val);
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal = FvVector4(val);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjStringOld::FvObjStringOld(FvDataType* dataType, bool bDefault)
:FvObj(dataType)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_STRING);
	if(bDefault)
		m_kVal = FVOBJ_GETVAL(dataType->pDefaultValue(), FvObjStringOld);
	else
		m_kVal = "";
}

FvObjStringOld::FvObjStringOld(FvDataType* dataType, const FvString& s)
:FvObj(dataType), m_kVal(s)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_STRING);
}

FvObjStringOld::FvObjStringOld(FvDataType* dataType, const char* s)
:FvObj(dataType), m_kVal(s)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_STRING);
}

bool FvObjStringOld::operator<( const FvObjStringOld & other ) const
{
	int ret = m_kVal.compare(other.m_kVal);
	if(ret < 0) return true;
	else return false;
}
FvObjStringOld& FvObjStringOld::operator=(const FvString& val)
{
	if(m_kVal == val)
		return *this;
	if(HasOwner())
	{
		FvObjStringOld oldVal(*this);
		oldVal.IncRef();
		m_kVal=val;
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal=val;
	}
	return *this;
}
FvObjStringOld& FvObjStringOld::operator=(const char* val)
{
	if(m_kVal == val)
		return *this;
	if(HasOwner())
	{
		FvObjStringOld oldVal(*this);
		oldVal.IncRef();
		m_kVal=val;
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal=val;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjArrayOld::FvObjArrayOld(FvDataType* dataType, bool bDefault)
:FvObj(dataType)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_ARRAY);

	FvArrayDataType* pkArrayDataType = static_cast<FvArrayDataType*>(dataType);
	m_kElementVector.reserve(pkArrayDataType->GetReserve());

	if(bDefault)
	{
		FVOBJ_CONVERT_P(dataType->pDefaultValue(), FvObjArrayOld, pArray);
		ElementVector::iterator itrB = pArray->m_kElementVector.begin();
		ElementVector::iterator itrE = pArray->m_kElementVector.end();
		FvUInt32 ref(0);
		while(itrB != itrE)
		{
			FvObjPtr pObj = (*itrB)->Copy();
			pObj->SetRef(ref);
			m_kElementVector.push_back(pObj);
			++itrB;
			++ref;
		}
	}
}

FvObjArrayOld::FvObjArrayOld(const FvObjArrayOld& src)
:FvObj(src.m_pkDataType)
{
	m_kElementVector.reserve(src.m_kElementVector.capacity());

	ElementVector::const_iterator itrB = src.m_kElementVector.begin();
	ElementVector::const_iterator itrE = src.m_kElementVector.end();
	FvUInt32 ref(0);
	while(itrB != itrE)
	{
		FvObjPtr pObj = (*itrB)->Copy();
		pObj->SetRef(ref);
		m_kElementVector.push_back(pObj);
		++itrB;
		++ref;
	}
}

FvObjArrayOld::~FvObjArrayOld()
{
	Detach();
}

int FvObjArrayOld::GetSize()
{
	AddObjCnt(ms_uiRTTI);
	int iSize = (int)sizeof(FvObjArrayOld);
	FvUInt32 cnt = Size();
	for(FvUInt32 i=0; i<cnt; ++i)
	{
		iSize += m_kElementVector[i]->GetSize();
	}
	return iSize;
}

bool FvObjArrayOld::operator<( const FvObjArrayOld & other ) const
{
	ElementVector::size_type size1 = m_kElementVector.size();
	ElementVector::size_type size2 = other.m_kElementVector.size();
	if(size1 < size2) return true;
	if(size2 < size1) return false;

	for(ElementVector::size_type i=0; i<size1; ++i)
	{
		if(m_kElementVector[i] < other.m_kElementVector[i]) return true;
		if(other.m_kElementVector[i] < m_kElementVector[i]) return false;
	}

	return false;
}

FvObjPtr FvObjArrayOld::operator[](FvUInt32 idx) const
{
	return At(idx);
}

FvDataType* FvObjArrayOld::GetElementDataType() const
{
	FvArrayDataType* pDataType = static_cast<FvArrayDataType*>(m_pkDataType);
	return pDataType->GetElemType().GetObject();
}

FvObjPtr FvObjArrayOld::At(FvUInt32 idx) const
{
	if(idx >= Size()) return NULL;
	return m_kElementVector[idx];
}

bool FvObjArrayOld::PushBack(FvObjPtr pObj)
{
	if(!CheckElement(pObj)) return false;
	if(pObj->HasOwner()) return false;

	FvUInt32 idx = Size();
	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_ADD, idx, NULL, pObj);

	m_kElementVector.push_back(pObj);
	pObj->SetRef(idx);
	pObj->Attach(this);
	return true;
}

bool FvObjArrayOld::Insert(FvUInt32 idx, FvObjPtr pObj)
{
	if(idx > Size()) return false;
	if(!CheckElement(pObj)) return false;
	if(pObj->HasOwner()) return false;

	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_ADD, idx, NULL, pObj);

	m_kElementVector.insert(m_kElementVector.begin()+idx, pObj);
	FvUInt32 cnt = Size();
	for(FvUInt32 i=idx; i<cnt; ++i)
	{
		m_kElementVector[i]->SetRef(i);
	}
	pObj->Attach(this);
	return true;
}

bool FvObjArrayOld::Erase(FvUInt32 idx)
{
	if(idx >= Size()) return false;
	ElementVector::iterator itr = m_kElementVector.begin()+idx;
	(*itr)->Detach();

	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_DEL, idx, *itr, NULL);

	m_kElementVector.erase(itr);
	FvUInt32 cnt = Size();
	for(FvUInt32 i=idx; i<cnt; ++i)
	{
		m_kElementVector[i]->SetRef(i);
	}
	return true;
}

bool FvObjArrayOld::Erase(FvObjPtr pObj)
{
	if(!CheckElement(pObj)) return false;

	ElementVector::iterator itrB = m_kElementVector.begin();
	FvUInt32 ref(0);
	bool bSetRef(false);
	while(itrB != m_kElementVector.end())
	{
		if(bSetRef)
		{
			(*itrB)->SetRef(ref);
			++itrB;
			++ref;
		}
		else if(*itrB == pObj)
		{
			(*itrB)->Detach();
			if(HasOwner())
				PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_DEL, ref, pObj, NULL);
			itrB = m_kElementVector.erase(itrB);
			bSetRef = true;
		}
		else
		{
			++itrB;
			++ref;
		}
	}

	return false;
}

void FvObjArrayOld::Clear()
{
	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_CLR, 0, NULL, NULL);

	ElementVector::iterator itrB = m_kElementVector.begin();
	ElementVector::iterator itrE = m_kElementVector.end();
	while(itrB != itrE)
	{
		(*itrB)->Detach();
		++itrB;
	}

	m_kElementVector.clear();
}

void FvObjArrayOld::Reserve(FvUInt32 uiCnt)
{
	m_kElementVector.reserve(uiCnt);
}

void FvObjArrayOld::PopBack()
{
	if(Empty())
		return;

	FvUInt32 uiIdx = Size() -1;
	FvObjPtr spVal = m_kElementVector.back();
	spVal->Detach();

	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_DEL, uiIdx, spVal, NULL);

	m_kElementVector.pop_back();
}

bool FvObjArrayOld::Set(FvUInt32 idx, FvObjPtr pObj)
{
	if(idx>=Size() || !CheckElement(pObj))
		return false;

	FvObjPtr pOldVal = m_kElementVector[idx];
	FV_ASSERT(pOldVal);
	pOldVal->Detach();
	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_MOD, idx, pOldVal, pObj);
	m_kElementVector[idx] = pObj;
	pObj->Attach(this);
	pObj->SetRef(idx);
	return true;
}

void FvObjArrayOld::Attach(FvPropertyOwnerBase* owner)
{
	FvPropertyOwnerBase::Attach(owner);

	ElementVector::iterator itrB = m_kElementVector.begin();
	ElementVector::iterator itrE = m_kElementVector.end();
	while(itrB != itrE)
	{
		(*itrB)->Attach(this);
		++itrB;
	}
}

void FvObjArrayOld::Detach()
{
	FvPropertyOwnerBase::Detach();

	ElementVector::iterator itrB = m_kElementVector.begin();
	ElementVector::iterator itrE = m_kElementVector.end();
	while(itrB != itrE)
	{
		(*itrB)->Detach();
		++itrB;
	}
}

bool FvObjArrayOld::CheckElement(FvObjPtr pObj)
{
	if(!pObj) return false;

	FvArrayDataType* pDataType = static_cast<FvArrayDataType*>(m_pkDataType);
	if(pDataType->GetElemType()->IsSameType(pObj))
		return true;
	else
		return false;
}

FvInt32 FvObjArrayOld::PropertyDivisions()
{
	//return -1;
	return (FvInt32)Size();
}

FvPropertyOwnerBase* FvObjArrayOld::PropertyVassal(FvUInt32 ref)
{
	FV_ASSERT(ref < Size());
	return m_kElementVector[ref].GetObject();
}

void FvObjArrayOld::PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
									 FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue)
{
	FvObjPtr oldObj, newObj;

	switch(op)
	{
	case FvPropertyOwnerBase::FVPROP_OP_ADD:
		{
			FV_ASSERT(opIdx <= Size());
			FvArrayDataType* pDataType = static_cast<FvArrayDataType*>(m_pkDataType);
			newObj = pDataType->GetElemType()->CreateFromStream(data, false);
			FV_ASSERT(newObj);
			m_kElementVector.insert(m_kElementVector.begin()+opIdx, newObj);
			FvUInt32 cnt = Size();
			for(FvUInt32 i=opIdx; i<cnt; ++i)
			{
				m_kElementVector[i]->SetRef(i);
			}
			newObj->Attach(this);
			newObj->SetNewObj();
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_DEL:
		{
			FV_ASSERT(opIdx < Size());
			oldObj = m_kElementVector[opIdx];
			oldObj->Detach();
			m_kElementVector.erase(m_kElementVector.begin()+opIdx);
			FvUInt32 cnt = Size();
			for(FvUInt32 i=opIdx; i<cnt; ++i)
			{
				m_kElementVector[i]->SetRef(i);
			}
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_CLR:
		{
			ElementVector::iterator itrB = m_kElementVector.begin();
			ElementVector::iterator itrE = m_kElementVector.end();
			while(itrB != itrE)
			{
				(*itrB)->Detach();
				++itrB;
			}

			m_kElementVector.clear();
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_MOD:
		{
			FV_ASSERT(opIdx < Size());
			oldObj = m_kElementVector[opIdx];
			oldObj->Detach();
			FvArrayDataType* pDataType = static_cast<FvArrayDataType*>(m_pkDataType);
			newObj = pDataType->GetElemType()->CreateFromStream(data, false);
			FV_ASSERT(newObj);
			m_kElementVector[opIdx] = newObj;
			newObj->SetRef(opIdx);
			newObj->Attach(this);
			newObj->SetNewObj();
		}
		break;
	default:
		FV_ASSERT(0);
		break;
	}

	oldValue.second = oldObj;
	newValue.second = newObj;
}

void FvObjArrayOld::PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, FvObjPtr oldVal, FvObjPtr newVal)
{
	FV_ASSERT(m_pkOwner);
	ChangeVal oldChgVal(NULL, oldVal);
	ChangeVal newChgVal(NULL, newVal);
	ChangePath path(1, m_uiOwnerRef);
	m_pkOwner->PropertyChanged(op, opIdx, oldChgVal, newChgVal, path);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjIntMap::FvObjIntMap(FvDataType* dataType, bool bDefault)
:FvObj(dataType)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_INTMAP);
	if(bDefault)
	{
		FVOBJ_CONVERT_P(dataType->pDefaultValue(), FvObjIntMap, pMap);
		FvUInt32 cnt = (FvUInt32)pMap->m_kElementMap.size();
		if(cnt)
		{
			m_kElementRefVector.resize(cnt, m_kElementMap.end());

			ElementMap::iterator itrB = pMap->m_kElementMap.begin();
			ElementMap::iterator itrE = pMap->m_kElementMap.end();
			while(itrB != itrE)
			{
				FvUInt32 ref = itrB->second->GetRef();
				FV_ASSERT(ref < cnt);
				FV_ASSERT(m_kElementRefVector[ref] == m_kElementMap.end());
				std::pair<ElementMap::iterator, bool> ret =
					m_kElementMap.insert(std::make_pair(itrB->first, itrB->second->Copy()));
				FV_ASSERT(ret.second);
				m_kElementRefVector[ref] = ret.first;
				ret.first->second->SetRef(ref);
				++itrB;
			}
		}
	}
}

FvObjIntMap::FvObjIntMap(const FvObjIntMap& src)
:FvObj(src.m_pkDataType)
{
	FvUInt32 cnt = (FvUInt32)src.m_kElementMap.size();
	if(cnt)
	{
		m_kElementRefVector.resize(cnt, m_kElementMap.end());

		ElementMap::const_iterator itrB = src.m_kElementMap.begin();
		ElementMap::const_iterator itrE = src.m_kElementMap.end();
		while(itrB != itrE)
		{
			FvUInt32 ref = itrB->second->GetRef();
			FV_ASSERT(ref < cnt);
			FV_ASSERT(m_kElementRefVector[ref] == m_kElementMap.end());
			std::pair<ElementMap::iterator, bool> ret =
				m_kElementMap.insert(std::make_pair(itrB->first, itrB->second->Copy()));
			FV_ASSERT(ret.second);
			m_kElementRefVector[ref] = ret.first;
			ret.first->second->SetRef(ref);
			++itrB;
		}
	}
}

FvObjIntMap::~FvObjIntMap()
{
	Detach();
}

int FvObjIntMap::GetSize()
{
	AddObjCnt(ms_uiRTTI);
	int iSize = (int)sizeof(FvObjIntMap);
	ElementRefVector::iterator itrB = m_kElementRefVector.begin();
	ElementRefVector::iterator itrE = m_kElementRefVector.end();
	while(itrB != itrE)
	{
		iSize += (*itrB)->second->GetSize();
		++itrB;
	}
	return iSize;
}

bool FvObjIntMap::operator<( const FvObjIntMap & other ) const
{
	ElementMap::size_type size1 = m_kElementMap.size();
	ElementMap::size_type size2 = other.m_kElementMap.size();
	if(size1 < size2) return true;
	if(size2 < size1) return false;

	//! 不考虑顺序
	ElementMap::const_iterator itrB = m_kElementMap.begin();
	ElementMap::const_iterator itrE = m_kElementMap.end();
	ElementMap::const_iterator itrB1 = other.m_kElementMap.begin();
	while(itrB != itrE)
	{
		if(itrB->first < itrB1->first) return true;
		if(itrB1->first < itrB->first) return false;
		if(itrB->second < itrB1->second) return true;
		if(itrB1->second < itrB->second) return false;

		++itrB;
		++itrB1;
	}

	return false;
}

FvDataType* FvObjIntMap::GetElementDataType() const
{
	FvIntMapDataType* pDataType = static_cast<FvIntMapDataType*>(m_pkDataType);
	return pDataType->GetElemType().GetObject();
}

bool FvObjIntMap::Get(FvUInt32 idx, FvInt32& key, FvObjPtr& val) const
{
	if(idx >= Size()) return false;
	key = m_kElementRefVector[idx]->first;
	val = m_kElementRefVector[idx]->second;
	return true;
}

FvObjPtr FvObjIntMap::Find(FvInt32 key) const
{
	ElementMap::const_iterator itr = m_kElementMap.find(key);
	if(itr != m_kElementMap.end())
		return itr->second;
	else
		return NULL;
}

bool FvObjIntMap::Insert(FvInt32 key, FvObjPtr pObj)
{
	if(!CheckElement(pObj)) return false;
	if(pObj->HasOwner()) return false;
	std::pair<ElementMap::iterator,bool> ret =
		m_kElementMap.insert(std::make_pair(key,pObj));
	if(ret.second)
	{
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_ADD, 0, 0, NULL, key, pObj);
		pObj->SetRef(Size());
		pObj->Attach(this);
		m_kElementRefVector.push_back(ret.first);
	}
	return ret.second;
}

bool FvObjIntMap::Erase(FvInt32 key)
{
	ElementMap::iterator itr = m_kElementMap.find(key);
	if(itr != m_kElementMap.end())
	{
		itr->second->Detach();
		FvUInt32 ref = itr->second->GetRef();
		FV_ASSERT(ref < Size());
		FV_ASSERT(m_kElementRefVector[ref] == itr);
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_DEL, ref, key, itr->second, 0, NULL);
		if(m_kElementRefVector.size() > 1)
		{
			m_kElementRefVector[ref] = m_kElementRefVector[m_kElementRefVector.size()-1];
			m_kElementRefVector.pop_back();
			m_kElementRefVector[ref]->second->SetRef(ref);
		}
		else
		{
			m_kElementRefVector.clear();
		}
		m_kElementMap.erase(itr);
		return true;
	}
	return false;
}

void FvObjIntMap::Clear()
{
	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_CLR, 0, 0, NULL, 0, NULL);

	ElementRefVector::iterator itrB = m_kElementRefVector.begin();
	ElementRefVector::iterator itrE = m_kElementRefVector.end();
	while(itrB != itrE)
	{
		(*itrB)->second->Detach();
		++itrB;
	}

	m_kElementMap.clear();
	m_kElementRefVector.clear();
}

bool FvObjIntMap::Set(FvInt32 key, FvObjPtr pObj)
{
	if(!CheckElement(pObj)) return false;
	ElementMap::iterator itr = m_kElementMap.find(key);
	if(itr != m_kElementMap.end())
	{
		itr->second->Detach();
		FvUInt32 ref = itr->second->GetRef();
		FV_ASSERT(ref < Size());
		FV_ASSERT(m_kElementRefVector[ref] == itr);
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_MOD, ref, key, itr->second, NULL, pObj);
		itr->second = pObj;
		pObj->SetRef(ref);
		return true;
	}
	else
	{
		return Insert(key, pObj);
	}
}

void FvObjIntMap::Attach(FvPropertyOwnerBase* owner)
{
	FvPropertyOwnerBase::Attach(owner);

	ElementRefVector::iterator itrB = m_kElementRefVector.begin();
	ElementRefVector::iterator itrE = m_kElementRefVector.end();
	while(itrB != itrE)
	{
		(*itrB)->second->Attach(this);
		++itrB;
	}
}

void FvObjIntMap::Detach()
{
	FvPropertyOwnerBase::Detach();

	ElementRefVector::iterator itrB = m_kElementRefVector.begin();
	ElementRefVector::iterator itrE = m_kElementRefVector.end();
	while(itrB != itrE)
	{
		(*itrB)->second->Detach();
		++itrB;
	}
}

bool FvObjIntMap::CheckElement(FvObjPtr pObj)
{
	if(!pObj) return false;

	FvIntMapDataType* pDataType = static_cast<FvIntMapDataType*>(m_pkDataType);
	if(pDataType->GetElemType()->IsSameType(pObj))
		return true;
	else
		return false;
}

FvInt32 FvObjIntMap::PropertyDivisions()
{
	//return -1;
	return (FvInt32)Size();
}

FvPropertyOwnerBase* FvObjIntMap::PropertyVassal(FvUInt32 ref)
{
	FV_ASSERT(ref < Size());
	return m_kElementRefVector[ref]->second.GetObject();
}

void FvObjIntMap::PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
								  FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue)
{
	FvObjPtr oldKey, newKey;
	FvObjPtr oldObj, newObj;

	switch(op)
	{
	case FvPropertyOwnerBase::FVPROP_OP_ADD:
		{
			FV_ASSERT(opIdx == 0);
			FvInt32 key;
			data >> key;
			FvIntMapDataType* pDataType = static_cast<FvIntMapDataType*>(m_pkDataType);
			newKey = new FvObjInt32Old(pDataType->GetKeyType().GetObject(), key);
			newObj = pDataType->GetElemType()->CreateFromStream(data, false);
			FV_ASSERT(newObj);
			std::pair<ElementMap::iterator,bool> ret =
				m_kElementMap.insert(std::make_pair(key,newObj));
			FV_ASSERT(ret.second);
			newObj->SetRef(Size());
			newObj->Attach(this);
			newObj->SetNewObj();
			m_kElementRefVector.push_back(ret.first);
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_DEL:
		{
			FV_ASSERT(opIdx < Size());
			FvIntMapDataType* pDataType = static_cast<FvIntMapDataType*>(m_pkDataType);
			oldKey = new FvObjInt32Old(pDataType->GetKeyType().GetObject(), m_kElementRefVector[opIdx]->first);
			oldObj = m_kElementRefVector[opIdx]->second;
			oldObj->Detach();
			m_kElementMap.erase(m_kElementRefVector[opIdx]);
			if(m_kElementRefVector.size() > 1)
			{
				m_kElementRefVector[opIdx] = m_kElementRefVector[m_kElementRefVector.size()-1];
				m_kElementRefVector.pop_back();
				m_kElementRefVector[opIdx]->second->SetRef(opIdx);
			}
			else
			{
				m_kElementRefVector.clear();
			}
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_CLR:
		{
			ElementRefVector::iterator itrB = m_kElementRefVector.begin();
			ElementRefVector::iterator itrE = m_kElementRefVector.end();
			while(itrB != itrE)
			{
				(*itrB)->second->Detach();
				++itrB;
			}

			m_kElementMap.clear();
			m_kElementRefVector.clear();
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_MOD:
		{
			FV_ASSERT(opIdx < Size());
			FvIntMapDataType* pDataType = static_cast<FvIntMapDataType*>(m_pkDataType);
			oldKey = new FvObjInt32Old(pDataType->GetKeyType().GetObject(), m_kElementRefVector[opIdx]->first);
			oldObj = m_kElementRefVector[opIdx]->second;
			oldObj->Detach();

			newKey = oldKey;
			newObj = pDataType->GetElemType()->CreateFromStream(data, false);
			FV_ASSERT(newObj);
			m_kElementRefVector[opIdx]->second = newObj;
			newObj->SetRef(opIdx);
			newObj->Attach(this);
			newObj->SetNewObj();
		}
		break;
	default:
		FV_ASSERT(0);
		break;
	}

	oldValue.first = oldKey;
	oldValue.second = oldObj;
	newValue.first = newKey;
	newValue.second = newObj;
}

void FvObjIntMap::PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, FvInt32 oldKey, FvObjPtr oldVal, FvInt32 newKey, FvObjPtr newVal)
{
	FV_ASSERT(m_pkOwner);

	FvIntMapDataType* pDataType = static_cast<FvIntMapDataType*>(m_pkDataType);
	FvObjInt32Old kOldKey(pDataType->GetKeyType().GetObject(), oldKey);
	FvObjInt32Old kNewKey(pDataType->GetKeyType().GetObject(), newKey);

	ChangeVal oldChgVal(&kOldKey, oldVal);
	ChangeVal newChgVal(&kNewKey, newVal);
	ChangePath path(1, m_uiOwnerRef);
	m_pkOwner->PropertyChanged(op, opIdx, oldChgVal, newChgVal, path);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjStrMap::FvObjStrMap(FvDataType* dataType, bool bDefault)
:FvObj(dataType)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_STRMAP);
	if(bDefault)
	{
		FVOBJ_CONVERT_P(dataType->pDefaultValue(), FvObjStrMap, pMap);
		FvUInt32 cnt = (FvUInt32)pMap->m_kElementMap.size();
		if(cnt)
		{
			m_kElementRefVector.resize(cnt, m_kElementMap.end());

			ElementMap::iterator itrB = pMap->m_kElementMap.begin();
			ElementMap::iterator itrE = pMap->m_kElementMap.end();
			while(itrB != itrE)
			{
				FvUInt32 ref = itrB->second->GetRef();
				FV_ASSERT(ref < cnt);
				FV_ASSERT(m_kElementRefVector[ref] == m_kElementMap.end());
				std::pair<FvObjStrMap::ElementMap::iterator, bool> ret =
					m_kElementMap.insert(std::make_pair(itrB->first, itrB->second->Copy()));
				FV_ASSERT(ret.second);
				m_kElementRefVector[ref] = ret.first;
				ret.first->second->SetRef(ref);
				++itrB;
			}
		}
	}
}

FvObjStrMap::FvObjStrMap(const FvObjStrMap& src)
:FvObj(src.m_pkDataType)
{
	FvUInt32 cnt = (FvUInt32)src.m_kElementMap.size();
	if(cnt)
	{
		m_kElementRefVector.resize(cnt, m_kElementMap.end());

		ElementMap::const_iterator itrB = src.m_kElementMap.begin();
		ElementMap::const_iterator itrE = src.m_kElementMap.end();
		while(itrB != itrE)
		{
			FvUInt32 ref = itrB->second->GetRef();
			FV_ASSERT(ref < cnt);
			FV_ASSERT(m_kElementRefVector[ref] == m_kElementMap.end());
			std::pair<FvObjStrMap::ElementMap::iterator, bool> ret =
				m_kElementMap.insert(std::make_pair(itrB->first, itrB->second->Copy()));
			FV_ASSERT(ret.second);
			m_kElementRefVector[ref] = ret.first;
			ret.first->second->SetRef(ref);
			++itrB;
		}
	}
}

FvObjStrMap::~FvObjStrMap()
{
	Detach();
}

int FvObjStrMap::GetSize()
{
	AddObjCnt(ms_uiRTTI);
	int iSize = (int)sizeof(FvObjStrMap);
	ElementRefVector::iterator itrB = m_kElementRefVector.begin();
	ElementRefVector::iterator itrE = m_kElementRefVector.end();
	while(itrB != itrE)
	{
		iSize += (*itrB)->second->GetSize();
		++itrB;
	}
	return iSize;
}

bool FvObjStrMap::operator<( const FvObjStrMap & other ) const
{
	ElementMap::size_type size1 = m_kElementMap.size();
	ElementMap::size_type size2 = other.m_kElementMap.size();
	if(size1 < size2) return true;
	if(size2 < size1) return false;

	//! 不考虑顺序
	ElementMap::const_iterator itrB = m_kElementMap.begin();
	ElementMap::const_iterator itrE = m_kElementMap.end();
	ElementMap::const_iterator itrB1 = other.m_kElementMap.begin();
	while(itrB != itrE)
	{
		int ret = itrB->first.compare(itrB1->first);
		if(ret < 0) return true;
		if(ret > 0) return false;
		if(itrB->second < itrB1->second) return true;
		if(itrB1->second < itrB->second) return false;

		++itrB;
		++itrB1;
	}

	return false;
}

FvDataType* FvObjStrMap::GetElementDataType() const
{
	FvStringMapDataType* pDataType = static_cast<FvStringMapDataType*>(m_pkDataType);
	return pDataType->GetElemType().GetObject();
}

bool FvObjStrMap::Get(FvUInt32 idx, FvString& key, FvObjPtr& val) const
{
	if(idx >= Size()) return false;
	key = m_kElementRefVector[idx]->first;
	val = m_kElementRefVector[idx]->second;
	return true;
}

FvObjPtr FvObjStrMap::Find(const char* key) const
{
	ElementMap::const_iterator itr = m_kElementMap.find(FvString(key));
	if(itr != m_kElementMap.end())
		return itr->second;
	else
		return NULL;
}

FvObjPtr FvObjStrMap::Find(const FvString& key) const
{
	ElementMap::const_iterator itr = m_kElementMap.find(key);
	if(itr != m_kElementMap.end())
		return itr->second;
	else
		return NULL;
}

bool FvObjStrMap::Insert(const char* key, FvObjPtr pObj)
{
	if(!CheckElement(pObj)) return false;
	if(pObj->HasOwner()) return false;
	FvString sKey(key);
	std::pair<ElementMap::iterator,bool> ret =
		m_kElementMap.insert(std::make_pair(sKey,pObj));
	if(ret.second)
	{
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_ADD, 0, "", NULL, sKey, pObj);
		pObj->SetRef(Size());
		pObj->Attach(this);
		m_kElementRefVector.push_back(ret.first);
	}
	return ret.second;
}

bool FvObjStrMap::Insert(const FvString& key, FvObjPtr pObj)
{
	if(!CheckElement(pObj)) return false;
	if(pObj->HasOwner()) return false;
	std::pair<ElementMap::iterator,bool> ret =
		m_kElementMap.insert(std::make_pair(key,pObj));
	if(ret.second)
	{
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_ADD, 0, "", NULL, key, pObj);
		pObj->SetRef(Size());
		pObj->Attach(this);
		m_kElementRefVector.push_back(ret.first);
	}
	return ret.second;
}

bool FvObjStrMap::Erase(const char* key)
{
	FvString sKey(key);
	ElementMap::iterator itr = m_kElementMap.find(sKey);
	if(itr != m_kElementMap.end())
	{
		itr->second->Detach();
		FvUInt32 ref = itr->second->GetRef();
		FV_ASSERT(ref < Size());
		FV_ASSERT(m_kElementRefVector[ref] == itr);
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_DEL, ref, sKey, itr->second, "", NULL);
		if(m_kElementRefVector.size() > 1)
		{
			m_kElementRefVector[ref] = m_kElementRefVector[m_kElementRefVector.size()-1];
			m_kElementRefVector.pop_back();
			m_kElementRefVector[ref]->second->SetRef(ref);
		}
		else
		{
			m_kElementRefVector.clear();
		}
		m_kElementMap.erase(itr);
		return true;
	}
	return false;
}

bool FvObjStrMap::Erase(const FvString& key)
{
	ElementMap::iterator itr = m_kElementMap.find(key);
	if(itr != m_kElementMap.end())
	{
		itr->second->Detach();
		FvUInt32 ref = itr->second->GetRef();
		FV_ASSERT(ref < Size());
		FV_ASSERT(m_kElementRefVector[ref] == itr);
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_DEL, ref, key, itr->second, "", NULL);
		if(m_kElementRefVector.size() > 1)
		{
			m_kElementRefVector[ref] = m_kElementRefVector[m_kElementRefVector.size()-1];
			m_kElementRefVector.pop_back();
			m_kElementRefVector[ref]->second->SetRef(ref);
		}
		else
		{
			m_kElementRefVector.clear();
		}
		m_kElementMap.erase(itr);
		return true;
	}
	return false;
}

void FvObjStrMap::Clear()
{
	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_CLR, 0, "", NULL, "", NULL);

	ElementRefVector::iterator itrB = m_kElementRefVector.begin();
	ElementRefVector::iterator itrE = m_kElementRefVector.end();
	while(itrB != itrE)
	{
		(*itrB)->second->Detach();
		++itrB;
	}

	m_kElementMap.clear();
	m_kElementRefVector.clear();
}

bool FvObjStrMap::Set(const char* key, FvObjPtr pObj)
{
	if(!CheckElement(pObj)) return false;
	ElementMap::iterator itr = m_kElementMap.find(key);
	if(itr != m_kElementMap.end())
	{
		itr->second->Detach();
		FvUInt32 ref = itr->second->GetRef();
		FV_ASSERT(ref < Size());
		FV_ASSERT(m_kElementRefVector[ref] == itr);
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_MOD, ref, key, itr->second, NULL, pObj);
		itr->second = pObj;
		pObj->SetRef(ref);
		return true;
	}
	else
	{
		return Insert(key, pObj);
	}
}

bool FvObjStrMap::Set(const FvString& key, FvObjPtr pObj)
{
	if(!CheckElement(pObj)) return false;
	ElementMap::iterator itr = m_kElementMap.find(key);
	if(itr != m_kElementMap.end())
	{
		itr->second->Detach();
		FvUInt32 ref = itr->second->GetRef();
		FV_ASSERT(ref < Size());
		FV_ASSERT(m_kElementRefVector[ref] == itr);
		if(HasOwner())
			PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_MOD, ref, key, itr->second, NULL, pObj);
		itr->second = pObj;
		pObj->SetRef(ref);
		return true;
	}
	else
	{
		return Insert(key, pObj);
	}
}

void FvObjStrMap::Attach(FvPropertyOwnerBase* owner)
{
	FvPropertyOwnerBase::Attach(owner);

	ElementRefVector::iterator itrB = m_kElementRefVector.begin();
	ElementRefVector::iterator itrE = m_kElementRefVector.end();
	while(itrB != itrE)
	{
		(*itrB)->second->Attach(this);
		++itrB;
	}
}

void FvObjStrMap::Detach()
{
	FvPropertyOwnerBase::Detach();

	ElementRefVector::iterator itrB = m_kElementRefVector.begin();
	ElementRefVector::iterator itrE = m_kElementRefVector.end();
	while(itrB != itrE)
	{
		(*itrB)->second->Detach();
		++itrB;
	}
}

bool FvObjStrMap::CheckElement(FvObjPtr pObj)
{
	if(!pObj) return false;

	FvStringMapDataType* pDataType = static_cast<FvStringMapDataType*>(m_pkDataType);
	if(pDataType->GetElemType()->IsSameType(pObj))
		return true;
	else
		return false;
}

FvInt32 FvObjStrMap::PropertyDivisions()
{
	//return -1;
	return (FvInt32)Size();
}

FvPropertyOwnerBase* FvObjStrMap::PropertyVassal(FvUInt32 ref)
{
	FV_ASSERT(ref < Size());
	return m_kElementRefVector[ref]->second.GetObject();
}

void FvObjStrMap::PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
								   FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue)
{
	FvObjPtr oldKey, newKey;
	FvObjPtr oldObj, newObj;

	switch(op)
	{
	case FvPropertyOwnerBase::FVPROP_OP_ADD:
		{
			FV_ASSERT(opIdx == 0);
			FvString key;
			data >> key;
			FvStringMapDataType* pDataType = static_cast<FvStringMapDataType*>(m_pkDataType);
			newKey = new FvObjStringOld(pDataType->GetKeyType().GetObject(), key);
			newObj = pDataType->GetElemType()->CreateFromStream(data, false);
			FV_ASSERT(newObj);
			std::pair<ElementMap::iterator,bool> ret =
				m_kElementMap.insert(std::make_pair(key,newObj));
			FV_ASSERT(ret.second);
			newObj->SetRef(Size());
			newObj->Attach(this);
			newObj->SetNewObj();
			m_kElementRefVector.push_back(ret.first);
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_DEL:
		{
			FV_ASSERT(opIdx < Size());
			FvStringMapDataType* pDataType = static_cast<FvStringMapDataType*>(m_pkDataType);
			oldKey = new FvObjStringOld(pDataType->GetKeyType().GetObject(), m_kElementRefVector[opIdx]->first);
			oldObj = m_kElementRefVector[opIdx]->second;
			oldObj->Detach();
			m_kElementMap.erase(m_kElementRefVector[opIdx]);
			if(m_kElementRefVector.size() > 1)
			{
				m_kElementRefVector[opIdx] = m_kElementRefVector[m_kElementRefVector.size()-1];
				m_kElementRefVector.pop_back();
				m_kElementRefVector[opIdx]->second->SetRef(opIdx);
			}
			else
			{
				m_kElementRefVector.clear();
			}
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_CLR:
		{
			ElementRefVector::iterator itrB = m_kElementRefVector.begin();
			ElementRefVector::iterator itrE = m_kElementRefVector.end();
			while(itrB != itrE)
			{
				(*itrB)->second->Detach();
				++itrB;
			}

			m_kElementMap.clear();
			m_kElementRefVector.clear();
		}
		break;
	case FvPropertyOwnerBase::FVPROP_OP_MOD:
		{
			FV_ASSERT(opIdx < Size());
			FvStringMapDataType* pDataType = static_cast<FvStringMapDataType*>(m_pkDataType);
			oldKey = new FvObjStringOld(pDataType->GetKeyType().GetObject(), m_kElementRefVector[opIdx]->first);
			oldObj = m_kElementRefVector[opIdx]->second;
			oldObj->Detach();

			newKey = oldKey;
			newObj = pDataType->GetElemType()->CreateFromStream(data, false);
			FV_ASSERT(newObj);
			m_kElementRefVector[opIdx]->second = newObj;
			newObj->SetRef(opIdx);
			newObj->Attach(this);
			newObj->SetNewObj();
		}
		break;
	default:
		FV_ASSERT(0);
		break;
	}

	oldValue.first = oldKey;
	oldValue.second = oldObj;
	newValue.first = newKey;
	newValue.second = newObj;
}

void FvObjStrMap::PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, const FvString& oldKey, FvObjPtr oldVal, const FvString& newKey, FvObjPtr newVal)
{
	FV_ASSERT(m_pkOwner);

	FvStringMapDataType* pDataType = static_cast<FvStringMapDataType*>(m_pkDataType);
	FvObjStringOld kOldKey(pDataType->GetKeyType().GetObject(), oldKey);
	FvObjStringOld kNewKey(pDataType->GetKeyType().GetObject(), newKey);

	ChangeVal oldChgVal(&kOldKey, oldVal);
	ChangeVal newChgVal(&kNewKey, newVal);
	ChangePath path(1, m_uiOwnerRef);
	m_pkOwner->PropertyChanged(op, opIdx, oldChgVal, newChgVal, path);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjStructOld::FvObjStructOld(FvDataType* dataType, bool bDefault)
:FvObj(dataType)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_STRUCT);
	if(bDefault)
	{
		FVOBJ_CONVERT_P(dataType->pDefaultValue(), FvObjStructOld, pStruct);
		ElementVector::iterator itrB = pStruct->m_kElementVector.begin();
		ElementVector::iterator itrE = pStruct->m_kElementVector.end();
		FvUInt32 ref(0);
		while(itrB != itrE)
		{
			FvObjPtr pObj = (*itrB)->Copy();
			pObj->SetRef(ref);
			m_kElementVector.push_back(pObj);
			++itrB;
			++ref;
		}
	}
	else
	{
		FvStructDataType* pDataType = static_cast<FvStructDataType*>(&*dataType);
		FvUInt32 cnt = (FvUInt32)pDataType->GetNumFields();
		FV_ASSERT(cnt);
		m_kElementVector.resize(cnt, NULL);

#define FVOBJ_NEWOBJ_OPERATOR(_ID, _CLASS)			\
		case _ID:									\
			pObj = new _CLASS(pElementType, false);	\
			break

		for(FvUInt32 i=0; i<cnt; ++i)
		{
			FvDataType* pElementType = &*pDataType->GetFieldDataType(i);
			FvObjPtr pObj;
			switch(pElementType->TypeID())
			{
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_INT8, FvObjInt8Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_UINT8, FvObjUInt8Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_INT16, FvObjInt16Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_UINT16, FvObjUInt16Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_INT32, FvObjInt32Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_UINT32, FvObjUInt32Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_INT64, FvObjInt64Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_UINT64, FvObjUInt64Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_FLOAT, FvObjFloatOld);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_DOUBLE, FvObjDoubleOld);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_VECTOR2, FvObjVector2Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_VECTOR3, FvObjVector3Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_VECTOR4, FvObjVector4Old);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_STRING, FvObjStringOld);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_ARRAY, FvObjArrayOld);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_INTMAP, FvObjIntMap);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_STRMAP, FvObjStrMap);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_STRUCT, FvObjStructOld);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_MAILBOX, FvObjMailBoxOld);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_BLOB, FvObjBlobOld);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_USERDATAOBJ, FvUserDataObject);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_BOOL, FvObjBoolOld);
				FVOBJ_NEWOBJ_OPERATOR(FV_DATATYPEID_NUM_FIXARRAY, FvObjNumFixArrayOld);
			default:
				FV_ASSERT(0);
				break;
			}

			pObj->SetRef(i);
			m_kElementVector[i] = pObj;
		}
	}
}

FvObjStructOld::FvObjStructOld(const FvObjStructOld& src)
:FvObj(src.m_pkDataType)
{
	ElementVector::const_iterator itrB = src.m_kElementVector.begin();
	ElementVector::const_iterator itrE = src.m_kElementVector.end();
	FvUInt32 ref(0);
	while(itrB != itrE)
	{
		FvObjPtr pObj = (*itrB)->Copy();
		pObj->SetRef(ref);
		m_kElementVector.push_back(pObj);
		++itrB;
		++ref;
	}
}

FvObjStructOld::FvObjStructOld(FvDataType* dataType, FvUInt32 dummy)
:FvObj(dataType)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_STRUCT);
}

FvObjStructOld::~FvObjStructOld()
{
	Detach();
}

int FvObjStructOld::GetSize()
{
	AddObjCnt(ms_uiRTTI);
	int iSize = (int)sizeof(FvObjStructOld);
	ElementVector::iterator itrB = m_kElementVector.begin();
	ElementVector::iterator itrE = m_kElementVector.end();
	while(itrB != itrE)
	{
		iSize += (*itrB)->GetSize();
		++itrB;
	}
	return iSize;
}

bool FvObjStructOld::operator<( const FvObjStructOld & other ) const
{
	//! 不比较字段名
	ElementVector::size_type size1 = m_kElementVector.size();
	ElementVector::size_type size2 = other.m_kElementVector.size();
	if(size1 < size2) return true;
	if(size2 < size1) return false;

	for(ElementVector::size_type i=0; i<size1; ++i)
	{
		if(m_kElementVector[i] < other.m_kElementVector[i]) return true;
		if(other.m_kElementVector[i] < m_kElementVector[i]) return false;
	}

	return false;
}

FvObjPtr FvObjStructOld::operator[](FvUInt32 idx) const
{
	return At(idx);
}

FvUInt32 FvObjStructOld::GetFieldNum() const
{
	FvStructDataType* pDataType = static_cast<FvStructDataType*>(m_pkDataType);
	return (FvUInt32)pDataType->GetNumFields();
}

bool FvObjStructOld::GetFieldIndex(const char* fieldName, FvUInt32& idx) const
{
	FvStructDataType* pDataType = static_cast<FvStructDataType*>(m_pkDataType);
	FvInt32 ret = pDataType->GetFieldIndex(FvString(fieldName));
	if(ret != -1)
	{
		idx = (FvUInt32) ret;
		return true;
	}
	else
	{
		return false;
	}
}

bool FvObjStructOld::GetFieldIndex(const FvString& fieldName, FvUInt32& idx) const
{
	FvStructDataType* pDataType = static_cast<FvStructDataType*>(m_pkDataType);
	FvInt32 ret = pDataType->GetFieldIndex(fieldName);
	if(ret != -1)
	{
		idx = (FvUInt32) ret;
		return true;
	}
	else
	{
		return false;
	}
}

bool FvObjStructOld::GetFieldName(FvUInt32 index, FvString& name) const
{
	FvStructDataType* pDataType = static_cast<FvStructDataType*>(m_pkDataType);
	if(index >= pDataType->GetNumFields()) return false;
	name = pDataType->GetFieldName(index);
	return true;
}

FvDataType* FvObjStructOld::GetFieldDataType(FvUInt32 index) const
{
	FvStructDataType* pDataType = static_cast<FvStructDataType*>(m_pkDataType);
	if(index >= pDataType->GetNumFields()) return NULL;
	return pDataType->GetFieldDataType(index).GetObject();
}

FvObjPtr FvObjStructOld::At(FvUInt32 idx) const
{
	if(idx >= GetFieldNum()) return NULL;
	return m_kElementVector[idx];
}

FvObjPtr FvObjStructOld::AtName(const char* fieldName) const
{
	FvUInt32 idx(0);
	if(GetFieldIndex(fieldName, idx))
		return At(idx);
	else
		return NULL;
}

FvObjPtr FvObjStructOld::AtName(const FvString& fieldName) const
{
	FvUInt32 idx(0);
	if(GetFieldIndex(fieldName, idx))
		return At(idx);
	else
		return NULL;
}

bool FvObjStructOld::Set(FvUInt32 idx, FvObjPtr pObj)
{
	if(!pObj || idx >= GetFieldNum())
		return false;

	FvDataType* pkDataType = GetFieldDataType(idx);
	if(!pkDataType->IsSameType(pObj))
		return false;

	FvObjPtr pOldVal = m_kElementVector[idx];
	FV_ASSERT(pOldVal);
	pOldVal->Detach();
	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_MOD, idx, pOldVal, pObj);
	m_kElementVector[idx] = pObj;
	pObj->Attach(this);
	pObj->SetRef(idx);
	return true;
}

bool FvObjStructOld::SetName(const char* fieldName, FvObjPtr pObj)
{
	FvUInt32 idx(0);
	if(!pObj || !GetFieldIndex(fieldName, idx))
		return false;

	FvDataType* pkDataType = GetFieldDataType(idx);
	if(!pkDataType->IsSameType(pObj))
		return false;

	FvObjPtr pOldVal = m_kElementVector[idx];
	FV_ASSERT(pOldVal);
	pOldVal->Detach();
	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_MOD, idx, pOldVal, pObj);
	m_kElementVector[idx] = pObj;
	pObj->Attach(this);
	pObj->SetRef(idx);
	return true;
}

bool FvObjStructOld::SetName(const FvString& fieldName, FvObjPtr pObj)
{
	FvUInt32 idx(0);
	if(!pObj || !GetFieldIndex(fieldName, idx))
		return false;

	FvDataType* pkDataType = GetFieldDataType(idx);
	if(!pkDataType->IsSameType(pObj))
		return false;

	FvObjPtr pOldVal = m_kElementVector[idx];
	FV_ASSERT(pOldVal);
	pOldVal->Detach();
	if(HasOwner())
		PropertyChangeNotify(FvPropertyOwnerBase::FVPROP_OP_MOD, idx, pOldVal, pObj);
	m_kElementVector[idx] = pObj;
	pObj->Attach(this);
	pObj->SetRef(idx);
	return true;
}

void FvObjStructOld::Attach(FvPropertyOwnerBase* owner)
{
	FvPropertyOwnerBase::Attach(owner);

	ElementVector::iterator itrB = m_kElementVector.begin();
	ElementVector::iterator itrE = m_kElementVector.end();
	while(itrB != itrE)
	{
		(*itrB)->Attach(this);
		++itrB;
	}
}

void FvObjStructOld::Detach()
{
	FvPropertyOwnerBase::Detach();

	ElementVector::iterator itrB = m_kElementVector.begin();
	ElementVector::iterator itrE = m_kElementVector.end();
	while(itrB != itrE)
	{
		(*itrB)->Detach();
		++itrB;
	}
}

FvInt32 FvObjStructOld::PropertyDivisions()
{
	//return -1;
	return (FvInt32)GetFieldNum();
}

FvPropertyOwnerBase* FvObjStructOld::PropertyVassal(FvUInt32 ref)
{
	FV_ASSERT(ref < GetFieldNum());
	return m_kElementVector[ref].GetObject();
}

void FvObjStructOld::PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
								  FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue)
{
	FvObjPtr oldObj, newObj;

	switch(op)
	{
	case FvPropertyOwnerBase::FVPROP_OP_MOD:
		{
			FV_ASSERT(opIdx < GetFieldNum());
			FvStructDataType* pDataType = static_cast<FvStructDataType*>(m_pkDataType);
			FvDataTypePtr spFieldType = pDataType->GetFieldDataType(opIdx);
			oldObj = m_kElementVector[opIdx];
			oldObj->Detach();
			newObj = spFieldType->CreateFromStream(data, false);
			FV_ASSERT(newObj);
			m_kElementVector[opIdx] = newObj;
			newObj->SetRef(opIdx);
			newObj->Attach(this);
			newObj->SetNewObj();
		}
		break;
	default:
		FV_ASSERT(0);
		break;
	}

	oldValue.second = oldObj;
	newValue.second = newObj;
}

void FvObjStructOld::PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, FvObjPtr oldVal, FvObjPtr newVal)
{
	FV_ASSERT(m_pkOwner);
	ChangeVal oldChgVal(NULL, oldVal);
	ChangeVal newChgVal(NULL, newVal);
	ChangePath path(1, m_uiOwnerRef);
	m_pkOwner->PropertyChanged(op, opIdx, oldChgVal, newChgVal, path);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjMailBoxOld::FvObjMailBoxOld()
:FvObj(FvMailBoxDataType::pInstance())
{
	m_kMBRef.Init();
}

FvObjMailBoxOld::FvObjMailBoxOld(const FvObjMailBoxOld& src)
:FvObj(src.m_pkDataType)
{
	memcpy(&m_kMBRef, &src.m_kMBRef, sizeof(m_kMBRef));
}

FvObjMailBoxOld::FvObjMailBoxOld(FvDataType* dataType, bool bDefault)
:FvObj(FvMailBoxDataType::pInstance())
{
	m_kMBRef.Init();
}

FvObjMailBoxOld::~FvObjMailBoxOld()
{
}

bool FvObjMailBoxOld::operator==( const FvObjMailBoxOld& kMailBox ) const
{
	return m_kMBRef.m_iID==kMailBox.m_kMBRef.m_iID &&
		m_kMBRef.m_kAddr==kMailBox.m_kMBRef.m_kAddr;
}

bool FvObjMailBoxOld::operator!=( const FvObjMailBoxOld& kMailBox ) const
{
	return m_kMBRef.m_iID!=kMailBox.m_kMBRef.m_iID ||
		m_kMBRef.m_kAddr!=kMailBox.m_kMBRef.m_kAddr;
}

char FvObjMailBoxOld::ms_acBuf[128];
const char* FvObjMailBoxOld::c_str()
{
	sprintf(ms_acBuf, "%s, Id:%d, type:%d, Component:%d", m_kMBRef.m_kAddr.c_str(), m_kMBRef.m_iID, m_kMBRef.GetType(), m_kMBRef.GetComponent());
	return ms_acBuf;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjBlobOld::FvObjBlobOld(int iSize)
:FvObj(FvBlobDataType::pInstance())
{
	FV_ASSERT(iSize > 0);
	m_pcBegin = new char[iSize];
	m_pcCurr = m_pcBegin;
	m_pcEnd = m_pcBegin + iSize;
	m_pcRead = m_pcBegin;
}

FvObjBlobOld::FvObjBlobOld(const FvObjBlobOld& src)
:FvObj(src.m_pkDataType)
{
	int iSize = (int)(src.m_pcEnd - src.m_pcBegin);
	FV_ASSERT(iSize > 0);
	m_pcBegin = new char[iSize];
	memcpy(m_pcBegin, src.m_pcBegin, iSize);
	m_pcCurr = src.m_pcCurr - src.m_pcBegin + m_pcBegin;
	m_pcRead = src.m_pcRead - src.m_pcBegin + m_pcBegin;
	m_pcEnd = m_pcBegin + iSize;
}

FvObjBlobOld::FvObjBlobOld(FvDataType* dataType, bool bDefault)
:FvObj(FvBlobDataType::pInstance())
{
	int iSize = DEFSIZE;
	m_pcBegin = new char[iSize];
	m_pcCurr = m_pcBegin;
	m_pcEnd = m_pcBegin + iSize;
	m_pcRead = m_pcBegin;
}

FvObjBlobOld::~FvObjBlobOld()
{
	delete [] m_pcBegin;
}

int	FvObjBlobOld::GetSize()
{
	AddObjCnt(ms_uiRTTI);
	return (int)sizeof(FvObjBlobOld) + (int)(m_pcEnd - m_pcBegin);
}

void* FvObjBlobOld::Reserve(int iBytes)
{
	char* pOldCurr = m_pcCurr;
	m_pcCurr += iBytes;

	if(m_pcCurr > m_pcEnd)
	{
		int multiplier = (int)((m_pcCurr - m_pcBegin)/(m_pcEnd - m_pcBegin) + 1);
		int newSize = multiplier * (int)(m_pcEnd - m_pcBegin);
		char* pNewData = new char[newSize];
		memcpy(pNewData, m_pcBegin, pOldCurr - m_pcBegin);
		m_pcCurr = m_pcCurr - m_pcBegin + pNewData;
		pOldCurr = pOldCurr - m_pcBegin + pNewData;
		m_pcRead = m_pcRead - m_pcBegin + pNewData;
		delete [] m_pcBegin;
		m_pcBegin = pNewData;
		m_pcEnd = m_pcBegin + newSize;
	}

	return pOldCurr;
}

int FvObjBlobOld::Size() const
{
	return (int)(m_pcCurr - m_pcBegin);
}

const void*	FvObjBlobOld::Retrieve(int iBytes)
{
	char* pOldRead = m_pcRead;

	FV_IF_NOT_ASSERT_DEV(m_pcRead + iBytes <= m_pcCurr)
	{
		m_bError = true;

		return iBytes <= int(sizeof(ms_acErrBuf)) ? ms_acErrBuf : NULL;
	}

	m_pcRead += iBytes;

	return pOldRead;
}

int FvObjBlobOld::RemainingLength() const
{
	return (int)(m_pcCurr - m_pcRead);
}

char FvObjBlobOld::Peek()
{
	FV_IF_NOT_ASSERT_DEV(m_pcRead < m_pcCurr)
	{
		m_bError = true;
		return -1;
	}

	return *(char*)m_pcRead;
}

void* FvObjBlobOld::Data() const
{
	return m_pcBegin;
}

void FvObjBlobOld::Reset()
{
	m_pcRead = m_pcCurr = m_pcBegin;
}

void FvObjBlobOld::Rewind()
{
	m_pcRead = m_pcBegin;
}

void FvObjBlobOld::ModifyNotify()
{
	if(HasOwner())
		PropertyModifyNotify(NULL);	//! old值赋NULL了
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjBoolOld::FvObjBoolOld(FvDataType* dataType, bool bDefault)
:FvObj(dataType)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_BOOL);
	if(bDefault)
		m_kVal = FVOBJ_GETVAL(dataType->pDefaultValue(), FvObjBoolOld);
	else
		m_kVal = false;
}

FvObjBoolOld::FvObjBoolOld(FvDataType* dataType, const bool b, FvUInt8 dummy)
:FvObj(dataType), m_kVal(b)
{
	FV_ASSERT(dataType && dataType->TypeID() == FV_DATATYPEID_BOOL);
}

bool FvObjBoolOld::operator<( const FvObjBoolOld & other ) const
{
	if(m_kVal < other.m_kVal)
		return true;
	else
		return false;
}

FvObjBoolOld& FvObjBoolOld::operator=(const bool val)
{
	if(m_kVal == val)
		return *this;
	if(HasOwner())
	{
		FvObjBoolOld oldVal(*this);
		oldVal.IncRef();
		m_kVal=val;
		PropertyModifyNotify(&oldVal);
	}
	else
	{
		m_kVal=val;
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjNumFixArrayOld::FvObjNumFixArrayOld(FvDataType* dataType, bool bDefault)
:FvObj(dataType)
{

}

FvObjNumFixArrayOld::FvObjNumFixArrayOld(const FvObjNumFixArrayOld& src)
:FvObj(src.m_pkDataType)
{

}

FvObjNumFixArrayOld::~FvObjNumFixArrayOld()
{

}

int FvObjNumFixArrayOld::GetSize()
{
	return m_kElementVector.size();
}

bool FvObjNumFixArrayOld::operator<( const FvObjNumFixArrayOld & other ) const
{
	//! 不比较字段名
	ElementVector::size_type size1 = m_kElementVector.size();
	ElementVector::size_type size2 = other.m_kElementVector.size();
	if(size1 < size2) return true;
	if(size2 < size1) return false;

	for(ElementVector::size_type i=0; i<size1; ++i)
	{
		if(m_kElementVector[i] < other.m_kElementVector[i]) return true;
		if(other.m_kElementVector[i] < m_kElementVector[i]) return false;
	}

	return false;
}

FvObjPtr FvObjNumFixArrayOld::operator[](FvUInt32 idx) const
{
	FvUInt32 uiSize = (FvUInt32)m_kElementVector.size();
	if(uiSize <= idx)
		return NULL;
	else
		return m_kElementVector[idx];
}