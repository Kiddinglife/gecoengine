//{future header message}
#ifndef __FvDataTypes_H__
#define __FvDataTypes_H__

#include "FvEntityDefDefines.h"
#include "FvObj.h"
#include "FvDataDescription.h"


enum FvDataTypeID_Old
{
	FV_DATATYPEID_INT8,
	FV_DATATYPEID_UINT8,
	FV_DATATYPEID_INT16,
	FV_DATATYPEID_UINT16,
	FV_DATATYPEID_INT32,
	FV_DATATYPEID_UINT32,
	FV_DATATYPEID_INT64,
	FV_DATATYPEID_UINT64,
	FV_DATATYPEID_FLOAT,
	FV_DATATYPEID_DOUBLE,
	FV_DATATYPEID_VECTOR2,
	FV_DATATYPEID_VECTOR3,
	FV_DATATYPEID_VECTOR4,
	FV_DATATYPEID_STRING,
	FV_DATATYPEID_ARRAY,
	FV_DATATYPEID_INTMAP,
	FV_DATATYPEID_STRMAP,
	FV_DATATYPEID_STRUCT,
	FV_DATATYPEID_MAILBOX,
	FV_DATATYPEID_BLOB,
	FV_DATATYPEID_USERDATAOBJ,
	FV_DATATYPEID_BOOL,
	FV_DATATYPEID_NUM_FIXARRAY,
};


class FV_ENTITYDEF_API FvStringDataType : public FvDataType
{
public:
	FvStringDataType( FvMetaDataType * pMeta ) : FvDataType( pMeta ) {}

protected:
	virtual void AddToMD5( FvMD5 & md5 ) const;
	virtual bool operator<( const FvDataType & other ) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const;

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;

private:
	FvObjPtr m_spDefaultValue;
};


class FV_ENTITYDEF_API FvArrayDataType : public FvDataType
{
public:
	FvArrayDataType( FvMetaDataType * pMeta, FvDataTypePtr elementTypePtr, FvUInt32 size, FvUInt32 reserve );

	FvUInt32 GetSize() const
	{
		return m_uiSize;
	}

	FvUInt32 GetReserve() const
	{
		return m_uiReserve;
	}

	FvDataTypePtr GetElemType() const
	{
		return m_spElementTypePtr;
	}

protected:
	virtual FvString TypeName() const;
	virtual void AddToMD5( FvMD5 & md5 ) const;
	virtual bool operator<( const FvDataType & other ) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const;

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;

private:
	FvDataTypePtr	m_spElementTypePtr;
	FvUInt32		m_uiSize;
	FvUInt32		m_uiReserve;
	FvObjPtr		m_spDefaultValue;
};


class FV_ENTITYDEF_API FvStructDataType : public FvDataType
{
public:
	struct Field
	{
		FvString		m_kName;
		FvDataTypePtr	m_spType;
		FvInt32			m_iDBLen;
		bool			m_bIsPersistent;
	};
	typedef std::vector<Field>	Fields;
	typedef Fields::const_iterator Fields_iterator;

	FvStructDataType( FvMetaDataType* pMeta, Fields& fields )
	:FvDataType( pMeta )
	{
		m_kFields.swap( fields );

		for (Fields::const_iterator i = m_kFields.begin(); i != m_kFields.end(); ++i)
			m_kFieldMap[i->m_kName] = i - m_kFields.begin();
	}

	FvInt32 GetFieldIndex(const FvString& fieldName) const
	{
		FieldMap::const_iterator found = m_kFieldMap.find( fieldName );
		return (found != m_kFieldMap.end()) ? found->second : -1;
	}

	FvDataTypePtr GetFieldDataType(FvInt32 index)
	{
		return m_kFields[index].m_spType;
	}

	const FvString& GetFieldName(int index) const
	{
		return m_kFields[index].m_kName;
	}

	Fields::size_type GetNumFields() const { return m_kFields.size(); }
	const Fields& GetFields() const	{ return m_kFields; }

protected:
	virtual FvString TypeName() const;
	virtual void AddToMD5( FvMD5 & md5 ) const;
	virtual bool operator<( const FvDataType & other ) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const;

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;


private:
	typedef std::map<FvString,FvInt32> FieldMap;

	Fields		m_kFields;
	FieldMap	m_kFieldMap;

	FvObjPtr	m_spDefaultValue;
};


class FV_ENTITYDEF_API FvIntMapDataType : public FvDataType
{
public:
	FvIntMapDataType( FvMetaDataType* pMeta, FvDataTypePtr keyTypePtr, FvDataTypePtr elementTypePtr )
	:FvDataType( pMeta )
	,m_spKeyTypePtr( keyTypePtr )
	,m_spElementTypePtr( elementTypePtr )
	{}

	FvDataTypePtr GetKeyType() const
	{
		return m_spKeyTypePtr;
	}

	FvDataTypePtr GetElemType() const
	{
		return m_spElementTypePtr;
	}

protected:
	virtual FvString TypeName() const;
	virtual void AddToMD5( FvMD5 & md5 ) const;
	virtual bool operator<( const FvDataType & other ) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const;

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;

private:
	FvDataTypePtr	m_spKeyTypePtr;
	FvDataTypePtr	m_spElementTypePtr;
	FvObjPtr		m_spDefaultValue;
};


