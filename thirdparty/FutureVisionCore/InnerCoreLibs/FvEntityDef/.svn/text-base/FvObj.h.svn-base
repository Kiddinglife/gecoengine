//{future header message}
#ifndef __FvObj_H__
#define __FvObj_H__

#include "FvEntityDefDefines.h"
#include "FvPropertyOwnerBase.h"

#include <FvBaseTypes.h>
#include <FvDebug.h>
#include <FvSmartPointer.h>
#include <FvVector2.h>
#include <FvVector3.h>
#include <FvVector4.h>
#include <FvNetTypes.h>

#include <vector>
#include <map>

class FvDataType;
class FvXMLSection;
typedef FvSmartPointer<FvXMLSection> FvXMLSectionPtr;

enum FvObjRTTIType
{
	FVOBJ_RTTI_OBJ,
	FVOBJ_RTTI_INT8,
	FVOBJ_RTTI_UINT8,
	FVOBJ_RTTI_INT16,
	FVOBJ_RTTI_UINT16,
	FVOBJ_RTTI_INT32,
	FVOBJ_RTTI_UINT32,
	FVOBJ_RTTI_INT64,
	FVOBJ_RTTI_UINT64,
	FVOBJ_RTTI_FLOAT,
	FVOBJ_RTTI_DOUBLE,
	FVOBJ_RTTI_VECTOR2,
	FVOBJ_RTTI_VECTOR3,
	FVOBJ_RTTI_VECTOR4,
	FVOBJ_RTTI_STRING,
	FVOBJ_RTTI_ARRAY,
	FVOBJ_RTTI_INTMAP,
	FVOBJ_RTTI_STRMAP,
	FVOBJ_RTTI_STRUCT,
	FVOBJ_RTTI_MAILBOX,
	FVOBJ_RTTI_BLOB,
	FVOBJ_RTTI_USERDATAOBJ,
	FVOBJ_RTTI_BOOL,
	FVOBJ_RTTI_NUMFIXARRAY,
	FVOBJ_RTTI_MAX,
};

//#ifdef _DEBUG

//typedef FvUInt32 FvObjRTTIType;

#define FVOBJ_RTTI_DEFINE								\
public:													\
	virtual FvObjRTTIType RTTI() { return ms_uiRTTI; }	\
	static FvObj* VerifyRTTI(FvObjPtr pObj)				\
	{													\
		FV_ASSERT(pObj && pObj->RTTI() == ms_uiRTTI);	\
		return pObj.GetObject();						\
	}													\
	static const FvObjRTTIType ms_uiRTTI;

#define FVOBJ_RTTI_IMPLEMENT(_CLASSNAME, _VAL)			\
	FV_ENTITYDEF_API const FvObjRTTIType _CLASSNAME::ms_uiRTTI = _VAL;

//#else
//
//#define FVOBJ_RTTI_DEFINE
//#define FVOBJ_RTTI_IMPLEMENT(_CLASSNAME, _VAL)
//
//#endif


class FvObj;
typedef FvSmartPointer<FvObj> FvObjPtr;

class FV_ENTITYDEF_API FvObj : public FvReferenceCount, public FvPropertyOwnerBase
{
	FVOBJ_RTTI_DEFINE
public:
	FvObj(FvDataType* dataType):m_pkDataType(dataType) {}
	virtual~FvObj() {}

	FvDataType*			DataType() const { return m_pkDataType; }
	virtual FvObjPtr	Copy() = 0;

	static void			ClearObjCnt();
	static FvUInt64*	GetObjCnt() { return m_uiObjCnt; }
	virtual int			GetSize() = 0;

protected:
	void				PropertyModifyNotify(FvObjPtr spOldVal);
	void				AddObjCnt(int iIndex);

protected:
	FvDataType*		m_pkDataType;
	static FvUInt64	m_uiObjCnt[FVOBJ_RTTI_MAX];
};

