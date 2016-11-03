#include "FvDataTypes.h"

#include <FvNetTypes.h>
#include <FvDebug.h>
#include <FvMD5.h>
#include <FvMemoryStream.h>
#include <FvXMLSection.h>

#include <limits>

FV_DECLARE_DEBUG_COMPONENT2( "DataTypes", 0 )

FvInt32 DATA_TYPES_TOKEN = 0;

//extern FvInt32 DATA_TYPES_TOKEN;
//FvInt32* pDATA_TYPES_TOKEN = &DATA_TYPES_TOKEN;


extern bool operator<(FvObjPtr spObj1, FvObjPtr spObj2);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class INT_TYPE, class CLASS_TYPE>
class FvOldSimpleDataType : public FvDataType
{
public:
	FvOldSimpleDataType( FvMetaDataType * pMeta ) : FvDataType( pMeta ) {}

protected:
	virtual void AddToMD5( FvMD5 & md5 ) const
	{
		if (std::numeric_limits<INT_TYPE>::is_signed)
			md5.Append( "Int", sizeof( "Int" ) );
		else
			md5.Append( "UInt", sizeof( "UInt" ) );
		int size = sizeof(INT_TYPE);
		md5.Append( &size, sizeof(int) );
	}
	virtual bool operator<( const FvDataType & other ) const
	{
		if (this->FvDataType::operator<( other )) return true;
		if (other.FvDataType::operator<( *this )) return false;

		const FvOldSimpleDataType& otherInt = static_cast<const FvOldSimpleDataType&>(other);
		return m_spDefaultValue < otherInt.m_spDefaultValue;
	}

	virtual void SetDefaultValue( FvXMLSectionPtr pSection )
	{
		m_spDefaultValue = pSection ? this->CreateFromSection( pSection ) : new CLASS_TYPE(const_cast<FvOldSimpleDataType*>(this), INT_TYPE(0));
	}
	virtual FvObjPtr pDefaultValue() const
	{
		return m_spDefaultValue;
	}

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
	{
		INT_TYPE value = FVOBJ_GETVAL(pValue, CLASS_TYPE);
		stream << value;
	}
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
	{
		INT_TYPE value(0);
		stream >> value;

		FvObj* pObj = new CLASS_TYPE(const_cast<FvOldSimpleDataType*>(this), value);
		return pObj;
	}
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
	{
		INT_TYPE value = FVOBJ_GETVAL(pValue, CLASS_TYPE);
		pSection->Set<INT_TYPE>(value);
	}
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const
	{
		INT_TYPE value = pSection->As<INT_TYPE>(0);
		return new CLASS_TYPE(const_cast<FvOldSimpleDataType*>(this), value);
	}

private:
	FvObjPtr	m_spDefaultValue;
};

template <class DATATYPE>
class FvSimpleMetaDataType : public FvMetaDataType
{
public:
	FvSimpleMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvMetaDataType::AddMetaType( this );
	}
	virtual ~FvSimpleMetaDataType()
	{
		FvMetaDataType::DelMetaType( this );
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		return new DATATYPE( this );
	}
};

#define FV_SIMPLE_DATA_TYPE( TYPE, NAME, ID ) \
	FvSimpleMetaDataType< TYPE > s_##NAME##_metaDataType( #NAME, ID );

#define SIMPLE_DATA_TYPE_FORTEMPLET( TYPE, CLASS, TYPEID, NAME ) \
	FvSimpleMetaDataType< FvOldSimpleDataType< TYPE, CLASS > > s_##NAME##_metaDataType( #NAME, TYPEID )

SIMPLE_DATA_TYPE_FORTEMPLET(FvUInt8, FvObjUInt8Old, FV_DATATYPEID_UINT8, UINT8);
SIMPLE_DATA_TYPE_FORTEMPLET(FvUInt16, FvObjUInt16Old, FV_DATATYPEID_UINT16, UINT16);
SIMPLE_DATA_TYPE_FORTEMPLET(FvUInt32, FvObjUInt32Old, FV_DATATYPEID_UINT32, UINT32);
SIMPLE_DATA_TYPE_FORTEMPLET(FvUInt64, FvObjUInt64Old, FV_DATATYPEID_UINT64, UINT64);

SIMPLE_DATA_TYPE_FORTEMPLET(FvInt8, FvObjInt8Old, FV_DATATYPEID_INT8, INT8);
SIMPLE_DATA_TYPE_FORTEMPLET(FvInt16, FvObjInt16Old, FV_DATATYPEID_INT16, INT16);
SIMPLE_DATA_TYPE_FORTEMPLET(FvInt32, FvObjInt32Old, FV_DATATYPEID_INT32, INT32);
SIMPLE_DATA_TYPE_FORTEMPLET(FvInt64, FvObjInt64Old, FV_DATATYPEID_INT64, INT64);

SIMPLE_DATA_TYPE_FORTEMPLET(float, FvObjFloatOld, FV_DATATYPEID_FLOAT, FLOAT32);
SIMPLE_DATA_TYPE_FORTEMPLET(double, FvObjDoubleOld, FV_DATATYPEID_DOUBLE, FLOAT64);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FvStringDataType::AddToMD5( FvMD5 & md5 ) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
}
bool FvStringDataType::operator<( const FvDataType & other ) const
{
	if (this->FvDataType::operator<( other )) return true;
	if (other.FvDataType::operator<( *this )) return false;

	const FvStringDataType& otherStr = static_cast<const FvStringDataType&>(other);
	return m_spDefaultValue < otherStr.m_spDefaultValue;
}

void FvStringDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	m_spDefaultValue = pSection ? this->CreateFromSection( pSection ) : new FvObjStringOld(this, "");
}
FvObjPtr FvStringDataType::pDefaultValue() const
{
	return m_spDefaultValue;
}

void FvStringDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	FvString s = FVOBJ_GETVAL(pValue, FvObjStringOld);
	stream << s;
}
FvObjPtr FvStringDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	FvString value;
	stream >> value;

	FvObj* pObj = new FvObjStringOld(const_cast<FvStringDataType*>(this), value);
	return pObj;
}
void FvStringDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	FvString s = FVOBJ_GETVAL(pValue, FvObjStringOld);
	pSection->SetString(s);
}
FvObjPtr FvStringDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	FvString value;
	value = pSection->AsString();
	return new FvObjStringOld(const_cast<FvStringDataType*>(this), value);
}