class FV_ENTITYDEF_API FvStringMapDataType : public FvDataType
{
public:
	FvStringMapDataType( FvMetaDataType* pMeta, FvDataTypePtr keyTypePtr, FvDataTypePtr elementTypePtr )
	:FvDataType( pMeta )
	,m_spKeyTypePtr( keyTypePtr )
	,m_spElementTypePtr( elementTypePtr )
	{}

	FvDataTypePtr GetKeyType() const
	{
		return m_spKeyTypePtr;
	}

	FvDataTypePtr GetElemType() const
	{
		return m_spElementTypePtr;
	}

protected:
	virtual FvString TypeName() const;
	virtual void AddToMD5( FvMD5 & md5 ) const;
	virtual bool operator<( const FvDataType & other ) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const;

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;

private:
	FvDataTypePtr	m_spKeyTypePtr;
	FvDataTypePtr	m_spElementTypePtr;
	FvObjPtr		m_spDefaultValue;
};


class FV_ENTITYDEF_API FvMailBoxDataType : public FvDataType
{
	friend class FvMailBoxMetaDataType;
public:
	FvMailBoxDataType(FvMetaDataType* pMetaDataType);

	static FvMailBoxDataType& Instance() { return *ms_pkInstance; }
	static FvMailBoxDataType* pInstance() { return ms_pkInstance; }

protected:
	virtual void AddToMD5(FvMD5& md5) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const { return m_spDefaultValue; }
	virtual FvXMLSectionPtr pDefaultSection() const { return m_spDefaultSection; }

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;

private:
	static	void CreateInstance(FvMetaDataType* pMetaDataType);
	static	void DestroyInstance();
	static FvMailBoxDataType* ms_pkInstance;

	FvObjPtr		m_spDefaultValue;
	FvXMLSectionPtr	m_spDefaultSection;
};


class FV_ENTITYDEF_API FvBlobDataType : public FvDataType
{
	friend class FvBlobMetaDataType;
public:
	FvBlobDataType(FvMetaDataType* pMetaDataType);

	static FvBlobDataType& Instance() { return *ms_pkInstance; }
	static FvBlobDataType* pInstance() { return ms_pkInstance; }

protected:
	virtual void AddToMD5(FvMD5& md5) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const { return m_spDefaultValue; }
	virtual FvXMLSectionPtr pDefaultSection() const { return m_spDefaultSection; }

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;

private:
	static	void CreateInstance(FvMetaDataType* pMetaDataType);
	static	void DestroyInstance();
	static FvBlobDataType* ms_pkInstance;

	FvObjPtr		m_spDefaultValue;
	FvXMLSectionPtr	m_spDefaultSection;
};


class FV_ENTITYDEF_API FvBoolDataType : public FvDataType
{
public:
	FvBoolDataType( FvMetaDataType * pMeta ) : FvDataType( pMeta ) {}

protected:
	virtual void AddToMD5( FvMD5 & md5 ) const;
	virtual bool operator<( const FvDataType & other ) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const;

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;

private:
	FvObjPtr m_spDefaultValue;
};


class FV_ENTITYDEF_API FvNumFixArrayDataType : public FvDataType
{
public:
	struct Field
	{
		FvString		m_kName;
		FvDataTypePtr	m_spType;
		FvInt32			m_iDBLen;
		bool			m_bIsPersistent;
	};
	typedef std::vector<Field>	Fields;
	typedef Fields::const_iterator Fields_iterator;

	FvNumFixArrayDataType( FvMetaDataType* pMeta, Fields& fields )
	:FvDataType( pMeta )
	{
		m_kFields.swap( fields );

		for (Fields::const_iterator i = m_kFields.begin(); i != m_kFields.end(); ++i)
			m_kFieldMap[i->m_kName] = i - m_kFields.begin();
	}

	FvInt32 GetFieldIndex(const FvString& fieldName) const
	{
		FieldMap::const_iterator found = m_kFieldMap.find( fieldName );
		return (found != m_kFieldMap.end()) ? found->second : -1;
	}

	FvDataTypePtr GetFieldDataType(FvInt32 index)
	{
		return m_kFields[index].m_spType;
	}

	const FvString& GetFieldName(int index) const
	{
		return m_kFields[index].m_kName;
	}

	Fields::size_type GetNumFields() const { return m_kFields.size(); }
	const Fields& GetFields() const	{ return m_kFields; }

protected:
	virtual FvString TypeName() const;
	virtual void AddToMD5( FvMD5 & md5 ) const;
	virtual bool operator<( const FvDataType & other ) const;

	virtual void SetDefaultValue( FvXMLSectionPtr pSection );
	virtual FvObjPtr pDefaultValue() const;

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;


private:
	typedef std::map<FvString,FvInt32> FieldMap;

	Fields		m_kFields;
	FieldMap	m_kFieldMap;

	FvObjPtr	m_spDefaultValue;
};


#endif // __FvDataTypes_H__