#define FVOBJ_OPERATOR_GET(_TYPE) operator _TYPE() { return (_TYPE)m_Val; }
#define FVOBJ_OPERATOR_SET(_TYPE, _RWTYPE) FvOldSimpleData##_RWTYPE& operator=(_TYPE val);

#ifndef FV_SERVER

#define FVOBJ_SIMPLEDATA_DEFINE(_DATATYPE, _CLASSNAME)														\
class FvOldSimpleData##_DATATYPE;																				\
typedef FvOldSimpleData##_DATATYPE _CLASSNAME;																	\
class FV_ENTITYDEF_API FvOldSimpleData##_DATATYPE : public FvObj												\
{																											\
	FVOBJ_RTTI_DEFINE																						\
public:																										\
	_CLASSNAME(FvDataType* dt, bool bDefault=true);															\
	_CLASSNAME(FvDataType* dt, _DATATYPE v);																\
	_CLASSNAME(const FvOldSimpleData##_DATATYPE& src):FvObj(src.m_pkDataType), m_Val(src.m_Val) {}				\
																											\
	virtual FvObjPtr	Copy() { return new _CLASSNAME(*this); }											\
	virtual int			GetSize() { AddObjCnt(ms_uiRTTI); return (int)sizeof(_CLASSNAME); }					\
																											\
	bool operator<( const _CLASSNAME & other ) const;														\
																											\
	FVOBJ_OPERATOR_GET(FvInt8)																				\
	FVOBJ_OPERATOR_GET(FvUInt8)																				\
	FVOBJ_OPERATOR_GET(FvInt16)																				\
	FVOBJ_OPERATOR_GET(FvUInt16)																			\
	FVOBJ_OPERATOR_GET(FvInt32)																				\
	FVOBJ_OPERATOR_GET(FvUInt32)																			\
	FVOBJ_OPERATOR_GET(FvInt64)																				\
	FVOBJ_OPERATOR_GET(FvUInt64)																			\
	FVOBJ_OPERATOR_GET(float)																				\
	FVOBJ_OPERATOR_GET(double)																				\
																											\
protected:																									\
	FVOBJ_OPERATOR_SET(FvInt8, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvUInt8, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvInt16, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvUInt16, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvInt32, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvUInt32, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvInt64, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvUInt64, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(float, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(double, _DATATYPE)																	\
																											\
protected:																									\
	_DATATYPE	m_Val;																						\
};																											\
typedef FvSmartPointer<_CLASSNAME> _CLASSNAME##Ptr

#else

#define FVOBJ_SIMPLEDATA_DEFINE(_DATATYPE, _CLASSNAME)														\
class FvOldSimpleData##_DATATYPE;																				\
typedef FvOldSimpleData##_DATATYPE _CLASSNAME;																	\
class FV_ENTITYDEF_API FvOldSimpleData##_DATATYPE : public FvObj												\
{																											\
	FVOBJ_RTTI_DEFINE																						\
public:																										\
	_CLASSNAME(FvDataType* dt, bool bDefault=true);															\
	_CLASSNAME(FvDataType* dt, _DATATYPE v);																\
	_CLASSNAME(const FvOldSimpleData##_DATATYPE& src):FvObj(src.m_pkDataType), m_Val(src.m_Val) {}				\
																											\
	virtual FvObjPtr	Copy() { return new _CLASSNAME(*this); }											\
	virtual int			GetSize() { AddObjCnt(ms_uiRTTI); return (int)sizeof(_CLASSNAME); }					\
																											\
	bool operator<( const _CLASSNAME & other ) const;														\
																											\
	FVOBJ_OPERATOR_GET(FvInt8)																				\
	FVOBJ_OPERATOR_GET(FvUInt8)																				\
	FVOBJ_OPERATOR_GET(FvInt16)																				\
	FVOBJ_OPERATOR_GET(FvUInt16)																			\
	FVOBJ_OPERATOR_GET(FvInt32)																				\
	FVOBJ_OPERATOR_GET(FvUInt32)																			\
	FVOBJ_OPERATOR_GET(FvInt64)																				\
	FVOBJ_OPERATOR_GET(FvUInt64)																			\
	FVOBJ_OPERATOR_GET(float)																				\
	FVOBJ_OPERATOR_GET(double)																				\
																											\
	FVOBJ_OPERATOR_SET(FvInt8, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvUInt8, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvInt16, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvUInt16, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvInt32, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvUInt32, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvInt64, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(FvUInt64, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(float, _DATATYPE)																	\
	FVOBJ_OPERATOR_SET(double, _DATATYPE)																	\
																											\
protected:																									\
	_DATATYPE	m_Val;																						\
};																											\
typedef FvSmartPointer<_CLASSNAME> _CLASSNAME##Ptr

#endif

FVOBJ_SIMPLEDATA_DEFINE(FvInt8, FvObjInt8Old);
FVOBJ_SIMPLEDATA_DEFINE(FvUInt8, FvObjUInt8Old);
FVOBJ_SIMPLEDATA_DEFINE(FvInt16, FvObjInt16Old);
FVOBJ_SIMPLEDATA_DEFINE(FvUInt16, FvObjUInt16Old);
FVOBJ_SIMPLEDATA_DEFINE(FvInt32, FvObjInt32Old);
FVOBJ_SIMPLEDATA_DEFINE(FvUInt32, FvObjUInt32Old);
FVOBJ_SIMPLEDATA_DEFINE(FvInt64, FvObjInt64Old);
FVOBJ_SIMPLEDATA_DEFINE(FvUInt64, FvObjUInt64Old);
FVOBJ_SIMPLEDATA_DEFINE(float, FvObjFloatOld);
FVOBJ_SIMPLEDATA_DEFINE(double, FvObjDoubleOld);


class FV_ENTITYDEF_API FvObjVector2Old : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend void FvFromSectionToVector( FvXMLSectionPtr pSection, FvObjVector2Old & obj );
	friend void FvFromVectorToSection( FvObjVector2Old & obj, FvXMLSectionPtr pSection );
public:
	FvObjVector2Old(FvDataType* dataType, bool bDefault=true);
	FvObjVector2Old(FvDataType* dataType, const FvVector2& v);
	FvObjVector2Old(FvDataType* dataType, float x, float y);
	FvObjVector2Old(FvDataType* dataType, const float* pfVector);
	FvObjVector2Old(const FvObjVector2Old& src):FvObj(src.m_pkDataType), m_kVal(src.m_kVal) {}

	virtual FvObjPtr	Copy() { return new FvObjVector2Old(*this); }
	virtual int			GetSize() { AddObjCnt(ms_uiRTTI); return (int)sizeof(FvObjVector2Old); }

	operator float*() { return m_kVal; }
	operator const float*() { return m_kVal; }
	bool operator<( const FvObjVector2Old & other ) const;

#ifndef FV_SERVER
protected:
#endif
	FvObjVector2Old& operator=(const FvVector2& val);
	FvObjVector2Old& operator=(const float* val);

protected:
	FvVector2	m_kVal;
};
typedef FvSmartPointer<FvObjVector2Old> FvObjVector2Ptr;


class FV_ENTITYDEF_API FvObjVector3Old : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend void FvFromSectionToVector( FvXMLSectionPtr pSection, FvObjVector3Old & obj );
	friend void FvFromVectorToSection( FvObjVector3Old & obj, FvXMLSectionPtr pSection );
public:
	FvObjVector3Old(FvDataType* dataType, bool bDefault=true);
	FvObjVector3Old(FvDataType* dataType, const FvVector3& v);
	FvObjVector3Old(FvDataType* dataType, float x, float y, float z);
	FvObjVector3Old(FvDataType* dataType, const float* pfVector);
	FvObjVector3Old(const FvObjVector3Old& src):FvObj(src.m_pkDataType), m_kVal(src.m_kVal) {}

	virtual FvObjPtr	Copy() { return new FvObjVector3Old(*this); }
	virtual int			GetSize() { AddObjCnt(ms_uiRTTI); return (int)sizeof(FvObjVector3Old); }

	operator float*() { return m_kVal; }
	operator const float*() { return m_kVal; }
	bool operator<( const FvObjVector3Old & other ) const;

#ifndef FV_SERVER
protected:
#endif
	FvObjVector3Old& operator=(const FvVector3& val);
	FvObjVector3Old& operator=(const float* val);

protected:
	FvVector3	m_kVal;
};
typedef FvSmartPointer<FvObjVector3Old> FvObjVector3Ptr;


class FV_ENTITYDEF_API FvObjVector4Old : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend void FvFromSectionToVector( FvXMLSectionPtr pSection, FvObjVector4Old & obj );
	friend void FvFromVectorToSection( FvObjVector4Old & obj, FvXMLSectionPtr pSection );
public:
	FvObjVector4Old(FvDataType* dataType, bool bDefault=true);
	FvObjVector4Old(FvDataType* dataType, const FvVector4& v);
	FvObjVector4Old(FvDataType* dataType, float x, float y, float z, float w);
	FvObjVector4Old(FvDataType* dataType, const float* pfVector);
	FvObjVector4Old(const FvObjVector4Old& src):FvObj(src.m_pkDataType), m_kVal(src.m_kVal) {}

	virtual FvObjPtr	Copy() { return new FvObjVector4Old(*this); }
	virtual int			GetSize() { AddObjCnt(ms_uiRTTI); return (int)sizeof(FvObjVector4Old); }

	operator float*() { return m_kVal; }
	operator const float*() { return m_kVal; }
	bool operator<( const FvObjVector4Old & other ) const;

#ifndef FV_SERVER
protected:
#endif
	FvObjVector4Old& operator=(const FvVector4& val);
	FvObjVector4Old& operator=(const float* val);

protected:
	FvVector4	m_kVal;
};
typedef FvSmartPointer<FvObjVector4Old> FvObjVector4Ptr;


class FV_ENTITYDEF_API FvObjStringOld : public FvObj
{
	FVOBJ_RTTI_DEFINE
public:
	FvObjStringOld(FvDataType* dataType, bool bDefault=true);
	FvObjStringOld(FvDataType* dataType, const FvString& s);
	FvObjStringOld(FvDataType* dataType, const char* s);
	FvObjStringOld(const FvObjStringOld& src):FvObj(src.m_pkDataType), m_kVal(src.m_kVal) {}

	virtual FvObjPtr	Copy() { return new FvObjStringOld(*this); }
	virtual int			GetSize() { AddObjCnt(ms_uiRTTI); return (int)sizeof(FvObjStringOld); }

	operator FvString&() { return m_kVal; }
	operator const FvString&() { return m_kVal; }
	operator char*() { return (char*)m_kVal.c_str(); }
	operator const char*() { return m_kVal.c_str(); }
	bool operator<( const FvObjStringOld & other ) const;

#ifndef FV_SERVER
protected:
#endif
	FvObjStringOld& operator=(const FvString& val);
	FvObjStringOld& operator=(const char* val);

protected:
	FvString	m_kVal;
};
typedef FvSmartPointer<FvObjStringOld> FvObjStringPtr;


class FV_ENTITYDEF_API FvObjArrayOld : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend class FvArrayDataType;
public:
	FvObjArrayOld(FvDataType* dataType, bool bDefault=true);
	FvObjArrayOld(const FvObjArrayOld& src);
	~FvObjArrayOld();

	virtual FvObjPtr	Copy() { return new FvObjArrayOld(*this); }
	virtual int			GetSize();

	bool operator<( const FvObjArrayOld & other ) const;
	FvObjPtr operator[](FvUInt32 idx) const;

	FvDataType*			GetElementDataType() const;

	FvUInt32			Capacity() const { return (FvUInt32)m_kElementVector.capacity(); }
	FvUInt32			Size() const { return (FvUInt32)m_kElementVector.size(); }
	FvObjPtr			At(FvUInt32 idx) const;
	bool				Empty() const { return m_kElementVector.empty(); }

#ifndef FV_SERVER
protected:
#endif
	bool				PushBack(FvObjPtr pObj);
	bool				Insert(FvUInt32 idx, FvObjPtr pObj);
	bool				Erase(FvUInt32 idx);
	bool				Erase(FvObjPtr pObj);
	void				Clear();
	void				Reserve(FvUInt32 uiCnt);
	void				PopBack();
	bool				Set(FvUInt32 idx, FvObjPtr pObj);

protected:
	virtual	void		Attach(FvPropertyOwnerBase* owner);
	virtual	void		Detach();
	bool				CheckElement(FvObjPtr pObj);
	virtual FvInt32		PropertyDivisions();
	virtual FvPropertyOwnerBase* PropertyVassal(FvUInt32 ref);
	virtual void		PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
							FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue);
	void				PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, FvObjPtr oldVal, FvObjPtr newVal);

protected:
	typedef std::vector<FvObjPtr> ElementVector;
	ElementVector		m_kElementVector;
};
typedef FvSmartPointer<FvObjArrayOld> FvObjArrayPtr;


