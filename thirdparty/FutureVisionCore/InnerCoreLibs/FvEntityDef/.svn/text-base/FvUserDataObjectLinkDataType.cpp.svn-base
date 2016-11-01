#include "FvUserDataObjectLinkDataType.h"
#include "FvUserDataObject.h"
#include <FvUniqueID.h>
#include <FvMD5.h>


FvUserDataObjectLinkDataType::FvUserDataObjectLinkDataType(FvMetaDataType* pkMeta)
:FvDataType(pkMeta)
{

}

void FvUserDataObjectLinkDataType::AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	if(pValue == NULL)
	{
		stream << FvUniqueID::Zero();
		return;
	}
	else if(!const_cast<FvUserDataObjectLinkDataType*>(this)->IsSameType(pValue))
	{
		FV_ERROR_MSG("%s: type is not a UserDataObject\n", __FUNCTION__);
		stream << FvUniqueID::Zero();
		return;
	}

	FvUserDataObject* pkUDO = FVOBJ_CONVERT(pValue, FvUserDataObject);
	stream << pkUDO->Guid();
}

FvObjPtr FvUserDataObjectLinkDataType::CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const
{
	FvUniqueID kGuid;
	stream >> kGuid;

	if(stream.Error())
	{
		FV_ERROR_MSG( "%s: Not enough data on stream to read value\n", __FUNCTION__ );
		return NULL;
	}

	if(kGuid == FvUniqueID::Zero())
	{
		return ms_spDefaultValue;
	}

	return FvUserDataObject::CreateRef(kGuid);
}

void FvUserDataObjectLinkDataType::AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const
{
	FV_ASSERT(pValue &&
		const_cast<FvUserDataObjectLinkDataType*>(this)->IsSameType(pValue));

	FvUserDataObject* pkUDO = FVOBJ_CONVERT(pValue, FvUserDataObject);
	pSection->WriteString( "guid", pkUDO->Guid() );
}

FvObjPtr FvUserDataObjectLinkDataType::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	FvString kGuid = pSection->ReadString( "guid", "" );

	return FvUserDataObject::CreateRef(kGuid);
}


void FvUserDataObjectLinkDataType::AddToMD5(FvMD5& md5) const
{
	const char* md5String = "UserDataObjectLinkDataType";
	md5.Append( md5String, sizeof(md5String) );
}


FvUserDataObjectLinkDataType* FvUserDataObjectLinkDataType::ms_pkInstance = NULL;
FvObjPtr FvUserDataObjectLinkDataType::ms_spDefaultValue = NULL;

void FvUserDataObjectLinkDataType::CreateInstance(FvMetaDataType* pMetaDataType)
{
	FV_ASSERT(!ms_pkInstance);
	ms_pkInstance = new FvUserDataObjectLinkDataType(pMetaDataType);
	ms_pkInstance->IncRef();
	ms_spDefaultValue = new FvUserDataObject(NULL);
}

void FvUserDataObjectLinkDataType::DestroyInstance()
{
	FV_ASSERT(ms_pkInstance);
	delete ms_pkInstance;
	ms_pkInstance = NULL;
	ms_spDefaultValue = NULL;
}

class FvUserDataObjectLinkMetaDataType : public FvMetaDataType
{
public:
	FvUserDataObjectLinkMetaDataType(const char* name, const FvUInt32 id):FvMetaDataType(name,id)
	{
		FvUserDataObjectLinkDataType::CreateInstance(this);
		FvMetaDataType::AddMetaType(this);
	}

	virtual ~FvUserDataObjectLinkMetaDataType()
	{
		FvMetaDataType::DelMetaType(this);
		FvUserDataObjectLinkDataType::DestroyInstance();
	}

	virtual FvDataTypePtr GetType( FvXMLSectionPtr pSection )
	{
		return FvUserDataObjectLinkDataType::pInstance();
	}
};

//static FvUserDataObjectLinkMetaDataType s_UserDataObject_metaDataType("UDO_REF", FV_DATATYPEID_USERDATAOBJ);
static FvUserDataObjectLinkMetaDataType s_UserDataObject_metaDataType("UDO", FV_DATATYPEID_USERDATAOBJ);