FV_SIMPLE_DATA_TYPE( FvStringDataType, STRING, FV_DATATYPEID_STRING )

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define VECTOR_DATA_TYPE_HELPERS( VECTOR )									\
void FvFromSectionToVector( FvXMLSectionPtr pSection, FvObj##VECTOR##Old & obj )	\
{																			\
	Fv##VECTOR v = pSection->As##VECTOR();									\
	obj = v;																\
}																			\
																			\
void FvFromVectorToSection( FvObj##VECTOR##Old & obj, FvXMLSectionPtr pSection )	\
{																			\
	Fv##VECTOR v = (Fv##VECTOR)obj;											\
	pSection->Set##VECTOR( v );												\
}

VECTOR_DATA_TYPE_HELPERS( Vector2 )
VECTOR_DATA_TYPE_HELPERS( Vector3 )
VECTOR_DATA_TYPE_HELPERS( Vector4 )

template <class VECTOR, class VECTORDATA>
class FvVectorDataType : public FvDataType
{
public:
	FvVectorDataType( FvMetaDataType * pMeta )
	:FvDataType( pMeta ) {}

protected:
	virtual void AddToMD5( FvMD5 & md5 ) const
	{
		md5.Append( "Vector", sizeof( "Vector" ) );
		md5.Append( &NUM_ELEMENTS, sizeof(FvInt32) );
	}
	virtual bool operator<( const FvDataType & other ) const
	{
		if (this->FvDataType::operator<( other )) return true;
		if (other.FvDataType::operator<( *this )) return false;

		const FvVectorDataType& otherVec = static_cast< const FvVectorDataType& >( other );
		return m_spDefaultValue < otherVec.m_spDefaultValue;
	}

	virtual void SetDefaultValue( FvXMLSectionPtr pSection )
	{
		m_spDefaultValue = pSection ? CreateFromSection(pSection) : new VECTOR(const_cast<FvVectorDataType*>(this), false);
	}
	virtual FvObjPtr pDefaultValue() const
	{
		return m_spDefaultValue;
	}

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
	{
		int len = sizeof(float)*NUM_ELEMENTS;
		float* pfloat = FVOBJ_GETVAL(pValue, VECTOR);
		char* pBuf = static_cast<char*>(stream.Reserve(len));
		FvCrazyCopy(pBuf, pfloat, len);
	}
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
	{
		int len = sizeof(float)*NUM_ELEMENTS;
		const float* pSrc = static_cast<const float*>(stream.Retrieve(len));
		return new VECTOR(const_cast<FvVectorDataType*>(this), pSrc);
	}
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
	{
		FVOBJ_CONVERT_P(pValue, VECTOR, pObj);
		FvFromVectorToSection(*pObj, pSection);
	}
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const
	{
		VECTOR* pObj = new VECTOR(const_cast<FvVectorDataType*>(this), false);
		FvFromSectionToVector(pSection, *pObj);
		return pObj;
	}

private:
	static const FvInt32 NUM_ELEMENTS;
	FvObjPtr	m_spDefaultValue;
};

template <class VECTOR, class VECTORDATA>
const FvInt32 FvVectorDataType< VECTOR, VECTORDATA >::NUM_ELEMENTS = sizeof( VECTORDATA )/sizeof( float );

#define SIMPLE_DATA_TYPE_FORVECTOR( CLASS, VECTOR, TYPEID, NAME ) \
	FvSimpleMetaDataType< FvVectorDataType< CLASS, VECTOR > > s_##NAME##_metaDataType( #NAME, TYPEID )

SIMPLE_DATA_TYPE_FORVECTOR(FvObjVector2Old, FvVector2, FV_DATATYPEID_VECTOR2, VECTOR2);
SIMPLE_DATA_TYPE_FORVECTOR(FvObjVector3Old, FvVector3, FV_DATATYPEID_VECTOR3, VECTOR3);
SIMPLE_DATA_TYPE_FORVECTOR(FvObjVector4Old, FvVector4, FV_DATATYPEID_VECTOR4, VECTOR4);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvArrayDataType::FvArrayDataType( FvMetaDataType * pMeta, FvDataTypePtr elementTypePtr, FvUInt32 size, FvUInt32 reserve )
:FvDataType( pMeta )
,m_spElementTypePtr( elementTypePtr )
,m_uiSize( size )
,m_uiReserve(reserve)
{
	if(m_uiSize)
	{
		FvObjArrayOld* pArray = new FvObjArrayOld(this, false);
		for(FvUInt32 i = 0; i < m_uiSize; ++i)
		{
			FvObjPtr pElement = m_spElementTypePtr->pDefaultValue()->Copy();
			pElement->SetRef(i);
			pArray->m_kElementVector.push_back(pElement);
		}
		m_spDefaultValue = pArray;
	}
	else
	{
		m_spDefaultValue = new FvObjArrayOld(this, false);
	}
}

FvString FvArrayDataType::TypeName() const
{
	return FvDataType::TypeName() +" of "+ m_spElementTypePtr->TypeName();
}
void FvArrayDataType::AddToMD5( FvMD5 & md5 ) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
	md5.Append(&m_uiSize, sizeof(m_uiSize));
	m_spElementTypePtr->AddToMD5(md5);
}
bool FvArrayDataType::operator<( const FvDataType & other ) const
{
	if (this->FvDataType::operator<( other )) return true;
	if (other.FvDataType::operator<( *this )) return false;

	FvArrayDataType & otherSeq = (FvArrayDataType&)other;
	if (m_uiSize < otherSeq.m_uiSize) return true;
	if (otherSeq.m_uiSize < m_uiSize) return false;

	if(*m_spElementTypePtr < *otherSeq.m_spElementTypePtr) return true;
	if(*otherSeq.m_spElementTypePtr < *m_spElementTypePtr) return false;

	return m_spDefaultValue < otherSeq.m_spDefaultValue;
}

void FvArrayDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	if(pSection) m_spDefaultValue = CreateFromSection(pSection);
}
FvObjPtr FvArrayDataType::pDefaultValue() const
{
	return m_spDefaultValue;
}

void FvArrayDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjArrayOld, pArray);
	FvInt32 cnt = pArray->m_kElementVector.size();
	stream << cnt;

	for(FvInt32 i=0; i<cnt; ++i)
	{
		m_spElementTypePtr->AddToStream(pArray->m_kElementVector[i], stream, isPersistentOnly);
	}
}
FvObjPtr FvArrayDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	FvInt32 size(0);
	stream >> size;

	if (stream.Error())
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Missing size parameter on stream\n" );
		return NULL;
	}

	if (stream.RemainingLength() < size || size < 0)
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Invalid size on stream: %d "
			"(%d bytes remaining)\n",
			size, stream.RemainingLength() );
		stream.Error( true );
		return NULL;
	}

	FvObjArrayOld* pArray = new FvObjArrayOld(const_cast<FvArrayDataType*>(this), false);

	for (FvUInt32 i = 0; i < (FvUInt32)size; ++i)
	{
		FvObjPtr pElement = m_spElementTypePtr->CreateFromStream( stream, isPersistentOnly );
		if (pElement)
		{
			pElement->SetRef(i);
			pArray->m_kElementVector.push_back(pElement);
		}
		else
		{
			stream.Error( true );
			break;
		}
	}

	if (stream.Error())
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Insufficient data on stream To create %d "
			"elements\n", size );
		return NULL;
	}

	return pArray;
}
void FvArrayDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjArrayOld, pArray);
	FvInt32 cnt = pArray->m_kElementVector.size();

	for(FvInt32 i=0; i<cnt; ++i)
	{
		FvXMLSectionPtr itemSection = pSection->NewSection( "item" );
		m_spElementTypePtr->AddToSection(pArray->m_kElementVector[i], itemSection);
	}
}
FvObjPtr FvArrayDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	if(!pSection)
	{
		FV_ERROR_MSG( "ArrayDataType::CreateFromSection: "
			"Section is NULL.\n" );
		return NULL;
	}

	FvObjArrayOld* pArray = new FvObjArrayOld(const_cast<FvArrayDataType*>(this), false);

	if(m_uiSize)
	{
		for(FvUInt32 i = 0; i < m_uiSize; ++i)
		{
			FvObjPtr pElement = m_spElementTypePtr->pDefaultValue()->Copy();
			pElement->SetRef(i);
			pArray->m_kElementVector.push_back(pElement);
		}
	}
	else
	{
		FvUInt32 size = pSection->CountChildren();

		for(FvUInt32 i = 0; i < size; ++i)
		{
			FvObjPtr pElement = m_spElementTypePtr->CreateFromSection(pSection->OpenChild(i));
			pElement->SetRef(i);
			pArray->m_kElementVector.push_back(pElement);
		}
	}

	return pArray;
}

class FvArrayMetaDataType : public FvMetaDataType
{
public:
	FvArrayMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvMetaDataType::AddMetaType(this);
	}

	virtual ~FvArrayMetaDataType()
	{
		FvMetaDataType::DelMetaType(this);
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		FvUInt32 size = pSection->ReadInt( "size", 0 );
		FvUInt32 reserve = pSection->ReadInt( "reserve", 0 );
		if(size > reserve)
			reserve = size;
		FvDataTypePtr pOfType =	FvDataType::BuildDataType( pSection->OpenSection( "of" ) );

		if (pOfType)
		{
			return new FvArrayDataType(this, pOfType, size, reserve);
		}

		FV_ERROR_MSG( "FvSequenceMetaDataType::GetType: "
			"Unable To create sequence of '%s'\n",
				pSection->ReadString( "of" ).c_str() );

		return NULL;
	}
};

static FvArrayMetaDataType s_ARRAY_metaDataType("ARRAY", FV_DATATYPEID_ARRAY);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvString FvStructDataType::TypeName() const
{
	FvString acc = FvDataType::TypeName();
	for (Fields::const_iterator it = m_kFields.begin(); it != m_kFields.end(); ++it)
	{
		acc += (it == m_kFields.begin()) ? " props " : ", ";
		acc += it->m_kName + ":(" + it->m_spType->TypeName() + ")";
	}
	return acc;
}
void FvStructDataType::AddToMD5( FvMD5 & md5 ) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
	for (Fields::const_iterator it = m_kFields.begin(); it != m_kFields.end(); ++it)
	{
		md5.Append( it->m_kName.data(), it->m_kName.size() );
		it->m_spType->AddToMD5( md5 );
	}
}
bool FvStructDataType::operator<( const FvDataType & other ) const
{
	if (this->FvDataType::operator<( other )) return true;
	if (other.FvDataType::operator<( *this )) return false;

	FvStructDataType & otherStruct = (FvStructDataType&)other;
	if(m_kFields.size() < otherStruct.m_kFields.size()) return true;
	if(otherStruct.m_kFields.size() < m_kFields.size()) return false;

	for (FvUInt32 i = 0; i < m_kFields.size(); ++i)
	{
		FvInt32 res = m_kFields[i].m_kName.compare( otherStruct.m_kFields[i].m_kName );
		if (res < 0) return true;
		if (res > 0) return false;

		if ((*m_kFields[i].m_spType) < (*otherStruct.m_kFields[i].m_spType)) return true;
		if ((*otherStruct.m_kFields[i].m_spType) < (*m_kFields[i].m_spType)) return false;
	}

	return m_spDefaultValue < otherStruct.m_spDefaultValue;
}

void FvStructDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	m_spDefaultValue = pSection ? CreateFromSection(pSection) : new FvObjStructOld(this, false);
}
FvObjPtr FvStructDataType::pDefaultValue() const
{
	return m_spDefaultValue;
}

void FvStructDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjStructOld, pStruct);

	for(FvUInt32 i=0; i<m_kFields.size(); ++i)
	{
		m_kFields[i].m_spType->AddToStream(pStruct->m_kElementVector[i], stream, isPersistentOnly);
	}
}
FvObjPtr FvStructDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	FvObjStructOld* pStruct = new FvObjStructOld(const_cast<FvStructDataType*>(this), FvUInt32(1));
	pStruct->m_kElementVector.resize(m_kFields.size());

	for (FvUInt32 i = 0; i < m_kFields.size(); ++i)
	{
		FvObjPtr pElement = m_kFields[i].m_spType->CreateFromStream( stream, isPersistentOnly );
		if (pElement)
		{
			pElement->SetRef(i);
			pStruct->m_kElementVector[i] = pElement;
		}
		else
		{
			stream.Error( true );
			break;
		}
	}

	if (stream.Error())
	{
		return NULL;
	}

	return pStruct;
}
void FvStructDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjStructOld, pStruct);
	FvInt32 cnt = m_kFields.size();

	for(int i=0; i<cnt; ++i)
	{
		FvXMLSectionPtr itemSection = pSection->NewSection( m_kFields[i].m_kName );
		m_kFields[i].m_spType->AddToSection(pStruct->m_kElementVector[i], itemSection);
	}
}
FvObjPtr FvStructDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	if(!pSection)
	{
		FV_ERROR_MSG( "FvStructDataType::CreateFromSection: "
			"Section is NULL.\n" );
		return NULL;
	}

	FvObjStructOld* pStruct = new FvObjStructOld(const_cast<FvStructDataType*>(this), FvUInt32(1));
	pStruct->m_kElementVector.resize(m_kFields.size());

	for(FvUInt32 i = 0; i < m_kFields.size(); ++i)
	{
		FvXMLSectionPtr pChildSection = pSection->OpenSection( m_kFields[i].m_kName );
		if (pChildSection)
		{
			FvObjPtr pValue = m_kFields[i].m_spType->CreateFromSection( pChildSection );
			if (!pValue)
				return NULL;	// error already printed

			pValue->SetRef(i);
			pStruct->m_kElementVector[i] = pValue;
		}
		else
		{
			pStruct->m_kElementVector[i] = m_kFields[i].m_spType->pDefaultValue()->Copy();
		}
	}

	return pStruct;
}

class FvStructMetaDataType : public FvMetaDataType
{
public:
	FvStructMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvMetaDataType::AddMetaType( this );
	}

	virtual ~FvStructMetaDataType()
	{
		FvMetaDataType::DelMetaType( this );
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		FvStructDataType::Fields fields;
/**
		FvXMLSectionPtr pParentSect = pSection->OpenSection( "Parent" );
		if (pParentSect)
		{
			FvDataTypePtr pParent = FvDataType::BuildDataType( pParentSect );
			if (!pParent || pParent->pMetaDataType() != this)
			{
				FV_ERROR_MSG( "<Parent> of %s must also be a %s\n", Name(), Name() );
				return NULL;
			}

			FvStructDataType* pParentStruct = static_cast<FvStructDataType*>(&*pParent);
			fields = pParentStruct->GetFields();
		}
**/
		FvXMLSectionPtr pProps = pSection->OpenSection( "Properties" );
		if((!pProps || pProps->CountChildren() == 0) && fields.empty())
		{
			FV_ERROR_MSG( "<Properties> section missing or empty in %s type spec\n",
				Name() );
			return NULL;
		}

		for(FvXMLSectionIterator it = pProps->Begin(); it != pProps->End(); ++it)
		{
			if((*it)->GetType() != TiXmlNode::ELEMENT)
				continue;

			FvStructDataType::Field f;
			f.m_kName = (*it)->SectionName();

			FvStructDataType::Fields::iterator itr = fields.begin();
			while(itr != fields.end())
			{
				if(itr->m_kName.compare(f.m_kName) == 0)
				{
					FV_ERROR_MSG( "Find same property '%s' in %s type\n",
						f.m_kName.c_str(), Name() );
					return NULL;
				}
				++itr;
			}

			//f.m_spType = FvDataType::BuildDataType( (*it)->OpenSection( "Type" ) );
			f.m_spType = FvDataType::BuildDataType( (*it) );//! 去除Type标签
			if (!f.m_spType)
			{
				FV_ERROR_MSG( "Could not build type for property '%s' in %s type\n",
					f.m_kName.c_str(), Name() );
				return NULL;
			}

			f.m_iDBLen = (*it)->ReadInt( "DatabaseLength", FV_DEFAULT_DATABASE_LENGTH );
			f.m_bIsPersistent = (*it)->ReadBool( "Persistent", true );

			fields.push_back( f );
		}

		FV_ASSERT_DEV( !fields.empty() );

		return new FvStructDataType( this, fields );
	}
};

static FvStructMetaDataType s_STRUCT_metaDataType("STRUCT", FV_DATATYPEID_STRUCT);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvString FvIntMapDataType::TypeName() const
{
	return FvDataType::TypeName() +" of "+ m_spElementTypePtr->TypeName();
}
void FvIntMapDataType::AddToMD5( FvMD5 & md5 ) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
	m_spElementTypePtr->AddToMD5(md5);
}
bool FvIntMapDataType::operator<( const FvDataType & other ) const
{
	if (this->FvDataType::operator<( other )) return true;
	if (other.FvDataType::operator<( *this )) return false;

	FvIntMapDataType & otherSeq = (FvIntMapDataType&)other;
	if(*m_spElementTypePtr < *otherSeq.m_spElementTypePtr) return true;
	if(*otherSeq.m_spElementTypePtr < *m_spElementTypePtr) return false;

	return m_spDefaultValue < otherSeq.m_spDefaultValue;
}

void FvIntMapDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	m_spDefaultValue = pSection ? CreateFromSection(pSection) : new FvObjIntMap(this, false);
}
FvObjPtr FvIntMapDataType::pDefaultValue() const
{
	return m_spDefaultValue;
}

void FvIntMapDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjIntMap, pMap);
	FvInt32 cnt = pMap->m_kElementRefVector.size();
	stream << cnt;
	FvObjIntMap::ElementRefVector::iterator itrB = pMap->m_kElementRefVector.begin();
	FvObjIntMap::ElementRefVector::iterator itrE = pMap->m_kElementRefVector.end();
	while(itrB != itrE)
	{
		stream << (*itrB)->first;
		m_spElementTypePtr->AddToStream((*itrB)->second, stream, isPersistentOnly);
		++itrB;
	}
}
FvObjPtr FvIntMapDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	FvInt32 size(0);
	stream >> size;

	if (stream.Error())
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Missing size parameter on stream\n" );
		return NULL;
	}

	if (stream.RemainingLength() < size || size < 0)
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Invalid size on stream: %d "
			"(%d bytes remaining)\n",
			size, stream.RemainingLength() );
		stream.Error( true );
		return NULL;
	}

	FvObjIntMap* pMap = new FvObjIntMap(const_cast<FvIntMapDataType*>(this), false);
	FvInt32 id(0);
	for (FvUInt32 i = 0; i < (FvUInt32)size; ++i)
	{
		stream >> id;
		FvObjPtr pElement = m_spElementTypePtr->CreateFromStream( stream, isPersistentOnly );
		if (pElement)
		{
			pElement->SetRef(i);
			std::pair<FvObjIntMap::ElementMap::iterator, bool> ret =
				pMap->m_kElementMap.insert(std::make_pair(id, pElement));
			FV_ASSERT(ret.second);
			pMap->m_kElementRefVector.push_back(ret.first);
		}
		else
		{
			stream.Error( true );
			break;
		}
	}

	if (stream.Error())
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Insufficient data on stream to create %d "
			"elements\n", size );
		return NULL;
	}

	return pMap;
}
void FvIntMapDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjIntMap, pMap);

	FvObjIntMap::ElementRefVector::iterator itrB = pMap->m_kElementRefVector.begin();
	FvObjIntMap::ElementRefVector::iterator itrE = pMap->m_kElementRefVector.end();
	while(itrB != itrE)
	{
		FvXMLSectionPtr itemSection = pSection->NewSection( "item" );
		FvXMLSectionPtr itemKeySection = itemSection->NewSection( "Key" );
		FvXMLSectionPtr itemValSection = itemSection->NewSection( "Val" );
		itemKeySection->SetInt((*itrB)->first);
		m_spElementTypePtr->AddToSection((*itrB)->second, itemValSection);
		++itrB;
	}
}
FvObjPtr FvIntMapDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	if (!pSection)
	{
		FV_ERROR_MSG( "ArrayDataType::CreateFromSection: "
			"Section is NULL.\n" );
		return NULL;
	}

	FvUInt32 size = pSection->CountChildren();

	FvObjIntMap* pMap = new FvObjIntMap(const_cast<FvIntMapDataType*>(this), false);
	FvInt32 id(0);

	for (FvUInt32 i = 0; i < size; i++)
	{
		FvXMLSectionPtr pItemSection = pSection->OpenChild(i);
		FvXMLSectionPtr pKey = pItemSection->OpenSection("Key");
		id = pKey->AsInt();
		FvObjPtr pElement = m_spElementTypePtr->CreateFromSection( pItemSection->OpenSection("Val") );

		pElement->SetRef(i);
		std::pair<FvObjIntMap::ElementMap::iterator, bool> ret =
			pMap->m_kElementMap.insert(std::make_pair(id, pElement));
		FV_ASSERT(ret.second);
		pMap->m_kElementRefVector.push_back(ret.first);
	}

	return pMap;
}

class FvIntMapMetaDataType : public FvMetaDataType
{
public:
	FvIntMapMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvMetaDataType::AddMetaType( this );
	}

	virtual ~FvIntMapMetaDataType()
	{
		FvMetaDataType::DelMetaType( this );
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		FvXMLSectionPtr pKeySection = FvXMLSection::CreateSection("Type");
		pKeySection->SetString("INT32");
		FvDataTypePtr pKeyType = FvDataType::BuildDataType( pKeySection );
		FvDataTypePtr pOfType =	FvDataType::BuildDataType( pSection->OpenSection( "of" ) );

		if (pOfType)
		{
			return new FvIntMapDataType(this, pKeyType, pOfType);
		}

		FV_ERROR_MSG( "FvIntMapMetaDataType::GetType: "
			"Unable to create sequence of '%s'\n",
			pSection->ReadString( "of" ).c_str() );

		return NULL;
	}
};

//! 屏蔽Map类
//static FvIntMapMetaDataType s_INT_MAP_metaDataType("INTMAP", FV_DATATYPEID_INTMAP);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvString FvStringMapDataType::TypeName() const
{
	return FvDataType::TypeName() +" of "+ m_spElementTypePtr->TypeName();
}
void FvStringMapDataType::AddToMD5( FvMD5 & md5 ) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
	m_spElementTypePtr->AddToMD5(md5);
}
bool FvStringMapDataType::operator<( const FvDataType & other ) const
{
	if (this->FvDataType::operator<( other )) return true;
	if (other.FvDataType::operator<( *this )) return false;

	FvStringMapDataType & otherSeq = (FvStringMapDataType&)other;
	if(*m_spElementTypePtr < *otherSeq.m_spElementTypePtr) return true;
	if(*otherSeq.m_spElementTypePtr < *m_spElementTypePtr) return false;

	return m_spDefaultValue < otherSeq.m_spDefaultValue;
}

void FvStringMapDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	m_spDefaultValue = pSection ? CreateFromSection(pSection) : new FvObjStrMap(this, false);
}
FvObjPtr FvStringMapDataType::pDefaultValue() const
{
	return m_spDefaultValue;
}

void FvStringMapDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjStrMap, pMap);
	FvInt32 cnt = pMap->m_kElementRefVector.size();
	stream << cnt;
	FvObjStrMap::ElementRefVector::iterator itrB = pMap->m_kElementRefVector.begin();
	FvObjStrMap::ElementRefVector::iterator itrE = pMap->m_kElementRefVector.end();
	while(itrB != itrE)
	{
		stream << (*itrB)->first;
		m_spElementTypePtr->AddToStream((*itrB)->second, stream, isPersistentOnly);
		++itrB;
	}
}
FvObjPtr FvStringMapDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	FvInt32 size(0);
	stream >> size;

	if (stream.Error())
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Missing size parameter on stream\n" );
		return NULL;
	}

	if (stream.RemainingLength() < size || size < 0)
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Invalid size on stream: %d "
			"(%d bytes remaining)\n",
			size, stream.RemainingLength() );
		stream.Error( true );
		return NULL;
	}

	FvObjStrMap* pMap = new FvObjStrMap(const_cast<FvStringMapDataType*>(this), false);
	for (FvUInt32 i = 0; i < (FvUInt32)size; ++i)
	{
		FvString key;
		stream >> key;
		FvObjPtr pElement = m_spElementTypePtr->CreateFromStream( stream, isPersistentOnly );
		if (pElement)
		{
			pElement->SetRef(i);
			std::pair<FvObjStrMap::ElementMap::iterator, bool> ret =
				pMap->m_kElementMap.insert(std::make_pair(key, pElement));
			FV_ASSERT(ret.second);
			pMap->m_kElementRefVector.push_back(ret.first);
		}
		else
		{
			stream.Error( true );
			break;
		}
	}

	if (stream.Error())
	{
		FV_ERROR_MSG( "SequenceDataType::CreateFromStream: "
			"Insufficient data on stream to create %d "
			"elements\n", size );
		return NULL;
	}

	return pMap;
}
void FvStringMapDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjStrMap, pMap);

	FvObjStrMap::ElementRefVector::iterator itrB = pMap->m_kElementRefVector.begin();
	FvObjStrMap::ElementRefVector::iterator itrE = pMap->m_kElementRefVector.end();
	while(itrB != itrE)
	{
		FvXMLSectionPtr itemSection = pSection->NewSection( "item" );
		FvXMLSectionPtr itemKeySection = itemSection->NewSection( "Key" );
		FvXMLSectionPtr itemValSection = itemSection->NewSection( "Val" );
		itemKeySection->SetString((*itrB)->first);
		m_spElementTypePtr->AddToSection((*itrB)->second, itemValSection);
		++itrB;
	}
}
FvObjPtr FvStringMapDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	if (!pSection)
	{
		FV_ERROR_MSG( "ArrayDataType::CreateFromSection: "
			"Section is NULL.\n" );
		return NULL;
	}

	FvUInt32 size = pSection->CountChildren();

	FvObjStrMap* pMap = new FvObjStrMap(const_cast<FvStringMapDataType*>(this), false);

	for (FvUInt32 i = 0; i < size; i++)
	{
		FvXMLSectionPtr pItemSection = pSection->OpenChild(i);
		FvXMLSectionPtr pKey = pItemSection->OpenSection("Key");
		FvString key = pKey->AsString();
		FvObjPtr pElement = m_spElementTypePtr->CreateFromSection( pItemSection->OpenSection("Val") );

		pElement->SetRef(i);
		std::pair<FvObjStrMap::ElementMap::iterator, bool> ret =
			pMap->m_kElementMap.insert(std::make_pair(key, pElement));
		FV_ASSERT(ret.second);
		pMap->m_kElementRefVector.push_back(ret.first);
	}

	return pMap;
}