class FV_ENTITYDEF_API FvObjIntMap : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend class FvIntMapDataType;
public:
	FvObjIntMap(FvDataType* dataType, bool bDefault=true);
	FvObjIntMap(const FvObjIntMap& src);
	~FvObjIntMap();

	virtual FvObjPtr	Copy() { return new FvObjIntMap(*this); }
	virtual int			GetSize();

	bool operator<( const FvObjIntMap & other ) const;

	FvDataType*			GetElementDataType() const;

	FvUInt32			Size() const { return (FvUInt32)m_kElementRefVector.size(); }
	bool				Get(FvUInt32 idx, FvInt32& key, FvObjPtr& val) const;
	FvObjPtr			Find(FvInt32 key) const;
	bool				Empty() const { return m_kElementRefVector.empty(); }

#ifndef FV_SERVER
protected:
#endif
	bool				Insert(FvInt32 key, FvObjPtr pObj);
	bool				Erase(FvInt32 key);
	void				Clear();
	bool				Set(FvInt32 key, FvObjPtr pObj);

protected:
	virtual	void		Attach(FvPropertyOwnerBase* owner);
	virtual	void		Detach();
	bool				CheckElement(FvObjPtr pObj);
	virtual FvInt32		PropertyDivisions();
	virtual FvPropertyOwnerBase* PropertyVassal(FvUInt32 ref);
	virtual void		PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
							FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue);
	void				PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, FvInt32 oldKey, FvObjPtr oldVal, FvInt32 newKey, FvObjPtr newVal);

protected:
	typedef std::map<FvInt32,FvObjPtr> ElementMap;
	typedef std::vector<ElementMap::iterator> ElementRefVector;
	ElementMap			m_kElementMap;
	ElementRefVector	m_kElementRefVector;
};
typedef FvSmartPointer<FvObjIntMap> FvObjIntMapPtr;


class FV_ENTITYDEF_API FvObjStrMap : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend class FvStringMapDataType;
public:
	FvObjStrMap(FvDataType* dataType, bool bDefault=true);
	FvObjStrMap(const FvObjStrMap& src);
	~FvObjStrMap();

	virtual FvObjPtr	Copy() { return new FvObjStrMap(*this); }
	virtual int			GetSize();

	bool operator<( const FvObjStrMap & other ) const;

	FvDataType*			GetElementDataType() const;

	FvUInt32			Size() const { return (FvUInt32)m_kElementRefVector.size(); }
	bool				Get(FvUInt32 idx, FvString& key, FvObjPtr& val) const;
	FvObjPtr			Find(const char* key) const;
	FvObjPtr			Find(const FvString& key) const;
	bool				Empty() const { return m_kElementRefVector.empty(); }

#ifndef FV_SERVER
protected:
#endif
	bool				Insert(const char* key, FvObjPtr pObj);
	bool				Insert(const FvString& key, FvObjPtr pObj);
	bool				Erase(const char* key);
	bool				Erase(const FvString& key);
	void				Clear();
	bool				Set(const char* key, FvObjPtr pObj);
	bool				Set(const FvString& key, FvObjPtr pObj);