class FvStringMapMetaDataType : public FvMetaDataType
{
public:
	FvStringMapMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvMetaDataType::AddMetaType( this );
	}

	virtual ~FvStringMapMetaDataType()
	{
		FvMetaDataType::DelMetaType( this );
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		FvXMLSectionPtr pKeySection = FvXMLSection::CreateSection("Type");
		pKeySection->SetString("STRING");
		FvDataTypePtr pKeyType = FvDataType::BuildDataType( pKeySection );
		FvDataTypePtr pOfType = FvDataType::BuildDataType( pSection->OpenSection( "of" ) );

		if (pOfType)
		{
			return new FvStringMapDataType(this, pKeyType, pOfType);
		}

		FV_ERROR_MSG( "FvStringMapMetaDataType::GetType: "
			"Unable to create sequence of '%s'\n",
			pSection->ReadString( "of" ).c_str() );

		return NULL;
	}
};

//! 屏蔽Map类
//static FvStringMapMetaDataType s_STRING_MAP_metaDataType("STRMAP", FV_DATATYPEID_STRMAP);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvMailBoxDataType::FvMailBoxDataType(FvMetaDataType* pMetaDataType)
:FvDataType(pMetaDataType)
,m_spDefaultValue(NULL)
,m_spDefaultSection(NULL)
{
}

void FvMailBoxDataType::AddToMD5(FvMD5& md5) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
}

void FvMailBoxDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	if(!m_spDefaultValue)
	{
		m_spDefaultValue = new FvObjMailBoxOld();

		m_spDefaultSection = new FvXMLSection( "Default" );
		AddToSection( m_spDefaultValue, m_spDefaultSection );
	}
}

void FvMailBoxDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjMailBoxOld, pMB);
	stream << pMB->m_kMBRef;
}

FvObjPtr FvMailBoxDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	FvObjMailBoxOld* pMB = new FvObjMailBoxOld();
	stream >> pMB->m_kMBRef;
	return pMB;
}

void FvMailBoxDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjMailBoxOld, pMB);

	FvXMLSectionPtr ipSection = pSection->NewSection( "IP" );
	FvXMLSectionPtr portSection = pSection->NewSection( "Port" );
	FvXMLSectionPtr idSection = pSection->NewSection( "ID" );
	FvXMLSectionPtr typeSection = pSection->NewSection( "Type" );
	FvXMLSectionPtr componentSection = pSection->NewSection( "Component" );

	ipSection->SetInt(pMB->m_kMBRef.m_kAddr.m_uiIP);
	portSection->SetInt(pMB->m_kMBRef.m_kAddr.m_uiPort);
	idSection->SetInt(pMB->m_kMBRef.m_iID);
	typeSection->SetInt(pMB->m_kMBRef.GetType());
	componentSection->SetInt(pMB->m_kMBRef.GetComponent());
}

FvObjPtr FvMailBoxDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	FvObjMailBoxOld* pMB = new FvObjMailBoxOld();

	FvXMLSectionPtr ipSection = pSection->OpenSection( "IP" );
	FvXMLSectionPtr portSection = pSection->OpenSection( "Port" );
	FvXMLSectionPtr idSection = pSection->OpenSection( "ID" );
	FvXMLSectionPtr typeSection = pSection->OpenSection( "Type" );
	FvXMLSectionPtr componentSection = pSection->OpenSection( "Component" );

	if(ipSection)	pMB->m_kMBRef.m_kAddr.m_uiIP = ipSection->AsInt();
	if(portSection)	pMB->m_kMBRef.m_kAddr.m_uiPort = portSection->AsInt();
	if(idSection)	pMB->m_kMBRef.m_iID = idSection->AsInt();
	if(typeSection)	pMB->m_kMBRef.SetType(typeSection->AsInt());
	if(componentSection)
		pMB->m_kMBRef.SetComponent((FvEntityMailBoxRef::Component)componentSection->AsInt());

	return pMB;
}

FvMailBoxDataType* FvMailBoxDataType::ms_pkInstance = NULL;

void FvMailBoxDataType::CreateInstance(FvMetaDataType* pMetaDataType)
{
	FV_ASSERT(!ms_pkInstance);
	ms_pkInstance = new FvMailBoxDataType(pMetaDataType);
	ms_pkInstance->IncRef();
}

void FvMailBoxDataType::DestroyInstance()
{
	FV_ASSERT(ms_pkInstance);
	delete ms_pkInstance;
	ms_pkInstance = NULL;
}

class FvMailBoxMetaDataType : public FvMetaDataType
{
public:
	FvMailBoxMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvMailBoxDataType::CreateInstance(this);
		FvMetaDataType::AddMetaType(this);
	}

	virtual ~FvMailBoxMetaDataType()
	{
		FvMetaDataType::DelMetaType(this);
		FvMailBoxDataType::DestroyInstance();
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		return FvMailBoxDataType::pInstance();
	}
};