protected:
	virtual	void		Attach(FvPropertyOwnerBase* owner);
	virtual	void		Detach();
	bool				CheckElement(FvObjPtr pObj);
	virtual FvInt32		PropertyDivisions();
	virtual FvPropertyOwnerBase* PropertyVassal(FvUInt32 ref);
	virtual void		PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
							FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue);
	void				PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, const FvString& oldKey, FvObjPtr oldVal, const FvString& newKey, FvObjPtr newVal);

protected:
	typedef std::map<FvString,FvObjPtr> ElementMap;
	typedef std::vector<ElementMap::iterator> ElementRefVector;
	ElementMap			m_kElementMap;
	ElementRefVector	m_kElementRefVector;
};
typedef FvSmartPointer<FvObjStrMap> FvObjStrMapPtr;


class FV_ENTITYDEF_API FvObjStructOld : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend class FvStructDataType;
public:
	FvObjStructOld(FvDataType* dataType, bool bDefault=true);
	FvObjStructOld(const FvObjStructOld& src);
	~FvObjStructOld();

	virtual FvObjPtr	Copy() { return new FvObjStructOld(*this); }
	virtual int			GetSize();

	bool operator<( const FvObjStructOld & other ) const;
	FvObjPtr operator[](FvUInt32 idx) const;

	FvUInt32			GetFieldNum() const;
	bool				GetFieldIndex(const char* fieldName, FvUInt32& idx) const;
	bool				GetFieldIndex(const FvString& fieldName, FvUInt32& idx) const;
	bool 				GetFieldName(FvUInt32 index, FvString& name) const;
	FvDataType*			GetFieldDataType(FvUInt32 index) const;

	FvObjPtr			At(FvUInt32 idx) const;
	FvObjPtr			AtName(const char* fieldName) const;
	FvObjPtr			AtName(const FvString& fieldName) const;

#ifndef FV_SERVER
protected:
#endif
	bool				Set(FvUInt32 idx, FvObjPtr pObj);
	bool				SetName(const char* fieldName, FvObjPtr pObj);
	bool				SetName(const FvString& fieldName, FvObjPtr pObj);

protected:
	FvObjStructOld(FvDataType* dataType, FvUInt32 dummy);
	virtual	void		Attach(FvPropertyOwnerBase* owner);
	virtual	void		Detach();
	virtual FvInt32		PropertyDivisions();
	virtual FvPropertyOwnerBase* PropertyVassal(FvUInt32 ref);
	virtual void		PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
							FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue);
	void				PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, FvObjPtr oldVal, FvObjPtr newVal);

protected:
	typedef std::vector<FvObjPtr> ElementVector;
	ElementVector		m_kElementVector;
};
typedef FvSmartPointer<FvObjStructOld> FvObjStructPtr;


class FV_ENTITYDEF_API FvObjMailBoxOld : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend class FvMailBoxDataType;
public:
	FvObjMailBoxOld();
	FvObjMailBoxOld(const FvObjMailBoxOld& src);
	FvObjMailBoxOld(FvDataType* dataType, bool bDefault=true);
	~FvObjMailBoxOld();

	virtual FvObjPtr	Copy() { return new FvObjMailBoxOld(*this); }
	virtual int			GetSize() { AddObjCnt(ms_uiRTTI); return (int)sizeof(FvObjMailBoxOld); }
	bool operator<( const FvObjMailBoxOld & other ) const { return false; }
	bool operator==( const FvObjMailBoxOld& kMailBox ) const;
	bool operator!=( const FvObjMailBoxOld& kMailBox ) const;

	const char*			c_str();