static FvMailBoxMetaDataType s_MailBox_metaDataType("MAILBOX", FV_DATATYPEID_MAILBOX);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvBlobDataType::FvBlobDataType(FvMetaDataType* pMetaDataType)
:FvDataType(pMetaDataType)
,m_spDefaultValue(NULL)
,m_spDefaultSection(NULL)
{
}

void FvBlobDataType::AddToMD5(FvMD5& md5) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
}

void FvBlobDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	if(!m_spDefaultValue)
	{
		m_spDefaultValue = new FvObjBlobOld();

		m_spDefaultSection = new FvXMLSection( "Default" );
		AddToSection( m_spDefaultValue, m_spDefaultSection );
	}
}

void FvBlobDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjBlobOld, pBlob);
	stream.AppendString(static_cast<const char*>(pBlob->Data()), pBlob->Size());
}

FvObjPtr FvBlobDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	int	iLen = stream.ReadPackedInt();
	FvObjBlobOld* pBlob = new FvObjBlobOld(iLen < FvObjBlobOld::DEFSIZE ? FvObjBlobOld::DEFSIZE : iLen);
	pBlob->AddBlob(stream.Retrieve(iLen), iLen);
	return pBlob;
}

void FvBlobDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjBlobOld, pBlob);
	FvString kData(static_cast<const char*>(pBlob->Data()), pBlob->Size());	//! TODO: 有点恶心,要拷贝一份
	pSection->SetBlob(kData);
}

FvObjPtr FvBlobDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	FvString kData = pSection->AsBlob();//! TODO: 有点恶心,要拷贝一份
	int iLen = (int)kData.length();
	FvObjBlobOld* pBlob = new FvObjBlobOld(iLen < FvObjBlobOld::DEFSIZE ? FvObjBlobOld::DEFSIZE : iLen);
	pBlob->AddBlob(kData.data(), iLen);
	return pBlob;
}

FvBlobDataType* FvBlobDataType::ms_pkInstance = NULL;

void FvBlobDataType::CreateInstance(FvMetaDataType* pMetaDataType)
{
	FV_ASSERT(!ms_pkInstance);
	ms_pkInstance = new FvBlobDataType(pMetaDataType);
	ms_pkInstance->IncRef();
}

void FvBlobDataType::DestroyInstance()
{
	FV_ASSERT(ms_pkInstance);
	delete ms_pkInstance;
	ms_pkInstance = NULL;
}

class FvBlobMetaDataType : public FvMetaDataType
{
public:
	FvBlobMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvBlobDataType::CreateInstance(this);
		FvMetaDataType::AddMetaType(this);
	}

	virtual ~FvBlobMetaDataType()
	{
		FvMetaDataType::DelMetaType(this);
		FvBlobDataType::DestroyInstance();
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		return FvBlobDataType::pInstance();
	}
};

static FvBlobMetaDataType s_Blob_metaDataType("BLOB", FV_DATATYPEID_BLOB);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FvBoolDataType::AddToMD5( FvMD5 & md5 ) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
}
bool FvBoolDataType::operator<( const FvDataType & other ) const
{
	if (this->FvDataType::operator<( other )) return true;
	if (other.FvDataType::operator<( *this )) return false;

	const FvBoolDataType& otherStr = static_cast<const FvBoolDataType&>(other);
	return m_spDefaultValue < otherStr.m_spDefaultValue;
}

void FvBoolDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	m_spDefaultValue = pSection ? this->CreateFromSection( pSection ) : new FvObjBoolOld(this, false, 0);
}
FvObjPtr FvBoolDataType::pDefaultValue() const
{
	return m_spDefaultValue;
}

void FvBoolDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	bool b = FVOBJ_GETVAL(pValue, FvObjBoolOld);
	stream << b;
}
FvObjPtr FvBoolDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	bool value;
	stream >> value;

	FvObj* pObj = new FvObjBoolOld(const_cast<FvBoolDataType*>(this), value, 0);
	return pObj;
}
void FvBoolDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	bool b = FVOBJ_GETVAL(pValue, FvObjBoolOld);
	pSection->SetBool(b);
}
FvObjPtr FvBoolDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	bool value;
	value = pSection->AsBool();
	return new FvObjBoolOld(const_cast<FvBoolDataType*>(this), value, 0);
}

class FvBoolMetaDataType : public FvMetaDataType
{
public:
	FvBoolMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvMetaDataType::AddMetaType(this);
	}

	virtual ~FvBoolMetaDataType()
	{
		FvMetaDataType::DelMetaType(this);
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		return new FvBoolDataType(this);
	}
};

static FvBoolMetaDataType s_Bool_metaDataType("BOOL", FV_DATATYPEID_BOOL);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvString FvNumFixArrayDataType::TypeName() const
{
	FvString acc = FvDataType::TypeName();
	for (Fields::const_iterator it = m_kFields.begin(); it != m_kFields.end(); ++it)
	{
		acc += (it == m_kFields.begin()) ? " props " : ", ";
		acc += it->m_kName + ":(" + it->m_spType->TypeName() + ")";
	}
	return acc;
}
void FvNumFixArrayDataType::AddToMD5( FvMD5 & md5 ) const
{
	FvString name = FvDataType::TypeName();
	md5.Append( name.data(), name.size() );
	for (Fields::const_iterator it = m_kFields.begin(); it != m_kFields.end(); ++it)
	{
		md5.Append( it->m_kName.data(), it->m_kName.size() );
		it->m_spType->AddToMD5( md5 );
	}
}
bool FvNumFixArrayDataType::operator<( const FvDataType & other ) const
{
	if (this->FvDataType::operator<( other )) return true;
	if (other.FvDataType::operator<( *this )) return false;

	FvNumFixArrayDataType & otherStruct = (FvNumFixArrayDataType&)other;
	if(m_kFields.size() < otherStruct.m_kFields.size()) return true;
	if(otherStruct.m_kFields.size() < m_kFields.size()) return false;

	for (FvUInt32 i = 0; i < m_kFields.size(); ++i)
	{
		FvInt32 res = m_kFields[i].m_kName.compare( otherStruct.m_kFields[i].m_kName );
		if (res < 0) return true;
		if (res > 0) return false;

		if ((*m_kFields[i].m_spType) < (*otherStruct.m_kFields[i].m_spType)) return true;
		if ((*otherStruct.m_kFields[i].m_spType) < (*m_kFields[i].m_spType)) return false;
	}

	return m_spDefaultValue < otherStruct.m_spDefaultValue;
}