protected:
	FvEntityMailBoxRef	m_kMBRef;
	static char			ms_acBuf[128];
};
typedef FvSmartPointer<FvObjMailBoxOld> FvObjMailBoxPtr;


class FV_ENTITYDEF_API FvObjBlobOld : public FvObj, public FvBinaryOStream, public FvBinaryIStream
{
	FVOBJ_RTTI_DEFINE
public:
	FvObjBlobOld(int iSize=DEFSIZE);
	FvObjBlobOld(const FvObjBlobOld& src);
	FvObjBlobOld(FvDataType* dataType, bool bDefault=true);
	~FvObjBlobOld();

	virtual FvObjPtr	Copy() { return new FvObjBlobOld(*this); }
	virtual int			GetSize();
	bool operator<( const FvObjBlobOld & other ) const { return false; }

	virtual void*		Reserve(int iBytes);
	virtual int			Size() const;

	virtual const void*	Retrieve(int iBytes);
	virtual int			RemainingLength() const;
	virtual char		Peek();

	void*				Data() const;
	void				Reset();
	void				Rewind();

	void				ModifyNotify();	//! 需要调用该接口强制同步属性

	static const int DEFSIZE = 64;

	friend FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjBlobOld& kBlob);
	friend FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjBlobOld& kBlob);

protected:
	char*				m_pcBegin;
	char*				m_pcCurr;
	char*				m_pcEnd;
	char*				m_pcRead;
};

FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjBlobOld& kBlob)
{
	int	iLen = kIS.ReadPackedInt();
	kBlob.Reset();
	kBlob.AddBlob(kIS.Retrieve(iLen), iLen);
	return kIS;
}

FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjBlobOld& kBlob)
{
	kOS.AppendString(static_cast<const char*>(kBlob.Data()), kBlob.Size());
	return kOS;
}


class FV_ENTITYDEF_API FvObjBoolOld : public FvObj
{
	FVOBJ_RTTI_DEFINE
public:
	FvObjBoolOld(FvDataType* dataType, bool bDefault=true);
	FvObjBoolOld(FvDataType* dataType, const bool b, FvUInt8 dummy);
	FvObjBoolOld(const FvObjBoolOld& src):FvObj(src.m_pkDataType), m_kVal(src.m_kVal) {}

	virtual FvObjPtr	Copy() { return new FvObjBoolOld(*this); }
	virtual int			GetSize() { AddObjCnt(ms_uiRTTI); return (int)sizeof(FvObjBoolOld); }

	operator bool() { return m_kVal; }
	bool operator<( const FvObjBoolOld & other ) const;

#ifndef FV_SERVER
protected:
#endif
	FvObjBoolOld& operator=(const bool val);

protected:
	bool	m_kVal;
};
typedef FvSmartPointer<FvObjBoolOld> FvObjBoolOldPtr;


class FV_ENTITYDEF_API FvObjNumFixArrayOld : public FvObj
{
	FVOBJ_RTTI_DEFINE
	friend class FvNumFixArrayDataType;
public:
	FvObjNumFixArrayOld(FvDataType* dataType, bool bDefault=true);
	FvObjNumFixArrayOld(const FvObjNumFixArrayOld& src);
	~FvObjNumFixArrayOld();

	virtual FvObjPtr	Copy() { return new FvObjNumFixArrayOld(*this); }
	virtual int			GetSize();

	bool operator<( const FvObjNumFixArrayOld & other ) const;
	FvObjPtr operator[](FvUInt32 idx) const;

//	FvUInt32			GetFieldNum() const;
//	bool				GetFieldIndex(const char* fieldName, FvUInt32& idx) const;
//	bool				GetFieldIndex(const FvString& fieldName, FvUInt32& idx) const;
//	bool 				GetFieldName(FvUInt32 index, FvString& name) const;
//	FvDataType*			GetFieldDataType(FvUInt32 index) const;
//
//	FvObjPtr			At(FvUInt32 idx) const;
//	FvObjPtr			AtName(const char* fieldName) const;
//	FvObjPtr			AtName(const FvString& fieldName) const;
//
//#ifndef FV_SERVER
//protected:
//#endif
//	bool				Set(FvUInt32 idx, FvObjPtr pObj);
//	bool				SetName(const char* fieldName, FvObjPtr pObj);
//	bool				SetName(const FvString& fieldName, FvObjPtr pObj);

protected:
	//FvObjStructOld(FvDataType* dataType, FvUInt32 dummy);
	//virtual	void		Attach(FvPropertyOwnerBase* owner);
	//virtual	void		Detach();
	//virtual FvInt32		PropertyDivisions();
	//virtual FvPropertyOwnerBase* PropertyVassal(FvUInt32 ref);
	//virtual void		PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
	//	FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue);
	//void				PropertyChangeNotify(FvPropertyOperator op, FvUInt32 opIdx, FvObjPtr oldVal, FvObjPtr newVal);

protected:
	typedef std::vector<FvObjPtr> ElementVector;
	ElementVector		m_kElementVector;
};
typedef FvSmartPointer<FvObjNumFixArrayOld> FvObjNumFixArrayOldPtr;


#ifdef _DEBUG

#define FVOBJ_ASSERT(_POBJ, _TYPE)					\
	FV_ASSERT(_POBJ->RTTI()==_TYPE::ms_uiRTTI)

#define FVOBJ_CONVERT(_POBJ, _CHILDTYPE)			\
	static_cast<_CHILDTYPE*>(_CHILDTYPE::VerifyRTTI(_POBJ))

#define FVOBJ_CONVERT_P(_POBJ, _CHILDTYPE, _PVAR)	\
	_CHILDTYPE* _PVAR = static_cast<_CHILDTYPE*>(_CHILDTYPE::VerifyRTTI(_POBJ))

#define FVOBJ_CONVERT_SP(_POBJ, _CHILDTYPE, _PVAR)	\
	_CHILDTYPE##Ptr _PVAR = static_cast<_CHILDTYPE*>(_CHILDTYPE::VerifyRTTI(_POBJ))

#define FVOBJ_GETVAL(_POBJ, _CHILDTYPE)				\
	(*static_cast<_CHILDTYPE*>(_CHILDTYPE::VerifyRTTI(_POBJ)))

#define FVOBJ_SETVAL(_POBJ, _CHILDTYPE, _VAL)		\
	(*static_cast<_CHILDTYPE*>(_CHILDTYPE::VerifyRTTI(_POBJ))=(_VAL))

#else

#define FVOBJ_ASSERT(_POBJ, _TYPE)					\
	FV_ASSERT(_POBJ->RTTI()==_TYPE::ms_uiRTTI)

#define FVOBJ_CONVERT(_POBJ, _CHILDTYPE)			\
	static_cast<_CHILDTYPE*>(&*_POBJ)

#define FVOBJ_CONVERT_P(_POBJ, _CHILDTYPE, _PVAR)	\
	_CHILDTYPE* _PVAR = static_cast<_CHILDTYPE*>(&*_POBJ)

#define FVOBJ_CONVERT_SP(_POBJ, _CHILDTYPE, _PVAR)	\
	_CHILDTYPE##Ptr _PVAR = static_cast<_CHILDTYPE*>(&*_POBJ)

#define FVOBJ_GETVAL(_POBJ, _CHILDTYPE)				\
	(*static_cast<_CHILDTYPE*>(&*_POBJ))

#define FVOBJ_SETVAL(_POBJ, _CHILDTYPE, _VAL)		\
	(*static_cast<_CHILDTYPE*>(&*_POBJ)=(_VAL))

#endif


#endif//__FvObj_H__