void FvNumFixArrayDataType::SetDefaultValue( FvXMLSectionPtr pSection )
{
	m_spDefaultValue = pSection ? CreateFromSection(pSection) : new FvObjNumFixArrayOld(this, false);
}
FvObjPtr FvNumFixArrayDataType::pDefaultValue() const
{
	return m_spDefaultValue;
}

void FvNumFixArrayDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	FVOBJ_CONVERT_P(pValue, FvObjNumFixArrayOld, pStruct);

	for(FvUInt32 i=0; i<m_kFields.size(); ++i)
	{
		m_kFields[i].m_spType->AddToStream(pStruct->m_kElementVector[i], stream, isPersistentOnly);
	}
}
FvObjPtr FvNumFixArrayDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	FvObjNumFixArrayOld* pStruct = new FvObjNumFixArrayOld(const_cast<FvNumFixArrayDataType*>(this), FvUInt32(1));
	pStruct->m_kElementVector.resize(m_kFields.size());

	for (FvUInt32 i = 0; i < m_kFields.size(); ++i)
	{
		FvObjPtr pElement = m_kFields[i].m_spType->CreateFromStream( stream, isPersistentOnly );
		if (pElement)
		{
			pElement->SetRef(i);
			pStruct->m_kElementVector[i] = pElement;
		}
		else
		{
			stream.Error( true );
			break;
		}
	}

	if (stream.Error())
	{
		return NULL;
	}

	return pStruct;
}
void FvNumFixArrayDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
/**
	FVOBJ_CONVERT_P(pValue, FvObjStructOld, pStruct);
	FvInt32 cnt = m_kFields.size();

	for(int i=0; i<cnt; ++i)
	{
		FvXMLSectionPtr itemSection = pSection->NewSection( m_kFields[i].m_kName );
		m_kFields[i].m_spType->AddToSection(pStruct->m_kElementVector[i], itemSection);
	}
**/
}
FvObjPtr FvNumFixArrayDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
/**
	if(!pSection)
	{
		FV_ERROR_MSG( "FvNumFixArrayDataType::CreateFromSection: "
			"Section is NULL.\n" );
		return NULL;
	}

	FvObjStructOld* pStruct = new FvObjStructOld(const_cast<FvNumFixArrayDataType*>(this), FvUInt32(1));
	pStruct->m_kElementVector.resize(m_kFields.size());

	for(FvUInt32 i = 0; i < m_kFields.size(); ++i)
	{
		FvXMLSectionPtr pChildSection = pSection->OpenSection( m_kFields[i].m_kName );
		if (pChildSection)
		{
			FvObjPtr pValue = m_kFields[i].m_spType->CreateFromSection( pChildSection );
			if (!pValue)
				return NULL;	// error already printed

			pValue->SetRef(i);
			pStruct->m_kElementVector[i] = pValue;
		}
		else
		{
			pStruct->m_kElementVector[i] = m_kFields[i].m_spType->pDefaultValue()->Copy();
		}
	}

	return pStruct;
**/
	return NULL;
}

class FvNumFixArrayMetaDataType : public FvMetaDataType
{
public:
	FvNumFixArrayMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvMetaDataType::AddMetaType( this );
	}

	virtual ~FvNumFixArrayMetaDataType()
	{
		FvMetaDataType::DelMetaType( this );
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		FvDataTypePtr pOfType =	FvDataType::BuildDataType( pSection->OpenSection( "of" ) );
		if(!pOfType)
			return NULL;

		FvNumFixArrayDataType::Fields fields;
/**
		FvXMLSectionPtr pParentSect = pSection->OpenSection( "Parent" );
		if (pParentSect)
		{
			FvDataTypePtr pParent = FvDataType::BuildDataType( pParentSect );
			if (!pParent || pParent->pMetaDataType() != this)
			{
				FV_ERROR_MSG( "<Parent> of %s must also be a %s\n", Name(), Name() );
				return NULL;
			}

			FvNumFixArrayDataType* pParentStruct = static_cast<FvNumFixArrayDataType*>(&*pParent);
			fields = pParentStruct->GetFields();
		}
**/
		FvXMLSectionPtr pProps = pSection->OpenSection( "Properties" );
		if((!pProps || pProps->CountChildren() == 0) && fields.empty())
		{
			FV_ERROR_MSG( "<Properties> section missing or empty in %s type spec\n",
				Name() );
			return NULL;
		}

		for(FvXMLSectionIterator it = pProps->Begin(); it != pProps->End(); ++it)
		{
			if((*it)->GetType() != TiXmlNode::ELEMENT)
				continue;

			FvNumFixArrayDataType::Field f;
			f.m_kName = (*it)->SectionName();
/**
			FvNumFixArrayDataType::Fields::iterator itr = fields.begin();
			while(itr != fields.end())
			{
				if(itr->m_kName.compare(f.m_kName) == 0)
				{
					FV_ERROR_MSG( "Find same property '%s' in %s type\n",
						f.m_kName.c_str(), Name() );
					return NULL;
				}
				++itr;
			}

			//f.m_spType = FvDataType::BuildDataType( (*it)->OpenSection( "Type" ) );
			f.m_spType = FvDataType::BuildDataType( (*it) );//! 去除Type标签
			if (!f.m_spType)
			{
				FV_ERROR_MSG( "Could not build type for property '%s' in %s type\n",
					f.m_kName.c_str(), Name() );
				return NULL;
			}
			f.m_iDBLen = (*it)->ReadInt( "DatabaseLength", FV_DEFAULT_DATABASE_LENGTH );
			f.m_bIsPersistent = (*it)->ReadBool( "Persistent", true );
**/
			f.m_spType = pOfType;
			f.m_iDBLen = FV_DEFAULT_DATABASE_LENGTH;
			f.m_bIsPersistent = true;
			fields.push_back( f );
		}

		FV_ASSERT_DEV( !fields.empty() );

		return new FvNumFixArrayDataType( this, fields );
	}
};

static FvNumFixArrayMetaDataType s_NUM_FIXARRAY_metaDataType("NUMFIXARRAY", FV_DATATYPEID_NUM_FIXARRAY);
